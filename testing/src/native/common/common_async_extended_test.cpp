// common_async_extended_test.cpp — Comprehensive async primitive unit tests
//
// Extends common_async_test.cpp with full coverage of async.h:
//   - AsyncTask lifecycle (create, independence, reuse)
//   - AsyncTaskBuilder protocol (get_task, set_result, set_exception, overwrite)
//   - AsyncYield protocol (create, awaiter, is_completed, get_result)
//   - Awaiter protocol (get_awaiter, is_completed, get_result)
//   - Pointer tagging (resolve_native_int_slot tagged/untagged)
//   - TaskRun function pointer (register, call, unregister, re-register)
//   - Concurrent access from multiple threads
//
// Links: chaos_common + chaos_fmt (no ThreadPool dependency).

#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/async.h>
#include <chaos/ptr_tag.h>

#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

using namespace chaos::il2cpp::common;

// ── Test helpers ──────────────────────────────────────────────────────────────
namespace {
    std::atomic<int> g_concurrent_call_count{0};
    CHAOS_IL2CPP_INTPTR CountingTaskRun(CHAOS_IL2CPP_INTPTR arg) noexcept {
        g_concurrent_call_count.fetch_add(1, std::memory_order_relaxed);
        return arg;
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// AsyncTask lifecycle
// ══════════════════════════════════════════════════════════════════════════════

TEST(CommonAsyncExtended, CreateReturnsNonNull) {
    auto handle = async_task_create();
    EXPECT_NE(0, handle);
}

TEST(CommonAsyncExtended, CreateInitialState) {
    auto handle = async_task_create();
    auto* task = require_async_task(handle);
    EXPECT_FALSE(task->completed);
    EXPECT_FALSE(task->faulted);
    EXPECT_EQ(0, task->result);
    EXPECT_EQ(0, task->exception);
}

TEST(CommonAsyncExtended, MultipleTasksIndependent) {
    auto h1 = async_task_create();
    auto h2 = async_task_create();
    ASSERT_NE(h1, 0);
    ASSERT_NE(h2, 0);

    auto* t1 = require_async_task(h1);
    auto* t2 = require_async_task(h2);

    // Modify t1, verify t2 unaffected
    t1->result = 42;
    t1->completed = true;
    EXPECT_EQ(42, t1->result);
    EXPECT_TRUE(t1->completed);
    EXPECT_EQ(0, t2->result);
    EXPECT_FALSE(t2->completed);

    // Modify t2, verify t1 unaffected
    t2->result = 99;
    t2->faulted = true;
    t2->completed = true;
    EXPECT_EQ(99, t2->result);
    EXPECT_TRUE(t2->faulted);
    EXPECT_EQ(42, t1->result);
    EXPECT_FALSE(t1->faulted);
}

TEST(CommonAsyncExtended, CreateManyTasks) {
    constexpr int kCount = 100;
    CHAOS_IL2CPP_INTPTR handles[kCount];
    for (int i = 0; i < kCount; ++i) {
        handles[i] = async_task_create();
        EXPECT_NE(0, handles[i]);
    }
    // Verify all are independent by writing to each
    for (int i = 0; i < kCount; ++i) {
        auto* task = require_async_task(handles[i]);
        task->result = static_cast<CHAOS_IL2CPP_INTPTR>(i);
    }
    for (int i = 0; i < kCount; ++i) {
        auto* task = require_async_task(handles[i]);
        EXPECT_EQ(i, task->result);
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// AsyncTaskBuilder protocol
// ══════════════════════════════════════════════════════════════════════════════

TEST(CommonAsyncExtended, BuilderGetTaskLazyCreate) {
    CHAOS_IL2CPP_INTPTR slot_storage = 0;
    CHAOS_IL2CPP_INTPTR builder_ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot_storage);

    // First call: slot is 0, creates new task
    auto handle1 = async_task_builder_get_task(builder_ref);
    EXPECT_NE(0, handle1);
    EXPECT_EQ(slot_storage, handle1);  // slot now holds the handle

    // Second call: slot is non-zero, returns same handle
    auto handle2 = async_task_builder_get_task(builder_ref);
    EXPECT_EQ(handle1, handle2);
}

TEST(CommonAsyncExtended, BuilderSetResult) {
    CHAOS_IL2CPP_INTPTR slot_storage = 0;
    CHAOS_IL2CPP_INTPTR builder_ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot_storage);

    async_task_builder_set_result_raw(builder_ref, 42);

    auto* task = require_async_task(slot_storage);
    EXPECT_TRUE(task->completed);
    EXPECT_FALSE(task->faulted);
    EXPECT_EQ(42, task->result);
    EXPECT_EQ(0, task->exception);
}

TEST(CommonAsyncExtended, BuilderSetException) {
    CHAOS_IL2CPP_INTPTR slot_storage = 0;
    CHAOS_IL2CPP_INTPTR builder_ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot_storage);

    async_task_builder_set_exception(builder_ref, 0xBAD);

    auto* task = require_async_task(slot_storage);
    EXPECT_TRUE(task->completed);
    EXPECT_TRUE(task->faulted);
    EXPECT_EQ(0xBAD, task->exception);
    EXPECT_EQ(0, task->result);
}

TEST(CommonAsyncExtended, BuilderSetResultOverwritesPrevious) {
    CHAOS_IL2CPP_INTPTR slot_storage = 0;
    CHAOS_IL2CPP_INTPTR builder_ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot_storage);

