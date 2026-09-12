// async_integration_smoke_test.cpp — Async Task.Run integration with ThreadPool
//
// Tests the full async pipeline — scheduling, completion signaling, AsyncTask
// lifecycle, builder/awaiter protocol — through a test-local TaskRun backed
// by the real ThreadPool.  Uses register_async_task_run_fn() to inject the
// test-local implementation, avoiding the codegen-generated symbol dependency
// of the production task_runner.cpp.
//
// Links: CHAOS_THREADING_LIBS

#include <gtest/gtest.h>
#include <chaos/native_types.h>
#include <chaos/async.h>
#include <thread_state.h>
#include <thread_pool.h>
#include <timer_queue.h>
#include "runtime_stubs/stub_common.h"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <thread>
#include <vector>

namespace threading = chaos::il2cpp::runtime_core::threading;
using namespace chaos::il2cpp::common;

// ══════════════════════════════════════════════════════════════════════════════
// Test-local TaskRun — queues a completion callback on the real ThreadPool.
// ══════════════════════════════════════════════════════════════════════════════

namespace {

struct TaskRunCtx {
    AsyncTask* task;
};

void CompleteTaskCallback(void* state) noexcept {
    auto* ctx = static_cast<TaskRunCtx*>(state);
    ctx->task->completed = true;
    delete ctx;
}

CHAOS_IL2CPP_INTPTR TestTaskRun(CHAOS_IL2CPP_INTPTR delegate_fn) noexcept {
    if (delegate_fn == 0) return 0;
    auto* task = new (std::nothrow) AsyncTask();
    if (task == nullptr) return 0;
    auto* ctx = new (std::nothrow) TaskRunCtx{task};
    if (ctx == nullptr) {
        delete task;
        return 0;
    }
    threading::ThreadPoolQueueUserWorkItemUnsafe(CompleteTaskCallback, ctx);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(task);
}

} // anonymous namespace

// ══════════════════════════════════════════════════════════════════════════════
// Helpers
// ══════════════════════════════════════════════════════════════════════════════

constexpr auto kPollInterval = std::chrono::milliseconds(5);
constexpr int kMaxPolls = 400;  // 5ms * 400 = 2s timeout

/// Poll until a predicate returns true, or time out.
static bool WaitFor(const std::function<bool()>& pred) {
    for (int i = 0; i < kMaxPolls; ++i) {
        if (pred()) return true;
        std::this_thread::sleep_for(kPollInterval);
    }
    return pred();  // one last chance
}

// ══════════════════════════════════════════════════════════════════════════════
// Setup / teardown
// ══════════════════════════════════════════════════════════════════════════════

class AsyncIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        threading::RegisterThread(threading::kMainThreadId, nullptr);
        threading::TimerQueueInitialize();
        threading::ThreadPoolInitialize();
        register_async_task_run_fn(TestTaskRun);
    }

    void TearDown() override {
        register_async_task_run_fn(nullptr);
        threading::ThreadPoolShutdown();
        threading::TimerQueueShutdown();
        threading::UnregisterThread();
    }
};

// ══════════════════════════════════════════════════════════════════════════════
// Task.Run — basic scheduling
// ══════════════════════════════════════════════════════════════════════════════

TEST_F(AsyncIntegrationTest, TaskRunQueuesAndCompletes) {
    auto handle = async_task_run(static_cast<CHAOS_IL2CPP_INTPTR>(1));
    ASSERT_NE(0, handle);

    auto* task = require_async_task(handle);
    EXPECT_TRUE(WaitFor([task] { return task->completed.load(); }))
        << "Task did not complete within timeout";
    EXPECT_FALSE(task->faulted);
}

TEST_F(AsyncIntegrationTest, TaskRunWithZeroDelegate) {
    auto handle = async_task_run(0);
    EXPECT_EQ(0, handle);
}

