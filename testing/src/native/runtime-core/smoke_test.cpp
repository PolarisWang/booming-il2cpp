#include <gtest/gtest.h>

// Smoke test — verifies gtest builds and links correctly.
TEST(Smoke, BuildPipelineWorks)
{
    EXPECT_EQ(1 + 1, 2);
}

TEST(Smoke, GtestAssertionsWork)
{
    EXPECT_TRUE(true);
    EXPECT_FALSE(false);
    EXPECT_STREQ("hello", "hello");
}
