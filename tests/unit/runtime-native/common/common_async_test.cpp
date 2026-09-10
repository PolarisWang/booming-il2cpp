#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/async.h>

using namespace chaos::il2cpp::common;

TEST(CommonAsync, AsyncTaskCreate) {
    auto handle = async_task_create();
    EXPECT_NE(0, handle);

    auto* task = require_async_task(handle);
    EXPECT_FALSE(task->completed);
    EXPECT_FALSE(task->faulted);
    EXPECT_EQ(0, task->result);
    EXPECT_EQ(0, task->exception);
}

TEST(CommonAsync, AsyncTaskBuilderSetResult) {
    CHAOS_IL2CPP_INTPTR slot_storage = 0;
    CHAOS_IL2CPP_INTPTR builder_ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot_storage);

    async_task_builder_set_result_raw(builder_ref, 42);

    auto* task = require_async_task(slot_storage);
    EXPECT_TRUE(task->completed);
    EXPECT_FALSE(task->faulted);
    EXPECT_EQ(42, task->result);
}

TEST(CommonAsync, AsyncTaskBuilderSetException) {
    CHAOS_IL2CPP_INTPTR slot_storage = 0;
    CHAOS_IL2CPP_INTPTR builder_ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot_storage);

    async_task_builder_set_exception(builder_ref, 0xBAD);

    auto* task = require_async_task(slot_storage);
    EXPECT_TRUE(task->completed);
    EXPECT_TRUE(task->faulted);
    EXPECT_EQ(0xBAD, task->exception);
}

TEST(CommonAsync, AsyncYieldCreate) {
    auto handle = async_yield_create();
    EXPECT_EQ(1, handle);
}

TEST(CommonAsync, AsyncYieldGetAwaiter) {
    CHAOS_IL2CPP_INTPTR slot = 0;
    CHAOS_IL2CPP_INTPTR awaiter_ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot);
    auto awaiter = async_yield_get_awaiter(awaiter_ref);
    EXPECT_EQ(0, awaiter);
}

TEST(CommonAsync, AsyncYieldIsCompleted) {
    CHAOS_IL2CPP_INTPTR slot = 0;
    auto completed = async_yield_get_is_completed(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot));
    EXPECT_EQ(1, completed);
}

TEST(CommonAsync, AsyncTaskGetAwaiter) {
    auto task_handle = async_task_create();
    auto awaiter = async_task_get_awaiter(task_handle);
    EXPECT_EQ(task_handle, awaiter);
}

TEST(CommonAsync, AsyncTaskAwaiterIsCompleted) {
    auto* task = new AsyncTask{};
    task->completed = true;
    CHAOS_IL2CPP_INTPTR slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(task);
    CHAOS_IL2CPP_INTPTR awaiter_ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot);

    auto completed = async_task_awaiter_get_is_completed(awaiter_ref);
    EXPECT_EQ(1, completed);

    task->completed = false;
    completed = async_task_awaiter_get_is_completed(awaiter_ref);
    EXPECT_EQ(0, completed);

    delete task;
}

TEST(CommonAsync, AsyncTaskAwaiterGetResult) {
    auto* task = new AsyncTask{};
    task->result = 99;
    task->completed = true;
    CHAOS_IL2CPP_INTPTR slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(task);
    CHAOS_IL2CPP_INTPTR awaiter_ref = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot);

    auto result = async_task_awaiter_get_result_raw(awaiter_ref);
    EXPECT_EQ(99, result);

    delete task;
}

TEST(CommonAsync, RegisterAsyncTaskRunFn) {
    EXPECT_EQ(nullptr, g_async_task_run_fn);

    auto test_fn = [](CHAOS_IL2CPP_INTPTR) -> CHAOS_IL2CPP_INTPTR { return 42; };
    register_async_task_run_fn(test_fn);
    EXPECT_NE(nullptr, g_async_task_run_fn);

    auto result = async_task_run(0);
    EXPECT_EQ(42, result);

    register_async_task_run_fn(nullptr);
    EXPECT_EQ(nullptr, g_async_task_run_fn);
}

TEST(CommonAsync, AsyncTaskRunNoRegisteredFn) {
    // When no fn is registered, async_task_run returns 0
    EXPECT_EQ(nullptr, g_async_task_run_fn);
    auto result = async_task_run(static_cast<CHAOS_IL2CPP_INTPTR>(0xDEAD));
    EXPECT_EQ(0, result);
}
