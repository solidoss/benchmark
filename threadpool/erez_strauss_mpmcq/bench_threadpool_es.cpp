#include "es/lockfree/pointer_mpmc_queue.h"
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <variant>
#include <vector>
using namespace std;

namespace {

using IndexT                          = uint64_t;
constexpr size_t buffer_capacity      = 0;
atomic<uint64_t> handle_call_count    = 0;
atomic<uint64_t> active_message_count = 0;
atomic<bool>     running              = true;

struct Message {
    IndexT index_{0};
};

using QueueT = es::lockfree::pointer_mpmc_queue<es::lockfree::mpmc_queue, Message>;

struct Context {
    QueueT& rq_;
};

void handle(unique_ptr<Message>& _rv, Context& _rctx)
{
    //++handle_call_count;
    if (--_rv->index_) {
        while (!_rctx.rq_.push(std::move(_rv)))
            ;
    } else if (active_message_count.fetch_sub(1) == 1) {
        running = false;
        running.notify_one();
    }
}
} // namespace

int main(int argc, char* argv[])
{
    QueueT q1{1024 * 16};
    QueueT q2{1024 * 16};

    size_t         repeat_count     = 10000;
    size_t         message_count    = 10000;
    size_t         tp1_thread_count = 4;
    size_t         tp2_thread_count = 4;
    constexpr bool use_unique_ptr   = true;
    Context        ctx1{q2};
    Context        ctx2{q1};

    auto consumer1 = [&]() {
        do {
            unique_ptr<Message> v;
            if (q1.pop(v)) {
                handle(v, ctx1);
            }
        } while (running);
    };
    auto consumer2 = [&]() {
        do {
            unique_ptr<Message> v;
            if (q2.pop(v)) {
                handle(v, ctx2);
            }
        } while (running);
    };
    std::vector<std::thread> consumers;
    consumers.resize(tp1_thread_count + tp2_thread_count);
    for (size_t i = 0; i < tp1_thread_count; ++i)
        consumers[i] = std::thread{consumer1};
    for (size_t i = 0; i < tp2_thread_count; ++i)
        consumers[i + tp1_thread_count] = std::thread{consumer2};

    active_message_count  = message_count;
    const auto start_time = chrono::high_resolution_clock::now();
    for (size_t i = 0; i < message_count; ++i) {
        auto mp = make_unique<Message>(repeat_count);
        while (!q1.push(std::move(mp)))
            ;
    }

    running.wait(true);
    const auto stop_time = chrono::high_resolution_clock::now();
    cout << "Duration: " << chrono::duration_cast<chrono::microseconds>(stop_time - start_time).count() << "us" << endl;

    for (auto& c : consumers)
        c.join();
    return 0;
}