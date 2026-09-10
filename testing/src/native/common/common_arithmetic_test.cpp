/// common_arithmetic_test — Unit tests for arithmetic.h wrapping/shift operations.

#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/arithmetic.h>
#include <limits>

using namespace chaos::il2cpp::common;

// ═══════════════════════════════════════════════════════════════════════════
// wrap_add
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonArithmetic, WrapAddNormal) {
    EXPECT_EQ(5, wrap_add(2, 3));
    EXPECT_EQ(0, wrap_add(-1, 1));
    EXPECT_EQ(-5, wrap_add(-2, -3));
}

TEST(CommonArithmetic, WrapAddOverflow) {
    // INT32_MAX + 1 wraps to INT32_MIN
    EXPECT_EQ(std::numeric_limits<CHAOS_IL2CPP_INT32>::min(),
              wrap_add(std::numeric_limits<CHAOS_IL2CPP_INT32>::max(), 1));
    // INT32_MIN + (-1) wraps to INT32_MAX
    EXPECT_EQ(std::numeric_limits<CHAOS_IL2CPP_INT32>::max(),
              wrap_add(std::numeric_limits<CHAOS_IL2CPP_INT32>::min(), -1));
}

// ═══════════════════════════════════════════════════════════════════════════
// wrap_sub
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonArithmetic, WrapSubNormal) {
    EXPECT_EQ(3, wrap_sub(5, 2));
    EXPECT_EQ(-3, wrap_sub(2, 5));
    EXPECT_EQ(0, wrap_sub(1, 1));
}

TEST(CommonArithmetic, WrapSubOverflow) {
    // INT32_MIN - 1 wraps to INT32_MAX
    EXPECT_EQ(std::numeric_limits<CHAOS_IL2CPP_INT32>::max(),
              wrap_sub(std::numeric_limits<CHAOS_IL2CPP_INT32>::min(), 1));
    // INT32_MAX - (-1) wraps to INT32_MIN
    EXPECT_EQ(std::numeric_limits<CHAOS_IL2CPP_INT32>::min(),
              wrap_sub(std::numeric_limits<CHAOS_IL2CPP_INT32>::max(), -1));
}

// ═══════════════════════════════════════════════════════════════════════════
// wrap_mul
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonArithmetic, WrapMulNormal) {
    EXPECT_EQ(6, wrap_mul(2, 3));
    EXPECT_EQ(-6, wrap_mul(2, -3));
    EXPECT_EQ(0, wrap_mul(0, 42));
}

TEST(CommonArithmetic, WrapMulOverflow) {
    // Large values that overflow when multiplied
    CHAOS_IL2CPP_INT32 a = 100000;
    CHAOS_IL2CPP_INT32 b = 100000;
    // Expected = (100000 * 100000) mod 2^32 reinterpreted as int32
    // 100000 * 100000 = 10000000000 = 0x02540BE400
    // Truncated to 32-bit: 0x540BE400 = 1410065408
    EXPECT_EQ(1410065408, wrap_mul(a, b));
}

// ═══════════════════════════════════════════════════════════════════════════
// div — checked division (CHAOS_IL2CPP_FAIL on div-by-zero or INT32_MIN/-1)
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonArithmetic, DivNormal) {
    EXPECT_EQ(2, chaos::il2cpp::common::div(6, 3));
    EXPECT_EQ(-2, chaos::il2cpp::common::div(6, -3));
    EXPECT_EQ(0, chaos::il2cpp::common::div(1, 2));
    EXPECT_EQ(1, chaos::il2cpp::common::div(3, 2));
}

TEST(CommonArithmetic, DivByZero) {
    EXPECT_DEATH(chaos::il2cpp::common::div(1, 0), "");
}

TEST(CommonArithmetic, DivInt32MinByNegOne) {
    // INT32_MIN / -1 triggers overflow guard → CHAOS_IL2CPP_FAIL
    EXPECT_DEATH(chaos::il2cpp::common::div(std::numeric_limits<CHAOS_IL2CPP_INT32>::min(), -1), "");
}

// ═══════════════════════════════════════════════════════════════════════════
// rem — checked remainder (div-by-zero → FAIL, INT32_MIN % -1 → 0)
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonArithmetic, RemNormal) {
    EXPECT_EQ(1, chaos::il2cpp::common::rem(7, 3));
    EXPECT_EQ(-1, chaos::il2cpp::common::rem(-7, 3));
    EXPECT_EQ(0, chaos::il2cpp::common::rem(6, 3));
}

TEST(CommonArithmetic, RemByZero) {
    EXPECT_DEATH(chaos::il2cpp::common::rem(1, 0), "");
}

TEST(CommonArithmetic, RemInt32MinByNegOne) {
    // Special case: INT32_MIN % -1 returns 0 instead of overflowing
    EXPECT_EQ(0, chaos::il2cpp::common::rem(std::numeric_limits<CHAOS_IL2CPP_INT32>::min(), -1));
}

// ═══════════════════════════════════════════════════════════════════════════
// shift_left
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonArithmetic, ShiftLeftNormal) {
    EXPECT_EQ(4, shift_left(1, 2));
    EXPECT_EQ(0, shift_left(0, 5));
    EXPECT_EQ(-1, shift_left(-1, 0));  // shift by 0 returns original
}

TEST(CommonArithmetic, ShiftLeftMasked) {
    // shift amount is masked with 31, so shift_left(1, 33) == shift_left(1, 1)
    EXPECT_EQ(shift_left(1, 1), shift_left(1, 33));
    EXPECT_EQ(shift_left(1, 0), shift_left(1, 32));
}

// ═══════════════════════════════════════════════════════════════════════════
// shift_right (arithmetic — sign-extending)
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonArithmetic, ShiftRightNormal) {
    EXPECT_EQ(2, shift_right(4, 1));
    EXPECT_EQ(0, shift_right(1, 1));
    EXPECT_EQ(-1, shift_right(-1, 1));  // sign-extending: -1 >> N = -1
}

TEST(CommonArithmetic, ShiftRightNegative) {
    // -8 >> 2 = -2 (arithmetic shift, sign-extending)
    EXPECT_EQ(-2, shift_right(-8, 2));
    // -1 >> any positive amount = -1 (all bits are 1, sign-extending keeps them)
    EXPECT_EQ(-1, shift_right(-1, 5));
}

TEST(CommonArithmetic, ShiftRightMasked) {
    EXPECT_EQ(shift_right(16, 2), shift_right(16, 34));
    EXPECT_EQ(shift_right(16, 0), shift_right(16, 32));
}

// ═══════════════════════════════════════════════════════════════════════════
// shift_right_unsigned (logical — zero-filling)
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonArithmetic, ShiftRightUnsignedNormal) {
    EXPECT_EQ(2, shift_right_unsigned(4, 1));
    EXPECT_EQ(0, shift_right_unsigned(1, 1));
}

TEST(CommonArithmetic, ShiftRightUnsignedNegative) {
    // -1 (0xFFFFFFFF) >> 28 = 0x0000000F = 15 (zero-filling)
    EXPECT_EQ(15, shift_right_unsigned(-1, 28));
    // -8 >> 2 = 0x3FFFFFFE = 1073741822 (zero-filling for unsigned shift)
    EXPECT_EQ(1073741822, shift_right_unsigned(-8, 2));
}

TEST(CommonArithmetic, ShiftRightUnsignedMasked) {
    EXPECT_EQ(shift_right_unsigned(16, 2), shift_right_unsigned(16, 34));
}
