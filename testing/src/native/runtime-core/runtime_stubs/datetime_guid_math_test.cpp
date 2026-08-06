// datetime_guid_math_test.cpp — Unit tests for datetime/guid/math stubs (T0-4)
//
// Tests all public API functions for null/zero-input safety and basic
// functional correctness.  All three stubs are full implementations
// (not no-ops), so tests verify actual behavior.
//
// Links against CHAOS_COMMON_TEST_LIBS for symbol resolution.

#include <gtest/gtest.h>
#include <chaos/native_types.h>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>

// ── Extern "C" declarations for datetime_stubs ──
extern "C" {
CHAOS_IL2CPP_INT64 ChaosDatetimeGetUtcNow(void) noexcept;
CHAOS_IL2CPP_INT32 ChaosDatetimeGetHashCode(CHAOS_IL2CPP_INTPTR datetime) noexcept;
CHAOS_IL2CPP_INTPTR ChaosDateTimeToString(CHAOS_IL2CPP_INT64 dt) noexcept;
CHAOS_IL2CPP_INTPTR ChaosDateTimeToStringFormat(CHAOS_IL2CPP_INT64 dt, CHAOS_IL2CPP_INTPTR format) noexcept;
CHAOS_IL2CPP_INT64 ChaosDateTimeAddDays(CHAOS_IL2CPP_INT64 dt, CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_INT64 ChaosDateTimeAddHours(CHAOS_IL2CPP_INT64 dt, CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_INT64 ChaosDateTimeAddMinutes(CHAOS_IL2CPP_INT64 dt, CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_INT32 ChaosDateTimeCompare(CHAOS_IL2CPP_INT64 left, CHAOS_IL2CPP_INT64 right) noexcept;
CHAOS_IL2CPP_INT32 ChaosDateTimeDaysInMonth(CHAOS_IL2CPP_INT32 year, CHAOS_IL2CPP_INT32 month) noexcept;
CHAOS_IL2CPP_INT64 ChaosTimeSpanFromDays(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_INT64 ChaosTimeSpanFromHours(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_INT64 ChaosTimeSpanFromMinutes(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_INT64 ChaosTimeSpanParse(CHAOS_IL2CPP_INTPTR value) noexcept;
void ChaosDateTimeCtor3(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INT32 year,
                        CHAOS_IL2CPP_INT32 month, CHAOS_IL2CPP_INT32 day) noexcept;
void ChaosDateTimeCtor6(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INT32 year,
                        CHAOS_IL2CPP_INT32 month, CHAOS_IL2CPP_INT32 day,
                        CHAOS_IL2CPP_INT32 hour, CHAOS_IL2CPP_INT32 minute,
                        CHAOS_IL2CPP_INT32 second) noexcept;
CHAOS_IL2CPP_INT64 ChaosDateTimeParse(CHAOS_IL2CPP_INTPTR value) noexcept;
void ChaosTimeSpanCtor(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INT32 hours,
                       CHAOS_IL2CPP_INT32 minutes, CHAOS_IL2CPP_INT32 seconds) noexcept;
}  // extern "C"

// ── Extern "C" declarations for guid_stubs ──
extern "C" {
void                ChaosGuidCtor(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosGuidNewGuid(void) noexcept;
CHAOS_IL2CPP_INTPTR ChaosGuidParse(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT32  ChaosGuidGetHashCode(CHAOS_IL2CPP_INTPTR guid) noexcept;
CHAOS_IL2CPP_INTPTR ChaosGuidToString(CHAOS_IL2CPP_INTPTR guid) noexcept;
}  // extern "C"

// ── Extern "C" declarations for math_stubs ──
extern "C" {
CHAOS_IL2CPP_INT32    ChaosMathAbsInt32(CHAOS_IL2CPP_INT32 value) noexcept;
CHAOS_IL2CPP_FLOAT64  ChaosMathAbsDouble(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_FLOAT64  ChaosMathCeiling(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_FLOAT64  ChaosMathFloor(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_FLOAT64  ChaosMathRound(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_FLOAT64  ChaosMathRound2(CHAOS_IL2CPP_FLOAT64 value, CHAOS_IL2CPP_INT32 digits) noexcept;
CHAOS_IL2CPP_INT32    ChaosMathMaxInt32(CHAOS_IL2CPP_INT32 a, CHAOS_IL2CPP_INT32 b) noexcept;
CHAOS_IL2CPP_FLOAT64  ChaosMathMaxDouble(CHAOS_IL2CPP_FLOAT64 a, CHAOS_IL2CPP_FLOAT64 b) noexcept;
CHAOS_IL2CPP_INT32    ChaosMathMinInt32(CHAOS_IL2CPP_INT32 a, CHAOS_IL2CPP_INT32 b) noexcept;
CHAOS_IL2CPP_FLOAT64  ChaosMathMinDouble(CHAOS_IL2CPP_FLOAT64 a, CHAOS_IL2CPP_FLOAT64 b) noexcept;
CHAOS_IL2CPP_FLOAT64  ChaosMathPow(CHAOS_IL2CPP_FLOAT64 x, CHAOS_IL2CPP_FLOAT64 y) noexcept;
CHAOS_IL2CPP_FLOAT64  ChaosMathSin(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64  ChaosMathCos(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_INT64    ChaosMathBigMul(CHAOS_IL2CPP_INT32 a, CHAOS_IL2CPP_INT32 b) noexcept;
CHAOS_IL2CPP_FLOAT64  ChaosMathSqrt(CHAOS_IL2CPP_FLOAT64 value) noexcept;
}  // extern "C"

// ═══════════════════════════════════════════════════════════════════════════
// DateTime tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(DatetimeStubs, GetUtcNow_ReturnsNonZero) {
    EXPECT_NE(ChaosDatetimeGetUtcNow(), 0);
}

TEST(DatetimeStubs, GetHashCode_Null) {
    EXPECT_EQ(ChaosDatetimeGetHashCode(0), 0);
}

TEST(DatetimeStubs, ToString_ValidTicks) {
    // 2024-01-15 10:30:00 in ticks
    CHAOS_IL2CPP_INT64 ticks = 638407314000000000LL;
    EXPECT_NE(ChaosDateTimeToString(ticks), 0);
}

TEST(DatetimeStubs, ToStringFormat_NullFormat) {
    CHAOS_IL2CPP_INT64 ticks = 638407314000000000LL;
    // Null format should return default formatted string
    EXPECT_NE(ChaosDateTimeToStringFormat(ticks, 0), 0);
}

TEST(DatetimeStubs, AddDays_Positive) {
    CHAOS_IL2CPP_INT64 tick_day = 864000000000LL;
    CHAOS_IL2CPP_INT64 base = 0;
    EXPECT_EQ(ChaosDateTimeAddDays(base, 1.0), tick_day);
}

TEST(DatetimeStubs, AddDays_Negative) {
    CHAOS_IL2CPP_INT64 tick_day = 864000000000LL;
    CHAOS_IL2CPP_INT64 base = 0;
    EXPECT_EQ(ChaosDateTimeAddDays(base, -1.0), -tick_day);
}

TEST(DatetimeStubs, AddHours_Positive) {
    CHAOS_IL2CPP_INT64 tick_hour = 36000000000LL;
    EXPECT_EQ(ChaosDateTimeAddHours(0, 2.0), 2 * tick_hour);
}

TEST(DatetimeStubs, AddMinutes_Positive) {
    CHAOS_IL2CPP_INT64 tick_min = 600000000LL;
    EXPECT_EQ(ChaosDateTimeAddMinutes(0, 30.0), 30 * tick_min);
}

TEST(DatetimeStubs, Compare_Less) {
    EXPECT_EQ(ChaosDateTimeCompare(100, 200), -1);
}

TEST(DatetimeStubs, Compare_Equal) {
    EXPECT_EQ(ChaosDateTimeCompare(100, 100), 0);
}

TEST(DatetimeStubs, Compare_Greater) {
    EXPECT_EQ(ChaosDateTimeCompare(200, 100), 1);
}

TEST(DatetimeStubs, DaysInMonth_January) {
    EXPECT_EQ(ChaosDateTimeDaysInMonth(2024, 1), 31);
}

TEST(DatetimeStubs, DaysInMonth_FebruaryLeap) {
    EXPECT_EQ(ChaosDateTimeDaysInMonth(2024, 2), 29);
}

TEST(DatetimeStubs, DaysInMonth_FebruaryNonLeap) {
    EXPECT_EQ(ChaosDateTimeDaysInMonth(2023, 2), 28);
}

TEST(DatetimeStubs, DaysInMonth_InvalidMonth) {
    EXPECT_EQ(ChaosDateTimeDaysInMonth(2024, 13), 0);
}

TEST(DatetimeStubs, DaysInMonth_InvalidMonthZero) {
    EXPECT_EQ(ChaosDateTimeDaysInMonth(2024, 0), 0);
}

TEST(DatetimeStubs, TimeSpanFromDays) {
    CHAOS_IL2CPP_INT64 tick_day = 864000000000LL;
    EXPECT_EQ(ChaosTimeSpanFromDays(1.0), tick_day);
}

TEST(DatetimeStubs, TimeSpanFromHours) {
    CHAOS_IL2CPP_INT64 tick_hour = 36000000000LL;
    EXPECT_EQ(ChaosTimeSpanFromHours(2.0), 2 * tick_hour);
}

TEST(DatetimeStubs, TimeSpanFromMinutes) {
    CHAOS_IL2CPP_INT64 tick_min = 600000000LL;
    EXPECT_EQ(ChaosTimeSpanFromMinutes(30.0), 30 * tick_min);
}

TEST(DatetimeStubs, TimeSpanParse_Null) {
    EXPECT_EQ(ChaosTimeSpanParse(0), 0);
}

TEST(DatetimeStubs, DateTimeParse_Null) {
    EXPECT_EQ(ChaosDateTimeParse(0), 0);
}

TEST(DatetimeStubs, Ctor3_NullInstance) {
    // Should not crash when instance is null
    ChaosDateTimeCtor3(0, 2024, 1, 15);
}

TEST(DatetimeStubs, Ctor6_NullInstance) {
    ChaosDateTimeCtor6(0, 2024, 1, 15, 10, 30, 0);
}

TEST(DatetimeStubs, TimeSpanCtor_NullInstance) {
    ChaosTimeSpanCtor(0, 1, 30, 0);
}

TEST(DatetimeStubs, GetHashCode_Valid) {
    CHAOS_IL2CPP_INT64 ticks = 638407314000000000LL;
    // Hash of the ticks value itself (implementation reads from pointer)
    CHAOS_IL2CPP_INT32 expected = static_cast<CHAOS_IL2CPP_INT32>(ticks)
                                 ^ static_cast<CHAOS_IL2CPP_INT32>(ticks >> 32);
    EXPECT_EQ(ChaosDatetimeGetHashCode(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ticks)), expected);
}

// ═══════════════════════════════════════════════════════════════════════════
// Guid tests (null-safety + smoke)
// ═══════════════════════════════════════════════════════════════════════════

TEST(GuidStubs, NewGuid_ReturnsNonNull) {
    EXPECT_NE(ChaosGuidNewGuid(), 0);
}

TEST(GuidStubs, Parse_Null) {
    EXPECT_EQ(ChaosGuidParse(0), 0);
}

TEST(GuidStubs, GetHashCode_Null) {
    EXPECT_EQ(ChaosGuidGetHashCode(0), 0);
}

TEST(GuidStubs, ToString_Null) {
    EXPECT_EQ(ChaosGuidToString(0), 0);
}

TEST(GuidStubs, Ctor_NullInstance) {
    // Should not crash when instance is null
    ChaosGuidCtor(0, 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// Math tests (value-type only — no null params possible)
// ═══════════════════════════════════════════════════════════════════════════

TEST(MathStubs, AbsInt32_Positive) {
    EXPECT_EQ(ChaosMathAbsInt32(42), 42);
}

TEST(MathStubs, AbsInt32_Negative) {
    EXPECT_EQ(ChaosMathAbsInt32(-42), 42);
}

TEST(MathStubs, AbsInt32_Zero) {
    EXPECT_EQ(ChaosMathAbsInt32(0), 0);
}

TEST(MathStubs, AbsDouble_Positive) {
    EXPECT_DOUBLE_EQ(ChaosMathAbsDouble(3.14), 3.14);
}

TEST(MathStubs, AbsDouble_Negative) {
    EXPECT_DOUBLE_EQ(ChaosMathAbsDouble(-3.14), 3.14);
}

TEST(MathStubs, AbsDouble_NaN) {
    EXPECT_TRUE(std::isnan(ChaosMathAbsDouble(std::numeric_limits<double>::quiet_NaN())));
}

TEST(MathStubs, Ceiling) {
    EXPECT_DOUBLE_EQ(ChaosMathCeiling(3.14), 4.0);
}

TEST(MathStubs, Ceiling_Negative) {
    EXPECT_DOUBLE_EQ(ChaosMathCeiling(-3.14), -3.0);
}

TEST(MathStubs, Floor) {
    EXPECT_DOUBLE_EQ(ChaosMathFloor(3.14), 3.0);
}

TEST(MathStubs, Floor_Negative) {
    EXPECT_DOUBLE_EQ(ChaosMathFloor(-3.14), -4.0);
}

TEST(MathStubs, Round) {
    EXPECT_DOUBLE_EQ(ChaosMathRound(3.5), 4.0);
}

TEST(MathStubs, Round2) {
    EXPECT_DOUBLE_EQ(ChaosMathRound2(3.14159, 2), 3.14);
}

TEST(MathStubs, MaxInt32_ABigger) {
    EXPECT_EQ(ChaosMathMaxInt32(10, 5), 10);
}

TEST(MathStubs, MaxInt32_BBigger) {
    EXPECT_EQ(ChaosMathMaxInt32(5, 10), 10);
}

TEST(MathStubs, MaxInt32_Equal) {
    EXPECT_EQ(ChaosMathMaxInt32(7, 7), 7);
}

TEST(MathStubs, MaxDouble) {
    EXPECT_DOUBLE_EQ(ChaosMathMaxDouble(3.14, 2.71), 3.14);
}

TEST(MathStubs, MinInt32_ABigger) {
    EXPECT_EQ(ChaosMathMinInt32(10, 5), 5);
}

TEST(MathStubs, MinInt32_BBigger) {
    EXPECT_EQ(ChaosMathMinInt32(5, 10), 5);
}

TEST(MathStubs, MinInt32_Equal) {
    EXPECT_EQ(ChaosMathMinInt32(7, 7), 7);
}

TEST(MathStubs, MinDouble) {
    EXPECT_DOUBLE_EQ(ChaosMathMinDouble(3.14, 2.71), 2.71);
}

TEST(MathStubs, Pow) {
    EXPECT_DOUBLE_EQ(ChaosMathPow(2.0, 3.0), 8.0);
}

TEST(MathStubs, Sin) {
    EXPECT_DOUBLE_EQ(ChaosMathSin(0.0), 0.0);
}

TEST(MathStubs, Cos) {
    EXPECT_DOUBLE_EQ(ChaosMathCos(0.0), 1.0);
}

TEST(MathStubs, BigMul) {
    EXPECT_EQ(ChaosMathBigMul(100000, 200000), 20000000000LL);
}

TEST(MathStubs, BigMul_Negative) {
    EXPECT_EQ(ChaosMathBigMul(-100000, 200000), -20000000000LL);
}

TEST(MathStubs, Sqrt_FromDouble) {
    // Real stub signature is FLOAT64(double), same as Pow/Sin/Cos.
    EXPECT_DOUBLE_EQ(ChaosMathSqrt(9.0), 3.0);
}

TEST(MathStubs, Sqrt_FromDoubleZero) {
    EXPECT_DOUBLE_EQ(ChaosMathSqrt(0.0), 0.0);
}
