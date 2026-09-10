// ── Exception sentinel propagation test — all EH modes ────────────────
//
// Validates that ThreadAbort/ThreadInterrupt sentinel values are correctly
// propagated through SETJMP (iOS/Android) and WIN32_SEH (Windows) modes.
//
// The CPP_THROW mode is covered by exception_sentinel_test.cpp.
//
// SETJMP mode:
//   chaos_raise_exception → TLS save → longjmp to push'd jmp_buf
//   Sentinel guard: if (obj < 0) pop + re-raise to outer handler
//
// WIN32_SEH mode:
//   chaos_raise_exception → TLS save → RaiseException(0xE0000001)
//   Sentinel guard: __except(CHAOS_SEH_FILTER_ALL()) → if sentinel, re-raise
//
// Each mode's tests are guarded by #if defined(CHAOS_IL2CPP_EH_*) so they
// compile only on the target platform.  On other platforms the test body
// calls GTEST_SKIP().

#include <gtest/gtest.h>

// Sentinel constants (kManagedExceptionThreadAbort, etc.).
#include <generated_code_compat.h>

// chaos_raise_exception and TLS globals for all three EH modes.
#include <exception_jmp.h>

// =========================================================================
// SETJMP mode (iOS / Android) — setjmp/longjmp delivery
// =========================================================================

#if defined(CHAOS_IL2CPP_EH_SETJMP)

#include <csetjmp>

using namespace chaos::il2cpp::runtime_core;

// ── Helper: single-level try/catch via setjmp ─────────────────────────
// Pushes a jmp_buf, calls chaos_raise_exception(val), and returns whatever
// landed in g_chaos_exception_obj after the longjmp.  Returns
// kManagedExceptionNormal if no exception was raised (should not happen).
static CHAOS_IL2CPP_INTPTR setjmp_catch_value(CHAOS_IL2CPP_INTPTR raise_val)
{
    auto* jmp = push_exception_jmp_buf();
    volatile CHAOS_IL2CPP_INTPTR result = kManagedExceptionNormal;

    if (setjmp(*jmp) == 0) {
        // TRY: raise the exception — this longjmps back to setjmp above.
        chaos_raise_exception(raise_val);
    } else {
        // CATCH: longjmp landed here; g_chaos_exception_obj holds the value.
        result = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(g_chaos_exception_obj);
    }
    pop_exception_jmp_buf();
    return result;
}

TEST(ExceptionSentinelAllModes, SETJMP_RaiseExceptionTriggersLongjmp)
{
    // chaos_raise_exception must longjmp back to the setjmp site and the
    // exception value must be available through g_chaos_exception_obj TLS.
    CHAOS_IL2CPP_INTPTR caught = setjmp_catch_value(kManagedExceptionThreadAbort);
    EXPECT_EQ(caught, kManagedExceptionThreadAbort);
}

TEST(ExceptionSentinelAllModes, SETJMP_ExceptionValuePreservedInTLS)
{
    // Each sentinel constant must round-trip correctly through
    // chaos_raise_exception → g_chaos_exception_obj.
    CHAOS_IL2CPP_INTPTR caught;

    caught = setjmp_catch_value(kManagedExceptionThreadAbort);
    EXPECT_EQ(caught, kManagedExceptionThreadAbort);

    caught = setjmp_catch_value(kManagedExceptionThreadInterrupt);
    EXPECT_EQ(caught, kManagedExceptionThreadInterrupt);

    caught = setjmp_catch_value(kManagedExceptionNormal);
    EXPECT_EQ(caught, kManagedExceptionNormal);

    caught = setjmp_catch_value(42);
    EXPECT_EQ(caught, 42);
}

