// async_task_factory_test.cpp — ASYNC-P2-5: Task.Factory.StartNew.
//
// The codegen half of this feature is asserted in the managed tests
// (RuntimeHelperShapeRegistryTests.TaskFactoryStartNew_* and
// AsyncPipelineTests.TaskFactoryStartNew_ResolvesToThreadPoolRunner), which pin
// that the registry resolves the delegate-only overload to
// chaos_task_factory_start_new and that the real pipeline emits that call.
//
// This file asserts the NATIVE half: that the shim actually queues the delegate
// on the thread pool and returns an observable task.
//
// Why chaos_task_factory_start_new is a separate symbol rather than routing
// straight to async_task_run: Task.Factory.StartNew has a first (receiver)
// argument — the factory handle — so the call site has arity 2.  The shim drops
// that handle and forwards the delegate.  Dropping it is correct, not lossy:
// the default TaskFactory (the only one get_Factory can return) schedules on the
// default scheduler, which is exactly where async_task_run queues.  A future
// non-default scheduler would need a real factory object here.

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

namespace chaos::il2cpp::runtime_core::threading {
void RegisterAsyncTaskRun() noexcept;
}

namespace {
std::atomic<int> g_invoke_count{0};

CHAOS_IL2CPP_INTPTR RecordingDelegateMethod() {
    g_invoke_count.fetch_add(1);
    return 42;
}

DelegateObject* MakeRecordingDelegate() {
    auto* del = new DelegateObject();
    del->type_info = nullptr;
    del->chaos_delegate_target = 0;
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

class TaskFactoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        threading::RegisterThread(threading::kMainThreadId, nullptr);
        threading::TimerQueueInitialize();
        threading::ThreadPoolInitialize();
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

// ── get_Factory returns a usable, non-null token ─────────────────────────────

TEST_F(TaskFactoryTest, DefaultFactoryIsNotNull)
{
    // A 0 handle would make `var f = Task.Factory; f.StartNew(work)` pass a null
    // receiver into StartNew.  The runtime has no factory object model, so the
    // token is opaque and never dereferenced — but it must not look like null.
    EXPECT_NE(0, chaos_task_default_factory());
}

// ── The counter-example: StartNew's delegate must actually RUN ───────────────

TEST_F(TaskFactoryTest, StartNewInvokesTheDelegateOnThePool)
{
    DelegateObject* del = MakeRecordingDelegate();
    const CHAOS_IL2CPP_INTPTR factory = chaos_task_default_factory();
    const CHAOS_IL2CPP_INTPTR task = chaos_task_factory_start_new(
        factory, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(del));

    ASSERT_NE(0, task) << "StartNew must return a task handle, not 0";

    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(task) != 0; }))
        << "the task must complete once the delegate has run";

    // The load-bearing claim: the delegate's METHOD BODY ran.  A stub that
    // completed the task without invoking anything passes the assert above and
    // fails this one — exactly the fake-green this pins.
    EXPECT_EQ(1, g_invoke_count.load());
}

TEST_F(TaskFactoryTest, StartNewTaskIsResolvedNotFaultedNotCanceled)
{
    DelegateObject* del = MakeRecordingDelegate();
    const CHAOS_IL2CPP_INTPTR task = chaos_task_factory_start_new(
        chaos_task_default_factory(), reinterpret_cast<CHAOS_IL2CPP_INTPTR>(del));

    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(task) != 0; }));

    EXPECT_EQ(0, ChaosAsyncTaskGetIsFaulted(task)) << "a successful StartNew is not faulted";
    EXPECT_EQ(0, ChaosAsyncTaskGetIsCanceled(task)) << "a successful StartNew is not cancelled";
}

// ── Null delegate must not produce a task ────────────────────────────────────

TEST_F(TaskFactoryTest, StartNewWithNullDelegateReturnsZero)
{
    EXPECT_EQ(0, chaos_task_factory_start_new(chaos_task_default_factory(), 0));
    EXPECT_EQ(0, g_invoke_count.load());
}

// ── The factory handle is genuinely ignored (documented, not accidental) ─────

TEST_F(TaskFactoryTest, StartNewIgnoresTheFactoryHandle)
{
    // Any non-zero receiver behaves identically — the token is opaque and the
    // default factory is the only one that exists.  Pinned so that adding a real
    // factory object model later is a conscious change, not a silent one.
    DelegateObject* del = MakeRecordingDelegate();
    const CHAOS_IL2CPP_INTPTR task = chaos_task_factory_start_new(
        0xBEEF, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(del));

    ASSERT_NE(0, task);
    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(task) != 0; }));
    EXPECT_EQ(1, g_invoke_count.load());
}

// ── Many concurrent StartNew calls each invoke their own delegate ────────────

TEST_F(TaskFactoryTest, ConcurrentStartNewEachInvokeOnce)
{
    constexpr int kCount = 32;
    CHAOS_IL2CPP_INTPTR tasks[kCount];
    DelegateObject* dels[kCount];

    for (int i = 0; i < kCount; ++i) {
        dels[i] = MakeRecordingDelegate();
        tasks[i] = chaos_task_factory_start_new(
            chaos_task_default_factory(), reinterpret_cast<CHAOS_IL2CPP_INTPTR>(dels[i]));
        ASSERT_NE(0, tasks[i]);
    }

    ASSERT_TRUE(WaitFor([&] {
        for (int i = 0; i < kCount; ++i) {
            if (ChaosAsyncTaskGetIsCompleted(tasks[i]) == 0) return false;
        }
        return true;
    })) << "not all StartNew tasks completed";

    EXPECT_EQ(kCount, g_invoke_count.load())
        << "every StartNew must invoke its delegate exactly once";
}
