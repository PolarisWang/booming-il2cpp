#ifndef CHAOS_IL2CPP_GC_YOUNG_COLLECTOR_H_
#define CHAOS_IL2CPP_GC_YOUNG_COLLECTOR_H_

#include <chaos/native_types.h>

#include "gc_region.h"
#include "gc_card_table.h"

// ======================================================================
// CRAG Young Collector — Cheney-style copying nursery GC
//
// == Flow ==
// 1. STW safepoint (generation-based)
// 2. Scan precise roots (GcSlotMap) + conservative fallback
// 3. Scan dirty cards for old→nursery cross-gen references
// 4. Cheney copy: for each root pointing into nursery, copy the object
//    to a tenured region, place a forwarding pointer, and enqueue
//    the copy for field scanning.
// 5. Drain the Cheney scan queue (BFS over the copied object graph).
// 6. Release old nursery regions, allocate fresh ones.
// 7. Clear card table for the scanned range.
// ======================================================================

namespace chaos::il2cpp::runtime_core {

/// Maximum objects to promote in a single young collection.
/// Beyond this, remaining nursery objects are treated as unreachable.
static constexpr CHAOS_IL2CPP_SIZE kMaxPromoteObjects = 256 * 1024;

/// Result of a young collection.
struct YoungCollectionResult {
    CHAOS_IL2CPP_SIZE objects_promoted;   // Number of live objects copied to tenured
    CHAOS_IL2CPP_SIZE bytes_promoted;     // Total bytes copied
    CHAOS_IL2CPP_SIZE bytes_reclaimed;    // Nursery bytes freed (reclaimed)
    CHAOS_IL2CPP_SIZE dirty_cards_scanned; // Cards scanned for cross-gen refs
    bool              timed_out;           // True if kMaxPromoteObjects was reached

    /// Condemned generation for this collection (align CoreCLR gc_mark's
    /// condemned_gen, mark_phase.cpp:1393).  A GC condemns every generation
    /// YOUNGER than or equal to this value; objects whose region generation is
    /// NEWER (numerically greater) than this are not promoted/marked in this
    /// collection (handled by an older-gen collection).  For a young GC that
    /// collects nursery (gen0) + survivor (gen1), the condemned gen is gen1
    /// (kRegionGenGen1=1); gen0/gen1 objects (region_gen <= 1) are processed,
    /// gen2/old objects (region_gen > 1) are left untouched.
    uint8_t condemned_gen_num{kRegionGenGen1};

    /// Cheney BFS worklist: tenured addresses of promoted objects, to be
    /// scanned in Phase 3 for transitive closure.  The array is pre-allocated
    /// by the caller (GcYoungCollection) and filled by GcScavengeObject.
    void** bfs_worklist{nullptr};
    int    bfs_worklist_capacity{0};
    int    bfs_worklist_count{0};
};

/// Perform a young collection on the shared young generation.
/// Operates on g_young_gen.region (set before call, reset after).
/// Called after STW safepoint has been reached.
YoungCollectionResult GcYoungCollection(bool force_skip_gen1 = false);

/// Check whether @a ptr points into any nursery region.
/// Used by the card table scanner to filter old→nursery references.
bool IsInNursery(const void* ptr);

/// Scavenge a single object: copy from nursery to tenured if not already forwarded.
/// Returns the tenured address (either newly copied or the existing forwarding target).
void* GcScavengeObject(void* obj, YoungCollectionResult* result = nullptr);

/// Same as GcScavengeObject but the caller guarantees @a obj points into the
/// nursery.  Skips the IsInNursery check for a ~2x speedup on known-nursery
/// references in the young GC hot path.
void* GcScavengeObjectKnownNursery(void* obj, YoungCollectionResult* result = nullptr);

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_YOUNG_COLLECTOR_H_