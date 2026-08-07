/// common_native_types_test — Unit tests for native_types.h type system.
///
/// Tests type sizes, numeric limits, macro correctness, atomic intrinsic
/// compilation, and chaos_load_indirect/chaos_store_indirect templates.

#include <gtest/gtest.h>
#include <chaos/common.h>
#include <cstdint>
#include <climits>
#include <cstdlib>

using namespace chaos::il2cpp::common;

// ═══════════════════════════════════════════════════════════════════════════
// Type sizes
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonNativeTypes, TypeSizes) {
    EXPECT_EQ(1, sizeof(CHAOS_IL2CPP_INT8));
    EXPECT_EQ(2, sizeof(CHAOS_IL2CPP_INT16));
    EXPECT_EQ(4, sizeof(CHAOS_IL2CPP_INT32));
    EXPECT_EQ(8, sizeof(CHAOS_IL2CPP_INT64));
    EXPECT_EQ(1, sizeof(CHAOS_IL2CPP_UINT8));
    EXPECT_EQ(2, sizeof(CHAOS_IL2CPP_UINT16));
    EXPECT_EQ(4, sizeof(CHAOS_IL2CPP_UINT32));
    EXPECT_EQ(8, sizeof(CHAOS_IL2CPP_UINT64));
}

TEST(CommonNativeTypes, FloatSizes) {
    EXPECT_EQ(4, sizeof(CHAOS_IL2CPP_FLOAT32));
    EXPECT_EQ(8, sizeof(CHAOS_IL2CPP_FLOAT64));
}

TEST(CommonNativeTypes, IntPtrSize) {
    // On 64-bit Windows, both INTPTR and SIZE should be 8 bytes.
    EXPECT_EQ(8, sizeof(CHAOS_IL2CPP_INTPTR));
    EXPECT_EQ(8, sizeof(CHAOS_IL2CPP_UINTPTR));
    EXPECT_EQ(8, sizeof(CHAOS_IL2CPP_SIZE));
}

// ═══════════════════════════════════════════════════════════════════════════
// Numeric limits
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonNativeTypes, NumericLimitsMin) {
    EXPECT_EQ(-128, CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(CHAOS_IL2CPP_INT8));
    EXPECT_EQ(-32768, CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(CHAOS_IL2CPP_INT16));
    EXPECT_EQ(-2147483647 - 1, CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(CHAOS_IL2CPP_INT32));
    EXPECT_EQ(0, CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(CHAOS_IL2CPP_UINT8));
    EXPECT_EQ(0, CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(CHAOS_IL2CPP_UINT32));
}

TEST(CommonNativeTypes, NumericLimitsMax) {
    EXPECT_EQ(127, CHAOS_IL2CPP_NUMERIC_LIMITS_MAX(CHAOS_IL2CPP_INT8));
    EXPECT_EQ(32767, CHAOS_IL2CPP_NUMERIC_LIMITS_MAX(CHAOS_IL2CPP_INT16));
    EXPECT_EQ(2147483647, CHAOS_IL2CPP_NUMERIC_LIMITS_MAX(CHAOS_IL2CPP_INT32));
    EXPECT_EQ(255, CHAOS_IL2CPP_NUMERIC_LIMITS_MAX(CHAOS_IL2CPP_UINT8));
    EXPECT_EQ(65535, CHAOS_IL2CPP_NUMERIC_LIMITS_MAX(CHAOS_IL2CPP_UINT16));
    EXPECT_EQ(0xFFFFFFFFu, CHAOS_IL2CPP_NUMERIC_LIMITS_MAX(CHAOS_IL2CPP_UINT32));
}

// ═══════════════════════════════════════════════════════════════════════════
// Explicit MIN/MAX constants
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonNativeTypes, ExplicitMinMax) {
    EXPECT_EQ(-128, CHAOS_IL2CPP_INT8_MIN);
    EXPECT_EQ(127, CHAOS_IL2CPP_INT8_MAX);
    EXPECT_EQ(-32768, CHAOS_IL2CPP_INT16_MIN);
    EXPECT_EQ(32767, CHAOS_IL2CPP_INT16_MAX);
    EXPECT_EQ(static_cast<int64_t>(-2147483647 - 1), CHAOS_IL2CPP_INT32_MIN);
    EXPECT_EQ(2147483647, CHAOS_IL2CPP_INT32_MAX);
    EXPECT_EQ(0xFFFFFFFFu, CHAOS_IL2CPP_UINT32_MAX);
    EXPECT_EQ(0xFFFFFFFFFFFFFFFFu, CHAOS_IL2CPP_UINT64_MAX);
    EXPECT_EQ(255u, CHAOS_IL2CPP_UINT8_MAX);
    EXPECT_EQ(65535u, CHAOS_IL2CPP_UINT16_MAX);
}

