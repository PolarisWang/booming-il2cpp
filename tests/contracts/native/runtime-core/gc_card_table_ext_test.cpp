/// gc_card_table_ext_test — Card table edge case tests.
///
/// Extends gc_region_test.cpp card table coverage with:
///   1. GcRegisterHeapRange overlapping ranges
///   2. DirtyCard at card boundary
///   3. ClearAllCards with 100+ dirty cards
///   4. ScanDirtyCards empty range — callback count = 0
///   5. ScanDirtyCards partially dirty — only dirty cards scanned

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <atomic>
#include <thread>

#include <chaos/native_types.h>
#include "gc_card_table.h"
#include "gc_test_macros.h"

using namespace chaos::il2cpp::runtime_core;

static int g_failures = 0;
static int g_tests    = 0;
static int g_sub      = 0;

// ── Test 1: Overlapping heap ranges ─────────────────────────────────
void TestOverlappingRange() {
    TEST("OverlappingRange");

    // Allocate a heap region and register overlapping ranges.
    void* heap = std::calloc(1, 2 * 1024 * 1024);
    if (!heap) { GC_FAIL("heap alloc failed"); return; }

    GcSetHeapBase(heap);
    auto base = reinterpret_cast<uintptr_t>(heap);

    // Register a range and a sub-range (should not double-free).
    GcRegisterHeapRange(reinterpret_cast<uintptr_t>(heap), base + 1024 * 1024);
    GcRegisterHeapRange(reinterpret_cast<uintptr_t>(heap), base + 512 * 1024);

    // Just verify no crash — overlapping ranges should be handled gracefully.
    GC_CHECK(true, "overlapping heap ranges did not crash");

    std::free(heap);
}

// ── Test 2: DirtyCard at card boundary ──────────────────────────────
void TestDirtyAtBoundary() {
    TEST("DirtyAtBoundary");

    void* heap = std::calloc(1, 65536);
    if (!heap) { GC_FAIL("heap alloc failed"); return; }
    GcSetHeapBase(heap);

    auto base = reinterpret_cast<uintptr_t>(heap);

    // Dirty a card at the very beginning of the range.
    void* obj_begin = heap;
    DirtyCard(obj_begin);
    bool dirty1 = IsDirty(obj_begin);
    ClearCard(obj_begin);

    // Dirty a card near the end.
    void* obj_end = reinterpret_cast<void*>(base + 65500);
    DirtyCard(obj_end);
    bool dirty2 = IsDirty(obj_end);

    GC_CHECK(dirty1, "first card address dirty");
    GC_CHECK(dirty2, "last card address dirty");

    ClearAllCards();
    std::free(heap);
}

// ── Test 3: ClearAllCards with 100+ dirty cards ─────────────────────
void TestClearAllCards() {
    TEST("ClearAllCards");

    void* heap = std::calloc(1, 1024 * 1024);
    if (!heap) { GC_FAIL("heap alloc failed"); return; }
    GcSetHeapBase(heap);

    auto base = reinterpret_cast<uintptr_t>(heap);

    // Dirty 200 cards.
    for (size_t offset = 0; offset < 200; offset++) {
        void* obj = reinterpret_cast<void*>(base + offset * 4096 + 64);
        DirtyCard(obj);
    }

    ClearAllCards();

    // Verify all clean.
    bool all_clean = true;
    for (size_t offset = 0; offset < 200; offset++) {
        void* obj = reinterpret_cast<void*>(base + offset * 4096 + 64);
        if (IsDirty(obj)) { all_clean = false; break; }
    }
    GC_CHECK(all_clean, "all 200 cards clean after ClearAllCards");

    std::free(heap);
}

// ── Test 4: ScanDirtyCards empty range ──────────────────────────────
void TestScanDirtyEmpty() {
    TEST("ScanDirtyEmpty range");

    void* heap = std::calloc(1, 65536);
    if (!heap) { GC_FAIL("heap alloc failed"); return; }
    GcSetHeapBase(heap);

    auto base = reinterpret_cast<uintptr_t>(heap);

    int count = 0;
    ScanDirtyCards(base, base + 65536, [&](uintptr_t, uintptr_t, uintptr_t) {
        count++;
    });
    GC_CHECK(count == 0, "no dirty cards found in clean range");

    std::free(heap);
}

// ── Test 5: ScanDirtyCards partial ──────────────────────────────────
void TestScanDirtyPartial() {
    TEST("ScanDirtyPartial");

    void* heap = std::calloc(1, 65536);
    if (!heap) { GC_FAIL("heap alloc failed"); return; }
    GcSetHeapBase(heap);

    auto base = reinterpret_cast<uintptr_t>(heap);

    // Dirty every other card.
    for (size_t offset = 0; offset < 10; offset += 2) {
        void* obj = reinterpret_cast<void*>(base + offset * 4096 + 64);
        DirtyCard(obj);
    }

    int count = 0;
    ScanDirtyCards(base, base + 65536, [&](uintptr_t, uintptr_t, uintptr_t) {
        count++;
    });
    GC_CHECK(count > 0, "dirty cards found in partially dirty range");
    GC_CHECK(count <= 10, "no more than 10 dirty cards (every other of 10)");

    ClearAllCards();
    std::free(heap);
}

