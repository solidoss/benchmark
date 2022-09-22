#include "solid/frame/mprpc/mprpcservice.hpp"
#include "solid/frame/mprpc/mprpcconfiguration.hpp"
#include "solid/frame/mprpc/mprpccompression_snappy.hpp"
#include "solid/frame/mprpc/mprpcsocketstub_openssl.hpp"

#include "solid/system/log.hpp"
#include "solid/frame/manager.hpp"
#include "solid/frame/scheduler.hpp"
#include "solid/frame/service.hpp"

#include "solid/frame/aio/aioresolver.hpp"

#include "bench_server_engine.hpp"
#include "bench_protocol.hpp"

#include <sstream>

using namespace solid;
using namespace std;

using AioSchedulerT = frame::Scheduler<frame::aio::Reactor>;

namespace bench_server{
    
namespace{
    struct Context{
        Context():ipcservice(manager){
        }
        AioSchedulerT               scheduler;
        
        frame::Manager              manager;
        frame::mprpc::ServiceT      ipcservice;
    };
    
    unique_ptr<Context> ctx;
    
    template <class M>
    void complete_message(
        frame::mprpc::ConnectionContext& _rctx,
        std::shared_ptr<M>&              _rsent_msg_ptr,
        std::shared_ptr<M>&              _rrecv_msg_ptr,
        ErrorConditionT const&           _rerror)
    {
        solid_dbg(generic_logger, Info, "received message on server");
        solid_check(!_rerror);

        if (_rrecv_msg_ptr) {
            solid_check(!_rsent_msg_ptr);
            {
                istringstream iss{std::move(_rrecv_msg_ptr->str)};
                while(!iss.eof()){
                    _rrecv_msg_ptr->vec.emplace_back();
                    iss>>_rrecv_msg_ptr->vec.back();
                }
                _rrecv_msg_ptr->str.clear();
            }
            solid_check(!_rctx.service().sendResponse(_rctx.recipientId(), std::move(_rrecv_msg_ptr)));
        }

        if (_rsent_msg_ptr) {
            solid_check(!_rrecv_msg_ptr);
        }
    }
    
}//namespace
    
int start(const bool _secure, const bool _compress, const std::string &_listen_addr){
    ErrorConditionT     err;
    
    ctx.reset(new Context);
    
    ctx->scheduler.start(1);
    
    {
        auto proto = frame::mprpc::serialization_v3::create_protocol<reflection::v1::metadata::Variant, uint8_t>(
            reflection::v1::metadata::factory,
            [&](auto& _rmap) {
                auto lambda = [&](const uint8_t _id, const std::string_view _name, auto const& _rtype) {
                    using TypeT = typename std::decay_t<decltype(_rtype)>::TypeT;
                    _rmap.template registerMessage<TypeT>(_id, _name, complete_message<TypeT>);
                };
                bench::configure_protocol(lambda);
            });
        frame::mprpc::Configuration cfg(ctx->scheduler, proto);

        cfg.server.listener_address_str = _listen_addr;

        cfg.server.connection_start_state = frame::mprpc::ConnectionState::Active;
        cfg.connection_recv_buffer_start_capacity_kb = 64;
        cfg.connection_send_buffer_start_capacity_kb = 64;
        
        if(_secure){
            frame::mprpc::openssl::setup_server(
                cfg,
                [](frame::aio::openssl::Context &_rctx) -> ErrorCodeT{
                    _rctx.loadVerifyFile("echo-ca-cert.pem"/*"/etc/pki/tls/certs/ca-bundle.crt"*/);
                    _rctx.loadCertificateFile("echo-server-cert.pem");
                    _rctx.loadPrivateKeyFile("echo-server-key.pem");
                    return ErrorCodeT();
                },
                frame::mprpc::openssl::NameCheckSecureStart{"echo-client"}//does nothing - OpenSSL does not check for hostname on SSL_accept
            );
        }

        if(_compress){
            frame::mprpc::snappy::setup(cfg);
        }

        ctx->ipcservice.start(std::move(cfg));
        
        return ctx->ipcservice.configuration().server.listenerPort();
    }
}

void stop(const bool _wait){
    ctx->manager.stop();
    ctx->scheduler.stop(_wait);
    ctx.reset();
}
}//namespace bench
