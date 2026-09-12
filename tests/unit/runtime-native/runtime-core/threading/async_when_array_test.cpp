// async_when_array_test.cpp — ASYNC-P2-3: the managed-array WhenAll overload.
//
// chaos_task_when_all_array is the ShapeRegistry entry point codegen actually
// calls (Task.WhenAll(Task[]) passes a managed array handle).  Two counter-
// examples live here:
//
//   1. RESULT SET.  design-v1-01.md Phase 2 asks for
//      "3 task 各返 1/2/3 → 结果集 = [1,2,3]（不只能断言"完成了"）".
//      The aggregate used to record only fault/completion and never collected
//      the children's payloads, so "it completed" was the strongest true
//      assertion available.  These tests require the per-index values.
//
//   2. CHILD-ARRAY LIFETIME.  WhenAllAnyManagedArray copies the managed array's
//      element handles into a heap buffer, hands that buffer to
//      WhenAllAnyInternal (which stores the pointer in its state), and then
//      frees it.  When children complete LATER, the continuation dereferences
//      the freed buffer.  A synchronous completion never runs that path, which
//      is why this survived: every existing test used already-completed
//      children.  The asynchronously-completing tests below are the ones that
//      read the freed memory.
//
// Both are asserted through the managed handle that codegen really passes, not
// the flat-pointer helper, so the tests exercise the overload under test.

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

/// Build a managed Task[]-shaped array holding `handles`.
///
/// Uses the same layout the runtime does (ManagedArrayAccessor header with the
/// elements contiguous after it), so this goes through exactly the accessor
/// path WhenAllAnyManagedArray uses.
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

/// Read element `i` from a managed result array.
CHAOS_IL2CPP_INTPTR ElementAt(CHAOS_IL2CPP_INTPTR array_handle, size_t i) {
    auto* arr = get_managed_array(array_handle);
    return accessor_get_elements(const_cast<ManagedArrayAccessor*>(arr))[i];
}

/// Length of a managed result array.
CHAOS_IL2CPP_INT32 ArrayLength(CHAOS_IL2CPP_INTPTR array_handle) {
    return static_cast<CHAOS_IL2CPP_INT32>(get_managed_array(array_handle)->length);
}

}  // namespace

// ══════════════════════════════════════════════════════════════════════════════
// 1. The result set — [1,2,3], not just "completed"
// ══════════════════════════════════════════════════════════════════════════════

TEST(AsyncWhenAllArray, ResultSetHoldsEachChildsValueInOrder)
{
    // The design's named counter-example, verbatim: three tasks returning
    // 1/2/3 must produce the result set [1,2,3].  Asserting only "completed"
    // passes for an aggregate that collects nothing.
    std::vector<CHAOS_IL2CPP_INTPTR> children = {Resolved(1), Resolved(2), Resolved(3)};
    const CHAOS_IL2CPP_INTPTR tasks = MakeTaskArray(children);

    const CHAOS_IL2CPP_INTPTR agg = chaos_task_when_all_array(tasks);
    ASSERT_NE(0, agg);
    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(agg) != 0; }));

    const CHAOS_IL2CPP_INTPTR results = require_async_task(agg)->result;
    ASSERT_NE(0, results)
        << "the aggregate must carry a result array, not just a completion flag";

    EXPECT_EQ(3, ArrayLength(results));
    EXPECT_EQ(1, ElementAt(results, 0));
    EXPECT_EQ(2, ElementAt(results, 1));
    EXPECT_EQ(3, ElementAt(results, 2));

    FreeTaskArray(tasks);
}

TEST(AsyncWhenAllArray, ResultSetPreservesOrderNotCompletionOrder)
{
    // Children complete in reverse order (3, then 2, then 1) but the result set
    // must stay in DECLARATION order.  An implementation that appends results as
    // they arrive produces [3,2,1] and passes the test above only because the
    // synchronous path happens to complete in order.
    std::vector<CHAOS_IL2CPP_INTPTR> children = {Pending(), Pending(), Pending()};
    const CHAOS_IL2CPP_INTPTR tasks = MakeTaskArray(children);

    const CHAOS_IL2CPP_INTPTR agg = chaos_task_when_all_array(tasks);
    ASSERT_NE(0, agg);

    CompleteLater(children[2], 30, std::chrono::milliseconds(30));   // first
    CompleteLater(children[1], 20, std::chrono::milliseconds(60));
    CompleteLater(children[0], 10, std::chrono::milliseconds(90));   // last

    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(agg) != 0; }));

    const CHAOS_IL2CPP_INTPTR results = require_async_task(agg)->result;
    ASSERT_NE(0, results);
    EXPECT_EQ(10, ElementAt(results, 0)) << "index 0 is child 0's value (10)";
    EXPECT_EQ(20, ElementAt(results, 1)) << "index 1 is child 1's value (20)";
    EXPECT_EQ(30, ElementAt(results, 2)) << "index 2 is child 2's value (30)";

    FreeTaskArray(tasks);
}

TEST(AsyncWhenAllArray, ResultSetIsEmptyForNoChildren)
{
    const CHAOS_IL2CPP_INTPTR tasks = MakeTaskArray({});
    const CHAOS_IL2CPP_INTPTR agg = chaos_task_when_all_array(tasks);
    ASSERT_NE(0, agg);
    EXPECT_EQ(1, ChaosAsyncTaskGetIsCompleted(agg));
    FreeTaskArray(tasks);
}

