#include <iostream>
#include <string>
#include "boost/program_options.hpp"

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

//-----------------------------------------------------------------------------
//      Parameters
//-----------------------------------------------------------------------------
struct Parameters{
    Parameters():default_port("5555"){}

    const string            default_port;
    bool                    secure;
    bool                    compress;
    bool                    print_response;
    string                  connect_host;
    size_t                  loop_count;
    size_t                  connection_count;
    string                  text_file_path;
};

//-----------------------------------------------------------------------------

bool parseArguments(Parameters &_par, int argc, char *argv[]);


using StringVectorT = std::vector<std::string>;

class TokenizerClient {
public:
    explicit TokenizerClient():messages_transferred(0), tokens_transferred(0) {}
    
    int start(const Parameters &_rp){
        {
            ifstream ifs(_rp.text_file_path);
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
        call_count = _rp.connection_count;
        for(size_t i = 0; i < _rp.connection_count; ++i){
            AsyncClientCall* call = new AsyncClientCall;
            call->stub = Tokenizer::NewStub(grpc::CreateChannel(_rp.connect_host, grpc::InsecureChannelCredentials()));
            call->loop_count = _rp.loop_count;
            call->index = i;
            call->msg.set_text(line_vec_[i % line_vec_.size()]);
            call->context = new ClientContext;
            call->response_reader = call->stub->PrepareAsyncTokenize(call->context, call->msg, &cq_);
            call->response_reader->StartCall();
            call->response_reader->Finish(&call->msg, &call->status, (void*)call);
        }
        
        print_response = _rp.print_response;
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

//-----------------------------------------------------------------------------
//      main
//-----------------------------------------------------------------------------
int main(int argc, char *argv[]){
    Parameters p;

    if(parseArguments(p, argc, argv)) return 0;
    
    TokenizerClient tc;
    if(tc.start(p) == 0){
        tc.run();
    }
    
    return 0;
}

//-----------------------------------------------------------------------------

bool parseArguments(Parameters &_par, int argc, char *argv[]){
    using namespace boost::program_options;
    try{
        options_description desc("Bench server");
        desc.add_options()
            ("help,h", "List program options")
            ("connect-host,c", value<std::string>(&_par.connect_host)->default_value("localhost:" + _par.default_port), "gRPC Connect Host")
            ("secure,s", value<bool>(&_par.secure)->implicit_value(true)->default_value(true), "Secure communication")
            ("compress", value<bool>(&_par.compress)->implicit_value(true)->default_value(true), "Compress communication")
            ("loop-count,l", value<size_t>(&_par.loop_count)->default_value(1000), "Roundtrip count per connection")
            ("connection-count,N", value<size_t>(&_par.connection_count)->default_value(100), "Connection count")
            ("text_file,t", value<string>(&_par.text_file_path)->default_value("test_text.txt"), "Path to text file")
            ("print-response", value<bool>(&_par.print_response)->implicit_value(true)->default_value(false), "Prints the response")
        ;
        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);
        if (vm.count("help")) {
            cout << desc << "\n";
            return true;
        }
        return false;
    }catch(exception& e){
        cout << e.what() << "\n";
        return true;
    }
}
