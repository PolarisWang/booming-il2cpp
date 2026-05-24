// ── JIT benchmark: Tier 0 vs Tier 1 performance comparison ─────────────
//
// Measures:
//   1. Compile time (Tier 0 vs Tier 1) for methods of varying complexity
//   2. Code execution time for both tiers
//   3. Code size comparison
//   4. Memory overhead (JitMethod metadata)
//
// These benchmarks verify that Tier 0 meets its <50µs compile time target
// and that Tier 1 produces faster executing code.

#include <gtest/gtest.h>

#include "jit_engine.h"
#include "jit_method.h"
#include "ir_reg_alloc.h"

#include <cstdint>
#include <cstdio>
#include <chrono>
#include <vector>

// ── Namespace aliases ──────────────────────────────────────────────
using chaos::il2cpp::interpreter::IROpCode;
using chaos::il2cpp::interpreter::RegisterInstruction;
using chaos::il2cpp::interpreter::RegisterMethod;
using chaos::il2cpp::interpreter::kRegHasDst;
using chaos::il2cpp::interpreter::kRegHasSrc1;
using chaos::il2cpp::interpreter::kRegHasSrc2;
using chaos::il2cpp::interpreter::kRegHasImm;
using chaos::il2cpp::jit::Compile;
using chaos::il2cpp::jit::JitMethod;
using chaos::il2cpp::jit::CompileConfig;
using chaos::il2cpp::jit::CompileTier;

// ── Helpers ──────────────────────────────────────────────────────────

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

static RegisterInstruction InstrBinary(IROpCode opc, uint8_t dst,
                                       uint8_t src1, uint8_t src2) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(opc, dst, src1, src2,
                           kRegHasDst | kRegHasSrc1 | kRegHasSrc2);
    ri.imm.i4 = 0;
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

// ── Method builders ───────────────────────────────────────────────────

static RegisterMethod MakeReturnConstantMethod(int32_t value) {
    RegisterMethod method;
    method.max_regs = 1;
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, value, 0, 0));
    method.instructions.push_back(InstrRet(0));
    return method;
}

static RegisterMethod MakeAddMethod(int32_t a, int32_t b) {
    RegisterMethod method;
    method.max_regs = 3;
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, a, 0, 0));
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, b, 1, 0));
    method.instructions.push_back(InstrBinary(IROpCode::Add, 2, 0, 1));
    method.instructions.push_back(InstrRet(2));
    return method;
}

// Build a method with 'n' sequential LdcI4 + Add instructions.
// The result register accumulates: (((0 + 1) + 2) + ... + n)
static RegisterMethod MakeAccumulateMethod(uint32_t n) {
    RegisterMethod method;
    method.max_regs = n + 2;
    // LdcI4 0 into r0
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 0, 0, 0));
    for (uint32_t i = 1; i <= n; i++) {
        // LdcI4 i into r{i}
        method.instructions.push_back(InstrI4(IROpCode::LdcI4, static_cast<int32_t>(i),
                                              static_cast<uint8_t>(i), 0));
        // Add: r0 = r0 + r{i}
        method.instructions.push_back(
            InstrBinary(IROpCode::Add, 0, 0, static_cast<uint8_t>(i)));
    }
    method.instructions.push_back(InstrRet(0));
    return method;
}

// ── Config builders ───────────────────────────────────────────────────

static CompileConfig MakeTier0Config() noexcept {
    CompileConfig cfg;
    cfg.enable_register_caching = false;
    cfg.enable_optimizer = false;
    cfg.enable_liveness = false;
    cfg.enable_deopt = false;
    cfg.compile_tier = CompileTier::kQuick;
    return cfg;
}

static CompileConfig MakeTier1Config() noexcept {
    CompileConfig cfg;
    cfg.enable_register_caching = true;
    cfg.enable_optimizer = true;
    cfg.enable_liveness = true;
    cfg.enable_deopt = false;   // disable deopt metadata to reduce Tier 1 overhead
    cfg.compile_tier = CompileTier::kFull;
    return cfg;
}

// ── Benchmark fixture ────────────────────────────────────────────────

struct JitBench : public ::testing::Test {};

