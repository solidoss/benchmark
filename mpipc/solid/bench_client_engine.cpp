#include "solid/frame/manager.hpp"
#include "solid/frame/scheduler.hpp"
#include "solid/frame/service.hpp"

#include "solid/frame/aio/aioresolver.hpp"

#include "solid/frame/mpipc/mpipcservice.hpp"
#include "solid/frame/mpipc/mpipcconfiguration.hpp"
#include "solid/frame/mpipc/mpipcsocketstub_openssl.hpp"
#include "solid/frame/mpipc/mpipccompression_snappy.hpp"

#include <atomic>
#include <mutex>
#include <condition_variable>

#include "bench_client_engine.hpp"
#include "bench_protocol.hpp"

#include <fstream>
#include <sstream>

using namespace solid;
using namespace std;

namespace bench_client{

using AioSchedulerT = frame::Scheduler<frame::aio::Reactor>;
using StringVectorT = std::vector<std::string>;
using AtomicSizeT = std::atomic<size_t>;
    
namespace{
    
    struct Context{
        Context():ipcservice(manager), ramp_up_connection_count(0), ramp_down_connection_count(0), messages_transferred(0), tokens_transferred(0){
            
#ifdef SOLID_HAS_DEBUG
            Debug::the().levelMask("ew");
            Debug::the().moduleMask("any frame_mpipc");
            Debug::the().initStdErr(false, nullptr);
#endif
            
        }
        AioSchedulerT               scheduler;
        
        frame::Manager              manager;
        frame::mpipc::ServiceT      ipcservice;
        frame::aio::Resolver        resolver;
        
        StringVectorT               line_vec;
        
        mutex                       mtx;
        condition_variable          cnd;
        AtomicSizeT                 ramp_up_connection_count;
        size_t                      ramp_down_connection_count;
        size_t                      loop_count;
        size_t                      connection_count;
        bool                        print_response;
        AtomicSizeT                 messages_transferred;
        AtomicSizeT                 tokens_transferred;
    };
    
    unique_ptr<Context> ctx;
    
    template <class M>
    void complete_message(
        frame::mpipc::ConnectionContext& _rctx,
        std::shared_ptr<M>&              _rsent_msg_ptr,
        std::shared_ptr<M>&              _rrecv_msg_ptr,
        ErrorConditionT const&           _rerror)
    {
        idbg("message on client");
        SOLID_CHECK(not _rerror);
        SOLID_CHECK(_rrecv_msg_ptr && _rsent_msg_ptr);
        SOLID_CHECK(_rrecv_msg_ptr->str.empty() && _rrecv_msg_ptr->vec.size());
        
        auto   &con_val = *_rctx.any().cast<pair<size_t, size_t>>();
        
        ++ctx->messages_transferred;
        ctx->tokens_transferred += _rrecv_msg_ptr->vec.size();
        
        if(ctx->print_response){
            cout<<con_val.first<<':'<<con_val.second<<' ';
            for(const auto &token: _rrecv_msg_ptr->vec){
                cout<<'['<<token<<']'<<' ';
            }
            cout<<endl;
        }
        
        if(con_val.second > 1){
            --con_val.second;
            _rsent_msg_ptr->str = ctx->line_vec[con_val.first % ctx->line_vec.size()];
            ++con_val.first;
            
            _rctx.service().sendMessage(_rctx.recipientId(), std::move(_rsent_msg_ptr), {frame::mpipc::MessageFlagsE::WaitResponse});
        }else if(con_val.second == 1){
            --con_val.second;
        }else{
            _rctx.service().closeConnection(_rctx.recipientId());
        }
    }
    
    void connection_stop(frame::mpipc::ConnectionContext& _rctx)
    {
        if(_rctx.isConnectionActive()){
            unique_lock<mutex> lock(ctx->mtx);
            SOLID_ASSERT(_rctx.device());
            --ctx->ramp_down_connection_count;
            if(ctx->ramp_down_connection_count == 0){
                cout<<"Done: msgcnt = "<<ctx->messages_transferred<<" tokencnt = "<<ctx->tokens_transferred<<endl;
                _exit(0);
                ctx->cnd.notify_one();
            }
        }
    }

