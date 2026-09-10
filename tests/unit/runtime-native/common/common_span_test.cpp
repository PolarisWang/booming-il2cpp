#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/span.h>

using namespace chaos::il2cpp::common;

TEST(CommonSpan, SpanRuntimeEntryDefaults) {
    SpanRuntimeEntry entry;
    EXPECT_EQ(SpanStorageKind::kArray, entry.storage_kind);
    EXPECT_EQ(0, entry.owner_handle);
    EXPECT_EQ(0, entry.data_handle);
    EXPECT_EQ(0, entry.start);
    EXPECT_EQ(0, entry.length);
    EXPECT_FALSE(entry.read_only);
}

TEST(CommonSpan, SpanRuntimeEntryCustomValues) {
    SpanRuntimeEntry entry;
    entry.storage_kind = SpanStorageKind::kRawInt32;
    entry.owner_handle = 123;
    entry.data_handle = 456;
    entry.start = 10;
    entry.length = 100;
    entry.read_only = true;

    EXPECT_EQ(SpanStorageKind::kRawInt32, entry.storage_kind);
    EXPECT_EQ(123, entry.owner_handle);
    EXPECT_EQ(456, entry.data_handle);
    EXPECT_EQ(10, entry.start);
    EXPECT_EQ(100, entry.length);
    EXPECT_TRUE(entry.read_only);
}

TEST(CommonSpan, MemoryRuntimeEntryDefaults) {
    MemoryRuntimeEntry entry;
    EXPECT_EQ(0, entry.array_handle);
    EXPECT_EQ(0, entry.start);
    EXPECT_EQ(0, entry.length);
}

TEST(CommonSpan, MemoryRuntimeEntryCustomValues) {
    MemoryRuntimeEntry entry;
    entry.array_handle = 789;
    entry.start = 5;
    entry.length = 50;

    EXPECT_EQ(789, entry.array_handle);
    EXPECT_EQ(5, entry.start);
    EXPECT_EQ(50, entry.length);
}

TEST(CommonSpan, SpanStorageKindValues) {
    EXPECT_EQ(1, static_cast<int>(SpanStorageKind::kArray));
    EXPECT_EQ(2, static_cast<int>(SpanStorageKind::kRawInt32));
}
