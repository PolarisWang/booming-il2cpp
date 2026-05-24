// ── Hybrid mode performance benchmark ──────────────────────────────
//
// Measures:
//   1. Upgrade latency:  Time from first call to JIT compilation (P50/P99)
//   2. Dispatch overhead: AOT dispatch path (counter decrement) cost
//   3. Trampoline alloc:  PrecodeArena trampoline allocation latency
//   4. Multi-method:      Batch method upgrade throughput
//
// Results are printed as CSV for baseline tracking.

#include <gtest/gtest.h>

#include "jit_precode.h"
#include "jit_engine.h"
#include "jit_method.h"

#include <codegen_bridge.h>

#include <cstdint>
#include <cstdio>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>

// ── Namespace aliases ──────────────────────────────────────────────
using chaos::il2cpp::interpreter::IROpCode;
using chaos::il2cpp::interpreter::RegisterInstruction;
using chaos::il2cpp::interpreter::RegisterMethod;
using chaos::il2cpp::interpreter::kRegHasDst;
using chaos::il2cpp::interpreter::kRegHasSrc1;
using chaos::il2cpp::interpreter::kRegHasImm;
using chaos::il2cpp::jit::Compile;
using chaos::il2cpp::jit::JitMethod;
using chaos::il2cpp::jit::CompileConfig;
using chaos::il2cpp::jit::HybridPrecode;
using chaos::il2cpp::jit::PrecodeArena;
using chaos::il2cpp::jit::HybridStubDispatchImpl;
using chaos::il2cpp::jit::kPrecodeCompiled;
using chaos::il2cpp::jit::kJitUpgradeThreshold;

// ── Helpers ────────────────────────────────────────────────────────

static uint64_t MakeHeader(IROpCode opc, uint8_t dst, uint8_t src1,
                           uint8_t src2, uint8_t flags) noexcept {
    return static_cast<uint64_t>(opc) |
           (static_cast<uint64_t>(dst)   << 16) |
           (static_cast<uint64_t>(src1)  << 24) |
           (static_cast<uint64_t>(src2)  << 32) |
           (static_cast<uint64_t>(flags) << 40);
}

static RegisterInstruction InstrI4(IROpCode opc, int32_t imm,
                                   uint8_t dst = 0, uint8_t flags = 0) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(opc, dst, 0, 0, flags | kRegHasDst | kRegHasImm);
    ri.imm.i4 = imm;
    return ri;
}

static RegisterInstruction InstrRet(uint8_t src) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::Ret, 0, src, 0, kRegHasSrc1);
    ri.imm.i4 = 0;
    return ri;
}

static uint64_t ExecuteNative(void* entry) {
    uint64_t args_buf[8] = {};
    uint64_t ret_buf[2] = {};
    using NativeEntry = void (*)(void*, void*);
    auto native_entry = reinterpret_cast<NativeEntry>(entry);
    native_entry(args_buf, ret_buf);
    return ret_buf[0];
}

static void AotReturn42(void* /*args_buf*/, void* ret_buf) noexcept {
    auto* ret = static_cast<uint64_t*>(ret_buf);
    ret[0] = 42ULL;
}

static RegisterMethod MakeReturnConstantMethod(int32_t value) {
    RegisterMethod method;
    method.max_regs = 1;
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, value, 0, 0));
    method.instructions.push_back(InstrRet(0));
    return method;
}

// Timing helper (microseconds)
template <typename F>
static double MeasureUs(F&& f) {
    auto start = std::chrono::high_resolution_clock::now();
    f();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::micro>(end - start).count();
}

// ── AOT helper functions (file scope) ─────────────────────────────

static void AotReturn0(void* /*args_buf*/, void* ret_buf) noexcept {
    auto* ret = static_cast<uint64_t*>(ret_buf);
    ret[0] = 0ULL;
}
static void AotReturn255(void* /*args_buf*/, void* ret_buf) noexcept {
    auto* ret = static_cast<uint64_t*>(ret_buf);
    ret[0] = 255ULL;
}
static void AotReturn1000(void* /*args_buf*/, void* ret_buf) noexcept {
    auto* ret = static_cast<uint64_t*>(ret_buf);
    ret[0] = 1000ULL;
}

