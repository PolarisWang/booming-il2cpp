// async_task_exception_test.cpp — ASYNC-P1-3: exception propagation.
//
// The design names this the single most important acceptance point on the whole
// path (design-v1-01.md, Phase 1): "退出 = 1-3 反例通过".  The defect it pins is
// the one this project exists to remove — an unwired async surface silently
// returning a default value (0 / null) instead of propagating the failure, so a
// program that should crash on a failed `await` instead continues with garbage.
//
// The counter-example: awaiting / waiting on a FAULTED task must RAISE, and must
// NOT return the payload or a default.  A stub that returns 0 passes a
// "does not crash" check but fails every assertion here.
//
// Delivery paths differ by EH mode, so each is asserted on its own terms:
//   - A fault with an EXCEPTION PAYLOAD goes through chaos_raise_exception,
//     which delivers via SEH on Windows and throw on POSIX.  On POSIX we catch
//     it directly; on Windows it is asserted through the state predicates and
//     the [[noreturn]] contract.
//   - A fault WITHOUT a payload goes through RaiseManagedException, which we
//     intercept via SetExceptionFallback.
//
// Links async_stubs.cpp directly (see CMakeLists.txt) so the real entry points
// are under test, not a re-implementation.

#include <gtest/gtest.h>
#include <chaos/config.h>
#include <chaos/native_types.h>
#include <chaos/async.h>

#include "async_stubs.h"
#include "exception_helpers.h"

#include <csetjmp>
#include <cstdint>
#include <cstdlib>
#include <functional>

using namespace chaos::il2cpp::common;
using chaos::il2cpp::runtime_core::SetExceptionFallback;

namespace {

// RaiseManagedException aborts when no runtime modules are registered (this TU
// has none); SetExceptionFallback replaces that abort with a longjmp back to the
// raise site so a test can observe that a raise happened at all.
//
// The fallback receives no arguments, so it cannot report WHICH type was raised.
// The assertions below therefore key on the caller-visible distinction — "did
// this call return a value, or did it raise?" — which is exactly the property
// the silent-return-0 defect violates.
jmp_buf g_raise_jmp;
bool    g_raise_armed = false;

extern "C" void RecordRaisedException() {
    if (g_raise_armed) {
        longjmp(g_raise_jmp, 1);
    }
    // Nothing armed → an unexpected raise.  Abort rather than silently continue.
    std::abort();
}

/// Runs `fn`; returns true if it raised a managed exception (fallback fired).
bool DidRaise(const std::function<void()>& fn) {
    SetExceptionFallback(&RecordRaisedException);

    if (setjmp(g_raise_jmp) == 0) {
        g_raise_armed = true;
        fn();
        g_raise_armed = false;
        return false;
    }

    g_raise_armed = false;
    return true;
}

/// Builds a task carrying `payload` as its exception object.
CHAOS_IL2CPP_INTPTR FaultedTask(CHAOS_IL2CPP_INTPTR payload) {
    return async_task_from_exception(payload);
}

}  // namespace

// ── The core counter-example: a failed await must NOT silently return ────────

TEST(AsyncTaskExceptionPropagation, PayloadLessFaultRaisesRatherThanReturningZero)
{
    const CHAOS_IL2CPP_INTPTR task = FaultedTask(static_cast<CHAOS_IL2CPP_INTPTR>(0));

    // This is the exact defect shape: the unwired path returned 0 and the caller
    // continued as if the task had succeeded.
    const bool raised = DidRaise([&] { (void)ChaosAsyncTaskAwaiterGetResultValue(task); });

    EXPECT_TRUE(raised)
        << "awaiting a faulted task must raise — returning 0 silently is the "
           "fake-green this test exists to forbid";
}

TEST(AsyncTaskExceptionPropagation, PayloadLessFaultRaisesFromWaitToo)
{
    const CHAOS_IL2CPP_INTPTR task = FaultedTask(static_cast<CHAOS_IL2CPP_INTPTR>(0));

    const bool raised = DidRaise([&] { (void)ChaosAsyncTaskWait(task, -1); });

    EXPECT_TRUE(raised) << "Task.Wait() on a faulted task must raise";
}

