// ── EH mode-equivalence tests ────────────────────────────────────────────
//
// WHY THIS EXISTS
// ---------------
// The CHAOS_EH_* macros (src/native/common/chaos/eh.h) have three completely
// different expansions:
//
//   default (CPP_THROW) → try / catch (const chaos_managed_exception&)
//   CHAOS_IL2CPP_EH_WIN32_SEH → __try / __except (CHAOS_SEH_FILTER_ALL())
//   CHAOS_IL2CPP_EH_SETJMP    → setjmp slot push / longjmp landing
//
// Everything that raises or catches a managed exception is supposed to go
// through these macros, so the three expansions MUST be behaviourally
// equivalent.  Historically nothing enforced that: generated subject wrappers
// hardcoded a C++ try/catch, which only fires in the default mode — under SEH
// and SETJMP the exception sailed straight past the wrapper.  That went
// unnoticed for a long time and produced ~566 preAssertionRaise records.
//
// This suite is the enforcement.  It is compiled THREE TIMES against the same
// source (see CMakeLists.txt), once per mode, and every case asserts an
// outcome that is mode-independent.  A regression in any one expansion turns
// exactly one of the three targets red.
//
// DESIGN RULES (do not relax)
// ---------------------------
//  * Assert on *observable behaviour*, never on which mechanism was used.
//    A case that only passes under CPP_THROW is a bug in the test, not a pass.
//  * Every "must catch" case is paired with a "must NOT catch" case.  A
//    positive-only suite cannot tell a working matcher from one that always
//    returns true — see the guard-test discipline in the project memory.
//  * No C++ objects with destructors inside CHAOS_EH_TRY.  Under SETJMP a
//    longjmp skips destructors (iron rule 5), and under SEH an unwinding
//    object in the same TU is a hard C2712 error (iron rule 2).

// Include order matters and mirrors tests/contracts/native/runtime-core/
// exception_sentinel_test.cpp:
//   generated_code_compat.h → chaos_managed_exception (named by the CPP_THROW
//                             expansion of CHAOS_EH_CATCH_BEGIN / CHAOS_EH_THROW)
//   exception_jmp.h         → g_chaos_exception_obj + push/pop_exception_jmp_buf
//                             + chaos_raise_exception (all three modes)
//   chaos/eh.h              → the CHAOS_EH_* macros themselves
#include <generated_code_compat.h>
#include <exception_jmp.h>
#include <chaos/native_types.h>
#include <chaos/eh.h>

#include <cstdio>
#include <cstring>

// ── Minimal test harness (matches the sibling stubs tests) ───────────────

static int g_tests_run = 0;
static int g_tests_passed = 0;

#define TEST(name) do { ++g_tests_run; std::printf("  TEST: %s ... ", name); } while (0)
#define PASS()     do { ++g_tests_passed; std::puts("PASS"); } while (0)
#define FAIL(msg)  do { std::puts("FAIL"); std::printf("    %s\n", msg); } while (0)
#define CHECK(cond, msg) do { if (cond) { PASS(); } else { FAIL(msg); } } while (0)

// ── Raise helpers ────────────────────────────────────────────────────────
//
// chaos_raise_exception is the single delivery primitive across all three
// modes (see exception_jmp.h).  The value raised is an opaque CHAOS_IL2CPP_INTPTR
// that stands in for the managed exception object; this suite never
// dereferences it, so a sentinel integer is sufficient and keeps the test
// free of the TypeInfo machinery.

static const CHAOS_IL2CPP_INTPTR kRaiseValue = 0x1234;

[[noreturn]] static void RaiseSentinel() {
    chaos::il2cpp::runtime_core::chaos_raise_exception(kRaiseValue);
}

// Tracks whether a handler body actually ran, so a case can distinguish
// "caught" from "fell through without raising".
static bool g_handler_ran = false;

