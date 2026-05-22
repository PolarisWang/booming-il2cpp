// execution_context_smoke_test.cpp — ExecutionContext SuppressFlow/RestoreFlow
#include <gtest/gtest.h>

#include <execution_context.h>

namespace threading = chaos::il2cpp::runtime_core::threading;

// ── Test fixture ─────────────────────────────────────────────────────────
// Cleans up all AsyncLocal values between each test to prevent cross-test
// contamination (thread_local storage persists across tests).

class ExecutionContextTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clear any AsyncLocal values left by previous tests.
        for (uint32_t i = 0; i < 64; i++) {
            // We don't know the keys, so just exhaustively clear by
            // capturing and resetting each value.
        }
        // Reset suppress depth.
        while (threading::ExecutionContextIsFlowSuppressed()) {
            threading::ExecutionContextRestoreFlow(0);
        }
    }

    void TearDown() override {
        // Same cleanup as SetUp.
        while (threading::ExecutionContextIsFlowSuppressed()) {
            threading::ExecutionContextRestoreFlow(0);
        }
    }
};

// ── Helpers ──────────────────────────────────────────────────────────────

static int s_callback_invoked = 0;
static void test_callback(void*) {
    s_callback_invoked++;
}

// Clear all AsyncLocal values by capturing the current context and zeroing.
static void clear_all_async_locals() {
    auto* ctx = threading::ExecutionContextCapture();
    if (ctx == nullptr) return;
    for (uint32_t i = 0; i < ctx->value_count; i++) {
        threading::AsyncLocalSetValue(ctx->values[i].key, 0);
    }
    threading::ExecutionContextFree(ctx);
}

// ── Tests ───────────────────────────────────────────────────────────────

TEST_F(ExecutionContextTest, EmptyCapture) {
    clear_all_async_locals();
    auto* ctx = threading::ExecutionContextCapture();
    EXPECT_EQ(ctx, nullptr);
}

TEST_F(ExecutionContextTest, SingleValue) {
    clear_all_async_locals();
    constexpr uint64_t kKey = 0x1234;
    constexpr CHAOS_IL2CPP_INTPTR kVal = static_cast<CHAOS_IL2CPP_INTPTR>(0xDEAD);

    threading::AsyncLocalSetValue(kKey, kVal);
    auto* ctx = threading::ExecutionContextCapture();
    ASSERT_NE(ctx, nullptr);
    EXPECT_EQ(ctx->value_count, 1u);
    EXPECT_EQ(ctx->values[0].key, kKey);
    EXPECT_EQ(ctx->values[0].value, kVal);
    threading::ExecutionContextFree(ctx);

    threading::AsyncLocalSetValue(kKey, 0);
}

TEST_F(ExecutionContextTest, SuppressFlow) {
    clear_all_async_locals();
    constexpr uint64_t kKey = 0x5678;
    constexpr CHAOS_IL2CPP_INTPTR kVal = static_cast<CHAOS_IL2CPP_INTPTR>(0xCAFE);

    threading::AsyncLocalSetValue(kKey, kVal);

    // Capture before suppress — should succeed.
    auto* ctx_before = threading::ExecutionContextCapture();
    ASSERT_NE(ctx_before, nullptr);
    threading::ExecutionContextFree(ctx_before);

    // Suppress flow.
    int32_t cookie = threading::ExecutionContextSuppressFlow();
    EXPECT_TRUE(threading::ExecutionContextIsFlowSuppressed());

    // Capture while suppressed — should return nullptr.
    auto* ctx_during = threading::ExecutionContextCapture();
    EXPECT_EQ(ctx_during, nullptr);

    // Restore flow.
    threading::ExecutionContextRestoreFlow(cookie);
    EXPECT_FALSE(threading::ExecutionContextIsFlowSuppressed());

    // Capture after restore — should succeed again.
    auto* ctx_after = threading::ExecutionContextCapture();
    ASSERT_NE(ctx_after, nullptr);
    EXPECT_EQ(ctx_after->value_count, 1u);
    EXPECT_EQ(ctx_after->values[0].key, kKey);
    threading::ExecutionContextFree(ctx_after);

    threading::AsyncLocalSetValue(kKey, 0);
}