TEST(ExceptionSentinelAllModes, SETJMP_SentinelRethrow)
{
    // Nested try/catch.  The inner handler encounters a sentinel (<0),
    // pops its jmp_buf, and chaos_raise_exception's to the outer handler.
    // The outer handler must receive the original sentinel value.

    volatile bool outer_caught = false;
    volatile CHAOS_IL2CPP_INTPTR outer_obj = kManagedExceptionNormal;

    auto* outer_jmp = push_exception_jmp_buf();
    if (setjmp(*outer_jmp) == 0) {
        // ── Outer TRY ────────────────────────────────────────────
        volatile bool inner_rethrow = false;

        auto* inner_jmp = push_exception_jmp_buf();
        if (setjmp(*inner_jmp) == 0) {
            // INNER TRY: raise a sentinel
            chaos_raise_exception(kManagedExceptionThreadAbort);
        } else {
            // INNER CATCH
            CHAOS_IL2CPP_INTPTR obj =
                reinterpret_cast<CHAOS_IL2CPP_INTPTR>(g_chaos_exception_obj);
            if (obj < 0) {
                // Sentinel guard: pop inner buffer, re-raise to outer
                inner_rethrow = true;
                pop_exception_jmp_buf();
                chaos_raise_exception(obj);
            }
        }
        pop_exception_jmp_buf();

    } else {
        // ── Outer CATCH — sentinel should arrive here ────────────
        outer_caught = true;
        outer_obj =
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(g_chaos_exception_obj);
    }
    pop_exception_jmp_buf();

    EXPECT_TRUE(outer_caught);
    EXPECT_EQ(outer_obj, kManagedExceptionThreadAbort);
}

TEST(ExceptionSentinelAllModes, SETJMP_SentinelRethrowThreadInterrupt)
{
    // Same as SETJMP_SentinelRethrow but with ThreadInterrupt.

    volatile bool outer_caught = false;
    volatile CHAOS_IL2CPP_INTPTR outer_obj = kManagedExceptionNormal;

    auto* outer_jmp = push_exception_jmp_buf();
    if (setjmp(*outer_jmp) == 0) {
        auto* inner_jmp = push_exception_jmp_buf();
        if (setjmp(*inner_jmp) == 0) {
            chaos_raise_exception(kManagedExceptionThreadInterrupt);
        } else {
            CHAOS_IL2CPP_INTPTR obj =
                reinterpret_cast<CHAOS_IL2CPP_INTPTR>(g_chaos_exception_obj);
            if (obj < 0) {
                pop_exception_jmp_buf();
                chaos_raise_exception(obj);
            }
        }
        pop_exception_jmp_buf();
    } else {
        outer_caught = true;
        outer_obj =
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(g_chaos_exception_obj);
    }
    pop_exception_jmp_buf();

    EXPECT_TRUE(outer_caught);
    EXPECT_EQ(outer_obj, kManagedExceptionThreadInterrupt);
}

TEST(ExceptionSentinelAllModes, SETJMP_NormalExceptionCaughtByInnerHandler)
{
    // A normal (non-sentinel) exception is caught by the inner handler
    // and NOT re-raised to the outer scope.

    volatile bool outer_caught = false;
    volatile bool inner_caught = false;
    volatile CHAOS_IL2CPP_INTPTR inner_obj = kManagedExceptionNormal;

    auto* outer_jmp = push_exception_jmp_buf();
    if (setjmp(*outer_jmp) == 0) {
        // ── Outer TRY ────────────────────────────────────────────
        auto* inner_jmp = push_exception_jmp_buf();
        if (setjmp(*inner_jmp) == 0) {
            // INNER TRY: raise a normal exception (value >= 0)
            chaos_raise_exception(100);
        } else {
            // INNER CATCH
            inner_caught = true;
            inner_obj =
                reinterpret_cast<CHAOS_IL2CPP_INTPTR>(g_chaos_exception_obj);
            // Not a sentinel — handle it here, do NOT re-raise.
        }
        pop_exception_jmp_buf();

    } else {
        // ── Outer CATCH — should NOT be reached ──────────────────
        outer_caught = true;
    }
    pop_exception_jmp_buf();

    EXPECT_TRUE(inner_caught);
    EXPECT_EQ(inner_obj, 100);
    EXPECT_FALSE(outer_caught);
}

