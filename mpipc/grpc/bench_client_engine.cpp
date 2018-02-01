#include "bench_client_engine.hpp"

#include <string>
#include <iostream>
#include <memory>
#include <vector>

#include <grpc++/grpc++.h>
#include <grpc/support/log.h>
#include <thread>

#include <fstream>
#include <sstream>
#include <atomic>

#include "bench.grpc.pb.h"

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;
using bench::BenchMessage;
using bench::Tokenizer;

using AtomicSizeT = std::atomic<size_t>;

using namespace std;

namespace{

using StringVectorT = std::vector<std::string>;

class TokenizerClient {
public:
    explicit TokenizerClient():messages_transferred(0), tokens_transferred(0) {}
    
    int start(
        const bool _secure, 
        const bool _compress,
        const std::string &_connect_addr,
        const std::string &_defaul_port,
        const size_t _connection_count,
        const size_t _loop_count,
        const std::string &_text_file_path,
        const bool _print_response
    ){
        {
            ifstream ifs(_text_file_path);
            if(ifs){
                while(!ifs.eof()){
                    line_vec_.emplace_back();
                    getline(ifs, line_vec_.back());
                    if(line_vec_.back().empty()){
                        line_vec_.pop_back();
                    }
                }
            }else{
                return -1;
            }
        }
        
        //create the channels
        call_count = _connection_count;
        for(size_t i = 0; i < _connection_count; ++i){
            AsyncClientCall* call = new AsyncClientCall;
            call->stub = Tokenizer::NewStub(grpc::CreateChannel(_connect_addr, grpc::InsecureChannelCredentials()));
            call->loop_count = _loop_count;
            call->index = i;
            call->msg.set_text(line_vec_[i % line_vec_.size()]);
            call->context = new ClientContext;
            call->response_reader = call->stub->PrepareAsyncTokenize(call->context, call->msg, &cq_);
            call->response_reader->StartCall();
            call->response_reader->Finish(&call->msg, &call->status, (void*)call);
        }
        
        print_response = _print_response;
        return 0;
    }
    
    void run(){
        void* got_tag;
        bool ok = false;
        
        while (cq_.Next(&got_tag, &ok)) {
            AsyncClientCall* call = static_cast<AsyncClientCall*>(got_tag);
            GPR_ASSERT(ok);
            GPR_ASSERT(call->msg.tokens_size() != 0);
            
            if(print_response){
                cout<<call->index<<' ';
                for(size_t i = 0; i < call->msg.tokens_size(); ++i){
                    cout<<'['<<call->msg.tokens(i)<<']'<<' ';
                }
                cout<<endl;
            }
            
            ++messages_transferred;
            tokens_transferred += call->msg.tokens_size();
            
            --call->loop_count;
            
            if(call->loop_count != 0)
            {
                ++call->index;
                
                call->msg.clear_tokens();
                call->msg.set_text(line_vec_[call->index % line_vec_.size()]);
                delete call->context;
                call->context = new ClientContext;
                call->response_reader = call->stub->PrepareAsyncTokenize(call->context, call->msg, &cq_);
                call->response_reader->StartCall();
                call->response_reader->Finish(&call->msg, &call->status, (void*)call);
            }else{
                delete call->context;
                delete call;
                --call_count;
                if(call_count == 0){
                    cout<<"Done: msgcnt = "<<messages_transferred<<" tokencnt = "<<tokens_transferred<<endl;
                    return;
                }
            }
        }
    }

  private:

    // struct for keeping state and data information
    struct AsyncClientCall {
        // Container for the data we expect from the server.
        BenchMessage msg;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext *context;

        // Storage for the status of the RPC upon completion.
        Status status;

        std::unique_ptr<Tokenizer::Stub>    stub;
        
        std::unique_ptr<ClientAsyncResponseReader<BenchMessage>> response_reader;
        size_t  loop_count;
        size_t  index;
    };
    

    // Out of the passed in Channel comes the stub, stored here, our view of the
    // server's exposed services.
    //std::vector<> stub_vec_;

    // The producer-consumer queue we use to communicate asynchronously with the
    // gRPC runtime.
    CompletionQueue cq_;
    StringVectorT   line_vec_;
    size_t          call_count;
    bool            print_response;
    AtomicSizeT     messages_transferred;
    AtomicSizeT     tokens_transferred;
};

std::unique_ptr<TokenizerClient>    client_ptr;

}//namespace

namespace bench_client{
int start(
    const bool _secure, 
    const bool _compress,
    const std::string &_connect_addr,
    const std::string &_defaul_port,
    const size_t _connection_count,
    const size_t _loop_count,
    const std::string &_text_file_path,
    const bool _print_response
){
    client_ptr.reset(new TokenizerClient);
    return client_ptr->start(_secure, _compress, _connect_addr, _defaul_port, _connection_count, _loop_count, _text_file_path, _print_response);
}

void wait(){
    client_ptr->run();
}

void stop(const bool _wait){
    
}
}//namespace bench
