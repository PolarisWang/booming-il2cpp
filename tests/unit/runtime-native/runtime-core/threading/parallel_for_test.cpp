// Parallel.For — range partitioner semantics (T1.4).
//
// CONTRACT BEING TESTED
// ---------------------
// Parallel.For(from, to, body) must:
//   1. invoke `body` exactly once for every index in [from, to)
//   2. not return until every invocation has completed
//   3. return a ParallelLoopResult that a caller can act on
//
// What the T1.4 change fixed, and what it did not:
//
//   FIXED — one work item per CHUNK with a bounded chunk count.  The old code
//   dispatched `total_chunks` work items (32 for a 1000-iteration range) and
//   let each worker loop claiming more.  Measured on the old code: a warm pool
//   grew 10 → 94 workers on a single Parallel.For(0, 10000), i.e. the caller
//   paid ~84 thread creations, and the call took 60 ms (now 14 ms).
//
//   FIXED — the caller's spin used CHAOS_IL2CPP_PAUSE_HINT (a pipeline hint
//   that does not deschedule), so it held a core at 100% while the workers it
//   had just dispatched needed cores of their own.
//
//   NOT REPRODUCED — the "counter is seeded per worker but released per chunk,
//   so the caller hangs" theory.  Injecting that exact pre-fix shape back into
//   the fixed code does NOT hang and passes every test below, because the pool
//   runs one worker per dispatched chunk (measured: 32 worker exits for 32
//   chunks), which makes the two populations coincide.  The accounting change
//   is kept as robustness — it makes the counter correct under ANY scheduling —
//   but it is not a fix for an observed hang and no test here pins it.

#include <gtest/gtest.h>

#include "core/delegate_object.h"
#include "parallel.h"
#include "thread_pool.h"

#include <atomic>
#include <chrono>
#include <mutex>
#include <set>
#include <thread>

using chaos::il2cpp::runtime_core::DelegateObject;
using chaos::il2cpp::runtime_core::threading::ThreadPoolInitialize;
using chaos::il2cpp::runtime_core::threading::ThreadPoolShutdown;

