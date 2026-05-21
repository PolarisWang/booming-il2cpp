#ifndef CHAOS_IL2CPP_GC_GEN1_H_
#define CHAOS_IL2CPP_GC_GEN1_H_

#include <chaos/native_types.h>

#include <atomic>
#include <cstdint>

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// Gen1 — intermediate generation (survivor-area mark-sweep)
//
// == Design ==
// Gen1 shares physical memory with the survivor area (the second half of
// the 16 MB nursery region).  Objects promoted from Gen0 (young half of
// nursery) land in Gen1 via bump-pointer allocation (TryAllocateInGen1,
// identical to the existing TryAllocateInSurvivor).
//
// When Gen1 collection triggers (budget exhausted / interval exceeded),
// GcGen1Collection() performs a STW mark-sweep on the survivor range
// [gen1->begin, gen1_bump), then promotes ALL live objects to
// Gen2 (old gen) via g_old_gen.Allocate() + memcpy.  The survivor area
// is fully reset after collection.
//
// == Why this design ==
// - Zero new memory: Gen1 reuses the 8 MB survivor area already carved
//   from the nursery region.
// - Simple correct: full sweep + full promote avoids fragmentation and
//   dangling-pointer bugs in Gen1 itself.
// - Low overhead: Gen1 roots = Gen0 pointers + Gen2 dirty cards +
//   thread stacks + GCHandles.  No BGC interaction (BGC ignores Gen1).
// ======================================================================

/// Conservative max object size for Gen1 objects (same as young collector).
/// Used as fallback when TypeInfo/GcLayout is not available.
static constexpr CHAOS_IL2CPP_SIZE kGen1MaxEstObjectSize = 2048;

/// Minimum object count threshold for Gen1 mark-sweep.
/// If Gen1 has fewer live objects than this, skip mark-sweep overhead
/// and promote all directly to Gen2.
static constexpr CHAOS_IL2CPP_SIZE kGen1MinPromoteThreshold = 64;

/// Result of a Gen1 collection.
struct Gen1CollectionResult {
    CHAOS_IL2CPP_SIZE objects_promoted{0};   // Objects promoted to Gen2
    CHAOS_IL2CPP_SIZE bytes_promoted{0};     // Bytes copied to Gen2
    CHAOS_IL2CPP_SIZE bytes_reclaimed{0};    // Bytes freed from Gen1 (dead objects)
    CHAOS_IL2CPP_SIZE objects_in_gen1{0};    // Total objects in Gen1 before collection
    uint64_t           pause_ns{0};           // STW pause duration
    bool               promotion_failed{false}; // True if any Gen2 alloc failed (gen1_bump NOT reset)
};

/// Process-wide Gen1 state (defined in gc_gen1.cpp).
struct Gen1State {
    /// Total bytes ever allocated into Gen1 (since last Gen1 collection).
    /// Used by the scheduler for budget-based triggering.
    std::atomic<CHAOS_IL2CPP_SIZE> total_allocated{0};

    /// Number of Gen1 collections performed.
    std::atomic<uint64_t> collection_count{0};

    /// Bytes that survived the last Gen1 collection (promoted to Gen2).
    CHAOS_IL2CPP_SIZE last_survived_bytes{0};

    /// Young GCs since last Gen1 collection (for interval-based triggering).
    std::atomic<int> young_gc_since_last_gen1{0};
};

extern Gen1State g_gen1_state;

/// Perform a Gen1 collection: mark-sweep the Gen1 area [gen1->begin,
/// gen1_bump), then promote marked (live) objects to Gen2.
/// Called at a STW safepoint.  Resets gen1_bump to gen1->begin
/// after collection so the entire 8 MB Gen1 space is reusable.
Gen1CollectionResult GcGen1Collection();

/// Check whether @a ptr falls within the allocated portion of Gen1
/// ([begin, gen1_bump)).  Returns false for unallocated Gen1 space.
bool IsInGen1(const void* ptr);

/// Check whether @a ptr falls within the full Gen1 region range
/// ([begin, end)), including unallocated space.
/// Unlike IsInGen1(), this checks against gen1_end (the region boundary)
/// rather than gen1_bump (the allocation frontier), making it suitable
/// for card table filtering where unallocated pages have no objects
/// but still belong to the Gen1 address range.
bool IsInGen1Range(const void* ptr);

/// Allocate @a size bytes in Gen1.  Same as TryAllocateInSurvivor;
/// the alias exists for semantic clarity (Gen0→Gen1 promotion).
void* TryAllocateInGen1(CHAOS_IL2CPP_SIZE size);

/// Estimate Gen1 fragmentation: 1 - (live_bytes / total_bytes).
/// Used by the scheduler to trigger Gen1 collection at high fragmentation.
float Gen1Fragmentation();

/// Check whether an independent Gen1 collection should be triggered.
/// Returns true when:
///   1. Gen1 has data (gen1_bump > gen1->begin)
///   2. BGC is NOT actively marking Gen1 (IsGen1MarkingActive() == false)
///   3. Any of:
///      a. Gen1 occupancy > 80%
///      b. Gen1 fragmentation > 50%
///      c. young_gc_since_last_gen1 >= kGen1MaxInterval (8)
bool GcGen1ShouldCollect();

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_GEN1_H_
