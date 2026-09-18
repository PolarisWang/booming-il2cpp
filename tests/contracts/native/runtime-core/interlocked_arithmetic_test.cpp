// interlocked_arithmetic_test.cpp — regression tests for the Interlocked
// Increment/Decrement arithmetic helpers, at every width the shape registry
// claims.
//
// WHY THIS FILE EXISTS
// --------------------
// The shape registry registered only the Int64 forms of
// `Interlocked.Increment` / `Interlocked.Decrement`.  The Int32, UInt32 and
// UInt64 callees therefore matched no shape at all and lowered to the
// zero-argument external-runtime catch-all, which returns 0.  Against
// generated tests of the form
//
//     int x = default;
//     Assert.AreEqual(1, Interlocked.Increment(ref x));
//
// the 0 is a *wrong answer*, not a crash — it surfaced as a `failed` record in
// the threading chunk's fact results and was invisible to every native test.
//
// The failure mode is a wrong value, not an absent symbol, so a test that only
// checks "it links" proves nothing.  Each case below executes the helper and
// asserts the arithmetic contract.
//
// interlocked_stubs.cpp is compiled directly (not linked from the prebuilt
// lib) because that lib uses GNU ar format, which MSVC cannot reliably resolve
// for extern "C" symbols.

#include <gtest/gtest.h>
#include <chaos/native_types.h>

extern "C" {
#include "runtime_stubs/interlocked_stubs.h"
}

namespace {

// Assert both the returned value and the mutated storage.  The managed
// contract is "return the NEW value", so a helper that stored correctly but
// returned the old value would still be a defect.

TEST(InterlockedArithmetic, IncrementInt32_ReturnsNewValueAndMutates) {
    CHAOS_IL2CPP_INT32 value = 0;
    auto ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value);

    EXPECT_EQ(ChaosInterlockedIncrementInt32(ptr), 1);
    EXPECT_EQ(value, 1);

    EXPECT_EQ(ChaosInterlockedIncrementInt32(ptr), 2);
    EXPECT_EQ(value, 2);
}

TEST(InterlockedArithmetic, DecrementInt32_ReturnsNewValueAndMutates) {
    CHAOS_IL2CPP_INT32 value = 10;
    auto ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value);

    EXPECT_EQ(ChaosInterlockedDecrementInt32(ptr), 9);
    EXPECT_EQ(value, 9);
}

// The generated test `Decrement_2_int_0` starts from `default(int)` = 0 and
// expects -1.  That is the exact case the catch-all got wrong (it returned 0),
// so it is pinned explicitly rather than left to the general case above.
TEST(InterlockedArithmetic, DecrementInt32_FromDefaultIsMinusOne) {
    CHAOS_IL2CPP_INT32 value = 0;
    auto ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value);

    EXPECT_EQ(ChaosInterlockedDecrementInt32(ptr), -1);
    EXPECT_EQ(value, -1);
}

TEST(InterlockedArithmetic, ExchangeInt32_ReturnsPreviousValue) {
    CHAOS_IL2CPP_INT32 value = 10;
    auto ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value);

    EXPECT_EQ(ChaosInterlockedExchangeInt32(ptr, 20), 10);
    EXPECT_EQ(value, 20);
}

TEST(InterlockedArithmetic, IncrementUInt32_ReturnsNewValueAndMutates) {
    CHAOS_IL2CPP_UINT32 value = 0;
    auto ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value);

    EXPECT_EQ(ChaosInterlockedIncrementUInt32(ptr), 1u);
    EXPECT_EQ(value, 1u);
}

// `Decrement_16_uint_0` decrements default(uint) = 0 and expects 0xFFFFFFFF.
// Unsigned wraparound is the contract here, not an edge case to paper over —
// this assertion is what catches a helper written with signed arithmetic.
TEST(InterlockedArithmetic, DecrementUInt32_WrapsToMax) {
    CHAOS_IL2CPP_UINT32 value = 0;
    auto ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value);

    EXPECT_EQ(ChaosInterlockedDecrementUInt32(ptr), 0xFFFFFFFFu);
    EXPECT_EQ(value, 0xFFFFFFFFu);
}

TEST(InterlockedArithmetic, IncrementUInt64_ReturnsNewValueAndMutates) {
    CHAOS_IL2CPP_UINT64 value = 0;
    auto ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value);

    EXPECT_EQ(ChaosInterlockedIncrementUInt64(ptr), 1u);
    EXPECT_EQ(value, 1u);
}

TEST(InterlockedArithmetic, DecrementUInt64_WrapsToMax) {
    CHAOS_IL2CPP_UINT64 value = 0;
    auto ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value);

    EXPECT_EQ(ChaosInterlockedDecrementUInt64(ptr), 0xFFFFFFFFFFFFFFFFull);
    EXPECT_EQ(value, 0xFFFFFFFFFFFFFFFFull);
}

