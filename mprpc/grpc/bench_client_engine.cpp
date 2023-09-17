#include "bench_client_engine.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <grpc++/grpc++.h>
#include <grpc/support/log.h>
#include <thread>

#include <atomic>
#include <fstream>
#include <sstream>

#include "bench.grpc.pb.h"

using bench::BenchMessage;
using bench::StreamingTokenizer;
using bench::Tokenizer;
using grpc::Channel;
using grpc::ClientAsyncReader;
using grpc::ClientAsyncReaderWriter;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;

using AtomicSizeT = std::atomic<size_t>;

using namespace std;

namespace {

using StringVectorT = std::vector<std::string>;
class TokenizerClient;

// struct for keeping state and data information
struct AsyncClientCall {
    TokenizerClient& rclient_;
    // Container for the data we expect from the server.
    BenchMessage reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // Storage for the status of the RPC upon completion.
    Status status;

    std::unique_ptr<ClientAsyncResponseReader<BenchMessage>> response_reader;

    std::unique_ptr<ClientAsyncReaderWriter<BenchMessage, BenchMessage>> stream_rw_;

    enum {
        STREAM_DONE_CONNECT,
        STREAM_DONE_READ,
        STREAM_DONE_WRITE,
        STREAM_DONE
    } stage_
        = STREAM_DONE_CONNECT;

    AsyncClientCall(TokenizerClient& _rclient)
        : rclient_(_rclient)
    {
    }

    AsyncClientCall(AsyncClientCall&& _other)
        : rclient_(_other.rclient_)
        , response_reader(std::move(_other.response_reader))
        , stream_rw_(std::move(_other.stream_rw_))
    {
    }
};

using TokenizerClientPtrT    = std::unique_ptr<TokenizerClient>;
using TokenizerClientVectorT = std::vector<TokenizerClientPtrT>;

struct Context {
    CompletionQueue        cq_;
    StringVectorT          line_vec_;
    size_t                 call_count;
    bool                   print_response;
    AtomicSizeT            messages_transferred{0};
    AtomicSizeT            tokens_transferred{0};
    AtomicSizeT            connection_count_;
    TokenizerClientVectorT client_vec_;

    int start(const bool _secure, const bool _compress,
        const std::string& _connect_addr, const std::string& _defaul_port,
        const size_t _connection_count, const size_t _loop_count,
        const std::string& _text_file_path, const bool _print_response, const bool _streaming);

    void run();
};

class TokenizerClient {
    std::unique_ptr<Tokenizer::Stub>          stub_;
    std::unique_ptr<StreamingTokenizer::Stub> streaming_stub_;
    Context&                                  rctx_;
    size_t                                    line_index_;
    size_t                                    loop_count_;

public:
    TokenizerClient(std::shared_ptr<Channel> _channel,
        Context& _rctx, const size_t _line_index, const size_t _loop_count, const bool _is_streaming)
        : rctx_(_rctx)
        , line_index_(_line_index)
        , loop_count_(_loop_count)
    {

        if (_is_streaming) {
            streaming_stub_ = StreamingTokenizer::NewStub(_channel);
        } else {
            stub_ = Tokenizer::NewStub(_channel);
        }
    }

    void start()
    {
        BenchMessage req;
        req.set_text(rctx_.line_vec_[line_index_ % rctx_.line_vec_.size()]);
        ++line_index_;

        if (stub_) {

            AsyncClientCall* call = new AsyncClientCall(*this);

            call->response_reader = stub_->PrepareAsyncTokenize(&call->context, req, &rctx_.cq_);
            call->response_reader->StartCall();

            call->response_reader->Finish(&call->reply, &call->status, (void*)call);
        } else {
            AsyncClientCall* call = new AsyncClientCall(*this);

            call->stream_rw_ = streaming_stub_->PrepareAsyncTokenize(&call->context, &rctx_.cq_);
            call->stream_rw_->StartCall(call);
        }
    }

