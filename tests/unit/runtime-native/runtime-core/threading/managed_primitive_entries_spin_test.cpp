// managed_primitive_entries_spin_test.cpp — T2.5: SpinLock + SpinWait
//
// WHAT THIS PINS
// --------------
// SpinLock and SpinWait are VALUE TYPES: their state is in the struct, not
// behind a handle.  So the receiver pointer IS the lock word, and the failure
// worth guarding against is different from the handle-based types:
//
//   * handle types — the risk was "operated on no object";
//   * value types  — the risk is "operated on a COPY and discarded the result",
//     which is indistinguishable from correct from the caller's side.
//
// The BYREF WRITE-BACK is where that becomes visible, and it is the most
// important assertion here.  The chunk's generated test is:
//
//     bool __ref_0_0_0 = default;
//     ...Create<SpinLock>().Enter(ref __ref_0_0_0);
//     Assert.AreEqual(true, __ref_0_0_0);
//
// A helper that took the bool BY VALUE (rather than as a pointer) would acquire
// the lock and leave the caller's variable false — passing a naive "did it
// return" test and failing this one.
//
// The other properties: mutual exclusion is real (a second acquire fails while
// held), the owner-only Exit rule holds, and SpinWait actually advances its
// counter rather than being a no-op.

#include "runtime_stubs/managed_primitive_entries.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>

namespace {

// SpinLock's storage: the native entry treats the receiver as a pointer to an
// INT32 lock word (owner thread id, 0 = free).  Allocate enough that a stray
// wider access is visible rather than silently in-bounds.
struct FakeSpinLock {
    CHAOS_IL2CPP_INT32 word = 0;
    CHAOS_IL2CPP_INT32 pad  = 0;
};

static_assert(sizeof(FakeSpinLock) >= 4, "the lock word must exist");

CHAOS_IL2CPP_INTPTR AsIntPtr(FakeSpinLock* s) noexcept {
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s);
}

/// The managed `ref bool` carrier: a raw pointer to the caller's bool.
CHAOS_IL2CPP_INTPTR RefBool(CHAOS_IL2CPP_INT32* flag) noexcept {
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(flag);
}

}  // namespace

// ── The load-bearing case: byref write-back ────────────────────────────

TEST(ManagedPrimitiveEntries_Spin, EnterWritesTheByrefTrue)
{
    FakeSpinLock lock;
    // Exactly what the generated test does: initialise to default(false).
    CHAOS_IL2CPP_INT32 lockTaken = 0;

    EXPECT_EQ(ChaosSpinLockEnter(AsIntPtr(&lock), RefBool(&lockTaken)), 1);

    EXPECT_EQ(lockTaken, 1)
        << "Enter MUST write true through the byref. The chunk's own generated "
           "test asserts this; a helper that took the bool by value would leave "
           "it false and fail there.";
}

TEST(ManagedPrimitiveEntries_Spin, TryEnterWritesTheByrefOnBothOutcomes)
{
    FakeSpinLock lock;
    CHAOS_IL2CPP_INT32 lockTaken = 0x7F;  // deliberately dirty to catch "never written"

    // First acquire succeeds -> must write 1.
    EXPECT_EQ(ChaosSpinLockTryEnter(AsIntPtr(&lock), RefBool(&lockTaken)), 1);
    EXPECT_EQ(lockTaken, 1) << "a successful TryEnter must report true";

    // Second acquire on the same (already held) lock must fail AND overwrite the
    // out-param with false.  Leaving the stale 1 would make the caller believe it
    // holds a lock it does not have.
    lockTaken = 0x7F;
    EXPECT_EQ(ChaosSpinLockTryEnter(AsIntPtr(&lock), RefBool(&lockTaken)), 0);
    EXPECT_EQ(lockTaken, 0) << "a failed TryEnter must report false";

    ChaosSpinLockExit(AsIntPtr(&lock));
}

TEST(ManagedPrimitiveEntries_Spin, TryEnterWithTimeoutWritesTheByref)
{
    FakeSpinLock lock;
    CHAOS_IL2CPP_INT32 lockTaken = 0x7F;

    // timeout 0 == poll; the lock is free, so this succeeds.
    EXPECT_EQ(ChaosSpinLockTryEnterInt32(AsIntPtr(&lock), 0, RefBool(&lockTaken)), 1);
    EXPECT_EQ(lockTaken, 1);

    // Now held: a poll must fail and set the out-param to false.
    lockTaken = 0x7F;
    EXPECT_EQ(ChaosSpinLockTryEnterInt32(AsIntPtr(&lock), 0, RefBool(&lockTaken)), 0);
    EXPECT_EQ(lockTaken, 0);

    ChaosSpinLockExit(AsIntPtr(&lock));
}

