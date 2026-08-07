/// common_checked_conv_test_ext — Extended overflow/death tests for checked_conv.h.
///
/// Complements common_checked_conv_test.cpp which covers valid-range conversions.
/// These tests verify the CHAOS_IL2CPP_FAIL paths when values exceed the target range.

#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/checked_conv.h>
#include <limits>

using namespace chaos::il2cpp::common;

// ═══════════════════════════════════════════════════════════════════════════
// checked_conv_ovf_i1 — overflow death tests (out of [-128, 127])
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonCheckedConvExt, I1_AboveMax) {
    EXPECT_DEATH(checked_conv_ovf_i1(128), "");
}

TEST(CommonCheckedConvExt, I1_BelowMin) {
    EXPECT_DEATH(checked_conv_ovf_i1(-129), "");
}

TEST(CommonCheckedConvExt, I1_AtBoundaries) {
    // These are the values at the edge — should NOT die
    EXPECT_EQ(127, checked_conv_ovf_i1(127));
    EXPECT_EQ(-128, checked_conv_ovf_i1(-128));
}

// ═══════════════════════════════════════════════════════════════════════════
// checked_conv_ovf_u1 — overflow death tests (out of [0, 255])
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonCheckedConvExt, U1_AboveMax) {
    EXPECT_DEATH(checked_conv_ovf_u1(256), "");
}

TEST(CommonCheckedConvExt, U1_BelowMin) {
    EXPECT_DEATH(checked_conv_ovf_u1(-1), "");
}

TEST(CommonCheckedConvExt, U1_AtBoundaries) {
    EXPECT_EQ(255, checked_conv_ovf_u1(255));
    EXPECT_EQ(0, checked_conv_ovf_u1(0));
}

// ═══════════════════════════════════════════════════════════════════════════
// checked_conv_ovf_i2 — overflow death tests (out of [-32768, 32767])
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonCheckedConvExt, I2_AboveMax) {
    EXPECT_DEATH(checked_conv_ovf_i2(32768), "");
}

TEST(CommonCheckedConvExt, I2_BelowMin) {
    EXPECT_DEATH(checked_conv_ovf_i2(-32769), "");
}

TEST(CommonCheckedConvExt, I2_AtBoundaries) {
    EXPECT_EQ(32767, checked_conv_ovf_i2(32767));
    EXPECT_EQ(-32768, checked_conv_ovf_i2(-32768));
}

// ═══════════════════════════════════════════════════════════════════════════
// checked_conv_ovf_u2 — overflow death tests (out of [0, 65535])
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonCheckedConvExt, U2_AboveMax) {
    EXPECT_DEATH(checked_conv_ovf_u2(65536), "");
}

TEST(CommonCheckedConvExt, U2_BelowMin) {
    EXPECT_DEATH(checked_conv_ovf_u2(-1), "");
}

TEST(CommonCheckedConvExt, U2_AtBoundaries) {
    EXPECT_EQ(65535, checked_conv_ovf_u2(65535));
    EXPECT_EQ(0, checked_conv_ovf_u2(0));
}
