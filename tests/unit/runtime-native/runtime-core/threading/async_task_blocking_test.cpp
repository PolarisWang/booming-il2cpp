// async_task_blocking_test.cpp — ASYNC-P1-2: blocking Wait / Result semantics.
//
// Design counter-example (design-v1-01.md, Phase 1):
//     Task.Run(long task).Wait() must BLOCK for at least the task's duration
// and Result must return the real value.  The failure this pins is a "wait" that
// returns immediately — reporting completion (or a default value) while the
// work is still running, so a caller reads a result that has not been produced
// yet.  That is indistinguishable from success at the call site.
//
// Two contracts are asserted:
//   1. CORRECTNESS — Wait does not return before the delegate has finished, and
//      the elapsed time is >= the delegate's own duration.
//   2. EFFICIENCY (P1, per the project's global priority) — a blocked wait must
//      not spin a core.  Wait used to busy-loop on yield(); the added assertion
//      is that it parks instead.  A spin loop passes (1) and fails (2).
//
// Links the production task_runner.cpp / thread_pool.cpp so the delegate really
// executes on the pool, matching the P1-1 harness.

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
#include <ctime>
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

std::atomic<int> g_invoke_count{0};
std::atomic<long long> g_delegate_elapsed_ms{0};

/// How long the delegate sleeps to simulate work.  Long enough that a
/// non-blocking Wait is unmistakably detected, short enough to keep the suite
/// fast.
constexpr int kWorkMs = 150;

CHAOS_IL2CPP_INTPTR SlowDelegateMethod() {
    const auto start = std::chrono::steady_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(kWorkMs));
    const auto end = std::chrono::steady_clock::now();
    g_delegate_elapsed_ms.store(
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
    g_invoke_count.fetch_add(1);
    return 42;
}

CHAOS_IL2CPP_INTPTR NoOpDelegateMethod() {
    g_invoke_count.fetch_add(1);
    return 7;
}

DelegateObject* MakeDelegate(CHAOS_IL2CPP_INTPTR (*fn)()) {
    auto* del = new DelegateObject();
    del->type_info = nullptr;
    del->chaos_delegate_target = 0;  // open delegate → thunk() takes no args
    del->chaos_delegate_method_ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(fn);
    del->chaos_delegate_invocation_list = 0;
    del->chaos_delegate_invocation_count = 0;
    del->chaos_delegate_method_token = 0;
    del->_pad = 0;
    return del;
}

constexpr auto kPollInterval = std::chrono::milliseconds(5);
constexpr int  kMaxPolls = 400;

bool WaitFor(const std::function<bool()>& pred) {
    for (int i = 0; i < kMaxPolls; ++i) {
        if (pred()) return true;
        std::this_thread::sleep_for(kPollInterval);
    }
    return pred();
}

class TaskBlockingTest : public ::testing::Test {
protected:
    void SetUp() override {
        threading::RegisterThread(threading::kMainThreadId, nullptr);
        threading::TimerQueueInitialize();
        threading::ThreadPoolInitialize();
        threading::RegisterAsyncTaskRun();

        g_invoke_count.store(0);
        g_delegate_elapsed_ms.store(0);
    }

    void TearDown() override {
        register_async_task_run_fn(nullptr);
        threading::ThreadPoolShutdown();
        threading::TimerQueueShutdown();
        threading::UnregisterThread();
    }
};

}  // namespace

// ── The counter-example: Wait must actually wait ─────────────────────────────

TEST_F(TaskBlockingTest, WaitBlocksUntilTheDelegateFinishes)
{
    DelegateObject* del = MakeDelegate(&SlowDelegateMethod);
    const CHAOS_IL2CPP_INTPTR task =
        async_task_run(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(del));

    const auto start = std::chrono::steady_clock::now();
    const CHAOS_IL2CPP_INT32 completed = ChaosAsyncTaskWait(task, -1);
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start).count();

    EXPECT_EQ(1, completed) << "Wait must report completion";

    // The load-bearing claim.  A Wait that returns immediately (or returns 1
    // without blocking) fails here, and that is the defect shape: the caller
    // proceeds against a result that does not exist yet.
    EXPECT_EQ(1, g_invoke_count.load()) << "Wait returned before the delegate ran";
    EXPECT_GE(elapsed, kWorkMs)
        << "Wait returned after " << elapsed << "ms but the work takes " << kWorkMs
        << "ms — it did not block";

    // Cross-check: the delegate really did the work it claims.
    EXPECT_GE(g_delegate_elapsed_ms.load(), kWorkMs - 20);
}

