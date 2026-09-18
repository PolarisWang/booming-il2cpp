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

}  // namespace
