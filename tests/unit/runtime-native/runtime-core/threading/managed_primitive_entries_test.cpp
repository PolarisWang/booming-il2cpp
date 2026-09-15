// managed_primitive_entries_test.cpp — T2.2 exit criterion.
//
// WHAT THIS PINS
// --------------
// T2.2 wires ReaderWriterLockSlim's managed surface to real native calls.  The
// interesting part is NOT the wiring — it is WHICH entries can be wired at all.
//
// THE RECEIVER RULE (learned from a real chunk build, not assumed):
// for an instance method the generated shim declares ONE slot per MANAGED
// PARAMETER, plus one for the receiver ONLY when the method takes no parameters.
// So `EnterReadLock()` gets a receiver slot, but `TryEnterReadLock(int)` gets
// only the int.  Every parameterised overload therefore has no way to reach its
// instance and is deliberately left unregistered.
//
// Two properties are asserted for the entries that DO get the receiver:
//
//   1. The operation reaches a REAL lock: EnterReadLock is followed by a
//      non-blocking write acquire that must fail.  A helper returning a
//      constant would pass a "did it return 1" check and fail this one.
//
//   2. An UNBOUND instance (what SubjectInstanceFactory.Create<T>() actually
//      produces — GetUninitializedObject runs no ctor, so the handle field is 0)
//      is reported as failure, NOT as success.  This is the whole point of the
//      task: the chunk currently reports `ExitReadLock` passing while the
//      generated body is `return 42L;`.  An entry that answered "fine" for an
//      unbound instance would reproduce that defect natively.
//
// The test drives the real T2.0 handle ABI underneath.

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

    // TryEnter*Lock are deliberately NOT wired (codegen does not pass the
    // receiver for instance methods that take parameters), so there are no
    // native entries to test here — they stay on the fallback path.
    EXPECT_EQ(ChaosReaderWriterLockSlimDisposeManaged(AsIntPtr(&neverConstructed)), 0);

    EXPECT_EQ(neverConstructed.handle, 0) << "nothing may have been bound as a side effect";
}

TEST(ManagedPrimitiveEntries, ANullReceiverIsAnsweredRatherThanDereferenced)
{
    EXPECT_EQ(ChaosReaderWriterLockSlimEnterReadLockInfinite(0), 0);
    EXPECT_EQ(ChaosReaderWriterLockSlimExitWriteLock(0), 0);
    EXPECT_EQ(ChaosReaderWriterLockSlimDisposeManaged(0), 0);
}

// ── 3. Dispose releases the binding ────────────────────────────────────

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

// ── 4. TryEnter* — now wired via receiver injection (Phase A) ────────

TEST(ManagedPrimitiveEntries, TryEnterReadLockReachesTheInstancesOwnLock)
{
    FakeRwls a;
    const CHAOS_IL2CPP_INTPTR rw = Bind(a);

    // Free lock: TryEnterReadLock(int) with timeout 0 must succeed.
    EXPECT_EQ(ChaosReaderWriterLockSlimTryEnterReadLockInt32(AsIntPtr(&a), 0), 1);

    // Now the lock is held in read mode: a non-blocking write acquire must fail.
    EXPECT_EQ(ChaosReaderWriterLockSlimEnterWrite(rw, 0), 0)
        << "TryEnterReadLock must actually enter the lock";

    ChaosReaderWriterLockSlimExitRead(rw);
    ChaosReaderWriterLockSlimDestroy(rw);
}

TEST(ManagedPrimitiveEntries, TryEnterUpgradeableReadLockReachesTheInstancesOwnLock)
{
    FakeRwls a;
    const CHAOS_IL2CPP_INTPTR rw = Bind(a);

    // Poll with 0 ticks.  The lock is free, so this must succeed.
    CHAOS_IL2CPP_INT64 zeroTicks = 0;
    EXPECT_EQ(ChaosReaderWriterLockSlimTryEnterUpgradeableReadLockTimeSpan(
                  AsIntPtr(&a), reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&zeroTicks)), 1)
        << "upgradeable read with 0-ticks must poll and succeed on a free lock";

    // While upgradeable read is held, a regular write must fail.
    EXPECT_EQ(ChaosReaderWriterLockSlimEnterWrite(rw, 0), 0);

    ChaosReaderWriterLockSlimExitUpgradeableRead(rw);
    ChaosReaderWriterLockSlimDestroy(rw);
}

TEST(ManagedPrimitiveEntries, TryEnterWriteLockFailsWhenWriteLockHeld)
{
    FakeRwls a;
    const CHAOS_IL2CPP_INTPTR rw = Bind(a);

    ASSERT_EQ(ChaosReaderWriterLockSlimEnterWriteLockInfinite(AsIntPtr(&a)), 1);
    EXPECT_EQ(ChaosReaderWriterLockSlimTryEnterWriteLockInt32(AsIntPtr(&a), 0), 0)
        << "TryEnterWriteLock must fail when write lock is already held";

    ChaosReaderWriterLockSlimExitWrite(rw);
    ChaosReaderWriterLockSlimDestroy(rw);
}
