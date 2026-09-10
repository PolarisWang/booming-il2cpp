// varargs_stubs_test.cpp — Unit tests for varargs P/Invoke stubs (t1a-3)
//
// Tests all 6 runtime helper functions for variadic arg buffer manipulation.
//
// Links against CHAOS_COMMON_TEST_LIBS for ground lib resolution.
// varargs_stubs.cpp is compiled directly (minimal deps, no GC requirement).

#include <gtest/gtest.h>
#include <chaos/native_types.h>

#include <cstring>
#include <cmath>

// ═══════════════════════════════════════════════════════════════════════════
// Forward declarations for the stub functions
// ═══════════════════════════════════════════════════════════════════════════

extern "C" {
CHAOS_IL2CPP_UINT64 ChaosVarargsGetArg(CHAOS_IL2CPP_INTPTR* buf, uint32_t index) noexcept;
uint32_t ChaosVarargsGetCount(uint32_t count) noexcept;
CHAOS_IL2CPP_INT32 ChaosVarargsGetInt32(CHAOS_IL2CPP_INTPTR* buf, uint32_t index) noexcept;
CHAOS_IL2CPP_INT64 ChaosVarargsGetInt64(CHAOS_IL2CPP_INTPTR* buf, uint32_t index) noexcept;
double ChaosVarargsGetDouble(CHAOS_IL2CPP_INTPTR* buf, uint32_t index) noexcept;
void ChaosVarargsSetArg(CHAOS_IL2CPP_INTPTR* buf, uint32_t index, CHAOS_IL2CPP_UINT64 value) noexcept;
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosVarargsGetCount tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Varargs_GetCount, Zero) {
    EXPECT_EQ(ChaosVarargsGetCount(0), 0u);
}

TEST(Varargs_GetCount, One) {
    EXPECT_EQ(ChaosVarargsGetCount(1), 1u);
}

TEST(Varargs_GetCount, Sixteen) {
    EXPECT_EQ(ChaosVarargsGetCount(16), 16u);
}

TEST(Varargs_GetCount, MaxBounds) {
    EXPECT_EQ(ChaosVarargsGetCount(UINT32_MAX), UINT32_MAX);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosVarargsGetArg tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Varargs_GetArg, ReturnsValueAtIndex) {
    CHAOS_IL2CPP_INTPTR buf[4] = {10, 20, 30, 40};

    EXPECT_EQ(ChaosVarargsGetArg(buf, 0), 10u);
    EXPECT_EQ(ChaosVarargsGetArg(buf, 1), 20u);
    EXPECT_EQ(ChaosVarargsGetArg(buf, 2), 30u);
    EXPECT_EQ(ChaosVarargsGetArg(buf, 3), 40u);
}

TEST(Varargs_GetArg, NullBuffer) {
    EXPECT_EQ(ChaosVarargsGetArg(nullptr, 0), 0u);
}

TEST(Varargs_GetArg, NegativeValues) {
    CHAOS_IL2CPP_INTPTR buf[2] = {
        static_cast<CHAOS_IL2CPP_INTPTR>(-1),
        static_cast<CHAOS_IL2CPP_INTPTR>(-42)
    };

    EXPECT_EQ(ChaosVarargsGetArg(buf, 0), static_cast<CHAOS_IL2CPP_UINT64>(-1));
    EXPECT_EQ(ChaosVarargsGetArg(buf, 1), static_cast<CHAOS_IL2CPP_UINT64>(-42));
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosVarargsGetInt32 tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Varargs_GetInt32, PositiveValues) {
    CHAOS_IL2CPP_INTPTR buf[3] = {100, 200, 300};

    EXPECT_EQ(ChaosVarargsGetInt32(buf, 0), 100);
    EXPECT_EQ(ChaosVarargsGetInt32(buf, 1), 200);
    EXPECT_EQ(ChaosVarargsGetInt32(buf, 2), 300);
}

TEST(Varargs_GetInt32, NegativeValues) {
    CHAOS_IL2CPP_INTPTR buf[2] = {
        static_cast<CHAOS_IL2CPP_INTPTR>(-1),
        static_cast<CHAOS_IL2CPP_INTPTR>(-100)
    };

    EXPECT_EQ(ChaosVarargsGetInt32(buf, 0), -1);
    EXPECT_EQ(ChaosVarargsGetInt32(buf, 1), -100);
}