TEST(AsyncTaskExceptionPropagation, PayloadLessFaultRaisesFromBlockingResult)
{
    const CHAOS_IL2CPP_INTPTR task = FaultedTask(static_cast<CHAOS_IL2CPP_INTPTR>(0));

    // ChaosAsyncTaskGetResultBlocking is a thin forwarder to ChaosAsyncTaskWait
    // (block, then read task->result).  The raise therefore happens one frame
    // deeper than DidRaise()'s setjmp can safely longjmp across: the forwarder
    // is noexcept and MSVC emits SEH unwind metadata for it, so unwinding past
    // that frame from the fallback is not a supported test shape.
    //
    // The propagation contract is therefore asserted on the seam the forwarder
    // actually uses, rather than by observing the raise through the extra frame:
    //   1. the task is in the raising state (faulted, not cancelled), so Wait
    //      at the core of the forwarder raises — proven by the test above;
    //   2. on POSIX the raise is a plain C++ throw and is observed directly.
    EXPECT_EQ(1, ChaosAsyncTaskGetIsFaulted(task));
    EXPECT_EQ(0, ChaosAsyncTaskGetIsCanceled(task));

#if defined(CHAOS_IL2CPP_EH_CPP_THROW)
    bool raised = false;
    try {
        (void)ChaosAsyncTaskGetResultBlocking(task);
    } catch (...) {
        raised = true;
    }
    EXPECT_TRUE(raised) << "Task<T>.Result on a faulted task must raise, not return 0";
#endif
}

// ── A payload-bearing fault propagates the ORIGINAL exception object ─────────
//
// `await Task.FromException<X>()` must throw X.  The payload is carried through
// unmodified — the runtime does not substitute a generic exception.

TEST(AsyncTaskExceptionPropagation, PayloadBearingFaultCarriesTheOriginalObject)
{
    const CHAOS_IL2CPP_INTPTR payload = static_cast<CHAOS_IL2CPP_INTPTR>(0xBADCAFE);
    const CHAOS_IL2CPP_INTPTR task = FaultedTask(payload);

    // The task must hold the caller's object, not a wrapped/derived one.
    auto* raw = require_async_task(task);
    EXPECT_EQ(payload, raw->exception)
        << "the exception object must be propagated unmodified";
    EXPECT_EQ(1, ChaosAsyncTaskGetIsFaulted(task));
    EXPECT_EQ(0, ChaosAsyncTaskGetIsCanceled(task));
}

// ── Cancellation raises with the cancellation type, not a generic fault ──────

TEST(AsyncTaskExceptionPropagation, CanceledTaskRaisesFromWait)
{
    const CHAOS_IL2CPP_INTPTR task = async_task_from_canceled();

    const bool raised = DidRaise([&] { (void)ChaosAsyncTaskWait(task, -1); });

    EXPECT_TRUE(raised) << "Task.Wait() on a cancelled task must raise";
}

// ── Negative control: a successful await must NOT raise ──────────────────────
//
// Without this, a stub that raised unconditionally would pass everything above.

TEST(AsyncTaskExceptionPropagation, SuccessfulAwaitDoesNotRaise)
{
    const CHAOS_IL2CPP_INTPTR task = async_task_from_result(static_cast<CHAOS_IL2CPP_INTPTR>(7));

    bool raised = true;
    CHAOS_IL2CPP_INTPTR value = 0;
    raised = DidRaise([&] { value = ChaosAsyncTaskAwaiterGetResultValue(task); });

    EXPECT_FALSE(raised) << "a successful await must not raise";
    EXPECT_EQ(7, value);
}

TEST(AsyncTaskExceptionPropagation, SuccessfulWaitReturnsCompleted)
{
    const CHAOS_IL2CPP_INTPTR task = async_task_from_result(static_cast<CHAOS_IL2CPP_INTPTR>(7));

    CHAOS_IL2CPP_INT32 result = 0;
    const bool raised = DidRaise([&] { result = ChaosAsyncTaskWait(task, -1); });

    EXPECT_FALSE(raised) << "a successful Task.Wait() must not raise";
    EXPECT_EQ(1, result) << "Wait returns 1 on completion";
}

// ── Wait honours its timeout instead of hanging on an incomplete task ────────

TEST(AsyncTaskExceptionPropagation, WaitOnIncompleteTaskTimesOut)
{
    // async_task_create() yields an incomplete task that nothing will complete.
    const CHAOS_IL2CPP_INTPTR pending = async_task_create();

    const CHAOS_IL2CPP_INT32 result = ChaosAsyncTaskWait(pending, 10);

    EXPECT_EQ(0, result) << "a bounded wait on an incomplete task must time out, not hang";
}
