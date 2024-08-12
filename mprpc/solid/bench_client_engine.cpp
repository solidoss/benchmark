#include "solid/frame/mprpc/mprpccompression_snappy.hpp"
#include "solid/frame/mprpc/mprpcconfiguration.hpp"
#include "solid/frame/mprpc/mprpcservice.hpp"
#include "solid/frame/mprpc/mprpcsocketstub_openssl.hpp"

#include "solid/frame/manager.hpp"
#include "solid/frame/scheduler.hpp"
#include "solid/frame/service.hpp"

#include "solid/utility/threadpool.hpp"

#include "solid/frame/aio/aioresolver.hpp"

#include <atomic>
#include <condition_variable>
#include <mutex>

static void message_created();

#include "bench_client_engine.hpp"
#include "bench_protocol.hpp"

#include <fstream>
#include <sstream>

using namespace solid;
using namespace std;
namespace {
const LoggerT logger("bench_client");
}

namespace bench_client {

using AioSchedulerT = frame::Scheduler<frame::aio::Reactor<Event<32>>>;
using StringVectorT = std::vector<std::string>;
using AtomicSizeT   = std::atomic<size_t>;
using CallPoolT     = ThreadPool<Function<void(), 80>, Function<void(), 80>>;

namespace {

atomic_size_t msg_count{0};

struct Context {
    AioSchedulerT scheduler;

    frame::Manager         manager;
    frame::mprpc::ServiceT ipcservice;
    CallPoolT              cwp{{1, 100, 0}, [](const size_t) {}, [](const size_t) {}};
    frame::aio::Resolver   resolver;

    StringVectorT line_vec;

    mutex              mtx;
    condition_variable cnd;
    AtomicSizeT        ramp_up_connection_count;
    size_t             ramp_down_connection_count;
    AtomicSizeT        pending_ramp_down_connection_count;
    size_t             loop_count;
    size_t             connection_count;
    bool               print_response;
    AtomicSizeT        messages_transferred;
    AtomicSizeT        tokens_transferred;
    atomic<uint64_t>   payload_trasferred;

    Context()
        : ipcservice(manager)
        , resolver([this](std::function<void()>&& _fnc) {
            cwp.pushOne(std::move(_fnc));
        })
        , ramp_up_connection_count(0)
        , ramp_down_connection_count(0)
        , pending_ramp_down_connection_count(0)
        , messages_transferred(0)
        , tokens_transferred(0)
        , payload_trasferred(0)
    {
    }

