#include "boost/function.hpp"
#include "solid/system/statistic.hpp"
#include "solid/utility/function.hpp"
#include <array>
#include <chrono>
#include <cstddef>
#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <type_traits>
using namespace solid;
using namespace std;

namespace {
enum struct FunctionChoice {
    Std,
    MStd,
    Solid,
    Boost,
};

class TestBase {
public:
    virtual ~TestBase() {}
    virtual void     clear()        = 0;
    virtual void     create(size_t) = 0;
    virtual uint64_t run(size_t)    = 0;
};

struct PrintSize {
    PrintSize(const size_t _sz, const size_t _align)
    {
        cout << "sizeof(closure): " << _sz << " alignof(closure) " << _align << endl;
    }
};

template <class F, size_t CS>
class FunctionTest : public TestBase {
    using DequeT = std::deque<F>;
    DequeT fnc_dq;

public:
    FunctionTest()
    {
        cout << "sizeof(F) = " << sizeof(F) << " sizeof(maxalign_t) = " << sizeof(std::max_align_t) << " alignof(maxalign_t) = " << alignof(std::max_align_t) << endl;
    }

    void clear() override
    {
        fnc_dq.clear();
    }
    template <class Fnc>
    void push(Fnc&& _f)
    {
        static_assert(std::is_trivially_copyable_v<std::remove_cvref_t<Fnc>>);
        static const PrintSize ps(sizeof(Fnc), alignof(Fnc));
        fnc_dq.emplace_back(std::move(_f));
    }

    void create(const size_t _create_count) override
    {
        for (size_t i = 0; i < _create_count; ++i) {
            array<size_t, CS> arr;
            for (size_t j = 0; j < arr.size(); ++j) {
                arr[j] = i;
            }
            push(
                [arr = std::move(arr)](const size_t /*_i*/) {
                    uint64_t r = 0;
                    for (const auto& v : arr) {
                        r += v;
                    }
                    return r;
                });
        }
    }

    uint64_t run(const size_t _i) override
    {
        uint64_t v = 0;
        for (const auto& f : fnc_dq) {
            v += f(_i);
        }
        return v;
    }
};

template <class F>
TestBase* create_test(const size_t _closure_size)
{
    if (_closure_size < 1) {
        return new FunctionTest<F, 0>();
    }
    if (_closure_size < 2) {
        return new FunctionTest<F, 1>();
    }
    if (_closure_size == 3) {
        return new FunctionTest<F, 3>();
    }
    if (_closure_size < 4) {
        return new FunctionTest<F, 2>();
    }
    if (_closure_size < 8) {
        return new FunctionTest<F, 4>();
    }
    if (_closure_size < 16) {
        return new FunctionTest<F, 8>();
    }
    if (_closure_size < 32) {
        return new FunctionTest<F, 16>();
    }
    if (_closure_size < 64) {
        return new FunctionTest<F, 32>();
    }
    if (_closure_size < 128) {
        return new FunctionTest<F, 64>();
    }
    return new FunctionTest<F, 128>();
}

TestBase* create_test(const FunctionChoice _fnc_choice, const size_t _closure_size)
{
    switch (_fnc_choice) {
    case FunctionChoice::Solid:
        return create_test<solid::Function<uint64_t(const size_t)>>(_closure_size);
    case FunctionChoice::Std:
        return create_test<std::function<uint64_t(const size_t)>>(_closure_size);
    case FunctionChoice::Boost:
        return create_test<boost::function<uint64_t(const size_t)>>(_closure_size);
    case FunctionChoice::MStd:
        return create_test<std::move_only_function<uint64_t(const size_t) const>>(_closure_size);
    }
    return nullptr;
}

} // namespace

int main(int argc, char* argv[])
{
    FunctionChoice fnc_choice = FunctionChoice::Std;

    if (argc > 1) {
        switch (argv[1][0]) {
        case 's':
            fnc_choice = FunctionChoice::Solid;
            break;
        case 'S':
            fnc_choice = FunctionChoice::Std;
            break;
        case 'M':
            fnc_choice = FunctionChoice::MStd;
            break;
        case 'B':
            fnc_choice = FunctionChoice::Boost;
            break;
        default:
            cout << "Unknown function choice!" << endl;
            return -1;
        }
    }

    size_t closure_size = 0;
    if (argc > 2) {
        closure_size = atoi(argv[2]);
    }

    size_t create_count = 1000;
    if (argc > 3) {
        create_count = atoi(argv[3]);
    }
    size_t repeat_count = 1000;
    if (argc > 4) {
        repeat_count = atoi(argv[4]);
    }
    cout << "Test " << solid::to_underlying(fnc_choice) << " function with closure_size = " << closure_size << " create_count = " << create_count << " repeat_count = " << repeat_count << endl;

    TestBase* pt             = create_test(fnc_choice, closure_size);
    uint64_t  clear_time_ns  = 0;
    uint64_t  create_time_ns = 0;
    uint64_t  run_time_ns    = 0;

    for (size_t i = 0; i < repeat_count; ++i) {
        auto start_time = std::chrono::high_resolution_clock::now();
        pt->clear();
        {
            auto const     stop_time = std::chrono::high_resolution_clock::now();
            uint64_t const ns        = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_time - start_time).count();
            solid_statistic_add(clear_time_ns, ns);
            start_time = stop_time;
        }
        pt->create(create_count);
        {
            auto const     stop_time = std::chrono::high_resolution_clock::now();
            uint64_t const ns        = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_time - start_time).count();
            solid_statistic_add(create_time_ns, ns);
            start_time = stop_time;
        }
        for (size_t j = 0; j < 10; ++j) {
            pt->run(j);
        }
        {
            auto const     stop_time = std::chrono::high_resolution_clock::now();
            uint64_t const ns        = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_time - start_time).count();
            solid_statistic_add(run_time_ns, ns);
        }
    }
    delete pt;
    cout << "clear_time_ms = " << clear_time_ns / 1000000u << " create_time_ns = " << create_time_ns / 1000000u << " run_time_ns = " << run_time_ns / 1000000u << endl;

    return 0;
}
