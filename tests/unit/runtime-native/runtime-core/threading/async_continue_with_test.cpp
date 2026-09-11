// async_continue_with_test.cpp — ASYNC-P2-2: Task.ContinueWith.
//
// Design counter-example (design-v1-01.md, Phase 2):
//     ContinueWith — each representative overload must actually RECEIVE the
//     antecedent and run its continuation, not merely be wired to something.
//
// ContinueWith is the composition primitive the rest of the surface is built
// on, so the assertions here are about the continuation's inputs, not its
// existence:
//   - the continuation runs at all (the trivial half)
//   - it receives the ANTECEDENT task, so it can inspect the prior result/fault
//   - it receives the antecedent's RESULT VALUE, so `t.ContinueWith(t => t.Result + 1)`
//     composes rather than always reading 0
//   - it runs when the antecedent ALREADY completed (the common case in
//     generated code, where the antecedent finishes before ContinueWith is
//     called) as well as when it completes later
//   - it runs on a FAULTED antecedent too — ContinueWith is not skipped on
//     failure, that is what TaskContinuationOptions/OnlyOn* would restrict
//   - the continuation's own task carries the continuation's return value
//
// The failure these pin: a ContinueWith wired as "run this later" that passes
// the continuation a zeroed/absent antecedent, or drops it on an already-
// completed antecedent.  Both pass a "the callback ran" assertion.

#include <gtest/gtest.h>
#include <chaos/native_types.h>
#include <chaos/async.h>

#include "async_stubs.h"
#include "core/delegate_object.h"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <thread>

using namespace chaos::il2cpp::common;
using chaos::il2cpp::runtime_core::DelegateObject;

namespace {

// ── Observation channel ──────────────────────────────────────────────────────
// The continuation delegate receives the antecedent task handle and echoes back
// what it saw, so the test can assert on the continuation's INPUT rather than
// only on the fact that it was called.

std::atomic<CHAOS_IL2CPP_INTPTR> g_seen_antecedent{0};
std::atomic<CHAOS_IL2CPP_INTPTR> g_seen_result{0};
std::atomic<int>                 g_continuation_runs{0};

/// The continuation body.  `arg` is the antecedent task handle.
CHAOS_IL2CPP_INTPTR ContinuationBody(CHAOS_IL2CPP_INTPTR arg) {
    g_seen_antecedent.store(arg);
    auto* antecedent = reinterpret_cast<AsyncTask*>(arg);
    if (antecedent != nullptr) {
        g_seen_result.store(antecedent->result);
    }
    g_continuation_runs.fetch_add(1);
    // The continuation's own task resolves with the observed result + 1, so the
    // caller can chain.
    return g_seen_result.load() + 1;
}

DelegateObject* MakeDelegate(CHAOS_IL2CPP_INTPTR (*fn)(CHAOS_IL2CPP_INTPTR)) {
    auto* del = new DelegateObject();
    del->type_info = nullptr;
    del->chaos_delegate_target = 0;  // open delegate
    del->chaos_delegate_method_ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(fn);
    del->chaos_delegate_invocation_list = 0;
    del->chaos_delegate_invocation_count = 0;
    del->chaos_delegate_method_token = 0;
    del->_pad = 0;
    return del;
}

void ResetObservations() {
    g_seen_antecedent.store(0);
    g_seen_result.store(0);
    g_continuation_runs.store(0);
}

constexpr auto kPollInterval = std::chrono::milliseconds(2);
constexpr int  kMaxPolls = 1500;

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

CHAOS_IL2CPP_INTPTR Pending() { return async_task_create(); }

TaskSource* SourceFor(CHAOS_IL2CPP_INTPTR handle) {
    auto* src = new TaskSource();
    src->task = require_async_task(handle);
    return src;
}

}  // namespace

// ══════════════════════════════════════════════════════════════════════════════
// The continuation receives the antecedent
// ══════════════════════════════════════════════════════════════════════════════

