// layout_engine_test.cpp — LayoutEngine unit tests.
//
// Tests the chaos::il2cpp::layout namespace functions:
//   AlignUp, NaturalAlignment, ComputeStructAlignment,
//   RecursionGuard, GetLayoutEngine, VerifyModuleLayouts.
//
// Pure unit tests — no runtime bootstrap required.

#include <gtest/gtest.h>

#include <layout_engine.h>

#include <cstdint>

using namespace chaos::il2cpp::layout;

// ════════════════════════════════════════════════════════════════════════════
// L0 — Test fixture (friend of LayoutEngine for white-box access)
// ════════════════════════════════════════════════════════════════════════════

class LayoutEngineWhiteboxTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// ════════════════════════════════════════════════════════════════════════════
// L1 — AlignUp
// ════════════════════════════════════════════════════════════════════════════

TEST_F(LayoutEngineWhiteboxTest, AlignUpAlignmentOne) {
    EXPECT_EQ(LayoutEngine::AlignUp(0u, 1u), 0u);
    EXPECT_EQ(LayoutEngine::AlignUp(5u, 1u), 5u);
}

TEST_F(LayoutEngineWhiteboxTest, AlignUpAlignmentFour) {
    EXPECT_EQ(LayoutEngine::AlignUp(0u, 4u), 0u);
    EXPECT_EQ(LayoutEngine::AlignUp(1u, 4u), 4u);
    EXPECT_EQ(LayoutEngine::AlignUp(3u, 4u), 4u);
    EXPECT_EQ(LayoutEngine::AlignUp(4u, 4u), 4u);
    EXPECT_EQ(LayoutEngine::AlignUp(5u, 4u), 8u);
}

TEST_F(LayoutEngineWhiteboxTest, AlignUpAlignmentEight) {
    EXPECT_EQ(LayoutEngine::AlignUp(7u, 8u), 8u);
    EXPECT_EQ(LayoutEngine::AlignUp(8u, 8u), 8u);
    EXPECT_EQ(LayoutEngine::AlignUp(15u, 8u), 16u);
}

TEST_F(LayoutEngineWhiteboxTest, AlignUpZeroAlignment) {
    EXPECT_EQ(LayoutEngine::AlignUp(42u, 0u), 42u);
}

TEST_F(LayoutEngineWhiteboxTest, AlignUpLargeValues) {
    EXPECT_EQ(LayoutEngine::AlignUp(1024u, 8u), 1024u);
    EXPECT_EQ(LayoutEngine::AlignUp(1025u, 8u), 1032u);
}

TEST_F(LayoutEngineWhiteboxTest, AlignUpPowerOfTwoBoundary) {
    // Start from a=2: for a=1, AlignUp(0, 1) == 0 (every offset is 1-aligned).
    for (uint32_t a = 2u; a <= 64u; a <<= 1u) {
        EXPECT_EQ(LayoutEngine::AlignUp(0u, a), 0u);
        EXPECT_EQ(LayoutEngine::AlignUp(a - 1u, a), a);
        EXPECT_EQ(LayoutEngine::AlignUp(a, a), a);
        EXPECT_EQ(LayoutEngine::AlignUp(a + 1u, a), a + a);
    }
}

// ════════════════════════════════════════════════════════════════════════════
// L2 — NaturalAlignment
// ════════════════════════════════════════════════════════════════════════════

TEST_F(LayoutEngineWhiteboxTest, NaturalAlignmentZero) {
    EXPECT_EQ(LayoutEngine::NaturalAlignment(0u), 1u);
}

TEST_F(LayoutEngineWhiteboxTest, NaturalAlignmentOne) {
    EXPECT_EQ(LayoutEngine::NaturalAlignment(1u), 1u);
}

TEST_F(LayoutEngineWhiteboxTest, NaturalAlignmentTwo) {
    EXPECT_EQ(LayoutEngine::NaturalAlignment(2u), 2u);
}

TEST_F(LayoutEngineWhiteboxTest, NaturalAlignmentFour) {
    EXPECT_EQ(LayoutEngine::NaturalAlignment(4u), 4u);
}

TEST_F(LayoutEngineWhiteboxTest, NaturalAlignmentEightAndAbove) {
    EXPECT_EQ(LayoutEngine::NaturalAlignment(8u), 8u);
    EXPECT_EQ(LayoutEngine::NaturalAlignment(16u), 8u);
    EXPECT_EQ(LayoutEngine::NaturalAlignment(100u), 8u);
}

// ════════════════════════════════════════════════════════════════════════════
// L3 — ComputeStructAlignment
// ════════════════════════════════════════════════════════════════════════════