    async_task_builder_set_result_raw(builder_ref, 10);
    // Overwrite
    async_task_builder_set_result_raw(builder_ref, 20);

    auto* task = require_async_task(slot_storage);
    EXPECT_TRUE(task->completed);
    EXPECT_FALSE(task->faulted);
    EXPECT_EQ(20, task->result);
}

TEST(CommonAsyncExtended, BuilderSetExceptionAfterResult) {
    CHAOS_IL2CPP_INTPTR slot_storage = 0;
    CHAOS_IL2CPP_INTPTR builder_ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot_storage);

    async_task_builder_set_result_raw(builder_ref, 100);
    async_task_builder_set_exception(builder_ref, 0xFF);

    // set_exception does NOT clear result — only sets exception + faulted + completed
    auto* task = require_async_task(slot_storage);
    EXPECT_TRUE(task->completed);
    EXPECT_TRUE(task->faulted);
    EXPECT_EQ(0xFF, task->exception);
    EXPECT_EQ(100, task->result);  // result preserved from prior set_result_raw
}

TEST(CommonAsyncExtended, BuilderSetResultAfterException) {
    CHAOS_IL2CPP_INTPTR slot_storage = 0;
    CHAOS_IL2CPP_INTPTR builder_ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot_storage);

    async_task_builder_set_exception(builder_ref, 0xFF);
    async_task_builder_set_result_raw(builder_ref, 200);

    // SetResult should clear exception and faulted
    auto* task = require_async_task(slot_storage);
    EXPECT_TRUE(task->completed);
    EXPECT_FALSE(task->faulted);
    EXPECT_EQ(200, task->result);
    EXPECT_EQ(0, task->exception);
}

TEST(CommonAsyncExtended, BuilderGetTaskPreCreated) {
    // If slot already has a handle, builder_get_task returns it
    auto pre_created = async_task_create();
    ASSERT_NE(0, pre_created);

    CHAOS_IL2CPP_INTPTR slot_storage = pre_created;
    CHAOS_IL2CPP_INTPTR builder_ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot_storage);

    auto handle = async_task_builder_get_task(builder_ref);
    EXPECT_EQ(pre_created, handle);
}

// ══════════════════════════════════════════════════════════════════════════════
// AsyncYield protocol
// ══════════════════════════════════════════════════════════════════════════════

TEST(CommonAsyncExtended, YieldCreateReturnsOne) {
    auto handle = async_yield_create();
    EXPECT_EQ(1, handle);
}

