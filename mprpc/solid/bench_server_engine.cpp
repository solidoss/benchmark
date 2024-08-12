#include "solid/frame/mprpc/mprpccompression_snappy.hpp"
#include "solid/frame/mprpc/mprpcconfiguration.hpp"
#include "solid/frame/mprpc/mprpcservice.hpp"
#include "solid/frame/mprpc/mprpcsocketstub_openssl.hpp"

#include "solid/frame/manager.hpp"
#include "solid/frame/scheduler.hpp"
#include "solid/frame/service.hpp"
#include "solid/system/log.hpp"

#include "solid/frame/aio/aioresolver.hpp"

static void message_created();

#include "bench_protocol.hpp"
#include "bench_server_engine.hpp"

#include <sstream>

using namespace solid;
using namespace std;

using AioSchedulerT = frame::Scheduler<frame::aio::Reactor<Event<32>>>;

namespace bench_server {

namespace {
struct Context {
    Context()
        : ipcservice(manager)
    {
    }
    AioSchedulerT scheduler;

    frame::Manager         manager;
    frame::mprpc::ServiceT ipcservice;
};

auto create_message_ptr = [](auto& _rctx, auto& _rmsgptr) {
    using PtrT  = std::decay_t<decltype(_rmsgptr)>;
    using ElemT = typename PtrT::element_type;

    _rmsgptr = ElemT::create();
};

unique_ptr<Context> ctx;

template <class M>
void complete_message(frame::mprpc::ConnectionContext& _rctx,
    frame::mprpc::MessagePointerT<M>&                  _rsent_msg_ptr,
    frame::mprpc::MessagePointerT<M>&                  _rrecv_msg_ptr,
    ErrorConditionT const&                             _rerror)
{
    solid_dbg(generic_logger, Info, "received message on server");
    solid_check(!_rerror, "error " << _rerror.message());

    if (_rrecv_msg_ptr) {
        solid_check(!_rsent_msg_ptr);
        {
            istringstream iss{std::move(_rrecv_msg_ptr->str)};
            while (!iss.eof()) {
                _rrecv_msg_ptr->vec.emplace_back();
                iss >> _rrecv_msg_ptr->vec.back();
            }
            _rrecv_msg_ptr->str.clear();
        }
        solid_check(!_rctx.service().sendResponse(_rctx.recipientId(),
            std::move(_rrecv_msg_ptr)));
    }

    if (_rsent_msg_ptr) {
        solid_check(!_rrecv_msg_ptr);
        _rsent_msg_ptr->str.clear();
        _rsent_msg_ptr->vec.clear();
        cacheable_cache(std::move(_rsent_msg_ptr));
    }
}
atomic_size_t msg_count{0};
} // namespace

int start(const bool _secure, const bool _compress,
    const std::string& _listen_addr)
{
    ErrorConditionT err;

    ctx.reset(new Context);

    ctx->scheduler.start(3);

    {
        auto proto = frame::mprpc::serialization_v3::create_protocol<
            reflection::v1::metadata::Variant, uint8_t>(
            reflection::v1::metadata::factory, [&](auto& _rmap) {
                auto lambda = [&](const uint8_t _id, const std::string_view _name,
                                  auto const& _rtype) {
                    using TypeT = typename std::decay_t<decltype(_rtype)>::TypeT;
                    _rmap.template registerMessage<EnableCacheable<TypeT>>(_id, _name,
                        complete_message<EnableCacheable<TypeT>>, create_message_ptr);
                };
                bench::configure_protocol(lambda);
            });
        frame::mprpc::Configuration cfg(ctx->scheduler, proto);

        cfg.server.listener_address_str = _listen_addr;

        cfg.server.connection_start_state            = frame::mprpc::ConnectionState::Active;
        cfg.connection_recv_buffer_start_capacity_kb = 64;
        cfg.connection_send_buffer_start_capacity_kb = 64;

        if (_secure) {
            frame::mprpc::openssl::setup_server(
                cfg,
                [](frame::aio::openssl::Context& _rctx) -> ErrorCodeT {
                    _rctx.loadVerifyFile(
                        "echo-ca-cert.pem" /*"/etc/pki/tls/certs/ca-bundle.crt"*/);
                    _rctx.loadCertificateFile("echo-server-cert.pem");
                    _rctx.loadPrivateKeyFile("echo-server-key.pem");
                    return ErrorCodeT();
                },
                frame::mprpc::openssl::NameCheckSecureStart{"echo-client"}
                // does nothing - OpenSSL does not check for hostname on SSL_accept
            );
        }

        if (_compress) {
            frame::mprpc::snappy::setup(cfg);
        }

        {
            frame::mprpc::ServiceStartStatus start_status;
            ctx->ipcservice.start(start_status, std::move(cfg));

            solid_dbg(generic_logger, Info, "server listens on: " << start_status.listen_addr_vec_.back());
            return start_status.listen_addr_vec_.back().port();
        }
    }
}

void stop(const bool _wait)
{
    cout << "Server Messages created: " << msg_count.load() << endl;
    ctx->manager.stop();
    ctx->scheduler.stop(_wait);
    ctx.reset();
}
} // namespace bench_server

void message_created()
{
    ++bench_server::msg_count;
}