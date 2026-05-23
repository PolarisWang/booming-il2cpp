/// common_status_test — Unit tests for status.h Status enum.

#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/status.h>

using namespace chaos::il2cpp::common;

// ═══════════════════════════════════════════════════════════════════════════
// Status values
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonStatus, OkValue) {
    EXPECT_EQ(0, static_cast<int>(Status::kOk));
}

TEST(CommonStatus, KnownValues) {
    EXPECT_EQ(1, static_cast<int>(Status::kInvalidArgument));
    EXPECT_EQ(2, static_cast<int>(Status::kInvalidState));
    EXPECT_EQ(3, static_cast<int>(Status::kNotFound));
    EXPECT_EQ(4, static_cast<int>(Status::kNotSupported));
    EXPECT_EQ(5, static_cast<int>(Status::kManagedException));
    EXPECT_EQ(6, static_cast<int>(Status::kInternalError));
    EXPECT_EQ(7, static_cast<int>(Status::kMetadataResolutionFailed));
    EXPECT_EQ(8, static_cast<int>(Status::kClassInitFailed));
    EXPECT_EQ(9, static_cast<int>(Status::kRuntimeCallFailed));
}

// ═══════════════════════════════════════════════════════════════════════════
// is_ok / is_failure
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonStatus, IsOk) {
    EXPECT_TRUE(is_ok(Status::kOk));
    EXPECT_FALSE(is_ok(Status::kInvalidArgument));
    EXPECT_FALSE(is_ok(Status::kInternalError));
}

TEST(CommonStatus, IsFailure) {
    EXPECT_FALSE(is_failure(Status::kOk));
    EXPECT_TRUE(is_failure(Status::kInvalidArgument));
    EXPECT_TRUE(is_failure(Status::kInternalError));
}

// ═══════════════════════════════════════════════════════════════════════════
// is_ok / is_failure constexpr
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonStatus, Constexpr) {
    constexpr bool ok = is_ok(Status::kOk);
    constexpr bool fail = is_failure(Status::kNotFound);
    EXPECT_TRUE(ok);
    EXPECT_TRUE(fail);
}
