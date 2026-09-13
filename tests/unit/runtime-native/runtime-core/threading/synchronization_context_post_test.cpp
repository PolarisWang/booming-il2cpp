// SynchronizationContext — Post/Send dispatch semantics (T1.5).
//
// The load-bearing distinction is Post (asynchronous) vs Send (inline).  The
// pre-T1.5 implementation ran the callback inline in BOTH, making the two
// indistinguishable — see synchronization_context.cpp for why that is a real
// semantic defect and not a harmless simplification.
//
// Every test here is written so that an inline-Post implementation FAILS it.

#include <gtest/gtest.h>

#include "synchronization_context.h"
#include "thread_pool.h"

#include <atomic>
#include <chrono>
#include <thread>

using chaos::il2cpp::runtime_core::threading::SynchronizationContextCreate;
using chaos::il2cpp::runtime_core::threading::SynchronizationContextDestroy;
using chaos::il2cpp::runtime_core::threading::SynchronizationContextGetCurrent;
using chaos::il2cpp::runtime_core::threading::SynchronizationContextPost;
using chaos::il2cpp::runtime_core::threading::SynchronizationContextSend;
using chaos::il2cpp::runtime_core::threading::SynchronizationContextSetCurrent;
using chaos::il2cpp::runtime_core::threading::ThreadPoolInitialize;
using chaos::il2cpp::runtime_core::threading::ThreadPoolShutdown;

namespace {

/// Poll until `pred` or the budget runs out.  Deliberately generous: these
/// tests assert ordering, not latency, so a slow worker must not read as a
/// failure.  (Windows timer granularity here makes short fixed sleeps
/// unreliable — sleep_for(2ms) actually costs ~15ms.)
template <typename Pred>
bool WaitFor(Pred pred, int budget_ms = 5000)
{
    const auto deadline = std::chrono::steady_clock::now() +
                          std::chrono::milliseconds(budget_ms);
    while (std::chrono::steady_clock::now() < deadline) {
        if (pred()) return true;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return pred();
}

// ── The core counter-example ─────────────────────────────────
//
// "Post is asynchronous" is asserted as "the callback does not run on the
// calling thread".  That discriminates an inline Post cleanly and WITHOUT a
// timing race: an inline implementation runs the callback on the caller by
// definition, so the thread id alone settles it.  (A "did it run before Post
// returned?" assertion cannot: a fast worker makes that observation
// inherently racy, so it would be flaky against a correct implementation.)
TEST(SynchronizationContextPost, CallbackExecutesOnAWorkerThreadNotTheCaller)
{
    ThreadPoolInitialize();

    std::atomic<bool> ran{false};
    std::atomic<std::thread::id> ran_on{};

    struct Payload {
        std::atomic<bool>* ran;
        std::atomic<std::thread::id>* ran_on;
    };
    Payload payload{&ran, &ran_on};

    auto* ctx = SynchronizationContextCreate();
    ASSERT_NE(ctx, nullptr);

    const auto caller = std::this_thread::get_id();
    const bool queued = SynchronizationContextPost(ctx, [](void* s) {
        auto* p = static_cast<Payload*>(s);
        p->ran_on->store(std::this_thread::get_id(), std::memory_order_release);
        p->ran->store(true, std::memory_order_release);
    }, &payload);

    EXPECT_TRUE(queued);

    ASSERT_TRUE(WaitFor([&] { return ran.load(std::memory_order_acquire); }))
        << "a posted callback must run";

    // Inline Post would make this false: it would have run on `caller`.
    EXPECT_NE(ran_on.load(std::memory_order_acquire), caller)
        << "Post must dispatch to a ThreadPool worker, not run inline on the "
           "calling thread — running on the caller makes Post and Send the "
           "same operation, which is the defect T1.5 exists to close";

    SynchronizationContextDestroy(ctx);
    ThreadPoolShutdown();
}

// ── Send stays inline (the other half of the contract) ─────────────────

TEST(SynchronizationContextSend, SendRunsInlineOnTheCallingThread)
{
    std::atomic<bool> ran{false};
    std::atomic<std::thread::id> ran_on{};

    auto* ctx = SynchronizationContextCreate();
    ASSERT_NE(ctx, nullptr);

    const auto caller = std::this_thread::get_id();
    struct Payload { std::atomic<bool>* ran; std::atomic<std::thread::id>* ran_on; };
    Payload payload{&ran, &ran_on};

    const bool ok = SynchronizationContextSend(ctx, [](void* s) {
        auto* p = static_cast<Payload*>(s);
        p->ran_on->store(std::this_thread::get_id(), std::memory_order_release);
        p->ran->store(true, std::memory_order_release);
    }, &payload);

    EXPECT_TRUE(ok);
    // Send is synchronous: by the time it returns the work is already done.
    EXPECT_TRUE(ran.load(std::memory_order_acquire))
        << "Send must be synchronous — the callback is complete on return";
    EXPECT_EQ(ran_on.load(std::memory_order_acquire), caller)
        << "Send runs inline on the calling thread";

    SynchronizationContextDestroy(ctx);
}

// ── Argument validation and .Current bookkeeping ───────────────────────

TEST(SynchronizationContextPost, RejectsNullContextAndNullCallback)
{
    auto* ctx = SynchronizationContextCreate();
    ASSERT_NE(ctx, nullptr);

    EXPECT_FALSE(SynchronizationContextPost(nullptr, [](void*) {}, nullptr));
    EXPECT_FALSE(SynchronizationContextPost(ctx, nullptr, nullptr));
    EXPECT_FALSE(SynchronizationContextSend(nullptr, [](void*) {}, nullptr));
    EXPECT_FALSE(SynchronizationContextSend(ctx, nullptr, nullptr));

    SynchronizationContextDestroy(ctx);
}

// .Current must faithfully round-trip what was Set, because ConfigureAwait(true)
// and ConfigureAwait(false) are distinguished by reading it.  A Set that
// silently discards its argument makes the two indistinguishable.
TEST(SynchronizationContextCurrent, SetAndGetRoundTrip)
{
    auto* a = SynchronizationContextCreate();
    auto* b = SynchronizationContextCreate();
    ASSERT_NE(a, nullptr);
    ASSERT_NE(b, nullptr);

    EXPECT_EQ(SynchronizationContextGetCurrent(), nullptr)
        << "a thread starts with no context installed";

    auto* prev = SynchronizationContextSetCurrent(a);
    EXPECT_EQ(prev, nullptr);
    EXPECT_EQ(SynchronizationContextGetCurrent(), a);

    prev = SynchronizationContextSetCurrent(b);
    EXPECT_EQ(prev, a) << "SetCurrent must return the PREVIOUS context";
    EXPECT_EQ(SynchronizationContextGetCurrent(), b);

    // Restore and clear.
    SynchronizationContextSetCurrent(a);
    SynchronizationContextSetCurrent(nullptr);
    EXPECT_EQ(SynchronizationContextGetCurrent(), nullptr);

    SynchronizationContextDestroy(a);
    SynchronizationContextDestroy(b);
}

}  // namespace