TEST(CommonAsyncExtended, YieldGetAwaiterFromSlot) {
    CHAOS_IL2CPP_INTPTR slot_storage = 0;
    CHAOS_IL2CPP_INTPTR awaiter_ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot_storage);

    auto awaiter = async_yield_get_awaiter(awaiter_ref);
    EXPECT_EQ(0, awaiter);  // slot was 0, so awaiter is 0

    // With non-zero slot value
    slot_storage = 42;
    awaiter = async_yield_get_awaiter(awaiter_ref);
    EXPECT_EQ(42, awaiter);  // returns slot value directly
}

TEST(CommonAsyncExtended, YieldGetAwaiterTaggedPointer) {
    // Test with tagged slot (bit 0 set)
    CHAOS_IL2CPP_INTPTR dummy = 0x12345678;
    CHAOS_IL2CPP_INTPTR tagged = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dummy) | 1;
    CHAOS_IL2CPP_INTPTR slot_storage = tagged;
    CHAOS_IL2CPP_INTPTR awaiter_ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot_storage);

    auto awaiter = async_yield_get_awaiter(awaiter_ref);
    EXPECT_EQ(tagged, awaiter);
}

TEST(CommonAsyncExtended, YieldIsCompletedAlwaysTrue) {
    CHAOS_IL2CPP_INTPTR slot = 0;
    auto completed = async_yield_get_is_completed(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot));
    EXPECT_EQ(1, completed);

    // Even with garbage in slot, yield is always completed
    slot = 0xDEADBEEF;
    completed = async_yield_get_is_completed(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot));
    EXPECT_EQ(1, completed);
}

TEST(CommonAsyncExtended, YieldGetResultNoCrash) {
    CHAOS_IL2CPP_INTPTR slot = 0;
    // Should not crash — the result is ignored
    async_yield_get_result(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot));
    SUCCEED();
}

// ══════════════════════════════════════════════════════════════════════════════
// AsyncTask awaiter protocol
// ══════════════════════════════════════════════════════════════════════════════

TEST(CommonAsyncExtended, TaskGetAwaiterReturnsHandle) {
    auto handle = async_task_create();
    auto awaiter = async_task_get_awaiter(handle);
    EXPECT_EQ(handle, awaiter);
}

TEST(CommonAsyncExtended, TaskAwaiterIsCompletedTrue) {
    auto* task = new AsyncTask();
    task->completed = true;
    CHAOS_IL2CPP_INTPTR slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(task);
    CHAOS_IL2CPP_INTPTR awaiter_ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot);

    auto completed = async_task_awaiter_get_is_completed(awaiter_ref);
    EXPECT_EQ(1, completed);
    delete task;
}

TEST(CommonAsyncExtended, TaskAwaiterIsCompletedFalse) {
    auto* task = new AsyncTask();  // completed=false, faulted=false
    CHAOS_IL2CPP_INTPTR slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(task);
    CHAOS_IL2CPP_INTPTR awaiter_ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot);

    auto completed = async_task_awaiter_get_is_completed(awaiter_ref);
    EXPECT_EQ(0, completed);
    delete task;
}

TEST(CommonAsyncExtended, TaskAwaiterIsCompletedFaulted) {
    auto* task = new AsyncTask();
    task->faulted = true;
    task->completed = true;
    CHAOS_IL2CPP_INTPTR slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(task);
    CHAOS_IL2CPP_INTPTR awaiter_ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot);

    auto completed = async_task_awaiter_get_is_completed(awaiter_ref);
    EXPECT_EQ(1, completed);  // faulted but still completed
    delete task;
}

TEST(CommonAsyncExtended, TaskAwaiterGetResult) {
    auto* task = new AsyncTask();
    task->result = 99;
    task->completed = true;
    CHAOS_IL2CPP_INTPTR slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(task);
    CHAOS_IL2CPP_INTPTR awaiter_ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot);

    auto result = async_task_awaiter_get_result_raw(awaiter_ref);
    EXPECT_EQ(99, result);
    delete task;
}

