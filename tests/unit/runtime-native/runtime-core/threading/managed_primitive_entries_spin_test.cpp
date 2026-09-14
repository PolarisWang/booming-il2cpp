// managed_primitive_entries_spin_test.cpp — T2.5: SpinLock + SpinWait
//
// WHAT THIS PINS
// --------------
// SpinLock and SpinWait are VALUE TYPES and codegen does NOT forward the
// receiver through SimpleForward dispatch, so the native entries receive only
// the managed parameter slots.  The registered ABI therefore uses a single
// GLOBAL lock word rather than a per-instance one.
//
// The load-bearing assertion is the BYREF WRITE-BACK.  The chunk's generated
// test is:
//
//     bool __ref_0_0_0 = default;
//     ...Create<SpinLock>().Enter(ref __ref_0_0_0);
//     Assert.AreEqual(true, __ref_0_0_0);
//
// so `ChaosSpinLockEnter` MUST write 1 through the pointer.  A helper that
// ignored the out-param and only returned 1 would pass a naive "did it return"
// check and fail the generated test — that gap is what these tests close.
//
// The generated shim signatures (from the chunk's native-aot.generated.cpp)
// are the contract the arity here must match:
//     SpinLock::Enter(ref bool)         -> ChaosSpinLockEnter(arg0)
//     SpinLock::TryEnter(ref bool)      -> ChaosSpinLockTryEnter(arg0)
//     SpinLock::TryEnter(int, ref bool) -> ChaosSpinLockTryEnterInt32(arg0, arg1)
//     SpinLock::TryEnter(TS, ref bool)  -> ChaosSpinLockTryEnterTimeSpan(arg0, arg1)
//     SpinLock::Exit()                  -> ChaosSpinLockExit()

#include "runtime_stubs/managed_primitive_entries.h"

#include <gtest/gtest.h>

#include <cstdint>

namespace {

/// The managed `ref bool` carrier: a raw pointer to the caller's bool.
CHAOS_IL2CPP_INTPTR RefBool(CHAOS_IL2CPP_INT32* flag) noexcept {
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(flag);
}

}  // namespace

// ── The load-bearing case: byref write-back ────────────────────────────

TEST(ManagedPrimitiveEntries_Spin, EnterWritesTheByrefTrue)
{
    CHAOS_IL2CPP_INT32 lockTaken = 0;   // default(false), exactly as codegen does

    EXPECT_EQ(ChaosSpinLockEnter(RefBool(&lockTaken)), 1);
    EXPECT_EQ(lockTaken, 1)
        << "Enter MUST write true through the byref. The chunk's own generated "
           "test asserts this; a helper that took the bool by value would leave "
           "it false and fail there.";

    ChaosSpinLockExit();
}

TEST(ManagedPrimitiveEntries_Spin, TryEnterWritesTheByrefOnBothOutcomes)
{
    CHAOS_IL2CPP_INT32 lockTaken = 0x7F;  // dirty, to catch "never written"

    EXPECT_EQ(ChaosSpinLockTryEnter(RefBool(&lockTaken)), 1);
    EXPECT_EQ(lockTaken, 1) << "a successful TryEnter must report true";

    // While held, a second TryEnter must fail AND overwrite the out-param with
    // false.  Leaving the stale 1 would make the caller believe it holds a lock
    // it does not have.
    lockTaken = 0x7F;
    EXPECT_EQ(ChaosSpinLockTryEnter(RefBool(&lockTaken)), 0);
    EXPECT_EQ(lockTaken, 0) << "a failed TryEnter must report false";

    ChaosSpinLockExit();
}

TEST(ManagedPrimitiveEntries_Spin, TryEnterWithTimeoutWritesTheByref)
{
    CHAOS_IL2CPP_INT32 lockTaken = 0x7F;

    // timeout 0 == poll; the lock is free, so this succeeds.
    EXPECT_EQ(ChaosSpinLockTryEnterInt32(0, RefBool(&lockTaken)), 1);
    EXPECT_EQ(lockTaken, 1);

    // Now held: a poll must fail and set the out-param to false.
    lockTaken = 0x7F;
    EXPECT_EQ(ChaosSpinLockTryEnterInt32(0, RefBool(&lockTaken)), 0);
    EXPECT_EQ(lockTaken, 0);

    ChaosSpinLockExit();
}

