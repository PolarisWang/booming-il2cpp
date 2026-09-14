// managed_primitive_entries_test.cpp — T2.2 exit criterion.
//
// WHAT THIS PINS
// --------------
// T2.2 wires ReaderWriterLockSlim's managed surface to real native calls.  The
// interesting part is NOT the wiring — it is that the receiver is not passed by
// codegen, so each entry recovers the native handle from the managed instance
// itself (T2.1's field).  Three properties follow, and each is asserted here:
//
//   1. The operation reaches the SAME native lock the instance is bound to.
//      A helper that ignored the receiver and used a global would pass a
//      "does it return 1" test and be completely wrong — so the test binds two
//      locks and shows one does not observe the other.
//
//   2. An UNBOUND instance (what SubjectInstanceFactory.Create<T>() actually
//      produces — GetUninitializedObject runs no ctor, so the handle field is 0)
//      is reported as failure, NOT as success.  This is the whole point of the
//      task: the chunk currently reports `ExitReadLock` passing while the
//      generated body is `return 42L;`.  An entry that answered "fine" for an
//      unbound instance would reproduce that defect natively.
//
//   3. Mutual exclusion is real.  A reader cannot be blocked out by another
//      reader, and a writer cannot enter while a reader holds.  Asserting on the
//      RESULT of EnterWriteLock while a read lock is held is what makes this a
//      behavior test rather than a "did the call return" test.
//
// The test drives the real T2.0 handle ABI underneath, so "bound to the same
// lock" is checked by actually contending, not by comparing ids.

#include "runtime_stubs/managed_primitive_entries.h"
#include "runtime_stubs/managed_handle_stubs.h"
#include "runtime_stubs/synchronization_stubs.h"

#include <gtest/gtest.h>

#include <cstdint>

namespace {

// Stand-in managed object: 8-byte header then the handle field, matching the
// layout contract managed_handle_stubs.cpp documents.
struct FakeRwls {
    CHAOS_IL2CPP_INTPTR header[1] = {0xDEADBEEF};
    CHAOS_IL2CPP_INTPTR handle = 0;
};

static_assert(sizeof(FakeRwls) == 16, "header + one pointer");

CHAOS_IL2CPP_INTPTR AsIntPtr(FakeRwls* inst) noexcept {
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(inst);
}

/// Bind a fresh native RW lock to `inst`, the way a real ctor would.
CHAOS_IL2CPP_INTPTR Bind(FakeRwls& inst) noexcept {
    const CHAOS_IL2CPP_INTPTR rw = ChaosReaderWriterLockSlimCreate();
    EXPECT_NE(rw, 0);
    ChaosManagedHandleStore(AsIntPtr(&inst), rw);
    return rw;
}

}  // namespace

// ── 1. The receiver is what selects the lock ───────────────────────────

TEST(ManagedPrimitiveEntries, EnterReadLockReachesTheInstancesOwnLock)
{
    FakeRwls a;
    const CHAOS_IL2CPP_INTPTR rw = Bind(a);

    EXPECT_EQ(ChaosReaderWriterLockSlimEnterReadLockInfinite(AsIntPtr(&a)), 1);

    // Now the lock must be held in read mode: an exclusive acquire from the
    // same native lock cannot succeed immediately.  This is the assertion that
    // distinguishes "entered a real lock" from "returned 1".
    EXPECT_EQ(ChaosReaderWriterLockSlimEnterWrite(rw, 0), 0)
        << "a read lock is held, so a non-blocking write acquire must fail";

    ChaosReaderWriterLockSlimExitRead(rw);
    ChaosReaderWriterLockSlimDestroy(rw);
}

