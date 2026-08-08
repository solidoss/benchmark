#include "fpikus/ring-atomic-queue.h"
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
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

using VariantT = std::variant<Message, unique_ptr<Message>>;

// RingAtomicMapQueueMPMC is a key-value (map) queue: every element carries a
// Key that must be a non-zero, lock-free atomic value (Key{} == 0 is the
// reserved empty-slot marker). We do not use the key for routing here, so we
// tag every element with a constant non-zero sentinel and carry the payload in
// the value.
constexpr uint64_t message_key = 1;
using QueueT                   = RingAtomicMapQueueMPMC<uint64_t, VariantT>;

struct Context {
    QueueT& rq_;
};

void handle(Message& _rv, Context& _rctx)
{
    //++handle_call_count;
    if (--_rv.index_) {
        while (!_rctx.rq_.push(message_key, std::move(_rv)))
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
        while (!_rctx.rq_.push(message_key, std::move(_rv)))
            ;
    } else if (active_message_count.fetch_sub(1) == 1) {
        running = false;
        running.notify_one();
    }
}

struct Notificator {
    alignas(hardware_constructive_interference_size) bool running_{true};
};

// Backing storage for a queue: the RingAtomicMapQueueMPMC does not own its
// memory, it is handed a span of raw bytes to use as the ring buffer.
struct QueueStorage {
    void*            memory_{nullptr};
    std::align_val_t align_{};

    explicit QueueStorage(size_t element_count, size_t element_size, size_t element_align)
        : align_{element_align}
    {
        const size_t bytes = element_count * element_size;
        memory_            = ::operator new(bytes, align_);
    }
    ~QueueStorage()
    {
        ::operator delete(memory_, align_);
    }
};

} // namespace

int main(int argc, char* argv[])
{
    size_t         repeat_count     = 10000;
    size_t         message_count    = 10000;
    size_t         tp1_thread_count = 4;
    size_t         tp2_thread_count = 4;
    constexpr bool use_unique_ptr   = true;

    constexpr size_t queue_capacity = 1024 * 16;

    QueueStorage store1{queue_capacity, QueueT::element_size(), QueueT::element_align()};
    QueueStorage store2{queue_capacity, QueueT::element_size(), QueueT::element_align()};

    QueueT  q1{store1.memory_, queue_capacity * QueueT::element_size()};
    QueueT  q2{store2.memory_, queue_capacity * QueueT::element_size()};
    Context ctx1{q2};
    Context ctx2{q1};

    auto consumer = [](QueueT& _rq, Context& _rctx, Notificator* _pn) {
        do {
            VariantT v;
            while (_rq.pop(v) != 0) {
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
        while (!q1.push(message_key, std::move(mp)))
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
