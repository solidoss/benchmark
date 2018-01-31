#include "solid/system/debug.hpp"
#include "solid/frame/manager.hpp"
#include "solid/frame/scheduler.hpp"
#include "solid/frame/service.hpp"

#include "solid/frame/aio/aioresolver.hpp"

#include "solid/frame/mpipc/mpipcservice.hpp"
#include "solid/frame/mpipc/mpipcconfiguration.hpp"
#include "solid/frame/mpipc/mpipcsocketstub_openssl.hpp"
#include "solid/frame/mpipc/mpipccompression_snappy.hpp"

#include "bench_server_engine.hpp"
#include "bench_protocol.hpp"

#include <sstream>

using namespace solid;
using namespace std;

using AioSchedulerT = frame::Scheduler<frame::aio::Reactor>;

namespace bench{
    
namespace{
    struct Context{
        Context():ipcservice(manager){
#ifdef SOLID_HAS_DEBUG
            Debug::the().levelMask("ew");
            Debug::the().moduleMask("any");
            Debug::the().initStdErr(false, nullptr);
#endif
        }
        AioSchedulerT               scheduler;
        
        frame::Manager              manager;
        frame::mpipc::ServiceT      ipcservice;
    } ctx;
    
    template <class M>
    void complete_message(
        frame::mpipc::ConnectionContext& _rctx,
        std::shared_ptr<M>&              _rsent_msg_ptr,
        std::shared_ptr<M>&              _rrecv_msg_ptr,
        ErrorConditionT const&           _rerror)
    {
        SOLID_CHECK(not _rerror);

        if (_rrecv_msg_ptr) {
            SOLID_CHECK(not _rsent_msg_ptr);
            {
                istringstream iss{std::move(_rrecv_msg_ptr->str)};
                while(not iss.eof()){
                    _rrecv_msg_ptr->vec.emplace_back();
                    iss>>_rrecv_msg_ptr->vec.back();
                }
                _rrecv_msg_ptr->str.clear();
            }
            SOLID_CHECK(!_rctx.service().sendResponse(_rctx.recipientId(), std::move(_rrecv_msg_ptr)));
        }

        if (_rsent_msg_ptr) {
            SOLID_CHECK(not _rrecv_msg_ptr);
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
    
    int server_start(const bool _secure, const bool _compress, std::string &&_listen_addr){
        ErrorConditionT     err;
        
        err = ctx.scheduler.start(1);

        if (err) {
            return -1;
        }
        
        {
            auto                        proto = frame::mpipc::serialization_v1::Protocol::create();
            frame::mpipc::Configuration cfg(ctx.scheduler, proto);

            bench::ProtoSpecT::setup<MessageSetup>(*proto);

            cfg.server.listener_address_str = std::move(_listen_addr);

            cfg.server.connection_start_state = frame::mpipc::ConnectionState::Active;
            
            if(_secure){
                frame::mpipc::openssl::setup_server(
                    cfg,
                    [](frame::aio::openssl::Context &_rctx) -> ErrorCodeT{
                        _rctx.loadVerifyFile("echo-ca-cert.pem"/*"/etc/pki/tls/certs/ca-bundle.crt"*/);
                        _rctx.loadCertificateFile("echo-server-cert.pem");
                        _rctx.loadPrivateKeyFile("echo-server-key.pem");
                        return ErrorCodeT();
                    },
                    frame::mpipc::openssl::NameCheckSecureStart{"echo-client"}//does nothing - OpenSSL does not check for hostname on SSL_accept
                );
            }

            if(_compress){
                frame::mpipc::snappy::setup(cfg);
            }

            err = ctx.ipcservice.reconfigure(std::move(cfg));

            if (err) {
                ctx.manager.stop();
                return -2;
            }
            
            return ctx.ipcservice.configuration().server.listenerPort();
        }
    }
    
    void server_stop(const bool _wait){
        ctx.manager.stop();
        ctx.scheduler.stop(_wait);
    }
}//namespace bench
