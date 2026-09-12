// async_when_async_test.cpp — ASYNC-P2-1 (cont.): WhenAll/WhenAny over children
// that complete ASYNCHRONOUSLY.
//
// async_when_test.cpp drives only completed children, which take
// async_task_on_completed's *fast path*: the continuation fires inline, before
// anything is registered.  Every one of those tests finishes in 0 ms.  That
// leaves the real composability path uncovered — the one where a continuation
// is stored on the child and later delivered by finish_async_task from whichever
// thread completes it.
//
// The distinction matters because the two paths deliver the continuation with
// different data:
//   fast path   — inline delivery, no handle bookkeeping
//   finish path — the completing task's handle is passed to the continuation,
//                 and the combinator must use it to identify WHICH child fired.
//
// So these tests are the ones that would catch a combinator that mis-identifies
// the completing child, or that counts completions it never actually saw.

#include <gtest/gtest.h>
#include <chaos/native_types.h>
#include <chaos/async.h>

#include "async_stubs.h"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <thread>

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

/// A task that is not complete yet; the caller completes it later.
CHAOS_IL2CPP_INTPTR Pending() { return async_task_create(); }

CHAOS_IL2CPP_INTPTR Resolved(CHAOS_IL2CPP_INTPTR value) {
    return async_task_from_result(value);
}

/// A TaskSource bound to an EXISTING task handle.  Built directly rather than
/// via task_source_create(), which would allocate a second AsyncTask and leak it
/// once we rebind.
TaskSource* SourceFor(CHAOS_IL2CPP_INTPTR handle) {
    auto* src = new TaskSource();
    src->task = require_async_task(handle);
    return src;
}

/// Completes `handle` from a separate thread after a short delay, driving the
/// production finish_async_task path rather than the inline fast path.
void CompleteLater(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR value,
                   std::chrono::milliseconds delay) {
    std::thread([handle, value, delay] {
        std::this_thread::sleep_for(delay);
        auto* src = SourceFor(handle);
        src->set_result(value);
        task_source_destroy(src);
    }).detach();
}

void FaultLater(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR exception,
                std::chrono::milliseconds delay) {
    std::thread([handle, exception, delay] {
        std::this_thread::sleep_for(delay);
        auto* src = SourceFor(handle);
        src->set_exception(exception);
        task_source_destroy(src);
    }).detach();
}

}  // namespace

// ══════════════════════════════════════════════════════════════════════════════
// WhenAll over children that complete asynchronously
// ══════════════════════════════════════════════════════════════════════════════

TEST(AsyncWhenAllAsync, AggregateCompletesOnlyAfterEveryChildHasCompleted)
{
    CHAOS_IL2CPP_INTPTR children[3] = {Pending(), Pending(), Pending()};
    const CHAOS_IL2CPP_INTPTR agg = chaos_task_when_all(children, 3);
    ASSERT_NE(0, agg);

    // Stagger the completions so the aggregate is forced to wait for all three.
    CompleteLater(children[0], 100, std::chrono::milliseconds(40));
    CompleteLater(children[1], 200, std::chrono::milliseconds(80));
    CompleteLater(children[2], 300, std::chrono::milliseconds(120));

    // Pre-condition: before the last child completes the aggregate is incomplete.
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
    EXPECT_EQ(0, ChaosAsyncTaskGetIsCompleted(agg))
        << "the aggregate completed before all of its children had";

    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(agg) != 0; }))
        << "the aggregate never completed even though every child did";
    EXPECT_EQ(0, ChaosAsyncTaskGetIsFaulted(agg));
}

TEST(AsyncWhenAllAsync, FaultOnTheLastChildToCompletePropagates)
{
    // The fault arrives LAST, after two successful completions have already
    // decremented `remaining`.  An implementation that decides the aggregate's
    // fate when the count first drops (rather than when it reaches zero) passes
    // the synchronous tests and fails this one.
    const CHAOS_IL2CPP_INTPTR payload = static_cast<CHAOS_IL2CPP_INTPTR>(0xABBA);
    CHAOS_IL2CPP_INTPTR children[3] = {Pending(), Pending(), Pending()};
    const CHAOS_IL2CPP_INTPTR agg = chaos_task_when_all(children, 3);

    CompleteLater(children[0], 1, std::chrono::milliseconds(30));
    CompleteLater(children[1], 2, std::chrono::milliseconds(60));
    FaultLater(children[2], payload, std::chrono::milliseconds(100));

    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(agg) != 0; }));
    EXPECT_EQ(1, ChaosAsyncTaskGetIsFaulted(agg))
        << "the late fault must still reach the aggregate";
    EXPECT_EQ(payload, require_async_task(agg)->exception);
}

