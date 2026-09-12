// async_task_run_e2e_test.cpp — ASYNC-P1-1: Task.Run end-to-end.
//
// Design counter-example (design-v1-01.md, Phase 1):
//     `Task.Run(() => 42).Result` → 42
// and the generated C++ must contain the async_task_run call rather than fall
// through to ChaosExternalRuntimeFallback.  The emission half of that claim is
// asserted in the managed test
// tests/unit/managed/codegen/AsyncPipelineTests.cs::TaskRun_LowersToNativeAsyncTaskRun.
// This file asserts the NATIVE half: that the delegate actually RUNS on the
// thread pool and its completion is observable.
//
// Why this test exists at all — the coverage gap it closes:
//   Every existing Task.Run test (async_integration_smoke_test.cpp) substitutes
//   a test-local stand-in for TaskRun via register_async_task_run_fn().  The
//   PRODUCTION implementation in task_runner.cpp — the one registered by
//   runtime_init.cpp::InitializeRuntime — was therefore never executed by any
//   test.  A delegate-invocation regression there would leave every Task.Run
//   "passing" while doing nothing.  This test drives the real one.
//
// Links the production task_runner.cpp and thread_pool.cpp directly (see
// CMakeLists.txt) and registers the real RegisterAsyncTaskRun(), so the code
// under test is the shipping path, not a stand-in.

#include <gtest/gtest.h>
#include <chaos/native_types.h>
#include <chaos/async.h>

#include "core/delegate_object.h"
#include "thread_pool.h"
#include "thread_state.h"
#include "timer_queue.h"
#include "runtime_stubs/async_stubs.h"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <thread>

using namespace chaos::il2cpp::common;
using chaos::il2cpp::runtime_core::DelegateObject;

namespace threading = chaos::il2cpp::runtime_core::threading;

// The production entry point, declared the same way runtime_init.cpp declares it.
namespace chaos::il2cpp::runtime_core::threading {
void RegisterAsyncTaskRun() noexcept;
}

namespace {
// ── A real managed-style delegate whose method body records the invocation ──
//
// InvokeSingleEntry distinguishes closed from open delegates: with a non-null
// chaos_delegate_target it calls thunk(target, args...) (closed instance
// method); with target == 0 it calls thunk(args...) (open/static).  This test
// uses the open form, so the thunk takes no parameters.
std::atomic<int> g_invoke_count{0};

CHAOS_IL2CPP_INTPTR RecordingDelegateMethod() {
    g_invoke_count.fetch_add(1);
    return 42;
}

/// Builds an open (static-style) DelegateObject wired to RecordingDelegateMethod.
DelegateObject* MakeRecordingDelegate() {
    auto* del = new DelegateObject();
    del->type_info = nullptr;
    del->chaos_delegate_target = 0;  // open delegate → thunk() takes no args
    del->chaos_delegate_method_ptr =
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&RecordingDelegateMethod);
    del->chaos_delegate_invocation_list = 0;
    del->chaos_delegate_invocation_count = 0;
    del->chaos_delegate_method_token = 0;
    del->_pad = 0;
    return del;
}

constexpr auto kPollInterval = std::chrono::milliseconds(5);
constexpr int  kMaxPolls = 400;  // 2s

bool WaitFor(const std::function<bool()>& pred) {
    for (int i = 0; i < kMaxPolls; ++i) {
        if (pred()) return true;
        std::this_thread::sleep_for(kPollInterval);
    }
    return pred();
}

class TaskRunE2ETest : public ::testing::Test {
protected:
    void SetUp() override {
        threading::RegisterThread(threading::kMainThreadId, nullptr);
        threading::TimerQueueInitialize();
        threading::ThreadPoolInitialize();
        // Register the PRODUCTION implementation — not a test stand-in.
        threading::RegisterAsyncTaskRun();

        g_invoke_count.store(0);

    }

    void TearDown() override {
        register_async_task_run_fn(nullptr);
        threading::ThreadPoolShutdown();
        threading::TimerQueueShutdown();
        threading::UnregisterThread();
    }
};

}  // namespace

// ── The counter-example: the delegate must actually RUN ──────────────────────

TEST_F(TaskRunE2ETest, TaskRunInvokesTheDelegateOnThePool)
{
    DelegateObject* del = MakeRecordingDelegate();

    const CHAOS_IL2CPP_INTPTR task =
        async_task_run(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(del));

    ASSERT_NE(0, task) << "Task.Run must return a task handle, not 0";

    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(task) != 0; }))
        << "the task must complete once the delegate has run";

    // The load-bearing claim: the delegate's METHOD BODY ran.  A stub that
    // completed the task without invoking anything passes the assertion above
    // and fails this one — which is exactly the fake-green this pins.
    EXPECT_EQ(1, g_invoke_count.load());
}

TEST_F(TaskRunE2ETest, CompletedTaskIsResolvedNotFaultedNotCanceled)
{
    DelegateObject* del = MakeRecordingDelegate();
    const CHAOS_IL2CPP_INTPTR task =
        async_task_run(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(del));

    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(task) != 0; }));

    EXPECT_EQ(0, ChaosAsyncTaskGetIsFaulted(task)) << "a successful Task.Run is not faulted";
    EXPECT_EQ(0, ChaosAsyncTaskGetIsCanceled(task)) << "a successful Task.Run is not cancelled";
}

TEST_F(TaskRunE2ETest, WaitOnTaskRunReturnsAfterTheDelegateCompletes)
{
    DelegateObject* del = MakeRecordingDelegate();
    const CHAOS_IL2CPP_INTPTR task =
        async_task_run(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(del));

    // Blocking wait must not raise for a task that succeeds.
    const CHAOS_IL2CPP_INT32 completed = ChaosAsyncTaskWait(task, -1);

    EXPECT_EQ(1, completed);
    EXPECT_EQ(1, g_invoke_count.load()) << "Wait returned before the delegate ran";
}

// ── Task.Run(0) is the null-delegate case and must not produce a task ────────

TEST_F(TaskRunE2ETest, TaskRunWithNullDelegateReturnsZero)
{
    EXPECT_EQ(0, async_task_run(0));
    EXPECT_EQ(0, g_invoke_count.load());
}

// ── Many concurrent Task.Run calls each invoke their own delegate ────────────

TEST_F(TaskRunE2ETest, ConcurrentTaskRunsEachInvokeOnce)
{
    constexpr int kCount = 32;
    CHAOS_IL2CPP_INTPTR tasks[kCount];
    DelegateObject* dels[kCount];

    for (int i = 0; i < kCount; ++i) {
        dels[i] = MakeRecordingDelegate();
        tasks[i] = async_task_run(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(dels[i]));
        ASSERT_NE(0, tasks[i]);
    }

    ASSERT_TRUE(WaitFor([&] {
        for (int i = 0; i < kCount; ++i) {
            if (ChaosAsyncTaskGetIsCompleted(tasks[i]) == 0) return false;
        }
        return true;
    })) << "not all Task.Run tasks completed";

    EXPECT_EQ(kCount, g_invoke_count.load())
        << "every Task.Run must invoke its delegate exactly once";
}
