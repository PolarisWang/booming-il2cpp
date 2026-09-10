#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/checked_conv.h>

using chaos::il2cpp::common::checked_conv_ovf_i1;
using chaos::il2cpp::common::checked_conv_ovf_u1;
using chaos::il2cpp::common::checked_conv_ovf_i2;
using chaos::il2cpp::common::checked_conv_ovf_u2;

TEST(CommonCheckedConv, I1_ValidRange) {
    auto r = checked_conv_ovf_i1(0);    EXPECT_EQ(0, static_cast<int>(r));
    r = checked_conv_ovf_i1(42);        EXPECT_EQ(42, static_cast<int>(r));
    r = checked_conv_ovf_i1(-1);        EXPECT_EQ(-1, static_cast<int>(r));
    r = checked_conv_ovf_i1(127);       EXPECT_EQ(127, static_cast<int>(r));
    r = checked_conv_ovf_i1(-128);      EXPECT_EQ(-128, static_cast<int>(r));
}

TEST(CommonCheckedConv, U1_ValidRange) {
    auto r = checked_conv_ovf_u1(0);    EXPECT_EQ(0, static_cast<int>(r));
    r = checked_conv_ovf_u1(255);       EXPECT_EQ(255, static_cast<int>(r));
    r = checked_conv_ovf_u1(128);       EXPECT_EQ(128, static_cast<int>(r));
    r = checked_conv_ovf_u1(1);         EXPECT_EQ(1, static_cast<int>(r));
}

TEST(CommonCheckedConv, I2_ValidRange) {
    auto r = checked_conv_ovf_i2(0);    EXPECT_EQ(0, static_cast<int>(r));
    r = checked_conv_ovf_i2(32767);     EXPECT_EQ(32767, static_cast<int>(r));
    r = checked_conv_ovf_i2(-32768);    EXPECT_EQ(-32768, static_cast<int>(r));
    r = checked_conv_ovf_i2(-1);        EXPECT_EQ(-1, static_cast<int>(r));
}

TEST(CommonCheckedConv, U2_ValidRange) {
    auto r = checked_conv_ovf_u2(0);    EXPECT_EQ(0, static_cast<int>(r));
    r = checked_conv_ovf_u2(65535);     EXPECT_EQ(65535, static_cast<int>(r));
    r = checked_conv_ovf_u2(100);       EXPECT_EQ(100, static_cast<int>(r));
}
