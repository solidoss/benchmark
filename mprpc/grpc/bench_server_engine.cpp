#include "bench_server_engine.hpp"
#include <grpc++/grpc++.h>
#include <grpc/support/log.h>
#include <sstream>
#include <thread>
// #include <unistd.h>

#include "bench.grpc.pb.h"

using bench::BenchMessage;
using bench::StreamingTokenizer;
using bench::Tokenizer;
using grpc::Server;
using grpc::ServerAsyncReaderWriter;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;

using namespace std;

extern mutex gmtx;

namespace {

class ServerImpl final {
    std::unique_ptr<ServerCompletionQueue> cq_;
    Tokenizer::AsyncService                service_;
    StreamingTokenizer::AsyncService       streaming_service_;
    std::unique_ptr<Server>                server_;

public:
    ~ServerImpl()
    {
        server_->Shutdown();
        // Always shutdown the completion queue after the server.
        cq_->Shutdown();
    }

    // There is no shutdown handling in this code.
    void Run(const bool _secure, const bool _compress,
        const std::string& _listen_addr, const bool _streaming)
    {
        std::string server_address(_listen_addr);

        ServerBuilder builder;

        // Listen on the given address without any authentication mechanism.
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        // Register "service_" as the instance through which we'll communicate with
        // clients. In this case it corresponds to an *asynchronous* service.
        builder.RegisterService(&service_);
        builder.RegisterService(&streaming_service_);
        // Get hold of the completion queue used for the asynchronous communication
        // with the gRPC runtime.
        cq_ = builder.AddCompletionQueue();
        // Finally assemble the server.
        server_ = builder.BuildAndStart();
        std::cout << "Server listening on " << server_address << std::endl;

        // Proceed to the server's main loop.
        const size_t        thread_count = 3;
        std::vector<thread> threads;

        if (_streaming) {
            new CallData(&streaming_service_, cq_.get());
        } else {
            new CallData(&service_, cq_.get());
        }

        for (size_t i = 0; i < thread_count; ++i) {
            threads.emplace_back(thread{[this]() { HandleRpcs(); }});
        }

        HandleRpcs();

        for (auto& thr : threads) {
            thr.join();
        }
    }

private:
    // Class encompasing the state and logic needed to serve a request.
    class CallData {
        // The means of communication with the gRPC runtime for an asynchronous
        // server.
        Tokenizer::AsyncService*          service_           = nullptr;
        StreamingTokenizer::AsyncService* streaming_service_ = nullptr;
        // The producer-consumer queue where for asynchronous server notifications.
        ServerCompletionQueue* cq_;
        // Context for the rpc, allowing to tweak aspects of it such as the use
        // of compression, authentication, as well as to send metadata back to the
        // client.
        ServerContext ctx_;
        // What we get from the client.
        BenchMessage req_;
        // What we get from the client.
        BenchMessage res_;

        // The means to get back to the client.
        ServerAsyncResponseWriter<BenchMessage> responder_;

        ServerAsyncReaderWriter<BenchMessage, BenchMessage> streaming_rw_;

        // Let's implement a tiny state machine with the following states.
        enum CallStatus { CREATE,
            PROCESS,
            STREAM_CONNECT,
            STREAM_READ,
            STREAM_WRITE,
            FINISH };
        CallStatus status_; // The current serving state.

        CallData(Tokenizer::AsyncService* service, StreamingTokenizer::AsyncService* streaming_service, ServerCompletionQueue* cq)
            : service_(service)
            , streaming_service_(streaming_service)
            , cq_(cq)
            , responder_(&ctx_)
            , streaming_rw_(&ctx_)
            , status_(CREATE)
        {
            // Invoke the serving logic right away.
            Proceed();
        }

    public:
        // Take in the "service" instance (in this case representing an asynchronous
        // server) and the completion queue "cq" used for asynchronous communication
        // with the gRPC runtime.
        CallData(Tokenizer::AsyncService* service, ServerCompletionQueue* cq)
            : service_(service)
            , cq_(cq)
            , responder_(&ctx_)
            , streaming_rw_(&ctx_)
            , status_(CREATE)
        {
            // Invoke the serving logic right away.
            Proceed();
        }