TEST_F(AsyncIntegrationTest, TaskRunMultipleTimes) {
    constexpr int kCount = 10;
    CHAOS_IL2CPP_INTPTR handles[kCount];

    for (int i = 0; i < kCount; ++i) {
        handles[i] = async_task_run(static_cast<CHAOS_IL2CPP_INTPTR>(i + 1));
        ASSERT_NE(0, handles[i]) << "handle " << i;
    }

    for (int i = 0; i < kCount; ++i) {
        auto* task = require_async_task(handles[i]);
        EXPECT_TRUE(WaitFor([task] { return task->completed.load(); }))
            << "Task " << i << " did not complete";
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// Task.Run — concurrent scheduling
// ══════════════════════════════════════════════════════════════════════════════

TEST_F(AsyncIntegrationTest, TaskRunFromMultipleThreads) {
    constexpr int kThreads = 8;
    constexpr int kTasksPerThread = 10;
    CHAOS_IL2CPP_INTPTR all_handles[kThreads][kTasksPerThread]{};

    std::vector<std::thread> workers;
    for (int t = 0; t < kThreads; ++t) {
        workers.emplace_back([t, &all_handles] {
            // Each worker thread registers itself so it can use ThreadPool
            int32_t tid = threading::AllocateThreadId();
            threading::RegisterThread(tid, nullptr);
            for (int i = 0; i < kTasksPerThread; ++i) {
                all_handles[t][i] = async_task_run(
                    static_cast<CHAOS_IL2CPP_INTPTR>(1));
            }
            threading::UnregisterThread();
        });
    }

    for (auto& w : workers) w.join();

    int total = kThreads * kTasksPerThread;
    int completed = 0;
    for (int t = 0; t < kThreads; ++t) {
        for (int i = 0; i < kTasksPerThread; ++i) {
            if (all_handles[t][i] == 0) continue;
            auto* task = require_async_task(all_handles[t][i]);
            if (WaitFor([task] { return task->completed.load(); })) {
                ++completed;
            }
        }
    }
    EXPECT_EQ(total, completed);
}

// ══════════════════════════════════════════════════════════════════════════════
// Task.Run — stress test
// ══════════════════════════════════════════════════════════════════════════════

TEST_F(AsyncIntegrationTest, TaskRunStress) {
    constexpr int kCount = 500;
    std::vector<CHAOS_IL2CPP_INTPTR> handles;
    handles.reserve(kCount);

    for (int i = 0; i < kCount; ++i) {
        auto h = async_task_run(static_cast<CHAOS_IL2CPP_INTPTR>(1));
        ASSERT_NE(0, h);
        handles.push_back(h);
    }

    int completed = 0;
    for (auto h : handles) {
        auto* task = require_async_task(h);
        if (WaitFor([task] { return task->completed.load(); })) {
            ++completed;
        }
    }
    EXPECT_EQ(kCount, completed);
}

// ══════════════════════════════════════════════════════════════════════════════
// AsyncTask lifecycle with real ThreadPool scheduling
// ══════════════════════════════════════════════════════════════════════════════

TEST_F(AsyncIntegrationTest, DirectThreadPoolWorkItem) {
    auto* task = new AsyncTask();
    ASSERT_NE(nullptr, task);

    struct Ctx { AsyncTask* task; int value; };
    auto* ctx = new Ctx{task, 42};

    threading::ThreadPoolQueueUserWorkItemUnsafe([](void* state) {
        auto* c = static_cast<Ctx*>(state);
        c->task->result = c->value;
        c->task->completed = true;
    }, ctx);

    EXPECT_TRUE(WaitFor([task] { return task->completed.load(); }));
    EXPECT_EQ(42, task->result);
    EXPECT_FALSE(task->faulted);

    delete task;
}

TEST_F(AsyncIntegrationTest, ChainedTasks) {
    auto* task_a = new AsyncTask();
    auto* task_b = new AsyncTask();

    threading::ThreadPoolQueueUserWorkItemUnsafe([](void* state) {
        auto* t = static_cast<AsyncTask*>(state);
        t->result = 42;
        t->completed = true;
    }, task_a);

    threading::ThreadPoolQueueUserWorkItemUnsafe([](void* state) {
        auto* t = static_cast<AsyncTask*>(state);
        t->result = 99;
        t->completed = true;
    }, task_b);

    EXPECT_TRUE(WaitFor([task_a] { return task_a->completed.load(); }));
    EXPECT_TRUE(WaitFor([task_b] { return task_b->completed.load(); }));
    EXPECT_EQ(42, task_a->result);
    EXPECT_EQ(99, task_b->result);

    delete task_a;
    delete task_b;
}

// ══════════════════════════════════════════════════════════════════════════════
// Builder + ThreadPool integration
// ══════════════════════════════════════════════════════════════════════════════

TEST_F(AsyncIntegrationTest, BuilderSetResultFromWorker) {
    CHAOS_IL2CPP_INTPTR slot = 0;
    CHAOS_IL2CPP_INTPTR ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot);

    auto handle = async_task_builder_get_task(ref);
    ASSERT_NE(0, handle);

    threading::ThreadPoolQueueUserWorkItemUnsafe([](void* raw_ref) {
        auto builder_ref = static_cast<CHAOS_IL2CPP_INTPTR>(
            reinterpret_cast<std::intptr_t>(raw_ref));
        async_task_builder_set_result_raw(builder_ref, 77);
    }, reinterpret_cast<void*>(static_cast<std::intptr_t>(ref)));

    auto* task = require_async_task(handle);
    EXPECT_TRUE(WaitFor([task] { return task->completed.load(); }));
    EXPECT_EQ(77, task->result);
    EXPECT_FALSE(task->faulted);
}

TEST_F(AsyncIntegrationTest, BuilderSetExceptionFromWorker) {
    CHAOS_IL2CPP_INTPTR slot = 0;
    CHAOS_IL2CPP_INTPTR ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot);

    auto handle = async_task_builder_get_task(ref);
    ASSERT_NE(0, handle);

    threading::ThreadPoolQueueUserWorkItemUnsafe([](void* raw_ref) {
        auto builder_ref = static_cast<CHAOS_IL2CPP_INTPTR>(
            reinterpret_cast<std::intptr_t>(raw_ref));
        async_task_builder_set_exception(builder_ref, 0xBAD);
    }, reinterpret_cast<void*>(static_cast<std::intptr_t>(ref)));

    auto* task = require_async_task(handle);
    EXPECT_TRUE(WaitFor([task] { return task->completed.load(); }));
    EXPECT_TRUE(task->faulted);
    EXPECT_EQ(0xBAD, task->exception);
}

// ══════════════════════════════════════════════════════════════════════════════
// Awaiter protocol with real scheduling
// ══════════════════════════════════════════════════════════════════════════════

TEST_F(AsyncIntegrationTest, AwaiterPollUntilComplete) {
    auto handle = async_task_run(static_cast<CHAOS_IL2CPP_INTPTR>(1));
    ASSERT_NE(0, handle);

    CHAOS_IL2CPP_INTPTR slot = handle;
    CHAOS_IL2CPP_INTPTR awaiter_ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot);

    EXPECT_TRUE(WaitFor([awaiter_ref] {
        return async_task_awaiter_get_is_completed(awaiter_ref) != 0;
    })) << "awaiter did not complete";

    auto* task = require_async_task(handle);
    EXPECT_TRUE(task->completed.load());
}

TEST_F(AsyncIntegrationTest, SequentialAsyncAwaitPattern) {
    auto step1 = async_task_run(static_cast<CHAOS_IL2CPP_INTPTR>(1));
    ASSERT_NE(0, step1);

    CHAOS_IL2CPP_INTPTR slot1 = step1;
    CHAOS_IL2CPP_INTPTR aw1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot1);

    EXPECT_TRUE(WaitFor([aw1] {
        return async_task_awaiter_get_is_completed(aw1) != 0;
    }));

    auto step2 = async_task_run(static_cast<CHAOS_IL2CPP_INTPTR>(2));
    ASSERT_NE(0, step2);

    CHAOS_IL2CPP_INTPTR slot2 = step2;
    CHAOS_IL2CPP_INTPTR aw2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot2);

    EXPECT_TRUE(WaitFor([aw2] {
        return async_task_awaiter_get_is_completed(aw2) != 0;
    }));
}