TEST(AsyncContinueWith, ContinuationReceivesTheAntecedentHandle)
{
    ResetObservations();
    const CHAOS_IL2CPP_INTPTR antecedent = Resolved(7);
    DelegateObject* body = MakeDelegate(&ContinuationBody);

    const CHAOS_IL2CPP_INTPTR cont =
        chaos_task_continue_with(antecedent, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(body));
    ASSERT_NE(0, cont) << "ContinueWith must return a continuation task";

    ASSERT_TRUE(WaitFor([&] { return g_continuation_runs.load() == 1; }))
        << "the continuation never ran on an already-completed antecedent";

    EXPECT_EQ(antecedent, g_seen_antecedent.load())
        << "the continuation must be handed the antecedent task, not null";
}

TEST(AsyncContinueWith, ContinuationSeesTheAntecedentsResult)
{
    // The composition claim: `t.ContinueWith(a => a.Result + 1)` must observe the
    // antecedent's real payload.  A continuation handed a zeroed antecedent
    // observes 0 and still "runs" — which is why this is asserted separately.
    ResetObservations();
    const CHAOS_IL2CPP_INTPTR antecedent = Resolved(41);
    DelegateObject* body = MakeDelegate(&ContinuationBody);

    (void)chaos_task_continue_with(antecedent, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(body));

    ASSERT_TRUE(WaitFor([&] { return g_continuation_runs.load() == 1; }));
    EXPECT_EQ(41, g_seen_result.load())
        << "the continuation read the antecedent's result, not a default";
}

TEST(AsyncContinueWith, ContinuationTaskCarriesTheContinuationsReturnValue)
{
    // Chaining depends on this: the returned task must resolve with what the
    // continuation returned, otherwise `ContinueWith(...).Result` is 0 regardless
    // of the continuation's work.
    ResetObservations();
    const CHAOS_IL2CPP_INTPTR antecedent = Resolved(41);
    DelegateObject* body = MakeDelegate(&ContinuationBody);

    const CHAOS_IL2CPP_INTPTR cont =
        chaos_task_continue_with(antecedent, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(body));

    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(cont) != 0; }));
    EXPECT_EQ(42, require_async_task(cont)->result)
        << "the continuation returned 41+1, so its task must resolve to 42";
    EXPECT_EQ(0, ChaosAsyncTaskGetIsFaulted(cont));
}

// ══════════════════════════════════════════════════════════════════════════════
// Both completion orders must work
// ══════════════════════════════════════════════════════════════════════════════
//
// Generated code hits both: an antecedent that already finished (the inline
// fast path) and one still running (the finish_async_task path).  A ContinueWith
// implemented for only one order passes half the tests and silently drops
// continuations in production.

TEST(AsyncContinueWith, RunsImmediatelyWhenTheAntecedentAlreadyCompleted)
{
    ResetObservations();
    const CHAOS_IL2CPP_INTPTR antecedent = Resolved(5);
    DelegateObject* body = MakeDelegate(&ContinuationBody);

    (void)chaos_task_continue_with(antecedent, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(body));

    // Give an asynchronous implementation a moment to prove it is asynchronous;
    // the assertion is that the run happened, not that it happened inline.
    ASSERT_TRUE(WaitFor([&] { return g_continuation_runs.load() == 1; }));
    EXPECT_EQ(5, g_seen_result.load());
}

TEST(AsyncContinueWith, RunsLaterWhenTheAntecedentCompletesAfterRegistration)
{
    ResetObservations();
    const CHAOS_IL2CPP_INTPTR antecedent = Pending();
    DelegateObject* body = MakeDelegate(&ContinuationBody);

    const CHAOS_IL2CPP_INTPTR cont =
        chaos_task_continue_with(antecedent, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(body));

    // Not yet: the antecedent is still pending.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(0, g_continuation_runs.load()) << "the continuation ran too early";

    // Complete the antecedent from another thread → finish_async_task path.
    std::thread([antecedent] {
        auto* src = SourceFor(antecedent);
        src->set_result(123);
        task_source_destroy(src);
    }).join();

    ASSERT_TRUE(WaitFor([&] { return g_continuation_runs.load() == 1; }))
        << "the continuation was dropped when the antecedent completed later";
    EXPECT_EQ(123, g_seen_result.load());
    EXPECT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(cont) != 0; }));
    EXPECT_EQ(124, require_async_task(cont)->result);
}

// ══════════════════════════════════════════════════════════════════════════════
// A faulted antecedent must still run the continuation
// ══════════════════════════════════════════════════════════════════════════════

