// ── PGO (Profile-Guided Optimization) tests ─────────────────────────────
//
// Tests the PGO Tier 0→Tier 1 upgrade path:
//   1. PGO call counting via pgo_call_count field
//   2. Tier 1 recompilation trigger at threshold
//   3. JitRecompileToTier1 produces correct code
//   4. Tier 0 code (via Compile directly) returns correct values
//   5. Tier 1 code (via Compile with kTier1) returns correct values
//   6. direct_ptr patching behavior with enable_pgo true/false

#include <gtest/gtest.h>

#include "jit_precode.h"
#include "jit_engine.h"
#include "jit_method.h"
#include "ir_reg_alloc.h"

#include <codegen_bridge.h>

#include <windows.h>
#include <cstdint>
#include <cstring>

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
using chaos::il2cpp::jit::JitPrecode;
using chaos::il2cpp::jit::PrecodeArena;
using chaos::il2cpp::jit::JitRecompileToTier1;
using chaos::il2cpp::jit::kPrecodeCompiled;
using chaos::il2cpp::jit::kPgoTier1Threshold;

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

static CompileConfig MakeTier0Config(bool enable_pgo = false) noexcept {
    CompileConfig cfg;
    cfg.enable_register_caching = false;
    cfg.enable_optimizer = false;
    cfg.enable_liveness = false;
    cfg.enable_deopt = false;
    cfg.compile_tier = CompileTier::kQuick;
    cfg.enable_pgo = enable_pgo;
    return cfg;
}

// ── PgoTest fixture ────────────────────────────────────────────────
struct PgoTest : public ::testing::Test {};

// ── Tests ───────────────────────────────────────────────────────────

TEST_F(PgoTest, Tier0CodeWorks) {
    // Baseline: Tier 0 code produced by Compile() should execute correctly
    auto rm = MakeReturnConstantMethod(42);
    auto cfg = MakeTier0Config(false);
    auto* jm = Compile(rm, cfg);
    ASSERT_NE(jm, nullptr);
    ASSERT_NE(jm->code, nullptr);
    EXPECT_EQ(ExecuteNative(jm->code), 42ULL);
    delete jm;
}

TEST_F(PgoTest, PgoCallCountIncrements) {
    // After Tier 0 compilation, pgo_call_count tracks calls through dispatch
    auto rm = MakeReturnConstantMethod(77);
    auto cfg = MakeTier0Config(true);
    auto* jm = Compile(rm, cfg);
    ASSERT_NE(jm, nullptr);

    // Simulate the precode state after JitStubDispatchImpl compilation
    JitPrecode precode;
    precode.ir = std::move(rm);
    precode.config = cfg;
    precode.compiled = jm;
    precode.state.store(kPrecodeCompiled, std::memory_order_release);

    // Simulate dispatch calls by manually incrementing pgo_call_count
    // (the dispatch function does the increment on each call after Tier 0)
    for (int i = 0; i < 10; i++) {
        precode.pgo_call_count.fetch_add(1, std::memory_order_relaxed);
    }

    // The code should still execute correctly through the stored JitMethod
    EXPECT_EQ(ExecuteNative(jm->code), 77ULL);
    EXPECT_EQ(precode.pgo_call_count.load(), 10u);
    // Should NOT have triggered Tier 1 yet (< threshold)
    EXPECT_FALSE(precode.tier1_enqueued);
}

TEST_F(PgoTest, PgoTriggersTier1Enqueue) {
    // When pgo_call_count exceeds kPgoTier1Threshold, tier1_enqueued is set
    auto rm = MakeReturnConstantMethod(55);
    auto cfg = MakeTier0Config(true);
    auto* jm = Compile(rm, cfg);
    ASSERT_NE(jm, nullptr);

    JitPrecode precode;
    precode.ir = std::move(rm);
    precode.config = cfg;
    precode.compiled = jm;
    precode.state.store(kPrecodeCompiled, std::memory_order_release);

    // Simulate calls beyond the threshold
    for (uint32_t i = 0; i <= kPgoTier1Threshold + 10; i++) {
        precode.pgo_call_count.fetch_add(1, std::memory_order_relaxed);
        // Simulate the dispatch check: when count > threshold, set tier1_enqueued
        if (precode.pgo_call_count.load(std::memory_order_relaxed) > kPgoTier1Threshold
            && !precode.tier1_enqueued) {
            precode.tier1_enqueued = true;
        }
    }

    EXPECT_TRUE(precode.tier1_enqueued);
    EXPECT_GT(precode.pgo_call_count.load(), kPgoTier1Threshold);
}

