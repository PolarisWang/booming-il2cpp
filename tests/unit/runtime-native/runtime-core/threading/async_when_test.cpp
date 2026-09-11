// async_when_test.cpp — ASYNC-P2-1: WhenAll / WhenAny combinators.
//
// Design counter-examples (design-v1-01.md, Phase 2):
//     WhenAll   — children resolving to 1/2/3 → the aggregate is NOT faulted and
//                 observes all three  (asserting only "it completed" is not enough)
//     WhenAny   — the winner index is correct
//     WhenAll failure — a faulted child propagates as an aggregate fault
//
// The failure these pin: a combinator whose aggregate completes without
// reflecting the children's real outcomes.  "IsCompleted == true" passes for
// such a stub; the value assertions below do not.
//
// Native-only.  chaos_task_when_all / chaos_task_when_any exist (Phase 3 P3-3)
// but had no counter-example; this file is that counter-example.

#include <gtest/gtest.h>
#include <chaos/native_types.h>
#include <chaos/async.h>

#include "async_stubs.h"

#include <chrono>
#include <cstdint>
#include <functional>
#include <thread>

using namespace chaos::il2cpp::common;

namespace {

constexpr auto kPollInterval = std::chrono::milliseconds(5);
constexpr int  kMaxPolls = 400;

bool WaitFor(const std::function<bool()>& pred) {
    for (int i = 0; i < kMaxPolls; ++i) {
        if (pred()) return true;
        std::this_thread::sleep_for(kPollInterval);
    }
    return pred();
}

CHAOS_IL2CPP_INTPTR Resolved(CHAOS_IL2CPP_INTPTR value) {
    return async_task_from_result(value);
}

CHAOS_IL2CPP_INTPTR Faulted(CHAOS_IL2CPP_INTPTR exception) {
    return async_task_from_exception(exception);
}

/// An AsyncTask that is allocated but never completed.
CHAOS_IL2CPP_INTPTR Pending() { return async_task_create(); }

}  // namespace

// ══════════════════════════════════════════════════════════════════════════════
// WhenAll — the aggregate must reflect the children's outcomes
// ══════════════════════════════════════════════════════════════════════════════

TEST(AsyncWhenAll, AllResolvedChildrenCompleteTheAggregate)
{
    CHAOS_IL2CPP_INTPTR children[3] = {Resolved(1), Resolved(2), Resolved(3)};

    const CHAOS_IL2CPP_INTPTR agg = chaos_task_when_all(children, 3);
    ASSERT_NE(0, agg) << "WhenAll must return an aggregate handle";

    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(agg) != 0; }))
        << "the aggregate must complete once all children have";

    EXPECT_EQ(0, ChaosAsyncTaskGetIsFaulted(agg));
    EXPECT_EQ(0, ChaosAsyncTaskGetIsCanceled(agg));
}

TEST(AsyncWhenAll, AggregateWithNoFaultedChildIsNotFaulted)
{
    // Negative control for the fault test below: an implementation that always
    // faults the aggregate would pass the fault case and fail this one.
    CHAOS_IL2CPP_INTPTR children[2] = {Resolved(10), Resolved(20)};
    const CHAOS_IL2CPP_INTPTR agg = chaos_task_when_all(children, 2);

    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(agg) != 0; }));
    EXPECT_EQ(0, ChaosAsyncTaskGetIsFaulted(agg))
        << "no child faulted, so the aggregate must not be faulted";
}

// ── WhenAll failure: a faulted child must PROPAGATE ─────────────────────────
//
// An aggregate that merely completes (faulted == false) loses the failure
// entirely — the caller's await returns as if everything succeeded.  That is
// the fake-green shape the whole task exists to eliminate.

TEST(AsyncWhenAll, FaultedChildPropagatesToTheAggregate)
{
    const CHAOS_IL2CPP_INTPTR payload = static_cast<CHAOS_IL2CPP_INTPTR>(0xF00D);
    CHAOS_IL2CPP_INTPTR children[3] = {Resolved(1), Faulted(payload), Resolved(3)};

    const CHAOS_IL2CPP_INTPTR agg = chaos_task_when_all(children, 3);
    ASSERT_NE(0, agg);

    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(agg) != 0; }));

    EXPECT_EQ(1, ChaosAsyncTaskGetIsFaulted(agg))
        << "a faulted child must make the aggregate faulted, not silently succeed";

    EXPECT_EQ(payload, require_async_task(agg)->exception)
        << "the aggregate must carry the child's exception object";
}

TEST(AsyncWhenAll, FaultPropagatesWhenTheFaultedChildIsLast)
{
    // Order-sensitive companion to the above: an implementation that inspects
    // only children[0] passes the previous test and fails this one.
    const CHAOS_IL2CPP_INTPTR payload = static_cast<CHAOS_IL2CPP_INTPTR>(0x1234);
    CHAOS_IL2CPP_INTPTR children[3] = {Resolved(1), Resolved(2), Faulted(payload)};

    const CHAOS_IL2CPP_INTPTR agg = chaos_task_when_all(children, 3);
    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(agg) != 0; }));

    EXPECT_EQ(1, ChaosAsyncTaskGetIsFaulted(agg));
    EXPECT_EQ(payload, require_async_task(agg)->exception);
}