TEST(AsyncContinueWith, ContinuationRunsOnAFaultedAntecedent)
{
    // ContinueWith is unconditional by default: the continuation runs and can
    // inspect the fault.  Skipping it on failure would silently drop cleanup and
    // logging continuations — the exact bug that makes production code leak.
    ResetObservations();
    const CHAOS_IL2CPP_INTPTR payload = static_cast<CHAOS_IL2CPP_INTPTR>(0xDEAD);
    const CHAOS_IL2CPP_INTPTR antecedent = async_task_from_exception(payload);
    DelegateObject* body = MakeDelegate(&ContinuationBody);

    (void)chaos_task_continue_with(antecedent, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(body));

    ASSERT_TRUE(WaitFor([&] { return g_continuation_runs.load() == 1; }))
        << "the continuation was skipped because the antecedent faulted";
    EXPECT_EQ(antecedent, g_seen_antecedent.load());

    // The continuation can see the fault through the antecedent handle.
    EXPECT_EQ(1, async_task_get_is_faulted(antecedent));
    EXPECT_EQ(payload, require_async_task(antecedent)->exception);
}

TEST(AsyncContinueWith, ContinuationRunsOnACancelledAntecedent)
{
    ResetObservations();
    const CHAOS_IL2CPP_INTPTR antecedent = async_task_from_canceled();
    DelegateObject* body = MakeDelegate(&ContinuationBody);

    (void)chaos_task_continue_with(antecedent, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(body));

    ASSERT_TRUE(WaitFor([&] { return g_continuation_runs.load() == 1; }))
        << "the continuation was skipped on cancellation";
}

// ══════════════════════════════════════════════════════════════════════════════
// Chaining and degenerate inputs
// ══════════════════════════════════════════════════════════════════════════════

TEST(AsyncContinueWith, ContinuationsChain)
{
    // a → b → c: each continuation observes the previous stage's value.  This is
    // the property that makes ContinueWith a composition primitive rather than a
    // callback registration.
    ResetObservations();
    DelegateObject* body = MakeDelegate(&ContinuationBody);

    const CHAOS_IL2CPP_INTPTR first =
        chaos_task_continue_with(Resolved(1), reinterpret_cast<CHAOS_IL2CPP_INTPTR>(body));
    ASSERT_NE(0, first);
    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(first) != 0; }));
    EXPECT_EQ(2, require_async_task(first)->result);

    const CHAOS_IL2CPP_INTPTR second =
        chaos_task_continue_with(first, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(body));
    ASSERT_TRUE(WaitFor([&] { return ChaosAsyncTaskGetIsCompleted(second) != 0; }));
    EXPECT_EQ(3, require_async_task(second)->result)
        << "the second stage must see the first stage's result (2), not the original";
}

TEST(AsyncContinueWith, ContinuationRunsExactlyOnce)
{
    // The continuation must be delivered a single time.  A double delivery runs
    // user cleanup/logging twice and is invisible to a `>= 1` assertion.
    ResetObservations();
    const CHAOS_IL2CPP_INTPTR antecedent = Pending();
    DelegateObject* body = MakeDelegate(&ContinuationBody);

    (void)chaos_task_continue_with(antecedent, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(body));
    std::thread([antecedent] {
        auto* src = SourceFor(antecedent);
        src->set_result(9);
        task_source_destroy(src);
    }).join();

    ASSERT_TRUE(WaitFor([&] { return g_continuation_runs.load() >= 1; }));
    std::this_thread::sleep_for(std::chrono::milliseconds(80));
    EXPECT_EQ(1, g_continuation_runs.load()) << "the continuation ran more than once";
}

TEST(AsyncContinueWith, NullAntecedentIsRejected)
{
    ResetObservations();
    DelegateObject* body = MakeDelegate(&ContinuationBody);
    EXPECT_EQ(0, chaos_task_continue_with(0, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(body)));
}

TEST(AsyncContinueWith, NullContinuationIsRejected)
{
    ResetObservations();
    EXPECT_EQ(0, chaos_task_continue_with(Resolved(1), 0));
}

TEST(AsyncContinueWith, RejectedInputsDoNotRunTheContinuation)
{
    ResetObservations();
    DelegateObject* body = MakeDelegate(&ContinuationBody);
    (void)chaos_task_continue_with(0, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(body));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(0, g_continuation_runs.load());
}
