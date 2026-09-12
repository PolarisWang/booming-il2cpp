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
int g_post_count = 0;
void* g_post_state = nullptr;
void CountingCallback(void* state) {
    ++g_post_count;
    g_post_state = state;
}
}  // namespace

TEST(SynchronizationContext, PostInvokesTheCallbackWithItsState)
{
    g_post_count = 0;
    g_post_state = nullptr;
    SynchronizationContext* ctx = SynchronizationContextCreate();
    ASSERT_NE(ctx, nullptr);

    int marker = 0;
    EXPECT_TRUE(SynchronizationContextPost(ctx, CountingCallback, &marker));
    EXPECT_EQ(g_post_count, 1) << "Post must actually invoke the callback";
    EXPECT_EQ(g_post_state, &marker) << "and pass the caller's state through";

    SynchronizationContextDestroy(ctx);
}

TEST(SynchronizationContext, SendInvokesTheCallbackWithItsState)
{
    g_post_count = 0;
    g_post_state = nullptr;
    SynchronizationContext* ctx = SynchronizationContextCreate();
    ASSERT_NE(ctx, nullptr);

    int marker = 0;
    EXPECT_TRUE(SynchronizationContextSend(ctx, CountingCallback, &marker));
    EXPECT_EQ(g_post_count, 1) << "Send must actually invoke the callback";
    EXPECT_EQ(g_post_state, &marker);

    SynchronizationContextDestroy(ctx);
}

TEST(SynchronizationContext, InvalidArgumentsAreRejected)
{
    EXPECT_FALSE(SynchronizationContextPost(nullptr, CountingCallback, nullptr));
    EXPECT_FALSE(SynchronizationContextPost(SynchronizationContextCreate(), nullptr, nullptr));
    // Null-safe destroy must not crash.
    SynchronizationContextDestroy(nullptr);
}