// ── Benchmark fixture ─────────────────────────────────────────────
struct HybridBenchTest : public ::testing::Test {
    static void PrintCsvLine(const char* benchmark, double p50, double p99,
                             double min, double max, double avg, int samples) {
        std::printf("BENCH,%s,P50=%.1f,P99=%.1f,MIN=%.1f,MAX=%.1f,AVG=%.1f,N=%d\n",
                    benchmark, p50, p99, min, max, avg, samples);
        std::fflush(stdout);
    }
};

// ── Upgrade latency benchmarks ────────────────────────────────────

TEST_F(HybridBenchTest, UpgradeLatency_P50_P99) {
    // Measure the time from first AOT call through to JIT compilation.
    // This is the complete upgrade latency including counter decrement
    // overhead + Compile() time.
    PrecodeArena arena;

    constexpr int kSamples = 50;
    std::vector<double> latencies;
    latencies.reserve(kSamples);

    for (int s = 0; s < kSamples; s++) {
        HotpatchEntryV0 entry{};
        HybridPrecode precode;
        precode.call_counter.store(kJitUpgradeThreshold, std::memory_order_relaxed);
        precode.ir = MakeReturnConstantMethod(77);
        precode.config = CompileConfig{};
        precode.aot_entry = reinterpret_cast<void*>(&AotReturn42);
        precode.entry = &entry;
        precode.trampoline = arena.AllocateHybridTrampoline(&precode);
        entry.direct_ptr = precode.trampoline;

        // Time the upgrade sequence: all AOT calls + trigger JIT
        double elapsed = MeasureUs([&]() {
            // kJitUpgradeThreshold AOT calls (all return aot_entry)
            for (uint32_t i = 0; i < kJitUpgradeThreshold; i++) {
                HybridStubDispatchImpl(&precode);
            }
            // Final call triggers JIT
            HybridStubDispatchImpl(&precode);
        });

        latencies.push_back(elapsed);
    }

    std::sort(latencies.begin(), latencies.end());
    double p50 = latencies[kSamples * 50 / 100];
    double p99 = latencies[kSamples * 99 / 100];
    double min = latencies.front();
    double max = latencies.back();
    double avg = 0.0;
    for (auto v : latencies) avg += v;
    avg /= kSamples;

    PrintCsvLine("upgrade-latency", p50, p99, min, max, avg, kSamples);
    EXPECT_LT(p50, 5000.0) << "P50 upgrade latency should be <5ms";
    EXPECT_LT(p99, 20000.0) << "P99 upgrade latency should be <20ms";
}

TEST_F(HybridBenchTest, CompileTimeOnly) {
    // Measure the pure Compile() time (without dispatch overhead).
    // This represents the lower bound of upgrade latency.
    constexpr int kSamples = 30;
    std::vector<double> times;
    times.reserve(kSamples);

    auto rm = MakeReturnConstantMethod(77);
    CompileConfig cfg{};

    for (int s = 0; s < kSamples; s++) {
        double elapsed = MeasureUs([&]() {
            auto* jit = Compile(rm, cfg);
            // Prevent dead code elimination
            if (jit != nullptr) {
                delete jit;
            }
        });
        times.push_back(elapsed);
    }

    std::sort(times.begin(), times.end());
    double p50 = times[kSamples * 50 / 100];
    double p99 = times[kSamples * 99 / 100];

    PrintCsvLine("compile-time-only", p50, p99,
                 times.front(), times.back(),
                 [&]() { double s = 0; for (auto v : times) s += v; return s / kSamples; }(),
                 kSamples);
    EXPECT_LT(p50, 200.0) << "P50 compile time should be <200us for simple method";
}

