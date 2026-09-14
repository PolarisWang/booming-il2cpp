// synchronization_context_test.cpp — Phase 3: SynchronizationContext.
//
// THE COUNTEREXAMPLE
// ------------------
// The one property that must hold is that SetSynchronizationContext actually
// INSTALLS its argument, and get_Current reports it back.  If Set silently
// discarded its argument (or stored it per-source rather than per-thread), then
// `Current` would read as "no context" no matter what the caller installed —
// and ConfigureAwait(true) and ConfigureAwait(false) would become
// indistinguishable, because the runtime decides where to resume by reading
// Current at completion time.
//
// This is a set/get round-trip with a negative control: after clearing the
// context, get_Current must report none rather than the previously-installed
// one.  A getter that returns a cached non-null value passes the positive half
// and fails here.
//
// The threaded test pins per-thread semantics: installing a context on thread A
// must not make it current on thread B.

#include <gtest/gtest.h>

#include "synchronization_context.h"
#include "thread_pool.h"

#include <atomic>
#include <chrono>
#include <thread>

using namespace chaos::il2cpp::runtime_core::threading;

// ══════════════════════════════════════════════════════════════════════════════
// 1. Set/Get round-trips — and clearing actually clears
// ══════════════════════════════════════════════════════════════════════════════

TEST(SynchronizationContext, SetThenGetReturnsTheInstalledContext)
{
    SynchronizationContext* ctx = SynchronizationContextCreate();
    ASSERT_NE(ctx, nullptr);

    SynchronizationContext* prev = SynchronizationContextSetCurrent(ctx);
    EXPECT_EQ(prev, nullptr) << "no context was installed before this one";

    EXPECT_EQ(SynchronizationContextGetCurrent(), ctx)
        << "get_Current must report what SetSynchronizationContext installed";

    // Restore for other tests on this thread.
    SynchronizationContextSetCurrent(prev);
    SynchronizationContextDestroy(ctx);
}

TEST(SynchronizationContext, ClearingActuallyClears)
{
    SynchronizationContext* ctx = SynchronizationContextCreate();
    ASSERT_NE(ctx, nullptr);
    SynchronizationContextSetCurrent(ctx);
    ASSERT_EQ(SynchronizationContextGetCurrent(), ctx);

    // Clear it.
    SynchronizationContextSetCurrent(nullptr);
    EXPECT_EQ(SynchronizationContextGetCurrent(), nullptr)
        << "clearing the context must make get_Current report none — a getter "
        << "that caches the previously-installed value would pass the positive "
        << "test above and fail here";

    SynchronizationContextDestroy(ctx);
}

TEST(SynchronizationContext, SetReturnsThePreviousContextForRestoration)
{
    SynchronizationContext* first = SynchronizationContextCreate();
    SynchronizationContext* second = SynchronizationContextCreate();
    ASSERT_NE(first, nullptr);
    ASSERT_NE(second, nullptr);

    SynchronizationContextSetCurrent(first);
    SynchronizationContext* prev = SynchronizationContextSetCurrent(second);
    EXPECT_EQ(prev, first)
        << "Set must return the PREVIOUS context so an awaiter can restore it "
        << "after running a continuation";

    EXPECT_EQ(SynchronizationContextGetCurrent(), second);

    SynchronizationContextSetCurrent(nullptr);
    SynchronizationContextDestroy(first);
    SynchronizationContextDestroy(second);
}

// ══════════════════════════════════════════════════════════════════════════════
// 2. Per-thread, not global
// ══════════════════════════════════════════════════════════════════════════════

TEST(SynchronizationContext, IsPerThreadNotGlobal)
{
    SynchronizationContext* main_ctx = SynchronizationContextCreate();
    ASSERT_NE(main_ctx, nullptr);
    SynchronizationContextSetCurrent(main_ctx);

    std::atomic<CHAOS_IL2CPP_INTPTR> seen_on_other_thread{-1};
    std::thread other([&] {
        seen_on_other_thread.store(
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(SynchronizationContextGetCurrent()));
    });
    other.join();

    EXPECT_EQ(seen_on_other_thread.load(), static_cast<CHAOS_IL2CPP_INTPTR>(0))
        << "a context installed on one thread must NOT be current on another — "
        << "SynchronizationContext is thread-affine by definition";

    // And the main thread still has it.
    EXPECT_EQ(SynchronizationContextGetCurrent(), main_ctx);

    SynchronizationContextSetCurrent(nullptr);
    SynchronizationContextDestroy(main_ctx);
}

// ══════════════════════════════════════════════════════════════════════════════
// 3. Post / Send actually invoke
// ══════════════════════════════════════════════════════════════════════════════

