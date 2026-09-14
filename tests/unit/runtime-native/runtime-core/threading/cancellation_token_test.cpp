// CancellationToken / CancellationTokenSource — native semantics tests.
//
// These are COUNTEREXAMPLE tests in the sense the roadmap requires for T1.1:
// each asserts the behaviour that a silent no-op implementation would get
// WRONG, so a regression to "check the flag and return" fails here instead of
// passing quietly.
//
// The ABI is (source_id, ...) with 0 == CancellationToken.None.  A token's
// "cancellation state" is the state of its source.

#include <gtest/gtest.h>

#include "cancellation_token.h"
#include "exception_helpers.h"

#include <csetjmp>
#include <cstdlib>
#include <functional>

extern "C" {
CHAOS_IL2CPP_INT32 chaos_cancellation_token_is_cancellation_requested(CHAOS_IL2CPP_INT32);
CHAOS_IL2CPP_INT32 chaos_cancellation_token_can_be_canceled(CHAOS_IL2CPP_INT32);
void chaos_cancellation_token_throw_if_cancellation_requested(CHAOS_IL2CPP_INT32);
CHAOS_IL2CPP_INT32 chaos_cancellation_token_source_create(void);
void chaos_cancellation_token_source_cancel(CHAOS_IL2CPP_INT32);
void chaos_cancellation_token_source_dispose(CHAOS_IL2CPP_INT32);
CHAOS_IL2CPP_INT32 chaos_cancellation_token_source_get_token(CHAOS_IL2CPP_INT32);
}

