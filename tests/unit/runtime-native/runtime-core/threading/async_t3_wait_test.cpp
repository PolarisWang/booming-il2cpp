// async_t3_wait_test.cpp — T3: Task.WaitAll / Task.WaitAny (blocking).
//
// WHAT THIS PINS
// --------------
// WaitAll/WaitAny are the BLOCKING siblings of WhenAll/WhenAny: they do not
// return an aggregate Task — they park the calling thread until the given
// tasks reach a terminal state, then return a Boolean (WaitAll) / an index
// (WaitAny).  The native entries `chaos_task_wait_all/any` unpack the managed
// Task[] array and block on each child.
//
// The property that matters is that they BLOCK and then RELEASE — an
// implementation that returned a constant would pass "did it return" and fail
// WaitAllBlocksUntilPendingChildCompletes below, which is the counter-example
// this file exists for.
//
// (Tasks are intentionally not destroyed, matching the existing array tests:
// the process exits shortly after and a double-free here would be a worse
// failure mode than the leak.)

#include <gtest/gtest.h>
#include <chaos/native_types.h>
#include <chaos/async.h>

#include "async_stubs.h"
#include "runtime_stubs/stub_common.h"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <thread>
#include <vector>

using namespace chaos::il2cpp::common;

namespace {

constexpr auto kPollInterval = std::chrono::milliseconds(2);
constexpr int  kMaxPolls = 1500;

bool WaitFor(const std::function<bool()>& pred) {
    for (int i = 0; i < kMaxPolls; ++i) {
        if (pred()) return true;
        std::this_thread::sleep_for(kPollInterval);
    }
    return pred();
}

/// Build a managed Task[]-shaped array holding `handles` (same layout as the
/// runtime — ManagedArrayAccessor header + contiguous elements), so the native
/// entry goes through exactly the accessor path codegen would use.
CHAOS_IL2CPP_INTPTR MakeTaskArray(const std::vector<CHAOS_IL2CPP_INTPTR>& handles) {
    const size_t n = handles.size();
    const size_t bytes = sizeof(ManagedArrayAccessor) + n * sizeof(CHAOS_IL2CPP_INTPTR);
    auto* raw = static_cast<uint8_t*>(std::malloc(bytes));
    std::memset(raw, 0, bytes);
    auto* arr = reinterpret_cast<ManagedArrayAccessor*>(raw);
    arr->element_type_shape = 0;
    arr->element_type_info = nullptr;
    arr->length = static_cast<CHAOS_IL2CPP_INTPTR>(n);
    auto* elements = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(raw + sizeof(ManagedArrayAccessor));
    for (size_t i = 0; i < n; ++i) elements[i] = handles[i];
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(arr);
}

void FreeTaskArray(CHAOS_IL2CPP_INTPTR handle) {
    std::free(reinterpret_cast<void*>(handle));
}

CHAOS_IL2CPP_INTPTR Resolved(CHAOS_IL2CPP_INTPTR value) {
    return async_task_from_result(value);
}

}  // namespace

// ── WaitAll ──────────────────────────────────────────────────────────

TEST(AsyncT3Wait, WaitAllOnAllCompleteReturnsOne)
{
    std::vector<CHAOS_IL2CPP_INTPTR> handles = {
        Resolved(1), Resolved(2), Resolved(3),
    };
    const CHAOS_IL2CPP_INTPTR arr = MakeTaskArray(handles);

    EXPECT_EQ(chaos_task_wait_all(arr), 1)
        << "WaitAll on an all-complete array must report success";

    FreeTaskArray(arr);
}

TEST(AsyncT3Wait, WaitAllBlocksUntilPendingChildCompletes)
{
    // The counter-example: a constant-returning implementation passes the test
    // above and fails this one.
    AsyncTask* pending = require_async_task(async_task_create());
    std::vector<CHAOS_IL2CPP_INTPTR> handles = {
        Resolved(10), reinterpret_cast<CHAOS_IL2CPP_INTPTR>(pending),
    };
    const CHAOS_IL2CPP_INTPTR arr = MakeTaskArray(handles);

    std::atomic<bool> returned{false};
    std::atomic<bool> ok{false};
    std::thread waiter([&] {
        ok.store(chaos_task_wait_all(arr) == 1, std::memory_order_release);
        returned.store(true, std::memory_order_release);
    });

    // While the child is pending, WaitAll must still be parked.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_FALSE(returned.load(std::memory_order_acquire))
        << "WaitAll must not return while a child is still pending";

    // Release the child; WaitAll must now finish.
    pending->completed.store(true, std::memory_order_release);
    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> g(pending->wait_mtx);
        pending->wait_cv.notify_all();
    }
    waiter.join();
    EXPECT_TRUE(returned.load());
    EXPECT_TRUE(ok.load()) << "WaitAll must succeed once every child completed";

    FreeTaskArray(arr);
}

// ── WaitAny ──────────────────────────────────────────────────────────

TEST(AsyncT3Wait, WaitAnyReturnsTheIndexOfACompletedTask)
{
    std::vector<CHAOS_IL2CPP_INTPTR> handles = {
        Resolved(7), Resolved(8),
    };
    const CHAOS_IL2CPP_INTPTR arr = MakeTaskArray(handles);

    const CHAOS_IL2CPP_INT32 idx = chaos_task_wait_any(arr);
    EXPECT_EQ(idx, 0)
        << "index 0 holds a completed task and must be reported as such";

    FreeTaskArray(arr);
}

TEST(AsyncT3Wait, WaitAnyWaitsForAPendingChild)
{
    AsyncTask* pending = require_async_task(async_task_create());
    std::vector<CHAOS_IL2CPP_INTPTR> handles = {
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(pending),
    };
    const CHAOS_IL2CPP_INTPTR arr = MakeTaskArray(handles);

    std::atomic<bool> returned{false};
    std::thread waiter([&] {
        (void)chaos_task_wait_any(arr);
        returned.store(true, std::memory_order_release);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_FALSE(returned.load(std::memory_order_acquire))
        << "WaitAny must block while every child is pending";

    pending->completed.store(true, std::memory_order_release);
    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> g(pending->wait_mtx);
        pending->wait_cv.notify_all();
    }
    waiter.join();
    EXPECT_TRUE(returned.load());

    FreeTaskArray(arr);
}

// ── Null / invalid rejection ─────────────────────────────────────────

TEST(AsyncT3Wait, NullArrayIsRejectedWithoutDereferencing)
{
    EXPECT_EQ(chaos_task_wait_all(0), 0);
    EXPECT_EQ(chaos_task_wait_any(0), -1);
}