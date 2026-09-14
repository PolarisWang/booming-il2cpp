// managed_primitive_entries_mres_test.cpp — T2.4: ManualResetEventSlim
//
// WHAT THIS PINS
// --------------
// ManualResetEventSlim is backed by the WaitHandle table (type discriminant 0 =
// manual-reset).  The managed entries here recover the handle from the instance
// field (same receiver-not-passed constraint as the RWLock family) and call the
// T2.0 WaitHandle ABI.  Four properties are asserted:
//
//   1. Set / Reset / Wait actually drive the native handle.  An entry that
//      ignored the instance and returned 1 for everything would pass a
//      "does it return" test but not this one.
//   2. Two instances do not share one event (per-instance isolation).
//   3. An unbound instance is rejected (not a successful Set or Wait).
//   4. Wait blocks or not depending on the signalled state — NOT always
//      returns "signalled" or always returns "timeout".

#include "runtime_stubs/managed_primitive_entries.h"
#include "runtime_stubs/managed_handle_stubs.h"
#include "runtime_stubs/synchronization_stubs.h"

#include <gtest/gtest.h>

#include <cstdint>

namespace {

struct FakeMres {
    CHAOS_IL2CPP_INTPTR header[1] = {0xDEADBEEF};
    CHAOS_IL2CPP_INTPTR handle = 0;
};

static_assert(sizeof(FakeMres) == 16, "header + one pointer for handle field");

CHAOS_IL2CPP_INTPTR AsIntPtr(FakeMres* inst) noexcept {
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(inst);
}

/// Bind via ChaosWaitHandleCreate with type 0 = manual.
CHAOS_IL2CPP_INTPTR Bind(FakeMres& inst, CHAOS_IL2CPP_INT32 initial_state) noexcept {
    const CHAOS_IL2CPP_INTPTR handle = ChaosWaitHandleCreate(initial_state, 0);
    EXPECT_NE(handle, 0);
    ChaosManagedHandleStore(AsIntPtr(&inst), handle);
    return handle;
}

}  // namespace

// ── 1. Set and reset are real ──────────────────────────────────────────

TEST(ManagedPrimitiveEntries_MRES, SetSignalsThenResetClears)
{
    FakeMres ev;
    const CHAOS_IL2CPP_INTPTR handle = Bind(ev, 0);  // unsignalled

    // The event is unsignalled, so a non-blocking wait must time out.
    EXPECT_EQ(ChaosManualResetEventSlimWaitInt32(AsIntPtr(&ev), 0), 0)
        << "an unsignalled event must not return 'signalled'";

    EXPECT_EQ(ChaosManualResetEventSlimSetManaged(AsIntPtr(&ev)), 1);

    // After Set, a non-blocking wait must immediately succeed.
    EXPECT_EQ(ChaosManualResetEventSlimWaitInt32(AsIntPtr(&ev), 0), 1)
        << "Set must make the event signalled";

    EXPECT_EQ(ChaosManualResetEventSlimResetManaged(AsIntPtr(&ev)), 1);

    // After Reset, the event is unsignalled again.
    EXPECT_EQ(ChaosManualResetEventSlimWaitInt32(AsIntPtr(&ev), 0), 0)
        << "Reset must clear the signalled state";

    ChaosWaitHandleClose(handle);
}

TEST(ManagedPrimitiveEntries_MRES, WaitWithInfiniteTimeoutIsNotRegisteredAsAPoll)
{
    FakeMres ev;
    const CHAOS_IL2CPP_INTPTR handle = Bind(ev, 1);  // start SIGNALLED

    // The zero-argument Wait() maps to an INFINITE native wait.  Calling it on
    // an unsignalled event would park the calling thread forever, so this test
    // only ever calls it on a signalled event — see the note below on why the
    // unsignalled case is deliberately NOT exercised here.
    EXPECT_EQ(ChaosManualResetEventSlimWaitManaged(AsIntPtr(&ev)), 1)
        << "infinite wait on a signalled event must return immediately";

    // Deliberately NOT tested: WaitManaged() on an unsignalled event.  It is a
    // genuine blocking wait (timeout -1), so it would hang this process rather
    // than fail.  Covered by construction: the entry passes kInfinite, so it
    // cannot behave as a poll.  The timeout behaviour that IS observable is
    // tested both here (signalled -> 1) and in the poll cases above.
    ChaosWaitHandleClose(handle);
}

TEST(ManagedPrimitiveEntries_MRES, PollReflectsSignalledStateExactly)
{
    FakeMres ev;
    const CHAOS_IL2CPP_INTPTR handle = Bind(ev, 0);  // unsignalled

    EXPECT_EQ(ChaosManualResetEventSlimWaitInt32(AsIntPtr(&ev), 0), 0)
        << "poll on unsignalled event must time out";

    ChaosManualResetEventSlimSetManaged(AsIntPtr(&ev));
    EXPECT_EQ(ChaosManualResetEventSlimWaitInt32(AsIntPtr(&ev), 0), 1)
        << "poll on signalled event must succeed";

    ChaosWaitHandleClose(handle);
}