    bool step(AsyncClientCall* _pcall, AtomicSizeT& _rmsgs, AtomicSizeT& _rtokens)
    {
        if (stub_) {
            if (loop_count_) {
                ++_rmsgs;
                _rtokens += _pcall->reply.tokens_size();
                AsyncClientCall* call = new AsyncClientCall(std::move(*_pcall));
                delete _pcall;
                BenchMessage req;
                req.set_text(rctx_.line_vec_[line_index_ % rctx_.line_vec_.size()]);
                ++line_index_;
                --loop_count_;
                call->response_reader = stub_->PrepareAsyncTokenize(&call->context, req, &rctx_.cq_);

                call->response_reader->StartCall();

                call->response_reader->Finish(&call->reply, &call->status, (void*)call);
                return true;
            }
            delete _pcall;
            return false;
        } else {
            if (_pcall->stage_ == AsyncClientCall::STREAM_DONE_READ) {
                ++_rmsgs;
                _rtokens += _pcall->reply.tokens_size();
                _pcall->reply.clear_tokens();
                --loop_count_;
                if (loop_count_ != 0) {
                    BenchMessage req;
                    req.set_text(rctx_.line_vec_[line_index_ % rctx_.line_vec_.size()]);
                    ++line_index_;

                    _pcall->stage_ = AsyncClientCall::STREAM_DONE_WRITE;
                    _pcall->stream_rw_->Write(req, _pcall);
                } else {
                    // Write the sentinel to let server know we're done
                    // BenchMessage req;
                    //_pcall->stage_ = AsyncClientCall::STREAM_DONE;
                    //_pcall->stream_rw_->Write(req, _pcall);
                    _pcall->stage_ = AsyncClientCall::STREAM_DONE;
                    _pcall->stream_rw_->Finish(&_pcall->status, (void*)_pcall);
                    delete _pcall;
                    return false;
                }
                return true;
            } else if (_pcall->stage_ == AsyncClientCall::STREAM_DONE_WRITE) {
                _pcall->stage_ = AsyncClientCall::STREAM_DONE_READ;
                _pcall->stream_rw_->Read(&_pcall->reply, _pcall);
                return true;
            } else if (_pcall->stage_ == AsyncClientCall::STREAM_DONE) {
                delete _pcall;
                return false;
            } else {
                GPR_ASSERT(_pcall->stage_ == AsyncClientCall::STREAM_DONE_CONNECT);
                BenchMessage req;
                req.set_text(rctx_.line_vec_[line_index_ % rctx_.line_vec_.size()]);
                ++line_index_;
                _pcall->stage_ = AsyncClientCall::STREAM_DONE_WRITE;
                _pcall->stream_rw_->Write(req, _pcall);
                return true;
            }
        }
    }
};

int Context::start(const bool _secure, const bool _compress,
    const std::string& _connect_addr, const std::string& _defaul_port,
    const size_t _connection_count, const size_t _loop_count,
    const std::string& _text_file_path, const bool _print_response, const bool _streaming)
{
    {
        ifstream ifs(_text_file_path);
        if (ifs) {
            while (!ifs.eof()) {
                line_vec_.emplace_back();
                getline(ifs, line_vec_.back());
                if (line_vec_.back().empty()) {
                    line_vec_.pop_back();
                }
            }
        } else {
            return -1;
        }
    }
    print_response    = _print_response;
    connection_count_ = _connection_count;
    grpc::ChannelArguments args;
    args.SetInt(GRPC_ARG_USE_LOCAL_SUBCHANNEL_POOL, 1);

    for (size_t i = 0; i < _connection_count; ++i) {
        client_vec_.emplace_back(
            make_unique<TokenizerClient>(
                grpc::CreateCustomChannel(_connect_addr, grpc::InsecureChannelCredentials(), args), *this, i, _loop_count, _streaming));
        client_vec_.back()->start();
    }
    return 0;
}

void Context::run()
{
    void* got_tag;
    bool  ok = false;

    while (cq_.Next(&got_tag, &ok)) {
        AsyncClientCall* call = static_cast<AsyncClientCall*>(got_tag);
        GPR_ASSERT(ok);
        // GPR_ASSERT(call->reply.tokens_size() != 0);

        if (print_response) {
            cout << "Response: ";
            for (size_t i = 0; i < call->reply.tokens_size(); ++i) {
                cout << '[' << call->reply.tokens(i) << ']' << ' ';
            }
            cout << endl;
        }

        //++messages_transferred;
        // tokens_transferred += call->reply.tokens_size();

        if (call->rclient_.step(call, messages_transferred, tokens_transferred)) {
        } else {
            if (connection_count_.fetch_sub(1) == 1) {
                cout << "Client Done: msgcnt = " << messages_transferred
                     << " tokencnt = " << tokens_transferred << endl;
                cq_.Shutdown();
                exit(0);
                return;
            }
        }
    }
}

std::unique_ptr<Context> ctx_ptr;

} // namespace

namespace bench_client {
int start(const bool _secure, const bool _compress,
    const std::string& _connect_addr, const std::string& _defaul_port,
    const size_t _connection_count, const size_t _loop_count,
    const std::string& _text_file_path, const bool _print_response, const bool _streaming)
{
    ctx_ptr = std::make_unique<Context>();
    return ctx_ptr->start(_secure, _compress, _connect_addr, _defaul_port,
        _connection_count, _loop_count, _text_file_path,
        _print_response, _streaming);
}

void wait()
{
    const size_t        thread_count = 3;
    std::vector<thread> threads;

    for (size_t i = 0; i < thread_count; ++i) {
        threads.emplace_back(thread{[]() { ctx_ptr->run(); }});
    }

    ctx_ptr->run();

    for (auto& thr : threads) {
        thr.join();
    }
}

void stop(const bool _wait) {}
} // namespace bench_client
