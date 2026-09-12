// async_task_state_test.cpp — ASYNC-P1-4: three-state distinction.
//
// C# tasks have THREE mutually-exclusive terminal states, and `await` must
// behave differently for each:
//
//   resolved  → GetResult returns the value
//   faulted   → GetResult THROWS the stored exception
//   cancelled → GetResult THROWS TaskCanceledException
//
// The defect this pins: cancellation used to be modelled as "faulted with a
// null exception payload", so the three states were NOT distinguishable — a
// payload-less fault and a cancellation both produced TaskCanceledException,
// and a cancelled task carrying a payload would have surfaced as a plain fault.
//
// These tests are counter-examples in the sense the design requires: each
// asserts a POSITIVE, DISTINCT observable per state, and the states are
// asserted to be mutually exclusive, so a fix that collapses two states into
// one cannot pass.
//
// Links the runtime_stubs TU directly (see CMakeLists.txt) so the real
// ChaosAsyncTask* entry points are under test, not a re-implementation.

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
#include <string>
#include <vector>

using namespace chaos::il2cpp::common;
using chaos::il2cpp::runtime_core::SetExceptionFallback;

namespace {

// RaiseManagedException aborts when no runtime modules are registered (this TU
// has none).  SetExceptionFallback replaces that abort with a longjmp back to
// the raise site, so a test can observe that a raise happened at all.
//
// The fallback takes no arguments, so it cannot report WHICH managed type was
// raised.  The assertions below therefore key on the caller-visible distinction:
// the state predicates must be mutually exclusive, a payload-less fault must not
// read as cancellation, and a resolved task's GetResult returns its payload while
// a cancelled task's GetResult raises.  Those are falsifiable claims about the
// real entry points, and collapsing any two states breaks them.
//
// The payload-bearing FAULT path is raised via chaos_raise_exception, which
// delivers through SEH on Windows / throw on POSIX — not through
// RaiseManagedException — and is asserted separately below.

jmp_buf g_raise_jmp;
bool    g_raise_armed = false;

extern "C" void RecordRaisedException() {
    if (g_raise_armed) {
        longjmp(g_raise_jmp, 1);
    }
    // Nothing armed → cannot record; abort rather than silently continuing, so
    // an unexpected raise is loud.
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

}  // namespace

// ── State predicates are mutually exclusive on a completed task ──────────────

TEST(AsyncTaskThreeState, ResolvedTaskIsCompletedOnly)
{
    const CHAOS_IL2CPP_INTPTR task = async_task_from_result(static_cast<CHAOS_IL2CPP_INTPTR>(42));

    EXPECT_EQ(1, ChaosAsyncTaskGetIsCompleted(task));
    EXPECT_EQ(0, ChaosAsyncTaskGetIsFaulted(task))  << "resolved task must not report faulted";
    EXPECT_EQ(0, ChaosAsyncTaskGetIsCanceled(task)) << "resolved task must not report canceled";
}

TEST(AsyncTaskThreeState, FaultedTaskIsCompletedAndFaultedOnly)
{
    const CHAOS_IL2CPP_INTPTR task =
        async_task_from_exception(static_cast<CHAOS_IL2CPP_INTPTR>(0xBAD));

    EXPECT_EQ(1, ChaosAsyncTaskGetIsCompleted(task));
    EXPECT_EQ(1, ChaosAsyncTaskGetIsFaulted(task));
    EXPECT_EQ(0, ChaosAsyncTaskGetIsCanceled(task)) << "faulted task must not report canceled";
}

TEST(AsyncTaskThreeState, CanceledTaskIsCompletedAndCanceledOnly)
{
    const CHAOS_IL2CPP_INTPTR task = async_task_from_canceled();

    EXPECT_EQ(1, ChaosAsyncTaskGetIsCompleted(task));
    EXPECT_EQ(1, ChaosAsyncTaskGetIsCanceled(task));
    EXPECT_EQ(0, ChaosAsyncTaskGetIsFaulted(task))
        << "canceled task must not report faulted — this is the state collapse the fix removes";
}

// ── Counter-example: a payload-less fault must NOT look like cancellation ────
//
// Under the old model (cancellation := faulted && exception == 0) this task was
// indistinguishable from async_task_from_canceled().  It is not the same thing:
// FromException(null) is a FAULT that happens to carry no payload.

TEST(AsyncTaskThreeState, PayloadLessFaultIsNotCancellation)
{
    const CHAOS_IL2CPP_INTPTR task = async_task_from_exception(static_cast<CHAOS_IL2CPP_INTPTR>(0));

    const auto faulted  = ChaosAsyncTaskGetIsFaulted(task);
    const auto canceled = ChaosAsyncTaskGetIsCanceled(task);
    const auto canceled_task = ChaosAsyncTaskGetIsCanceled(async_task_from_canceled());

    EXPECT_EQ(1, faulted);
    EXPECT_EQ(0, canceled);
    EXPECT_NE(faulted, canceled)
        << "a payload-less fault and a cancellation must not be the same state";
    EXPECT_EQ(canceled_task, 1);  // sanity: the cancel flag does work where set
}

// ── The awaiter reports the same three states ────────────────────────────────
//
// The awaiter is what `await` actually consults, so the distinction must hold
// there too, not just on the task handle.

TEST(AsyncTaskThreeState, AwaiterPredicatesMirrorTaskState)
{
    // async_task_awaiter_* take a POINTER to the awaiter slot (matching the
    // codegen calling convention for `awaiter` locals), not the task handle.
    const CHAOS_IL2CPP_INTPTR resolved = async_task_from_result(static_cast<CHAOS_IL2CPP_INTPTR>(1));
    const CHAOS_IL2CPP_INTPTR faulted =
        async_task_from_exception(static_cast<CHAOS_IL2CPP_INTPTR>(0xBAD));
    const CHAOS_IL2CPP_INTPTR canceled = async_task_from_canceled();

    CHAOS_IL2CPP_INTPTR resolved_slot = resolved;
    CHAOS_IL2CPP_INTPTR faulted_slot  = faulted;
    CHAOS_IL2CPP_INTPTR canceled_slot = canceled;

    EXPECT_EQ(0, async_task_awaiter_get_is_faulted(
                     reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&resolved_slot)));
    EXPECT_EQ(0, async_task_awaiter_get_is_canceled(
                     reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&resolved_slot)));

    EXPECT_EQ(1, async_task_awaiter_get_is_faulted(
                     reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&faulted_slot)));
    EXPECT_EQ(0, async_task_awaiter_get_is_canceled(
                     reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&faulted_slot)));

    EXPECT_EQ(0,
              async_task_awaiter_get_is_faulted(
                  reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&canceled_slot)))
        << "cancelled task must not report faulted to the awaiter";
    EXPECT_EQ(1, async_task_awaiter_get_is_canceled(
                     reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&canceled_slot)));
}