TEST(CommonAsyncExtended, TaskAwaiterGetResultZero) {
    auto* task = new AsyncTask();
    task->result = 0;
    task->completed = true;
    CHAOS_IL2CPP_INTPTR slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(task);
    CHAOS_IL2CPP_INTPTR awaiter_ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot);

    // Getting result 0 from a completed non-faulted task is valid
    auto result = async_task_awaiter_get_result_raw(awaiter_ref);
    EXPECT_EQ(0, result);
    delete task;
}

// ══════════════════════════════════════════════════════════════════════════════
// Pointer tagging (resolve_native_int_slot)
// ══════════════════════════════════════════════════════════════════════════════

TEST(CommonAsyncExtended, ResolveUntaggedPointer) {
    CHAOS_IL2CPP_INTPTR value = 42;
    auto* resolved = resolve_native_int_slot(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value));
    EXPECT_EQ(resolved, &value);
    EXPECT_EQ(*resolved, 42);
}

TEST(CommonAsyncExtended, ResolveTaggedPointer) {
    CHAOS_IL2CPP_INTPTR value = 100;
    CHAOS_IL2CPP_INTPTR addr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value);
    CHAOS_IL2CPP_INTPTR tagged = addr | 1;  // managed pointer local slot tag

    auto* resolved = resolve_native_int_slot(tagged);
    EXPECT_EQ(resolved, &value);
    // Verify we can read/write through the resolved pointer
    EXPECT_EQ(*resolved, 100);
    *resolved = 200;
    EXPECT_EQ(value, 200);
}

TEST(CommonAsyncExtended, ResolveRoundTripThroughSlot) {
    // Simulate what the async state machine does: store a handle in a slot,
    // then pass a pointer-to-slot as the builder_ref / awaiter_ref.
    CHAOS_IL2CPP_INTPTR slot = 0;
    CHAOS_IL2CPP_INTPTR ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot);

    // Write through resolve_native_int_slot (simulating async_task_builder_get_task)
    auto* slot_ptr = resolve_native_int_slot(ref);
    *slot_ptr = async_task_create();

    // Read back via builder_get_task
    auto handle = async_task_builder_get_task(ref);
    EXPECT_EQ(handle, slot);
    EXPECT_NE(0, handle);

    auto* task = require_async_task(handle);
    EXPECT_FALSE(task->completed);
}

TEST(CommonAsyncExtended, ResolveBlockCopyAddressUntagged) {
    CHAOS_IL2CPP_INTPTR value = 77;
    auto* resolved = static_cast<CHAOS_IL2CPP_INTPTR*>(
        resolve_block_copy_address(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value)));
    EXPECT_EQ(*resolved, 77);
}

TEST(CommonAsyncExtended, ResolveBlockCopyAddressTaggedSlot) {
    CHAOS_IL2CPP_INTPTR value = 55;
    CHAOS_IL2CPP_INTPTR addr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value);
    CHAOS_IL2CPP_INTPTR tagged = addr | 1;

    auto* resolved = static_cast<CHAOS_IL2CPP_INTPTR*>(resolve_block_copy_address(tagged));
    EXPECT_EQ(*resolved, 55);
}

TEST(CommonAsyncExtended, ResolveBlockCopyAddressTaggedRawInt32) {
    CHAOS_IL2CPP_INTPTR value = 33;
    CHAOS_IL2CPP_INTPTR addr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value);
    CHAOS_IL2CPP_INTPTR tagged = addr | 2;  // raw int32 pointer tag

    auto* resolved = static_cast<CHAOS_IL2CPP_INTPTR*>(resolve_block_copy_address(tagged));
    EXPECT_EQ(*resolved, 33);
}

// ══════════════════════════════════════════════════════════════════════════════
// TaskRun function pointer (register/call/unregister)
// ══════════════════════════════════════════════════════════════════════════════

TEST(CommonAsyncExtended, TaskRunFnInitiallyNull) {
    EXPECT_EQ(nullptr, g_async_task_run_fn);
}

