#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/format.h>

using namespace chaos::il2cpp::common;

TEST(CommonFormat, FormatInt32) {
    auto s = format_int32(42);
    EXPECT_EQ("42", s);

    s = format_int32(-1);
    EXPECT_EQ("-1", s);

    s = format_int32(0);
    EXPECT_EQ("0", s);
}

TEST(CommonFormat, FormatFloat) {
    auto s = format_float(3.14f);
    // "F1" format: one decimal place
    EXPECT_EQ("3.1", s);
}

TEST(CommonFormat, FormatDouble) {
    auto s = format_double(2.71828);
    EXPECT_EQ("2.7", s);
}

TEST(CommonFormat, FormatString) {
    auto s = format_string("hello {} world", 42);
    EXPECT_EQ("hello 42 world", s);
}

TEST(CommonFormat, FormatStringMultipleArgs) {
    auto s = format_string("{} + {} = {}", 1, 2, 3);
    EXPECT_EQ("1 + 2 = 3", s);
}

TEST(CommonFormat, FormatAppendTo) {
    CHAOS_IL2CPP_STRING result;
    format_append_to(result, "{}", 1);
    format_append_to(result, "{}", 2);
    format_append_to(result, "{}", 3);
    EXPECT_EQ("123", result);
}

TEST(CommonFormat, FormatStringHex) {
    auto s = format_string("{:x}", 255);
    EXPECT_EQ("ff", s);
}