// ── Mutual exclusion is real ───────────────────────────────────────────

TEST(ManagedPrimitiveEntries_Spin, SecondAcquireFailsWhileHeld)
{
    FakeSpinLock lock;
    CHAOS_IL2CPP_INT32 taken = 0;

    ASSERT_EQ(ChaosSpinLockEnter(AsIntPtr(&lock), RefBool(&taken)), 1);
    EXPECT_NE(lock.word, 0)
        << "the lock word must record the owning thread id, not stay 0 — a "
           "free/held distinction that never changes would make the lock a no-op";

    CHAOS_IL2CPP_INT32 second = 0x7F;
    EXPECT_EQ(ChaosSpinLockTryEnter(AsIntPtr(&lock), RefBool(&second)), 0)
        << "a held lock must not be acquirable";
    EXPECT_EQ(second, 0);

    EXPECT_EQ(ChaosSpinLockExit(AsIntPtr(&lock)), 1);
    EXPECT_EQ(lock.word, 0) << "Exit must release the word";
}

TEST(ManagedPrimitiveEntries_Spin, ExitIsOwnerChecked)
{
    FakeSpinLock lock;
    CHAOS_IL2CPP_INT32 taken = 0;

    // Exit without ever entering must be rejected, not silently clear a word
    // that some other thread set.
    EXPECT_EQ(ChaosSpinLockExit(AsIntPtr(&lock)), 0)
        << "a lock never held must not report a successful release";
    EXPECT_EQ(lock.word, 0);

    ASSERT_EQ(ChaosSpinLockEnter(AsIntPtr(&lock), RefBool(&taken)), 1);
    EXPECT_EQ(ChaosSpinLockExit(AsIntPtr(&lock)), 1);

    // Double Exit: the second has nothing to release.
    EXPECT_EQ(ChaosSpinLockExit(AsIntPtr(&lock)), 0);
}

TEST(ManagedPrimitiveEntries_Spin, NullInputsAreAnswered)
{
    FakeSpinLock lock;
    CHAOS_IL2CPP_INT32 taken = 0x7F;

    // No out-param: nothing to report to, so refuse rather than lock blindly.
    EXPECT_EQ(ChaosSpinLockEnter(AsIntPtr(&lock), 0), 0);
    EXPECT_EQ(lock.word, 0) << "must not have acquired the lock without reporting it";

    // Null lock with a valid out-param: must not dereference.
    EXPECT_EQ(ChaosSpinLockEnter(0, RefBool(&taken)), 0);
    EXPECT_EQ(taken, 0) << "a failed Enter must still leave the out-param false";

    EXPECT_EQ(ChaosSpinLockExit(0), 0);
}

// ── SpinWait advances its state ────────────────────────────────────────

TEST(ManagedPrimitiveEntries_Spin, SpinOnceAdvancesTheCounter)
{
    CHAOS_IL2CPP_INT32 state[2] = {0, 0};
    const CHAOS_IL2CPP_INTPTR p = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(state);

    EXPECT_EQ(ChaosSpinWaitSpinOnce(p), 1);
    EXPECT_EQ(state[0], 1) << "SpinOnce() must count";

    EXPECT_EQ(ChaosSpinWaitSpinOnceInt32(p, 5), 5);
    EXPECT_EQ(state[0], 6)
        << "SpinOnce(int) must advance by that many — a no-op would make the two "
           "overloads indistinguishable and never escalate";
}

TEST(ManagedPrimitiveEntries_Spin, SpinOnceWithZeroOrNegativeIsSafe)
{
    CHAOS_IL2CPP_INT32 state[2] = {0, 0};
    const CHAOS_IL2CPP_INTPTR p = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(state);

    EXPECT_EQ(ChaosSpinWaitSpinOnceInt32(p, 0), 0);
    EXPECT_EQ(state[0], 0);

    // Negative is clamped to 0 rather than treated as a huge unsigned count
    // (which would spin for a very long time).
    EXPECT_EQ(ChaosSpinWaitSpinOnceInt32(p, -1), 0);
    EXPECT_EQ(state[0], 0) << "a negative iteration count must not spin";

    // Null receiver must not crash.
    EXPECT_EQ(ChaosSpinWaitSpinOnceInt32(0, 3), 3);
}