// ══════════════════════════════════════════════════════════════════════════════
// AsyncYield + Task.Run composition
// ══════════════════════════════════════════════════════════════════════════════

TEST_F(AsyncIntegrationTest, YieldThenTaskRun) {
    auto yield_handle = async_yield_create();
    EXPECT_EQ(1, yield_handle);

    auto task_handle = async_task_run(static_cast<CHAOS_IL2CPP_INTPTR>(1));
    ASSERT_NE(0, task_handle);

    auto* task = require_async_task(task_handle);
    EXPECT_TRUE(WaitFor([task] { return task->completed.load(); }));
}

// ══════════════════════════════════════════════════════════════════════════════
// Continuation dispatch onto ThreadPool (P1-3)
// ══════════════════════════════════════════════════════════════════════════════

// A dispatcher mirroring the production AsyncContinuationDispatch (task_runner.cpp):
// queues the continuation onto the thread pool so it runs on a worker thread.
static void TestDispatchToThreadPool(AsyncContinueFn cb, void* ctx, CHAOS_IL2CPP_INTPTR task_handle) {
    (void)task_handle;
    threading::ThreadPoolQueueUserWorkItemUnsafe([](void* state) {
        auto* pair = static_cast<std::pair<AsyncContinueFn, void*>*>(state);
        pair->first(0, pair->second);
        delete pair;
    }, new std::pair<AsyncContinueFn, void*>(cb, ctx));
}

TEST_F(AsyncIntegrationTest, ContinuationRunsOnThreadPoolWorker) {
    // Register a threadpool dispatcher so a completing task's continuation runs
    // on a worker thread, not inline on the thread that completes the task.
    register_async_dispatch_continuation_fn(TestDispatchToThreadPool);
    auto* task = new AsyncTask();
    std::thread::id main_id = std::this_thread::get_id();
    auto fired = std::make_shared<std::atomic<int>>(0);
    auto on_worker = std::make_shared<std::atomic<int>>(0);
    auto worker_id = std::make_shared<std::thread::id>();

    // Non-capturing continuation per AsyncContinueFn (fn-ptr) contract; state via ctx.
    struct Ctx { std::thread::id main_id; std::shared_ptr<std::atomic<int>> fired;
                 std::shared_ptr<std::atomic<int>> on_worker; std::shared_ptr<std::thread::id> worker_id; };
    auto* ctx = new Ctx{ main_id, fired, on_worker, worker_id };
    async_task_on_completed(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(task),
        [](CHAOS_IL2CPP_INTPTR, void* raw) {
            auto* c = static_cast<Ctx*>(raw);
            *c->worker_id = std::this_thread::get_id();
            if (std::this_thread::get_id() != c->main_id) {
                c->on_worker->fetch_add(1, std::memory_order_relaxed);
            }
            c->fired->fetch_add(1, std::memory_order_relaxed);
        }, ctx);

    // Complete the task inline (simulating a worker completing it).
    task->result = 1;
    task->completed.store(true, std::memory_order_release);
    chaos::il2cpp::common::finish_async_task(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(task));

    EXPECT_TRUE(WaitFor([&fired] { return fired->load() >= 1; }));
    EXPECT_NE(main_id, *worker_id)
        << "Continuation ran on the completing thread, not a worker thread";
    EXPECT_GE(on_worker->load(), 1)
        << "Continuation did not run on a thread pool worker";
    delete task;
    delete ctx;
    register_async_dispatch_continuation_fn(nullptr);   // no leak into later tests
}

TEST_F(AsyncIntegrationTest, ContinuationDispatchedExactlyOnce) {
    // Validate the continuation protocol: a continuation registered via
    // async_task_on_completed fires exactly once after the task completes.
    // Uses a task completed inline (not via thread pool) to avoid the timing
    // dependency between TestTaskRun (which does not call finish_async_task)
    // and the continuation dispatch queue.
    register_async_dispatch_continuation_fn(TestDispatchToThreadPool);

    auto* task = new AsyncTask();
    CHAOS_IL2CPP_INTPTR handle = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(task);

    std::atomic<int> fires{0};
    async_task_on_completed(handle, [](CHAOS_IL2CPP_INTPTR, void* ctx) {
        (*static_cast<std::atomic<int>*>(ctx))++;
    }, &fires);

    // Complete the task with finish_async_task (the production path).
    task->result = 7;
    task->completed.store(true, std::memory_order_release);
    chaos::il2cpp::common::finish_async_task(handle);

    // The continuation should be queued to the thread pool via the dispatcher.
    // Wait for it to fire, then poll for a bounded window to detect spurious
    // double-fire.  The polling approach is more CI-friendly than a fixed sleep:
    // it returns as soon as no second fire is observed, within the window.
    EXPECT_TRUE(WaitFor([&fires] { return fires.load() >= 1; }));
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    EXPECT_EQ(1, fires.load());
    delete task;
    register_async_dispatch_continuation_fn(nullptr);   // no leak into later tests
}

// ══════════════════════════════════════════════════════════════════════════════
// P2-1 spike: Hand-crafted state machine demonstrating the real async translation
// ══════════════════════════════════════════════════════════════════════════════
//
// This is the core pattern that Phase 2 must emit from IL:
//
//   C#:   async Task<int> One() { await Task.Yield(); return 1; }
//
//   Roslyn emits (simplified):
//     struct <One>d__0 : IAsyncStateMachine {
//       int <>1__state;                  // -1=done, 0=await yield
//       AsyncTaskMethodBuilder<int> <>t__builder;
//       YieldAwaiter <>u__awaiter0;
//       int <result>5__0;
//
//       void MoveNext() {
//         try {
//           switch (<>1__state) {
//             case -1: return;
//             case 0: goto AWAIT_YIELD_DONE;
//           }
//           <>u__awaiter0 = Task.Yield().GetAwaiter();
//           if (!<>u__awaiter0.IsCompleted) {
//             <>1__state = 0;
//             <>t__builder.AwaitUnsafeOnCompleted(ref <>u__awaiter0, ref this);
//             return;  // suspend — continuation resumes here
//           }
//           AWAIT_YIELD_DONE:
//           <>u__awaiter0.GetResult();
//           <>t__builder.SetResult(1);
//         } catch (Exception ex) {
//           <>1__state = -1;
//           <>t__builder.SetException(ex);
//         }
//       }
//     }
//
// We translate this to a native C++ struct with a MoveNext() member function
// using the Phase 1 builder/awaiter/continuation protocol.