// ── Test 6: GC-K2b generation-aware double-arg write barrier ─────────
// Verifies chaos_gc_dirty_card_dst_ref's three short-circuits (faithful to
// CoreCLR region write barrier): gen0 dst skips; ref==null skips;
// same/mature ref.gen >= dst.gen skips; only old->young marks.
// Deterministic: use a calloc heap (like the other tests) and drive the
// region→gen table explicitly via SetRegionGen on that heap, after growing
// the table once with a real region so g_region_to_gen is non-null.
void TestK2bDoubleArgBarrier() {
    TEST("K2b GenAwareBarrier");

    // Grow g_region_to_gen once via a real region alloc, then free it so the
    // table is non-null and SetRegionGen below is deterministic.
    Region* grow = RegionManager::Instance().AllocateRegion(RegionKind::REGION_TENURED, 64 * 1024);
    if (grow == nullptr) { GC_FAIL("grow region alloc failed"); return; }
    auto grow_base = reinterpret_cast<uintptr_t>(grow->begin);
    RegionManager::Instance().FreeRegion(grow->id);
    if (GetRegionGen(grow_base) != kRegionGenOld) { GC_FAIL("grow region table non-null"); return; }

    // Use a large calloc heap so dst and ref land in DIFFERENT 4MB regions
    // (the region→gen table is keyed by addr>>22), letting SetRegionGen assign
    // each its own generation deterministically.
    void* heap = std::calloc(1, 8 * 1024 * 1024);
    if (!heap) { GC_FAIL("heap alloc failed"); std::free(heap); return; }
    GcSetHeapBase(heap);
    GcRegisterHeapRange(reinterpret_cast<uintptr_t>(heap),
                        reinterpret_cast<uintptr_t>(heap) + 8 * 1024 * 1024);
    auto base = reinterpret_cast<uintptr_t>(heap);
    auto dst  = reinterpret_cast<void*>(base + 64);               // region cell 0
    auto ref  = reinterpret_cast<void*>(base + 4 * 1024 * 1024 + 64);  // region cell 1

    // (1) dst gen0 (young) -> no card regardless of ref.
    ClearAllCards();
    SetRegionGen(base + 64,  kRegionGenYoung);
    SetRegionGen(base + 4 * 1024 * 1024 + 64, kRegionGenOld);
    chaos_gc_dirty_card_dst_ref(dst, ref);
    GC_CHECK(!IsDirty(dst), "K2b: gen0 dest skips card (always scanned)");

    // (2) old dst + young ref (ref.gen 0 < dst.gen 2) -> marks card.
    ClearAllCards();
    SetRegionGen(base + 64,  kRegionGenOld);
    SetRegionGen(base + 4 * 1024 * 1024 + 64, kRegionGenYoung);
    chaos_gc_dirty_card_dst_ref(dst, ref);
    GC_CHECK(IsDirty(dst), "K2b: old->young marks card");

    // (3) old dst + old ref (same/mature gen) -> no card.
    ClearAllCards();
    SetRegionGen(base + 64,  kRegionGenOld);
    SetRegionGen(base + 4 * 1024 * 1024 + 64, kRegionGenOld);
    chaos_gc_dirty_card_dst_ref(dst, ref);
    GC_CHECK(!IsDirty(dst), "K2b: same/mature generation skips card");

    // (4) ref == null -> no card.
    ClearAllCards();
    SetRegionGen(base + 64, kRegionGenOld);
    chaos_gc_dirty_card_dst_ref(dst, nullptr);
    GC_CHECK(!IsDirty(dst), "K2b: null ref skips card");

    ClearAllCards();
    std::free(heap);
}

// ── Test 6 (P1-A4): CardBundleSet concurrent — no lost bundle bit ──────
// Multiple threads set DIFFERENT bundle bits in the SAME byte.  A non-atomic
// read-modify-write would lose bits → dirty card skipped in ScanDirtyCards
// fast-path → cross-gen edge dropped.  Atomic RMW must set all bits.
static void TestCardBundleConcurrent() {
    TEST("CardBundleConcurrentNoLostBit");

    // Ensure a card table + bundle exist via a heap range registration.
    void* heap = std::calloc(1, 16 * 1024 * 1024);
    if (!heap) { GC_FAIL("heap alloc failed"); return; }
    GcSetHeapBase(heap);
    GcRegisterHeapRange(reinterpret_cast<uintptr_t>(heap),
                        reinterpret_cast<uintptr_t>(heap) + 16 * 1024 * 1024);

    // 8 distinct bundle bits that alias to bundle byte 0.  Concurrent sets must
    // all survive (no lost bit).
    constexpr int kBits = 8;
    std::atomic<bool> start{false};
    std::thread workers[kBits];
    for (int b = 0; b < kBits; ++b) {
        workers[b] = std::thread([b, &start]() {
            while (!start.load(std::memory_order_acquire)) {}
            CardBundleSet(static_cast<uintptr_t>(b));
        });
    }
    start.store(true, std::memory_order_release);
    for (int b = 0; b < kBits; ++b) workers[b].join();

    bool all_set = true;
    for (int b = 0; b < kBits; ++b) {
        if (!CardBundleTest(static_cast<uintptr_t>(b))) { all_set = false; break; }
    }
    if (!all_set) { GC_FAIL("lost bundle bit under concurrency"); }

    ClearAllCards();
    std::free(heap);
}

// ── Main ────────────────────────────────────────────────────────────
int main() {
    puts("Card table edge case tests");
    puts("══════════════════════════\n");

    TestOverlappingRange();
    TestDirtyAtBoundary();
    TestClearAllCards();
    TestScanDirtyEmpty();
    TestScanDirtyPartial();
    TestK2bDoubleArgBarrier();
    TestCardBundleConcurrent();

    printf("\nResults: %d tests, %d failures\n", g_tests, g_failures);
    return g_failures > 0 ? 1 : 0;
}