TEST(ManagedPrimitiveEntries, TwoInstancesDoNotShareOneLock)
{
    FakeRwls a;
    FakeRwls b;
    const CHAOS_IL2CPP_INTPTR rwA = Bind(a);
    const CHAOS_IL2CPP_INTPTR rwB = Bind(b);
    ASSERT_NE(rwA, rwB);

    EXPECT_EQ(ChaosReaderWriterLockSlimEnterWriteLockInfinite(AsIntPtr(&a)), 1);

    // `b` is a DIFFERENT lock, so it must acquire cleanly while `a` holds its
    // writer.  An implementation that used one global lock (or read the wrong
    // instance) would fail here — which a single-instance test cannot detect.
    EXPECT_EQ(ChaosReaderWriterLockSlimEnterWriteLockInfinite(AsIntPtr(&b)), 1)
        << "two managed instances must map to two independent native locks";

    ChaosReaderWriterLockSlimExitWriteLock(AsIntPtr(&b));
    ChaosReaderWriterLockSlimExitWriteLock(AsIntPtr(&a));
    ChaosReaderWriterLockSlimDestroy(rwA);
    ChaosReaderWriterLockSlimDestroy(rwB);
}

// ── 2. Unbound instances must fail loudly, not succeed ─────────────────

TEST(ManagedPrimitiveEntries, AnUnboundInstanceIsNotASuccessfulEnter)
{
    // This is exactly what SubjectInstanceFactory.Create<ReaderWriterLockSlim>()
    // hands the generated test: a zero-initialised object whose ctor never ran.
    FakeRwls neverConstructed;

    EXPECT_EQ(ChaosReaderWriterLockSlimEnterReadLockInfinite(AsIntPtr(&neverConstructed)), 0)
        << "an instance with no native lock must not report a successful enter";
    EXPECT_EQ(ChaosReaderWriterLockSlimEnterWriteLockInfinite(AsIntPtr(&neverConstructed)), 0);
    EXPECT_EQ(ChaosReaderWriterLockSlimEnterUpgradeableReadLockInfinite(AsIntPtr(&neverConstructed)), 0);

    EXPECT_EQ(ChaosReaderWriterLockSlimExitReadLock(AsIntPtr(&neverConstructed)), 0);
    EXPECT_EQ(ChaosReaderWriterLockSlimExitWriteLock(AsIntPtr(&neverConstructed)), 0);
    EXPECT_EQ(ChaosReaderWriterLockSlimExitUpgradeableReadLock(AsIntPtr(&neverConstructed)), 0);

    EXPECT_EQ(ChaosReaderWriterLockSlimTryEnterReadLockInt32(AsIntPtr(&neverConstructed), 0), 0);
    EXPECT_EQ(ChaosReaderWriterLockSlimDisposeManaged(AsIntPtr(&neverConstructed)), 0);

    EXPECT_EQ(neverConstructed.handle, 0) << "nothing may have been bound as a side effect";
}

TEST(ManagedPrimitiveEntries, ANullReceiverIsAnsweredRatherThanDereferenced)
{
    EXPECT_EQ(ChaosReaderWriterLockSlimEnterReadLockInfinite(0), 0);
    EXPECT_EQ(ChaosReaderWriterLockSlimExitWriteLock(0), 0);
    EXPECT_EQ(ChaosReaderWriterLockSlimTryEnterWriteLockInt32(0, 0), 0);
    EXPECT_EQ(ChaosReaderWriterLockSlimDisposeManaged(0), 0);
}

// ── 3. Try-enter really reflects lock state ────────────────────────────

TEST(ManagedPrimitiveEntries, TryEnterWriteFailsWhileAReadLockIsHeld)
{
    FakeRwls a;
    const CHAOS_IL2CPP_INTPTR rw = Bind(a);

    // A writer timeout of 0 == "do not block".  With the lock free this must
    // succeed, so a helper that always returned 0 cannot pass this pair.
    EXPECT_EQ(ChaosReaderWriterLockSlimTryEnterWriteLockInt32(AsIntPtr(&a), 0), 1);
    ChaosReaderWriterLockSlimExitWriteLock(AsIntPtr(&a));

    ASSERT_EQ(ChaosReaderWriterLockSlimEnterReadLockInfinite(AsIntPtr(&a)), 1);
    EXPECT_EQ(ChaosReaderWriterLockSlimTryEnterWriteLockInt32(AsIntPtr(&a), 0), 0)
        << "an exclusive acquire must not succeed while a reader holds";
    ChaosReaderWriterLockSlimExitRead(rw);

    ChaosReaderWriterLockSlimDestroy(rw);
}