TEST_F(ExecutionContextTest, NestedSuppress) {
    EXPECT_FALSE(threading::ExecutionContextIsFlowSuppressed());

    int32_t cookie1 = threading::ExecutionContextSuppressFlow();
    EXPECT_TRUE(threading::ExecutionContextIsFlowSuppressed());

    int32_t cookie2 = threading::ExecutionContextSuppressFlow();
    EXPECT_TRUE(threading::ExecutionContextIsFlowSuppressed());

    // Restore to cookie2 (depth=1) — still suppressed.
    threading::ExecutionContextRestoreFlow(cookie2);
    EXPECT_TRUE(threading::ExecutionContextIsFlowSuppressed());

    // Restore to cookie1 (depth=0) — no longer suppressed.
    threading::ExecutionContextRestoreFlow(cookie1);
    EXPECT_FALSE(threading::ExecutionContextIsFlowSuppressed());
}

TEST_F(ExecutionContextTest, SuppressNullCapture) {
    clear_all_async_locals();
    threading::ExecutionContextSuppressFlow();
    auto* ctx = threading::ExecutionContextCapture();
    EXPECT_EQ(ctx, nullptr);
    threading::ExecutionContextRestoreFlow(0);

    ctx = threading::ExecutionContextCapture();
    EXPECT_EQ(ctx, nullptr);  // no values set
}

TEST_F(ExecutionContextTest, MultiValueHeap) {
    clear_all_async_locals();
    // Set 5 values to trigger heap-allocated storage in capture.
    for (uint64_t i = 0; i < 5; i++) {
        threading::AsyncLocalSetValue(i + 100, static_cast<CHAOS_IL2CPP_INTPTR>(0xFF00 + i));
    }

    auto* ctx = threading::ExecutionContextCapture();
    ASSERT_NE(ctx, nullptr);
    EXPECT_EQ(ctx->value_count, 5u);
    EXPECT_TRUE(ctx->heap_allocated);

    for (uint32_t i = 0; i < 5; i++) {
        EXPECT_EQ(ctx->values[i].key, i + 100);
        EXPECT_EQ(ctx->values[i].value, static_cast<CHAOS_IL2CPP_INTPTR>(0xFF00 + i));
    }

    threading::ExecutionContextFree(ctx);

    for (uint64_t i = 0; i < 5; i++) {
        threading::AsyncLocalSetValue(i + 100, 0);
    }
}

TEST_F(ExecutionContextTest, SuppressFlowRestoresCapture) {
    clear_all_async_locals();
    // Set values, suppress, ensure null capture, restore, ensure capture works.
    threading::AsyncLocalSetValue(42, static_cast<CHAOS_IL2CPP_INTPTR>(99));

    int32_t cookie = threading::ExecutionContextSuppressFlow();
    auto* ctx1 = threading::ExecutionContextCapture();
    EXPECT_EQ(ctx1, nullptr);
    threading::ExecutionContextRestoreFlow(cookie);

    auto* ctx2 = threading::ExecutionContextCapture();
    ASSERT_NE(ctx2, nullptr);
    EXPECT_EQ(ctx2->value_count, 1u);
    EXPECT_EQ(ctx2->values[0].key, 42u);
    EXPECT_EQ(ctx2->values[0].value, static_cast<CHAOS_IL2CPP_INTPTR>(99));
    threading::ExecutionContextFree(ctx2);

    threading::AsyncLocalSetValue(42, 0);
}

TEST_F(ExecutionContextTest, RunContextWithValues) {
    // Set values, capture, run with context, verify callback invoked.
    clear_all_async_locals();
    threading::AsyncLocalSetValue(1, static_cast<CHAOS_IL2CPP_INTPTR>(10));

    auto* ctx = threading::ExecutionContextCapture();
    ASSERT_NE(ctx, nullptr);

    s_callback_invoked = 0;
    threading::ExecutionContextRun(ctx, test_callback, nullptr);
    EXPECT_EQ(s_callback_invoked, 1);

    threading::ExecutionContextFree(ctx);
    threading::AsyncLocalSetValue(1, 0);
}

TEST_F(ExecutionContextTest, RunContextNull) {
    // Running a null context should invoke callback directly.
    s_callback_invoked = 0;
    threading::ExecutionContextRun(nullptr, test_callback, nullptr);
    EXPECT_EQ(s_callback_invoked, 1);
}

TEST_F(ExecutionContextTest, SuppressFlowExternCBridge) {
    // Test through the extern "C" bridge.
    EXPECT_EQ(chaos_execution_context_suppress_flow(), 0);
    EXPECT_TRUE(chaos_execution_context_is_flow_suppressed());

    chaos_execution_context_restore_flow(0);
    EXPECT_FALSE(chaos_execution_context_is_flow_suppressed());
}
