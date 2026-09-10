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
        threading::ThreadPoolInitialize();
        register_async_task_run_fn(TestTaskRun);
    }

    void TearDown() override {
        register_async_task_run_fn(nullptr);
        threading::ThreadPoolShutdown();
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
    EXPECT_TRUE(WaitFor([task] { return task->completed; }))
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
        EXPECT_TRUE(WaitFor([task] { return task->completed; }))
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
            if (WaitFor([task] { return task->completed; })) {
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
        if (WaitFor([task] { return task->completed; })) {
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

    EXPECT_TRUE(WaitFor([task] { return task->completed; }));
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

    EXPECT_TRUE(WaitFor([task_a] { return task_a->completed; }));
    EXPECT_TRUE(WaitFor([task_b] { return task_b->completed; }));
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
    EXPECT_TRUE(WaitFor([task] { return task->completed; }));
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
    EXPECT_TRUE(WaitFor([task] { return task->completed; }));
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
    EXPECT_TRUE(task->completed);
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
    EXPECT_TRUE(WaitFor([task] { return task->completed; }));
}