namespace {
std::atomic<int>  g_post_count{0};
std::atomic<void*> g_post_state{nullptr};
std::atomic<std::thread::id> g_post_thread{};

/// Records the invocation and releases any waiter.  The release stores are what
/// let the Post test rendezvous without assuming Post ran the callback inline.
void CountingCallback(void* state) {
    g_post_thread.store(std::this_thread::get_id(), std::memory_order_release);
    g_post_state.store(state, std::memory_order_release);
    g_post_count.fetch_add(1, std::memory_order_release);
}

/// Blocks until the callback has been observed, or the deadline expires.
bool WaitForPost(std::chrono::milliseconds budget = std::chrono::seconds(10)) {
    const auto deadline = std::chrono::steady_clock::now() + budget;
    while (std::chrono::steady_clock::now() < deadline) {
        if (g_post_count.load(std::memory_order_acquire) == 1) return true;
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    return false;
}
}  // namespace

// Post is ASYNCHRONOUS by contract: it queues the callback and returns without
// running it.  This test used to assert `g_post_count == 1` on the line right
// after Post returned, which only a Post that invoked the callback INLINE could
// satisfy — so when Post was corrected to actually queue, the test failed while
// the implementation was right.  A test that pins the opposite of the contract
// it is meant to protect is worse than no test: it makes a correct fix look
// like a regression.
//
// The first attempt at fixing this asserted "the callback did not run before
// Post returned" by reading the counter IMMEDIATELY.  That is still wrong, in a
// subtler way: the worker can land the callback between Post's queue-write and
// the caller's read, so a CORRECT implementation fails intermittently.  The
// sibling test (test_synchronization_context_post) had already rejected that
// approach for exactly this reason.  Timing cannot separate inline from queued.
//
// Thread id can, and does so deterministically: an inline Post runs the
// callback on the CALLER by definition, while a queued Post cannot — the caller
// is parked in the wait below, not running pool work.  So the discriminator is
//
//   inline  => callback ran on the calling thread
//   queued  => callback ran on some other thread
//
// with the counter/state assertions made AFTER the wait, where nothing races.
TEST(SynchronizationContext, PostInvokesTheCallbackWithItsState)
{
    ThreadPoolInitialize();
    g_post_count.store(0);
    g_post_state.store(nullptr);
    SynchronizationContext* ctx = SynchronizationContextCreate();
    ASSERT_NE(ctx, nullptr);

    const auto caller = std::this_thread::get_id();
    int marker = 0;
    EXPECT_TRUE(SynchronizationContextPost(ctx, CountingCallback, &marker));

    ASSERT_TRUE(WaitForPost())
        << "a queued Post callback must still run — asynchrony means 'later', "
           "not 'never'";
    EXPECT_EQ(g_post_count.load(std::memory_order_acquire), 1)
        << "and it must run exactly once";
    EXPECT_EQ(g_post_state.load(std::memory_order_acquire),
              static_cast<void*>(&marker))
        << "and pass the caller's state through";
    EXPECT_NE(g_post_thread.load(std::memory_order_acquire), caller)
        << "Post must dispatch to a ThreadPool worker rather than run inline on "
           "the calling thread; running on the caller makes Post and Send the "
           "same operation, which is the defect T1.5 exists to close";

    SynchronizationContextDestroy(ctx);
    ThreadPoolShutdown();
}

TEST(SynchronizationContext, SendInvokesTheCallbackWithItsState)
{
    g_post_count = 0;
    g_post_state.store(nullptr);
    SynchronizationContext* ctx = SynchronizationContextCreate();
    ASSERT_NE(ctx, nullptr);

    int marker = 0;
    EXPECT_TRUE(SynchronizationContextSend(ctx, CountingCallback, &marker));
    // Send IS synchronous by contract, so here the immediate check is the right
    // one — and it is what distinguishes Send from Post.  If Send ever starts
    // deferring, this fails while the Post test above still passes.
    EXPECT_EQ(g_post_count.load(std::memory_order_acquire), 1)
        << "Send must invoke the callback before returning";
    EXPECT_EQ(g_post_state.load(std::memory_order_acquire),
              static_cast<void*>(&marker));

    SynchronizationContextDestroy(ctx);
}

TEST(SynchronizationContext, InvalidArgumentsAreRejected)
{
    EXPECT_FALSE(SynchronizationContextPost(nullptr, CountingCallback, nullptr));
    EXPECT_FALSE(SynchronizationContextPost(SynchronizationContextCreate(), nullptr, nullptr));
    // Null-safe destroy must not crash.
    SynchronizationContextDestroy(nullptr);
}