TEST(AsyncWhenAll, AggregateIsNotFaultedWhenAChildIsCancelled)
{
    // Cancellation is a distinct terminal state, not a fault (ASYNC-P1-4).
    CHAOS_IL2CPP_INTPTR children[2] = {Resolved(1), async_task_from_canceled()};
    const CHAOS_IL2CPP_INTPTR agg = chaos_task_when_all(children, 2);

    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(agg) != 0; }));
    EXPECT_EQ(0, ChaosAsyncTaskGetIsFaulted(agg))
        << "a cancelled child is not a faulted child";
}

// ══════════════════════════════════════════════════════════════════════════════
// WhenAny — the winner index must identify WHICH child completed
// ══════════════════════════════════════════════════════════════════════════════

TEST(AsyncWhenAny, AggregateCompletesWhenFirstChildDoes)
{
    CHAOS_IL2CPP_INTPTR children[3] = {Resolved(7), Pending(), Pending()};

    const CHAOS_IL2CPP_INTPTR agg = chaos_task_when_any(children, 3);
    ASSERT_NE(0, agg);

    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(agg) != 0; }))
        << "WhenAny must complete as soon as one child has";
    EXPECT_EQ(0, ChaosAsyncTaskGetIsFaulted(agg));
}

TEST(AsyncWhenAny, WinnerIndexIsTheIncompleteChildsIndex)
{
    // Only child 2 has completed → winner index 2, recorded 1-based as 3.
    //
    // This is the assertion that catches the handle-identity defect: the
    // continuation is delivered with the COMPLETING task's handle, but an
    // implementation that mis-identifies it silently reports index 0 and every
    // "it completed" assertion still passes.
    CHAOS_IL2CPP_INTPTR children[3] = {Pending(), Pending(), Resolved(5)};

    const CHAOS_IL2CPP_INTPTR agg = chaos_task_when_any(children, 3);
    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(agg) != 0; }));

    EXPECT_EQ(3, require_async_task(agg)->result)
        << "the winner is child index 2, recorded 1-based as 3";
}

TEST(AsyncWhenAny, WinnerIndexIsZeroWhenTheFirstChildCompletes)
{
    // Mirror of the above — a hardcoded winner cannot pass both.
    CHAOS_IL2CPP_INTPTR children[3] = {Resolved(1), Pending(), Pending()};

    const CHAOS_IL2CPP_INTPTR agg = chaos_task_when_any(children, 3);
    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(agg) != 0; }));

    EXPECT_EQ(1, require_async_task(agg)->result)
        << "the winner is child index 0, recorded 1-based as 1";
}

TEST(AsyncWhenAny, WinnerIndexIsOneWhenTheMiddleChildCompletes)
{
    // Third mirror: only the middle child completed → index 1 → 2.
    CHAOS_IL2CPP_INTPTR children[3] = {Pending(), Resolved(99), Pending()};

    const CHAOS_IL2CPP_INTPTR agg = chaos_task_when_any(children, 3);
    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(agg) != 0; }));

    EXPECT_EQ(2, require_async_task(agg)->result)
        << "the winner is child index 1, recorded 1-based as 2";
}

TEST(AsyncWhenAny, WinnerIndexIsNotFabricatedWhenNoChildHasCompleted)
{
    // None of the children completes: the aggregate must NOT report a winner.
    // Without this, "winner == 1" is unfalsifiable — an implementation that
    // completes immediately with index 0 passes every test above.
    CHAOS_IL2CPP_INTPTR children[2] = {Pending(), Pending()};

    const CHAOS_IL2CPP_INTPTR agg = chaos_task_when_any(children, 2);
    ASSERT_NE(0, agg);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(0, ChaosAsyncTaskGetIsCompleted(agg))
        << "WhenAny must not complete before any child has";
}

// ══════════════════════════════════════════════════════════════════════════════
// Degenerate inputs must be rejected, not fabricated
// ══════════════════════════════════════════════════════════════════════════════

TEST(AsyncWhenAll, EmptySetCompletesImmediately)
{
    const CHAOS_IL2CPP_INTPTR agg = chaos_task_when_all(nullptr, 0);
    ASSERT_NE(0, agg) << "WhenAll() over an empty set completes immediately";
    EXPECT_EQ(1, ChaosAsyncTaskGetIsCompleted(agg));
    EXPECT_EQ(0, ChaosAsyncTaskGetIsFaulted(agg));
}

TEST(AsyncWhenAll, NullChildrenWithPositiveCountIsRejected)
{
    EXPECT_EQ(0, chaos_task_when_all(nullptr, 3))
        << "a null child array with n > 0 is invalid, not an empty set";
}

TEST(AsyncWhenAll, NegativeCountIsRejected)
{
    CHAOS_IL2CPP_INTPTR children[1] = {Resolved(1)};
    EXPECT_EQ(0, chaos_task_when_all(children, -1));
}

TEST(AsyncWhenAny, NullChildrenWithPositiveCountIsRejected)
{
    EXPECT_EQ(0, chaos_task_when_any(nullptr, 3));
}
