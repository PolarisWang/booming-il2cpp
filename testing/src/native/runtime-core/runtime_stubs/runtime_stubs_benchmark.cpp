// runtime_stubs_benchmark.cpp — Runtime Stubs microbenchmark suite
//
// Measures throughput (ops/sec) for key runtime stub entry points.
// Build pattern: compile stubs .cpp directly alongside benchmark (GNU ar compat).

#include <gtest/gtest.h>
#include <chaos/native_types.h>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <cstdint>

// ── AOT symbol stubs ──────────────────────────────────────────────────────
extern "C" const void* chaos_il2cpp_aot_hotpatch_module = nullptr;
extern "C" const char* const kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int32_t kChaosExternalRuntimeCount = 0;

using Clock = std::chrono::high_resolution_clock;

// ── Extern functions under test ───────────────────────────────────────────
extern "C" {
CHAOS_IL2CPP_INTPTR ChaosStringFastAllocate(CHAOS_IL2CPP_INTPTR length);
CHAOS_IL2CPP_INT32  ChaosStringCompare(CHAOS_IL2CPP_INTPTR str_a, CHAOS_IL2CPP_INTPTR str_b);
CHAOS_IL2CPP_INT32  ChaosStringIndexOf(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR value);
CHAOS_IL2CPP_INT64  ChaosDatetimeGetUtcNow(void);
CHAOS_IL2CPP_INTPTR ChaosDateTimeToString(CHAOS_IL2CPP_INT64 dt);
CHAOS_IL2CPP_INT64  ChaosMathSqrt(CHAOS_IL2CPP_INT64 value);
CHAOS_IL2CPP_INT32  ChaosMathAbsInt32(CHAOS_IL2CPP_INT32 value);
CHAOS_IL2CPP_FLOAT64 ChaosMathAbsDouble(CHAOS_IL2CPP_FLOAT64 value);
CHAOS_IL2CPP_FLOAT64 ChaosMathFloor(CHAOS_IL2CPP_FLOAT64 value);
CHAOS_IL2CPP_FLOAT64 ChaosMathCeiling(CHAOS_IL2CPP_FLOAT64 value);
CHAOS_IL2CPP_INTPTR ChaosGuidNewGuid(void);
CHAOS_IL2CPP_INTPTR ChaosGuidToString(CHAOS_IL2CPP_INTPTR guid);
CHAOS_IL2CPP_INT32  ChaosGuidGetHashCode(CHAOS_IL2CPP_INTPTR guid);
CHAOS_IL2CPP_INT64  ChaosTimeSpanFromDays(CHAOS_IL2CPP_FLOAT64 value);
CHAOS_IL2CPP_INT64  ChaosTimeSpanFromHours(CHAOS_IL2CPP_FLOAT64 value);
CHAOS_IL2CPP_INT64  ChaosTimeSpanFromMinutes(CHAOS_IL2CPP_FLOAT64 value);
CHAOS_IL2CPP_INT64  ChaosDateTimeAddDays(CHAOS_IL2CPP_INT64 dt, CHAOS_IL2CPP_FLOAT64 value);
CHAOS_IL2CPP_INT64  ChaosDateTimeAddHours(CHAOS_IL2CPP_INT64 dt, CHAOS_IL2CPP_FLOAT64 value);
CHAOS_IL2CPP_INT32  ChaosDateTimeCompare(CHAOS_IL2CPP_INT64 left, CHAOS_IL2CPP_INT64 right);
CHAOS_IL2CPP_INT32  ChaosDateTimeDaysInMonth(CHAOS_IL2CPP_INT32 year, CHAOS_IL2CPP_INT32 month);
}

// ── Benchmark helpers ─────────────────────────────────────────────────────

struct BenchResult {
    const char* name;
    double ns_per_op;
    double ops_per_sec;
    int iterations;
};

static void print_result(const BenchResult& r) {
    std::printf("  %-42s %8.1f ns/op  %8.2f M ops/sec  (%d iters)\n",
                r.name, r.ns_per_op, r.ops_per_sec / 1.0e6, r.iterations);
}

template<typename Fn>
static BenchResult run_bench(const char* name, int warmup, int iterations, Fn fn) {
    for (int i = 0; i < warmup; ++i) fn(i);
    auto start = Clock::now();
    for (int i = 0; i < iterations; ++i) fn(i);
    auto end = Clock::now();
    double total_ns = std::chrono::duration<double, std::nano>(end - start).count();
    double ns_per_op = total_ns / iterations;
    double ops_per_sec = 1.0e9 / ns_per_op;
    return {name, ns_per_op, ops_per_sec, iterations};
}

// ── Test cases ────────────────────────────────────────────────────────────

constexpr int kWarmup = 1000;
constexpr int kIter = 50000;
constexpr int kSmallIter = 5000;