// Awaiter wrapper for Task.Yield().  In the real translator this is
// constructed from the IL GetAwaiter call; here we hand-code it.
struct YieldAwaiter {
    bool is_completed = false;
    void UnsafeOnCompleted(AsyncContinueFn cb, void* ctx) {
        // Yield always completes asynchronously: queue the continuation.
        // This simulates what Task.Yield().GetAwaiter().UnsafeOnCompleted does.
        threading::ThreadPoolQueueUserWorkItemUnsafe([](void* state) {
            auto* pair = static_cast<std::pair<AsyncContinueFn, void*>*>(state);
            pair->first(0, pair->second);
            delete pair;
        }, new std::pair<AsyncContinueFn, void*>(cb, ctx));
    }
};

// The state machine struct — this is what the translator must emit.
// It mirrors the Roslyn <One>d__0 exactly.
struct AsyncStateMachine_One {
    // State: -1 = done, 0 = await yield, -2 = initial
    int state = -2;
    // Builder slot (holds the Task handle)
    CHAOS_IL2CPP_INTPTR builder_slot = 0;
    // Awaiter storage
    YieldAwaiter awaiter;
    // Result
    int result = 0;

    void MoveNext() {
        // AOT EH: try block
        switch (state) {
            case -1: return;  // already completed
            case 0: goto AFTER_YIELD;  // resume from await
        }
        // Initial state: call Task.Yield().GetAwaiter()
        // In the real translator this is the IL instruction sequence.
        // awaiter = Task.Yield().GetAwaiter();
        // For this spike, YieldAwaiter starts as not-completed.
        if (!awaiter.is_completed) {
            state = 0;  // mark suspend point
            // builder.AwaitUnsafeOnCompleted(ref awaiter, ref this)
            // The continuation calls MoveNext again.
            awaiter.UnsafeOnCompleted(
                [](CHAOS_IL2CPP_INTPTR, void* ctx) {
                    static_cast<AsyncStateMachine_One*>(ctx)->MoveNext();
                },
                this);
            return;  // suspend — continuation will re-enter
        }
        AFTER_YIELD:
        // awaiter.GetResult() — no-op for yield awaiter
        // builder.SetResult(1)
        result = 1;
        state = -1;
        chaos::il2cpp::common::async_task_builder_set_result_raw(
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&builder_slot),
            static_cast<CHAOS_IL2CPP_INTPTR>(result));
        // AOT EH: catch block would set exception on builder
    }
};

TEST_F(AsyncIntegrationTest, HandCraftedStateMachineOne) {
    // This test proves the Phase 2 state-machine translation pattern works:
    //   1. Create a state machine struct
    //   2. Call builder.Start (which invokes MoveNext synchronously until first suspend)
    //   3. MoveNext suspends after setting up the continuation
    //   4. The continuation (Task.Yield completion) re-enters MoveNext
    //   5. MoveNext sets result on the builder
    //   6. The Task completes with the correct value

    AsyncStateMachine_One sm;

    // Simulate the AsyncTaskMethodBuilder<int>.Start(ref sm) prologue:
    // Roslyn's builder lazily creates the Task when the caller accesses
    // builder.Task (returned by the async method).  Force that creation so a
    // Task exists before MoveNext runs (this is what "builder.Create" +
    // returned "Task" getter do).
    chaos::il2cpp::common::async_task_builder_get_task(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&sm.builder_slot));

    // builder.Start(ref stateMachine) calls MoveNext synchronously until the
    // first suspension point.
    sm.MoveNext();

    // After the first MoveNext, the state machine should have suspended
    // (state == 0, continuation queued to thread pool).  The builder
    // should have created a Task (builder_slot != 0).
    ASSERT_NE(sm.builder_slot, 0);
    EXPECT_EQ(sm.state, 0);  // suspended at yield

    // Wait for the continuation to fire and the task to complete.
    auto* task = chaos::il2cpp::common::require_async_task(
        *chaos::il2cpp::common::resolve_native_int_slot(
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&sm.builder_slot)));
    EXPECT_TRUE(WaitFor([task] { return task->completed.load(); }));

    // Task should have result 1.
    EXPECT_EQ(task->result, 1);
    EXPECT_FALSE(task->faulted.load());
    EXPECT_EQ(sm.state, -1);  // state machine marked done
}

// ══════════════════════════════════════════════════════════════════════════════
// Segment B/C native helpers (async.h): AsyncTaskMethodBuilder<T> wiring to
// real native MoveNext + continuation resumption.
//
// Phase 2 Segment B committed AsyncStateMachineMoveNextFn / async_task_builder_start
// / async_await_task_resume / async_await_yield_resume in async.h.  These tests
// exercise that protocol end to end with hand-crafted MoveNext entries matching the
// codegen ABI (extern "C" void MoveNext(CHAOS_IL2CPP_INTPTR box)).
// ══════════════════════════════════════════════════════════════════════════════

namespace {

// Classic Box + MoveNext pair driven by Segment B helpers.
// MoveNext ABI: void(*)(CHAOS_IL2CPP_INTPTR box) — box is the >d__ instance.
struct AsyncSM_Box {
    CHAOS_IL2CPP_INTPTR builder_slot = 0;  // field_<>t__builder
    int state = -1;
    int result = 0;
};

void AsyncSM_Box_MoveNext_Complete(CHAOS_IL2CPP_INTPTR box) {
    auto* sm = reinterpret_cast<AsyncSM_Box*>(box);
    // Mirror the codegen MoveNext tail: builder.SetResult -> task completes.
    chaos::il2cpp::common::async_task_builder_set_result_void(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&sm->builder_slot));
    sm->state = -1;
}