// ── Dispatch overhead benchmarks ──────────────────────────────────

TEST_F(HybridBenchTest, AotDispatchOverhead) {
    // Measure the overhead of a single AOT-path dispatch call
    // (counter decrement + trampoline + entry return).
    PrecodeArena arena;

    constexpr int kCalls = 1000;
    HotpatchEntryV0 entry{};
    HybridPrecode precode;
    precode.call_counter.store(kCalls + kJitUpgradeThreshold, std::memory_order_relaxed);
    precode.ir = MakeReturnConstantMethod(55);
    precode.config = CompileConfig{};
    precode.aot_entry = reinterpret_cast<void*>(&AotReturn42);
    precode.entry = &entry;
    precode.trampoline = arena.AllocateHybridTrampoline(&precode);
    entry.direct_ptr = precode.trampoline;

    double elapsed = MeasureUs([&]() {
        for (int i = 0; i < kCalls; i++) {
            HybridStubDispatchImpl(&precode);
        }
    });

    double per_call = elapsed / kCalls;
    std::printf("BENCH,aot-dispatch-overhead,PER_CALL_US=%.2f,TOTAL_US=%.1f,N=%d\n",
                per_call, elapsed, kCalls);
    std::fflush(stdout);

    EXPECT_LT(per_call, 10.0) << "AOT dispatch overhead should be <10us per call";
}

TEST_F(HybridBenchTest, CompiledDispatchOverhead) {
    // Measure dispatch overhead after JIT compilation (fast path).
    PrecodeArena arena;

    HotpatchEntryV0 entry{};
    HybridPrecode precode;
    precode.call_counter.store(1, std::memory_order_relaxed);
    precode.ir = MakeReturnConstantMethod(55);
    precode.config = CompileConfig{};
    precode.aot_entry = reinterpret_cast<void*>(&AotReturn42);
    precode.entry = &entry;
    precode.trampoline = arena.AllocateHybridTrampoline(&precode);
    entry.direct_ptr = precode.trampoline;

    // Trigger JIT
    HybridStubDispatchImpl(&precode);
    ASSERT_EQ(precode.state.load(), kPrecodeCompiled);

    constexpr int kCalls = 10000;
    double elapsed = MeasureUs([&]() {
        for (int i = 0; i < kCalls; i++) {
            HybridStubDispatchImpl(&precode);
        }
    });

    double per_call = elapsed / kCalls;
    std::printf("BENCH,compiled-dispatch-overhead,PER_CALL_US=%.3f,TOTAL_US=%.1f,N=%d\n",
                per_call, elapsed, kCalls);
    std::fflush(stdout);

    EXPECT_LT(per_call, 1.0) << "Compiled dispatch should be <1us per call";
}

// ── Trampoline allocation benchmarks ──────────────────────────────

TEST_F(HybridBenchTest, TrampolineAllocLatency) {
    // Measure PrecodeArena trampoline allocation latency.
    PrecodeArena arena;
    constexpr int kSamples = 100;

    std::vector<double> times;
    times.reserve(kSamples);

    for (int s = 0; s < kSamples; s++) {
        HybridPrecode precode;
        precode.aot_entry = reinterpret_cast<void*>(&AotReturn42);

        double elapsed = MeasureUs([&]() {
            volatile void* t = arena.AllocateHybridTrampoline(&precode);
            (void)t;
        });
        times.push_back(elapsed);
    }

    std::sort(times.begin(), times.end());
    double p50 = times[kSamples * 50 / 100];
    double p99 = times[kSamples * 99 / 100];

    PrintCsvLine("trampoline-alloc", p50, p99,
                 times.front(), times.back(),
                 [&]() { double s = 0; for (auto v : times) s += v; return s / kSamples; }(),
                 kSamples);
    EXPECT_LT(p50, 5.0) << "P50 trampoline allocation should be <5us";
}

// ── Multi-method upgrade benchmarks ──────────────────────────────