TEST(Varargs_GetInt32, TruncatesHighBits) {
    // 0xDEADBEEFCAFE1234 truncated to int32 = 0xCAFE1234 = -889192396
    CHAOS_IL2CPP_INTPTR buf[1] = {
        static_cast<CHAOS_IL2CPP_INTPTR>(0xDEADBEEFCAFE1234ULL)
    };

    EXPECT_EQ(ChaosVarargsGetInt32(buf, 0), static_cast<CHAOS_IL2CPP_INT32>(0xCAFE1234));
}

TEST(Varargs_GetInt32, NullBuffer) {
    EXPECT_EQ(ChaosVarargsGetInt32(nullptr, 0), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosVarargsGetInt64 tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Varargs_GetInt64, PositiveValues) {
    CHAOS_IL2CPP_INTPTR buf[3] = {0x7FFFFFFFFFFFFFFFULL, 1, 9999};

    EXPECT_EQ(ChaosVarargsGetInt64(buf, 0), 0x7FFFFFFFFFFFFFFFLL);
    EXPECT_EQ(ChaosVarargsGetInt64(buf, 1), 1);
    EXPECT_EQ(ChaosVarargsGetInt64(buf, 2), 9999);
}

TEST(Varargs_GetInt64, NegativeValues) {
    CHAOS_IL2CPP_INTPTR buf[2] = {
        static_cast<CHAOS_IL2CPP_INTPTR>(-1),
        static_cast<CHAOS_IL2CPP_INTPTR>(-9223372036854775807LL - 1)
    };

    EXPECT_EQ(ChaosVarargsGetInt64(buf, 0), -1);
    EXPECT_EQ(ChaosVarargsGetInt64(buf, 1), (-9223372036854775807LL - 1));
}

TEST(Varargs_GetInt64, NullBuffer) {
    EXPECT_EQ(ChaosVarargsGetInt64(nullptr, 0), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosVarargsGetDouble tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Varargs_GetDouble, PositiveValue) {
    double val = 3.14159;
    CHAOS_IL2CPP_INTPTR buf[1];
    std::memcpy(buf, &val, sizeof(val));

    double result = ChaosVarargsGetDouble(buf, 0);
    EXPECT_DOUBLE_EQ(result, 3.14159);
}

TEST(Varargs_GetDouble, NegativeValue) {
    double val = -2.71828;
    CHAOS_IL2CPP_INTPTR buf[1];
    std::memcpy(buf, &val, sizeof(val));

    double result = ChaosVarargsGetDouble(buf, 0);
    EXPECT_DOUBLE_EQ(result, -2.71828);
}

TEST(Varargs_GetDouble, Zero) {
    double val = 0.0;
    CHAOS_IL2CPP_INTPTR buf[1];
    std::memcpy(buf, &val, sizeof(val));

    double result = ChaosVarargsGetDouble(buf, 0);
    EXPECT_DOUBLE_EQ(result, 0.0);
}

TEST(Varargs_GetDouble, LargeValue) {
    double val = 1.0e100;
    CHAOS_IL2CPP_INTPTR buf[1];
    std::memcpy(buf, &val, sizeof(val));

    double result = ChaosVarargsGetDouble(buf, 0);
    EXPECT_DOUBLE_EQ(result, 1.0e100);
}

TEST(Varargs_GetDouble, NullBuffer) {
    EXPECT_DOUBLE_EQ(ChaosVarargsGetDouble(nullptr, 0), 0.0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosVarargsSetArg tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Varargs_SetArg, WriteAndRead) {
    CHAOS_IL2CPP_INTPTR buf[4] = {};

    ChaosVarargsSetArg(buf, 0, 42);
    ChaosVarargsSetArg(buf, 1, 0xDEADBEEF);
    ChaosVarargsSetArg(buf, 2, static_cast<CHAOS_IL2CPP_UINT64>(-1));

    EXPECT_EQ(ChaosVarargsGetArg(buf, 0), 42u);
    EXPECT_EQ(ChaosVarargsGetArg(buf, 1), 0xDEADBEEFull);
    EXPECT_EQ(ChaosVarargsGetArg(buf, 2), static_cast<CHAOS_IL2CPP_UINT64>(-1));
    EXPECT_EQ(ChaosVarargsGetArg(buf, 3), 0u);  // untouched
}

TEST(Varargs_SetArg, OverwriteExisting) {
    CHAOS_IL2CPP_INTPTR buf[2] = {100, 200};

    ChaosVarargsSetArg(buf, 0, 999);
    EXPECT_EQ(ChaosVarargsGetArg(buf, 0), 999u);
    EXPECT_EQ(ChaosVarargsGetArg(buf, 1), 200u);  // unchanged
}

TEST(Varargs_SetArg, NullBuffer) {
    // Should not crash
    ChaosVarargsSetArg(nullptr, 0, 42);
}

// ═══════════════════════════════════════════════════════════════════════════
// Integration: full cycle tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Varargs_Integration, FullCycle) {
    // Simulate a varargs call with mixed types packed into pointer-sized slots
    CHAOS_IL2CPP_INTPTR buf[6] = {};

    // Two integers and a double
    ChaosVarargsSetArg(buf, 0, 100);
    ChaosVarargsSetArg(buf, 1, 200);

    double pi = 3.14159;
    CHAOS_IL2CPP_UINT64 pi_bits = 0;
    std::memcpy(&pi_bits, &pi, sizeof(pi_bits));
    ChaosVarargsSetArg(buf, 2, pi_bits);

    // Three more int64s
    ChaosVarargsSetArg(buf, 3, static_cast<CHAOS_IL2CPP_UINT64>(-1));
    ChaosVarargsSetArg(buf, 4, 0x7FFFFFFFFFFFFFFFULL);
    ChaosVarargsSetArg(buf, 5, 0);

    // Verify
    EXPECT_EQ(ChaosVarargsGetInt32(buf, 0), 100);
    EXPECT_EQ(ChaosVarargsGetInt64(buf, 1), 200);
    EXPECT_DOUBLE_EQ(ChaosVarargsGetDouble(buf, 2), 3.14159);
    EXPECT_EQ(ChaosVarargsGetInt64(buf, 3), -1);
    EXPECT_EQ(ChaosVarargsGetInt64(buf, 4), 0x7FFFFFFFFFFFFFFFLL);
    EXPECT_EQ(ChaosVarargsGetInt64(buf, 5), 0);
    EXPECT_EQ(ChaosVarargsGetCount(6), 6u);
}

TEST(Varargs_Integration, SetAndRetrieveAcrossSlots) {
    CHAOS_IL2CPP_INTPTR buf[3] = {};

    // Set all three slots with distinct values
    ChaosVarargsSetArg(buf, 0, 0x1111111111111111ULL);
    ChaosVarargsSetArg(buf, 1, 0x2222222222222222ULL);
    ChaosVarargsSetArg(buf, 2, 0x3333333333333333ULL);

    // Verify no cross-contamination
    EXPECT_EQ(ChaosVarargsGetArg(buf, 0), 0x1111111111111111ULL);
    EXPECT_EQ(ChaosVarargsGetArg(buf, 1), 0x2222222222222222ULL);
    EXPECT_EQ(ChaosVarargsGetArg(buf, 2), 0x3333333333333333ULL);
}

TEST(Varargs_Integration, ZeroLengthBuffer) {
    // Buffer with 0 elements — just verify null/empty handling
    CHAOS_IL2CPP_INTPTR* null_buf = nullptr;
    EXPECT_EQ(ChaosVarargsGetArg(null_buf, 0), 0u);
    EXPECT_EQ(ChaosVarargsGetCount(0), 0u);

    // Non-null but empty — should not crash reading count
    CHAOS_IL2CPP_INTPTR buf[1] = {};
    EXPECT_EQ(ChaosVarargsGetArg(buf, 0), 0u);
}

TEST(Varargs_Integration, TypedAccessConsistency) {
    // Write value via SetArg, then read back through all typed accessors
    CHAOS_IL2CPP_INTPTR buf[1] = {};
    ChaosVarargsSetArg(buf, 0, 0xDEADBEEFCAFE1234ULL);

    // All typed views of the same bits
    EXPECT_EQ(ChaosVarargsGetArg(buf, 0), 0xDEADBEEFCAFE1234ULL);
    EXPECT_EQ(ChaosVarargsGetInt32(buf, 0), static_cast<CHAOS_IL2CPP_INT32>(0xCAFE1234));
    EXPECT_EQ(ChaosVarargsGetInt64(buf, 0), static_cast<CHAOS_IL2CPP_INT64>(0xDEADBEEFCAFE1234LL));
}

TEST(Varargs_Integration, PartialBufferFill) {
    // Only first 2 of 4 slots written — remaining should be zero
    CHAOS_IL2CPP_INTPTR buf[4] = {};
    std::memset(buf, 0xFF, sizeof(buf));  // fill with garbage

    ChaosVarargsSetArg(buf, 0, 42);
    ChaosVarargsSetArg(buf, 1, 99);

    EXPECT_EQ(ChaosVarargsGetArg(buf, 0), 42u);
    EXPECT_EQ(ChaosVarargsGetArg(buf, 1), 99u);
    // Note: remaining slots still contain 0xFF — no implicit zeroing
    EXPECT_NE(ChaosVarargsGetArg(buf, 2), 0u);
}