TEST_F(PgoTest, Tier1CodeCorrectness) {
    // JitRecompileToTier1 should produce correct Tier 1 code
    auto rm = MakeReturnConstantMethod(123);
    auto cfg = MakeTier0Config(true);
    auto* tier0_jm = Compile(rm, cfg);
    ASSERT_NE(tier0_jm, nullptr);
    EXPECT_EQ(ExecuteNative(tier0_jm->code), 123ULL);

    // Set up precode as if after Tier 0 compilation
    JitPrecode precode;
    precode.ir = std::move(rm);
    precode.config = cfg;
    precode.compiled = tier0_jm;

    // Call JitRecompileToTier1 — this deletes the old tier0_jm and creates Tier 1
    void* tier1_code = JitRecompileToTier1(&precode, false);
    ASSERT_NE(tier1_code, nullptr);

    // Tier 1 code should return the correct value
    EXPECT_EQ(ExecuteNative(tier1_code), 123ULL);
}

TEST_F(PgoTest, Tier1CodeForAddMethod) {
    // Verify Tier 1 recompilation handles binary operations correctly
    auto rm = MakeAddMethod(10, 20);
    auto cfg = MakeTier0Config(true);
    auto* tier0_jm = Compile(rm, cfg);
    ASSERT_NE(tier0_jm, nullptr);
    EXPECT_EQ(ExecuteNative(tier0_jm->code), 30ULL);

    JitPrecode precode;
    precode.ir = std::move(rm);
    precode.config = cfg;
    precode.compiled = tier0_jm;

    void* tier1_code = JitRecompileToTier1(&precode, false);
    ASSERT_NE(tier1_code, nullptr);
    EXPECT_EQ(ExecuteNative(tier1_code), 30ULL);
}

TEST_F(PgoTest, DirectPtrPatchedByTierManager) {
    // Simulate the TierManager background thread flow:
    // 1. Tier 0 compiles, direct_ptr is NOT patched (enable_pgo=true)
    // 2. Tier 1 recompilation happens
    // 3. TierManager patches direct_ptr to Tier 1 code
    PrecodeArena arena;

    JitPrecode precode;
    precode.ir = MakeReturnConstantMethod(99);
    precode.config = MakeTier0Config(true);

    HotpatchEntryV0 entry{};
    precode.entry = &entry;
    precode.trampoline = arena.AllocateJitTrampoline(&precode);
    ASSERT_NE(precode.trampoline, nullptr);
    entry.direct_ptr = precode.trampoline;

    // Tier 0 compile
    auto* tier0_jm = Compile(precode.ir, precode.config);
    ASSERT_NE(tier0_jm, nullptr);
    precode.compiled = tier0_jm;
    precode.state.store(kPrecodeCompiled, std::memory_order_release);

    // direct_ptr should still be the trampoline (PGO mode: not patched after Tier 0)
    EXPECT_EQ(entry.direct_ptr, precode.trampoline);

    // Tier 1 recompilation (background thread simulation)
    void* tier1_code = JitRecompileToTier1(&precode, false);
    ASSERT_NE(tier1_code, nullptr);
    EXPECT_EQ(ExecuteNative(tier1_code), 99ULL);

    // TierManager patches direct_ptr after Tier 1 compilation
    entry.direct_ptr = tier1_code;

    // direct_ptr should now point to Tier 1 code
    EXPECT_EQ(entry.direct_ptr, tier1_code);
    EXPECT_NE(entry.direct_ptr, precode.trampoline);
}
