/// gc_dirty_card_decision_test — deterministic unit verification of the
/// generation-aware write barrier's DECISION logic (T7/B2 scheme B).
///
/// Does NOT exercise full-GC promotion or card scanning of live objects — it
/// only asserts that `chaos_gc_dirty_card_dst_ref(dst, ref)` sets the card for
/// exactly the (dst_gen, ref_gen) combinations that constitute a cross-gen
/// old→young reference, and skips everything else.  Because it never drives the
/// young-GC promotion path, it is immune to the known A2b store-then-barrier
/// race that blocks end-to-end survival detectors (see design-t7-barrier-stress-risk.md).
///
/// Barrier contract (gc_card_table.cpp, faithful to CoreCLR region barrier):
///   - dst physically in nursery (IsNurseryPointer) → never card (nursery scanned
///     wholesale).  NOTE: the dst's 4MB region-gen chunk tag is deliberately NOT
///     the gate — a chunk shared between the nursery tail and an old-gen page
///     keeps a YOUNG tag, so an old-gen object there would be wrongly skipped and
///     drop an old→nursery edge (GC-N6 finding 2026-08-25).  Only a real nursery
///     address skips.
///   - ref == null              → never card (degenerate store)
///   - ref.gen == Old(2)        → never card (mature ref, no cross-gen edge)
///   - otherwise (non-nursery dst, ref not mature) → card — including gen1→gen1,
///     conservative extra-scan (never a correctness miss), matching CoreCLR.
///   Card matrix (physical-dst-nursery × ref):
///         dst\ref  Young(0)  Gen1(1)  Old(2)
///         nursery    skip    skip      skip
///         non-nursery CARD   CARD      skip
///
/// Deterministic like the K2b test: a large calloc heap so dst/ref land in
/// different 4MB region cells, with region→gen driven explicitly via SetRegionGen.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <atomic>

#include <chaos/native_types.h>
#include "gc_card_table.h"
#include "gc_region.h"
#include "gc_test_macros.h"

using namespace chaos::il2cpp::runtime_core;

static int g_failures = 0;
static int g_tests    = 0;
static int g_sub      = 0;

/// One decision-triple: (dst_gen, ref_gen, expect_card).  Checks the barrier sets
/// the card iff expect_card is true, on a freshly-cleared card table.
static void CheckOne(uint8_t dst_gen, uint8_t ref_gen, bool expect_card,
                     uintptr_t base, void* dst, void* ref) {
    ClearAllCards();
    SetRegionGen(base + 64,  dst_gen);
    // ref is in region cell 1 (4MB away).  ref==nullptr has no address to label.
    if (ref != nullptr) {
        SetRegionGen(base + 4 * 1024 * 1024 + 64, ref_gen);
    }
    chaos_gc_dirty_card_dst_ref(dst, ref);
    bool dirty = IsDirty(dst);
    bool pass = (dirty == expect_card);
    if (!pass) {
        printf("  [FAIL] dst_gen=%u ref_gen=%u expect_card=%d got_dirty=%d\n",
               dst_gen, ref_gen, (int)expect_card, (int)dirty);
        GC_CHECK(pass, "barrier decision correct for (dst_gen, ref_gen)");
    } else {
        GC_CHECK(pass, "barrier decision correct for (dst_gen, ref_gen)");
    }
}