// ═══════════════════════════════════════════════════════════════════════════
// Type equivalence
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonNativeTypes, TypeEquivalence) {
    EXPECT_TRUE((CHAOS_IL2CPP_IS_SAME(CHAOS_IL2CPP_INT32, int32_t)));
    EXPECT_TRUE((CHAOS_IL2CPP_IS_SAME(CHAOS_IL2CPP_INT64, int64_t)));
    EXPECT_TRUE((CHAOS_IL2CPP_IS_SAME(CHAOS_IL2CPP_UINT32, uint32_t)));
    EXPECT_TRUE((CHAOS_IL2CPP_IS_SAME(CHAOS_IL2CPP_UINT64, uint64_t)));
    EXPECT_TRUE((CHAOS_IL2CPP_IS_SAME(CHAOS_IL2CPP_FLOAT32, float)));
    EXPECT_TRUE((CHAOS_IL2CPP_IS_SAME(CHAOS_IL2CPP_FLOAT64, double)));
}

// ═══════════════════════════════════════════════════════════════════════════
// SUCCEEDED / FAILED macros
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonNativeTypes, SucceededFailed) {
    EXPECT_TRUE(CHAOS_IL2CPP_SUCCEEDED(0));
    EXPECT_TRUE(CHAOS_IL2CPP_SUCCEEDED(1));
    EXPECT_FALSE(CHAOS_IL2CPP_SUCCEEDED(-1));
    EXPECT_TRUE(CHAOS_IL2CPP_FAILED(-1));
    EXPECT_FALSE(CHAOS_IL2CPP_FAILED(0));
    EXPECT_FALSE(CHAOS_IL2CPP_FAILED(100));
}

// ═══════════════════════════════════════════════════════════════════════════
// IsSigned / IsUnsigned / IsIntegral / IsFloatingPoint
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonNativeTypes, IsSignedTrait) {
    EXPECT_TRUE(CHAOS_IL2CPP_IS_SIGNED(CHAOS_IL2CPP_INT32));
    EXPECT_FALSE(CHAOS_IL2CPP_IS_SIGNED(CHAOS_IL2CPP_UINT32));
    EXPECT_TRUE(CHAOS_IL2CPP_IS_FLOATING_POINT(CHAOS_IL2CPP_FLOAT32));
    EXPECT_TRUE(CHAOS_IL2CPP_IS_FLOATING_POINT(CHAOS_IL2CPP_FLOAT64));
    EXPECT_FALSE(CHAOS_IL2CPP_IS_FLOATING_POINT(CHAOS_IL2CPP_INT32));
    EXPECT_TRUE(CHAOS_IL2CPP_IS_INTEGRAL(CHAOS_IL2CPP_INT64));
    EXPECT_TRUE(CHAOS_IL2CPP_IS_INTEGRAL(CHAOS_IL2CPP_UINT8));
    EXPECT_FALSE(CHAOS_IL2CPP_IS_INTEGRAL(CHAOS_IL2CPP_FLOAT64));
}

// ═══════════════════════════════════════════════════════════════════════════
// MakeUnsigned
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonNativeTypes, MakeUnsigned) {
    EXPECT_TRUE((CHAOS_IL2CPP_IS_SAME(CHAOS_IL2CPP_MAKE_UNSIGNED(CHAOS_IL2CPP_INT32), CHAOS_IL2CPP_UINT32)));
    EXPECT_TRUE((CHAOS_IL2CPP_IS_SAME(CHAOS_IL2CPP_MAKE_UNSIGNED(CHAOS_IL2CPP_INT64), CHAOS_IL2CPP_UINT64)));
    EXPECT_TRUE((CHAOS_IL2CPP_IS_UNSIGNED(CHAOS_IL2CPP_MAKE_UNSIGNED(CHAOS_IL2CPP_INT32))));
}

// ═══════════════════════════════════════════════════════════════════════════
// Raw pointer tag constant
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonNativeTypes, RawPointerTag) {
    // Tag should be the high bit of a UINTPTR.
    CHAOS_IL2CPP_UINTPTR expected = CHAOS_IL2CPP_UINTPTR(1) << (sizeof(CHAOS_IL2CPP_UINTPTR) * CHAR_BIT - 1);
    EXPECT_EQ(expected, CHAOS_IL2CPP_RAW_POINTER_TAG);
}

// ═══════════════════════════════════════════════════════════════════════════
// chaos_resolve_native_int_slot
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonNativeTypes, ResolveNativeIntSlot) {
    CHAOS_IL2CPP_INTPTR value = 42;
    auto* slot = chaos_resolve_native_int_slot(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value));
    EXPECT_EQ(&value, slot);
    EXPECT_EQ(42, *slot);
}