TEST(ExceptionSentinelAllModes, SETJMP_MultipleSentinelTypes)
{
    // Verify multiple different sentinel values propagate correctly
    // through separate nested try/catch cycles.

    // ComFailure sentinel
    volatile bool outer_caught = false;
    volatile CHAOS_IL2CPP_INTPTR outer_obj = kManagedExceptionNormal;

    auto* outer_jmp = push_exception_jmp_buf();
    if (setjmp(*outer_jmp) == 0) {
        auto* inner_jmp = push_exception_jmp_buf();
        if (setjmp(*inner_jmp) == 0) {
            chaos_raise_exception(kManagedExceptionComFailure);
        } else {
            CHAOS_IL2CPP_INTPTR obj =
                reinterpret_cast<CHAOS_IL2CPP_INTPTR>(g_chaos_exception_obj);
            if (obj < 0) {
                pop_exception_jmp_buf();
                chaos_raise_exception(obj);
            }
        }
        pop_exception_jmp_buf();
    } else {
        outer_caught = true;
        outer_obj =
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(g_chaos_exception_obj);
    }
    pop_exception_jmp_buf();

    EXPECT_TRUE(outer_caught);
    EXPECT_EQ(outer_obj, kManagedExceptionComFailure);
}

#else  // !CHAOS_IL2CPP_EH_SETJMP

TEST(ExceptionSentinelAllModes, SETJMP_RaiseExceptionTriggersLongjmp)
{
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_SETJMP";
}

TEST(ExceptionSentinelAllModes, SETJMP_ExceptionValuePreservedInTLS)
{
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_SETJMP";
}

TEST(ExceptionSentinelAllModes, SETJMP_SentinelRethrow)
{
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_SETJMP";
}

TEST(ExceptionSentinelAllModes, SETJMP_SentinelRethrowThreadInterrupt)
{
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_SETJMP";
}

TEST(ExceptionSentinelAllModes, SETJMP_NormalExceptionCaughtByInnerHandler)
{
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_SETJMP";
}

TEST(ExceptionSentinelAllModes, SETJMP_MultipleSentinelTypes)
{
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_SETJMP";
}

#endif  // CHAOS_IL2CPP_EH_SETJMP


// =========================================================================
// WIN32_SEH mode (Windows) — RaiseException + __try/__except delivery
// =========================================================================

#if defined(CHAOS_IL2CPP_EH_WIN32_SEH)

// SEH __try/__except imposes C2712: cannot use in functions that require
// stack unwinding (i.e. functions with C++ objects possessing destructors).
// GoogleTest macros may create such objects, so SEH logic is isolated in
// POD-only helper functions that return results for assertion in the test
// body.

using chaos::il2cpp::runtime_core::kChaosManagedExceptionCode;

// ── Helper: raise & catch via __except, returning the exception code ──
// Returns 0 if the exception was not caught.
static DWORD seh_raise_and_get_code(CHAOS_IL2CPP_INTPTR raise_val)
{
    volatile DWORD code = 0;
    __try {
        chaos::il2cpp::runtime_core::chaos_raise_exception(raise_val);
    } __except (code = GetExceptionCode(),
                EXCEPTION_EXECUTE_HANDLER) {
        // handler body (no C++ objects with destructors)
    }
    return code;
}

// ── Helper: raise & catch, extracting both code and ExceptionInformation ──
struct seh_catch_info {
    DWORD                 code;
    CHAOS_IL2CPP_UINTPTR  info0;  // ExceptionInformation[0]
};
static seh_catch_info seh_raise_and_catch_info(CHAOS_IL2CPP_INTPTR raise_val)
{
    seh_catch_info info = {0, 0};
    __try {
        chaos::il2cpp::runtime_core::chaos_raise_exception(raise_val);
    } __except (info.code = GetExceptionCode(),
                info.info0 = static_cast<CHAOS_IL2CPP_UINTPTR>(
                    GetExceptionInformation()->ExceptionRecord->ExceptionInformation[0]),
                EXCEPTION_EXECUTE_HANDLER) {
        // handler body
    }
    return info;
}