// The signed/unsigned pairs must stay width-matched to their carrier.  A
// helper that truncated UInt64 to 32 bits would still pass the "returns 1"
// check above, so the full-width round trip is asserted separately.
TEST(InterlockedArithmetic, UInt64_CarriesFullWidth) {
    CHAOS_IL2CPP_UINT64 value = 0xFFFFFFFFull;
    auto ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value);

    EXPECT_EQ(ChaosInterlockedIncrementUInt64(ptr), 0x100000000ull);
    EXPECT_EQ(value, 0x100000000ull);
}

// Pre-existing helpers the shape registry already relied on.  Included so this
// file also pins the Int64 contract it must not have disturbed.
TEST(InterlockedArithmetic, IncrementInt64_StillCorrect) {
    CHAOS_IL2CPP_INT64 value = 0;
    auto ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value);

    EXPECT_EQ(ChaosInterlockedIncrementInt64(ptr), 1);
    EXPECT_EQ(value, 1);
}

TEST(InterlockedArithmetic, DecrementInt64_StillCorrect) {
    CHAOS_IL2CPP_INT64 value = 0;
    auto ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value);

    EXPECT_EQ(ChaosInterlockedDecrementInt64(ptr), -1);
    EXPECT_EQ(value, -1);
}

// Distinct storage must not alias: each helper operates on the pointer it was
// handed.  A cached/static target would make two interleaved counters collide,
// which the single-variable tests above cannot detect.
TEST(InterlockedArithmetic, SeparateLocationsAreIndependent) {
    CHAOS_IL2CPP_INT32 a = 0;
    CHAOS_IL2CPP_INT32 b = 0;
    auto pa = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&a);
    auto pb = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&b);

    EXPECT_EQ(ChaosInterlockedIncrementInt32(pa), 1);
    EXPECT_EQ(ChaosInterlockedIncrementInt32(pa), 2);
    EXPECT_EQ(ChaosInterlockedIncrementInt32(pb), 1);

    EXPECT_EQ(a, 2);
    EXPECT_EQ(b, 1);
}

// ── Thread.VolatileRead(ref object) ─────────────────────────────────────────
//
// A separate function from ChaosVolatileRead (INT32) on purpose: this overload
// returns a reference, so it must be pointer-sized.  Feeding it through the
// Int32 helper would truncate a 64-bit reference to its low 32 bits — a corrupt
// pointer rather than a clean failure — so the width is asserted directly.

TEST(ThreadVolatileRead, Object_PreservesFullPointerWidth) {
    // A value with bits set above bit 31, so a 32-bit truncation is visible.
    CHAOS_IL2CPP_INTPTR stored =
        static_cast<CHAOS_IL2CPP_INTPTR>(0x1234567890ABCDEFull);

    EXPECT_EQ(ChaosVolatileReadObject(
                  reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&stored)),
              stored);
}

TEST(ThreadVolatileRead, Object_NullReadsAsZero) {
    CHAOS_IL2CPP_INTPTR stored = 0;

    EXPECT_EQ(ChaosVolatileReadObject(
                  reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&stored)),
              0);
}

// The generated test `VolatileRead_40_object_0` reads from a
// `default(object)` field and expects null.  This is that exact case.
TEST(ThreadVolatileRead, Object_DefaultSlotIsNull) {
    CHAOS_IL2CPP_INTPTR slot = 0;  // a default(object) reference slot

    EXPECT_EQ(ChaosVolatileReadObject(
                  reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot)),
              0);
}

// ── Interlocked.CompareExchange — return semantics ──────────────────────────
//
// The managed contract is `T CompareExchange(ref T location, T value, T comparand)`
// returning the ORIGINAL value of `location`, NOT whether the swap happened.
//
// ChaosInterlockedCompareExchangeInt32 returned `atomic_compare_exchange_strong`'s
// own bool instead, so `CompareExchange(ref 0, 0, 0)` answered 1 where .NET
// answers 0 — a wrong value, and the generated `Assert.AreEqual(0, result)`
// failed.  The Int64 sibling already returned the original, so the two helpers
// disagreed with each other.
//
// Both outcomes are asserted: a success and a failure must BOTH return the
// original, which is what distinguishes this contract from a bool.

TEST(InterlockedCompareExchange, Int32_Success_ReturnsOriginal) {
    CHAOS_IL2CPP_INT32 value = 5;
    auto ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value);

    // comparand matches → swap happens; result is the OLD value (5), not 1.
    EXPECT_EQ(ChaosInterlockedCompareExchangeInt32(ptr, 99, 5), 5);
    EXPECT_EQ(value, 99);  // slot updated
}

TEST(InterlockedCompareExchange, Int32_Mismatch_ReturnsOriginal) {
    CHAOS_IL2CPP_INT32 value = 5;
    auto ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value);

    // comparand differs → no swap; result is still the current value (5), not 0.
    EXPECT_EQ(ChaosInterlockedCompareExchangeInt32(ptr, 99, 7), 5);
    EXPECT_EQ(value, 5);  // slot untouched
}

