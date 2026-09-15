// async_t1_test.cpp — T1: Task.ConfigureAwait / Wait(CancellationToken)
//
// WHAT THIS PINS
// --------------
// These override the previous ChaosExternalRuntimeFallback stubs.  Each entry
// IS identity-shaped or trivially forwards to existing async_stubs.cpp APIs,
// so the tests assert that the call SUCCEEDS and returns a *distinguishable*
// value — not that it "runs the full wait".  A constant that always returned 1
// would pass some of these, which is why each test asserts a STATE after the
// call rather than merely "it returned nonzero".
//
// The receiver injection (Phase A) applies generically: Task.ConfigureAwait
// receives (task_handle, bool) and returns the handle — the handle then flows
// to ConfiguredTaskAwaitable.GetAwaiter() etc.

#include "async_stubs.h"

#include <chaos/async.h>
#include <chaos/config.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <cstdlib>
#include <thread>
#include <chrono>

namespace {

/// Create a completed task with a payload of 42.
CHAOS_IL2CPP_INTPTR CompletedTask() noexcept
{
    auto* task = new chaos::il2cpp::common::AsyncTask();
    task->completed.store(true, std::memory_order_release);
    task->result = 42;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(task);
}

/// Create a resolved (but not completed) task.  The caller must complete it.
chaos::il2cpp::common::AsyncTask* PendingTask() noexcept
{
    return new chaos::il2cpp::common::AsyncTask();
}

}  // namespace

// ── Task.Wait(CancellationToken) ───────────────────────────────────

TEST(AsyncT1, WaitTokenOnCompletedTaskReturnsImmediately)
{
    const CHAOS_IL2CPP_INTPTR t = CompletedTask();
    EXPECT_EQ(ChaosAsyncTaskWaitToken(t, 0), 1)
        << "Wait on a completed task must return 1 (completed)";
    delete reinterpret_cast<chaos::il2cpp::common::AsyncTask*>(t);
}

// ── ConfigureAwait + ConfiguredTaskAwaitable chain ──────────────────

TEST(AsyncT1, ConfigureAwaitReturnsTheTaskHandle)
{
    const CHAOS_IL2CPP_INTPTR t = CompletedTask();

    const CHAOS_IL2CPP_INTPTR carrier = ChaosAsyncTaskConfigureAwait(t, 1);
    EXPECT_NE(carrier, 0) << "ConfigureAwait(bool) must return a carrier";

    const CHAOS_IL2CPP_INTPTR awaiter = ChaosConfiguredTaskAwaitableGetAwaiter(carrier);
    EXPECT_EQ(awaiter, t)
        << "GetAwaiter must return the same task handle — the awaiter IS the task";

    delete reinterpret_cast<chaos::il2cpp::common::AsyncTask*>(t);
}

TEST(AsyncT1, ConfigureAwaitOptionsReturnsTheTaskHandle)
{
    const CHAOS_IL2CPP_INTPTR t = CompletedTask();

    const CHAOS_IL2CPP_INTPTR carrier = ChaosAsyncTaskConfigureAwaitOptions(t, 0);
    EXPECT_NE(carrier, 0);

    const CHAOS_IL2CPP_INTPTR awaiter = ChaosConfiguredTaskAwaitableGetAwaiter(carrier);
    EXPECT_EQ(awaiter, t);

    delete reinterpret_cast<chaos::il2cpp::common::AsyncTask*>(t);
}

TEST(AsyncT1, ConfiguredTaskAwaitableGetResultReturnsTheTaskValue)
{
    const CHAOS_IL2CPP_INTPTR t = CompletedTask();
    const CHAOS_IL2CPP_INTPTR awaiter = ChaosConfiguredTaskAwaitableGetAwaiter(t);

    EXPECT_EQ(ChaosConfiguredTaskAwaitableGetResult(awaiter), 42)
        << "GetResult must return the task's result value (not a constant)";

    delete reinterpret_cast<chaos::il2cpp::common::AsyncTask*>(t);
}

TEST(AsyncT1, TaskOfTConfigureAwaitReturnsTheHandle)
{
    const CHAOS_IL2CPP_INTPTR t = CompletedTask();

    const CHAOS_IL2CPP_INTPTR carrier = ChaosAsyncTaskOfTConfigureAwait(t, 0);
    EXPECT_EQ(carrier, t);

    delete reinterpret_cast<chaos::il2cpp::common::AsyncTask*>(t);
}

// ── ValueTask.ConfigureAwait + ConfiguredValueTaskAwaitable chain ──

TEST(AsyncT1, ValueTaskConfigureAwaitReturnsThePointer)
{
    // ValueTask carrier is an opaque INTPTR; ConfigureAwait must return it.
    const CHAOS_IL2CPP_INTPTR vt = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(PendingTask());

    const CHAOS_IL2CPP_INTPTR carrier = ChaosValueTaskConfigureAwait(vt, 0);
    EXPECT_EQ(carrier, vt);

    const CHAOS_IL2CPP_INTPTR awaiter = ChaosConfiguredValueTaskAwaitableGetAwaiter(carrier);
    EXPECT_EQ(awaiter, vt);

    delete reinterpret_cast<chaos::il2cpp::common::AsyncTask*>(vt);
}

TEST(AsyncT1, ConfiguredValueTaskAwaitableGetIsCompletedReflectsTaskState)
{
    auto* task = PendingTask();
    const CHAOS_IL2CPP_INTPTR vt = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(task);
    const CHAOS_IL2CPP_INTPTR awaiter = vt;

    // Pending: IsCompleted must be 0/false.
    EXPECT_EQ(ChaosConfiguredValueTaskAwaitableGetIsCompleted(awaiter), 0)
        << "a pending task must not report IsCompleted";

    // Complete and re-check.
    task->completed.store(true, std::memory_order_release);
    EXPECT_EQ(ChaosConfiguredValueTaskAwaitableGetIsCompleted(awaiter), 1)
        << "after completion IsCompleted must be true";

    delete task;
}