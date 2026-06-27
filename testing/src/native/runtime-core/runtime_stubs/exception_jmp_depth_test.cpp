// exception_jmp_depth_test.cpp — Unit tests for exception_jmp.h SETJMP bounds
//
// Tests push_exception_jmp_buf / pop_exception_jmp_buf depth boundaries:
//   Depth1  — single try-catch works
//   Depth16 — legacy limit (16) works correctly
//   Depth32 — between legacy and new limit
//   Depth64 — new limit (64) edge — deepest allowed
//   Depth65 — overflow: push at depth 64 should print warning + abort
//
// These tests verify the CHAOS_IL2CPP_EH_SETJMP path specifically.
// The CHAOS_EH_TRY/CHAOS_EH_CATCH macros expand to push/setjmp/pop when
// CHAOS_IL2CPP_EH_SETJMP is defined.
//
// NOTE: On Windows with MSVC, the default EH mode is WIN32_SEH. To compile
// this test in SETJMP mode, define CHAOS_IL2CPP_EH_SETJMP before including
// <chaos/common.h> (which includes config.h that auto-selects WIN32_SEH).
// This test forces SETJMP to verify the jmp_buf stack boundary.

// Force SETJMP mode BEFORE any includes that pull in config.h
#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH) && !defined(CHAOS_IL2CPP_EH_CPP_THROW)
#  define CHAOS_IL2CPP_EH_SETJMP 1
#endif

#include <gtest/gtest.h>

#include <chaos/common.h>
#include <csetjmp>

// Direct include for exception_jmp.h (not transitively included by common.h)
#include "runtime-core/exception_jmp.h"

// ── TLS definitions required by exception_jmp.h ───────────────────────
// These are normally defined in exception_helpers.cpp (runtime-core lib).
// Since unit tests compile .cpp directly instead of linking the lib,
// we provide the TLS symbols here.
thread_local jmp_buf chaos::il2cpp::runtime_core::g_chaos_exception_jmp_stack[
    chaos::il2cpp::runtime_core::kMaxNestedTry] = {};
thread_local int chaos::il2cpp::runtime_core::g_chaos_exception_jmp_depth = 0;
thread_local void* volatile chaos::il2cpp::runtime_core::g_chaos_exception_obj = nullptr;

// ── Stubs: the managed exception types aren't needed for push/pop tests ──
// push/pop are inline in exception_jmp.h and don't depend on managed objects.

// ═══════════════════════════════════════════════════════════════════════
// Depth boundary tests: verify push/pop at key thresholds
// ═══════════════════════════════════════════════════════════════════════

namespace {

/// Helper: push N times then pop N times, expecting no crash.
/// Returns the depth after pushes (observed from TLS).
static int push_pop_n(int n) noexcept {
    using namespace chaos::il2cpp::runtime_core;
    for (int i = 0; i < n; i++) {
        push_exception_jmp_buf();
    }
    int depth = g_chaos_exception_jmp_depth;
    for (int i = 0; i < n; i++) {
        pop_exception_jmp_buf();
    }
    return depth;
}

} // anonymous namespace

// ── Depth 1: single try-catch ─────────────────────────────────────────
TEST(ExceptionJmpDepth, Depth1) {
    using namespace chaos::il2cpp::runtime_core;
    ASSERT_EQ(g_chaos_exception_jmp_depth, 0);
    EXPECT_EQ(push_pop_n(1), 1);
    ASSERT_EQ(g_chaos_exception_jmp_depth, 0);
}

// ── Depth half_limit: kMaxNestedTry/2 ─────────────────────────────────
TEST(ExceptionJmpDepth, DepthHalfLimit) {
    using namespace chaos::il2cpp::runtime_core;
    constexpr int kDepth = kMaxNestedTry / 2;
    ASSERT_EQ(g_chaos_exception_jmp_depth, 0);
    EXPECT_EQ(push_pop_n(kDepth), kDepth);
    ASSERT_EQ(g_chaos_exception_jmp_depth, 0);
}

// ── Depth at_limit: kMaxNestedTry — deepest allowed ───────────────────
TEST(ExceptionJmpDepth, DepthAtLimit) {
    using namespace chaos::il2cpp::runtime_core;
    ASSERT_EQ(g_chaos_exception_jmp_depth, 0);
    EXPECT_EQ(push_pop_n(kMaxNestedTry), kMaxNestedTry);
    ASSERT_EQ(g_chaos_exception_jmp_depth, 0);
}

// ── Depth overflow: push at depth kMaxNestedTry triggers abort ─────────
// The push_exception_jmp_buf function prints a fatal error and calls
// std::abort() when depth >= kMaxNestedTry.  Since abort is
// [[noreturn]], we can only test that the first 64 pushes succeed and
// that the 65th push terminates (death test).
//
// Death test: EXPECT_DEATH checks that the process terminates from abort.
// Must run LAST in the suite because abort kills the process.
TEST(ExceptionJmpDepth, DepthOverflow) {
    using namespace chaos::il2cpp::runtime_core;

    // Push up to the limit — these should all succeed
    for (int i = 0; i < kMaxNestedTry; i++) {
        push_exception_jmp_buf();
    }
    EXPECT_EQ(g_chaos_exception_jmp_depth, kMaxNestedTry);

    // The (kMaxNestedTry+1)th push must abort (death test)
    EXPECT_DEATH(
        push_exception_jmp_buf(),
        ".*"
    );
}

// ── Pop underflow: pop when depth == 0 should warn (not abort) ────────
TEST(ExceptionJmpDepth, PopUnderflow) {
    using namespace chaos::il2cpp::runtime_core;
    ASSERT_EQ(g_chaos_exception_jmp_depth, 0);

    // Pop when depth is already 0: should print warning and return without crashing
    // We can't easily capture stderr in gtest, but we verify no crash/abort occurs.
    pop_exception_jmp_buf();
    EXPECT_EQ(g_chaos_exception_jmp_depth, 0);

    // Multiple pops should also not crash
    pop_exception_jmp_buf();
    pop_exception_jmp_buf();
    EXPECT_EQ(g_chaos_exception_jmp_depth, 0);
}
