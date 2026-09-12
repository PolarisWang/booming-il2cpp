// async_when_each_test.cpp — ASYNC-P2-8: Task.WhenEach's completion-ordered stream.
//
// chaos_task_when_each_array is the ShapeRegistry entry point codegen calls
// (Task.WhenEach(Task[]) passes a managed array handle).  Unlike WhenAll (yields
// the aggregate once, after everything finished) and WhenAny (yields the first
// completed once), WhenEach yields EACH task as it completes.
//
// THE COUNTEREXAMPLE THIS FILE EXISTS FOR
// ---------------------------------------
// "It emitted the symbol" is not the contract.  A WhenEach that collected every
// task and returned them in ARRAY order would pass any test that only checks the
// element count — and would be a silently different program, because a consumer
// awaiting the enumerable is promised completion order.  So the central test here
// completes the children in an order DIFFERENT from their array order and asserts
// the drain observes the COMPLETION order.
//
// A second failure mode gets its own test: an element completing before the
// consumer first drains must not be lost.  Registration happens at construction,
// so this is the property that makes the "queue at completion time" design
// load-bearing rather than decorative.

#include <gtest/gtest.h>
#include <chaos/native_types.h>
#include <chaos/async.h>

#include "async_stubs.h"
#include "runtime_stubs/stub_common.h"

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

/// Build a managed Task[]-shaped array holding `handles`, using the same layout
/// the runtime does (ManagedArrayAccessor header, elements contiguous after it).
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

CHAOS_IL2CPP_INTPTR Pending() { return async_task_create(); }

TaskSource* SourceFor(CHAOS_IL2CPP_INTPTR handle) {
    auto* src = new TaskSource();
    src->task = require_async_task(handle);
    return src;
}

void CompleteLater(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR value,
                   std::chrono::milliseconds delay) {
    std::thread([handle, value, delay] {
        std::this_thread::sleep_for(delay);
        auto* src = SourceFor(handle);
        src->set_result(value);
        task_source_destroy(src);
    }).detach();
}

/// Drain everything the stream currently offers, appending handles in order.
std::vector<CHAOS_IL2CPP_INTPTR> DrainAll(CHAOS_IL2CPP_INTPTR stream) {
    std::vector<CHAOS_IL2CPP_INTPTR> out;
    while (chaos_task_when_each_may_have_next(stream)) {
        CHAOS_IL2CPP_INTPTR next = chaos_task_when_each_try_dequeue(stream);
        if (next == 0) break;  // momentarily empty; caller would await here
        out.push_back(next);
    }
    return out;
}

}  // namespace

// ══════════════════════════════════════════════════════════════════════════════
// 1. THE central counterexample — completion order, not array order
// ══════════════════════════════════════════════════════════════════════════════

TEST(AsyncWhenEach, YieldsInCompletionOrderNotArrayOrder)
{
    // Three PENDING tasks in array order [A, B, C].  Complete them in the order
    // C, A, B — deliberately different from the array order.
    CHAOS_IL2CPP_INTPTR a = Pending();
    CHAOS_IL2CPP_INTPTR b = Pending();
    CHAOS_IL2CPP_INTPTR c = Pending();

    CHAOS_IL2CPP_INTPTR arr = MakeTaskArray({a, b, c});
    CHAOS_IL2CPP_INTPTR stream = chaos_task_when_each_array(arr);
    ASSERT_NE(stream, 0);
    FreeTaskArray(arr);

    // Nothing completed yet, so nothing to hand out (but the stream is live).
    EXPECT_EQ(chaos_task_when_each_try_dequeue(stream), 0);
    EXPECT_EQ(chaos_task_when_each_may_have_next(stream), 1);

    CompleteLater(c, 30, std::chrono::milliseconds(5));
    CompleteLater(a, 10, std::chrono::milliseconds(40));
    CompleteLater(b, 20, std::chrono::milliseconds(75));

    // Drain as elements arrive; the ORDER of arrival is the assertion.
    std::vector<CHAOS_IL2CPP_INTPTR> seen;
    ASSERT_TRUE(WaitFor([&] {
        CHAOS_IL2CPP_INTPTR next = chaos_task_when_each_try_dequeue(stream);
        if (next != 0) { seen.push_back(next); return seen.size() >= 3; }
        return false;
    })) << "only observed " << seen.size() << " of 3 completions";

    // C completed first, then A, then B — NOT the array order [A,B,C].
    ASSERT_EQ(seen.size(), 3u);
    EXPECT_EQ(seen[0], c) << "first element must be the FIRST task to complete";
    EXPECT_EQ(seen[1], a);
    EXPECT_EQ(seen[2], b);

    // The stream terminates once everything has been handed out.
    EXPECT_EQ(chaos_task_when_each_may_have_next(stream), 0);
    chaos_task_when_each_destroy(stream);
}