TEST_F(LayoutEngineWhiteboxTest, ComputeStructAlignmentNullFields) {
    EXPECT_EQ(LayoutEngine::ComputeStructAlignment(nullptr, 0u), 1u);
}

TEST_F(LayoutEngineWhiteboxTest, ComputeStructAlignmentZeroFields) {
    // Non-null fields pointer with zero count — should return 1.
    FieldLayout fields[1] = {{0, 0, 1u, 1u, false}};
    EXPECT_EQ(LayoutEngine::ComputeStructAlignment(fields, 0u), 1u);
}

TEST_F(LayoutEngineWhiteboxTest, ComputeStructAlignmentSingleField) {
    FieldLayout fields[] = {{0, 0, 4u, 4u, false}};
    EXPECT_EQ(LayoutEngine::ComputeStructAlignment(fields, 1u), 4u);
}

TEST_F(LayoutEngineWhiteboxTest, ComputeStructAlignmentMaximumField) {
    FieldLayout fields[] = {
        {0, 0, 1u, 1u, false},
        {0, 0, 2u, 2u, false},
        {0, 0, 4u, 4u, false},
    };
    EXPECT_EQ(LayoutEngine::ComputeStructAlignment(fields, 3u), 4u);
}

TEST_F(LayoutEngineWhiteboxTest, ComputeStructAlignmentCappedAtPointerSize) {
    constexpr uint32_t ptr_size = static_cast<uint32_t>(sizeof(void*));
    FieldLayout fields[] = {{0, 0, 16u, 16u, false}};
    EXPECT_EQ(LayoutEngine::ComputeStructAlignment(fields, 1u), ptr_size);
}

TEST_F(LayoutEngineWhiteboxTest, ComputeStructAlignmentAllOnes) {
    FieldLayout fields[] = {
        {0, 0, 1u, 1u, false},
        {0, 0, 1u, 1u, false},
    };
    EXPECT_EQ(LayoutEngine::ComputeStructAlignment(fields, 2u), 1u);
}

TEST_F(LayoutEngineWhiteboxTest, ComputeStructAlignmentFromNaturalAlignments) {
    FieldLayout fields[] = {
        {0, 0, 1u, LayoutEngine::NaturalAlignment(1u), false},
        {0, 0, 2u, LayoutEngine::NaturalAlignment(2u), false},
        {0, 0, 4u, LayoutEngine::NaturalAlignment(4u), false},
        {0, 0, 8u, LayoutEngine::NaturalAlignment(8u), false},
    };
    constexpr uint32_t ptr_size = static_cast<uint32_t>(sizeof(void*));
    EXPECT_EQ(LayoutEngine::ComputeStructAlignment(fields, 4u), ptr_size);
}

// ════════════════════════════════════════════════════════════════════════════
// L4 — RecursionGuard
// ════════════════════════════════════════════════════════════════════════════

TEST_F(LayoutEngineWhiteboxTest, RecursionGuardEnterSucceeds) {
    RecursionGuard guard;
    EXPECT_TRUE(guard.Enter(42u));
    EXPECT_EQ(guard.visit_count, 1u);
    EXPECT_EQ(guard.depth, 1u);
}

TEST_F(LayoutEngineWhiteboxTest, RecursionGuardEnterLeaveCycle) {
    RecursionGuard guard;
    EXPECT_TRUE(guard.Enter(1u));
    EXPECT_TRUE(guard.Enter(2u));
    EXPECT_FALSE(guard.Enter(1u));  // cycle detected
    guard.Leave();
    guard.Leave();
    EXPECT_EQ(guard.visit_count, 0u);
    EXPECT_EQ(guard.depth, 0u);
}

TEST_F(LayoutEngineWhiteboxTest, RecursionGuardDepthLimit) {
    RecursionGuard guard;
    for (uint32_t i = 0u; i < kLayoutMaxDepth; ++i) {
        EXPECT_TRUE(guard.Enter(i + 100u));
    }
    // Next enter should fail (depth limit reached).
    EXPECT_FALSE(guard.Enter(kLayoutMaxDepth + 100u));
}

TEST_F(LayoutEngineWhiteboxTest, RecursionGuardScopedEnter) {
    RecursionGuard guard;
    {
        RecursionGuard::ScopedEnter enter(guard, 42u);
        EXPECT_TRUE(enter.ok);
        EXPECT_EQ(guard.visit_count, 1u);
        EXPECT_EQ(guard.depth, 1u);
    }
    // After scope exit, guard should be restored.
    EXPECT_EQ(guard.visit_count, 0u);
    EXPECT_EQ(guard.depth, 0u);
}

