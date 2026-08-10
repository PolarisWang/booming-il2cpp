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
// Uses real regions from RegionManager so the region→gen skewed table
// (g_region_to_gen) is populated and SetRegionGen/GetRegionGen are
// deterministic.
void TestK2bDoubleArgBarrier() {
    TEST("K2b GenAwareBarrier");

    // Create a young (nursery) region and an old (tenured) region so
    // AllocateRegion sets their region gen (young=0 / old=2) in the table.
    Region* young = RegionManager::Instance().AllocateRegion(
        RegionKind::REGION_NURSERY, 64 * 1024);
    Region* old = RegionManager::Instance().AllocateRegion(
        RegionKind::REGION_TENURED, 64 * 1024);
    if (young == nullptr || old == nullptr) { GC_FAIL("region alloc failed"); return; }
    uintptr_t young_base = reinterpret_cast<uintptr_t>(young->begin);
    uintptr_t old_base   = reinterpret_cast<uintptr_t>(old->begin);
    auto young_obj = reinterpret_cast<void*>(young_base + 64);
    auto old_obj   = reinterpret_cast<void*>(old_base + 64);

    // Register both region ranges with the card table so DirtyCard finds the
    // L2 segments (AllocateRegion does not register card coverage itself).
    GcRegisterHeapRange(young_base, young_base + young->end - young->begin);
    GcRegisterHeapRange(old_base,   old_base   + old->end   - old->begin);

    // Sanity: region→gen table reflects young vs old.
    GC_CHECK(GetRegionGen(young_base) == kRegionGenYoung, "nursery region is young(0)");
    GC_CHECK(GetRegionGen(old_base)   == kRegionGenOld,   "tenured region is old(2)");

    // (1) young dst -> no card regardless of ref (young GC scans whole nursery).
    ClearAllCards();
    chaos_gc_dirty_card_dst_ref(young_obj, old_obj);   // dst gen0
    GC_CHECK(!IsDirty(young_obj), "K2b: gen0 dest skips card (always scanned)");

    // (2) old dst + young ref -> marks card (old→young cross-gen ref).
    ClearAllCards();
    chaos_gc_dirty_card_dst_ref(old_obj, young_obj);   // ref.gen 0 < dst.gen 2
    GC_CHECK(IsDirty(old_obj), "K2b: old->young marks card");

    // (3) old dst + old ref (same/mature gen) -> no card.
    ClearAllCards();
    chaos_gc_dirty_card_dst_ref(old_obj, reinterpret_cast<void*>(old_base + 4096));
    GC_CHECK(!IsDirty(old_obj), "K2b: same/mature generation skips card");

    // (4) ref == null -> no card.
    ClearAllCards();
    chaos_gc_dirty_card_dst_ref(old_obj, nullptr);
    GC_CHECK(!IsDirty(old_obj), "K2b: null ref skips card");

    RegionManager::Instance().FreeRegion(young->id);
    RegionManager::Instance().FreeRegion(old->id);
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

    printf("\nResults: %d tests, %d failures\n", g_tests, g_failures);
    return g_failures > 0 ? 1 : 0;
}
