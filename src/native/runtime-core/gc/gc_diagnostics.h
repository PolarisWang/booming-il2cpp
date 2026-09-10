#ifndef CHAOS_IL2CPP_GC_DIAGNOSTICS_H_
#define CHAOS_IL2CPP_GC_DIAGNOSTICS_H_

#include <chaos/native_types.h>

namespace chaos::il2cpp::runtime_core {

struct YoungCollectionResult;  // fwd — gc_young_collector.h

/// VerifyHeap level (mirrors CoreCLR GCConfig HeapVerify level — authored here
/// as GC_HEAP_VERIFY_LEVEL; inject into CHAOS_GC_CONFIGURATION_KEYS).
enum class HeapVerifyLevel : uint8_t {
    kOff         = 0,   ///< No self-checks (default; zero overhead in prod).
    kCrucial     = 1,   ///< Verify region→generation map + old-gen/LOH OLD-marking.
    kFull        = 2,   ///< kCrucial + basic referenced-allocation walk.
};

/// Return the active VerifyHeap level (reads GcConfig().HeapVerify).
HeapVerifyLevel GcGetHeapVerifyLevel() noexcept;

/// Verify the region→generation table (CoreCLR verify_region_to_generation_map
/// analog): every nursery/gen1 Region and every old-gen/LOH page's 4MB region-gen
/// byte must match its actual generation.  A stale/mislabeled byte here is exactly
/// the class of "bookkeeping drift" that silently dropped cross-gen edges (see
/// gc_region_barrier_stress_test / GcMarkRangeOld).  LOG_ERROR+abort on mismatch.
void GcVerifyRegionToGenerationMap() noexcept;

/// Full VerifyHeap entry — call at GC boundaries (start/end of
/// chaos_gc_collect / GcYoungCollection) when HeapVerifyLevel != kOff.
void GcVerifyHeap() noexcept;

/// Verify that every object promoted by a young collection lies in a tracked
/// old-gen page (IsInOldGen) and its 4MB region-gen is OLD.  This guards the
/// A2-class regression where a promoted object's page is later untracked /
/// unsweep-coherent — a dangling risk when an old slot still references it.
/// Called after GcYoungCollection with the result.  Only active at kFull.
void GcVerifyPromotedTracked(const YoungCollectionResult& result) noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_DIAGNOSTICS_H_
