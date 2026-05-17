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

#include <cstdlib>
#include <cstdio>

// The sentinel constants are defined here.
#include <generated_code_compat.h>

// chaos_raise_exception for CPP_THROW mode.
#include <exception_jmp.h>

// ── Helpers ─────────────────────────────────────────────────────────────

static int g_tests_run = 0;
static int g_tests_passed = 0;

#define TEST(name) do { ++g_tests_run; std::printf("  TEST: %s ... ", name); } while(0)
#define PASS() do { ++g_tests_passed; std::puts("PASS"); } while(0)
#define FAIL(msg) do { std::puts("FAIL"); std::printf("    %s\n", msg); } while(0)

// ── Test 1: Sentinel constants ──────────────────────────────────────────

static void test_sentinel_constants() {
    TEST("kManagedExceptionNormal == 0");
    if (kManagedExceptionNormal == 0) { PASS(); }
    else { FAIL("expected 0"); }

    TEST("kManagedExceptionThreadAbort == -1");
    if (kManagedExceptionThreadAbort == static_cast<CHAOS_IL2CPP_INTPTR>(-1)) { PASS(); }
    else { FAIL("expected -1"); }

    TEST("kManagedExceptionThreadInterrupt == -2");
    if (kManagedExceptionThreadInterrupt == static_cast<CHAOS_IL2CPP_INTPTR>(-2)) { PASS(); }
    else { FAIL("expected -2"); }
}

// ── Test 2: Sentinel < 0 check ─────────────────────────────────────────

static void test_sentinel_less_than_zero() {
    // This mirrors the codegen guard: if (chaos_exception.object_value < 0) { throw; }
    // We verify that sentinel values correctly trigger the < 0 check.
    TEST("ThreadAbort sentinel < 0");
    if (kManagedExceptionThreadAbort < 0) { PASS(); }
    else { FAIL("sentinels must be < 0"); }

    TEST("ThreadInterrupt sentinel < 0");
    if (kManagedExceptionThreadInterrupt < 0) { PASS(); }
    else { FAIL("sentinels must be < 0"); }

    TEST("Normal exception >= 0");
    if (kManagedExceptionNormal >= 0) { PASS(); }
    else { FAIL("normal exceptions must be >= 0"); }
}

// ── Test 3: Sentinel throw propagates past typed catch (CPP_THROW) ────

// A helper that simulates the codegen pattern:
//   try { ... }
//   catch (const chaos_managed_exception& e) {
//       if (e.object_value < 0) { throw; }  // sentinel guard
//       ... typed catch logic ...
//   }

static bool caught_by_typed_handler = false;
static bool sentinel_rethrown = false;

static void test_sentinel_typed_catch_propagation() {
    caught_by_typed_handler = false;
    sentinel_rethrown = false;

    TEST("Sentinel re-throws past typed catch (ThreadAbort)");
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
        // Outer catch — sentinel should arrive here
    }

    if (sentinel_rethrown && !caught_by_typed_handler) { PASS(); }
    else { FAIL("sentinel should re-throw and not be caught by typed handler"); }
}

// ── Test 4: Normal exception is caught by typed handler ────────────────

static void test_normal_exception_caught_by_typed_handler() {
    caught_by_typed_handler = false;
    sentinel_rethrown = false;

    TEST("Normal exception caught by typed handler");
    try {
        throw chaos_managed_exception{kManagedExceptionNormal};
    } catch (const chaos_managed_exception& e) {
        if (e.object_value < 0) {
            sentinel_rethrown = true;
            throw;
        }
        caught_by_typed_handler = true;
    }

    if (caught_by_typed_handler && !sentinel_rethrown) { PASS(); }
    else { FAIL("normal exception should be caught by typed handler"); }
}

// ── Test 5: chaos_raise_exception compiles and links ───────────────────
// For CPP_THROW mode, chaos_raise_exception does `throw chaos_managed_exception{obj}`.
// We verify it compiles and links by calling it inside a try/catch.

static void test_raise_exception_compiles() {
    TEST("chaos_raise_exception with normal value");
    bool caught = false;
    try {
        chaos::il2cpp::runtime_core::chaos_raise_exception(42);
    } catch (const chaos_managed_exception&) {
        caught = true;
    }
    if (caught) { PASS(); }
    else { FAIL("chaos_raise_exception should throw"); }
}

// ── Main ────────────────────────────────────────────────────────────────

int main() {
    std::puts("=== Exception Sentinel Test ===");
    std::puts("");

    test_sentinel_constants();
    test_sentinel_less_than_zero();

#if defined(CHAOS_IL2CPP_EH_CPP_THROW)
    test_sentinel_typed_catch_propagation();
    test_normal_exception_caught_by_typed_handler();
    test_raise_exception_compiles();
#else
    std::puts("  SKIP: EH propagation tests require CHAOS_IL2CPP_EH_CPP_THROW");
#endif

    std::puts("");
    std::printf("Results: %d / %d passed\n", g_tests_passed, g_tests_run);

    return (g_tests_passed == g_tests_run) ? EXIT_SUCCESS : EXIT_FAILURE;
}
