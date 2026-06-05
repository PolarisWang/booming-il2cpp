// jit_inline_test.cpp — Inlining infrastructure unit tests
//
// Tests EvaluateInline heuristics (callee lookup, SEH rejection, cost model)
// and end-to-end inline compilation via the JIT pipeline.
//
// Pattern follows jit_il_smoke_test.cpp / jit_mode_test.cpp conventions.

#include <gtest/gtest.h>

#include "jit_inline.h"
#include "jit_precode.h"

#include <cstdint>

// ── Namespace aliases ──────────────────────────────────────────────
using chaos::il2cpp::interpreter::IROpCode;
using chaos::il2cpp::interpreter::RegisterInstruction;
using chaos::il2cpp::interpreter::RegisterMethod;
using chaos::il2cpp::interpreter::kRegHasDst;
using chaos::il2cpp::interpreter::kRegHasImm;
using chaos::il2cpp::interpreter::kRegHasSrc1;
using chaos::il2cpp::interpreter::kRegIsCall;
using chaos::il2cpp::jit::InlineConfig;
using chaos::il2cpp::jit::InlineDecision;
using chaos::il2cpp::jit::EvaluateInline;
using chaos::il2cpp::jit::g_token_to_precode;
using chaos::il2cpp::jit::JitPrecode;

// ── Helpers ─────────────────────────────────────────────────────

static uint64_t MakeHeader(IROpCode opc, uint8_t dst, uint8_t src1,
                            uint8_t src2, uint8_t flags) noexcept {
    return static_cast<uint64_t>(opc) |
           (static_cast<uint64_t>(dst)  << 16) |
           (static_cast<uint64_t>(src1) << 24) |
           (static_cast<uint64_t>(src2) << 32) |
           (static_cast<uint64_t>(flags) << 40);
}

static RegisterInstruction MakeInstrI4(IROpCode opc, int32_t imm,
                                        uint8_t dst = 0) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(opc, dst, 0, 0, kRegHasDst | kRegHasImm);
    ri.imm.i4 = imm;
    return ri;
}

static RegisterInstruction MakeRet(uint8_t src) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::Ret, 0, src, 0, kRegHasSrc1);
    ri.imm.i4 = 0;
    return ri;
}

static constexpr uint32_t kCalleeToken = 42;
static constexpr uint32_t kAltToken    = 99;

// ── Test fixture ──────────────────────────────────────────────────
struct JitInlineTest : public ::testing::Test {
    void SetUp() override {
        // Unregister tokens from previous test runs
        g_token_to_precode.Unregister(kCalleeToken);
        g_token_to_precode.Unregister(kAltToken);
    }

    void TearDown() override {
        g_token_to_precode.Unregister(kCalleeToken);
        g_token_to_precode.Unregister(kAltToken);
    }
};

// ── EvaluateInline: finds registered callee ──────────────────────
TEST_F(JitInlineTest, EvaluateInline_FindsCallee) {
    JitPrecode precode;
    precode.ir.max_regs = 1;
    precode.ir.instructions.push_back(MakeInstrI4(IROpCode::LdcI4, 42, 0));
    precode.ir.instructions.push_back(MakeRet(0));
    g_token_to_precode.Register(kCalleeToken, &precode);

    InlineConfig cfg;
    auto d = EvaluateInline(kCalleeToken, 0, true, 0, cfg);

    EXPECT_TRUE(d.should_inline);
    EXPECT_EQ(d.callee_token, kCalleeToken);
    EXPECT_EQ(d.callee_instr_count, 2u);
    EXPECT_EQ(d.callee_max_vreg, 1u);
    EXPECT_NE(d.callee_rm, nullptr);
}

// ── EvaluateInline: rejects unknown token ────────────────────────
TEST_F(JitInlineTest, EvaluateInline_RejectsUnknownToken) {
    InlineConfig cfg;
    auto d = EvaluateInline(kCalleeToken, 0, true, 0, cfg);

    EXPECT_FALSE(d.should_inline);
    EXPECT_EQ(d.callee_token, kCalleeToken);
}

