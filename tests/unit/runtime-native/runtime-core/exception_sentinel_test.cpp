// ── Exception sentinel propagation test ─────────────────────────────────
//
// Verifies that ThreadAbort/ThreadInterrupt sentinel values are correctly
// defined and that the sentinel guard logic (re-raise when object_value < 0)
// works as expected in CPP_THROW mode.
//
// The sentinel guard is emitted by StructuredIR.cs in codegen output:
//   if (chaos_exception.object_value < 0) { throw; }
//
// This test validates:
//   1. Sentinel constants have correct values
//   2. A sentinel throw propagates past typed catch handlers
//   3. A normal (non-sentinel) throw is caught by the correct handler

#include <gtest/gtest.h>
#include <cstdlib>
#include <cstdio>

// The sentinel constants are defined here.
#include <generated_code_compat.h>

// chaos_raise_exception for CPP_THROW mode.
#include <exception_jmp.h>

// ── Test 1: Sentinel constants ──────────────────────────────────────────

TEST(ExceptionSentinel, Constants)
{
    EXPECT_EQ(kManagedExceptionNormal, 0);
    EXPECT_EQ(kManagedExceptionThreadAbort,
              static_cast<CHAOS_IL2CPP_INTPTR>(-1));
    EXPECT_EQ(kManagedExceptionThreadInterrupt,
              static_cast<CHAOS_IL2CPP_INTPTR>(-2));
}

// ── Test 2: Sentinel < 0 check ─────────────────────────────────────────

TEST(ExceptionSentinel, SentinelLessThanZero)
{
    // This mirrors the codegen guard:
    //   if (chaos_exception.object_value < 0) { throw; }
    EXPECT_LT(kManagedExceptionThreadAbort, 0);
    EXPECT_LT(kManagedExceptionThreadInterrupt, 0);
    EXPECT_GE(kManagedExceptionNormal, 0);
}

// ── Tests 3-5: CPP_THROW propagation ───────────────────────────────────

#if defined(CHAOS_IL2CPP_EH_CPP_THROW)

// A helper that simulates the codegen pattern:
//   try { ... }
//   catch (const chaos_managed_exception& e) {
//       if (e.object_value < 0) { throw; }  // sentinel guard
//       ... typed catch logic ...
//   }

TEST(ExceptionSentinel, SentinelRethrowsPastTypedCatch)
{
    bool caught_by_typed_handler = false;
    bool sentinel_rethrown = false;

    // Sentinel re-throws past typed catch (ThreadAbort)
    try {
        throw chaos_managed_exception{kManagedExceptionThreadAbort};
    } catch (const chaos_managed_exception& e) {
        if (e.object_value < 0) {
            // Sentinel guard: re-raise
            sentinel_rethrown = true;
            throw;
        }
        caught_by_typed_handler = true;
    } catch (...) {
        // Outer catch -- sentinel should arrive here
    }

    EXPECT_TRUE(sentinel_rethrown);
    EXPECT_FALSE(caught_by_typed_handler);
}

TEST(ExceptionSentinel, NormalExceptionCaughtByTypedHandler)
{
    bool caught_by_typed_handler = false;
    bool sentinel_rethrown = false;

    try {
        throw chaos_managed_exception{kManagedExceptionNormal};
    } catch (const chaos_managed_exception& e) {
        if (e.object_value < 0) {
            sentinel_rethrown = true;
            throw;
        }
        caught_by_typed_handler = true;
    }

    EXPECT_TRUE(caught_by_typed_handler);
    EXPECT_FALSE(sentinel_rethrown);
}

TEST(ExceptionSentinel, RaiseExceptionCompiles)
{
    bool caught = false;
    try {
        chaos::il2cpp::runtime_core::chaos_raise_exception(42);
    } catch (const chaos_managed_exception&) {
        caught = true;
    }
    EXPECT_TRUE(caught);
}

#else  // !CHAOS_IL2CPP_EH_CPP_THROW

TEST(ExceptionSentinel, SentinelRethrowsPastTypedCatch)
{
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_CPP_THROW";
}

TEST(ExceptionSentinel, NormalExceptionCaughtByTypedHandler)
{
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_CPP_THROW";
}

TEST(ExceptionSentinel, RaiseExceptionCompiles)
{
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_CPP_THROW";
}

#endif  // CHAOS_IL2CPP_EH_CPP_THROW
