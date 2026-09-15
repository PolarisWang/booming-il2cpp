// managed_primitive_entries_spin_test.cpp — T2.5: SpinLock + SpinWait
//
// WHAT THIS PINS
// --------------
// SpinLock is a VALUE TYPE, so the receiver is a pointer to the struct's own
// storage — a stack slot owned by the caller.  A.2's receiver injection makes
// codegen pass that pointer as chaos_fn_arg_0, so these entries operate on the
// ACTUAL instance and give real per-instance mutual exclusion.
//
// The load-bearing assertion is the BYREF WRITE-BACK.  The chunk's generated
// test is:
//
//     bool __ref_0_0_0 = default;
//     ...Create<SpinLock>().Enter(ref __ref_0_0_0);
//     Assert.AreEqual(true, __ref_0_0_0);
//
// so `ChaosSpinLockEnter` MUST write 1 through the pointer.  A helper that
// ignored the out-param would pass a naive "did it return" check and fail the
// generated test — that gap is what these tests close.
//
// Shim signatures after A.2 receiver injection:
//     SpinLock::Enter(ref bool)              -> (spinlock, &lockTaken)
//     SpinLock::TryEnter(ref bool)           -> (spinlock, &lockTaken)
//     SpinLock::TryEnter(int, ref bool)      -> (spinlock, timeout, &lockTaken)
//     SpinLock::TryEnter(TimeSpan, ref bool) -> (spinlock, ticks, &lockTaken)
//     SpinLock::Exit()                       -> (spinlock)

#include "runtime_stubs/managed_primitive_entries.h"

#include <gtest/gtest.h>

#include <cstdint>

namespace {

/// A stand-in SpinLock instance: the native entry treats the receiver as a
/// pointer to an INT32 lock word.  Two independent instances let the tests
/// prove per-instance isolation (which the global-word version could not).
struct FakeSpinLock {
    CHAOS_IL2CPP_INT32 word = 0;
    CHAOS_IL2CPP_INT32 pad  = 0;
};

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
    CHAOS_IL2CPP_INT32 lockTaken = 0;   // default(false), exactly as codegen does

    EXPECT_EQ(ChaosSpinLockEnter(AsIntPtr(&lock), RefBool(&lockTaken)), 1);
    EXPECT_EQ(lockTaken, 1)
        << "Enter MUST write true through the byref. The chunk's own generated "
           "test asserts this; a helper that took the bool by value would leave "
           "it false and fail there.";
    EXPECT_NE(lock.word, 0) << "the lock word must record the owner";

    ChaosSpinLockExit(AsIntPtr(&lock));
}

TEST(ManagedPrimitiveEntries_Spin, TryEnterWritesTheByrefOnBothOutcomes)
{
    FakeSpinLock lock;
    CHAOS_IL2CPP_INT32 lockTaken = 0x7F;  // dirty, to catch "never written"

    EXPECT_EQ(ChaosSpinLockTryEnter(AsIntPtr(&lock), RefBool(&lockTaken)), 1);
    EXPECT_EQ(lockTaken, 1) << "a successful TryEnter must report true";

    // While held, a second TryEnter must fail AND overwrite the out-param with
    // false.  Leaving the stale 1 would make the caller believe it holds a lock
    // it does not have.
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

// ── Per-instance isolation (the property A.2 restored) ─────────────────

TEST(ManagedPrimitiveEntries_Spin, TwoInstancesAreIndependent)
{
    FakeSpinLock a, b;
    CHAOS_IL2CPP_INT32 taken = 0;

    ASSERT_EQ(ChaosSpinLockEnter(AsIntPtr(&a), RefBool(&taken)), 1);

    // Holding `a` must NOT block `b`.  With the old process-global lock word
    // this assertion failed — which is exactly why per-instance matters.
    EXPECT_EQ(ChaosSpinLockTryEnter(AsIntPtr(&b), RefBool(&taken)), 1)
        << "two SpinLock instances must be independently acquirable";
    ChaosSpinLockExit(AsIntPtr(&b));

    ChaosSpinLockExit(AsIntPtr(&a));
}

TEST(ManagedPrimitiveEntries_Spin, SecondAcquireFailsWhileHeld)
{
    FakeSpinLock lock;
    CHAOS_IL2CPP_INT32 taken = 0;
    ASSERT_EQ(ChaosSpinLockEnter(AsIntPtr(&lock), RefBool(&taken)), 1);

    CHAOS_IL2CPP_INT32 second = 0x7F;
    EXPECT_EQ(ChaosSpinLockTryEnter(AsIntPtr(&lock), RefBool(&second)), 0)
        << "a held lock must not be acquirable";
    EXPECT_EQ(second, 0);

    EXPECT_EQ(ChaosSpinLockExit(AsIntPtr(&lock)), 1);
    // Released: now acquirable again.
    EXPECT_EQ(ChaosSpinLockTryEnter(AsIntPtr(&lock), RefBool(&second)), 1);
    ChaosSpinLockExit(AsIntPtr(&lock));
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

// ── SpinWait performs the spin ─────────────────────────────────────────

TEST(ManagedPrimitiveEntries_Spin, SpinOnceSpinsThenYields)
{
    // SpinOnce(int) returns the number of iterations actually performed.
    EXPECT_GT(ChaosSpinWaitSpinOnceInt32(5), 0)
        << "SpinOnce(5) must spin";
    EXPECT_GE(ChaosSpinWaitSpinOnceInt32(0), 0)
        << "SpinOnce(0) is a no-op, must not crash";

    EXPECT_GT(ChaosSpinWaitSpinOnce(0), 0)
        << "SpinOnce() must spin and yield";
}

TEST(ManagedPrimitiveEntries_Spin, SpinOnceCapsLargeValues)
{
    constexpr CHAOS_IL2CPP_INTPTR kHuge = 999999;
    EXPECT_LE(ChaosSpinWaitSpinOnceInt32(kHuge), 10000)
        << "a huge iteration count must be capped";
}

TEST(ManagedPrimitiveEntries_Spin, NegativeIterationIsClamped)
{
    EXPECT_EQ(ChaosSpinWaitSpinOnceInt32(-1), 0)
        << "a negative iteration count must not spin";
}
