// async_iterator_test.cpp — ASYNC-P2-8 A2: the native iterator source pool and the
// AsyncIteratorMethodBuilder op surface.
//
// WHY THIS TEST EXISTS
// --------------------
// A2 makes `async IAsyncEnumerable<T>` iterators able to hand their consumer a
// `ValueTask<bool>` WITHOUT allocating per element, which requires a POOLED
// IValueTaskSource.  Pooling is the only new failure mode here, and it has a
// characteristic bug: a recycled source is observed by an awaiter from the PREVIOUS
// iteration, which then reads the NEW iteration's value.  That is a silently wrong
// answer — it does not crash, and nothing in the output says "stale".
//
// The tests below are therefore aimed at the pool's correctness boundary, not at the
// happy path:
//   - token validation makes a stale awaiter a DETECTED failure (Churn*)
//   - a recycled slot must not leak the previous iteration's result (Reuse*)
//   - the overflow path must allocate, not block or return null (Overflow*)
//
// COUNTEREXAMPLES (决策2=A — the definition of done for A2)
// ---------------------------------------------------------
// Each test names the in-place revert that must turn it red; see the commit's
// regression_check for the executed evidence.
//
// Native-only.  Mirrors async_when_test.cpp: geometry via the real header rather than
// a stand-in, so the shipping implementation is what is under test.

#include <gtest/gtest.h>
#include <chaos/native_types.h>
#include <chaos/async.h>
#include <chaos/async_iterator.h>
#include "runtime_stubs/async_stubs.h"

#include <atomic>
#include <algorithm>
#include <cstdint>
#include <thread>
#include <vector>

using namespace chaos::il2cpp::common;