TEST(CommonAsyncExtended, TaskRunFnRegisterAndCall) {
    auto test_fn = [](CHAOS_IL2CPP_INTPTR arg) -> CHAOS_IL2CPP_INTPTR {
        return arg + 1;
    };

    register_async_task_run_fn(test_fn);
    EXPECT_NE(nullptr, g_async_task_run_fn);

    auto result = async_task_run(41);
    EXPECT_EQ(42, result);

    register_async_task_run_fn(nullptr);
}

TEST(CommonAsyncExtended, TaskRunFnUnregisterReturnsZero) {
    register_async_task_run_fn(nullptr);
    auto result = async_task_run(0xDEAD);
    EXPECT_EQ(0, result);
}

TEST(CommonAsyncExtended, TaskRunFnReRegister) {
    auto fn_a = [](CHAOS_IL2CPP_INTPTR) -> CHAOS_IL2CPP_INTPTR { return 10; };
    auto fn_b = [](CHAOS_IL2CPP_INTPTR) -> CHAOS_IL2CPP_INTPTR { return 20; };

    register_async_task_run_fn(fn_a);
    EXPECT_EQ(10, async_task_run(0));
    register_async_task_run_fn(fn_b);
    EXPECT_EQ(20, async_task_run(0));
    register_async_task_run_fn(nullptr);
}

TEST(CommonAsyncExtended, TaskRunFnPassArg) {
    // Verify the delegate_fn parameter is passed through
    auto test_fn = [](CHAOS_IL2CPP_INTPTR arg) -> CHAOS_IL2CPP_INTPTR {
        return arg;
    };

    register_async_task_run_fn(test_fn);
    EXPECT_EQ(100, async_task_run(100));
    EXPECT_EQ(0, async_task_run(0));
    EXPECT_EQ(-1, async_task_run(-1));
    register_async_task_run_fn(nullptr);
}

// ══════════════════════════════════════════════════════════════════════════════
// Concurrent access — multiple threads on async primitives
// ══════════════════════════════════════════════════════════════════════════════

TEST(CommonAsyncExtended, ConcurrentCreateAndRead) {
    constexpr int kThreads = 8;
    constexpr int kTasksPerThread = 50;
    std::atomic<int> ready{0};
    std::atomic<int> errors{0};

    CHAOS_IL2CPP_INTPTR results[kThreads][kTasksPerThread]{};

    std::vector<std::thread> workers;
    for (int t = 0; t < kThreads; ++t) {
        workers.emplace_back([t, &results, &ready, &errors] {
            ready.fetch_add(1, std::memory_order_release);

            for (int i = 0; i < kTasksPerThread; ++i) {
                auto handle = async_task_create();
                if (handle == 0) {
                    errors.fetch_add(1, std::memory_order_relaxed);
                    continue;
                }
                auto* task = require_async_task(handle);
                task->result = static_cast<CHAOS_IL2CPP_INTPTR>(t * kTasksPerThread + i);
                task->completed = true;
                results[t][i] = handle;
            }
        });
    }

    for (auto& w : workers) w.join();

    EXPECT_EQ(0, errors.load());

    // Read back from main thread
    for (int t = 0; t < kThreads; ++t) {
        for (int i = 0; i < kTasksPerThread; ++i) {
            auto* task = require_async_task(results[t][i]);
            EXPECT_EQ(t * kTasksPerThread + i, task->result);
            EXPECT_TRUE(task->completed);
        }
    }
}

