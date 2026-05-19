/// gc_mark_bitmap_test — GcMarkBitmap unit tests.
///
/// Tests:
///   1. MarkRange single slot + TestSlot
///   2. MarkRange multiple slots
///   3. TestSlot before mark returns false
///   4. Clear after mark clears all
///   5. AnySet on empty / after mark / after clear
///   6. WordCount / ByteCount / SlotCapacity accessors

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "gc_mark_bitmap.h"
#include "gc_test_macros.h"

using namespace chaos::il2cpp::runtime_core;

static int g_failures = 0;
static int g_tests    = 0;
static int g_sub      = 0;

// Helper: create a GcMarkBitmap with inline storage.
// The poison zone (last 16 bytes after the bitmap) must be 0xCD.
static GcMarkBitmap CreateBitmap(size_t byte_count) {
    // Allocate extra 16 bytes for poison zone AFTER the bitmap data.
    auto* data = static_cast<unsigned char*>(std::calloc(1, byte_count + 16));
    // Initialize poison zone (16 bytes after bitmap).
    std::memset(data + byte_count, 0xCD, 16);
    return GcMarkBitmap(data, byte_count);
}

static void FreeBitmap(GcMarkBitmap& bm) {
    std::free(const_cast<uint64_t*>(bm.Words()));  // Words() points to the same buffer as data_
}

// ── Test 1: Mark single slot ────────────────────────────────────────
void TestMarkSingleSlot() {
    TEST("MarkSingleSlot");

    // 64 bytes = 512 slots
    auto bm = CreateBitmap(64);

    bm.MarkRange(0, 1);
    GC_CHECK(bm.TestSlot(0), "slot 0 is marked after MarkRange(0,1)");

    bm.MarkRange(100, 1);
    GC_CHECK(bm.TestSlot(100), "slot 100 is marked");

    // Adjacent slots should not be marked.
    GC_CHECK(!bm.TestSlot(1), "slot 1 is not marked");
    GC_CHECK(!bm.TestSlot(99), "slot 99 is not marked");
    GC_CHECK(!bm.TestSlot(101), "slot 101 is not marked");

    FreeBitmap(bm);
}

// ── Test 2: MarkRange multiple slots ────────────────────────────────
void TestMarkRange() {
    TEST("MarkRange multiple slots");

    auto bm = CreateBitmap(64);

    bm.MarkRange(5, 10);
    for (int i = 5; i < 15; i++) {
        if (!bm.TestSlot(static_cast<size_t>(i))) {
            GC_FAIL("slot in range not marked");
            FreeBitmap(bm);
            return;
        }
    }
    GC_CHECK(!bm.TestSlot(4), "slot before range not marked");
    GC_CHECK(!bm.TestSlot(15), "slot after range not marked");

    FreeBitmap(bm);
}

// ── Test 3: TestSlot before mark ────────────────────────────────────
void TestBeforeMark() {
    TEST("TestSlot before mark");

    auto bm = CreateBitmap(64);

    GC_CHECK(!bm.TestSlot(0), "slot 0 not marked initially");
    GC_CHECK(!bm.TestSlot(200), "slot 200 not marked initially");
    GC_CHECK(!bm.TestSlot(511), "slot 511 not marked initially");

    FreeBitmap(bm);
}

// ── Test 4: Clear after mark ────────────────────────────────────────
void TestClearAfterMark() {
    TEST("Clear after mark");

    auto bm = CreateBitmap(64);

    bm.MarkRange(10, 20);
    bm.Clear();

    for (int i = 0; i < 30; i++) {
        if (bm.TestSlot(static_cast<size_t>(i))) {
            GC_FAIL("slot still marked after Clear");
            FreeBitmap(bm);
            return;
        }
    }

    FreeBitmap(bm);
}

// ── Test 5: AnySet ─────────────────────────────────────────────────
void TestAnySet() {
    TEST("AnySet");

    auto bm = CreateBitmap(64);

    GC_CHECK(!bm.AnySet(), "AnySet false on empty bitmap");

    bm.MarkRange(0, 1);
    GC_CHECK(bm.AnySet(), "AnySet true after mark");

    bm.Clear();
    GC_CHECK(!bm.AnySet(), "AnySet false after clear");

    FreeBitmap(bm);
}

// ── Test 6: Accessors ───────────────────────────────────────────────
void TestAccessors() {
    TEST("WordCount / ByteCount / SlotCapacity");

    auto bm = CreateBitmap(128);

    GC_CHECK(bm.WordCount() == 128 / 8, "WordCount == 16");
    GC_CHECK(bm.ByteCount() == 128, "ByteCount == 128");
    GC_CHECK(bm.SlotCapacity() == 128 * 8, "SlotCapacity == 1024");

    FreeBitmap(bm);
}

// ── Main ────────────────────────────────────────────────────────────
int main() {
    puts("GcMarkBitmap unit tests");
    puts("═══════════════════════\n");

    TestMarkSingleSlot();
    TestMarkRange();
    TestBeforeMark();
    TestClearAfterMark();
    TestAnySet();
    TestAccessors();

    printf("\nResults: %d tests, %d failures\n", g_tests, g_failures);
    return g_failures > 0 ? 1 : 0;
}