TEST_F(HybridBenchTest, MultiMethodBatchUpgrade) {
    // Measure total time to upgrade N methods in sequence.
    PrecodeArena arena;

    for (int batch_size : {10, 50}) {
        std::vector<HybridPrecode> precodes(batch_size);
        std::vector<HotpatchEntryV0> entries(batch_size);

        for (int i = 0; i < batch_size; i++) {
            precodes[i].call_counter.store(kJitUpgradeThreshold, std::memory_order_relaxed);
            precodes[i].ir = MakeReturnConstantMethod(100 + i);
            precodes[i].config = CompileConfig{};
            precodes[i].aot_entry = reinterpret_cast<void*>(&AotReturn42);
            precodes[i].entry = &entries[i];
            precodes[i].trampoline = arena.AllocateHybridTrampoline(&precodes[i]);
            entries[i].direct_ptr = precodes[i].trampoline;
        }

        double elapsed = MeasureUs([&]() {
            for (int i = 0; i < batch_size; i++) {
                for (uint32_t j = 0; j < kJitUpgradeThreshold; j++) {
                    HybridStubDispatchImpl(&precodes[i]);
                }
                HybridStubDispatchImpl(&precodes[i]);
            }
        });

        double per_method = elapsed / batch_size;
        std::printf("BENCH,multi-method-upgrade,BATCH=%d,TOTAL_US=%.1f,PER_METHOD_US=%.1f\n",
                    batch_size, elapsed, per_method);
        std::fflush(stdout);
    }
}

// ── Correctness verification (not benchmark) ──────────────────────

TEST_F(HybridBenchTest, CorrectnessAcrossUpgrade) {
    // Verify correctness of all benchmark methods across upgrade.
    // Uses entry=nullptr for AOT phase to avoid the counter double-decrement
    // that occurs when the trampoline re-enters dispatch. The h1-core and e2e
    // tests already cover the full entry+trampoline dispatch mechanism.
    struct TestCase {
        void* aot_fn;
        int jit_val;
        uint64_t expected_aot;
        uint64_t expected_jit;  // JIT LdcI4 zero-extends 32-bit to uint64
    };

    TestCase cases[] = {
        {reinterpret_cast<void*>(&AotReturn42), 10, 42, 10},
        {reinterpret_cast<void*>(&AotReturn42), -5, 42, (uint64_t)(uint32_t)(int32_t)(-5)},
        {reinterpret_cast<void*>(&AotReturn0), 0, 0, 0},
        {reinterpret_cast<void*>(&AotReturn255), 255, 255, 255},
        {reinterpret_cast<void*>(&AotReturn1000), 1000, 1000, 1000},
    };

    for (auto& tc : cases) {
        HybridPrecode precode;
        precode.call_counter.store(kJitUpgradeThreshold, std::memory_order_relaxed);
        precode.ir = MakeReturnConstantMethod(tc.jit_val);
        precode.config = CompileConfig{};
        precode.aot_entry = tc.aot_fn;
        precode.entry = nullptr;  // Avoid trampoline re-entry for AOT phase

        // AOT phase — kJitUpgradeThreshold-1 calls return aot_entry directly
        for (uint32_t i = 0; i < kJitUpgradeThreshold - 1; i++) {
            void* r = HybridStubDispatchImpl(&precode);
            ASSERT_NE(r, nullptr);
            ASSERT_EQ(ExecuteNative(r), tc.expected_aot);
        }

        // Upgrade — final call triggers JIT
        HybridStubDispatchImpl(&precode);

        // JIT phase
        void* r = HybridStubDispatchImpl(&precode);
        ASSERT_NE(r, nullptr);
        ASSERT_EQ(ExecuteNative(r), tc.expected_jit);

        // Verify aot_entry preservation
        ASSERT_NE(precode.compiled, nullptr);
        ASSERT_EQ(precode.compiled->aot_entry, precode.aot_entry);
    }
}