TEST(CommonAsyncExtended, ConcurrentBuilderAccess) {
    // Multiple threads simulating async state machine builder pattern
    constexpr int kThreads = 8;
    std::atomic<int> ready{0};

    std::vector<std::thread> workers;
    for (int t = 0; t < kThreads; ++t) {
        workers.emplace_back([t, &ready] {
            CHAOS_IL2CPP_INTPTR slot = 0;
            CHAOS_IL2CPP_INTPTR ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot);

            ready.fetch_add(1, std::memory_order_release);

            // Simulate async state machine: get_task -> set_result
            auto handle = async_task_builder_get_task(ref);
            ASSERT_NE(0, handle);

            async_task_builder_set_result_raw(ref, t * 10);

            auto* task = require_async_task(handle);
            EXPECT_TRUE(task->completed);
            EXPECT_FALSE(task->faulted);
            EXPECT_EQ(t * 10, task->result);
        });
    }

    for (auto& w : workers) w.join();
}

TEST(CommonAsyncExtended, ConcurrentTaskRunFn) {
    g_concurrent_call_count.store(0, std::memory_order_relaxed);
    register_async_task_run_fn(CountingTaskRun);

    constexpr int kThreads = 8;
    std::vector<std::thread> workers;
    for (int t = 0; t < kThreads; ++t) {
        workers.emplace_back([t] {
            auto result = async_task_run(static_cast<CHAOS_IL2CPP_INTPTR>(t));
            EXPECT_EQ(t, result);
        });
    }

    for (auto& w : workers) w.join();

    EXPECT_EQ(kThreads, g_concurrent_call_count.load());
    register_async_task_run_fn(nullptr);
}

// ══════════════════════════════════════════════════════════════════════════════
// Edge cases
// ══════════════════════════════════════════════════════════════════════════════

TEST(CommonAsyncExtended, BuilderSetResultZero) {
    CHAOS_IL2CPP_INTPTR slot = 0;
    CHAOS_IL2CPP_INTPTR ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot);

    async_task_builder_set_result_raw(ref, 0);

    auto* task = require_async_task(slot);
    EXPECT_TRUE(task->completed);
    EXPECT_EQ(0, task->result);
    EXPECT_FALSE(task->faulted);
}

TEST(CommonAsyncExtended, BuilderSetExceptionZero) {
    CHAOS_IL2CPP_INTPTR slot = 0;
    CHAOS_IL2CPP_INTPTR ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot);

    async_task_builder_set_exception(ref, 0);

    auto* task = require_async_task(slot);
    EXPECT_TRUE(task->completed);
    EXPECT_TRUE(task->faulted);
    EXPECT_EQ(0, task->exception);
}

TEST(CommonAsyncExtended, BuilderSetResultLargeValue) {
    CHAOS_IL2CPP_INTPTR slot = 0;
    CHAOS_IL2CPP_INTPTR ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot);

    CHAOS_IL2CPP_INTPTR large = static_cast<CHAOS_IL2CPP_INTPTR>(0x7FFFFFFFFFFFFFFFLL);
    async_task_builder_set_result_raw(ref, large);

    auto* task = require_async_task(slot);
    EXPECT_EQ(large, task->result);
}

TEST(CommonAsyncExtended, BuilderSetExceptionLargeValue) {
    CHAOS_IL2CPP_INTPTR slot = 0;
    CHAOS_IL2CPP_INTPTR ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot);

    CHAOS_IL2CPP_INTPTR large = static_cast<CHAOS_IL2CPP_INTPTR>(0xFFFFFFFFFFFFFFFFLL);
    async_task_builder_set_exception(ref, large);

    auto* task = require_async_task(slot);
    EXPECT_EQ(large, task->exception);
}

TEST(CommonAsyncExtended, BuilderSlotReuse) {
    // Builder slot can be reused after a task completes (fresh state machine)
    CHAOS_IL2CPP_INTPTR slot = 0;
    CHAOS_IL2CPP_INTPTR ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot);

    // First state machine
    async_task_builder_set_result_raw(ref, 10);
    auto* task1 = require_async_task(slot);
    EXPECT_EQ(10, task1->result);

    // Reset slot for next state machine
    slot = 0;
    async_task_builder_set_result_raw(ref, 20);
    auto* task2 = require_async_task(slot);
    EXPECT_EQ(20, task2->result);
    EXPECT_NE(task1, task2);  // Different task instance
}