// The exact shape the generated test drives: `CompareExchange(ref 0, 0, 0)`.
// A bool-returning helper answers 1 here; the correct answer is 0.
TEST(InterlockedCompareExchange, Int32_ZeroComparand_ReturnsZero) {
    CHAOS_IL2CPP_INT32 value = 0;
    auto ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value);

    EXPECT_EQ(ChaosInterlockedCompareExchangeInt32(ptr, 0, 0), 0);
    EXPECT_EQ(value, 0);
}

// Pin the Int64 sibling too, so a future edit cannot make the two disagree
// again in the other direction.
TEST(InterlockedCompareExchange, Int64_ReturnsOriginalBothWays) {
    CHAOS_IL2CPP_INT64 hit = 5;
    auto ph = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&hit);
    EXPECT_EQ(ChaosInterlockedCompareExchangeInt64(ph, 99, 5), 5);
    EXPECT_EQ(hit, 99);

    CHAOS_IL2CPP_INT64 miss = 5;
    auto pm = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&miss);
    EXPECT_EQ(ChaosInterlockedCompareExchangeInt64(pm, 99, 7), 5);
    EXPECT_EQ(miss, 5);
}

// ── LazyInitializer.EnsureInitialized ───────────────────────────────────────
//
// The generated test drives:
//
//     int x = default; bool init = default; object sync = default;
//     var r = LazyInitializer.EnsureInitialized(ref x, ref init, ref sync);
//     Assert.AreEqual(0, r);  Assert.AreEqual(0, x);  Assert.AreEqual(true, init);
//
// Verified against .NET 8: on first call the target is set to default(T),
// `initialized` becomes true, `syncLock` becomes a non-null object, and the
// stored value is returned.  On a later call with `initialized` already true,
// nothing is touched.

// Local re-declarations — this file deliberately does not pull in the heavy
// threading_stubs.h include chain.
extern "C" {
CHAOS_IL2CPP_INTPTR chaos_lazy_initializer_ensure_initialized(
    CHAOS_IL2CPP_INTPTR target_ref, CHAOS_IL2CPP_INTPTR initialized_ref,
    CHAOS_IL2CPP_INTPTR sync_lock_ref, CHAOS_IL2CPP_INT32 carrier_width) noexcept;
}

TEST(LazyInitializerEnsureInitialized, ThreeArg_InitializesAndReturnsValue) {
    CHAOS_IL2CPP_INT32 target = 0;
    CHAOS_IL2CPP_UINT8 initialized = 0;
    CHAOS_IL2CPP_INTPTR sync_lock = 0;

    auto r = chaos_lazy_initializer_ensure_initialized(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&target),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&initialized),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&sync_lock),
        4);

    EXPECT_EQ(r, 0);            // default(int)
    EXPECT_EQ(target, 0);
    EXPECT_EQ(initialized, 1);  // flipped true
    EXPECT_NE(sync_lock, 0);    // sentinel assigned
}

// The second call must not re-run anything: the caller's `initialized` flag is
// the contract's whole point.  A pre-set flag with a modified target proves the
// helper reads the flag rather than the target.
TEST(LazyInitializerEnsureInitialized, AlreadyInitialized_LeavesTargetAlone) {
    CHAOS_IL2CPP_INT32 target = 77;
    CHAOS_IL2CPP_UINT8 initialized = 1;
    CHAOS_IL2CPP_INTPTR sync_lock = 0;

    auto r = chaos_lazy_initializer_ensure_initialized(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&target),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&initialized),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&sync_lock),
        4);

    EXPECT_EQ(r, 77);           // returns what was already there
    EXPECT_EQ(target, 77);      // NOT overwritten with default
    EXPECT_EQ(initialized, 1);
    EXPECT_EQ(sync_lock, 0);    // untouched
}

// Width matters: a 1-byte T must not be written as a pointer-sized slot, which
// would clobber the neighbouring `initialized` byte in the caller's frame.
TEST(LazyInitializerEnsureInitialized, ByteWidth_DoesNotClobberNeighbour) {
    // Lay the storage out exactly like the generated caller does.
    CHAOS_IL2CPP_UINT8 target = 0xAB;
    CHAOS_IL2CPP_UINT8 initialized = 0;
    CHAOS_IL2CPP_INTPTR sync_lock = 0;

    auto r = chaos_lazy_initializer_ensure_initialized(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&target),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&initialized),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&sync_lock),
        1);

    EXPECT_EQ(r, 0);
    EXPECT_EQ(target, 0);       // default(byte)
    EXPECT_EQ(initialized, 1);  // survived — a width-8 write would have smashed it
}

// A null target has no storage to initialise; the helper must not dereference it.
TEST(LazyInitializerEnsureInitialized, NullTarget_DoesNotCrash) {
    CHAOS_IL2CPP_UINT8 initialized = 0;
    CHAOS_IL2CPP_INTPTR sync_lock = 0;

    auto r = chaos_lazy_initializer_ensure_initialized(
        0,
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&initialized),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&sync_lock),
        4);

    EXPECT_EQ(r, 0);
    EXPECT_EQ(initialized, 0);  // not initialised, because nothing was written
}

}  // namespace