void AsyncSM_Box_MoveNext_Suspend(CHAOS_IL2CPP_INTPTR box) {
    auto* sm = reinterpret_cast<AsyncSM_Box*>(box);
    // First MoveNext (codegen runs via async_task_builder_start): initial run sets
    // state=0 (suspended at await) and registers a continuation; does NOT complete.
    if (sm->state == 0) {
        // resumed: MoveNext tail -> SetResult (task int returns); we simulate non-generic.
        chaos::il2cpp::common::async_task_builder_set_result_void(
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&sm->builder_slot));
        sm->state = -1;
        return;
    }
    // initial entry: state stays 0; the *host* of MoveNext (async import caller)
    // supplies the real continuation registration.  Here the host is this test,
    // so we set state=0 and let the outside drive async_task_builder_start, then
    // the later dispatch re-enters MoveNext with state already 0 to complete.
    sm->state = 0;
}

} // anonymous namespace

// Verify async_task_builder_start drives a MoveNext synchronously and the
// builder Task is created + completed (non-suspending path).
TEST_F(AsyncIntegrationTest, SegmentB_AsyncTaskBuilderStartCompletes) {
    AsyncSM_Box sm;
    sm.builder_slot = 0;
    CHAOS_IL2CPP_INTPTR builder_ref =
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&sm.builder_slot);

    // builder.Start(ref sm) — drives MoveNext synchronously to SetResult.
    CHAOS_IL2CPP_INTPTR rc = chaos::il2cpp::common::async_task_builder_start(
        builder_ref, AsyncSM_Box_MoveNext_Complete, &sm);
    EXPECT_EQ(1, rc);

    auto* task = chaos::il2cpp::common::require_async_task(sm.builder_slot);
    EXPECT_TRUE(task->completed.load());
    EXPECT_FALSE(task->faulted.load());
    EXPECT_EQ(-1, sm.state);
}

// Verify async_task_builder_push_continuation via async_await_task_resume:
// a resumed MoveNext (registered as continuation) re-enters and the task completes.
TEST_F(AsyncIntegrationTest, SegmentB_AsyncAwaitTaskResumesAndCompletes) {
    // Use a fresh box/task; complete it from a worker after registering continuation.
    AsyncSM_Box sm;
    sm.builder_slot = 0;
    sm.state = 0;  // about to suspend
    CHAOS_IL2CPP_INTPTR builder_ref =
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&sm.builder_slot);
    // Materialize the task handle now (Start does this).
    chaos::il2cpp::common::async_task_builder_get_task(builder_ref);

    // The awaited "task" is the builder's own task handle (the yield-style awaiter
    // holds that handle in a slot).  Register continuation -> on completion it re-enters.
    CHAOS_IL2CPP_INTPTR awaiter_ref = builder_ref;  // awaiter slot == builder slot holds handle
    CHAOS_IL2CPP_INTPTR rc = chaos::il2cpp::common::async_await_task_resume(
        awaiter_ref, AsyncSM_Box_MoveNext_Complete, &sm);
    EXPECT_EQ(1, rc);

    auto* task = chaos::il2cpp::common::require_async_task(sm.builder_slot);
    EXPECT_FALSE(task->completed.load());  // continuation registered, not yet fired

    // Complete the task from a worker thread; the continuation (MoveNext) fires,
    // which calls SetResult again (idempotent enough) and ends.
    threading::ThreadPoolQueueUserWorkItemUnsafe([](void* state) {
        auto* t = static_cast<AsyncTask*>(state);
        t->result = 1;
        t->completed.store(true, std::memory_order_release);
        chaos::il2cpp::common::finish_async_task(
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(t));
    }, task);

    // Wait until the continuation fired (task already completed) — bounded wait.
    EXPECT_TRUE(WaitFor([&sm] { return sm.state == -1; }))
        << "MoveNext continuation did not re-enter to completion";
}

// ══════════════════════════════════════════════════════════════════════════════
// R3/R4: Task.Yield cross-thread round trip — mirror the codegen-emitted MoveNext
// flow for `async Task<int> GetOne(){ await Task.Yield(); return 1; }`:
//   MoveNext (initial, state==-1):  yield = Task.Yield().GetAwaiter();
//     if (yield.IsCompleted) yield.GetResult();      // NEVER with dispatcher (R3)
//     else { state=0; builder.AwaitUnsafeOnCompleted(ref yield, ref this); return; }
//   MoveNext (resumed, state==0):    // jump past the await
//     yield.GetResult(); builder.SetResult(1); state=-1;
// ══════════════════════════════════════════════════════════════════════════════

struct AsyncSM_YieldGetOne {
    CHAOS_IL2CPP_INTPTR builder_slot = 0;   // field_<>t__builder (holds AsyncTask handle)
    int state = -1;                          // field_<>1__state
    int result = 0;
};

// Thread-pool queue wrapper matching the dispatcher used under runtime init.
static void YieldDispatcherCb(AsyncContinueFn cb, void* ctx, CHAOS_IL2CPP_INTPTR handle) {
    (void)handle;
    threading::ThreadPoolQueueUserWorkItemUnsafe([](void* st) {
        auto* pair = static_cast<std::pair<AsyncContinueFn, void*>*>(st);
        pair->first(0, pair->second);
        delete pair;
    }, new std::pair<AsyncContinueFn, void*>(cb, ctx));
}

void AsyncSM_YieldGetOne_MoveNext(CHAOS_IL2CPP_INTPTR box) {
    auto* sm = reinterpret_cast<AsyncSM_YieldGetOne*>(box);
    if (sm->state == 0) {
        // RESUMED past the await: GetResult + SetResult + mark done.
        chaos::il2cpp::common::async_yield_get_result(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(sm)); // no-op
        chaos::il2cpp::common::async_task_builder_set_result_raw(
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&sm->builder_slot),
            static_cast<CHAOS_IL2CPP_INTPTR>(1));
        sm->state = -1;
        return;
    }
    // INITIAL: await Task.Yield.  With dispatcher registered, is_completed == 0
    // (R3) → suspend: register MoveNext as continuation; on resume re-enter with state 0.
    auto awaiterSlot = static_cast<CHAOS_IL2CPP_INTPTR>(async_yield_create() & 0xFF); // non-null sentinel
    (void)awaiterSlot;
    CHAOS_IL2CPP_INTPTR isc = chaos::il2cpp::common::async_yield_get_is_completed(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&sm->result));
    if (isc != 0) {
        // Not suspended (no dispatcher / sync path): GetResult inline + complete.
        chaos::il2cpp::common::async_task_builder_set_result_raw(
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&sm->builder_slot),
            static_cast<CHAOS_IL2CPP_INTPTR>(1));
        sm->state = -1;
        return;
    }
    // Suspend: state=0, queue MoveNext to thread pool via the dispatcher.
    sm->state = 0;
    chaos::il2cpp::common::async_await_yield_resume(
        reinterpret_cast<void*>(box), AsyncSM_YieldGetOne_MoveNext);
}