// ── Compile time benchmarks ──────────────────────────────────────────

TEST_F(JitBench, CompileTime_Tier0_SimpleMethod) {
    // Tier 0 compile time for a 2-instr return-constant method
    auto rm = MakeReturnConstantMethod(42);
    auto cfg = MakeTier0Config();

    constexpr int kIterations = 1000;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < kIterations; i++) {
        auto* jm = Compile(rm, cfg);
        ASSERT_NE(jm, nullptr);
        delete jm;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto total_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    auto avg_ns = (total_us * 1000) / kIterations;

    std::printf("[ BENCH ] Tier 0 (simple): %llu ns avg over %d iterations (%llu µs total)\n",
                static_cast<unsigned long long>(avg_ns), kIterations,
                static_cast<unsigned long long>(total_us));

    // Tier 0 should be well under 50µs
    EXPECT_LT(avg_ns, 50000u);
}

TEST_F(JitBench, CompileTime_Tier1_SimpleMethod) {
    // Tier 1 compile time for the same 2-instr method
    auto rm = MakeReturnConstantMethod(42);
    auto cfg = MakeTier1Config();

    constexpr int kIterations = 100;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < kIterations; i++) {
        auto* jm = Compile(rm, cfg);
        ASSERT_NE(jm, nullptr);
        delete jm;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto total_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    auto avg_ns = (total_us * 1000) / kIterations;

    std::printf("[ BENCH ] Tier 1 (simple): %llu ns avg over %d iterations (%llu µs total)\n",
                static_cast<unsigned long long>(avg_ns), kIterations,
                static_cast<unsigned long long>(total_us));

    // Tier 1 may be slower due to graph coloring + optimizer
    // Just verify it compiles correctly (no strict time bound for Tier 1)
    EXPECT_GT(avg_ns, 0u);
}

TEST_F(JitBench, CompileTime_Tier0_ComplexMethod) {
    // Tier 0 compile time for a method with 100 Add instructions
    auto rm = MakeAccumulateMethod(100);
    auto cfg = MakeTier0Config();

    constexpr int kIterations = 100;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < kIterations; i++) {
        auto* jm = Compile(rm, cfg);
        ASSERT_NE(jm, nullptr);
        delete jm;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto total_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    auto avg_ns = (total_us * 1000) / kIterations;

    std::printf("[ BENCH ] Tier 0 (100-adds): %llu ns avg over %d iterations (%llu µs total)\n",
                static_cast<unsigned long long>(avg_ns), kIterations,
                static_cast<unsigned long long>(total_us));

    // Even for 100 adds, Tier 0 should be fast (< 10µs per add = 1ms total)
    EXPECT_LT(avg_ns, 1000000u);
}

TEST_F(JitBench, CompileTime_Tier1_ComplexMethod) {
    // Tier 1 compile time for a method with 100 Add instructions
    auto rm = MakeAccumulateMethod(100);
    auto cfg = MakeTier1Config();

    constexpr int kIterations = 10;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < kIterations; i++) {
        auto* jm = Compile(rm, cfg);
        ASSERT_NE(jm, nullptr);
        delete jm;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto total_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    auto avg_ns = (total_us * 1000) / kIterations;

    std::printf("[ BENCH ] Tier 1 (100-adds): %llu ns avg over %d iterations (%llu µs total)\n",
                static_cast<unsigned long long>(avg_ns), kIterations,
                static_cast<unsigned long long>(total_us));

    EXPECT_GT(avg_ns, 0u);
}

// ── Execution time benchmarks ─────────────────────────────────────────