namespace {

using chaos::il2cpp::runtime_core::SetExceptionFallback;

// RaiseManagedException aborts when no runtime modules are registered (this TU
// has none); SetExceptionFallback replaces that abort with a longjmp back to the
// raise site, so a test can observe that a raise happened at all.  Same shape as
// async_task_exception_test.cpp — the fallback takes no arguments, so the
// assertion keys on the caller-visible distinction ("did this return, or did it
// raise?"), which is exactly what the silent-return defect violates.
jmp_buf g_raise_jmp;
bool    g_raise_armed = false;

extern "C" void RecordRaisedException() {
    if (g_raise_armed) {
        longjmp(g_raise_jmp, 1);
    }
    std::abort();  // Unexpected raise while nothing was armed.
}

/// Runs `fn`; returns true if it raised a managed exception (fallback fired).
bool DidRaise(const std::function<void()>& fn) {
    SetExceptionFallback(&RecordRaisedException);

    if (setjmp(g_raise_jmp) == 0) {
        g_raise_armed = true;
        fn();
        g_raise_armed = false;
        return false;
    }

    g_raise_armed = false;
    return true;
}

// None (id 0) is a valid, inert token: never cancelled, cannot be cancelled.
TEST(CancellationToken, NoneIsInert)
{
    EXPECT_EQ(chaos_cancellation_token_is_cancellation_requested(0), 0);
    EXPECT_EQ(chaos_cancellation_token_can_be_canceled(0), 0);
}

// A live source starts uncancelled and becomes cancelled on cancel.
TEST(CancellationToken, SourceObservesItsOwnCancellation)
{
    const auto src = chaos_cancellation_token_source_create();
    ASSERT_NE(src, 0);

    EXPECT_EQ(chaos_cancellation_token_can_be_canceled(src), 1);
    EXPECT_EQ(chaos_cancellation_token_is_cancellation_requested(src), 0)
        << "a freshly created source must not report cancellation";

    chaos_cancellation_token_source_cancel(src);
    EXPECT_EQ(chaos_cancellation_token_is_cancellation_requested(src), 1)
        << "cancel() must be observable through the token";

    chaos_cancellation_token_source_dispose(src);
}

// get_token is the identity on the source id.  It exists as a named entry point
// so codegen does not route it to a fallback that would return 0 (None) and
// silently degrade a real token into an uncancellable one.
TEST(CancellationToken, GetTokenIsIdentityAndNotNone)
{
    const auto src = chaos_cancellation_token_source_create();
    ASSERT_NE(src, 0);

    const auto tok = chaos_cancellation_token_source_get_token(src);
    EXPECT_EQ(tok, src);
    EXPECT_NE(tok, 0) << "a real source must not collapse to CancellationToken.None";

    // Cancelling through the source must be visible through the token.
    chaos_cancellation_token_source_cancel(src);
    EXPECT_EQ(chaos_cancellation_token_is_cancellation_requested(tok), 1);

    chaos_cancellation_token_source_dispose(src);
}

// ── The load-bearing T1.1 assertion ────────────────────────────────────
//
// ThrowIfCancellationRequested on a CANCELLED token must actually raise.
// The pre-T1.1 implementation checked the flag and returned, so managed code
// after the call kept running — the call site looked right and the body
// executed anyway.
//
// Note the two-sided shape: the uncancelled case must NOT throw.  Asserting
// only the throwing half would pass for an implementation that always throws.
TEST(CancellationToken, ThrowIfCancellationRequestedRaisesOnCancelledToken)
{
    const auto src = chaos_cancellation_token_source_create();
    ASSERT_NE(src, 0);

    // Uncancelled: must return normally.  Asserting only the throwing half would
    // pass for an implementation that always throws.
    const bool raised_when_live = DidRaise(
        [&] { chaos_cancellation_token_throw_if_cancellation_requested(src); });
    EXPECT_FALSE(raised_when_live)
        << "an uncancelled token must not raise";

    // Cancelled: must raise.
    chaos_cancellation_token_source_cancel(src);
    const bool raised_when_cancelled = DidRaise(
        [&] { chaos_cancellation_token_throw_if_cancellation_requested(src); });
    ASSERT_TRUE(raised_when_cancelled)
        << "ThrowIfCancellationRequested on a cancelled token MUST raise — "
           "returning normally lets the enclosing method body run on, which is "
           "the silent-wrong-result failure this test exists to catch";

    chaos_cancellation_token_source_dispose(src);
}

// None is exempt: ThrowIfCancellationRequested on None is a documented no-op.
TEST(CancellationToken, ThrowIfCancellationRequestedOnNoneDoesNotRaise)
{
    const bool raised = DidRaise(
        [] { chaos_cancellation_token_throw_if_cancellation_requested(0); });
    EXPECT_FALSE(raised) << "CancellationToken.None must never throw";
}

// ── Linked sources (CreateLinkedTokenSource) ───────────────────────────
using chaos::il2cpp::runtime_core::threading::CancellationTokenSourceCreateLinked;

TEST(CancellationToken, LinkedSourceCancelsWhenAnyInputCancels)
{
    const auto a = chaos_cancellation_token_source_create();
    const auto b = chaos_cancellation_token_source_create();
    ASSERT_NE(a, 0);
    ASSERT_NE(b, 0);

    const uint32_t inputs[2] = {static_cast<uint32_t>(a), static_cast<uint32_t>(b)};
    const auto linked = CancellationTokenSourceCreateLinked(inputs, 2);
    ASSERT_NE(linked, 0u);

    EXPECT_EQ(chaos_cancellation_token_is_cancellation_requested(
                  static_cast<CHAOS_IL2CPP_INT32>(linked)), 0);

    // Cancelling the SECOND input must propagate — an implementation that only
    // wired the first would pass a single-input test.
    chaos_cancellation_token_source_cancel(b);
    EXPECT_EQ(chaos_cancellation_token_is_cancellation_requested(
                  static_cast<CHAOS_IL2CPP_INT32>(linked)), 1)
        << "cancelling any input must cancel the linked source";

    chaos_cancellation_token_source_dispose(a);
    chaos_cancellation_token_source_dispose(b);
    chaos_cancellation_token_source_dispose(static_cast<CHAOS_IL2CPP_INT32>(linked));
}

// Propagation is one-way: cancelling the linked source must NOT cancel inputs.
TEST(CancellationToken, LinkedSourcePropagatesOneWayOnly)
{
    const auto a = chaos_cancellation_token_source_create();
    ASSERT_NE(a, 0);

    const uint32_t inputs[1] = {static_cast<uint32_t>(a)};
    const auto linked = CancellationTokenSourceCreateLinked(inputs, 1);
    ASSERT_NE(linked, 0u);

    chaos_cancellation_token_source_cancel(static_cast<CHAOS_IL2CPP_INT32>(linked));
    EXPECT_EQ(chaos_cancellation_token_is_cancellation_requested(a), 0)
        << "cancelling the linked source must not cancel its inputs";

    chaos_cancellation_token_source_dispose(a);
    chaos_cancellation_token_source_dispose(static_cast<CHAOS_IL2CPP_INT32>(linked));
}

// An already-cancelled input must yield a born-cancelled link (Register fires
// the callback inline in that case).
TEST(CancellationToken, LinkedSourceFromAlreadyCancelledInputIsBornCancelled)
{
    const auto a = chaos_cancellation_token_source_create();
    ASSERT_NE(a, 0);
    chaos_cancellation_token_source_cancel(a);

    const uint32_t inputs[1] = {static_cast<uint32_t>(a)};
    const auto linked = CancellationTokenSourceCreateLinked(inputs, 1);
    ASSERT_NE(linked, 0u);

    EXPECT_EQ(chaos_cancellation_token_is_cancellation_requested(
                  static_cast<CHAOS_IL2CPP_INT32>(linked)), 1)
        << "a link over an already-cancelled source must be born cancelled";

    chaos_cancellation_token_source_dispose(a);
    chaos_cancellation_token_source_dispose(static_cast<CHAOS_IL2CPP_INT32>(linked));
}

// A 0 (None) entry can never cancel, so skipping it must leave the link live.
TEST(CancellationToken, LinkedSourceSkipsNoneEntries)
{
    const auto a = chaos_cancellation_token_source_create();
    ASSERT_NE(a, 0);

    const uint32_t inputs[2] = {0, static_cast<uint32_t>(a)};
    const auto linked = CancellationTokenSourceCreateLinked(inputs, 2);
    ASSERT_NE(linked, 0u);

    EXPECT_EQ(chaos_cancellation_token_is_cancellation_requested(
                  static_cast<CHAOS_IL2CPP_INT32>(linked)), 0)
        << "a None entry must not cancel the link";

    chaos_cancellation_token_source_cancel(a);
    EXPECT_EQ(chaos_cancellation_token_is_cancellation_requested(
                  static_cast<CHAOS_IL2CPP_INT32>(linked)), 1);

    chaos_cancellation_token_source_dispose(a);
    chaos_cancellation_token_source_dispose(static_cast<CHAOS_IL2CPP_INT32>(linked));
}

}  // namespace
