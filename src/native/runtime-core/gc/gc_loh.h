#ifndef CHAOS_IL2CPP_GC_LOH_H_
#define CHAOS_IL2CPP_GC_LOH_H_

#include <chaos/native_types.h>

#include "gc_card_table.h"
#include "gc_region.h"

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <utility>
#include <vector>

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// Large Object Heap (LOH) — objects > 85 KB
//
// == Design (CoreCLR-aligned) ==
// - Threshold: 85 KB (matching CoreCLR's LargeObjectAllocThreshold)
// - Allocation: VirtualAlloc 64 KB segments, carved into single-object chunks
// - No compaction: objects are too large to move (memmove cost > fragmentation benefit)
// - Free list: reclaimed segments tracked for reuse
// - Mark bitmap: per-segment, single-bit tracking (one object per segment)
//
// == Segment layout ==
// [LOHSegment header | mark_bit (1 byte) | payload (size aligned to segment)]
// ======================================================================

static constexpr CHAOS_IL2CPP_SIZE kLohThreshold = 85 * 1024;     // 85 KB
static constexpr CHAOS_IL2CPP_SIZE kLohSegmentSize = 64 * 1024;   // 64 KB min segment

/// LOH segment header (at the start of each VirtualAlloc'd block).
struct LohSegment {
    LohSegment* next;                        ///< Link in segment list
    CHAOS_IL2CPP_SIZE payload_size;          ///< Usable payload bytes
    std::atomic<bool> in_use;                ///< true = segment actively used
    std::atomic<bool> marked;                ///< true = object in this segment is marked live
    RegionId region_id;                      ///< Backing region ; kRegionIdInvalid if raw
};

/// Large Object Heap — mark-sweep with optional compaction.
///
/// Compaction mode controls when LOH objects are relocated to reduce
/// fragmentation.  Default is NONE (no compaction) since large objects
/// are expensive to move (≥85 KB memcpy per object).
class LargeObjectHeap {
public:
    LargeObjectHeap() = default;
    ~LargeObjectHeap();

    LargeObjectHeap(const LargeObjectHeap&) = delete;
    LargeObjectHeap& operator=(const LargeObjectHeap&) = delete;

    /// Initialize the LOH.
    bool Init();

    /// Allocate a large object (> 85 KB).
    /// Returns nullptr on OOM.
    void* Allocate(CHAOS_IL2CPP_SIZE size);

    /// Free a large object.
    void Free(void* ptr);

    /// Mark an object in the LOH as reachable.
    /// Returns true if newly marked.
    bool MarkObject(void* obj);

    /// Sweep the LOH: free unreachable segments.
    /// Returns total bytes reclaimed.
    CHAOS_IL2CPP_SIZE Sweep();

    /// Check if @a ptr falls within a LOH segment.
    bool IsInLOH(const void* ptr) const;

    /// Check if a LOH object is marked (reachable).
    /// Returns false if the object is not in the LOH or not marked.
    bool IsMarked(const void* obj) const;

    /// Scan dirty cards across all LOH segments for young GC.
    /// Calls @a callback(range_start, range_end) for each dirty range,
    /// grouping consecutive dirty cards.  @a dirty_card_count is updated
    /// with the total number of dirty cards found.
    template <typename Fn>
    void ScanDirtyCardsInSegmentsBatched(CHAOS_IL2CPP_SIZE* dirty_card_count,
                                          Fn&& callback) {
        // LOH segments are registered with the card table via GcRegisterHeapRange
        // (added in Allocate).  Walk segments and scan dirty cards within each
        // segment's payload range.
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto* seg = segment_list_; seg != nullptr; seg = seg->next) {
            if (!seg->in_use.load(std::memory_order_acquire)) continue;
            uintptr_t payload_start = reinterpret_cast<uintptr_t>(seg) + sizeof(LohSegment);
            uintptr_t payload_end = payload_start + seg->payload_size;
            ScanDirtyCardsBatched(payload_start, payload_end,
                                  dirty_card_count, callback);
        }
    }

    /// Total number of active segments.
    int SegmentCount() const { return segment_count_; }

    /// Total bytes allocated (sum of all segment payload sizes).
    CHAOS_IL2CPP_SIZE TotalAllocated() const {
        return total_allocated_.load(std::memory_order_relaxed);
    }

    /// LOH compaction mode (default: NONE — no compaction).
    enum class CompactMode : uint8_t {
        NONE = 0,       ///< No compaction (default, safe).
        ON_REQUEST = 1, ///< Compact only when explicitly requested.
        AUTOMATIC = 2,  ///< Compact when fragmentation exceeds threshold.
    };

    /// Set the compaction mode.
    void SetCompactMode(CompactMode mode) { compact_mode_ = mode; }

    /// Get the current compaction mode.
    CompactMode GetCompactMode() const { return compact_mode_; }

    /// Clear all segment marks (for testing — clears pre-mark from Allocate).
    void UnmarkAllForTesting();

    /// Compact the LOH: relocate live objects from fragmented segments
    /// to reduce free-segment interleaving.  Returns total bytes relocated,
    /// or 0 if no compaction was needed/performed.
    /// Only effective when compact_mode_ != NONE.
    /// Populates @a out_relocations with old→new address mappings so the
    /// caller can fix up references (e.g., via GlobalRelocate-style walk).
    CHAOS_IL2CPP_SIZE Compact(std::vector<std::pair<void*, void*>>& out_relocations);

    /// Diagnostics accessor for the active segment list (GCVerify / tooling).
    /// Returns the head of segment_list_ (may be null).  Not for mutation.
    LohSegment* SegmentListForDiag() const noexcept { return segment_list_; }

private:
    /// Allocate a new segment from the OS.
    LohSegment* AllocateSegment(CHAOS_IL2CPP_SIZE min_size);

    /// Free a segment back to the OS.
    void FreeSegment(LohSegment* seg);

    /// Find the segment containing @a ptr.
    LohSegment* FindSegment(const void* ptr) const;

    LohSegment* segment_list_ = nullptr;      ///< Singly-linked list of active segments
    LohSegment* free_segment_list_ = nullptr; ///< Free segments for reuse
    int segment_count_ = 0;
    std::atomic<CHAOS_IL2CPP_SIZE> total_allocated_{0};
    CompactMode compact_mode_{CompactMode::AUTOMATIC}; ///< Default: AUTOMATIC compaction
    mutable std::mutex mutex_;
};

/// Global LOH instance.
extern LargeObjectHeap g_loh;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_LOH_H_