// ═══════════════════════════════════════════════════════════════════════════
// chaos_load_indirect / chaos_store_indirect (via slot)
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonNativeTypes, LoadStoreIndirectViaSlot) {
    CHAOS_IL2CPP_INTPTR slot = 0;
    CHAOS_IL2CPP_INTPTR addr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&slot);

    chaos_store_indirect(addr, static_cast<CHAOS_IL2CPP_INTPTR>(99));
    EXPECT_EQ(99, chaos_load_indirect<CHAOS_IL2CPP_INTPTR>(addr));
}

TEST(CommonNativeTypes, LoadStoreIndirectViaPointer) {
    // Verify that chaos_load_indirect handles tagged (high-bit) pointers correctly.
    // The function strips CHAOS_IL2CPP_RAW_POINTER_TAG before dereferencing.
    auto* ptr = static_cast<CHAOS_IL2CPP_INTPTR*>(std::malloc(sizeof(CHAOS_IL2CPP_INTPTR)));
    ASSERT_NE(nullptr, ptr);
    *ptr = 42;

    // Compute tagged address: set the high bit (CHAOS_IL2CPP_RAW_POINTER_TAG)
    CHAOS_IL2CPP_UINTPTR raw = reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(ptr);
    CHAOS_IL2CPP_UINTPTR tagged = raw | (CHAOS_IL2CPP_UINTPTR(1) << 63);

    // Pass through the chaos_load_indirect / chaos_store_indirect API
    CHAOS_IL2CPP_INTPTR addr_as_slot = static_cast<CHAOS_IL2CPP_INTPTR>(tagged);
    CHAOS_IL2CPP_INTPTR loaded = chaos_load_indirect<CHAOS_IL2CPP_INTPTR>(addr_as_slot);
    EXPECT_EQ(42, loaded);

    chaos_store_indirect(addr_as_slot, static_cast<CHAOS_IL2CPP_INTPTR>(100));
    EXPECT_EQ(100, *ptr);

    std::free(ptr);
}

// ═══════════════════════════════════════════════════════════════════════════
// Memory operation macros compile check
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonNativeTypes, MemFuncs) {
    char src[16] = "hello";
    char dst[16] = {};
    CHAOS_IL2CPP_MEMCPY(dst, src, 6);
    EXPECT_EQ(0, CHAOS_IL2CPP_MEMCMP(dst, src, 6));

    CHAOS_IL2CPP_MEMSET(dst, 0, sizeof(dst));
    EXPECT_EQ(0, dst[0]);

    char overlap[] = "overlap";
    CHAOS_IL2CPP_MEMMOVE(overlap + 2, overlap, 4);
    EXPECT_EQ('o', overlap[2]);

    EXPECT_EQ(5, CHAOS_IL2CPP_STRLEN("hello"));
    EXPECT_EQ(0, CHAOS_IL2CPP_STRCMP("abc", "abc"));
    EXPECT_GT(CHAOS_IL2CPP_STRCMP("b", "a"), 0);
    EXPECT_LT(CHAOS_IL2CPP_STRCMP("a", "b"), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// Math function macros compile check
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonNativeTypes, MathFuncs) {
    EXPECT_FLOAT_EQ(0.0f, CHAOS_IL2CPP_SIN(0.0f));
    EXPECT_FLOAT_EQ(1.0f, CHAOS_IL2CPP_COS(0.0f));
    EXPECT_FLOAT_EQ(2.0f, CHAOS_IL2CPP_FABS(-2.0f));
    EXPECT_TRUE(CHAOS_IL2CPP_ISNAN(CHAOS_IL2CPP_NUMERIC_LIMITS_QUIET_NAN(CHAOS_IL2CPP_FLOAT64)));
    EXPECT_TRUE(CHAOS_IL2CPP_ISFINITE(1.0));
    EXPECT_FALSE(CHAOS_IL2CPP_ISFINITE(CHAOS_IL2CPP_NUMERIC_LIMITS_INFINITY(CHAOS_IL2CPP_FLOAT64)));
    EXPECT_TRUE(CHAOS_IL2CPP_ISINF(CHAOS_IL2CPP_NUMERIC_LIMITS_INFINITY(CHAOS_IL2CPP_FLOAT64)));
    EXPECT_FLOAT_EQ(3.0f, CHAOS_IL2CPP_FLOOR(3.7f));
    EXPECT_FLOAT_EQ(4.0f, CHAOS_IL2CPP_CEIL(3.1f));
    EXPECT_FLOAT_EQ(8.0f, CHAOS_IL2CPP_POW(2.0f, 3.0f));
    EXPECT_FLOAT_EQ(9.0f, CHAOS_IL2CPP_SQRT(81.0f));
}

// ═══════════════════════════════════════════════════════════════════════════
// CHAOS_IL2CPP_PAIR
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonNativeTypes, Pair) {
    CHAOS_IL2CPP_PAIR(int, double) p{42, 3.14};
    EXPECT_EQ(42, p.first);
    EXPECT_DOUBLE_EQ(3.14, p.second);
}