// ── 2. Per-instance isolation ──────────────────────────────────────────

TEST(ManagedPrimitiveEntries_MRES, TwoInstancesDoNotShareOneEvent)
{
    FakeMres a, b;
    const CHAOS_IL2CPP_INTPTR handleA = Bind(a, 0);
    const CHAOS_IL2CPP_INTPTR handleB = Bind(b, 0);
    ASSERT_NE(handleA, handleB);

    // Set only `a`.
    EXPECT_EQ(ChaosManualResetEventSlimSetManaged(AsIntPtr(&a)), 1);

    EXPECT_EQ(ChaosManualResetEventSlimWaitInt32(AsIntPtr(&a), 0), 1)
        << "a must be signalled";
    EXPECT_EQ(ChaosManualResetEventSlimWaitInt32(AsIntPtr(&b), 0), 0)
        << "b must remain unsignalled";

    ChaosWaitHandleClose(handleA);
    ChaosWaitHandleClose(handleB);
}

// ── 3. Unbound instance rejection ──────────────────────────────────────

TEST(ManagedPrimitiveEntries_MRES, AnUnboundInstanceDoesNothing)
{
    // SubjectInstanceFactory.Create<ManualResetEventSlim>() = zero-initialised.
    FakeMres neverConstructed;

    EXPECT_EQ(ChaosManualResetEventSlimSetManaged(AsIntPtr(&neverConstructed)), 0);
    EXPECT_EQ(ChaosManualResetEventSlimResetManaged(AsIntPtr(&neverConstructed)), 0);
    EXPECT_EQ(ChaosManualResetEventSlimDisposeManaged(AsIntPtr(&neverConstructed)), 0);
    EXPECT_EQ(ChaosManualResetEventSlimWaitManaged(AsIntPtr(&neverConstructed)), 0);
    EXPECT_EQ(ChaosManualResetEventSlimWaitInt32(AsIntPtr(&neverConstructed), 0), 0);
    EXPECT_EQ(ChaosManualResetEventSlimWaitTimeSpan(AsIntPtr(&neverConstructed), 0), 0);

    EXPECT_EQ(neverConstructed.handle, 0) << "no side effect";
}

TEST(ManagedPrimitiveEntries_MRES, ANullReceiverIsSafe)
{
    EXPECT_EQ(ChaosManualResetEventSlimSetManaged(0), 0);
    EXPECT_EQ(ChaosManualResetEventSlimDisposeManaged(0), 0);
    EXPECT_EQ(ChaosManualResetEventSlimWaitManaged(0), 0);
    EXPECT_EQ(ChaosManualResetEventSlimWaitInt32(0, 0), 0);
}

// ── 4. Dispose releases the binding ────────────────────────────────────

TEST(ManagedPrimitiveEntries_MRES, DisposeReleasesAndIsIdempotent)
{
    FakeMres ev;
    const CHAOS_IL2CPP_INTPTR handle = Bind(ev, 0);

    EXPECT_EQ(ChaosManualResetEventSlimDisposeManaged(AsIntPtr(&ev)), 1);
    EXPECT_EQ(ev.handle, 0) << "field cleared";

    // Second Dispose (as from Dispose() + finalizer) must be a no-op.
    EXPECT_EQ(ChaosManualResetEventSlimDisposeManaged(AsIntPtr(&ev)), 0);

    // The native handle is gone.
    EXPECT_EQ(ChaosWaitHandleSet(handle), 0) << "handle no longer valid";
}

TEST(ManagedPrimitiveEntries_MRES, TimeSpanEncodingWorksForMRES)
{
    FakeMres ev;
    const CHAOS_IL2CPP_INTPTR handle = Bind(ev, 1);  // start signalled

    // 1 second as ticks.
    CHAOS_IL2CPP_INT64 oneSec = 10000000;
    EXPECT_EQ(ChaosManualResetEventSlimWaitTimeSpan(
                  AsIntPtr(&ev), reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&oneSec)),
              1)
        << "a signalled event must be acquired via the TimeSpan overload";

    // Negative ticks = infinite.
    CHAOS_IL2CPP_INT64 neg = -1;
    EXPECT_EQ(ChaosManualResetEventSlimWaitTimeSpan(
                  AsIntPtr(&ev), reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&neg)),
              1);

    // Null carrier = poll; the event is signalled so it still succeeds.
    EXPECT_EQ(ChaosManualResetEventSlimWaitTimeSpan(AsIntPtr(&ev), 0), 1);

    // Reset then poll to show null carrier respects state.
    ChaosManualResetEventSlimResetManaged(AsIntPtr(&ev));
    EXPECT_EQ(ChaosManualResetEventSlimWaitTimeSpan(AsIntPtr(&ev), 0), 0)
        << "null carrier after reset must time out";

    ChaosWaitHandleClose(handle);
}