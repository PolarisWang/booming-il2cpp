/// gc_mark_bitmap_test — GcMarkBitmap unit tests (GoogleTest).

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "gc_mark_bitmap.h"
#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

namespace {

GcMarkBitmap CreateBitmap(size_t byte_count) {
    auto* data = static_cast<unsigned char*>(std::calloc(1, byte_count + 16));
    std::memset(data + byte_count, 0xCD, 16);
    return GcMarkBitmap(data, byte_count);
}

void FreeBitmap(GcMarkBitmap& bm) {
    std::free(const_cast<uint64_t*>(bm.Words()));
}

} // anonymous namespace

TEST(GcMarkBitmapTest, MarkSingleSlot) {
    auto bm = CreateBitmap(64);

    bm.MarkRange(0, 1);
    EXPECT_TRUE(bm.TestSlot(0));

    bm.MarkRange(100, 1);
    EXPECT_TRUE(bm.TestSlot(100));

    EXPECT_FALSE(bm.TestSlot(1));
    EXPECT_FALSE(bm.TestSlot(99));
    EXPECT_FALSE(bm.TestSlot(101));

    FreeBitmap(bm);
}

TEST(GcMarkBitmapTest, MarkRange) {
    auto bm = CreateBitmap(64);

    bm.MarkRange(5, 10);
    for (int i = 5; i < 15; i++) {
        EXPECT_TRUE(bm.TestSlot(static_cast<size_t>(i)));
    }
    EXPECT_FALSE(bm.TestSlot(4));
    EXPECT_FALSE(bm.TestSlot(15));

    FreeBitmap(bm);
}

TEST(GcMarkBitmapTest, TestSlotBeforeMark) {
    auto bm = CreateBitmap(64);

    EXPECT_FALSE(bm.TestSlot(0));
    EXPECT_FALSE(bm.TestSlot(200));
    EXPECT_FALSE(bm.TestSlot(511));

    FreeBitmap(bm);
}

TEST(GcMarkBitmapTest, ClearAfterMark) {
    auto bm = CreateBitmap(64);

    bm.MarkRange(10, 20);
    bm.Clear();

    for (int i = 0; i < 30; i++) {
        EXPECT_FALSE(bm.TestSlot(static_cast<size_t>(i)));
    }

    FreeBitmap(bm);
}

TEST(GcMarkBitmapTest, AnySet) {
    auto bm = CreateBitmap(64);

    EXPECT_FALSE(bm.AnySet());

    bm.MarkRange(0, 1);
    EXPECT_TRUE(bm.AnySet());

    bm.Clear();
    EXPECT_FALSE(bm.AnySet());

    FreeBitmap(bm);
}

TEST(GcMarkBitmapTest, Accessors) {
    auto bm = CreateBitmap(128);

    EXPECT_EQ(bm.WordCount(), 128 / 8);
    EXPECT_EQ(bm.ByteCount(), 128u);
    EXPECT_EQ(bm.SlotCapacity(), 128u * 8);

    FreeBitmap(bm);
}