TEST_F(LayoutEngineWhiteboxTest, RecursionGuardScopedEnterCycleDetection) {
    RecursionGuard guard;
    guard.Enter(1u);
    {
        RecursionGuard::ScopedEnter enter(guard, 1u);
        EXPECT_FALSE(enter.ok);  // cycle detected during ScopedEnter
    }
    EXPECT_EQ(guard.depth, 1u);
    EXPECT_EQ(guard.visit_count, 1u);
    guard.Leave();
}

TEST_F(LayoutEngineWhiteboxTest, RecursionGuardLeaveWhenEmpty) {
    RecursionGuard guard;
    guard.Leave();  // should be a no-op
    EXPECT_EQ(guard.visit_count, 0u);
    EXPECT_EQ(guard.depth, 0u);
}

TEST_F(LayoutEngineWhiteboxTest, RecursionGuardMultipleEnters) {
    RecursionGuard guard;
    for (uint32_t i = 0u; i < 10u; ++i) {
        EXPECT_TRUE(guard.Enter(i));
    }
    EXPECT_EQ(guard.visit_count, 10u);
    EXPECT_EQ(guard.depth, 10u);

    // Re-entering any visited type should detect cycle.
    EXPECT_FALSE(guard.Enter(5u));
}

TEST_F(LayoutEngineWhiteboxTest, RecursionGuardFullCycleThenClear) {
    RecursionGuard guard;
    for (uint32_t i = 0u; i < 10u; ++i) {
        EXPECT_TRUE(guard.Enter(i));
    }
    // Leave all 10.
    for (uint32_t i = 0u; i < 10u; ++i) {
        guard.Leave();
    }
    EXPECT_EQ(guard.visit_count, 0u);
    EXPECT_EQ(guard.depth, 0u);

    // After clear, entering a previously-visited type should succeed.
    EXPECT_TRUE(guard.Enter(5u));
    guard.Leave();
}

// ════════════════════════════════════════════════════════════════════════════
// L5 — GetLayoutEngine (singleton)
// ════════════════════════════════════════════════════════════════════════════

TEST_F(LayoutEngineWhiteboxTest, SingletonReturnsNonNull) {
    auto* engine = GetLayoutEngine();
    EXPECT_NE(engine, nullptr);
}

TEST_F(LayoutEngineWhiteboxTest, SingletonReturnsSamePointer) {
    auto* engine1 = GetLayoutEngine();
    auto* engine2 = GetLayoutEngine();
    EXPECT_EQ(engine1, engine2);
}

// ════════════════════════════════════════════════════════════════════════════
// L6 — LayoutEngine public API null-safety
// ════════════════════════════════════════════════════════════════════════════

TEST_F(LayoutEngineWhiteboxTest, GetOrComputeLayoutZeroHandleReturnsNull) {
    auto* engine = GetLayoutEngine();
    ASSERT_NE(engine, nullptr);
    EXPECT_EQ(engine->GetOrComputeLayout(0u), nullptr);
}

TEST_F(LayoutEngineWhiteboxTest, CountersStartAtZero) {
    auto* engine = GetLayoutEngine();
    ASSERT_NE(engine, nullptr);
    EXPECT_EQ(engine->GetCacheHitCount(), 0u);
    EXPECT_EQ(engine->GetComputeCount(), 0u);
}

TEST_F(LayoutEngineWhiteboxTest, InvalidateModuleCacheNoCrash) {
    auto* engine = GetLayoutEngine();
    ASSERT_NE(engine, nullptr);
    // Invalidate a module that has no cached layouts — should be a no-op.
    engine->InvalidateModuleCache(0u);
    SUCCEED();
}

TEST_F(LayoutEngineWhiteboxTest, InvalidateModuleCacheLargeModuleId) {
    auto* engine = GetLayoutEngine();
    ASSERT_NE(engine, nullptr);
    // Large module_id beyond kMaxModules — should be a no-op.
    engine->InvalidateModuleCache(0xFFFFFFFFu);
    SUCCEED();
}

// ════════════════════════════════════════════════════════════════════════════
// L7 — VerifyModuleLayouts (industrialization stub)
// ════════════════════════════════════════════════════════════════════════════

TEST_F(LayoutEngineWhiteboxTest, VerifyModuleLayoutsNullModuleNoCrash) {
    // Currently a no-op stub — just verify no crash.
    VerifyModuleLayouts(nullptr);
    SUCCEED();
}
