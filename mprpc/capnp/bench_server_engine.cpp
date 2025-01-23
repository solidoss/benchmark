#include "bench_server_engine.hpp"
#include "bench.capnp.h"
#include <capnp/ez-rpc.h>
#include <capnp/message.h>
#include <future>
#include <iostream>
#include <kj/debug.h>
#include <sstream>
#include <thread>
#include <vector>
using namespace std;

namespace {

struct TokenizerImpl : bench::Tokenizer::Server {
    ::kj::Promise<void> tokenize(TokenizeContext context) override
    {
        auto           rcv_msg = context.getParams().getValue();
        auto           snd_msg = context.getResults().getValue();
        istringstream  iss{rcv_msg.getText().cStr()};
        vector<string> tokens;
        while (!iss.eof()) {
            tokens.emplace_back();
            iss >> tokens.back();
        }
        auto lst_bld = snd_msg.initTokens(tokens.size());
        for (size_t i = 0; i < tokens.size(); ++i) {
            lst_bld.set(i, tokens[i].c_str());
        }
        return kj::READY_NOW;
    }
};

thread server_thread;
} // namespace

namespace bench_server {
int start(const bool _secure, const bool _compress,
    const std::string& _listen_addr, const bool _streaming)
{
    promise<int> port_promise;
    server_thread = thread([&]() {
        capnp::EzRpcServer server(kj::heap<TokenizerImpl>(), _listen_addr.c_str());
        auto&              waitScope = server.getWaitScope();
        auto               port      = server.getPort().wait(waitScope);
        if (port == 0) {
            // The address format "unix:/path/to/socket" opens a unix domain socket,
            // in which case the port will be zero.
            std::cout << "Listening on Unix socket..." << std::endl;
        } else {
            std::cout << "Listening on port " << port << "..." << std::endl;
        }
        port_promise.set_value(port);
        kj::NEVER_DONE.wait(waitScope);
    });

    auto fut = port_promise.get_future();

    fut.wait();
    server_thread.detach();
    return fut.get();
}

void stop(const bool _wait)
{
    if (_wait) {
        _exit(0);
    }
}

} // namespace bench_server