/// gc_card_table_ext_test — Card table edge case tests (GoogleTest).
///
/// Tests:
///   1. GcRegisterHeapRange overlapping ranges
///   2. DirtyCard at card boundary
///   3. ClearAllCards with 100+ dirty cards
///   4. ScanDirtyCards empty range — callback count = 0
///   5. ScanDirtyCards partially dirty — only dirty cards scanned

#include <cstdlib>
#include <cstring>

#include <chaos/native_types.h>
#include "gc_card_table.h"

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

// ── Test 1: Overlapping heap ranges ─────────────────────────────────

TEST(GcCardTableExt, OverlappingRange) {
    void* heap = std::calloc(1, 2 * 1024 * 1024);
    ASSERT_NE(heap, nullptr);

    GcSetHeapBase(heap);
    auto base = reinterpret_cast<uintptr_t>(heap);

    GcRegisterHeapRange(reinterpret_cast<uintptr_t>(heap), base + 1024 * 1024);
    GcRegisterHeapRange(reinterpret_cast<uintptr_t>(heap), base + 512 * 1024);

    // Just verify no crash.
    std::free(heap);
}

// ── Test 2: DirtyCard at card boundary ──────────────────────────────

TEST(GcCardTableExt, DirtyAtBoundary) {
    void* heap = std::calloc(1, 65536);
    ASSERT_NE(heap, nullptr);
    GcSetHeapBase(heap);
    auto base = reinterpret_cast<uintptr_t>(heap);

    void* obj_begin = heap;
    DirtyCard(obj_begin);
    bool dirty1 = IsDirty(obj_begin);
    ClearCard(obj_begin);

    void* obj_end = reinterpret_cast<void*>(base + 65500);
    DirtyCard(obj_end);
    bool dirty2 = IsDirty(obj_end);

    EXPECT_TRUE(dirty1) << "first card address dirty";
    EXPECT_TRUE(dirty2) << "last card address dirty";

    ClearAllCards();
    std::free(heap);
}

// ── Test 3: ClearAllCards with 100+ dirty cards ─────────────────────

TEST(GcCardTableExt, ClearAllCardsMany) {
    void* heap = std::calloc(1, 1024 * 1024);
    ASSERT_NE(heap, nullptr);
    GcSetHeapBase(heap);
    auto base = reinterpret_cast<uintptr_t>(heap);

    for (size_t offset = 0; offset < 200; offset++) {
        void* obj = reinterpret_cast<void*>(base + offset * 4096 + 64);
        DirtyCard(obj);
    }

    ClearAllCards();

    bool all_clean = true;
    for (size_t offset = 0; offset < 200; offset++) {
        void* obj = reinterpret_cast<void*>(base + offset * 4096 + 64);
        if (IsDirty(obj)) { all_clean = false; break; }
    }
    EXPECT_TRUE(all_clean) << "all 200 cards clean after ClearAllCards";

    std::free(heap);
}

// ── Test 4: ScanDirtyCards empty range ──────────────────────────────

TEST(GcCardTableExt, ScanDirtyEmpty) {
    void* heap = std::calloc(1, 65536);
    ASSERT_NE(heap, nullptr);
    GcSetHeapBase(heap);
    auto base = reinterpret_cast<uintptr_t>(heap);

    int count = 0;
    ScanDirtyCards(base, base + 65536, [&](uintptr_t, uintptr_t, uintptr_t) {
        count++;
    });
    EXPECT_EQ(count, 0) << "no dirty cards found in clean range";

    std::free(heap);
}

// ── Test 5: ScanDirtyCards partial ──────────────────────────────────

TEST(GcCardTableExt, ScanDirtyPartial) {
    void* heap = std::calloc(1, 65536);
    ASSERT_NE(heap, nullptr);
    GcSetHeapBase(heap);
    auto base = reinterpret_cast<uintptr_t>(heap);

    for (size_t offset = 0; offset < 10; offset += 2) {
        void* obj = reinterpret_cast<void*>(base + offset * 4096 + 64);
        DirtyCard(obj);
    }

    int count = 0;
    ScanDirtyCards(base, base + 65536, [&](uintptr_t, uintptr_t, uintptr_t) {
        count++;
    });
    EXPECT_GT(count, 0) << "dirty cards found in partially dirty range";
    EXPECT_LE(count, 10) << "no more than 10 dirty cards (every other of 10)";

    ClearAllCards();
    std::free(heap);
}