        CallData(StreamingTokenizer::AsyncService* service, ServerCompletionQueue* cq)
            : streaming_service_(service)
            , cq_(cq)
            , responder_(&ctx_)
            , streaming_rw_(&ctx_)
            , status_(CREATE)
        {
            // Invoke the serving logic right away.
            Proceed();
        }

        void Proceed()
        {
            if (status_ == CREATE) {

                if (service_) {
                    status_ = PROCESS;
                    service_->RequestTokenize(&ctx_, &req_, &responder_, cq_, cq_, this);
                } else {
                    status_ = STREAM_CONNECT;
                    streaming_service_->RequestTokenize(&ctx_, &streaming_rw_, cq_, cq_, this);
                    ctx_.AsyncNotifyWhenDone(this);
                }
            } else if (status_ == PROCESS) {
                new CallData(service_, streaming_service_, cq_);
                // res_.set_id(req_.id());
                res_.clear_tokens();
                {
                    istringstream iss{req_.text()};

                    while (!iss.eof()) {
                        string tmp;
                        iss >> tmp;
                        res_.add_tokens(std::move(tmp));
                    }
                    req_.clear_text();
                }
                // And we are done! Let the gRPC runtime know we've finished, using the
                // memory address of this instance as the uniquely identifying tag for
                // the event.
                status_ = FINISH;
                responder_.Finish(res_, Status::OK, this);
            } else if (status_ == STREAM_READ) {
                res_.clear_tokens();
                // res_.set_id(req_.id());
                if (false) {
                    lock_guard<mutex> lock(gmtx);
                    // cout<<this <<" Processing stream: "<<req_.id()<<" "<<req_.text().size()<<endl;
                }
                bool is_last = false;
                {
                    if (!req_.text().empty()) {
                        istringstream iss{req_.text()};

                        while (!iss.eof()) {
                            string tmp;
                            iss >> tmp;
                            res_.add_tokens(std::move(tmp));
                        }
                    } else {
                        is_last = true;
                    }
                }
                if (is_last) {
                    // streaming_responder_.WriteAndFinish(res_, grpc::WriteOptions(), Status::OK, this);
                    status_ = FINISH;
                    streaming_rw_.Finish(Status::OK, this);
                } else {
                    status_ = STREAM_WRITE;
                    streaming_rw_.Write(res_, this);
                }
            } else if (status_ == STREAM_WRITE) {
                status_ = STREAM_READ;
                req_.clear_text();
                streaming_rw_.Read(&req_, this);
            } else if (status_ == STREAM_CONNECT) {
                new CallData(service_, streaming_service_, cq_);

                status_ = STREAM_READ;
                req_.clear_text();
                streaming_rw_.Read(&req_, this);
            } else {
                assert(status_ == FINISH);
                // Once in the FINISH state, deallocate ourselves (CallData).
                delete this;
            }
        }
    }; // CallData

    // This can be run in multiple threads if needed.
    void HandleRpcs()
    {
        // Spawn a new CallData instance to serve new clients.
        void* tag; // uniquely identifies a request.
        bool  ok;
        while (true) {
            // Block waiting to read the next event from the completion queue. The
            // event is uniquely identified by its tag, which in this case is the
            // memory address of a CallData instance.
            // The return value of Next should always be checked. This return value
            // tells us whether there is any kind of event or cq_ is shutting down.
            cq_->Next(&tag, &ok);
            assert(ok);
            static_cast<CallData*>(tag)->Proceed();
        }
    }
};

} // namespace

namespace bench_server {
int start(const bool _secure, const bool _compress,
    const std::string& _listen_addr, const bool _streaming)
{
    thread thr{[=]() {
        ServerImpl server;
        server.Run(_secure, _compress, _listen_addr, _streaming);
    }};
    thr.detach();
    return 1;
}

void stop(const bool _wait)
{
    if (_wait) {
        exit(0);
    }
}

} // namespace bench_server