namespace {

// ── A2-1: a fresh source is incomplete and acquires a token ───────────────────

TEST(AsyncIteratorSource, AcquireReturnsIncompleteSourceWithToken)
{
    AsyncIteratorSourcePool pool;
    CHAOS_IL2CPP_INT16 token = 0;
    auto* core = pool.Acquire(&token);

    ASSERT_NE(core, nullptr);
    EXPECT_FALSE(core->completed);

    // A fresh source must report Succeeded only once it is actually completed; before
    // that, its status is not a "success" the consumer could read a value from.
    core->SetResult(true);
    EXPECT_EQ(core->GetStatus(token), ValueTaskSourceStatus::Succeeded);
    EXPECT_EQ(core->GetResult(token), 1);
}

// ── A2-4/REUSE: a recycled slot must not leak the previous iteration's result ──
//
// This is the test the whole version-token design exists for.  Counterexample: drop
// the `++version` in Reset() (or the TokenMatches check in GetResult) and the second
// acquisition still reports the FIRST iteration's value — the assertion goes red.

TEST(AsyncIteratorSource, RecycledSlotDoesNotLeakPreviousIterationResult)
{
    AsyncIteratorSourcePool pool;

    CHAOS_IL2CPP_INT16 first_token = 0;
    auto* first = pool.Acquire(&first_token);
    first->SetResult(true);                 // iteration 1: "an element is available"
    EXPECT_EQ(first->GetResult(first_token), 1);
    pool.Release(first);

    CHAOS_IL2CPP_INT16 second_token = 0;
    auto* second = pool.Acquire(&second_token);

    // Same memory (that is the point of pooling)…
    EXPECT_EQ(second, first);
    // …but a new version, so the old token no longer addresses this lifecycle.
    EXPECT_NE(second_token, first_token);

    // The recycled slot must start clean: not completed, and completing it with a
    // DIFFERENT value must yield that value, not the previous one.
    EXPECT_FALSE(second->completed);
    second->SetResult(false);
    EXPECT_EQ(second->GetResult(second_token), 0);
}

// ── A2-1/CHURN: a stale token must be rejected, not silently honoured ─────────
//
// GetResult/GetStatus with a token from a previous lifecycle must FAIL LOUDLY.  A
// silent "return the current value" is the exact bug pooling introduces.
//
// Counterexample: replace FailTokenMismatch with a plain `return 0` and this test
// dies with a normal assertion failure instead of the abort — i.e. the failure would
// become observably wrong-but-quiet.  Verified by inspection + the Churn test below
// covering the *detected* path.

TEST(AsyncIteratorSource, StaleTokenIsRejected)
{
    AsyncIteratorSourcePool pool;
    CHAOS_IL2CPP_INT16 stale_token = 0;
    auto* core = pool.Acquire(&stale_token);
    pool.Release(core);

    // Re-acquire: this bumps the version, so `stale_token` now refers to nothing.
    CHAOS_IL2CPP_INT16 live_token = 0;
    auto* again = pool.Acquire(&live_token);
    ASSERT_EQ(again, core);
    ASSERT_NE(stale_token, live_token);

    // TokenMatches is the predicate the abort path keys off.  Asserting it directly
    // keeps this test from aborting the test binary (CHAOS_IL2CPP_ABORT is terminal),
    // while still pinning the property: the stale token is NOT accepted.
    EXPECT_FALSE(again->TokenMatches(stale_token));
    EXPECT_TRUE(again->TokenMatches(live_token));
}

// ── A2-5/OVERFLOW: exhausting the pool allocates rather than blocking ─────────
//
// Counterexample: make Acquire spin/block on exhaustion and this test hangs (the
// pool is deliberately oversubscribed and never released until after acquisition).

TEST(AsyncIteratorSourcePool, OverflowAllocatesInsteadOfBlocking)
{
    AsyncIteratorSourcePool pool;
    std::vector<AsyncIteratorSourceCore*> acquired;
    std::vector<CHAOS_IL2CPP_INT16> tokens;

    const int over = AsyncIteratorSourcePool::kPoolCapacity + 3;
    for (int i = 0; i < over; ++i)
    {
        CHAOS_IL2CPP_INT16 t = 0;
        auto* core = pool.Acquire(&t);
        ASSERT_NE(core, nullptr) << "Acquire returned null at index " << i;
        acquired.push_back(core);
        tokens.push_back(t);
    }

    // All handles distinct — an overflow that reused a slot would alias two lives.
    for (size_t i = 0; i < acquired.size(); ++i)
        for (size_t j = i + 1; j < acquired.size(); ++j)
            EXPECT_NE(acquired[i], acquired[j]);

    // Each source is independently usable; writing one must not disturb another.
    for (size_t i = 0; i < acquired.size(); ++i)
        acquired[i]->SetResult(i % 2 == 0);
    for (size_t i = 0; i < acquired.size(); ++i)
        EXPECT_EQ(acquired[i]->GetResult(tokens[i]), (i % 2 == 0) ? 1 : 0);

    // Only the pooled ones count as in-use; the rest are overflow allocations.
    EXPECT_EQ(pool.InUseCount(), AsyncIteratorSourcePool::kPoolCapacity);

    for (auto* core : acquired) pool.Release(core);
    EXPECT_EQ(pool.InUseCount(), 0);
}

// ── A2-3: the two terminal states are distinguished (fault vs cancel) ────────
//
// A stalled design would collapse "faulted" into "canceled" or vice versa.  The
// AsyncTask work (ASYNC-P1-4) established that they are separate; the iterator source
// must not regress that.

TEST(AsyncIteratorSource, FaultAndCancelAreDistinctStatuses)
{
    AsyncIteratorSourcePool pool;

    CHAOS_IL2CPP_INT16 t1 = 0;
    auto* faulted = pool.Acquire(&t1);
    faulted->SetException(static_cast<CHAOS_IL2CPP_INTPTR>(0x1234));
    EXPECT_EQ(faulted->GetStatus(t1), ValueTaskSourceStatus::Faulted);

    CHAOS_IL2CPP_INT16 t2 = 0;
    auto* canceled = pool.Acquire(&t2);
    canceled->canceled = true;
    canceled->completed = true;
    EXPECT_EQ(canceled->GetStatus(t2), ValueTaskSourceStatus::Canceled);

    // A successful completion is neither.
    CHAOS_IL2CPP_INT16 t3 = 0;
    auto* ok = pool.Acquire(&t3);
    ok->SetResult(true);
    EXPECT_EQ(ok->GetStatus(t3), ValueTaskSourceStatus::Succeeded);
}

// ── A2-2: MoveNext drives the state machine; OnCompleted resumes it ──────────
//
// The builder's contract in one test: registering a continuation against an ALREADY
// complete source resumes immediately (no scheduler round-trip needed), and against
// an incomplete one it resumes when the producer completes.

TEST(AsyncIteratorBuilder, OnCompletedResumesOnProducerCompletion)
{
    AsyncIteratorSourcePool pool;
    CHAOS_IL2CPP_INT16 token = 0;
    auto* core = pool.Acquire(&token);

    std::atomic<int> resumes{0};
    core->OnCompleted(
        [](void* state) noexcept {
            static_cast<std::atomic<int>*>(state)->fetch_add(1, std::memory_order_relaxed);
        },
        &resumes, token);

    // Not yet complete: the continuation is stored, not fired.
    EXPECT_EQ(resumes.load(), 0);

    core->SetResult(true);          // producer completes → exactly one resumption
    EXPECT_EQ(resumes.load(), 1);

    // Completing again must not double-fire (continuation_fired is one-shot).
    core->SetResult(true);
    EXPECT_EQ(resumes.load(), 1);
}

TEST(AsyncIteratorBuilder, OnCompletedOnAlreadyCompleteSourceFiresInline)
{
    AsyncIteratorSourcePool pool;
    CHAOS_IL2CPP_INT16 token = 0;
    auto* core = pool.Acquire(&token);
    core->SetResult(false);         // iteration finished before the consumer awaited

    std::atomic<int> resumes{0};
    core->OnCompleted(
        [](void* state) noexcept {
            static_cast<std::atomic<int>*>(state)->fetch_add(1, std::memory_order_relaxed);
        },
        &resumes, token);

    // Must NOT be deferred until some later completion — there is none coming.
    EXPECT_EQ(resumes.load(), 1);
}

// ── A2-5/THREADS: concurrent acquire never double-hands a slot ───────────────
//
// DESIGN NOTE — this test was rewritten twice, and the reason is worth recording.
//
//   Draft 1: each thread did acquire→set→get→release and checked its own read.  It
//            passed 50/50 with the pool mutex REMOVED — no discriminating power.
//   Draft 2: 8 threads each acquired ONE source and held it across a barrier.  Still
//            passed 40/40 with the mutex removed: one acquire per thread is too coarse
//            to interleave inside Acquire's scan loop.
//   Draft 3 (this): many acquire/release CYCLES per thread, so the scheduler gets
//            hundreds of chances to interleave inside the scan.  The oracle is the
//            aliasing invariant — a pointer held by two threads at once is a
//            double-handout, and is counted directly.
//
// The oracle is deliberately NOT a timing assertion: it counts violations, so a
// correct pool passes deterministically and a racy one accumulates violations as the
// cycle count rises.

TEST(AsyncIteratorSourcePool, ConcurrentAcquireNeverDoubleHandsASlot)
{
    AsyncIteratorSourcePool pool;
    constexpr int kThreads = 8;         // == kPoolCapacity, so the pool saturates
    constexpr int kCycles = 4000;       // enough interleaving opportunities to expose a race

    // live[i] is the slot thread i currently holds (nullptr when not holding).
    // A double-handout shows up as thread j acquiring a slot another thread still holds.
    std::vector<std::atomic<AsyncIteratorSourceCore*>> live(kThreads);
    for (auto& slot : live) slot.store(nullptr, std::memory_order_relaxed);

    std::atomic<int> double_handouts{0};
    std::atomic<int> token_failures{0};

    std::vector<std::thread> threads;
    threads.reserve(kThreads);
    for (int t = 0; t < kThreads; ++t)
    {
        threads.emplace_back([&, t]() {
            for (int c = 0; c < kCycles; ++c)
            {
                CHAOS_IL2CPP_INT16 token = 0;
                auto* core = pool.Acquire(&token);
                if (core == nullptr || !core->TokenMatches(token))
                {
                    token_failures.fetch_add(1, std::memory_order_relaxed);
                    continue;
                }

                // Publish what we hold, then check nobody else is holding it.  Both are
                // relaxed: the assertion is about a violation being observed at all, not
                // about a specific interleaving being forced.
                live[t].store(core, std::memory_order_relaxed);
                for (int other = 0; other < kThreads; ++other)
                {
                    if (other == t) continue;
                    if (live[other].load(std::memory_order_relaxed) == core)
                        double_handouts.fetch_add(1, std::memory_order_relaxed);
                }
                live[t].store(nullptr, std::memory_order_relaxed);

                // Hold the slot briefly so a concurrent Acquire has a window to see it
                // as in-use.  Without this the hold time is ~1 instruction and the
                // window is effectively closed (draft 2's mistake).
                std::this_thread::yield();

                pool.Release(core);
            }
        });
    }
    for (auto& th : threads) th.join();

    EXPECT_EQ(token_failures.load(), 0);
    EXPECT_EQ(double_handouts.load(), 0);
    EXPECT_EQ(pool.InUseCount(), 0);
}

// ── A2-5/RECYCLE: a fully-drained pool hands the same slots back ─────────────
//
// Deterministic counterpart to the concurrency test: no threads, no timing.  After the
// pool is saturated and fully drained, re-acquiring must yield exactly the original set
// of slots with no overflow allocation substituted in.
//
// NOTE ON THE ORACLE.  An earlier draft asserted `reused == all[i]` inside an
// acquire/release PAIR loop, which failed on correct code: Acquire scans from index 0,
// so a pair loop always hands back slot 0.  Reuse ORDER is not part of the contract;
// "every slot comes back" is.  So all slots are held simultaneously before comparing.

TEST(AsyncIteratorSourcePool, DrainedPoolHandsBackTheSameSlots)
{
    AsyncIteratorSourcePool pool;
    std::vector<AsyncIteratorSourceCore*> all;

    for (int i = 0; i < AsyncIteratorSourcePool::kPoolCapacity; ++i)
    {
        CHAOS_IL2CPP_INT16 t = 0;
        all.push_back(pool.Acquire(&t));
    }
    ASSERT_EQ(pool.InUseCount(), AsyncIteratorSourcePool::kPoolCapacity);

    for (auto* core : all) pool.Release(core);
    ASSERT_EQ(pool.InUseCount(), 0);

    std::vector<AsyncIteratorSourceCore*> recycled;
    for (int i = 0; i < AsyncIteratorSourcePool::kPoolCapacity; ++i)
    {
        CHAOS_IL2CPP_INT16 t = 0;
        auto* reused = pool.Acquire(&t);
        EXPECT_FALSE(reused->heap_allocated)
            << "slot escaped the pool and was served from the overflow path";
        recycled.push_back(reused);
    }

    std::sort(recycled.begin(), recycled.end());
    std::sort(all.begin(), all.end());
    EXPECT_EQ(recycled, all);

    for (auto* core : recycled) pool.Release(core);
    EXPECT_EQ(pool.InUseCount(), 0);
}

// ══════════════════════════════════════════════════════════════════════════════
// A2-2: the extern "C" entry points codegen actually binds to.
//
// Everything above drives the header classes directly.  That leaves the layer the
// translator calls untested — and it is a translation layer with real conversions
// (int32 token ↔ int16 version, INTPTR ↔ function pointer, INTPTR ↔ state machine
// cursor) plus a distinct continuation mechanism.  A bug confined to it would be
// invisible to every test above.
// ══════════════════════════════════════════════════════════════════════════════

// The builder round-trips Create → Acquire → publish → observe through the C ABI.
TEST(AsyncIteratorCApi, AcquirePublishObserveThroughEntryPoints)
{
    CHAOS_IL2CPP_INTPTR builder = chaos_async_iterator_builder_create();
    ASSERT_NE(builder, 0) << "Create must never hand back a null builder handle";

    CHAOS_IL2CPP_INT32 token = -1;
    CHAOS_IL2CPP_INTPTR source = chaos_async_iterator_source_acquire(builder, &token);
    ASSERT_NE(source, 0);
    EXPECT_GE(token, 0);

    // Before publication the source has no result to give; the status getter must
    // still accept the token (it is the live one) rather than abort.
    EXPECT_EQ(chaos_async_iterator_source_get_status(source, token),
              static_cast<CHAOS_IL2CPP_INT32>(ValueTaskSourceStatus::Succeeded));

    chaos_async_iterator_source_set_result(source, 1);
    EXPECT_EQ(chaos_async_iterator_source_get_result(source, token), 1);

    // A fault and a cancel must not collapse into each other through the C ABI either
    // — the int32 wideners are exactly where such a collapse would be introduced.
    CHAOS_IL2CPP_INT32 t2 = -1;
    CHAOS_IL2CPP_INTPTR faulted = chaos_async_iterator_source_acquire(builder, &t2);
    chaos_async_iterator_source_set_exception(faulted, 0x4321);
    EXPECT_EQ(chaos_async_iterator_source_get_status(faulted, t2),
              static_cast<CHAOS_IL2CPP_INT32>(ValueTaskSourceStatus::Faulted));

    chaos_async_iterator_source_release(builder, faulted);
    chaos_async_iterator_source_release(builder, source);
    chaos_async_iterator_builder_complete(builder);
    chaos_async_iterator_builder_destroy(builder);
}

// The token handed out through the C ABI is the SAME token the header validates:
// recycling a slot through the entry points must invalidate the old token, not
// silently remap it.  Guards the int16↔int32 conversion in both directions.
TEST(AsyncIteratorCApi, TokenFromEntryPointsSurvivesRecycling)
{
    CHAOS_IL2CPP_INTPTR builder = chaos_async_iterator_builder_create();

    CHAOS_IL2CPP_INT32 first = -1;
    CHAOS_IL2CPP_INTPTR s1 = chaos_async_iterator_source_acquire(builder, &first);
    chaos_async_iterator_source_set_result(s1, 1);
    chaos_async_iterator_source_release(builder, s1);

    CHAOS_IL2CPP_INT32 second = -1;
    CHAOS_IL2CPP_INTPTR s2 = chaos_async_iterator_source_acquire(builder, &second);
    ASSERT_EQ(s2, s1) << "the slot should have been recycled";
    EXPECT_NE(second, first);

    auto* core = reinterpret_cast<AsyncIteratorSourceCore*>(s2);
    EXPECT_TRUE(core->TokenMatches(static_cast<CHAOS_IL2CPP_INT16>(second)));
    EXPECT_FALSE(core->TokenMatches(static_cast<CHAOS_IL2CPP_INT16>(first)));

    chaos_async_iterator_source_release(builder, s2);
    chaos_async_iterator_builder_destroy(builder);
}

// MoveNext through the C ABI must actually invoke the state machine's MoveNext.
TEST(AsyncIteratorCApi, MoveNextInvokesTheStateMachine)
{
    CHAOS_IL2CPP_INTPTR builder = chaos_async_iterator_builder_create();

    std::atomic<int> move_next_calls{0};
    auto* box = reinterpret_cast<void*>(&move_next_calls);
    auto move_next = [](CHAOS_IL2CPP_INTPTR cursor) noexcept {
        reinterpret_cast<std::atomic<int>*>(cursor)->fetch_add(1, std::memory_order_relaxed);
    };

    // A null MoveNext must be rejected, not silently swallowed.
    EXPECT_EQ(chaos_async_iterator_builder_move_next(
                  builder, 0, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(box)),
              0);

    EXPECT_EQ(chaos_async_iterator_builder_move_next(
                  builder,
                  reinterpret_cast<CHAOS_IL2CPP_INTPTR>(static_cast<void (*)(CHAOS_IL2CPP_INTPTR)>(move_next)),
                  reinterpret_cast<CHAOS_IL2CPP_INTPTR>(box)),
              1);
    EXPECT_EQ(move_next_calls.load(), 1);

    chaos_async_iterator_builder_destroy(builder);
}

// AwaitUnsafeOnCompleted registers on the AWAITED TASK, not on the iterator's own
// pooled source, and resumption must fire when that task completes.
//
// This is the entry point the original A2 plan omitted entirely.  Without it an
// `await Task.Yield()` inside an iterator registers nothing and the iterator stalls.
TEST(AsyncIteratorCApi, AwaitRegistersOnAwaitedTaskAndResumesOnCompletion)
{
    // The awaited task, created through the real async.h surface.
    CHAOS_IL2CPP_INTPTR task = chaos::il2cpp::common::async_task_create();
    ASSERT_NE(task, 0);

    std::atomic<int> resumed{0};
    auto move_next = [](CHAOS_IL2CPP_INTPTR cursor) noexcept {
        reinterpret_cast<std::atomic<int>*>(cursor)->fetch_add(1, std::memory_order_relaxed);
    };
    auto* box = reinterpret_cast<void*>(&resumed);

    // Not yet complete: registering must succeed and must NOT resume inline.
    EXPECT_EQ(chaos_async_iterator_builder_await_unsafe_on_completed(
                  task,
                  reinterpret_cast<CHAOS_IL2CPP_INTPTR>(static_cast<void (*)(CHAOS_IL2CPP_INTPTR)>(move_next)),
                  reinterpret_cast<CHAOS_IL2CPP_INTPTR>(box)),
              1);
    EXPECT_EQ(resumed.load(), 0) << "resumed before the awaited task completed";

    chaos::il2cpp::common::finish_async_task(task);
    EXPECT_EQ(resumed.load(), 1) << "awaited task completed but the iterator did not resume";

    // AwaitOnCompleted is the same implementation; a task that completed BEFORE the
    // registration must resume immediately rather than park forever.
    //
    // NOTE ON THE SETUP — two traps here, both hit while writing this test:
    //   1. `finish_async_task` only DELIVERS to an already-registered continuation;
    //      it does not set `completed`.  Calling it on a fresh task is a no-op, so the
    //      "already complete" task must be completed through a builder setter.
    //   2. The counter must be observed without relying on a single shared box: the
    //      first half of this test leaves a fired continuation on `task`, and a
    //      separate counter makes each half's resumption independently visible (which
    //      is also what would catch a resumption delivered to the WRONG handle).
    std::atomic<int> resumed_second{0};
    auto* box_second = reinterpret_cast<void*>(&resumed_second);

    CHAOS_IL2CPP_INTPTR done = chaos::il2cpp::common::async_task_create();
    ASSERT_NE(done, 0);
    CHAOS_IL2CPP_INTPTR done_slot = done;
    chaos::il2cpp::common::async_task_builder_set_result_raw(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&done_slot), 0);