    void print()
    {
        cout << "Tokens trasferred: " << tokens_transferred << endl;
        cout << "Messages transferred: " << messages_transferred << endl;
        cout << "Payload transferred: " << payload_trasferred << endl;
        cout << "Client Messages created: " << msg_count.load() << endl;
    }
};

auto create_message_ptr = [](auto& _rctx, auto& _rmsgptr) {
    using PtrT  = std::decay_t<decltype(_rmsgptr)>;
    using ElemT = typename PtrT::element_type;

    _rmsgptr = ElemT::create();
};

unique_ptr<Context> ctx;

// #define USE_TWO_MESSAGES

template <class M>
void complete_message(frame::mprpc::ConnectionContext& _rctx,
    frame::mprpc::MessagePointerT<M>&                  _rsent_msg_ptr,
    frame::mprpc::MessagePointerT<M>&                  _rrecv_msg_ptr,
    ErrorConditionT const&                             _rerror)
{
    solid_dbg(generic_logger, Info, "message on client");
    solid_check(!_rerror);
    solid_check(_rrecv_msg_ptr && _rsent_msg_ptr);
    solid_check(_rrecv_msg_ptr->str.empty() && _rrecv_msg_ptr->vec.size());

    auto& con_val = *_rctx.any().cast<pair<size_t, size_t>>();

    ++ctx->messages_transferred;
    ctx->tokens_transferred += _rrecv_msg_ptr->vec.size();

#if 0
    ctx->payload_trasferred += _rsent_msg_ptr->str.size();
    for (const auto& v : _rrecv_msg_ptr->vec) {
        ctx->payload_trasferred += v.size();
    }
#endif

    if (ctx->print_response) {
        cout << con_val.first << ':' << con_val.second << ' ';
        for (const auto& token : _rrecv_msg_ptr->vec) {
            cout << '[' << token << ']' << ' ';
        }
        cout << endl;
    }

    _rrecv_msg_ptr->str.clear();
    _rrecv_msg_ptr->vec.clear();
    cacheable_cache(std::move(_rrecv_msg_ptr));

    if (con_val.second > 1) {
        --con_val.second;
        _rsent_msg_ptr->str = ctx->line_vec[con_val.first % ctx->line_vec.size()];
        ++con_val.first;

        _rctx.service().sendMessage(_rctx.recipientId(), std::move(_rsent_msg_ptr),
            {frame::mprpc::MessageFlagsE::AwaitResponse});
    } else if (con_val.second == 1) {
        --con_val.second;
#ifdef USE_TWO_MESSAGES
    } else {
#endif
        solid_dbg(logger, Info, _rctx.recipientId());
        // cannot directly close the connection here because the pool will create
        // another one automatically
        //_rctx.service().closeConnection(_rctx.recipientId());
        const size_t crtcnt = ctx->pending_ramp_down_connection_count.fetch_add(1) + 1;
        if (crtcnt == ctx->connection_count) {
            // all connectios stopping
            _rctx.service().forceCloseConnectionPool(
                _rctx.recipientId(), [](frame::mprpc::ConnectionContext& _rctx) {
                    solid_dbg(generic_logger, Info, "------------------");
                    lock_guard<mutex> lock(ctx->mtx);
                    ctx->print();
                    cout << "ramp_down_connection_count: "
                         << ctx->ramp_down_connection_count << endl;
                    // solid_check(ctx->ramp_down_connection_count == 1, "failed:
                    // "<<ctx->ramp_down_connection_count); _exit(0);
                    ctx->cnd.notify_one();
                });
        }
    }
}

void connection_stop(frame::mprpc::ConnectionContext& _rctx)
{
    if (_rctx.isConnectionActive()) {
        solid_dbg(logger, Info, _rctx.recipientId());
        unique_lock<mutex> lock(ctx->mtx);
        solid_assert(_rctx.device());
        --ctx->ramp_down_connection_count;
        ctx->cnd.notify_one();
    }
}

void connection_start(frame::mprpc::ConnectionContext& _rctx)
{
    const size_t crt_idx = ctx->ramp_up_connection_count.fetch_add(1);
    if (crt_idx < ctx->connection_count) {
        solid_dbg(logger, Info, _rctx.recipientId());
#ifdef USE_TWO_MESSAGES
        const size_t v = 1;
#else
        const size_t v = 0;
#endif
        _rctx.any() = make_pair(crt_idx + 1 + v, ctx->loop_count - v);
        _rctx.service().sendMessage(
            _rctx.recipientId(),
            frame::mprpc::make_message<EnableCacheable<bench::Message>>(
                ctx->line_vec[crt_idx % ctx->line_vec.size()]),
            {frame::mprpc::MessageFlagsE::AwaitResponse});
#ifdef USE_TWO_MESSAGES
        _rctx.service().sendMessage(
            _rctx.recipientId(),
            frame::mprpc::make_message<EnableCacheable<bench::Message>>(
                ctx->line_vec[(crt_idx + 1) % ctx->line_vec.size()]),
            {frame::mprpc::MessageFlagsE::AwaitResponse});
#endif
    }
}

} // namespace

int start(const bool _secure, const bool _compress,
    const std::string& _connect_addr, const std::string& _default_port,
    const size_t _connection_count, const size_t _loop_count,
    const std::string& _text_file, const bool _print_response)
{

    solid_log(logger, Info, "");
    ctx.reset(new Context);
    {
        ifstream ifs(_text_file);
        if (ifs) {
            while (!ifs.eof()) {
                ctx->line_vec.emplace_back();
                getline(ifs, ctx->line_vec.back());
                if (ctx->line_vec.back().empty()) {
                    ctx->line_vec.pop_back();
                }
            }
        } else {
            return -1;
        }
    }

    ctx->scheduler.start(1);

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

        cfg.pool_max_active_connection_count = _connection_count;

        cfg.client.name_resolve_fnc = frame::mprpc::InternetResolverF(ctx->resolver, _default_port.c_str());

        cfg.client.connection_start_state = frame::mprpc::ConnectionState::Active;

        cfg.connection_stop_fnc                      = &connection_stop;
        cfg.client.connection_start_fnc              = &connection_start;
        cfg.connection_recv_buffer_start_capacity_kb = 64;
        cfg.connection_send_buffer_start_capacity_kb = 64;

        if (_secure) {
            frame::mprpc::openssl::setup_client(
                cfg,
                [](frame::aio::openssl::Context& _rctx) -> ErrorCodeT {
                    _rctx.loadVerifyFile("echo-ca-cert.pem");
                    _rctx.loadCertificateFile("echo-client-cert.pem");
                    _rctx.loadPrivateKeyFile("echo-client-key.pem");
                    return ErrorCodeT();
                },
                frame::mprpc::openssl::NameCheckSecureStart{"echo-server"});
        }

        if (_compress) {
            frame::mprpc::snappy::setup(cfg);
        }

        ctx->ipcservice.start(std::move(cfg));

        ctx->ipcservice.createConnectionPool(_connect_addr.c_str(),
            _connection_count);

        ctx->loop_count                 = _loop_count;
        ctx->ramp_down_connection_count = _connection_count;
        ctx->connection_count           = _connection_count;
        ctx->print_response             = _print_response;
        return 0;
    }
}

void wait()
{
    unique_lock<mutex> lock(ctx->mtx);

    if (!ctx->cnd.wait_for(lock, std::chrono::seconds(1000), []() {
            return ctx->ramp_down_connection_count == 0;
        })) {
        solid_throw("Process is taking too long.");
    }
}

void stop(const bool _wait)
{
    ctx->manager.stop();
    ctx->scheduler.stop(_wait);
    ctx.reset();
}
} // namespace bench_client

void message_created()
{
    ++bench_client::msg_count;
}