// ── Helper: nested SEH try/except — inner catches, may re-raise ────────
// Returns true if the outer handler caught the exception.
static bool seh_nested_catch(CHAOS_IL2CPP_INTPTR raise_val,
                             bool inner_rethrows_sentinel,
                             CHAOS_IL2CPP_INTPTR* out_outer_obj)
{
    volatile bool outer_caught = false;
    volatile CHAOS_IL2CPP_UINTPTR outer_obj = 0;

    __try {
        // ── Outer TRY ────────────────────────────────────────────
        __try {
            // INNER TRY
            chaos::il2cpp::runtime_core::chaos_raise_exception(raise_val);
        } __except (chaos::il2cpp::runtime_core::g_chaos_exception_obj =
                        reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(
                            GetExceptionInformation()->ExceptionRecord->ExceptionInformation[0])),
                    EXCEPTION_EXECUTE_HANDLER) {
            // INNER CATCH
            CHAOS_IL2CPP_INTPTR obj = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
                chaos::il2cpp::runtime_core::g_chaos_exception_obj);
            if (inner_rethrows_sentinel && obj < 0) {
                // Sentinel guard: re-raise to outer
                chaos::il2cpp::runtime_core::chaos_raise_exception(obj);
            }
            // else: handled — exception swallowed
        }
    } __except (chaos::il2cpp::runtime_core::g_chaos_exception_obj =
                    reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(
                        GetExceptionInformation()->ExceptionRecord->ExceptionInformation[0])),
                EXCEPTION_EXECUTE_HANDLER) {
        // ── Outer CATCH ──────────────────────────────────────────
        outer_caught = true;
        outer_obj = reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(
            chaos::il2cpp::runtime_core::g_chaos_exception_obj);
    }

    *out_outer_obj = static_cast<CHAOS_IL2CPP_INTPTR>(outer_obj);
    return outer_caught;
}

// ── Helper: raise & catch, read g_chaos_exception_obj from handler ──
// (avoids __try in the same function as GoogleTest EXPECT_* macros)
static CHAOS_IL2CPP_INTPTR seh_read_tls_after_raise(CHAOS_IL2CPP_INTPTR raise_val)
{
    volatile CHAOS_IL2CPP_INTPTR obj = kManagedExceptionNormal;
    __try {
        chaos::il2cpp::runtime_core::chaos_raise_exception(raise_val);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        obj = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos::il2cpp::runtime_core::g_chaos_exception_obj);
    }
    return obj;
}

// ── Tests ─────────────────────────────────────────────────────────────

TEST(ExceptionSentinelAllModes, WIN32_SEH_RaiseExceptionHasCorrectCode)
{
    // chaos_raise_exception must call RaiseException with
    // kChaosManagedExceptionCode (0xE0000001).
    DWORD code = seh_raise_and_get_code(kManagedExceptionThreadAbort);
    EXPECT_EQ(code, kChaosManagedExceptionCode);
}

TEST(ExceptionSentinelAllModes, WIN32_SEH_ExceptionObjInTLS)
{
    // After chaos_raise_exception, g_chaos_exception_obj must hold
    // the sentinel value (TLS store happens before RaiseException).
    CHAOS_IL2CPP_INTPTR obj =
        seh_read_tls_after_raise(kManagedExceptionThreadAbort);
    EXPECT_EQ(obj, kManagedExceptionThreadAbort);
}

TEST(ExceptionSentinelAllModes, WIN32_SEH_ExceptionInfoCarriesValue)
{
    // RaiseException(arguments=1, &obj) passes the sentinel value as
    // ExceptionInformation[0].  Verify it matches.
    seh_catch_info info = seh_raise_and_catch_info(kManagedExceptionThreadAbort);
    EXPECT_EQ(info.code, kChaosManagedExceptionCode);
    EXPECT_EQ(static_cast<CHAOS_IL2CPP_INTPTR>(info.info0),
              kManagedExceptionThreadAbort);
}

