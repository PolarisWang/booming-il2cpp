/// gc_bit_utils_test — GC bit utility function unit tests (GoogleTest).
///
/// Tests:
///   1. GcCtz64 on zero, powers of two, and all-bits-set
///   2. GcPopCount64 on zero, all-bits-set, and mixed patterns
///   3. GcForEachSetBit calls visitor for each set bit
///   4. GcForEachZeroBit calls visitor for each zero bit

#include <cstdint>

#include "gc_bit_utils.h"

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

// ── Test 1: GcCtz64 ─────────────────────────────────────────────────

TEST(GcBitUtils, Ctz64_ZeroReturns64) {
    EXPECT_EQ(GcCtz64(0), 64);
}

TEST(GcBitUtils, Ctz64_PowersOfTwo) {
    for (int k = 0; k < 64; k++) {
        uint64_t v = 1ULL << k;
        EXPECT_EQ(GcCtz64(v), k) << "GcCtz64(1<<" << k << ")";
    }
}

TEST(GcBitUtils, Ctz64_AllBitsSet) {
    EXPECT_EQ(GcCtz64(0xFFFFFFFFFFFFFFFFULL), 0);
}

// ── Test 2: GcPopCount64 ────────────────────────────────────────────

TEST(GcBitUtils, PopCount64) {
    EXPECT_EQ(GcPopCount64(0), 0);
    EXPECT_EQ(GcPopCount64(0xFFFFFFFFFFFFFFFFULL), 64);
    EXPECT_EQ(GcPopCount64(0xAAAAAAAAAAAAAAAAULL), 32);
    EXPECT_EQ(GcPopCount64(0x1), 1);
    EXPECT_EQ(GcPopCount64(0x8000000000000000ULL), 1);
    EXPECT_EQ(GcPopCount64(0xFF00FF00FF00FF00ULL), 32);
}

// ── Test 3: GcForEachSetBit ─────────────────────────────────────────

TEST(GcBitUtils, ForEachSetBit_SingleBit) {
    int count = 0;
    GcForEachSetBit(0x1ULL << 42, [&](int) { count++; });
    EXPECT_EQ(count, 1);
}

TEST(GcBitUtils, ForEachSetBit_MultipleBits) {
    int count = 0;
    GcForEachSetBit(0xAAAAAAAAAAAAAAAAULL, [&](int) { count++; });
    EXPECT_EQ(count, 32);
}

TEST(GcBitUtils, ForEachSetBit_NoBits) {
    int count = 0;
    GcForEachSetBit(0ULL, [&](int) { count++; });
    EXPECT_EQ(count, 0);
}

TEST(GcBitUtils, ForEachSetBit_AllBits) {
    int count = 0;
    GcForEachSetBit(0xFFFFFFFFFFFFFFFFULL, [&](int) { count++; });
    EXPECT_EQ(count, 64);
}

// ── Test 4: GcForEachZeroBit ────────────────────────────────────────

TEST(GcBitUtils, ForEachZeroBit_OneZero) {
    int count = 0;
    GcForEachZeroBit(0xFFFFFFFFFFFFFFFEULL, [&](int) { count++; });
    EXPECT_EQ(count, 1);
}

TEST(GcBitUtils, ForEachZeroBit_HalfZero) {
    int count = 0;
    GcForEachZeroBit(0xAAAAAAAAAAAAAAAAULL, [&](int) { count++; });
    EXPECT_EQ(count, 32);
}

TEST(GcBitUtils, ForEachZeroBit_AllZero) {
    int count = 0;
    GcForEachZeroBit(0ULL, [&](int) { count++; });
    EXPECT_EQ(count, 64);
}
