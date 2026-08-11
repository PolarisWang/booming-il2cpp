#include "gc_diagnostics.h"

#include <chaos/log.h>

#include "gc_config.h"
#include "gc_heap.h"
#include "gc_loh.h"
#include "gc_old_gen.h"
#include "gc_region.h"
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
    // Nursery + Gen1 regions must be young (gen0) — young GC scans them wholesale.
    auto* nursery = G_YoungGen().region.load(std::memory_order_acquire);
    if (nursery != nullptr && nursery->begin != nullptr) {
        CheckRegionGen(reinterpret_cast<uintptr_t>(nursery->begin), kRegionGenYoung, "nursery");
        CheckRegionGen(reinterpret_cast<uintptr_t>(nursery->end) - 1, kRegionGenYoung, "nursery_end");
    }
    auto* gen1 = G_YoungGen().gen1_region.load(std::memory_order_acquire);
    if (gen1 != nullptr && gen1->begin != nullptr) {
        CheckRegionGen(reinterpret_cast<uintptr_t>(gen1->begin), kRegionGenYoung, "gen1");
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
    // kFull: (future) referenced-allocation walk — region-gen consistency is the
    // highest-value, lowest-risk check and guards the known cross-gen edge bug.
}

}  // namespace chaos::il2cpp::runtime_core