TEST(AsyncWhenAllArray, ResultSetDistinguishesDuplicateValuesByPosition)
{
    // All children return the same value: the set must still have n entries, so
    // an implementation that dedupes or stops at the first cannot pass.
    std::vector<CHAOS_IL2CPP_INTPTR> children = {Resolved(7), Resolved(7), Resolved(7)};
    const CHAOS_IL2CPP_INTPTR tasks = MakeTaskArray(children);

    const CHAOS_IL2CPP_INTPTR agg = chaos_task_when_all_array(tasks);
    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(agg) != 0; }));

    const CHAOS_IL2CPP_INTPTR results = require_async_task(agg)->result;
    ASSERT_NE(0, results);
    EXPECT_EQ(3, ArrayLength(results)) << "duplicates must not be collapsed";
    for (int i = 0; i < 3; ++i) EXPECT_EQ(7, ElementAt(results, i));

    FreeTaskArray(tasks);
}

TEST(AsyncWhenAllArray, FaultedChildStillPropagatesThroughTheArrayOverload)
{
    // The array overload must preserve the fault semantics the flat overload has.
    const CHAOS_IL2CPP_INTPTR payload = static_cast<CHAOS_IL2CPP_INTPTR>(0xFEED);
    std::vector<CHAOS_IL2CPP_INTPTR> children = {
        Resolved(1), async_task_from_exception(payload), Resolved(3)};
    const CHAOS_IL2CPP_INTPTR tasks = MakeTaskArray(children);

    const CHAOS_IL2CPP_INTPTR agg = chaos_task_when_all_array(tasks);
    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(agg) != 0; }));

    EXPECT_EQ(1, ChaosAsyncTaskGetIsFaulted(agg));
    EXPECT_EQ(payload, require_async_task(agg)->exception);

    FreeTaskArray(tasks);
}

// ══════════════════════════════════════════════════════════════════════════════
// 2. Child-array lifetime when children complete LATER
// ══════════════════════════════════════════════════════════════════════════════
//
// WhenAllAnyManagedArray frees its copy of the element handles as soon as
// WhenAllAnyInternal returns, but the aggregate's continuation dereferences
// that buffer on every child completion.  With children that complete later,
// the buffer is already freed.  ASan would flag it directly; without ASan the
// symptom is a wrong winner/fault or a crash depending on what reuses the
// memory, so these tests assert the observable contract instead.

TEST(AsyncWhenAllArray, LaterCompletingChildrenStillCompleteTheAggregate)
{
    constexpr int kN = 4;
    std::vector<CHAOS_IL2CPP_INTPTR> children;
    for (int i = 0; i < kN; ++i) children.push_back(Pending());
    const CHAOS_IL2CPP_INTPTR tasks = MakeTaskArray(children);

    const CHAOS_IL2CPP_INTPTR agg = chaos_task_when_all_array(tasks);
    ASSERT_NE(0, agg);

    // Free the source array immediately: the aggregate must have taken its own
    // copy of the child handles, so the caller disposing of the array must not
    // affect it.
    FreeTaskArray(tasks);

    for (int i = 0; i < kN; ++i) {
        CompleteLater(children[i], (i + 1) * 11, std::chrono::milliseconds(30 + i * 20));
    }

    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(agg) != 0; }))
        << "the aggregate never completed; the child handles were not retained";
    EXPECT_EQ(0, ChaosAsyncTaskGetIsFaulted(agg));
}

TEST(AsyncWhenAnyArray, LateChildCompletionIsNotLostByArrayDisposal)
{
    // Same lifetime concern on the WhenAny path.  The observable contract:
    // disposing the caller's array before any child completes must not change
    // the outcome, because the aggregate holds its own copy.
    std::vector<CHAOS_IL2CPP_INTPTR> children = {Pending(), Pending(), Pending()};
    const CHAOS_IL2CPP_INTPTR tasks = MakeTaskArray(children);

    const CHAOS_IL2CPP_INTPTR agg = chaos_task_when_any_array(tasks);
    ASSERT_NE(0, agg);
    FreeTaskArray(tasks);

    // Only child 2 completes → winner index 2 → 1-based 3.
    CompleteLater(children[2], 5, std::chrono::milliseconds(50));

    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(agg) != 0; }));
    EXPECT_EQ(3, require_async_task(agg)->result)
        << "the winner must be child 2; a stale handle array reports the wrong index";
}

TEST(AsyncWhenAllArray, FaultFromALateChildIsStillObserved)
{
    // The fault path over the array overload with LATE children: the aggregate
    // must still inspect the child handles when the last one completes.
    const CHAOS_IL2CPP_INTPTR payload = static_cast<CHAOS_IL2CPP_INTPTR>(0xCAFE);
    std::vector<CHAOS_IL2CPP_INTPTR> children = {Pending(), Pending()};
    const CHAOS_IL2CPP_INTPTR tasks = MakeTaskArray(children);

    const CHAOS_IL2CPP_INTPTR agg = chaos_task_when_all_array(tasks);
    ASSERT_NE(0, agg);
    FreeTaskArray(tasks);

    CompleteLater(children[0], 1, std::chrono::milliseconds(20));
    std::thread([h = children[1], payload] {
        std::this_thread::sleep_for(std::chrono::milliseconds(60));
        auto* src = SourceFor(h);
        src->set_exception(payload);
        task_source_destroy(src);
    }).detach();

    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(agg) != 0; }))
        << "the aggregate did not complete after both children did";

    EXPECT_EQ(1, ChaosAsyncTaskGetIsFaulted(agg))
        << "child 1 faulted late; the aggregate must still see it";
    EXPECT_EQ(payload, require_async_task(agg)->exception);
}