    void connection_start(frame::mpipc::ConnectionContext& _rctx)
    {
        idbg(_rctx.recipientId());
        const size_t crt_idx = ctx->ramp_up_connection_count.fetch_add(1);
        if(crt_idx < ctx->connection_count){
            _rctx.any() = make_pair(crt_idx + 2, ctx->loop_count - 1);
            _rctx.service().sendMessage(_rctx.recipientId(), std::make_shared<bench::Message>(ctx->line_vec[crt_idx % ctx->line_vec.size()]), {frame::mpipc::MessageFlagsE::WaitResponse});
            _rctx.service().sendMessage(_rctx.recipientId(), std::make_shared<bench::Message>(ctx->line_vec[(crt_idx + 1) % ctx->line_vec.size()]), {frame::mpipc::MessageFlagsE::WaitResponse});
        }
    }

    template <typename T>
    struct MessageSetup {
        void operator()(frame::mpipc::serialization_v1::Protocol& _rprotocol, const size_t _protocol_idx, const size_t _message_idx)
        {
            _rprotocol.registerType<T>(complete_message<T>, _protocol_idx, _message_idx);
        }
    };
    
}//namespace
    
    int start(
        const bool _secure, const bool _compress,
        const std::string &_connect_addr,
        const std::string &_default_port,
        const size_t _connection_count, const size_t _loop_count,
        const std::string &_text_file,
        const bool _print_response){
        
        ctx.reset(new Context);
        {
            ifstream ifs(_text_file);
            if(ifs){
                while(!ifs.eof()){
                    ctx->line_vec.emplace_back();
                    getline(ifs, ctx->line_vec.back());
                    if(ctx->line_vec.back().empty()){
                        ctx->line_vec.pop_back();
                    }
                }
            }else{
                return -1;
            }
        }
        
        ErrorConditionT     err;
        
        err = ctx->scheduler.start(1);

        if (err) {
            return -2;
        }
        
        err = ctx->resolver.start(1);

        if (err) {
            return -3;
        }
        
        {
            auto                        proto = frame::mpipc::serialization_v1::Protocol::create();
            frame::mpipc::Configuration cfg(ctx->scheduler, proto);

            bench::ProtoSpecT::setup<MessageSetup>(*proto);
            
            cfg.pool_max_active_connection_count = _connection_count;

            cfg.client.name_resolve_fnc = frame::mpipc::InternetResolverF(ctx->resolver, _default_port.c_str());

            cfg.client.connection_start_state = frame::mpipc::ConnectionState::Active;
            
            cfg.connection_stop_fnc         = connection_stop;
            cfg.client.connection_start_fnc = connection_start;
            cfg.connection_recv_buffer_start_capacity_kb = 64;
            cfg.connection_send_buffer_start_capacity_kb = 64;
            
            if(_secure){
                frame::mpipc::openssl::setup_client(
                    cfg,
                    [](frame::aio::openssl::Context& _rctx) -> ErrorCodeT {
                        _rctx.loadVerifyFile("echo-ca-cert.pem");
                        _rctx.loadCertificateFile("echo-client-cert.pem");
                        _rctx.loadPrivateKeyFile("echo-client-key.pem");
                        return ErrorCodeT();
                    },
                    frame::mpipc::openssl::NameCheckSecureStart{"echo-server"});
            }

            if(_compress){
                frame::mpipc::snappy::setup(cfg);
            }

            err = ctx->ipcservice.reconfigure(std::move(cfg));

            if (err) {
                ctx->manager.stop();
                return -4;
            }
            
            err = ctx->ipcservice.createConnectionPool(_connect_addr.c_str(), _connection_count);
            
            if (err) {
                ctx->manager.stop();
                return -5;
            }
            ctx->loop_count = _loop_count;
            ctx->ramp_down_connection_count = _connection_count;
            ctx->connection_count = _connection_count;
            ctx->print_response = _print_response;
            return 0;
        }
    }
    
    void wait(){
        unique_lock<mutex> lock(ctx->mtx);

        if (not ctx->cnd.wait_for(lock, std::chrono::seconds(1000), []() { return ctx->ramp_down_connection_count == 0; })) {
            SOLID_THROW("Process is taking too long.");
        }
        
    }
    
    void stop(const bool _wait){
        ctx->manager.stop();
        ctx->scheduler.stop(_wait);
    }
}//namespace bench