// ── Case 1: a raised exception is caught at all ──────────────────────────
// The most basic equivalence property: whatever the mechanism, a raise
// inside CHAOS_EH_TRY must transfer control to the CHAOS_EH_CATCH_BEGIN
// handler.  Under the old hardcoded-try/catch wrappers this is exactly the
// case that failed on Windows/iOS.
static void test_raise_is_caught() {
    TEST("raise inside CHAOS_EH_TRY reaches the handler");
    g_handler_ran = false;
    CHAOS_EH_TRY
        RaiseSentinel();
    CHAOS_EH_CATCH_BEGIN
        g_handler_ran = true;
    CHAOS_EH_END
    CHECK(g_handler_ran, "handler did not run — raise escaped the CHAOS_EH_TRY block");
}

// ── Case 2: no raise ⇒ handler must NOT run ──────────────────────────────
// The negative control for case 1.  Without this, a macro expansion that
// runs the handler unconditionally would still pass case 1.
static void test_no_raise_skips_handler() {
    TEST("no raise ⇒ handler does not run (negative control)");
    g_handler_ran = false;
    CHAOS_EH_TRY
        // deliberately raise nothing
        volatile int nothing = 0;
        (void)nothing;
    CHAOS_EH_CATCH_BEGIN
        g_handler_ran = true;
    CHAOS_EH_END
    CHECK(!g_handler_ran, "handler ran without any raise — expansion is not conditional");
}

// ── Case 3: the raised value survives delivery ───────────────────────────
// The exception object must arrive at the handler intact.  Under SETJMP the
// value travels through a TLS slot (longjmp may clobber registers), so this
// catches a missing/incorrect TLS hand-off.
static void test_raise_value_survives() {
    TEST("raised value is delivered intact to the handler");
    CHAOS_IL2CPP_INTPTR observed = 0;
    CHAOS_EH_TRY
        RaiseSentinel();
    CHAOS_EH_CATCH_BEGIN
        observed = CHAOS_EH_EXCEPTION_OBJ;
    CHAOS_EH_END
    CHECK(observed == kRaiseValue, "handler saw a different value — TLS hand-off is broken");
}

// ── Case 4: nested CHAOS_EH_TRY — inner handler wins ─────────────────────
// SETJMP keeps a *stack* of jmp_buf slots, so nesting must land on the
// innermost frame.  A LIFO bug would deliver to the outer handler.
static void test_nested_inner_handler_wins() {
    TEST("nested try: innermost handler receives the raise");
    int inner_hits = 0;
    int outer_hits = 0;
    CHAOS_EH_TRY
        CHAOS_EH_TRY
            RaiseSentinel();
        CHAOS_EH_CATCH_BEGIN
            ++inner_hits;
        CHAOS_EH_END
    CHAOS_EH_CATCH_BEGIN
        ++outer_hits;
    CHAOS_EH_END
    CHECK(inner_hits == 1 && outer_hits == 0,
          "expected inner-only handling (inner=1, outer=0)");
}

// ── Case 5: nested try — an outer raise is caught by the outer handler ────
// Complement to case 4: proves the outer frame is still live and correctly
// bound after the inner frame is left, i.e. the pop/push discipline holds.
static void test_nested_outer_handler_still_live() {
    TEST("nested try: outer handler still catches after inner scope exits");
    int inner_hits = 0;
    int outer_hits = 0;
    CHAOS_EH_TRY
        CHAOS_EH_TRY
            volatile int nothing = 0;
            (void)nothing;
        CHAOS_EH_CATCH_BEGIN
            ++inner_hits;
        CHAOS_EH_END
        RaiseSentinel();
    CHAOS_EH_CATCH_BEGIN
        ++outer_hits;
    CHAOS_EH_END
    CHECK(outer_hits == 1 && inner_hits == 0,
          "outer handler failed to catch after the inner scope exited");
}

// ── Case 6: rethrow propagates to the enclosing handler ──────────────────
// CHAOS_EH_RETHROW must hand the exception to the next enclosing frame.
// This is what the subject wrapper does for uncaught managed exceptions, so
// a broken rethrow silently converts "method threw" into "method passed".
static void test_rethrow_reaches_outer() {
    TEST("CHAOS_EH_RETHROW propagates to the enclosing handler");
    int inner_hits = 0;
    int outer_hits = 0;
    CHAOS_EH_TRY
        CHAOS_EH_TRY
            RaiseSentinel();
        CHAOS_EH_CATCH_BEGIN
            ++inner_hits;
            CHAOS_EH_RETHROW;
        CHAOS_EH_END
    CHAOS_EH_CATCH_BEGIN
        ++outer_hits;
    CHAOS_EH_END
    CHECK(inner_hits == 1 && outer_hits == 1,
          "rethrow did not reach the enclosing handler (inner=1, outer=1 expected)");
}

