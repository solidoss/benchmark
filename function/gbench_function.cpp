#include <benchmark/benchmark.h>

#include "boost/function.hpp"
#include "solid/utility/common.hpp"
#include "solid/utility/function.hpp"
#include <array>
#include <cstddef>
#include <deque>
#include <functional>
#include <memory>
#include <type_traits>

using namespace solid;
using namespace std;

namespace {

template <class F, size_t CS>
class FunctionTest {
    struct alignas(64) Fun {
        F f_;
    };
    using DequeT = std::deque<Fun>;
    DequeT fnc_dq;

public:
    void clear()
    {
        fnc_dq.clear();
    }

    template <class Fnc>
    void push(Fnc&& _f)
    {
        fnc_dq.emplace_back(std::move(_f));
    }

    void create(const size_t _create_count)
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

    uint64_t run(const size_t _i)
    {
        uint64_t v = 0;
        for (const auto& f : fnc_dq) {
            v += f.f_(_i);
        }
        return v;
    }
};

// Template benchmark for Create operation
template <class F, size_t CS>
static void BM_Create(benchmark::State& state)
{
    const size_t        create_count = state.range(0);
    FunctionTest<F, CS> test;

    for (auto _ : state) {
        test.clear();
        test.create(create_count);
        benchmark::DoNotOptimize(test);
    }
    state.SetItemsProcessed(state.iterations() * create_count);
}

// Template benchmark for Run operation
template <class F, size_t CS>
static void BM_Run(benchmark::State& state)
{
    const size_t        create_count = state.range(0);
    FunctionTest<F, CS> test;
    test.create(create_count);

    uint64_t sum = 0;
    size_t   j   = 0;
    for (auto _ : state) {
        sum += test.run(++j);
        benchmark::DoNotOptimize(sum);
    }
    state.SetItemsProcessed(state.iterations() * create_count * 10);
}

// Template benchmark for combined Create+Run operation
template <class F, size_t CS>
static void BM_CreateAndRun(benchmark::State& state)
{
    const size_t        create_count = state.range(0);
    FunctionTest<F, CS> test;

    uint64_t sum = 0;
    for (auto _ : state) {
        test.clear();
        test.create(create_count);
        sum = 0;
        for (size_t j = 0; j < 10; ++j) {
            sum += test.run(j);
        }
        benchmark::DoNotOptimize(sum);
    }
    state.SetItemsProcessed(state.iterations() * create_count);
}

// Type aliases for function types
using StdFunction         = std::function<uint64_t(const size_t)>;
using StdMoveOnlyFunction = std::move_only_function<uint64_t(const size_t) const>;
using SolidFunction       = solid::Function<uint64_t(const size_t)>;
using SolidFunction64     = solid::Function64T<uint64_t(const size_t)>;
using BoostFunction       = boost::function<uint64_t(const size_t)>;

// Closure size 1 (8 bytes)
constexpr size_t CS1 = 1;

// Create benchmarks - closure size 1
BENCHMARK(BM_Create<StdFunction, CS1>)->Name("Create/std::function/CS1")->Arg(1000);
BENCHMARK(BM_Create<StdMoveOnlyFunction, CS1>)->Name("Create/std::move_only_function/CS1")->Arg(1000);
BENCHMARK(BM_Create<SolidFunction, CS1>)->Name("Create/solid::Function/CS1")->Arg(1000);
BENCHMARK(BM_Create<SolidFunction64, CS1>)->Name("Create/solid::Function64T/CS1")->Arg(1000);
BENCHMARK(BM_Create<BoostFunction, CS1>)->Name("Create/boost::function/CS1")->Arg(1000);

// Run benchmarks - closure size 1
BENCHMARK(BM_Run<StdFunction, CS1>)->Name("Run/std::function/CS1")->Arg(1000);
BENCHMARK(BM_Run<StdMoveOnlyFunction, CS1>)->Name("Run/std::move_only_function/CS1")->Arg(1000);
BENCHMARK(BM_Run<SolidFunction, CS1>)->Name("Run/solid::Function/CS1")->Arg(1000);
BENCHMARK(BM_Run<SolidFunction64, CS1>)->Name("Run/solid::Function64T/CS1")->Arg(1000);
BENCHMARK(BM_Run<BoostFunction, CS1>)->Name("Run/boost::function/CS1")->Arg(1000);

// CreateAndRun benchmarks - closure size 1
BENCHMARK(BM_CreateAndRun<StdFunction, CS1>)->Name("CreateAndRun/std::function/CS1")->Arg(1000);
BENCHMARK(BM_CreateAndRun<StdMoveOnlyFunction, CS1>)->Name("CreateAndRun/std::move_only_function/CS1")->Arg(1000);
BENCHMARK(BM_CreateAndRun<SolidFunction, CS1>)->Name("CreateAndRun/solid::Function/CS1")->Arg(1000);
BENCHMARK(BM_CreateAndRun<SolidFunction64, CS1>)->Name("CreateAndRun/solid::Function64T/CS1")->Arg(1000);
BENCHMARK(BM_CreateAndRun<BoostFunction, CS1>)->Name("CreateAndRun/boost::function/CS1")->Arg(1000);

// Closure size 2 (16 bytes)
constexpr size_t CS2 = 2;

// Create benchmarks - closure size 2
BENCHMARK(BM_Create<StdFunction, CS2>)->Name("Create/std::function/CS2")->Arg(1000);
BENCHMARK(BM_Create<StdMoveOnlyFunction, CS2>)->Name("Create/std::move_only_function/CS2")->Arg(1000);
BENCHMARK(BM_Create<SolidFunction, CS2>)->Name("Create/solid::Function/CS2")->Arg(1000);
BENCHMARK(BM_Create<SolidFunction64, CS2>)->Name("Create/solid::Function64T/CS2")->Arg(1000);
BENCHMARK(BM_Create<BoostFunction, CS2>)->Name("Create/boost::function/CS2")->Arg(1000);

// Run benchmarks - closure size 2
BENCHMARK(BM_Run<StdFunction, CS2>)->Name("Run/std::function/CS2")->Arg(1000);
BENCHMARK(BM_Run<StdMoveOnlyFunction, CS2>)->Name("Run/std::move_only_function/CS2")->Arg(1000);
BENCHMARK(BM_Run<SolidFunction, CS2>)->Name("Run/solid::Function/CS2")->Arg(1000);
BENCHMARK(BM_Run<SolidFunction64, CS2>)->Name("Run/solid::Function64T/CS2")->Arg(1000);
BENCHMARK(BM_Run<BoostFunction, CS2>)->Name("Run/boost::function/CS2")->Arg(1000);

// CreateAndRun benchmarks - closure size 2
BENCHMARK(BM_CreateAndRun<StdFunction, CS2>)->Name("CreateAndRun/std::function/CS2")->Arg(1000);
BENCHMARK(BM_CreateAndRun<StdMoveOnlyFunction, CS2>)->Name("CreateAndRun/std::move_only_function/CS2")->Arg(1000);
BENCHMARK(BM_CreateAndRun<SolidFunction, CS2>)->Name("CreateAndRun/solid::Function/CS2")->Arg(1000);
BENCHMARK(BM_CreateAndRun<SolidFunction64, CS2>)->Name("CreateAndRun/solid::Function64T/CS2")->Arg(1000);
BENCHMARK(BM_CreateAndRun<BoostFunction, CS2>)->Name("CreateAndRun/boost::function/CS2")->Arg(1000);

// Closure size 3 (24 bytes)
constexpr size_t CS3 = 3;

// Create benchmarks - closure size 3
BENCHMARK(BM_Create<StdFunction, CS3>)->Name("Create/std::function/CS3")->Arg(1000);
BENCHMARK(BM_Create<StdMoveOnlyFunction, CS3>)->Name("Create/std::move_only_function/CS3")->Arg(1000);
BENCHMARK(BM_Create<SolidFunction, CS3>)->Name("Create/solid::Function/CS3")->Arg(1000);
BENCHMARK(BM_Create<SolidFunction64, CS3>)->Name("Create/solid::Function64T/CS3")->Arg(1000);
BENCHMARK(BM_Create<BoostFunction, CS3>)->Name("Create/boost::function/CS3")->Arg(1000);

// Run benchmarks - closure size 3
BENCHMARK(BM_Run<StdFunction, CS3>)->Name("Run/std::function/CS3")->Arg(1000);
BENCHMARK(BM_Run<StdMoveOnlyFunction, CS3>)->Name("Run/std::move_only_function/CS3")->Arg(1000);
BENCHMARK(BM_Run<SolidFunction, CS3>)->Name("Run/solid::Function/CS3")->Arg(1000);
BENCHMARK(BM_Run<SolidFunction64, CS3>)->Name("Run/solid::Function64T/CS3")->Arg(1000);
BENCHMARK(BM_Run<BoostFunction, CS3>)->Name("Run/boost::function/CS3")->Arg(1000);

// CreateAndRun benchmarks - closure size 3
BENCHMARK(BM_CreateAndRun<StdFunction, CS3>)->Name("CreateAndRun/std::function/CS3")->Arg(1000);
BENCHMARK(BM_CreateAndRun<StdMoveOnlyFunction, CS3>)->Name("CreateAndRun/std::move_only_function/CS3")->Arg(1000);
BENCHMARK(BM_CreateAndRun<SolidFunction, CS3>)->Name("CreateAndRun/solid::Function/CS3")->Arg(1000);
BENCHMARK(BM_CreateAndRun<SolidFunction64, CS3>)->Name("CreateAndRun/solid::Function64T/CS3")->Arg(1000);
BENCHMARK(BM_CreateAndRun<BoostFunction, CS3>)->Name("CreateAndRun/boost::function/CS3")->Arg(1000);

// Closure size 4 (32 bytes)
constexpr size_t CS4 = 4;

// Create benchmarks - closure size 4
BENCHMARK(BM_Create<StdFunction, CS4>)->Name("Create/std::function/CS4")->Arg(1000);
BENCHMARK(BM_Create<StdMoveOnlyFunction, CS4>)->Name("Create/std::move_only_function/CS4")->Arg(1000);
BENCHMARK(BM_Create<SolidFunction, CS4>)->Name("Create/solid::Function/CS4")->Arg(1000);
BENCHMARK(BM_Create<SolidFunction64, CS4>)->Name("Create/solid::Function64T/CS4")->Arg(1000);
BENCHMARK(BM_Create<BoostFunction, CS4>)->Name("Create/boost::function/CS4")->Arg(1000);

// Run benchmarks - closure size 4
BENCHMARK(BM_Run<StdFunction, CS4>)->Name("Run/std::function/CS4")->Arg(1000);
BENCHMARK(BM_Run<StdMoveOnlyFunction, CS4>)->Name("Run/std::move_only_function/CS4")->Arg(1000);
BENCHMARK(BM_Run<SolidFunction, CS4>)->Name("Run/solid::Function/CS4")->Arg(1000);
BENCHMARK(BM_Run<SolidFunction64, CS4>)->Name("Run/solid::Function64T/CS4")->Arg(1000);
BENCHMARK(BM_Run<BoostFunction, CS4>)->Name("Run/boost::function/CS4")->Arg(1000);

// CreateAndRun benchmarks - closure size 4
BENCHMARK(BM_CreateAndRun<StdFunction, CS4>)->Name("CreateAndRun/std::function/CS4")->Arg(1000);
BENCHMARK(BM_CreateAndRun<StdMoveOnlyFunction, CS4>)->Name("CreateAndRun/std::move_only_function/CS4")->Arg(1000);
BENCHMARK(BM_CreateAndRun<SolidFunction, CS4>)->Name("CreateAndRun/solid::Function/CS4")->Arg(1000);
BENCHMARK(BM_CreateAndRun<SolidFunction64, CS4>)->Name("CreateAndRun/solid::Function64T/CS4")->Arg(1000);
BENCHMARK(BM_CreateAndRun<BoostFunction, CS4>)->Name("CreateAndRun/boost::function/CS4")->Arg(1000);

// Closure size 5 (40 bytes)
constexpr size_t CS5 = 5;

// Create benchmarks - closure size 5
BENCHMARK(BM_Create<StdFunction, CS5>)->Name("Create/std::function/CS5")->Arg(1000);
BENCHMARK(BM_Create<StdMoveOnlyFunction, CS5>)->Name("Create/std::move_only_function/CS5")->Arg(1000);
BENCHMARK(BM_Create<SolidFunction, CS5>)->Name("Create/solid::Function/CS5")->Arg(1000);
BENCHMARK(BM_Create<SolidFunction64, CS5>)->Name("Create/solid::Function64T/CS5")->Arg(1000);
BENCHMARK(BM_Create<BoostFunction, CS5>)->Name("Create/boost::function/CS5")->Arg(1000);

// Run benchmarks - closure size 5
BENCHMARK(BM_Run<StdFunction, CS5>)->Name("Run/std::function/CS5")->Arg(1000);
BENCHMARK(BM_Run<StdMoveOnlyFunction, CS5>)->Name("Run/std::move_only_function/CS5")->Arg(1000);
BENCHMARK(BM_Run<SolidFunction, CS5>)->Name("Run/solid::Function/CS5")->Arg(1000);
BENCHMARK(BM_Run<SolidFunction64, CS5>)->Name("Run/solid::Function64T/CS5")->Arg(1000);
BENCHMARK(BM_Run<BoostFunction, CS5>)->Name("Run/boost::function/CS5")->Arg(1000);

// CreateAndRun benchmarks - closure size 5
BENCHMARK(BM_CreateAndRun<StdFunction, CS5>)->Name("CreateAndRun/std::function/CS5")->Arg(1000);
BENCHMARK(BM_CreateAndRun<StdMoveOnlyFunction, CS5>)->Name("CreateAndRun/std::move_only_function/CS5")->Arg(1000);
BENCHMARK(BM_CreateAndRun<SolidFunction, CS5>)->Name("CreateAndRun/solid::Function/CS5")->Arg(1000);
BENCHMARK(BM_CreateAndRun<SolidFunction64, CS5>)->Name("CreateAndRun/solid::Function64T/CS5")->Arg(1000);
BENCHMARK(BM_CreateAndRun<BoostFunction, CS5>)->Name("CreateAndRun/boost::function/CS5")->Arg(1000);

// Closure size 6 (48 bytes)
constexpr size_t CS6 = 6;

// Create benchmarks - closure size 6
BENCHMARK(BM_Create<StdFunction, CS6>)->Name("Create/std::function/CS6")->Arg(1000);
BENCHMARK(BM_Create<StdMoveOnlyFunction, CS6>)->Name("Create/std::move_only_function/CS6")->Arg(1000);
BENCHMARK(BM_Create<SolidFunction, CS6>)->Name("Create/solid::Function/CS6")->Arg(1000);
BENCHMARK(BM_Create<SolidFunction64, CS6>)->Name("Create/solid::Function64T/CS6")->Arg(1000);
BENCHMARK(BM_Create<BoostFunction, CS6>)->Name("Create/boost::function/CS6")->Arg(1000);

// Run benchmarks - closure size 6
BENCHMARK(BM_Run<StdFunction, CS6>)->Name("Run/std::function/CS6")->Arg(1000);
BENCHMARK(BM_Run<StdMoveOnlyFunction, CS6>)->Name("Run/std::move_only_function/CS6")->Arg(1000);
BENCHMARK(BM_Run<SolidFunction, CS6>)->Name("Run/solid::Function/CS6")->Arg(1000);
BENCHMARK(BM_Run<SolidFunction64, CS6>)->Name("Run/solid::Function64T/CS6")->Arg(1000);
BENCHMARK(BM_Run<BoostFunction, CS6>)->Name("Run/boost::function/CS6")->Arg(1000);

// CreateAndRun benchmarks - closure size 6
BENCHMARK(BM_CreateAndRun<StdFunction, CS6>)->Name("CreateAndRun/std::function/CS6")->Arg(1000);
BENCHMARK(BM_CreateAndRun<StdMoveOnlyFunction, CS6>)->Name("CreateAndRun/std::move_only_function/CS6")->Arg(1000);
BENCHMARK(BM_CreateAndRun<SolidFunction, CS6>)->Name("CreateAndRun/solid::Function/CS6")->Arg(1000);
BENCHMARK(BM_CreateAndRun<SolidFunction64, CS6>)->Name("CreateAndRun/solid::Function64T/CS6")->Arg(1000);
BENCHMARK(BM_CreateAndRun<BoostFunction, CS6>)->Name("CreateAndRun/boost::function/CS6")->Arg(1000);

// Closure size 7 (56 bytes)
constexpr size_t CS7 = 7;

// Create benchmarks - closure size 7
BENCHMARK(BM_Create<StdFunction, CS7>)->Name("Create/std::function/CS7")->Arg(1000);
BENCHMARK(BM_Create<StdMoveOnlyFunction, CS7>)->Name("Create/std::move_only_function/CS7")->Arg(1000);
BENCHMARK(BM_Create<SolidFunction, CS7>)->Name("Create/solid::Function/CS7")->Arg(1000);
BENCHMARK(BM_Create<SolidFunction64, CS7>)->Name("Create/solid::Function64T/CS7")->Arg(1000);
BENCHMARK(BM_Create<BoostFunction, CS7>)->Name("Create/boost::function/CS7")->Arg(1000);

// Run benchmarks - closure size 7
BENCHMARK(BM_Run<StdFunction, CS7>)->Name("Run/std::function/CS7")->Arg(1000);
BENCHMARK(BM_Run<StdMoveOnlyFunction, CS7>)->Name("Run/std::move_only_function/CS7")->Arg(1000);
BENCHMARK(BM_Run<SolidFunction, CS7>)->Name("Run/solid::Function/CS7")->Arg(1000);
BENCHMARK(BM_Run<SolidFunction64, CS7>)->Name("Run/solid::Function64T/CS7")->Arg(1000);
BENCHMARK(BM_Run<BoostFunction, CS7>)->Name("Run/boost::function/CS7")->Arg(1000);

// CreateAndRun benchmarks - closure size 7
BENCHMARK(BM_CreateAndRun<StdFunction, CS7>)->Name("CreateAndRun/std::function/CS7")->Arg(1000);
BENCHMARK(BM_CreateAndRun<StdMoveOnlyFunction, CS7>)->Name("CreateAndRun/std::move_only_function/CS7")->Arg(1000);
BENCHMARK(BM_CreateAndRun<SolidFunction, CS7>)->Name("CreateAndRun/solid::Function/CS7")->Arg(1000);
BENCHMARK(BM_CreateAndRun<SolidFunction64, CS7>)->Name("CreateAndRun/solid::Function64T/CS7")->Arg(1000);
BENCHMARK(BM_CreateAndRun<BoostFunction, CS7>)->Name("CreateAndRun/boost::function/CS7")->Arg(1000);

// Closure size 8 (64 bytes)
constexpr size_t CS8 = 8;

// Create benchmarks - closure size 8
BENCHMARK(BM_Create<StdFunction, CS8>)->Name("Create/std::function/CS8")->Arg(1000);
BENCHMARK(BM_Create<StdMoveOnlyFunction, CS8>)->Name("Create/std::move_only_function/CS8")->Arg(1000);
BENCHMARK(BM_Create<SolidFunction, CS8>)->Name("Create/solid::Function/CS8")->Arg(1000);
BENCHMARK(BM_Create<SolidFunction64, CS8>)->Name("Create/solid::Function64T/CS8")->Arg(1000);
BENCHMARK(BM_Create<BoostFunction, CS8>)->Name("Create/boost::function/CS8")->Arg(1000);

// Run benchmarks - closure size 8
BENCHMARK(BM_Run<StdFunction, CS8>)->Name("Run/std::function/CS8")->Arg(1000);
BENCHMARK(BM_Run<StdMoveOnlyFunction, CS8>)->Name("Run/std::move_only_function/CS8")->Arg(1000);
BENCHMARK(BM_Run<SolidFunction, CS8>)->Name("Run/solid::Function/CS8")->Arg(1000);
BENCHMARK(BM_Run<SolidFunction64, CS8>)->Name("Run/solid::Function64T/CS8")->Arg(1000);
BENCHMARK(BM_Run<BoostFunction, CS8>)->Name("Run/boost::function/CS8")->Arg(1000);

// CreateAndRun benchmarks - closure size 8
BENCHMARK(BM_CreateAndRun<StdFunction, CS8>)->Name("CreateAndRun/std::function/CS8")->Arg(1000);
BENCHMARK(BM_CreateAndRun<StdMoveOnlyFunction, CS8>)->Name("CreateAndRun/std::move_only_function/CS8")->Arg(1000);
BENCHMARK(BM_CreateAndRun<SolidFunction, CS8>)->Name("CreateAndRun/solid::Function/CS8")->Arg(1000);
BENCHMARK(BM_CreateAndRun<SolidFunction64, CS8>)->Name("CreateAndRun/solid::Function64T/CS8")->Arg(1000);
BENCHMARK(BM_CreateAndRun<BoostFunction, CS8>)->Name("CreateAndRun/boost::function/CS8")->Arg(1000);

// Closure size 16 (128 bytes)
constexpr size_t CS16 = 16;

// Create benchmarks - closure size 16
BENCHMARK(BM_Create<StdFunction, CS16>)->Name("Create/std::function/CS16")->Arg(1000);
BENCHMARK(BM_Create<StdMoveOnlyFunction, CS16>)->Name("Create/std::move_only_function/CS16")->Arg(1000);
BENCHMARK(BM_Create<SolidFunction, CS16>)->Name("Create/solid::Function/CS16")->Arg(1000);
BENCHMARK(BM_Create<SolidFunction64, CS16>)->Name("Create/solid::Function64T/CS16")->Arg(1000);
BENCHMARK(BM_Create<BoostFunction, CS16>)->Name("Create/boost::function/CS16")->Arg(1000);

// Run benchmarks - closure size 16
BENCHMARK(BM_Run<StdFunction, CS16>)->Name("Run/std::function/CS16")->Arg(1000);
BENCHMARK(BM_Run<StdMoveOnlyFunction, CS16>)->Name("Run/std::move_only_function/CS16")->Arg(1000);
BENCHMARK(BM_Run<SolidFunction, CS16>)->Name("Run/solid::Function/CS16")->Arg(1000);
BENCHMARK(BM_Run<SolidFunction64, CS16>)->Name("Run/solid::Function64T/CS16")->Arg(1000);
BENCHMARK(BM_Run<BoostFunction, CS16>)->Name("Run/boost::function/CS16")->Arg(1000);

// CreateAndRun benchmarks - closure size 16
BENCHMARK(BM_CreateAndRun<StdFunction, CS16>)->Name("CreateAndRun/std::function/CS16")->Arg(1000);
BENCHMARK(BM_CreateAndRun<StdMoveOnlyFunction, CS16>)->Name("CreateAndRun/std::move_only_function/CS16")->Arg(1000);
BENCHMARK(BM_CreateAndRun<SolidFunction, CS16>)->Name("CreateAndRun/solid::Function/CS16")->Arg(1000);
BENCHMARK(BM_CreateAndRun<SolidFunction64, CS16>)->Name("CreateAndRun/solid::Function64T/CS16")->Arg(1000);
BENCHMARK(BM_CreateAndRun<BoostFunction, CS16>)->Name("CreateAndRun/boost::function/CS16")->Arg(1000);

} // namespace

BENCHMARK_MAIN();
