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
};

/// Perform a young collection (Cheney copy + promote).
/// Called after STW safepoint has been reached.
/// @param nursery  The nursery region(s) to collect.
/// @param tenured_target  Optional tenured region to promote into (null = auto-create).
YoungCollectionResult GcYoungCollection(Region* nursery, Region* tenured_target = nullptr);

/// Check whether @a ptr points into any nursery region.
/// Used by the card table scanner to filter old→nursery references.
bool IsInNursery(const void* ptr);

/// Scavenge a single object: copy from nursery to tenured if not already forwarded.
/// Returns the tenured address (either newly copied or the existing forwarding target).
void* GcScavengeObject(void* obj, YoungCollectionResult* result = nullptr);

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_YOUNG_COLLECTOR_H_