// ── The lock really is held/released ───────────────────────────────────

TEST(ManagedPrimitiveEntries_Spin, SecondAcquireFailsWhileHeld)
{
    CHAOS_IL2CPP_INT32 taken = 0;
    ASSERT_EQ(ChaosSpinLockEnter(RefBool(&taken)), 1);

    CHAOS_IL2CPP_INT32 second = 0x7F;
    EXPECT_EQ(ChaosSpinLockTryEnter(RefBool(&second)), 0)
        << "a held lock must not be acquirable";
    EXPECT_EQ(second, 0);

    EXPECT_EQ(ChaosSpinLockExit(), 1);
    // Released: now acquirable again.
    EXPECT_EQ(ChaosSpinLockTryEnter(RefBool(&second)), 1);
    ChaosSpinLockExit();
}

TEST(ManagedPrimitiveEntries_Spin, ExitReportsWhetherItReleased)
{
    // Exit with nothing held must report 0, not a spurious success.
    EXPECT_EQ(ChaosSpinLockExit(), 0)
        << "a lock never held must not report a successful release";

    CHAOS_IL2CPP_INT32 taken = 0;
    ASSERT_EQ(ChaosSpinLockEnter(RefBool(&taken)), 1);
    EXPECT_EQ(ChaosSpinLockExit(), 1);

    // Double Exit: the second has nothing to release.
    EXPECT_EQ(ChaosSpinLockExit(), 0);
}

TEST(ManagedPrimitiveEntries_Spin, NullOutParamIsAnswered)
{
    // No out-param: nothing to report to, so refuse rather than lock blindly.
    EXPECT_EQ(ChaosSpinLockEnter(0), 0);
    EXPECT_EQ(ChaosSpinLockTryEnter(0), 0);
    EXPECT_EQ(ChaosSpinLockTryEnterInt32(0, 0), 0);
    EXPECT_EQ(ChaosSpinLockTryEnterTimeSpan(0, 0), 0);

    // And the refusal must not have left the lock held.
    CHAOS_IL2CPP_INT32 taken = 0x7F;
    EXPECT_EQ(ChaosSpinLockTryEnter(RefBool(&taken)), 1)
        << "a refused Enter must not have acquired the lock";
    ChaosSpinLockExit();
}

// ── SpinWait performs the spin ─────────────────────────────────────────

TEST(ManagedPrimitiveEntries_Spin, SpinOnceSpinsThenYields)
{
    // No struct state to advance — the generated shim passes no receiver.
    // SpinOnce(int) returns the number of iterations actually performed.
    EXPECT_GT(ChaosSpinWaitSpinOnceInt32(5), 0)
        << "SpinOnce(5) must spin";
    EXPECT_GE(ChaosSpinWaitSpinOnceInt32(0), 0)
        << "SpinOnce(0) is a no-op, must not crash";

    // SpinOnce() with the undifferentiated INTPTR slot.
    EXPECT_GT(ChaosSpinWaitSpinOnce(0), 0)
        << "SpinOnce() must spin and yield";
}

TEST(ManagedPrimitiveEntries_Spin, SpinOnceCapsLargeValues)
{
    // Huge iteration counts must be clamped rather than spinning forever.
    constexpr CHAOS_IL2CPP_INTPTR kHuge = 999999;
    EXPECT_LE(ChaosSpinWaitSpinOnceInt32(kHuge), 10000)
        << "a huge iteration count must be capped";
}

TEST(ManagedPrimitiveEntries_Spin, NegativeIterationIsClamped)
{
    EXPECT_EQ(ChaosSpinWaitSpinOnceInt32(-1), 0)
        << "a negative iteration count must not spin";
}