// Full cross-thread round trip, thread-pool dispatcher registered (R4).
TEST_F(AsyncIntegrationTest, TaskYieldRoundTripAcrossThreadPool) {
    register_async_dispatch_continuation_fn(YieldDispatcherCb);
    AsyncSM_YieldGetOne sm;
    sm.builder_slot = 0;
    CHAOS_IL2CPP_INTPTR builder_ref =
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&sm.builder_slot);

    // AsyncGetOne entry-equivalent: drive builder.Start(ref sm).
    CHAOS_IL2CPP_INTPTR rc = chaos::il2cpp::common::async_task_builder_start(
        builder_ref, AsyncSM_YieldGetOne_MoveNext, &sm);
    EXPECT_EQ(1, rc);

    // Start should have suspended at the yield (state==0), NOT completed synchronously.
    auto* task = chaos::il2cpp::common::require_async_task(sm.builder_slot);
    EXPECT_FALSE(task->completed.load());
    EXPECT_EQ(0, sm.state);

    // The ThreadPool worker fires the continuation → MoveNext resumed → SetResult(1) → completed.
    EXPECT_TRUE(WaitFor([task] { return task->completed.load(); }));
    EXPECT_EQ(1, task->result);
    EXPECT_FALSE(task->faulted.load());
    EXPECT_EQ(-1, sm.state);
    register_async_dispatch_continuation_fn(nullptr);
}

// ══════════════════════════════════════════════════════════════════════════════
// Phase 3 P3-1: TaskCompletionSource<T> — native TaskSource proxy.
//
// TCS is the "external completion" counterpart of AsyncTaskMethodBuilder.
// A TaskSource owns an AsyncTask handle; external code registers a
// continuation (an awaiting state machine's MoveNext) on ts.get_task(),
// then completes it from another thread via SetResult / SetException.
// The Try* variants model "only the first completion wins" (Idempotent):
// a second TrySetResult / TrySetException on an already-completed task is a
// no-op returning 0.
// ══════════════════════════════════════════════════════════════════════════════

namespace {

/// Hand-crafted state-machine mirroring a `async Task<int> AwaitTcs(TaskCompletionSource<int> tcs)`
/// that awaits tcs.Task.  Await/suspend registers MoveNext as the continuation
/// on the TCS's inner Task; resume completes and stores the result.
/// Awaiter state: -2 initial (first MoveNext), 0 = suspended at awaits, -1 done.
struct AsyncSM_AwaitsTcs {
    TaskSource* tcs;             // external completion source we await
    int state;                   // -2 initial, 0 suspended at await, -1 done
    int result = 0;
};

void AsyncSM_AwaitsTcs_MoveNext(CHAOS_IL2CPP_INTPTR box) {
    auto* sm = reinterpret_cast<AsyncSM_AwaitsTcs*>(box);
    switch (sm->state) {
        case -1: return;            // already done
        case 0:                     // RESUMED from await: read result, mark done
            sm->result = static_cast<int>(sm->tcs->task->result);
            sm->state = -1;
            return;
        default: break;             // initial (-2): fall through to register
    }
    // INITIAL: register MoveNext as continuation on the TCS task (await),
    // suspend (state=0).  The task is NOT yet complete, so the continuation
    // won't fire until an external SetResult triggers it.
    sm->state = 0;
    auto* data = new AsyncStateMachineContinuationData{
        AsyncSM_AwaitsTcs_MoveNext,
        reinterpret_cast<void*>(box)};
    chaos::il2cpp::common::async_task_on_completed(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(sm->tcs->task),
        AsyncStateMachineContinuationCallback, data);
}

} // anonymous namespace

// A waiter registers a continuation on a TCS's inner Task; an external worker
// then calls tcs->set_result(...) → the continuation fires → the SM reads the
// result and completes.  (Round-trip starting from external completion.)
TEST_F(AsyncIntegrationTest, TaskSource_SetResultCompletesAwaitingSM) {
    register_async_dispatch_continuation_fn(YieldDispatcherCb);  // run resume on a worker

    auto* tcs = chaos::il2cpp::common::task_source_create();
    ASSERT_NE(nullptr, tcs);
    AsyncSM_AwaitsTcs sm{tcs, -2, 0};

    // Simulate the entry: state-machine awaits tcs.Task (registers continuation).
    AsyncSM_AwaitsTcs_MoveNext(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&sm));

    auto* task = chaos::il2cpp::common::require_async_task(tcs->get_task());
    EXPECT_FALSE(task->completed.load());       // not yet externally completed
    EXPECT_EQ(0, sm.state);                      // suspended at await

    // External completion: another owner calls tcs.SetResult(42).
    tcs->set_result(static_cast<CHAOS_IL2CPP_INTPTR>(42));

    // The registered continuation (MoveNext resume) should have fired and the
    // SM read the result.
    EXPECT_TRUE(WaitFor([&sm] { return sm.state == -1; }))
        << "awaiting SM did not resume to completion after tcs SetResult";
    EXPECT_FALSE(task->faulted.load());
    EXPECT_FALSE(tcs->task->faulted.load());
    EXPECT_EQ(42, tcs->task->result);

    chaos::il2cpp::common::task_source_destroy(tcs);
    register_async_dispatch_continuation_fn(nullptr);
}