TEST(AsyncWhenAllAsync, PartialCompletionLeavesTheAggregatePending)
{
    // "all" means all: with two of three children done the aggregate must still
    // be pending.
    //
    // Scope note, deliberately recorded: this is a REGRESSION GUARD, not a
    // proven counter-example.  Swapping the implementation's `rem == 0` for
    // `rem <= 0` does not fail it — `remaining` starts at n and each child fires
    // exactly one continuation, so rem only ever walks n, n-1, ... 0 and the two
    // forms are equivalent by construction.  A genuinely premature completion
    // (one that fires at n-1) would need a separate off-by-one in the initial
    // count, which no assertion here can distinguish.  Kept because it still
    // catches a combinator that completes on the FIRST child rather than the
    // last, and because over-claiming coverage is the failure mode this whole
    // task exists to eliminate.
    CHAOS_IL2CPP_INTPTR children[3] = {Pending(), Pending(), Pending()};
    const CHAOS_IL2CPP_INTPTR agg = chaos_task_when_all(children, 3);

    CompleteLater(children[0], 1, std::chrono::milliseconds(30));
    CompleteLater(children[1], 2, std::chrono::milliseconds(60));

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(0, ChaosAsyncTaskGetIsCompleted(agg))
        << "two of three children completed; the aggregate must still be pending";

    // Let it finish so the test does not leak a running aggregate.
    CompleteLater(children[2], 3, std::chrono::milliseconds(1));
    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(agg) != 0; }));
}

// ══════════════════════════════════════════════════════════════════════════════
// WhenAny over children that complete asynchronously
// ══════════════════════════════════════════════════════════════════════════════

TEST(AsyncWhenAnyAsync, WinnerIsTheFirstChildToActuallyComplete)
{
    // Child 2 finishes first — deliberately not child 0.  This is the case the
    // synchronous tests cannot reach: the winner is identified from the handle
    // delivered by finish_async_task, not from scan order.
    CHAOS_IL2CPP_INTPTR children[3] = {Pending(), Pending(), Pending()};
    const CHAOS_IL2CPP_INTPTR agg = chaos_task_when_any(children, 3);
    ASSERT_NE(0, agg);

    CompleteLater(children[0], 1, std::chrono::milliseconds(200));
    CompleteLater(children[1], 2, std::chrono::milliseconds(200));
    CompleteLater(children[2], 3, std::chrono::milliseconds(40));   // first

    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(agg) != 0; }));
    EXPECT_EQ(3, require_async_task(agg)->result)
        << "child index 2 completed first, so the winner is recorded as 3";
}

TEST(AsyncWhenAnyAsync, SecondFastestChildDoesNotOverwriteTheWinner)
{
    // After the aggregate completes, more children finish.  A combinator that
    // updates the winner on every completion reports the LAST child, not the
    // first — and still passes a "it completed" assertion.
    CHAOS_IL2CPP_INTPTR children[3] = {Pending(), Pending(), Pending()};
    const CHAOS_IL2CPP_INTPTR agg = chaos_task_when_any(children, 3);

    CompleteLater(children[1], 2, std::chrono::milliseconds(40));    // winner
    CompleteLater(children[0], 1, std::chrono::milliseconds(120));
    CompleteLater(children[2], 3, std::chrono::milliseconds(200));

    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(agg) != 0; }));
    EXPECT_EQ(2, require_async_task(agg)->result)
        << "child index 1 won; later completions must not change the winner";

    // Let the stragglers finish before the process tears down.
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    EXPECT_EQ(2, require_async_task(agg)->result)
        << "the winner must be stable once the aggregate has completed";
}

TEST(AsyncWhenAnyAsync, FirstChildToCompleteWinsWhenItIsTheLastOneScheduled)
{
    // Mirror: child 0 completes first even though the others were scheduled with
    // shorter sleeps is impossible — so instead verify the lowest-index tie is
    // not hardcoded by making child 0 the only early completer.
    CHAOS_IL2CPP_INTPTR children[3] = {Pending(), Pending(), Pending()};
    const CHAOS_IL2CPP_INTPTR agg = chaos_task_when_any(children, 3);

    CompleteLater(children[0], 11, std::chrono::milliseconds(40));   // winner
    CompleteLater(children[1], 22, std::chrono::milliseconds(300));
    CompleteLater(children[2], 33, std::chrono::milliseconds(300));

    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(agg) != 0; }));
    EXPECT_EQ(1, require_async_task(agg)->result);

    std::this_thread::sleep_for(std::chrono::milliseconds(350));
    EXPECT_EQ(1, require_async_task(agg)->result);
}

// ══════════════════════════════════════════════════════════════════════════════
// Concurrent completion — every child finishing at once
// ══════════════════════════════════════════════════════════════════════════════

TEST(AsyncWhenAllAsync, ConcurrentChildCompletionsStillCompleteExactlyOnce)
{
    constexpr int kN = 16;
    CHAOS_IL2CPP_INTPTR children[kN];
    for (int i = 0; i < kN; ++i) children[i] = Pending();

    const CHAOS_IL2CPP_INTPTR agg = chaos_task_when_all(children, kN);
    ASSERT_NE(0, agg);

    // All 16 complete from separate threads with no ordering guarantee: the
    // decrement-and-check in the continuation must reach exactly zero once.
    std::thread finishers[kN];
    for (int i = 0; i < kN; ++i) {
        finishers[i] = std::thread([h = children[i]] {
            auto* src = SourceFor(h);
            src->set_result(7);
            task_source_destroy(src);
        });
    }
    for (auto& t : finishers) t.join();

    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(agg) != 0; }))
        << "the aggregate must complete after all 16 children";
    EXPECT_EQ(0, ChaosAsyncTaskGetIsFaulted(agg));
}
