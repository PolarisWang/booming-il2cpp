#ifndef CHAOS_IL2CPP_GC_DEMOTION_H_
#define CHAOS_IL2CPP_GC_DEMOTION_H_

#include <chaos/native_types.h>

#include <cstdint>
#include <utility>
#include <vector>

#include "gc_old_gen.h"

namespace chaos::il2cpp::runtime_core {

/// Per-object relocation entry for demotion (old Gen2 addr → new Gen1 addr).
struct DemotionEntry {
    void* old_addr;
    void* new_addr;
    CHAOS_IL2CPP_SIZE size;
};

/// Default fragmentation threshold for demotion: pages with >40% free space
/// are candidates for evacuating live objects to Gen1.
static constexpr float kDemotionFragThreshold = 0.40f;

/// Maximum bytes to demote per full-GC cycle (1 MB).  Prevents excessive
/// STW pause from large-scale demotion + relocation.
static constexpr CHAOS_IL2CPP_SIZE kMaxDemotionBytes = 1024 * 1024;

/// Decide whether a page should be demoted based on its fragmentation ratio.
/// @param page        Old-gen page to evaluate.
/// @param frag_threshold  Fragmentation threshold [0..1].  Default 0.40.
/// @return true if the page is fragmented enough to warrant demotion.
bool ShouldDemote(const OldGenPage* page, float frag_threshold = kDemotionFragThreshold);

/// Scan old-gen pages for demotion candidates, relocate live objects to Gen1.
///
/// Called during STW full-GC, AFTER marking is complete but BEFORE sweep.
/// For each page exceeding the fragmentation threshold:
///   1. Walk the mark bitmap to find live objects
///   2. TryAllocateInGen1(obj_size) — if successful, memcpy to Gen1
///   3. Clear the mark bitmap bits for the demoted object (so sweep reclaims it)
///   4. Record a DemotionEntry for reference fix-up
///
/// @param old_gen    Reference to the MarkSweepOldGen instance.
/// @param max_bytes  Maximum total bytes to demote this cycle (default 1 MB).
/// @return Vector of DemotionEntry records.  Caller must pass these to
///         DemotionRelocate() after sweep to fix up all references.
std::vector<DemotionEntry> CollectDemotionCandidates(
    MarkSweepOldGen& old_gen,
    CHAOS_IL2CPP_SIZE max_bytes = kMaxDemotionBytes);

/// Relocate all old-gen slot pointers and thread-stack / GCHandle roots
/// after demotion has moved objects from Gen2 to Gen1.
///
/// Must be called at safepoint (all mutators suspended).
/// Equivalent to MarkSweepOldGen::GlobalRelocate + RelocateRoots, but
/// works with DemotionEntry (same layout as CompactPlanEntry).
///
/// @param entries  Demotion relocation entries from CollectDemotionCandidates.
/// @param old_gen  Reference to the MarkSweepOldGen whose pages to scan.
void DemotionRelocate(const std::vector<DemotionEntry>& entries,
                      MarkSweepOldGen& old_gen);

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_DEMOTION_H_