TEST(ManagedPrimitiveEntries, ReadersDoNotBlockEachOther)
{
    FakeRwls a;
    const CHAOS_IL2CPP_INTPTR rw = Bind(a);

    EXPECT_EQ(ChaosReaderWriterLockSlimEnterReadLockInfinite(AsIntPtr(&a)), 1);
    // A second read acquire on a free-in-read lock must succeed.  Modelling the
    // lock as a plain mutex would make this deadlock/fail — the distinction the
    // upgradeable-read family exists for.
    EXPECT_EQ(ChaosReaderWriterLockSlimTryEnterReadLockInt32(AsIntPtr(&a), 0), 1)
        << "shared mode must admit a second reader";

    ChaosReaderWriterLockSlimExitRead(rw);
    ChaosReaderWriterLockSlimExitRead(rw);
    ChaosReaderWriterLockSlimDestroy(rw);
}

// ── TimeSpan encoding ──────────────────────────────────────────────────

TEST(ManagedPrimitiveEntries, TimeSpanTicksAreConvertedNotReinterpreted)
{
    FakeRwls a;
    const CHAOS_IL2CPP_INTPTR rw = Bind(a);

    // 1 second == 10,000,000 ticks.  Passed as an INTPTR to the tick carrier.
    // On a free lock this must acquire (a rejected/unparsed timeout would not).
    CHAOS_IL2CPP_INT64 oneSecondTicks = 10000000;
    EXPECT_EQ(ChaosReaderWriterLockSlimTryEnterReadLockTimeSpan(
                  AsIntPtr(&a), reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&oneSecondTicks)),
              1);
    ChaosReaderWriterLockSlimExitRead(rw);

    // Negative ticks == infinite.  Must acquire a free lock, and must NOT be
    // treated as 0 (poll) — both happen to succeed here, so the discriminating
    // case is a lock that is briefly contended; that needs threads and belongs
    // with the contention tests, not a unit test.  What is pinned here is that
    // the negative path does not crash or flip to "denied".
    CHAOS_IL2CPP_INT64 infiniteTicks = -1;
    EXPECT_EQ(ChaosReaderWriterLockSlimTryEnterWriteLockTimeSpan(
                  AsIntPtr(&a), reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&infiniteTicks)),
              1);
    ChaosReaderWriterLockSlimExitWriteLock(AsIntPtr(&a));

    // A null carrier must be answered, not dereferenced.  It maps to timeout 0
    // (poll), and the lock is FREE here — so this legitimately acquires.
    EXPECT_EQ(ChaosReaderWriterLockSlimTryEnterReadLockTimeSpan(AsIntPtr(&a), 0), 1)
        << "a null TimeSpan carrier polls, and a free lock is acquirable";
    ChaosReaderWriterLockSlimExitRead(AsIntPtr(&a));

    ChaosReaderWriterLockSlimDestroy(rw);
}

// ── 4. Dispose releases the binding ────────────────────────────────────

TEST(ManagedPrimitiveEntries, DisposeReleasesTheBindingAndIsIdempotent)
{
    FakeRwls a;
    const CHAOS_IL2CPP_INTPTR rw = Bind(a);

    EXPECT_EQ(ChaosReaderWriterLockSlimDisposeManaged(AsIntPtr(&a)), 1);
    EXPECT_EQ(a.handle, 0) << "the field must be cleared";

    // Managed Dispose() and a finalizer can both run.
    EXPECT_EQ(ChaosReaderWriterLockSlimDisposeManaged(AsIntPtr(&a)), 0)
        << "a second Dispose has nothing to release";

    // And the native lock really is gone.  The T2.0 handle ABI answers an
    // unknown handle with -1 (see synchronization.cpp: FindRWLockFixed returns
    // nullptr -> -1), which is how "destroyed" is distinguished from "acquired"
    // (1) and "timed out" (0) — so this asserts the specific value.
    EXPECT_EQ(ChaosReaderWriterLockSlimEnterRead(rw, 0), -1)
        << "the destroyed lock's id must no longer resolve";
}
