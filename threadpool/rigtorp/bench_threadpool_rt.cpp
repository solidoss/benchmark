#include "rigtorp/MPMCQueue.h"
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <variant>
#include <vector>

using namespace std;
using namespace rigtorp;

namespace {
using IndexT                          = uint64_t;
constexpr size_t buffer_capacity      = 0;
atomic<uint64_t> handle_call_count    = 0;
atomic<uint64_t> active_message_count = 0;
atomic<bool>     running              = true;

struct Message {
    IndexT index_{0};
};

using VariantT = std::variant<Message, unique_ptr<Message>>;
using QueueT   = MPMCQueue<VariantT>;
struct Context {
    QueueT& rq_;
};

void handle(Message& _rv, Context& _rctx)
{
    //++handle_call_count;
    if (--_rv.index_) {
        while (!_rctx.rq_.try_push(std::move(_rv)))
            ;
    } else if (active_message_count.fetch_sub(1) == 1) {
        running = false;
        running.notify_one();
    }
}

void handle(unique_ptr<Message>& _rv, Context& _rctx)
{
    //++handle_call_count;
    if (--_rv->index_) {
        while (!_rctx.rq_.try_push(std::move(_rv)))
            ;
    } else if (active_message_count.fetch_sub(1) == 1) {
        running = false;
        running.notify_one();
    }
}

struct Notificator {
    alignas(hardware_constructive_interference_size) bool running_{true};
};

} // namespace

int main(int argc, char* argv[])
{

    size_t  repeat_count     = 10000;
    size_t  message_count    = 10000;
    size_t  tp1_thread_count = 4;
    size_t  tp2_thread_count = 4;
    bool    use_unique_ptr   = false;
    QueueT  q1(1024 * 16);
    QueueT  q2(1024 * 16);
    Context ctx1{q2};
    Context ctx2{q1};

    auto consumer = [](QueueT& _rq, Context& _rctx, Notificator* _pn) {
        do {
            VariantT v;
            while (_rq.try_pop(v)) {
                std::visit([&_rctx](auto& _rv) { handle(_rv, _rctx); }, v);
            }
        } while (_pn->running_);
    };

    std::vector<tuple<std::thread, unique_ptr<Notificator>>> consumers;
    consumers.reserve(tp1_thread_count + tp2_thread_count);
    for (size_t i = 0; i < tp1_thread_count; ++i) {
        auto pn = make_unique<Notificator>();
        consumers.emplace_back(std::thread{consumer, ref(q1), ref(ctx1), pn.get()}, std::move(pn));
    }
    for (size_t i = 0; i < tp2_thread_count; ++i) {
        auto pn = make_unique<Notificator>();
        consumers.emplace_back(std::thread{consumer, ref(q2), ref(ctx2), pn.get()}, std::move(pn));
    }

    active_message_count  = message_count;
    const auto start_time = chrono::high_resolution_clock::now();
    for (size_t i = 0; i < message_count; ++i) {
        auto mp = make_unique<Message>(repeat_count);
        while (!q1.try_push(std::move(mp)))
            ;
    }

    running.wait(true);
    const auto stop_time = chrono::high_resolution_clock::now();
    cout << "Duration: " << chrono::duration_cast<chrono::microseconds>(stop_time - start_time).count() << "us" << endl;

    for (auto& c : consumers) {
        get<1>(c)->running_ = false;
    }
    for (auto& c : consumers) {
        get<0>(c).join();
    }
    return 0;
}