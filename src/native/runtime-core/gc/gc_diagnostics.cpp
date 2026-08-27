#include "gc_diagnostics.h"

#include <chaos/log.h>

#include "gc_config.h"
#include "gc_gen1.h"
#include "gc_heap.h"
#include "gc_loh.h"
#include "gc_old_gen.h"
#include "gc_region.h"
#include "gc_young_collector.h"
#include "gc_young_gen.h"

namespace chaos::il2cpp::runtime_core {

HeapVerifyLevel GcGetHeapVerifyLevel() noexcept {
    int v = static_cast<int>(GcConfig().HeapVerify);
    if (v <= 0) return HeapVerifyLevel::kOff;
    if (v >= 2) return HeapVerifyLevel::kFull;
    return HeapVerifyLevel::kCrucial;
}

namespace {

/// Check a single 4MB region-gen byte.  LOG_ERROR (not aborts — keep it
/// recoverable/informative in CI, matching CoreCLR verify which logs+asserts
/// under _DEBUG; here we log ERROR_M so the test harness sees a FAIL marker).
inline void CheckRegionGen(uintptr_t addr, uint8_t expect, const char* what) noexcept {
    uint8_t got = GetRegionGen(addr);
    if (got != expect) {
        CHAOS_IL2CPP_LOG_ERROR_M("GCVerify",
            "region->gen mismatch: {0} addr={1} expect_gen={2} got_gen={3}",
            what, (void*)addr, (unsigned)expect, (unsigned)got);
    }
}

}  // namespace

void GcVerifyRegionToGenerationMap() noexcept {
    // Nursery = young (gen0); Gen1 = its own gen (gen1, M9) — young GC scans the
    // young side (gen0+gen1) wholesale.
    auto* nursery = G_YoungGen().region.load(std::memory_order_acquire);
    if (nursery != nullptr && nursery->begin != nullptr) {
        CheckRegionGen(reinterpret_cast<uintptr_t>(nursery->begin), kRegionGenYoung, "nursery");
        CheckRegionGen(reinterpret_cast<uintptr_t>(nursery->end) - 1, kRegionGenYoung, "nursery_end");
    }
    auto* gen1 = G_YoungGen().gen1_region.load(std::memory_order_acquire);
    if (gen1 != nullptr && gen1->begin != nullptr) {
        CheckRegionGen(reinterpret_cast<uintptr_t>(gen1->begin), kRegionGenGen1, "gen1");
    }

    // Old-gen pages must be OLD (gen2) — GcMarkRangeOld sets this at AllocatePage.
    // A stale 0 here is exactly the cross-gen-edge-dropping bug (barrier sees
    // gen0 and skips carding).  This is the T1-class bookkeeping-drift guard.
    for (auto* page = G_OldGen().PageList(); page != nullptr; page = page->next) {
        if (!page->in_use.load(std::memory_order_acquire)) continue;
        CheckRegionGen(reinterpret_cast<uintptr_t>(page->Payload()), kRegionGenOld, "old_gen_page");
    }

    // LOH payloads must be OLD (gen2) — GcMarkRangeOld at segment alloc.
    for (auto* seg = G_Loh().SegmentListForDiag(); seg != nullptr; seg = seg->next) {
        uintptr_t payload = reinterpret_cast<uintptr_t>(seg) + sizeof(LohSegment);
        CheckRegionGen(payload, kRegionGenOld, "loh_segment");
    }
}

void GcVerifyHeap() noexcept {
    if (GcGetHeapVerifyLevel() == HeapVerifyLevel::kOff) return;
    GcVerifyRegionToGenerationMap();
    if (GcGetHeapVerifyLevel() < HeapVerifyLevel::kFull) return;

    // kFull referenced-allocation walk (CoreCLR HeapVerify-grade): cheap
    // sampling over marked objects in every in-use old-gen page.  Validate that
    // a marked object (a) lives in a tracked old-gen page (IsInOldGen), (b) has
    // region-gen OLD, and (c) whose first word is a valid TypeInfo.  Also re-read
    // the existing bitmap poison tail (0xCD bytes after each page's bitmap) and
    // report if any were clobbered — catches out-of-bounds bitmap writes that a
    // silent verification would otherwise miss.  Cold path; only kFull (CI/debug,
    // CHAOS_GC_HeapVerify=2).  Sampling bound caps pause cost on large heaps.
    const auto* arr = G_OldGen().GetPageArray();
    if (arr == nullptr) return;
    constexpr int kMaxVerifyObjects = 4096;
    int verified = 0;
    for (int pi = 0; pi < arr->count && verified < kMaxVerifyObjects; pi++) {
        auto* page = arr->pages[pi];
        if (page == nullptr || !page->in_use.load(std::memory_order_acquire)) continue;

        // Bitmap poison tail: page->bitmap_bytes ALREADY includes the 16-byte
        // 0xCD poison guard appended after the real bitmap (AllocatePage does
        // bitmap_bytes += kBitmapPoison).  The last 16 bytes of the bitmap are
        // the poison.  If any were overwritten, an out-of-bounds bitmap write
        // occurred — report it (kFull only).
        unsigned char* bmp = page->MarkBitmap();
        constexpr CHAOS_IL2CPP_SIZE kBitmapPoison = 16;
        CHAOS_IL2CPP_SIZE bmp_useful_bytes = page->bitmap_bytes > kBitmapPoison
            ? (page->bitmap_bytes - kBitmapPoison) : 0;
        for (CHAOS_IL2CPP_SIZE k = 0; k < kBitmapPoison; k++) {
            unsigned char v = bmp[bmp_useful_bytes + k];
            if (v != 0xCD) {
                CHAOS_IL2CPP_LOG_ERROR_M("GCVerify",
                    "bitmap poison clobbered at page {0} tail byte {1} (0x{2:x})",
                    reinterpret_cast<void*>(page->Payload()),
                    static_cast<unsigned>(k), static_cast<unsigned>(v));
                break;
            }
        }

        // Sample up to a bounded number of marked object starts on this page.
        char* payload = page->Payload();
        CHAOS_IL2CPP_SIZE payload_size = page->payload_size;
        for (CHAOS_IL2CPP_SIZE bit = 0;
             bit < bmp_useful_bytes * 8 && verified < kMaxVerifyObjects; bit++) {
            if ((bmp[bit >> 3] & (1u << (bit & 7))) == 0) continue;
            void* obj = payload + bit * sizeof(void*);
            if (obj >= payload + payload_size) break;
            // In-place demotion (gc_old_gen.h:79-85): a demoted object is a live
            // gen1-owned object that stays RESIDENT in this old-gen page with its
            // mark-bit kept set (so sweep/BGC preserve it), while GetRegionGen
            // classifies it as Gen1 — NOT OLD.  The kFull walk below validates
            // OLD-gen residency + region-gen OLD, which is wrong for demoted
            // objects: every marked slot inside one is legitimately gen1-resident
            // in an old-gen page.  Normalize through the demoted set and skip, so
            // the verify tool reflects the in-place model (avoids an ERROR flood
            // — and the latent SEGFAULT of reading swept demoted pages as objects).
            if (IsInDemotedSet(obj)) {
                continue;  // legitimately gen1-owned, resident in this old-gen page
            }
            if (!G_OldGen().IsInOldGen(obj)) {
                CHAOS_IL2CPP_LOG_ERROR_M("GCVerify",
                    "kFull: marked object not in tracked old-gen page: {0}", obj);
            }
            uint8_t rg = GetRegionGen(reinterpret_cast<uintptr_t>(obj));
            if (rg != kRegionGenOld) {
                CHAOS_IL2CPP_LOG_ERROR_M("GCVerify",
                    "kFull: marked object region-gen not OLD: {0} gen={1}", obj, (unsigned)rg);
            }
            const void* fw = *static_cast<const void* const*>(obj);
            if (fw == nullptr ||
                !GcLayoutRegistry::Instance().IsValidTypeInfoPointer(fw)) {
                CHAOS_IL2CPP_LOG_ERROR_M("GCVerify",
                    "kFull: marked object first-word is not a valid TypeInfo: {0}", obj);
            }
            verified++;
        }
    }
}

// Promotion target validation.  A promoted object's legal destination depends on
// its source generation: a gen0 (nursery) object promotes to Gen1 (region-gen 1),
// and a surviving gen1 object either stays in Gen1 or promotes to gen2 (old).
// So asserting every promoted address IsInOldGen && region-gen==OLD is WRONG and
// false-positives on the common nursery→Gen1 path (P1-A2b-era wrong-boxing).
// Classify by the promoted target's OWN region-gen:
//   OLD  → must be IsInOldGen (tracked old-gen page)
//   Gen1 → must fall in [gen1_region.begin, gen1_bump) (IsInGen1)
//   other (gen0/unknown) → a promotion target should never land back in nursery.
void GcVerifyPromotedTracked(const YoungCollectionResult& result) noexcept {
    if (GcGetHeapVerifyLevel() < HeapVerifyLevel::kFull) return;
    for (int i = 0; i < result.bfs_worklist_count; ++i) {
        void* p = result.bfs_worklist[i];
        if (p == nullptr) continue;
        uint8_t rg = GetRegionGen(reinterpret_cast<uintptr_t>(p));
        if (rg == kRegionGenOld) {
            if (!G_OldGen().IsInOldGen(p)) {
                CHAOS_IL2CPP_LOG_ERROR_M("GCVerify",
                    "promoted OLD object not in tracked old-gen page: {0}", (void*)p);
            }
        } else if (rg == kRegionGenGen1) {
            if (!IsInGen1(p)) {
                CHAOS_IL2CPP_LOG_ERROR_M("GCVerify",
                    "promoted Gen1 object not in gen1 range: {0} (bump may be unset)", (void*)p);
            }
        } else {
            CHAOS_IL2CPP_LOG_ERROR_M("GCVerify",
                "promoted object landed in unexpected generation (gen={0}, not OLD/Gen1): {1}",
                (unsigned)rg, (void*)p);
        }
    }
}

}  // namespace chaos::il2cpp::runtime_core