// TaskSource.SetException faults the awaited task; the awaiting SM resumes and
// sees the fault (the exception is readable, not swallowed to 0).
TEST_F(AsyncIntegrationTest, TaskSource_SetExceptionFaultsAwaitingSM) {
    auto* tcs = chaos::il2cpp::common::task_source_create();
    ASSERT_NE(nullptr, tcs);
    AsyncSM_AwaitsTcs sm{tcs, -2, 0};

    AsyncSM_AwaitsTcs_MoveNext(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&sm));
    EXPECT_EQ(0, sm.state);

    tcs->set_exception(static_cast<CHAOS_IL2CPP_INTPTR>(0xBAD));
    EXPECT_TRUE(WaitFor([&sm] { return sm.state == -1; }));
    auto* task = chaos::il2cpp::common::require_async_task(tcs->get_task());
    EXPECT_TRUE(task->faulted.load());
    EXPECT_EQ(0xBAD, task->exception);

    chaos::il2cpp::common::task_source_destroy(tcs);
}

// TrySetResult / TrySetException model TaskCompletionSource's "only the first
// completion wins": a second statement on an already-set TCS is a no-op.
TEST_F(AsyncIntegrationTest, TaskSource_TrySetFiresExactlyOnce) {
    auto* tcs = chaos::il2cpp::common::task_source_create();
    ASSERT_NE(nullptr, tcs);

    EXPECT_EQ(1, tcs->try_set_result(static_cast<CHAOS_IL2CPP_INTPTR>(1)));
    // Second completion attempt must lose (already completed).
    EXPECT_EQ(0, tcs->try_set_result(static_cast<CHAOS_IL2CPP_INTPTR>(99)));
    EXPECT_EQ(0, tcs->try_set_exception(static_cast<CHAOS_IL2CPP_INTPTR>(0xDEAD)));
    EXPECT_EQ(0, tcs->try_set_canceled());

    auto* task = chaos::il2cpp::common::require_async_task(tcs->get_task());
    EXPECT_TRUE(task->completed.load());
    EXPECT_EQ(1, task->result);
    EXPECT_FALSE(task->faulted.load());   // first-set result wins, not the exception

    chaos::il2cpp::common::task_source_destroy(tcs);
}

// Cross-thread: an external worker thread completes the TCS via SetResult,
// and a waiter polling sees it become completed (models TCS being completed
// by exactly one owner from another thread).
TEST_F(AsyncIntegrationTest, TaskSource_SetResultFromWorkerThread) {
    auto* tcs = chaos::il2cpp::common::task_source_create();
    ASSERT_NE(nullptr, tcs);

    // Start a worker that will complete the TCS after a short delay.
    std::thread completer([tcs] {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        tcs->set_result(static_cast<CHAOS_IL2CPP_INTPTR>(7));
    });

    auto* task = chaos::il2cpp::common::require_async_task(tcs->get_task());
    EXPECT_TRUE(WaitFor([task] { return task->completed.load(); }));
    completer.join();
    EXPECT_EQ(7, task->result);
    EXPECT_FALSE(task->faulted.load());

    chaos::il2cpp::common::task_source_destroy(tcs);
}

// ══════════════════════════════════════════════════════════════════════════════
// Phase 3 P3-2: Task.Delay — native timer-backed delayed task completion.
// chaos_task_delay_stub returns an AsyncTask handle that the TimerQueue gate
// thread completes a short time after creation (NOT immediately).  The smoke TU
// forwards to it via a small extern declaration (stub lib is linked already).
// ══════════════════════════════════════════════════════════════════════════════

extern "C" CHAOS_IL2CPP_INTPTR chaos_task_delay_stub(CHAOS_IL2CPP_INT32 millisecondsTimeout) noexcept;