TEST(ExceptionSentinelAllModes, WIN32_SEH_ExceptionInfoMultipleValues)
{
    // Verify all sentinel values round-trip through ExceptionInformation[0].

    seh_catch_info info;

    info = seh_raise_and_catch_info(kManagedExceptionThreadAbort);
    EXPECT_EQ(static_cast<CHAOS_IL2CPP_INTPTR>(info.info0),
              kManagedExceptionThreadAbort);

    info = seh_raise_and_catch_info(kManagedExceptionThreadInterrupt);
    EXPECT_EQ(static_cast<CHAOS_IL2CPP_INTPTR>(info.info0),
              kManagedExceptionThreadInterrupt);

    info = seh_raise_and_catch_info(kManagedExceptionComFailure);
    EXPECT_EQ(static_cast<CHAOS_IL2CPP_INTPTR>(info.info0),
              kManagedExceptionComFailure);

    info = seh_raise_and_catch_info(kManagedExceptionNormal);
    EXPECT_EQ(static_cast<CHAOS_IL2CPP_INTPTR>(info.info0),
              kManagedExceptionNormal);
}

TEST(ExceptionSentinelAllModes, WIN32_SEH_SentinelRethrow)
{
    // Nested __try/__except.  Inner handler detects sentinel and re-raises.
    // The outer handler must receive the original sentinel.
    CHAOS_IL2CPP_INTPTR outer_obj = kManagedExceptionNormal;
    bool outer_caught = seh_nested_catch(
        kManagedExceptionThreadAbort, /*inner_rethrows_sentinel=*/true,
        &outer_obj);

    EXPECT_TRUE(outer_caught);
    EXPECT_EQ(outer_obj, kManagedExceptionThreadAbort);
}

TEST(ExceptionSentinelAllModes, WIN32_SEH_SentinelRethrowThreadInterrupt)
{
    CHAOS_IL2CPP_INTPTR outer_obj = kManagedExceptionNormal;
    bool outer_caught = seh_nested_catch(
        kManagedExceptionThreadInterrupt, /*inner_rethrows_sentinel=*/true,
        &outer_obj);

    EXPECT_TRUE(outer_caught);
    EXPECT_EQ(outer_obj, kManagedExceptionThreadInterrupt);
}

TEST(ExceptionSentinelAllModes, WIN32_SEH_NormalExceptionCaught)
{
    // A normal (non-sentinel) exception is caught by the inner handler
    // and NOT re-raised.
    CHAOS_IL2CPP_INTPTR outer_obj = kManagedExceptionNormal;
    bool outer_caught = seh_nested_catch(
        42, /*inner_rethrows_sentinel=*/false, &outer_obj);

    EXPECT_FALSE(outer_caught);
}

#else  // !CHAOS_IL2CPP_EH_WIN32_SEH

TEST(ExceptionSentinelAllModes, WIN32_SEH_RaiseExceptionHasCorrectCode)
{
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_WIN32_SEH";
}

TEST(ExceptionSentinelAllModes, WIN32_SEH_ExceptionObjInTLS)
{
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_WIN32_SEH";
}

TEST(ExceptionSentinelAllModes, WIN32_SEH_ExceptionInfoCarriesValue)
{
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_WIN32_SEH";
}

TEST(ExceptionSentinelAllModes, WIN32_SEH_ExceptionInfoMultipleValues)
{
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_WIN32_SEH";
}

TEST(ExceptionSentinelAllModes, WIN32_SEH_SentinelRethrow)
{
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_WIN32_SEH";
}

TEST(ExceptionSentinelAllModes, WIN32_SEH_SentinelRethrowThreadInterrupt)
{
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_WIN32_SEH";
}

TEST(ExceptionSentinelAllModes, WIN32_SEH_NormalExceptionCaught)
{
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_WIN32_SEH";
}

#endif  // CHAOS_IL2CPP_EH_WIN32_SEH