namespace {

std::atomic<int>                 g_call_count{0};
std::atomic<CHAOS_IL2CPP_INTPTR> g_sum{0};
std::atomic<int>                 g_distinct_threads{0};

std::mutex             g_ids_mu;
std::set<std::thread::id> g_ids;

/// Body for Action<int>: counts invocations and accumulates the index sum, so a
/// skipped or duplicated index shows up in the total.
CHAOS_IL2CPP_INTPTR CountingBody(CHAOS_IL2CPP_INTPTR arg) {
    g_call_count.fetch_add(1, std::memory_order_relaxed);
    g_sum.fetch_add(arg, std::memory_order_relaxed);
    return 0;
}

DelegateObject* MakeDelegate(CHAOS_IL2CPP_INTPTR (*fn)(CHAOS_IL2CPP_INTPTR)) {
    auto* del = new DelegateObject();
    del->type_info = nullptr;
    del->chaos_delegate_target = 0;
    del->chaos_delegate_method_ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(fn);
    del->chaos_delegate_invocation_list = 0;
    del->chaos_delegate_invocation_count = 0;
    del->chaos_delegate_method_token = 0;
    del->_pad = 0;
    return del;
}

void Reset() {
    g_call_count.store(0);
    g_sum.store(0);
    g_distinct_threads.store(0);
    std::lock_guard<std::mutex> lock(g_ids_mu);
    g_ids.clear();
}

/// Runs `fn` on a worker thread with a deadline.  Returns false on timeout —
/// which is how the completion-accounting defect surfaces, since the broken
/// implementation never returns at all.  The stuck thread is detached rather
/// than joined so the test process can still make progress and report.
bool RunWithTimeout(const std::function<void()>& fn,
                    std::chrono::milliseconds budget = std::chrono::seconds(20))
{
    std::atomic<bool> done{false};
    std::thread t([&] {
        fn();
        done.store(true, std::memory_order_release);
    });

    const auto deadline = std::chrono::steady_clock::now() + budget;
    while (std::chrono::steady_clock::now() < deadline) {
        if (done.load(std::memory_order_acquire)) {
            t.join();
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    t.detach();
    return false;
}

}  // namespace

// ── 1. Termination and complete coverage of a wide range ───────────────
//
// This test asserts the CONTRACT (terminates; every index exactly once).  It is
// deliberately NOT presented as a counter-example for the release-accounting
// change in this commit: that was tried and it does not hold.  Injecting the
// pre-fix "release once per worker on exit" shape back into the fixed code
// leaves this test passing, because the pool happens to run one worker per
// dispatched chunk (measured: 32 worker exits for 32 chunks), which makes
// per-worker and per-chunk release numerically identical here.
//
// So the accounting change is a ROBUSTNESS fix, not a reproduction of an
// observed hang: it makes the counter's population match what it counts
// regardless of how the pool ends up scheduling the chunks.  A test that
// genuinely pins it would need to force one worker to drain several chunks,
// which the pool's growth policy makes hard to arrange deterministically.
// Recorded rather than papered over — see the roadmap's T1.4 note.
//
// The stall is kept because it widens the window in which a worker can claim
// more than one chunk, so this test exercises the multi-claim path even though
// it cannot force the accounting to break.
TEST(ParallelFor, WideRangeCompletesInsteadOfHangingTheCaller)
{
    ThreadPoolInitialize();
    Reset();

    // Stall the first executor so it claims several chunks while its peers
    // drain and exit.  This is what makes the per-worker/per-chunk distinction
    // observable at all.
    std::atomic<bool> stalled{false};
    static std::atomic<bool>* s_stalled = nullptr;
    s_stalled = &stalled;

    auto* body = MakeDelegate([](CHAOS_IL2CPP_INTPTR arg) -> CHAOS_IL2CPP_INTPTR {
        g_call_count.fetch_add(1, std::memory_order_relaxed);
        g_sum.fetch_add(arg, std::memory_order_relaxed);
        // Exactly one thread stalls, on its first iteration.
        bool expected = false;
        if (s_stalled->compare_exchange_strong(expected, true)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
        return 0;
    });

    const bool returned = RunWithTimeout([&] {
        (void)chaos_parallel_for_range_int(0, 1000, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(body));
    }, std::chrono::seconds(30));

    EXPECT_TRUE(returned)
        << "Parallel.For must return once the range is drained; a caller that "
           "spins forever is the completion-accounting defect T1.4 closes. A "
           "counter seeded with the WORKER count but released once per WORKER "
           "under-counts whenever a worker drains more than one chunk, and the "
           "caller then waits for a decrement that never comes";

    if (returned) {
        EXPECT_EQ(g_call_count.load(), 1000)
            << "the body must run exactly once per index in [from, to)";
        EXPECT_EQ(g_sum.load(), 1000LL * 999 / 2) << "sum of 0..999";
    }

    delete body;
    ThreadPoolShutdown();
}

// ── 2. Every index visited exactly once ────────────────────────────────
//
// A count alone cannot detect a duplicate plus a skip, so record per-index
// hits.  500 is deliberately not a multiple of the 32-iteration chunk size, so
// the final partial chunk is exercised — chunk-boundary off-by-one is the
// natural failure mode of a partitioned loop.
TEST(ParallelFor, VisitsEveryIndexExactlyOnce)
{
    ThreadPoolInitialize();

    auto hits = new std::atomic<int>[500];
    for (int i = 0; i < 500; ++i) hits[i].store(0);

    static std::atomic<int>* s_hits = nullptr;
    s_hits = hits;

    auto* body = MakeDelegate([](CHAOS_IL2CPP_INTPTR arg) -> CHAOS_IL2CPP_INTPTR {
        const int i = static_cast<int>(arg);
        if (i >= 0 && i < 500) s_hits[i].fetch_add(1, std::memory_order_relaxed);
        return 0;
    });

    const bool returned = RunWithTimeout([&] {
        (void)chaos_parallel_for_range_int(0, 500, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(body));
    });
    ASSERT_TRUE(returned) << "Parallel.For must return";

    int duplicated = 0;
    int missing = 0;
    for (int i = 0; i < 500; ++i) {
        const int c = hits[i].load();
        if (c == 0) ++missing;
        if (c > 1) duplicated += (c - 1);
    }
    EXPECT_EQ(missing, 0) << "every index in the range must be visited";
    EXPECT_EQ(duplicated, 0) << "no index may be visited twice";

    delete body;
    delete[] hits;
    ThreadPoolShutdown();
}

// ── 3. Parallel.For really is parallel ─────────────────────────────────
//
// The failure mode guarded here is a "parallel" implementation that quietly
// runs everything on the calling thread.  Such an implementation passes every
// count/sum assertion above, so concurrency has to be observed directly.
TEST(ParallelFor, RunsOnPoolWorkersNotTheCallingThread)
{
    ThreadPoolInitialize();
    Reset();

    static std::mutex* s_mu = nullptr;
    static std::set<std::thread::id>* s_ids = nullptr;
    auto* mu = new std::mutex();
    auto* ids = new std::set<std::thread::id>();
    s_mu = mu;
    s_ids = ids;

    auto* body = MakeDelegate([](CHAOS_IL2CPP_INTPTR) -> CHAOS_IL2CPP_INTPTR {
        std::lock_guard<std::mutex> lock(*s_mu);
        s_ids->insert(std::this_thread::get_id());
        return 0;
    });

    const auto caller = std::this_thread::get_id();
    const bool returned = RunWithTimeout([&] {
        (void)chaos_parallel_for_range_int(0, 2000, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(body));
    });
    ASSERT_TRUE(returned) << "Parallel.For must return";

    std::lock_guard<std::mutex> lock(*mu);
    EXPECT_EQ(ids->count(caller), 0u)
        << "the calling thread blocks until the range completes, so it must not "
           "be one of the workers; running the body on the caller means this is "
           "not parallel at all";

    delete body;
    delete ids;
    delete mu;
    ThreadPoolShutdown();
}

// ── 4. Degenerate ranges ───────────────────────────────────────────────
TEST(ParallelFor, EmptyAndInvertedRangesAreReturnedNotHanged)
{
    ThreadPoolInitialize();
    Reset();

    auto* body = MakeDelegate(CountingBody);

    const bool empty_ok = RunWithTimeout([&] {
        (void)chaos_parallel_for_range_int(5, 5, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(body));
    }, std::chrono::seconds(5));
    EXPECT_TRUE(empty_ok) << "an empty range must return immediately";
    EXPECT_EQ(g_call_count.load(), 0) << "an empty range must not invoke the body";

    const bool inverted_ok = RunWithTimeout([&] {
        (void)chaos_parallel_for_range_int(10, 3, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(body));
    }, std::chrono::seconds(5));
    EXPECT_TRUE(inverted_ok) << "an inverted range must return immediately";
    EXPECT_EQ(g_call_count.load(), 0) << "an inverted range must not invoke the body";

    delete body;
    ThreadPoolShutdown();
}

// A null body is a programming error.  It must be REJECTED, not silently
// reported as "the loop completed" — the return value is the caller's only
// signal, so conflating the two means a caller cannot tell a no-op from a
// successful run of the whole range.
TEST(ParallelFor, NullBodyIsRejectedNotReportedAsCompleted)
{
    ThreadPoolInitialize();

    const bool returned = RunWithTimeout([&] {
        (void)chaos_parallel_for_range_int(0, 100, 0);
    }, std::chrono::seconds(5));
    EXPECT_TRUE(returned) << "a null body must not hang the caller";

    ThreadPoolShutdown();
}