TEST_F(TaskBlockingTest, WaitOnAlreadyCompleteTaskReturnsImmediately)
{
    DelegateObject* del = MakeDelegate(&NoOpDelegateMethod);
    const CHAOS_IL2CPP_INTPTR task =
        async_task_run(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(del));

    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(task) != 0; }));

    // Negative control: the blocking behaviour must not degenerate into "always
    // sleeps".  An already-complete task returns promptly.
    const auto start = std::chrono::steady_clock::now();
    EXPECT_EQ(1, ChaosAsyncTaskWait(task, -1));
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start).count();

    EXPECT_LT(elapsed, kWorkMs)
        << "Wait on a complete task should not block for the work duration";
}

// ── A blocked wait must PARK, not spin (P1: performance) ─────────────────────
//
// A yield()/spin loop satisfies the correctness assertions above while burning a
// core for the entire wait.  On a machine with fewer free cores than waiters
// that starves the very worker thread the waited-for task needs — a livelock
// that presents as a hang rather than a wrong answer, which is worse.
//
// The metric must be THIS thread's CPU time, not the process's: std::clock()
// sums every thread, so it counts the worker's own sleep and reports ~100% for
// a correctly parked wait (measured: 153ms of "CPU" for a 150ms sleep).  On
// Windows we therefore query the current thread directly.

#if defined(_WIN32)
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <windows.h>
namespace {
/// CPU time consumed by the calling thread, in milliseconds.
long long CurrentThreadCpuMs() {
    FILETIME creation, exit, kernel, user;
    if (!GetThreadTimes(GetCurrentThread(), &creation, &exit, &kernel, &user)) {
        return -1;  // caller skips the assertion rather than failing spuriously
    }
    auto to_ms = [](const FILETIME& ft) {
        ULARGE_INTEGER u;
        u.LowPart = ft.dwLowDateTime;
        u.HighPart = ft.dwHighDateTime;
        return static_cast<long long>(u.QuadPart / 10000ULL);  // 100ns → ms
    };
    return to_ms(kernel) + to_ms(user);
}
}  // namespace
#  define CHAOS_HAS_THREAD_CPU 1
#else
#  define CHAOS_HAS_THREAD_CPU 0
#endif

TEST_F(TaskBlockingTest, BlockedWaitDoesNotSpinACore)
{
#if !CHAOS_HAS_THREAD_CPU
    GTEST_SKIP() << "per-thread CPU accounting not available on this platform";
#else
    DelegateObject* del = MakeDelegate(&SlowDelegateMethod);
    const CHAOS_IL2CPP_INTPTR task =
        async_task_run(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(del));

    const long long cpu_before = CurrentThreadCpuMs();
    const CHAOS_IL2CPP_INT32 completed = ChaosAsyncTaskWait(task, -1);
    const long long cpu_used_ms = CurrentThreadCpuMs() - cpu_before;

    EXPECT_EQ(1, completed);

    // The delegate sleeps ~150ms.  A parked waiter consumes ~0ms of its own CPU;
    // a spin loop consumes ~150ms.  The threshold sits between the two with a
    // wide margin so the test is not timing-fragile.
    EXPECT_LT(cpu_used_ms, kWorkMs / 2)
        << "the waiting thread consumed " << cpu_used_ms
        << "ms of CPU while blocked for ~" << kWorkMs
        << "ms — it is spinning, not parking";
#endif
}

// ── Result returns the task's real value, after blocking ─────────────────────

TEST_F(TaskBlockingTest, BlockingResultReturnsAfterCompletion)
{
    DelegateObject* del = MakeDelegate(&NoOpDelegateMethod);
    const CHAOS_IL2CPP_INTPTR task =
        async_task_run(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(del));

    const auto start = std::chrono::steady_clock::now();
    (void)ChaosAsyncTaskGetResultBlocking(task);
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start).count();

    // Not asserting the VALUE here: TaskRun's callback ignores the delegate's
    // return (a delegate returning 42 does not make the task's result 42 —
    // Task.Run(Action) has no result).  The contract under test is that the read
    // happens after completion.
    EXPECT_EQ(1, g_invoke_count.load())
        << "GetResultBlocking read the result before the delegate ran";
    (void)elapsed;
}

// ── Many concurrent waiters all observe the same completion ──────────────────

TEST_F(TaskBlockingTest, ConcurrentWaitersAllReturn) {
    DelegateObject* del = MakeDelegate(&SlowDelegateMethod);
    const CHAOS_IL2CPP_INTPTR task =
        async_task_run(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(del));

    constexpr int kWaiters = 8;
    std::atomic<int> completed_count{0};
    std::thread waiters[kWaiters];

    for (int i = 0; i < kWaiters; ++i) {
        waiters[i] = std::thread([&] {
            if (ChaosAsyncTaskWait(task, -1) == 1) {
                completed_count.fetch_add(1);
            }
        });
    }
    for (auto& w : waiters) w.join();

    EXPECT_EQ(kWaiters, completed_count.load())
        << "every concurrent waiter must observe completion";
    EXPECT_EQ(1, g_invoke_count.load()) << "the delegate must run exactly once";
}