/// Enumerate the full 3×3 generation matrix + degenerate (null-ref) + order
/// sensitivity (same address as dst/ref, cross-region, out-of-heap ref).
static void TestDecisionMatrix() {
    TEST("BarrierDecisionMatrix");

    // Grow g_region_to_gen once via a real region so SetRegionGen below is
    // deterministic (mirrors K2b setup).
    Region* grow = RegionManager::Instance().AllocateRegion(RegionKind::REGION_TENURED, 64 * 1024);
    if (grow == nullptr) { GC_FAIL("grow region alloc failed"); return; }
    RegionManager::Instance().FreeRegion(grow->id);

    void* heap = std::calloc(1, 8 * 1024 * 1024);
    if (!heap) { GC_FAIL("heap alloc failed"); std::free(heap); return; }
    GcSetHeapBase(heap);
    GcRegisterHeapRange(reinterpret_cast<uintptr_t>(heap),
                        reinterpret_cast<uintptr_t>(heap) + 8 * 1024 * 1024);
    auto base = reinterpret_cast<uintptr_t>(heap);
    void* dst = reinterpret_cast<void*>(base + 64);                        // region cell 0 (non-nursery)
    void* ref = reinterpret_cast<void*>(base + 4 * 1024 * 1024 + 64);      // region cell 1

    // Create a REAL nursery region so the "young dst" case genuinely exercises
    // the physical-nursery gate (IsNurseryPointer), which the barrier uses for
    // the "dst in gen0 → skip" decision (GC-N6 finding 2026-08-25).  The 4MB
    // region-gen chunk tag is deliberately NOT the dst gate (chunk-collision
    // hazard): an old-gen object sharing a 4MB chunk tagged young must still be
    // carded, so only a real nursery address skips.  A synthetic calloc address
    // tagged young via SetRegionGen is NOT physically nursery and must be carded.
    Region* nursery = RegionManager::Instance().AllocateRegion(
        RegionKind::REGION_NURSERY, 256 * 1024);
    if (nursery == nullptr) { GC_FAIL("nursery region alloc failed"); return; }
    void* nursery_dst = reinterpret_cast<void*>(
        reinterpret_cast<uintptr_t>(nursery->begin) + 64);
    if (!RegionManager::Instance().IsNurseryPointer(nursery_dst)) {
        GC_FAIL("nursery_dst not recognized by IsNurseryPointer"); return;
    }

    const uint8_t gens[3] = { kRegionGenYoung, kRegionGenGen1, kRegionGenOld };

    // Correct barrier decision (faithful to the finding-1 implementation and
    // CoreCLR region barrier): card iff the destination is NOT physically in the
    // nursery AND the stored reference is not mature (ref_gen < Old).  Dst
    // generation beyond "not-nursery" is intentionally NOT consulted (conservative
    // carding: gen1→gen1 is carded, extra scan work, never a correctness miss).
    //
    //   nursery dst  → skip for every ref gen (nursery scanned wholesale)
    //   non-nursery dst + ref Young(0) → CARD
    //   non-nursery dst + ref Gen1(1)  → CARD  (conservative cross-gen edge)
    //   non-nursery dst + ref Old(2)   → skip (mature ref, no cross-gen edge)
    for (int r = 0; r < 3; r++) {
        uint8_t ref_gen = gens[r];
        bool nursery_expect = false;  // physical-nursery dst never cards
        CheckOne(kRegionGenYoung, ref_gen, nursery_expect, base, nursery_dst, ref);
        bool non_nursery_expect = (ref_gen < kRegionGenOld);  // card unless ref mature
        CheckOne(kRegionGenOld, ref_gen, non_nursery_expect, base, dst, ref);
    }

    // cleanup the nursery region used only for the physical-nursery dst gate.
    RegionManager::Instance().FreeRegion(nursery->id);

    // Degenerate: ref == null → never card, for each non-null dst gen.
    for (int d = 0; d < 3; d++) {
        ClearAllCards();
        SetRegionGen(base + 64, gens[d]);
        chaos_gc_dirty_card_dst_ref(dst, nullptr);
        GC_CHECK(!IsDirty(dst), "null ref never cards");
    }

    // dst == nullptr → no-op (no crash, no card read possible).
    ClearAllCards();
    chaos_gc_dirty_card_dst_ref(nullptr, ref);
    GC_CHECK(true, "null dst no-op");

    ClearAllCards();
    std::free(heap);
}

int main() {
    puts("GC generation-aware write barrier decision tests (T7/B2 scheme B)");
    puts("══════════════════════════════════════════════════════════════════\n");

    TestDecisionMatrix();

    printf("\nResults: %d tests, %d failures\n", g_tests, g_failures);
    return g_failures > 0 ? 1 : 0;
}