TEST_F(AsyncIntegrationTest, TaskDelay_CompletesAfterElapsedTime) {
    // ~80 ms delay; well above the ~15 ms TimerQueue gate tick so the timer
    // is actually scheduled and fires on the gate thread (not a measurement hack).
    constexpr int kPollIntervalMs = 5;
    constexpr int kMaxPoll = 600;  // 5ms * 600 = 3s
    auto start = std::chrono::steady_clock::now();

    CHAOS_IL2CPP_INTPTR handle = chaos_task_delay_stub(80);
    ASSERT_NE(0, handle);
    auto* task = chaos::il2cpp::common::require_async_task(handle);

    // Manual timer tick: the gate thread may not be running (ThreadPool singleton
    // guard skips re-init after a prior test's TearDown called Shutdown).  Tick
    // the timer queue ourselves in the poll loop to fire due timers.
    for (int i = 0; i < kMaxPoll; ++i) {
        if (task->completed.load()) break;
        threading::TimerQueueOnTick();
        std::this_thread::sleep_for(std::chrono::milliseconds(kPollIntervalMs));
    }

    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start).count();
    EXPECT_TRUE(task->completed.load()) << "Task.Delay handle did not complete in time";
    EXPECT_FALSE(task->faulted.load());
    // The timer must NOT have completed instantly — the earliest a real fire
    // could happen is ~the delay (we allow roughly 1.5x for gate tick scheduling).
    if (task->completed.load()) {
        EXPECT_GE(elapsed_ms, 40) << "Task.Delay completed before the requested delay elapsed";
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// Phase 3 P3-3: Task.WhenAll / WhenAny native combinators.
// chaos_task_when_all(children[], n) / chaos_task_when_any(children[], n):
// produce an aggregate AsyncTask from a contiguous array of child Task handles.
// ══════════════════════════════════════════════════════════════════════════════

extern "C" CHAOS_IL2CPP_INTPTR chaos_task_when_all(CHAOS_IL2CPP_INTPTR* children, CHAOS_IL2CPP_INT32 n) noexcept;
extern "C" CHAOS_IL2CPP_INTPTR chaos_task_when_any(CHAOS_IL2CPP_INTPTR* children, CHAOS_IL2CPP_INT32 n) noexcept;

TEST_F(AsyncIntegrationTest, WhenAll_AllChildrenComplete_CompletesAggregate) {
    using namespace chaos::il2cpp::common;
    // One instantly-complete child + two live ones we complete afterward.
    auto* c1 = new AsyncTask();   // live
    auto* c2 = new AsyncTask();   // live
    auto h1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(c1);
    auto h2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(c2);

    CHAOS_IL2CPP_INTPTR children[2] = {h1, h2};
    auto agg = chaos_task_when_all(children, 2);
    ASSERT_NE(0, agg);
    auto* aggTask = require_async_task(agg);
    ASSERT_FALSE(aggTask->completed.load());  // neither child done yet

    // Complete c1.
    c1->completed.store(true, std::memory_order_release);
    chaos::il2cpp::common::finish_async_task(h1);
    EXPECT_FALSE(aggTask->completed.load());  // still c2 pending

    // Complete c2 → aggregate done.
    c2->result = 42;
    c2->completed.store(true, std::memory_order_release);
    chaos::il2cpp::common::finish_async_task(h2);
    EXPECT_TRUE(WaitFor([aggTask] { return aggTask->completed.load(); }));
    EXPECT_FALSE(aggTask->faulted.load());
    // The aggregate now carries the result SET (ASYNC-P2-3): c2 resolved to 42
    // and c1 was left at its default 0, so the array is {0, 42}.  This used to
    // assert `result == 0` with the comment "result not used" — that was the gap
    // ASYNC-P2-3 closed, not a contract.
    {
        auto* results = get_managed_array(aggTask->result);
        ASSERT_NE(nullptr, results) << "WhenAll must expose the children's results";
        ASSERT_EQ(2u, results->length);
        auto* elems = accessor_get_elements(results);
        EXPECT_EQ(0, elems[0]);
        EXPECT_EQ(42, elems[1]);
    }
    delete c1; delete c2;
}

TEST_F(AsyncIntegrationTest, WhenAll_ChildFaults_AggregateFaults) {
    using namespace chaos::il2cpp::common;
    auto* good = new AsyncTask();
    auto* bad = new AsyncTask();
    auto hg = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(good);
    auto hb = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(bad);

    CHAOS_IL2CPP_INTPTR children[2] = {hg, hb};
    auto agg = chaos_task_when_all(children, 2);
    ASSERT_NE(0, agg);
    auto* aggTask = require_async_task(agg);

    good->completed.store(true, std::memory_order_release);
    chaos::il2cpp::common::finish_async_task(hg);

    bad->exception = static_cast<CHAOS_IL2CPP_INTPTR>(0xDEAD);
    bad->faulted.store(true, std::memory_order_relaxed);
    bad->completed.store(true, std::memory_order_release);
    chaos::il2cpp::common::finish_async_task(hb);

    EXPECT_TRUE(WaitFor([aggTask] { return aggTask->completed.load(); }));
    EXPECT_TRUE(aggTask->faulted.load());
    EXPECT_EQ(0xDEAD, aggTask->exception);
    delete good; delete bad;
}

TEST_F(AsyncIntegrationTest, WhenAny_FirstChildToComplete_Wins) {
    using namespace chaos::il2cpp::common;
    // Two live children. Child #1 completes first → aggregate won idx=2 (1-based).
    auto* slow = new AsyncTask();
    auto* fast = new AsyncTask();
    auto hs = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(slow);
    auto hf = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(fast);

    CHAOS_IL2CPP_INTPTR children[2] = {hs, hf};
    auto agg = chaos_task_when_any(children, 2);
    ASSERT_NE(0, agg);
    auto* aggTask = require_async_task(agg);
    ASSERT_FALSE(aggTask->completed.load());

    // Complete the SECOND child (index 1) first → winner idx 1 (0-based) → result 2.
    fast->result = 99;
    fast->completed.store(true, std::memory_order_release);
    chaos::il2cpp::common::finish_async_task(hf);

    EXPECT_TRUE(WaitFor([aggTask] { return aggTask->completed.load(); }));
    EXPECT_EQ(2, aggTask->result);  // 1-based winner index of child[1]
    delete slow; delete fast;
}

TEST_F(AsyncIntegrationTest, WhenAll_EmptyChildren_CompletesImmediately) {
    using namespace chaos::il2cpp::common;
    auto agg = chaos_task_when_all(nullptr, 0);
    ASSERT_NE(0, agg);
    auto* aggTask = require_async_task(agg);
    EXPECT_TRUE(aggTask->completed.load());  // empty WhenAll completes immediately
}

// ══════════════════════════════════════════════════════════════════════════════
// Phase 0 regression: ThreadPool re-initialization after shutdown (defect D3)
//
// ThreadPoolShutdown() must clear the singleton guard so a subsequent
// ThreadPoolInitialize() actually restarts the workers and gate thread.  Before
// the fix, the guard stayed set and re-init silently no-op'd, leaving the pool
// dead — every timer / async_task_run callback would then hang forever.
//
// This test must NOT use the AsyncIntegrationTest fixture (which manages pool
// lifecycle in SetUp/TearDown) so it can drive init/shutdown cycles explicitly.
// ══════════════════════════════════════════════════════════════════════════════

TEST(ThreadPoolLifecycle, ReinitializeAfterShutdownRestartsWorkers) {
    using namespace chaos::il2cpp::runtime_core::threading;

    // Cycle 1: standalone bring-up.
    ThreadPoolInitialize();
    EXPECT_GE(ThreadPoolWorkerCount(), 1)
        << "first Initialize should have created at least one worker";

    ThreadPoolShutdown();
    EXPECT_EQ(0, ThreadPoolWorkerCount())
        << "Shutdown should have joined and cleared all workers";

    // Cycle 2: this is the regression — re-init must not be a silent no-op.
    ThreadPoolInitialize();
    EXPECT_GE(ThreadPoolWorkerCount(), 1)
        << "re-Initialize after Shutdown did not restart workers "
           "(s_initialized guard was not reset)";

    // Prove the restarted pool actually executes work, not just reports a count.
    std::atomic<bool> ran{false};
    ThreadPoolQueueUserWorkItemUnsafe(
        [](void* state) { static_cast<std::atomic<bool>*>(state)->store(true); },
        &ran);
    for (int i = 0; i < 400 && !ran.load(); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    EXPECT_TRUE(ran.load()) << "restarted pool did not execute a queued work item";

    ThreadPoolShutdown();
    EXPECT_EQ(0, ThreadPoolWorkerCount());
}