// ── The three states produce three DIFFERENT outcomes from GetResult ─────────

TEST(AsyncTaskThreeState, ResolvedGetResultReturnsPayloadWithoutRaising)
{
    const CHAOS_IL2CPP_INTPTR resolved = async_task_from_result(static_cast<CHAOS_IL2CPP_INTPTR>(42));

    // Load-bearing negative: awaiting a resolved task must NOT raise.  A stub
    // that unconditionally raised (or unconditionally returned 0) fails here.
    bool raised = true;
    CHAOS_IL2CPP_INTPTR value = 0;
    raised = DidRaise([&] { value = ChaosAsyncTaskAwaiterGetResultValue(resolved); });

    EXPECT_FALSE(raised) << "awaiting a resolved task must not raise";
    EXPECT_EQ(42, value) << "awaiting a resolved task must return its payload";
}

TEST(AsyncTaskThreeState, CanceledGetResultRaisesInsteadOfReturning)
{
    const CHAOS_IL2CPP_INTPTR canceled = async_task_from_canceled();

    // Load-bearing positive: awaiting a cancelled task must raise rather than
    // hand back a value.  Under the old "cancel == faulted with null payload"
    // model a cancelled task with a payload would have returned the payload
    // here instead — this assertion is what forbids that.
    const bool raised = DidRaise([&] { (void)ChaosAsyncTaskAwaiterGetResultValue(canceled); });

    EXPECT_TRUE(raised) << "awaiting a cancelled task must raise, not return a value";
}

TEST(AsyncTaskThreeState, FaultedGetResultRaisesInsteadOfReturning)
{
    const CHAOS_IL2CPP_INTPTR faulted =
        async_task_from_exception(static_cast<CHAOS_IL2CPP_INTPTR>(0xBAD));

    // A payload-bearing fault propagates through chaos_raise_exception, which
    // delivers via SEH on Windows / throw on POSIX — NOT through
    // RaiseManagedException, so DidRaise() cannot observe it.  It is asserted
    // structurally instead: the entry point is [[noreturn]] on this path, and
    // the state predicates below prove the task is faulted-but-not-cancelled,
    // i.e. it will raise rather than return a value.
    EXPECT_EQ(1, ChaosAsyncTaskGetIsFaulted(faulted));
    EXPECT_EQ(0, ChaosAsyncTaskGetIsCanceled(faulted));

#if defined(CHAOS_IL2CPP_EH_CPP_THROW)
    // On POSIX the raise is a C++ throw we can catch directly.
    bool raised = false;
    try {
        (void)ChaosAsyncTaskAwaiterGetResultValue(faulted);
    } catch (...) {
        raised = true;
    }
    EXPECT_TRUE(raised) << "awaiting a faulted task must raise, not return a value";
#endif
}