TEST(RuntimeStubsBenchmark, StringFastAllocate) {
    std::printf("\n--- String Operations ---\n");

    auto r1 = run_bench("FastAllocate(len=8)", kWarmup, kIter,
        [](int) { ChaosStringFastAllocate(8); });
    print_result(r1);

    auto r2 = run_bench("FastAllocate(len=256)", kWarmup, kIter,
        [](int) { ChaosStringFastAllocate(256); });
    print_result(r2);
}

TEST(RuntimeStubsBenchmark, StringCompare) {
    std::printf("\n--- String Compare ---\n");

    // Compare with two null pointers (fast path)
    auto r1 = run_bench("Compare(null,null)", kWarmup, kIter,
        [](int) { ChaosStringCompare(0, 0); });
    print_result(r1);
}

TEST(RuntimeStubsBenchmark, DateTimeUtcNow) {
    std::printf("\n--- DateTime ---\n");

    auto r1 = run_bench("GetUtcNow", kWarmup, kIter,
        [](int) { ChaosDatetimeGetUtcNow(); });
    print_result(r1);

    auto r2 = run_bench("ToString(now)", kWarmup, kSmallIter,
        [](int) {
            auto ticks = ChaosDatetimeGetUtcNow();
            ChaosDateTimeToString(ticks);
        });
    print_result(r2);

    auto r3 = run_bench("Compare(equal)", kWarmup, kIter,
        [](int) { ChaosDateTimeCompare(1000000, 1000000); });
    print_result(r3);

    auto r4 = run_bench("DaysInMonth(2024,2)", kWarmup, kIter,
        [](int) { ChaosDateTimeDaysInMonth(2024, 2); });
    print_result(r4);
}

TEST(RuntimeStubsBenchmark, MathOperations) {
    std::printf("\n--- Math ---\n");

    double test_val = 42.5;
    CHAOS_IL2CPP_INT64 test_bits;
    std::memcpy(&test_bits, &test_val, sizeof(test_bits));

    auto r1 = run_bench("Sqrt(42.5)", kWarmup, kIter,
        [test_bits](int) { ChaosMathSqrt(test_bits); });
    print_result(r1);

    auto r2 = run_bench("AbsInt32", kWarmup, kIter,
        [](int i) { ChaosMathAbsInt32(i); });
    print_result(r2);

    auto r3 = run_bench("AbsDouble(42.5)", kWarmup, kIter,
        [](int) { ChaosMathAbsDouble(42.5); });
    print_result(r3);

    auto r4 = run_bench("Floor(42.5)", kWarmup, kIter,
        [](int) { ChaosMathFloor(42.5); });
    print_result(r4);

    auto r5 = run_bench("Ceiling(42.5)", kWarmup, kIter,
        [](int) { ChaosMathCeiling(42.5); });
    print_result(r5);
}

TEST(RuntimeStubsBenchmark, TimeSpanArithmetic) {
    std::printf("\n--- TimeSpan ---\n");

    auto r1 = run_bench("FromDays(1.5)", kWarmup, kIter,
        [](int) { ChaosTimeSpanFromDays(1.5); });
    print_result(r1);

    auto r2 = run_bench("FromHours(2.5)", kWarmup, kIter,
        [](int) { ChaosTimeSpanFromHours(2.5); });
    print_result(r2);

    auto r3 = run_bench("FromMinutes(30.0)", kWarmup, kIter,
        [](int) { ChaosTimeSpanFromMinutes(30.0); });
    print_result(r3);
}

TEST(RuntimeStubsBenchmark, DateTimeAdd) {
    std::printf("\n--- DateTime Add ---\n");

    auto r1 = run_bench("AddDays(now,1.0)", kWarmup, kIter,
        [](int) { ChaosDateTimeAddDays(1000000, 1.0); });
    print_result(r1);

    auto r2 = run_bench("AddHours(now,2.5)", kWarmup, kIter,
        [](int) { ChaosDateTimeAddHours(1000000, 2.5); });
    print_result(r2);
}

TEST(RuntimeStubsBenchmark, GuidOperations) {
    std::printf("\n--- Guid ---\n");

    auto r1 = run_bench("NewGuid", kWarmup, kSmallIter,
        [](int) { ChaosGuidNewGuid(); });
    print_result(r1);

    // Measure GetHashCode on a zeroed-out Guid
    CHAOS_IL2CPP_INT64 guid_buf[4] = {};
    auto r2 = run_bench("GetHashCode(zero)", kWarmup, kIter,
        [guid_buf](int) { ChaosGuidGetHashCode(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&guid_buf)); });
    print_result(r2);
}

TEST(RuntimeStubsBenchmark, LoopOverhead) {
    std::printf("\n--- Loop Overhead ---\n");
    auto r = run_bench("empty loop (calibration)", kWarmup * 10, kIter * 10,
        [](int) { /* no-op */ });
    print_result(r);
    EXPECT_LT(r.ns_per_op, 5.0);
}
