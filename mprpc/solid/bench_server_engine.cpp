#include "solid/frame/mprpc/mprpccompression_snappy.hpp"
#include "solid/frame/mprpc/mprpcconfiguration.hpp"
#include "solid/frame/mprpc/mprpcservice.hpp"
#include "solid/frame/mprpc/mprpcsocketstub_openssl.hpp"

#include "solid/frame/manager.hpp"
#include "solid/frame/scheduler.hpp"
#include "solid/frame/service.hpp"
#include "solid/system/log.hpp"

#include "solid/utility/pool.hpp"
#include "solid/utility/threadpool.hpp"

#include "solid/frame/aio/aioresolver.hpp"
#include <any>
#include <functional>

static void request_created();
static void response_created();

#include "bench_protocol.hpp"
#include "bench_server_engine.hpp"

#include <sstream>

using namespace solid;
using namespace std;
namespace bench_server {

namespace {
struct Context;

using AioSchedulerT = frame::Scheduler<frame::aio::Reactor<Event<32>>>;
using ThreadPoolT   = ThreadPool<SmallFunctionT<void(Context&)>, SmallFunctionT<void(Context&)>>;

struct Context {
    Context(bool _use_tp)
        : ipcservice(manager)
        , use_tp(_use_tp)
    {
    }
    AioSchedulerT scheduler;

    frame::Manager         manager;
    frame::mprpc::ServiceT ipcservice;
    const bool             use_tp;
    ThreadPoolT            tp;
};

using ContextRefT = std::reference_wrapper<Context>;

auto create_message_ptr = [](auto& _rctx, auto& _rmsgptr) {
    using PtrT  = std::decay_t<decltype(_rmsgptr)>;
    using ElemT = typename PtrT::element_type;

    _rmsgptr = Pool<ElemT>::create();
};

unique_ptr<Context> ctx;
atomic_size_t       req_count{0};
atomic_size_t       res_count{0};

template <typename Req, typename Res>
void complete_catch_all(frame::mprpc::ConnectionContext&,
    frame::mprpc::SendMessagePointerT<Req>& _rsent_msg_ptr,
    frame::mprpc::RecvMessagePointerT<Res>&,
    ErrorConditionT const&)
{
    if constexpr (std::is_same_v<Req, bench::Response>) {
        solid_check(_rsent_msg_ptr.useCount() == 1);
    }
    _rsent_msg_ptr.reset();
}

template <typename Req, typename Res>
void complete_message(frame::mprpc::ConnectionContext& _rctx,
    frame::mprpc::SendMessagePointerT<Req>&            _rsent_msg_ptr,
    frame::mprpc::RecvMessagePointerT<Res>&            _rrecv_msg_ptr,
    ErrorConditionT const&                             _rerror)
{
    solid_dbg(generic_logger, Info, "received message on server");
    solid_check(!_rerror, "error " << _rerror.message());

    if (_rrecv_msg_ptr) {
        solid_check(!_rsent_msg_ptr);
        auto exec = [recipient_id = _rctx.recipientId(), _rrecv_msg_ptr = std::move(_rrecv_msg_ptr)](Context& _rctx) mutable {
            auto res = solid::frame::mprpc::make_pool_message<bench::Response>(*_rrecv_msg_ptr);
            if (true) {
                istringstream iss{std::move(_rrecv_msg_ptr->str)};
                while (!iss.eof()) {
                    res->vec.emplace_back();
                    iss >> res->vec.back();
                }
                _rrecv_msg_ptr->str.clear();
            } else {
                res->vec.emplace_back(std::move(_rrecv_msg_ptr->str));
            }
            solid_check(!_rctx.ipcservice.sendResponse(recipient_id,
                std::move(res)));
        };
        auto& ctx_ref = *_rctx.service().any().cast<ContextRefT>();
        if (ctx_ref.get().use_tp) {
            ctx_ref.get().tp.pushOne(std::move(exec));
        } else {
            exec(ctx_ref);
        }
    }

    if (_rsent_msg_ptr) {
        solid_check(!_rrecv_msg_ptr);
    }
}
} // namespace

int start(const bool _secure, const bool _compress,
    const std::string& _listen_addr, uint32_t _tp_thread_count)
{
    ErrorConditionT err;

    ctx.reset(new Context(_tp_thread_count != 0));

    ctx->scheduler.start(3);
    if (_tp_thread_count) {
        ctx->tp.start({_tp_thread_count, 1000, 0}, [](const size_t, Context&) {}, [](const size_t, Context&) {}, ref(*ctx));
    }

    {
        auto proto = frame::mprpc::serialization_v3::create_protocol<
            reflection::v1::metadata::Variant, uint8_t>(
            reflection::v1::metadata::factory, [&](auto& _rmap) {
                auto lambda = [&](const uint8_t _id, const std::string_view _name,
                                  auto const& _rtype) {
                    using TypeT = typename std::decay_t<decltype(_rtype)>::type;
                    if constexpr (std::is_same_v<TypeT, bench::Request>) {
                        _rmap.template registerMessage<TypeT>(_id, _name, complete_message<TypeT, TypeT>, create_message_ptr);
                    } else {
                        _rmap.template registerMessage<TypeT>(_id, _name, complete_catch_all<TypeT, TypeT>, create_message_ptr);
                    }
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
            ctx->ipcservice.start(start_status, std::move(cfg), std::ref(*ctx));

            solid_dbg(generic_logger, Info, "server listens on: " << start_status.listen_addr_vec_.back());
            return start_status.listen_addr_vec_.back().port();
        }
    }
}

void stop(const bool _wait)
{
    cout << "Server Requests created: " << req_count.load() << endl;
    cout << "Server Responses created: " << res_count.load() << endl;
    ctx->manager.stop();
    ctx->scheduler.stop(_wait);
    cout << "ThreadPool stats: " << ctx->tp.statistic() << endl;
    ctx.reset();
}
} // namespace bench_server

void request_created()
{
    ++bench_server::req_count;
}
void response_created()
{
    ++bench_server::res_count;
}