// ══════════════════════════════════════════════════════════════════════════════
// 2. No completion may be lost before the first drain
// ══════════════════════════════════════════════════════════════════════════════

TEST(AsyncWhenEach, RecordsCompletionsThatOccurBeforeFirstDrain)
{
    // All children already completed at construction time.  A design that only
    // started listening when the consumer first called try_dequeue would lose
    // every one of them — which is why registration happens up front.
    CHAOS_IL2CPP_INTPTR a = Resolved(1);
    CHAOS_IL2CPP_INTPTR b = Resolved(2);

    CHAOS_IL2CPP_INTPTR arr = MakeTaskArray({a, b});
    CHAOS_IL2CPP_INTPTR stream = chaos_task_when_each_array(arr);
    ASSERT_NE(stream, 0);
    FreeTaskArray(arr);

    // Drain AFTER the fact: both must still be present.
    auto seen = DrainAll(stream);
    ASSERT_EQ(seen.size(), 2u)
        << "completions before the first drain must be queued, not lost";
    EXPECT_EQ(seen[0], a);
    EXPECT_EQ(seen[1], b);
    EXPECT_EQ(chaos_task_when_each_may_have_next(stream), 0);

    chaos_task_when_each_destroy(stream);
}

// ══════════════════════════════════════════════════════════════════════════════
// 3. Every child is yielded exactly once
// ══════════════════════════════════════════════════════════════════════════════

TEST(AsyncWhenEach, YieldsEveryChildExactlyOnce)
{
    constexpr int kCount = 5;
    std::vector<CHAOS_IL2CPP_INTPTR> tasks;
    tasks.reserve(kCount);
    for (int i = 0; i < kCount; ++i) tasks.push_back(Pending());

    CHAOS_IL2CPP_INTPTR arr = MakeTaskArray(tasks);
    CHAOS_IL2CPP_INTPTR stream = chaos_task_when_each_array(arr);
    ASSERT_NE(stream, 0);
    FreeTaskArray(arr);

    // Stagger completions so they interleave with draining.
    for (int i = 0; i < kCount; ++i)
        CompleteLater(tasks[i], static_cast<CHAOS_IL2CPP_INTPTR>(i),
                      std::chrono::milliseconds(5 * (i + 1)));

    std::vector<CHAOS_IL2CPP_INTPTR> seen;
    ASSERT_TRUE(WaitFor([&] {
        CHAOS_IL2CPP_INTPTR next = chaos_task_when_each_try_dequeue(stream);
        if (next != 0) { seen.push_back(next); return seen.size() >= static_cast<size_t>(kCount); }
        return false;
    })) << "only observed " << seen.size() << " of " << kCount;

    ASSERT_EQ(seen.size(), static_cast<size_t>(kCount));
    // No duplicates, and every original handle present.
    for (auto* t : tasks)
        EXPECT_EQ(std::count(seen.begin(), seen.end(), t), 1)
            << "handle yielded more than once (or never)";
    EXPECT_EQ(chaos_task_when_each_may_have_next(stream), 0);

    chaos_task_when_each_destroy(stream);
}

// ══════════════════════════════════════════════════════════════════════════════
// 4. Degenerate inputs must not hang or crash
// ══════════════════════════════════════════════════════════════════════════════

TEST(AsyncWhenEach, NullElementStillTerminatesTheStream)
{
    // A null slot occupies a position in the stream.  Accounting for it is what
    // keeps `may_have_next` from reporting a phantom element forever.
    CHAOS_IL2CPP_INTPTR a = Resolved(1);
    CHAOS_IL2CPP_INTPTR arr = MakeTaskArray({a, static_cast<CHAOS_IL2CPP_INTPTR>(0)});
    CHAOS_IL2CPP_INTPTR stream = chaos_task_when_each_array(arr);
    ASSERT_NE(stream, 0);
    FreeTaskArray(arr);

    ASSERT_TRUE(WaitFor([&] { return chaos_task_when_each_may_have_next(stream) == 0; }))
        << "a null element must still be accounted for so the stream terminates";
    chaos_task_when_each_destroy(stream);
}

TEST(AsyncWhenEach, InvalidHandleIsRejectedWithoutCrashing)
{
    EXPECT_EQ(chaos_task_when_each_array(0), 0);
    // Drain helpers must tolerate a null stream (DisposeAsync-after-failure path).
    EXPECT_EQ(chaos_task_when_each_may_have_next(0), 0);
    EXPECT_EQ(chaos_task_when_each_try_dequeue(0), 0);
    chaos_task_when_each_destroy(0);  // must not crash
}
