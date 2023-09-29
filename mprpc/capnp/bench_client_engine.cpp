#include "bench_client_engine.hpp"
#include "bench.capnp.h"
#include <atomic>
#include <capnp/ez-rpc.h>
#include <capnp/message.h>
#include <fstream>
#include <iostream>
#include <kj/debug.h>
#include <thread>
#include <vector>

using namespace std;

namespace {
vector<string> line_vec;
vector<thread> thr_vec;
atomic_size_t  messages_transferred{0};
atomic_size_t  tokens_transferred{0};
} // namespace

namespace bench_client {
int start(const bool _secure, const bool _compress,
    const std::string& _connect_addr, const std::string& _defaul_port,
    const size_t _connection_count, const size_t _loop_count,
    const std::string& _text_file_path, const bool _print_response, const bool _streaming)
{

    {
        ifstream ifs(_text_file_path);
        if (ifs) {
            while (!ifs.eof()) {
                line_vec.emplace_back();
                getline(ifs, line_vec.back());
                if (line_vec.back().empty()) {
                    line_vec.pop_back();
                }
            }
        } else {
            return -1;
        }
    }
    for (size_t i = 0; i < _connection_count; ++i) {
        thr_vec.emplace_back(
            [_connect_addr, i, _loop_count, _print_response]() {
                capnp::EzRpcClient client(_connect_addr.c_str());
                auto&              wait_scope       = client.getWaitScope();
                auto               tokenizer_client = client.getMain<bench::Tokenizer>();

                for (size_t j = 0; j < _loop_count; ++j) {
                    auto request = tokenizer_client.tokenizeRequest();
                    request.getValue().setText(line_vec[(i + j) % line_vec.size()].c_str());
                    auto       response = request.send().wait(wait_scope);
                    const auto tokens   = response.getValue().getTokens();
                    ++messages_transferred;
                    tokens_transferred += tokens.size();
                    if (_print_response) {
                        cout << "Response: ";

                        for (const auto& token : tokens) {
                            cout << '[' << token.cStr() << ']' << ' ';
                        }
                        cout << endl;
                    }
                }
            });
    }

    return 0;
}

void wait()
{
    for (auto& thr : thr_vec) {
        thr.join();
    }
    cout << "Client Done: msgcnt = " << messages_transferred
         << " tokencnt = " << tokens_transferred << endl;
}

void stop(const bool _wait) {}
} // namespace bench_client