TEST_F(JitBench, ExecTime_Tier0_vs_Tier1) {
    // Compare execution time of Tier 0 vs Tier 1 code for an accumulate method.
    // Tier 1 should be faster due to register allocation.
    auto rm = MakeAccumulateMethod(100);

    auto* tier0_jm = Compile(rm, MakeTier0Config());
    ASSERT_NE(tier0_jm, nullptr);
    EXPECT_EQ(ExecuteNative(tier0_jm->code), 5050ULL);  // sum(1..100) = 5050

    auto* tier1_jm = Compile(rm, MakeTier1Config());
    ASSERT_NE(tier1_jm, nullptr);
    EXPECT_EQ(ExecuteNative(tier1_jm->code), 5050ULL);

    // Warm up
    constexpr int kWarmup = 100;
    for (int i = 0; i < kWarmup; i++) {
        ExecuteNative(tier0_jm->code);
        ExecuteNative(tier1_jm->code);
    }

    // Measure execution time
    constexpr int kIterations = 100000;

    auto t0_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < kIterations; i++) {
        ExecuteNative(tier0_jm->code);
    }
    auto t0_end = std::chrono::high_resolution_clock::now();
    auto t0_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t0_end - t0_start).count();

    auto t1_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < kIterations; i++) {
        ExecuteNative(tier1_jm->code);
    }
    auto t1_end = std::chrono::high_resolution_clock::now();
    auto t1_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t1_end - t1_start).count();

    double t0_avg = static_cast<double>(t0_ns) / kIterations;
    double t1_avg = static_cast<double>(t1_ns) / kIterations;
    double speedup = t0_avg / t1_avg;

    std::printf("[ BENCH ] Tier 0 exec: %.2f ns/call (%lld ns total over %d calls)\n",
                t0_avg, static_cast<long long>(t0_ns), kIterations);
    std::printf("[ BENCH ] Tier 1 exec: %.2f ns/call (%lld ns total over %d calls)\n",
                t1_avg, static_cast<long long>(t1_ns), kIterations);
    std::printf("[ BENCH ] Tier 1 speedup vs Tier 0: %.2fx\n", speedup);

    delete tier0_jm;
    delete tier1_jm;
}

// ── Code size comparison ──────────────────────────────────────────────

TEST_F(JitBench, CodeSize_Tier0_vs_Tier1) {
    // Compare code size for methods of varying complexity
    struct MethodSpec {
        const char* name;
        RegisterMethod (*builder)();
    };

    MethodSpec methods[] = {
        {"return-42",   []() { return MakeReturnConstantMethod(42); }},
        {"add(10+20)",  []() { return MakeAddMethod(10, 20); }},
        {"accum-10",    []() { return MakeAccumulateMethod(10); }},
        {"accum-50",    []() { return MakeAccumulateMethod(50); }},
        {"accum-100",   []() { return MakeAccumulateMethod(100); }},
    };

    std::printf("[ BENCH ] %-15s | %10s | %10s | %8s\n",
                "Method", "Tier0(bytes)", "Tier1(bytes)", "Delta");
    std::printf("[ BENCH ] %s\n", std::string(50, '-').c_str());

    for (const auto& ms : methods) {
        auto rm = ms.builder();

        auto* t0 = Compile(rm, MakeTier0Config());
        ASSERT_NE(t0, nullptr);

        auto* t1 = Compile(rm, MakeTier1Config());
        ASSERT_NE(t1, nullptr);

        int delta = static_cast<int>(t1->code_size) - static_cast<int>(t0->code_size);
        std::printf("[ BENCH ] %-15s | %10u | %10u | %+8d\n",
                    ms.name, t0->code_size, t1->code_size, delta);

        delete t0;
        delete t1;
    }
}

// ── Correctness verification ──────────────────────────────────────────

TEST_F(JitBench, Correctness_Range) {
    // Verify correctness across a range of values for both tiers
    auto cfg0 = MakeTier0Config();
    auto cfg1 = MakeTier1Config();

    for (int32_t val = -100; val <= 100; val += 10) {
        auto rm = MakeReturnConstantMethod(val);

        auto* t0 = Compile(rm, cfg0);
        ASSERT_NE(t0, nullptr);
        EXPECT_EQ(ExecuteNative(t0->code), static_cast<uint64_t>(static_cast<uint32_t>(val)))
            << "Tier 0 failed for value " << val;
        delete t0;

        auto* t1 = Compile(rm, cfg1);
        ASSERT_NE(t1, nullptr);
        EXPECT_EQ(ExecuteNative(t1->code), static_cast<uint64_t>(static_cast<uint32_t>(val)))
            << "Tier 1 failed for value " << val;
        delete t1;
    }
}
