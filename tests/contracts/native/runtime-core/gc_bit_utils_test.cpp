/// gc_bit_utils_test — GC bit utility function unit tests.
///
/// Tests:
///   1. GcCtz64 on zero, powers of two, and all-bits-set
///   2. GcPopCount64 on zero, all-bits-set, and mixed patterns
///   3. GcForEachSetBit calls visitor for each set bit
///   4. GcForEachZeroBit calls visitor for each zero bit

#include <cstdio>
#include <cstdint>

#include "gc_bit_utils.h"
#include "gc_test_macros.h"

using namespace chaos::il2cpp::runtime_core;

static int g_failures = 0;
static int g_tests    = 0;
static int g_sub      = 0;

// ── Test 1: GcCtz64 ─────────────────────────────────────────────────
void TestCtz64() {
    TEST("GcCtz64");

    GC_SUBTEST("zero returns 64");
    int r = GcCtz64(0);
    GC_CHECK(r == 64, "GcCtz64(0) == 64");

    GC_SUBTEST("powers of two");
    for (int k = 0; k < 64; k++) {
        uint64_t v = 1ULL << k;
        int ctz = GcCtz64(v);
        if (ctz != k) {
            char msg[64];
            std::snprintf(msg, sizeof(msg), "GcCtz64(1<<%d) == %d (expected %d)", k, ctz, k);
            GC_FAIL(msg);
            return;
        }
    }

    GC_SUBTEST("all bits set");
    int all = GcCtz64(0xFFFFFFFFFFFFFFFFULL);
    GC_CHECK(all == 0, "GcCtz64(all) == 0");
}

// ── Test 2: GcPopCount64 ────────────────────────────────────────────
void TestPopCount64() {
    TEST("GcPopCount64");

    GC_CHECK(GcPopCount64(0) == 0, "PopCount64(0) == 0");
    GC_CHECK(GcPopCount64(0xFFFFFFFFFFFFFFFFULL) == 64,
             "PopCount64(all) == 64");
    GC_CHECK(GcPopCount64(0xAAAAAAAAAAAAAAAAULL) == 32,
             "PopCount64(0xAA..AA) == 32");
    GC_CHECK(GcPopCount64(0x1) == 1, "PopCount64(0x1) == 1");
    GC_CHECK(GcPopCount64(0x8000000000000000ULL) == 1,
             "PopCount64(0x8000..) == 1");
    GC_CHECK(GcPopCount64(0xFF00FF00FF00FF00ULL) == 32,
             "PopCount64(mixed) == 32");
}

// ── Test 3: GcForEachSetBit ─────────────────────────────────────────
void TestForEachSetBit() {
    TEST("GcForEachSetBit");

    // Single bit set.
    {
        int count = 0;
        uint64_t word = 0x1ULL << 42;
        GcForEachSetBit(word, [&](int /*bit*/) { count++; });
        GC_CHECK(count == 1, "single set bit iterated once");
    }

    // Multiple bits.
    {
        int count = 0;
        uint64_t word = 0xAAAAAAAAAAAAAAAAULL;  // 32 bits set
        GcForEachSetBit(word, [&](int /*bit*/) { count++; });
        GC_CHECK(count == 32, "32 set bits iterated 32 times");
    }

    // No bits set.
    {
        int count = 0;
        GcForEachSetBit(0ULL, [&](int /*bit*/) { count++; });
        GC_CHECK(count == 0, "no bits set -> no iterations");
    }

    // All bits set.
    {
        int count = 0;
        GcForEachSetBit(0xFFFFFFFFFFFFFFFFULL, [&](int /*bit*/) { count++; });
        GC_CHECK(count == 64, "all 64 bits set -> 64 iterations");
    }
}

// ── Test 4: GcForEachZeroBit ────────────────────────────────────────
void TestForEachZeroBit() {
    TEST("GcForEachZeroBit");

    // One zero bit in all-ones.
    {
        int count = 0;
        uint64_t word = 0xFFFFFFFFFFFFFFFEULL;  // bit 0 = 0
        GcForEachZeroBit(word, [&](int /*bit*/) { count++; });
        GC_CHECK(count == 1, "one zero bit iterated once");
    }

    // Half bits zero (0xAAAAAAAAAAAAAAAA has 32 zeros).
    {
        int count = 0;
        GcForEachZeroBit(0xAAAAAAAAAAAAAAAAULL, [&](int /*bit*/) { count++; });
        GC_CHECK(count == 32, "32 zero bits iterated 32 times");
    }

    // All zeros.
    {
        int count = 0;
        GcForEachZeroBit(0ULL, [&](int /*bit*/) { count++; });
        GC_CHECK(count == 64, "all zeros -> 64 iterations");
    }
}

// ── Main ────────────────────────────────────────────────────────────
int main() {
    puts("GC bit utility tests");
    puts("════════════════════\n");

    TestCtz64();
    TestPopCount64();
    TestForEachSetBit();
    TestForEachZeroBit();

    printf("\nResults: %d tests, %d failures\n", g_tests, g_failures);
    return g_failures > 0 ? 1 : 0;
}