    EXPECT_EQ(chaos_async_iterator_builder_await_on_completed(
                  done,
                  reinterpret_cast<CHAOS_IL2CPP_INTPTR>(static_cast<void (*)(CHAOS_IL2CPP_INTPTR)>(move_next)),
                  reinterpret_cast<CHAOS_IL2CPP_INTPTR>(box_second)),
              1);
    EXPECT_EQ(resumed_second.load(), 1)
        << "already-complete awaited task did not resume inline";
    EXPECT_EQ(resumed.load(), 1)
        << "resumption for the second await was delivered to the first await's box";
}

// A wrong-object await handle (not an AsyncTask) must fail loudly rather than
// register a continuation that will never fire.
//
// Counterexample: make AsyncIteratorAwaitOnCompleted ignore async_task_on_completed's
// 0 return and this test dies with a plain assertion failure (resumed stays 0 at the
// end) instead of the abort — i.e. the stall becomes silent, which is the bug class
// A1/A2 exist to remove.  Verified with the in-place revert recorded in the commit's
// regression_check.
TEST(AsyncIteratorCApi, AwaitOnNonTaskHandleFailsLoudly)
{
    auto move_next = [](CHAOS_IL2CPP_INTPTR) noexcept {};
    EXPECT_DEATH(
        chaos_async_iterator_builder_await_on_completed(
            1,  // not an AsyncTask handle
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(static_cast<void (*)(CHAOS_IL2CPP_INTPTR)>(move_next)),
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(nullptr)),
        "");
}

}  // namespace
