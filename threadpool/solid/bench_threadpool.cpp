#include "solid/system/log.hpp"
#include "solid/utility/threadpool.hpp"
#include <iostream>
#include <memory>
#include <variant>

using namespace solid;
using namespace std;

namespace {
Logger logger{"test"};

using IndexT                          = uint64_t;
constexpr size_t buffer_capacity      = 0;
atomic<uint64_t> handle_call_count    = 0;
atomic<uint64_t> active_message_count = 0;
atomic<bool>     running              = true;

struct Message {
    IndexT index_{0};
};

using VariantT = std::variant<Message, unique_ptr<Message>>;

using ThreadPoolT = ThreadPool<VariantT, size_t, solid::DefaultThreadPoolTraits<solid::EmptyThreadPoolStatistic>>;

struct Context {
    ThreadPoolT& rtp_;
};

void handle(Message& _rv, Context& _rctx)
{
    //++handle_call_count;
    if (--_rv.index_) {
        _rctx.rtp_.pushOne(std::move(_rv));
    } else if (active_message_count.fetch_sub(1) == 1) {
        running = false;
        running.notify_one();
    }
}

void handle(unique_ptr<Message>& _rv, Context& _rctx)
{
    //++handle_call_count;
    if (--_rv->index_) {
        _rctx.rtp_.pushOne(std::move(_rv));
    } else if (active_message_count.fetch_sub(1) == 1) {
        running = false;
        running.notify_one();
    }
}
} // namespace

int main(int argc, char* argv[])
{
    solid::log_start(std::cerr, {".*:EW", "test:EWS"});

    size_t repeat_count     = 10000;
    size_t message_count    = 10000;
    size_t tp1_thread_count = 4;
    size_t tp2_thread_count = 4;
    bool   use_unique_ptr   = false;

    {
        ThreadPoolT tp1;
        ThreadPoolT tp2;
        Context     ctx1{tp2};
        Context     ctx2{tp1};

        tp1.start(
            {tp1_thread_count},
            [](size_t, Context&) {},
            [](size_t, Context&) {},
            [](VariantT& _var, Context& _rctx) {
                std::visit([&_rctx](auto& _rv) { handle(_rv, _rctx); }, _var);
            },
            [](size_t, Context&) {

            },
            ref(ctx1));

        tp2.start(
            {tp2_thread_count},
            [](size_t, Context&) {},
            [](size_t, Context&) {},
            [](VariantT& _var, Context& _rctx) {
                std::visit([&_rctx](auto& _rv) { handle(_rv, _rctx); }, _var);
            },
            [](size_t, Context&) {

            },
            ref(ctx2));

        active_message_count  = message_count;
        const auto start_time = chrono::high_resolution_clock::now();
        for (size_t i = 0; i < message_count; ++i) {
            if (use_unique_ptr) {
                tp1.pushOne(make_unique<Message>(repeat_count));
            } else {
                tp1.pushOne(Message{repeat_count});
            }
        }

        running.wait(true);
        const auto stop_time = chrono::high_resolution_clock::now();
        cout << "Duration: " << chrono::duration_cast<chrono::microseconds>(stop_time - start_time).count() << "us" << endl;

        tp2.stop();
        tp1.stop();
        solid_log(logger, Statistic, "ThreadPool 1 statistic: " << tp1.statistic());
        solid_log(logger, Statistic, "ThreadPool 2 statistic: " << tp2.statistic());
    }
    solid_log(logger, Warning, "handle_call_count = " << handle_call_count.load());
    return 0;
}