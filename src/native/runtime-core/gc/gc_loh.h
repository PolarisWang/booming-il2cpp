#ifndef CHAOS_IL2CPP_GC_LOH_H_
#define CHAOS_IL2CPP_GC_LOH_H_

#include <chaos/native_types.h>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <mutex>

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
};

/// Large Object Heap — mark-sweep without compaction.
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

    /// Total number of active segments.
    int SegmentCount() const { return segment_count_; }

    /// Total bytes allocated (sum of all segment payload sizes).
    CHAOS_IL2CPP_SIZE TotalAllocated() const {
        return total_allocated_.load(std::memory_order_relaxed);
    }

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
    mutable std::mutex mutex_;
};

/// Global LOH instance.
extern LargeObjectHeap g_loh;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_LOH_H_