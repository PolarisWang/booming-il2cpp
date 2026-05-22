// execution_context_smoke.cpp — ExecutionContext capture, SuppressFlow/RestoreFlow
//
// Tests:
//   1. Capture with no AsyncLocal values → nullptr
//   2. Set 1 value → capture non-null, verify value
//   3. SuppressFlow → capture returns nullptr → RestoreFlow → capture returns non-null
//   4. Nested SuppressFlow (depth tracking)
//   5. IsFlowSuppressed
//   6. Set 5+ values → capture with heap-allocated storage

#include "execution_context.h"

#include <cstdio>
#include <cstdlib>

namespace threading = chaos::il2cpp::runtime_core::threading;

static int s_test_count = 0;
static int s_pass_count = 0;

#define TEST(name) do { s_test_count++; \
    std::printf("  TEST %s ... ", name); \
    if (!test_##name()) { std::printf("FAIL\n"); return false; } \
    std::printf("OK\n"); s_pass_count++; } while(0)

#define ASSERT(cond, msg) do { \
    if (!(cond)) { std::fprintf(stderr, "FAIL: %s\n", msg); return false; } \
} while(0)

// ── Tests ───────────────────────────────────────────────────────────────

static bool test_empty_capture() {
    auto* ctx = threading::ExecutionContextCapture();
    ASSERT(ctx == nullptr, "expected null capture with no AsyncLocals");
    return true;
}

static bool test_single_value() {
    constexpr uint64_t kKey = 0x1234;
    constexpr CHAOS_IL2CPP_INTPTR kVal = static_cast<CHAOS_IL2CPP_INTPTR>(0xDEAD);

    threading::AsyncLocalSetValue(kKey, kVal);

    auto* ctx = threading::ExecutionContextCapture();
    ASSERT(ctx != nullptr, "expected non-null capture");
    ASSERT(ctx->value_count == 1, "expected 1 value");
    ASSERT(ctx->values[0].key == kKey, "key mismatch");
    ASSERT(ctx->values[0].value == kVal, "value mismatch");

    threading::ExecutionContextFree(ctx);

    // Cleanup
    threading::AsyncLocalSetValue(kKey, 0);
    return true;
}

static bool test_suppress_flow() {
    constexpr uint64_t kKey = 0x5678;
    constexpr CHAOS_IL2CPP_INTPTR kVal = static_cast<CHAOS_IL2CPP_INTPTR>(0xCAFE);

    threading::AsyncLocalSetValue(kKey, kVal);

    // Capture before suppress — should succeed.
    auto* ctx_before = threading::ExecutionContextCapture();
    ASSERT(ctx_before != nullptr, "expected non-null before suppress");
    threading::ExecutionContextFree(ctx_before);

    // Suppress flow.
    int32_t cookie = threading::ExecutionContextSuppressFlow();
    ASSERT(threading::ExecutionContextIsFlowSuppressed(), "expected suppressed");

    // Capture while suppressed — should return nullptr.
    auto* ctx_during = threading::ExecutionContextCapture();
    ASSERT(ctx_during == nullptr, "expected null capture while suppressed");

    // Restore flow.
    threading::ExecutionContextRestoreFlow(cookie);
    ASSERT(!threading::ExecutionContextIsFlowSuppressed(), "expected not suppressed");

    // Capture after restore — should succeed again.
    auto* ctx_after = threading::ExecutionContextCapture();
    ASSERT(ctx_after != nullptr, "expected non-null after restore");
    ASSERT(ctx_after->value_count == 1, "expected 1 value");
    ASSERT(ctx_after->values[0].key == kKey, "key mismatch");
    threading::ExecutionContextFree(ctx_after);

    // Cleanup
    threading::AsyncLocalSetValue(kKey, 0);
    return true;
}

static bool test_nested_suppress() {
    int32_t cookie1 = threading::ExecutionContextSuppressFlow();
    ASSERT(threading::ExecutionContextIsFlowSuppressed(), "expected suppressed after 1st");

    int32_t cookie2 = threading::ExecutionContextSuppressFlow();
    ASSERT(threading::ExecutionContextIsFlowSuppressed(), "expected suppressed after 2nd");

    // Restore to cookie2 (depth=1).
    threading::ExecutionContextRestoreFlow(cookie2);
    ASSERT(threading::ExecutionContextIsFlowSuppressed(), "expected still suppressed after partial restore");

    // Restore to cookie1 (depth=0).
    threading::ExecutionContextRestoreFlow(cookie1);
    ASSERT(!threading::ExecutionContextIsFlowSuppressed(), "expected not suppressed after full restore");
    return true;
}

static bool test_suppress_null_capture() {
    // Suppress → capture null → restore — verifies the flow works via extern "C" path.
    threading::ExecutionContextSuppressFlow();
    auto* ctx = threading::ExecutionContextCapture();
    ASSERT(ctx == nullptr, "expected null capture while suppressed");
    threading::ExecutionContextRestoreFlow(0);

    ctx = threading::ExecutionContextCapture();
    ASSERT(ctx == nullptr, "expected null capture with no values after restore");
    return true;
}

static bool test_multi_value_heap() {
    // Set 5 values to trigger heap-allocated storage in capture.
    for (uint64_t i = 0; i < 5; i++) {
        threading::AsyncLocalSetValue(i + 100, static_cast<CHAOS_IL2CPP_INTPTR>(0xFF00 + i));
    }

    auto* ctx = threading::ExecutionContextCapture();
    ASSERT(ctx != nullptr, "expected non-null capture");
    ASSERT(ctx->value_count == 5, "expected 5 values");
    ASSERT(ctx->heap_allocated, "expected heap-allocated for 5 values");

    for (uint32_t i = 0; i < 5; i++) {
        ASSERT(ctx->values[i].key == i + 100, "key mismatch");
        ASSERT(ctx->values[i].value == static_cast<CHAOS_IL2CPP_INTPTR>(0xFF00 + i), "value mismatch");
    }

    threading::ExecutionContextFree(ctx);

    // Cleanup
    for (uint64_t i = 0; i < 5; i++) {
        threading::AsyncLocalSetValue(i + 100, 0);
    }
    return true;
}

// ── Main ─────────────────────────────────────────────────────────────────

int main() {
    std::printf("execution_context_smoke:\n");

    bool all_pass = true;
    all_pass &= test_empty_capture();
    all_pass &= test_single_value();
    all_pass &= test_suppress_flow();
    all_pass &= test_nested_suppress();
    all_pass &= test_suppress_null_capture();
    all_pass &= test_multi_value_heap();

    if (all_pass) {
        std::printf("execution_context_smoke OK (%d/%d)\n", s_pass_count, s_test_count);
        return EXIT_SUCCESS;
    } else {
        std::printf("execution_context_smoke FAILED (%d/%d)\n", s_pass_count, s_test_count);
        return EXIT_FAILURE;
    }
}