// ── EvaluateInline: rejects empty method ─────────────────────────
TEST_F(JitInlineTest, EvaluateInline_RejectsEmptyMethod) {
    JitPrecode precode;
    precode.ir.max_regs = 0;
    // No instructions
    g_token_to_precode.Register(kCalleeToken, &precode);

    InlineConfig cfg;
    auto d = EvaluateInline(kCalleeToken, 0, true, 0, cfg);

    EXPECT_FALSE(d.should_inline);
}

// ── EvaluateInline: rejects method with SEH ──────────────────────
TEST_F(JitInlineTest, EvaluateInline_RejectsSehMethod) {
    JitPrecode precode;
    precode.ir.max_regs = 1;
    precode.ir.instructions.push_back(MakeInstrI4(IROpCode::LdcI4, 42, 0));
    precode.ir.instructions.push_back(MakeRet(0));
    // Non-empty SEH clauses → rejection
    precode.ir.seh_clauses.push_back({});
    g_token_to_precode.Register(kCalleeToken, &precode);

    InlineConfig cfg;
    auto d = EvaluateInline(kCalleeToken, 0, true, 0, cfg);

    EXPECT_FALSE(d.should_inline);
}

// ── EvaluateInline: cost model rejects large method ─────────────
TEST_F(JitInlineTest, EvaluateInline_RejectsLargeMethod) {
    JitPrecode precode;
    precode.ir.max_regs = 1;
    for (uint32_t i = 0; i < 30; ++i)
        precode.ir.instructions.push_back(MakeInstrI4(IROpCode::LdcI4, 0, 0));
    precode.ir.instructions.push_back(MakeRet(0));
    g_token_to_precode.Register(kCalleeToken, &precode);

    InlineConfig cfg;
    cfg.max_callee_nodes = 25;
    auto d = EvaluateInline(kCalleeToken, 0, true, 0, cfg);

    EXPECT_FALSE(d.should_inline);
}

// ── EvaluateInline: loop depth reduces effective cost ───────────
TEST_F(JitInlineTest, EvaluateInline_LoopBonusEnablesInline) {
    JitPrecode precode;
    precode.ir.max_regs = 1;
    // 20 instructions — over a threshold of 15
    for (uint32_t i = 0; i < 20; ++i)
        precode.ir.instructions.push_back(MakeInstrI4(IROpCode::LdcI4, 0, 0));
    precode.ir.instructions.push_back(MakeRet(0));
    g_token_to_precode.Register(kCalleeToken, &precode);

    InlineConfig cfg;
    cfg.max_callee_nodes = 15;
    cfg.loop_bonus = 3;

    // Without loop depth: cost = 20 > 15 → rejected
    EXPECT_FALSE(EvaluateInline(kCalleeToken, 0, true, 0, cfg).should_inline);

    // With loop depth 2: cost = 20 - 2*3 = 14 < 15 → accepted
    EXPECT_TRUE(EvaluateInline(kCalleeToken, 0, true, 2, cfg).should_inline);
}

// ── EvaluateInline: return-used bonus reduces cost ──────────────
TEST_F(JitInlineTest, EvaluateInline_ReturnUsedBonus) {
    JitPrecode precode;
    precode.ir.max_regs = 1;
    // 22 instructions + missing return-used bonus → rejected
    for (uint32_t i = 0; i < 22; ++i)
        precode.ir.instructions.push_back(MakeInstrI4(IROpCode::LdcI4, 0, 0));
    precode.ir.instructions.push_back(MakeRet(0));
    g_token_to_precode.Register(kCalleeToken, &precode);

    InlineConfig cfg;
    cfg.max_callee_nodes = 20;
    cfg.return_used_bonus = 5;

    // return_value_used = false: cost = 22 > 20
    EXPECT_FALSE(EvaluateInline(kCalleeToken, 0, false, 0, cfg).should_inline);

    // return_value_used = true: cost = 22 - 5 = 17 < 20
    EXPECT_TRUE(EvaluateInline(kCalleeToken, 0, true, 0, cfg).should_inline);
}

