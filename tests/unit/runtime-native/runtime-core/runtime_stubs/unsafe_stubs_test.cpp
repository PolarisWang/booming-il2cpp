// unsafe_stubs_test.cpp — Unit tests for Unsafe pointer-intrinsic execute
// bodies (P0-B A2 cheap-class: System.Runtime.CompilerServices.Unsafe).
//
// Verifies the correct machine-pointer semantics that back the native execute
// bodies: reinterpretation identity and byte-strided address arithmetic.
// Links unsafe_stubs.cpp directly (self-contained, no GC dependency).

#include <gtest/gtest.h>

#include <chaos/native_types.h>

// ═══════════════════════════════════════════════════════════════════════════
// Forward declarations for the stub functions
// ═══════════════════════════════════════════════════════════════════════════

extern "C" {
CHAOS_IL2CPP_INTPTR ChaosUnsafeAsRef(CHAOS_IL2CPP_INTPTR source) noexcept;
CHAOS_IL2CPP_INTPTR ChaosUnsafeAsFromPointer(CHAOS_IL2CPP_INTPTR ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosUnsafeAddByteOffset(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR byteOffset) noexcept;
CHAOS_IL2CPP_INTPTR ChaosUnsafeSubtractByteOffset(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR byteOffset) noexcept;
CHAOS_IL2CPP_INTPTR ChaosUnsafeAddElementOffset(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR elementSize, CHAOS_IL2CPP_INT32 index) noexcept;
CHAOS_IL2CPP_INTPTR ChaosUnsafeSubtractElementOffset(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR elementSize, CHAOS_IL2CPP_INT32 index) noexcept;
CHAOS_IL2CPP_INTPTR ChaosUnsafeGetObjectValue(CHAOS_IL2CPP_INTPTR valueObj) noexcept;
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosUnsafeAsRef — identity reinterpretation
// ═══════════════════════════════════════════════════════════════════════════

TEST(Unsafe_AsRef, ReinterpretsAddressUnchanged) {
    int value = 42;
    auto addr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value);
    EXPECT_EQ(ChaosUnsafeAsRef(addr), addr);
}

TEST(Unsafe_AsRef, NullStaysNull) {
    EXPECT_EQ(ChaosUnsafeAsRef(0), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosUnsafeAsFromPointer — void* wrapped as reference
// ═══════════════════════════════════════════════════════════════════════════

TEST(Unsafe_AsFromPointer, WrapsRawAddress) {
    double value = 3.25;
    auto addr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value);
    EXPECT_EQ(ChaosUnsafeAsFromPointer(addr), addr);
}

TEST(Unsafe_AsFromPointer, NullStaysNull) {
    EXPECT_EQ(ChaosUnsafeAsFromPointer(0), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosUnsafeAddByteOffset / SubtractByteOffset — byte distance arithmetic
// ═══════════════════════════════════════════════════════════════════════════

TEST(Unsafe_AddByteOffset, AdvancesByByteDistance) {
    CHAOS_IL2CPP_UINT8 buf[64] = {};
    auto base = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&buf[0]);
    EXPECT_EQ(ChaosUnsafeAddByteOffset(base, 16),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&buf[16]));
}

TEST(Unsafe_AddByteOffset, ZeroOffsetIsIdentity) {
    int value = 0;
    auto addr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value);
    EXPECT_EQ(ChaosUnsafeAddByteOffset(addr, 0), addr);
}

TEST(Unsafe_SubtractByteOffset, RetreatsByByteDistance) {
    CHAOS_IL2CPP_UINT8 buf[64] = {};
    auto base = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&buf[16]);
    EXPECT_EQ(ChaosUnsafeSubtractByteOffset(base, 16),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&buf[0]));
}

TEST(Unsafe_SubtractByteOffset, ZeroOffsetIsIdentity) {
    int value = 0;
    auto addr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value);
    EXPECT_EQ(ChaosUnsafeSubtractByteOffset(addr, 0), addr);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosUnsafeAddElementOffset / SubtractElementOffset — strided element add
// ═══════════════════════════════════════════════════════════════════════════

TEST(Unsafe_AddElementOffset, AdvancesByElementStride) {
    // elementSize = sizeof(CHAOS_IL2CPP_INT64) = 8, index 3 -> +24 bytes
    CHAOS_IL2CPP_UINT8 buf[64] = {};
    auto base = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&buf[0]);
    const CHAOS_IL2CPP_INTPTR stride = static_cast<CHAOS_IL2CPP_INTPTR>(8);
    EXPECT_EQ(ChaosUnsafeAddElementOffset(base, stride, 3),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&buf[24]));
}

TEST(Unsafe_AddElementOffset, ZeroIndexIsIdentity) {
    int value = 0;
    auto addr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value);
    EXPECT_EQ(ChaosUnsafeAddElementOffset(addr, 4, 0), addr);
}

TEST(Unsafe_AddElementOffset, NegativeIndexRetreats) {
    // index = -2, elementSize = 8 -> retreat 16 bytes (must not wrap to a
    // huge unsigned add).
    CHAOS_IL2CPP_UINT8 buf[64] = {};
    auto base = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&buf[16]);
    const CHAOS_IL2CPP_INTPTR stride = static_cast<CHAOS_IL2CPP_INTPTR>(8);
    EXPECT_EQ(ChaosUnsafeAddElementOffset(base, stride, -2),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&buf[0]));
}

TEST(Unsafe_SubtractElementOffset, RetreatsByElementStride) {
    // elementSize = sizeof(CHAOS_IL2CPP_INT32) = 4, index 2 -> -8 bytes
    CHAOS_IL2CPP_UINT8 buf[64] = {};
    auto base = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&buf[8]);
    const CHAOS_IL2CPP_INTPTR stride = static_cast<CHAOS_IL2CPP_INTPTR>(4);
    EXPECT_EQ(ChaosUnsafeSubtractElementOffset(base, stride, 2),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&buf[0]));
}

TEST(Unsafe_SubtractElementOffset, ZeroIndexIsIdentity) {
    int value = 0;
    auto addr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value);
    EXPECT_EQ(ChaosUnsafeSubtractElementOffset(addr, 4, 0), addr);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosUnsafeGetObjectValue — stored reference identity
// ═══════════════════════════════════════════════════════════════════════════

TEST(Unsafe_GetObjectValue, ReturnsStoredReference) {
    int value = 7;
    auto addr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value);
    EXPECT_EQ(ChaosUnsafeGetObjectValue(addr), addr);
}

TEST(Unsafe_GetObjectValue, NullStaysNull) {
    EXPECT_EQ(ChaosUnsafeGetObjectValue(0), 0);
}