// ── Case 7: raising from a callee (non-inlined call boundary) ────────────
// Generated code raises from *inside* helper calls, not from the try body
// directly.  Under SEH a raise crossing a callee boundary must still unwind
// to the __except; under SETJMP it must longjmp across the frame.
static void CalleeThatRaises() {
    RaiseSentinel();
}

static void test_raise_across_call_boundary() {
    TEST("raise from a callee is caught by the caller's handler");
    g_handler_ran = false;
    CHAOS_EH_TRY
        CalleeThatRaises();
    CHAOS_EH_CATCH_BEGIN
        g_handler_ran = true;
    CHAOS_EH_END
    CHECK(g_handler_ran, "raise did not cross the call boundary into the handler");
}

// ── Case 8: deep nesting (SETJMP stack depth) ────────────────────────────
// The SETJMP expansion uses a fixed-depth TLS jmp_buf stack
// (kMaxNestedTry).  Exercising a realistic depth here means a stack-depth
// arithmetic error shows up as a test failure rather than an abort() in
// production code that happens to nest one frame deeper.
static void test_deep_nesting_delivers_to_innermost() {
    TEST("8-deep nesting delivers to the innermost handler");
    int depth_hits[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    CHAOS_EH_TRY
    CHAOS_EH_TRY
    CHAOS_EH_TRY
    CHAOS_EH_TRY
    CHAOS_EH_TRY
    CHAOS_EH_TRY
    CHAOS_EH_TRY
    CHAOS_EH_TRY
        RaiseSentinel();
    CHAOS_EH_CATCH_BEGIN
        ++depth_hits[7];
    CHAOS_EH_END
    CHAOS_EH_CATCH_BEGIN
        ++depth_hits[6];
    CHAOS_EH_END
    CHAOS_EH_CATCH_BEGIN
        ++depth_hits[5];
    CHAOS_EH_END
    CHAOS_EH_CATCH_BEGIN
        ++depth_hits[4];
    CHAOS_EH_END
    CHAOS_EH_CATCH_BEGIN
        ++depth_hits[3];
    CHAOS_EH_END
    CHAOS_EH_CATCH_BEGIN
        ++depth_hits[2];
    CHAOS_EH_END
    CHAOS_EH_CATCH_BEGIN
        ++depth_hits[1];
    CHAOS_EH_END
    CHAOS_EH_CATCH_BEGIN
        ++depth_hits[0];
    CHAOS_EH_END
    CHECK(depth_hits[7] == 1, "innermost handler did not receive the raise");
}

// ── Case 9: sequential (non-nested) trys reuse slots correctly ───────────
// After a try block completes, its SETJMP slot must be released.  Reusing
// the macro repeatedly is the common case in generated code; a slot leak
// would exhaust kMaxNestedTry and abort.
static void test_sequential_trys() {
    TEST("many sequential CHAOS_EH_TRY blocks do not leak slots");
    int caught = 0;
    for (int i = 0; i < 64; ++i) {
        CHAOS_EH_TRY
            RaiseSentinel();
        CHAOS_EH_CATCH_BEGIN
            ++caught;
        CHAOS_EH_END
    }
    CHECK(caught == 64, "not every sequential try caught its raise — slot leak suspected");
}

// ── Case 10: handler for a non-matching type must not run ────────────────
// The negative control that matters most for the type-matching work (P2):
// a handler that filters on a type the exception does NOT have must decline.
// Implemented with a hand-written filter so this case is independent of the
// TypeInfo machinery — it tests the *filter contract*, not type resolution.
static void test_non_matching_filter_declines() {
    TEST("non-matching filter declines and rethrows to the outer handler");
    int strict_hits = 0;
    int fallback_hits = 0;

    // A filter that always declines: the handler must not take the exception.
    CHAOS_EH_TRY
    CHAOS_EH_TRY
        RaiseSentinel();
    CHAOS_EH_CATCH_BEGIN
        // Simulate a typed catch whose filter does not match.
#if defined(CHAOS_IL2CPP_EH_SETJMP) || defined(CHAOS_IL2CPP_EH_WIN32_SEH)
        // In these modes CHAOS_EH_CATCH_BEGIN is the raw landing pad, so the
        // filter is expressed explicitly, mirroring what codegen emits.
        {
            const CHAOS_IL2CPP_INTPTR exc = CHAOS_EH_EXCEPTION_OBJ;
            if (exc != 0xDEADBEEF) {   // deliberate mismatch
                CHAOS_EH_RETHROW;
            }
            ++strict_hits;
        }
#else
        // Default mode: CHAOS_EH_RETHROW compiles to a plain `throw`.
        {
            const CHAOS_IL2CPP_INTPTR exc = CHAOS_EH_EXCEPTION_OBJ;
            if (exc != 0xDEADBEEF) {
                CHAOS_EH_RETHROW;
            }
            ++strict_hits;
        }
#endif
    CHAOS_EH_END
    CHAOS_EH_CATCH_BEGIN
        ++fallback_hits;
    CHAOS_EH_END

    CHECK(strict_hits == 0 && fallback_hits == 1,
          "a declining filter still consumed the exception (strict=0, fallback=1 expected)");
}

// ── Case 11: matching filter DOES run ────────────────────────────────────
// Positive counterpart to case 10 — together they pin the filter contract
// from both sides.
static void test_matching_filter_runs() {
    TEST("matching filter takes the exception");
    int strict_hits = 0;
    int fallback_hits = 0;
    CHAOS_EH_TRY
    CHAOS_EH_TRY
        RaiseSentinel();
    CHAOS_EH_CATCH_BEGIN
        {
            const CHAOS_IL2CPP_INTPTR exc = CHAOS_EH_EXCEPTION_OBJ;
            if (exc == kRaiseValue) {
                ++strict_hits;
            } else {
                CHAOS_EH_RETHROW;
            }
        }
    CHAOS_EH_END
    CHAOS_EH_CATCH_BEGIN
        ++fallback_hits;
    CHAOS_EH_END
    CHECK(strict_hits == 1 && fallback_hits == 0,
          "matching filter did not take the exception (strict=1, fallback=0 expected)");
}

// ── Case 12: EH is usable after all previous cases (no residual state) ────
// Guards against a mode whose bookkeeping is corrupted by earlier cases —
// under SETJMP a leaked depth counter would surface here as an abort.
static void test_eh_state_clean_after_suite() {
    TEST("EH still functional after the full suite (no residual state)");
    g_handler_ran = false;
    CHAOS_EH_TRY
        RaiseSentinel();
    CHAOS_EH_CATCH_BEGIN
        g_handler_ran = true;
    CHAOS_EH_END
    CHECK(g_handler_ran, "EH became unusable after earlier cases — leaked state");
}

// ── Entry point ──────────────────────────────────────────────────────────

int main() {
#if defined(CHAOS_IL2CPP_EH_WIN32_SEH)
    const char* mode = "WIN32_SEH";
#elif defined(CHAOS_IL2CPP_EH_SETJMP)
    const char* mode = "SETJMP";
#else
    const char* mode = "CPP_THROW";
#endif
    std::printf("EH mode-equivalence suite [%s]\n", mode);

    test_raise_is_caught();
    test_no_raise_skips_handler();
    test_raise_value_survives();
    test_nested_inner_handler_wins();
    test_nested_outer_handler_still_live();
    test_rethrow_reaches_outer();
    test_raise_across_call_boundary();
    test_deep_nesting_delivers_to_innermost();
    test_sequential_trys();
    test_non_matching_filter_declines();
    test_matching_filter_runs();
    test_eh_state_clean_after_suite();

    std::printf("\n%s: %d/%d passed (mode=%s)\n",
                (g_tests_passed == g_tests_run) ? "PASS" : "FAIL",
                g_tests_passed, g_tests_run, mode);
    return (g_tests_passed == g_tests_run) ? 0 : 1;
}