// ── EvaluateInline: rejects different token than registered ─────
TEST_F(JitInlineTest, EvaluateInline_DifferentToken) {
    JitPrecode precode;
    precode.ir.max_regs = 1;
    precode.ir.instructions.push_back(MakeInstrI4(IROpCode::LdcI4, 42, 0));
    precode.ir.instructions.push_back(MakeRet(0));
    g_token_to_precode.Register(kCalleeToken, &precode);

    InlineConfig cfg;
    // Look up a different token that is NOT registered
    auto d = EvaluateInline(kAltToken, 0, true, 0, cfg);

    EXPECT_FALSE(d.should_inline);
}

// ── EvaluateInline: hot-update re-registration updates precode ──
TEST_F(JitInlineTest, EvaluateInline_HotUpdateReregister) {
    JitPrecode precode1;
    precode1.ir.max_regs = 1;
    precode1.ir.instructions.push_back(MakeInstrI4(IROpCode::LdcI4, 10, 0));
    precode1.ir.instructions.push_back(MakeRet(0));

    JitPrecode precode2;
    precode2.ir.max_regs = 1;
    precode2.ir.instructions.push_back(MakeInstrI4(IROpCode::LdcI4, 99, 0));
    precode2.ir.instructions.push_back(MakeRet(0));

    // Register then re-register (hot-update)
    g_token_to_precode.Register(kCalleeToken, &precode1);
    g_token_to_precode.Register(kCalleeToken, &precode2);

    InlineConfig cfg;
    auto d = EvaluateInline(kCalleeToken, 0, true, 0, cfg);

    EXPECT_TRUE(d.should_inline);
    // Should see the LATEST registration's callee_rm
    ASSERT_NE(d.callee_rm, nullptr);
    EXPECT_EQ(d.callee_rm->max_regs, 1u);
}

// ── EvaluateInline: unregister removes callee ───────────────────
TEST_F(JitInlineTest, EvaluateInline_UnregisterRemovesCallee) {
    JitPrecode precode;
    precode.ir.max_regs = 1;
    precode.ir.instructions.push_back(MakeInstrI4(IROpCode::LdcI4, 42, 0));
    precode.ir.instructions.push_back(MakeRet(0));
    g_token_to_precode.Register(kCalleeToken, &precode);

    // Confirm it's found
    EXPECT_TRUE(EvaluateInline(kCalleeToken, 0, true, 0, InlineConfig{}).should_inline);

    // Unregister and confirm it's not found
    g_token_to_precode.Unregister(kCalleeToken);
    EXPECT_FALSE(EvaluateInline(kCalleeToken, 0, true, 0, InlineConfig{}).should_inline);
}

// ── EvaluateInline: cost floor at 1 ─────────────────────────────
TEST_F(JitInlineTest, EvaluateInline_CostFloorAtOne) {
    JitPrecode precode;
    precode.ir.max_regs = 1;
    // 2 instructions
    precode.ir.instructions.push_back(MakeInstrI4(IROpCode::LdcI4, 1, 0));
    precode.ir.instructions.push_back(MakeRet(0));
    g_token_to_precode.Register(kCalleeToken, &precode);

    InlineConfig cfg;
    cfg.max_callee_nodes = 25;
    cfg.loop_bonus = 100;  // huge bonus would make cost negative

    // Cost = max(1, 2 - 0 - 0) = 2; still below threshold
    auto d = EvaluateInline(kCalleeToken, 0, false, 0, cfg);
    EXPECT_TRUE(d.should_inline);

    // With loop depth 5 and return used: cost = max(1, 2 - 500 - 100) = 1
    auto d2 = EvaluateInline(kCalleeToken, 0, true, 5, cfg);
    EXPECT_TRUE(d2.should_inline);
    // Verify callee metadata is filled in
    EXPECT_EQ(d2.callee_instr_count, 2u);
}
