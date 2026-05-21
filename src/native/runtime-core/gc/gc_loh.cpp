#include "gc_loh.h"

#include <chaos/log.h>

#include "gc_card_table.h"

#include <cstdlib>
#include <new>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#else
    #include <sys/mman.h>
    #include <unistd.h>
#endif

namespace chaos::il2cpp::runtime_core {

LargeObjectHeap g_loh;

// ======================================================================
// OS-level page allocation helpers
// ======================================================================

static void* VirtualAllocPage(CHAOS_IL2CPP_SIZE size) {
#if defined(_WIN32) || defined(_WIN64)
    auto* ptr = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (ptr == nullptr) {
        CHAOS_IL2CPP_LOG_ERROR_M("LOH", "VirtualAlloc failed size={0} error={1}",
            static_cast<unsigned long long>(size),
            static_cast<unsigned long>(GetLastError()));
    }
    return ptr;
#else
    void* ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        CHAOS_IL2CPP_LOG_ERROR_M("LOH", "mmap failed size={0}", static_cast<unsigned long long>(size));
        return nullptr;
    }
    return ptr;
#endif
}

static void VirtualFreePage(void* ptr, CHAOS_IL2CPP_SIZE size) {
#if defined(_WIN32) || defined(_WIN64)
    VirtualFree(ptr, 0, MEM_RELEASE);
#else
    munmap(ptr, size);
#endif
}

// ======================================================================
// LOH lifecycle
// ======================================================================

bool LargeObjectHeap::Init() {
    CHAOS_IL2CPP_LOG_INFO_M("LOH", "init threshold={0}", static_cast<unsigned long long>(kLohThreshold));
    return true;
}

LargeObjectHeap::~LargeObjectHeap() {
    // Free all segments.
    auto* seg = segment_list_;
    while (seg != nullptr) {
        auto* next = seg->next;
        FreeSegment(seg);
        seg = next;
    }
    auto* free_seg = free_segment_list_;
    while (free_seg != nullptr) {
        auto* next = free_seg->next;
        FreeSegment(free_seg);
        free_seg = next;
    }
}

LohSegment* LargeObjectHeap::AllocateSegment(CHAOS_IL2CPP_SIZE min_size) {
    // Round up to kLohSegmentSize alignment.
    CHAOS_IL2CPP_SIZE seg_size = (min_size + kLohSegmentSize - 1) & ~(kLohSegmentSize - 1);
    if (seg_size < kLohSegmentSize) seg_size = kLohSegmentSize;

    CHAOS_IL2CPP_SIZE total_size = sizeof(LohSegment) + seg_size;
    auto* mem = static_cast<LohSegment*>(VirtualAllocPage(total_size));
    if (mem == nullptr) return nullptr;

    mem->next = nullptr;
    mem->payload_size = seg_size;
    mem->in_use.store(true, std::memory_order_release);
    mem->marked.store(false, std::memory_order_relaxed);

    return mem;
}

void LargeObjectHeap::FreeSegment(LohSegment* seg) {
    if (seg == nullptr) return;
    VirtualFreePage(seg, sizeof(LohSegment) + seg->payload_size);
}

LohSegment* LargeObjectHeap::FindSegment(const void* ptr) const {
    if (ptr == nullptr) return nullptr;
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
    std::lock_guard<std::mutex> lock(mutex_);

    // Check active segments.
    auto* seg = segment_list_;
    while (seg != nullptr) {
        uintptr_t seg_start = reinterpret_cast<uintptr_t>(seg) + sizeof(LohSegment);
        uintptr_t seg_end = seg_start + seg->payload_size;
        if (addr >= seg_start && addr < seg_end) {
            return seg;
        }
        seg = seg->next;
    }
    return nullptr;
}

// ======================================================================
// Allocation / deallocation
// ======================================================================

void* LargeObjectHeap::Allocate(CHAOS_IL2CPP_SIZE size) {
    if (size < kLohThreshold) {
        // Below threshold — should not reach here.
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    // Try free segment list first.
    LohSegment** pp = &free_segment_list_;
    while (*pp != nullptr) {
        LohSegment* seg = *pp;
        if (seg->payload_size >= size) {
            // Reuse this segment.
            *pp = seg->next;
            seg->next = segment_list_;
            segment_list_ = seg;
            seg->in_use.store(true, std::memory_order_release);
            // Pre-mark the segment so BgcSweep Phase 5 (g_loh.Sweep())
            // does not free this freshly-reused segment between the
            // mutex release below and the caller's first write to the
            // payload.  False-positive survival for one BGC cycle is
            // harmless — the next cycle properly marks or sweeps it.
            seg->marked.store(true, std::memory_order_release);
            segment_count_++;
            total_allocated_.fetch_add(seg->payload_size, std::memory_order_relaxed);
            void* payload = reinterpret_cast<char*>(seg) + sizeof(LohSegment);
            std::memset(payload, 0, seg->payload_size);
            return payload;
        }
        pp = &seg->next;
    }

    // Allocate new segment.
    auto* seg = AllocateSegment(size);
    if (seg == nullptr) return nullptr;

    // Pre-mark the segment before linking it into segment_list_ so
    // BgcSweep Phase 5 (g_loh.Sweep()) never sees a freshly-allocated
    // segment with marked=false.  Without this mark, the BGC sweep
    // can free the segment immediately after this thread releases the
    // LOH mutex — before the caller writes to the returned payload.
    seg->marked.store(true, std::memory_order_release);

    seg->next = segment_list_;
    segment_list_ = seg;
    segment_count_++;
    total_allocated_.fetch_add(seg->payload_size, std::memory_order_relaxed);

    void* payload = reinterpret_cast<char*>(seg) + sizeof(LohSegment);
    // Register the LOH segment payload with the card table so that
    // DirtyCard() write barrier tracks pointer writes into LOH objects.
    // This enables young GC Phase 1 to discover LOH→nursery references
    // via card scanning.
    GcRegisterHeapRange(
        reinterpret_cast<uintptr_t>(payload),
        reinterpret_cast<uintptr_t>(payload) + seg->payload_size);
    std::memset(payload, 0, seg->payload_size);
    return payload;
}

void LargeObjectHeap::Free(void* ptr) {
    if (ptr == nullptr) return;
    std::lock_guard<std::mutex> lock(mutex_);

    LohSegment** pp = &segment_list_;
    while (*pp != nullptr) {
        LohSegment* seg = *pp;
        uintptr_t seg_start = reinterpret_cast<uintptr_t>(seg) + sizeof(LohSegment);
        uintptr_t seg_end = seg_start + seg->payload_size;
        uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
        if (addr >= seg_start && addr < seg_end) {
            *pp = seg->next;
            seg->next = free_segment_list_;
            free_segment_list_ = seg;
            seg->in_use.store(false, std::memory_order_release);
            seg->marked.store(false, std::memory_order_relaxed);
            segment_count_--;
            return;
        }
        pp = &seg->next;
    }
}

// ======================================================================
// GC operations
// ======================================================================

void LargeObjectHeap::UnmarkAllForTesting() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto* seg = segment_list_; seg != nullptr; seg = seg->next) {
        seg->marked.store(false, std::memory_order_relaxed);
    }
}

bool LargeObjectHeap::MarkObject(void* obj) {
    if (obj == nullptr) return false;
    auto* seg = FindSegment(obj);
    if (seg == nullptr) return false;

    bool expected = false;
    return seg->marked.compare_exchange_strong(expected, true,
        std::memory_order_release, std::memory_order_acquire);
}

CHAOS_IL2CPP_SIZE LargeObjectHeap::Sweep() {
    CHAOS_IL2CPP_SIZE reclaimed = 0;
    int freed_count = 0;
    std::lock_guard<std::mutex> lock(mutex_);

    LohSegment** pp = &segment_list_;
    while (*pp != nullptr) {
        LohSegment* seg = *pp;
        if (!seg->marked.load(std::memory_order_acquire)) {
            // Unmarked — move to free list.
            *pp = seg->next;
            seg->next = free_segment_list_;
            free_segment_list_ = seg;
            seg->in_use.store(false, std::memory_order_release);
            reclaimed += seg->payload_size;
            segment_count_--;
            freed_count++;
        } else {
            // Marked — clear mark for next cycle.
            seg->marked.store(false, std::memory_order_relaxed);
            pp = &seg->next;
        }
    }

    // Trim the free list: keep at most kLohMaxFreeSegments entries and
    // release excess segments back to the OS.  This prevents unbounded
    // memory retention after large-object allocation spikes.
    constexpr int kLohMaxFreeSegments = 4;
    int keep = kLohMaxFreeSegments;
    LohSegment** fp = &free_segment_list_;
    while (*fp != nullptr && keep > 0) {
        fp = &(*fp)->next;
        keep--;
    }
    // Release remaining segments in the free list.
    while (*fp != nullptr) {
        LohSegment* excess = *fp;
        *fp = excess->next;
        CHAOS_IL2CPP_LOG_DEBUG_M("LOH", "sweep_release_segment payload={0}",
            static_cast<unsigned long long>(excess->payload_size));

        // Unregister payload range from card table before VirtualFree.
        GcUnregisterHeapRange(
            reinterpret_cast<uintptr_t>(excess) + sizeof(LohSegment),
            reinterpret_cast<uintptr_t>(excess) + sizeof(LohSegment) + excess->payload_size);

        // DIAGNOSTIC: check if this address looks like a valid VirtualAlloc
        // before freeing. Prevents crash from double-free or invalid pointer.
        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQuery(excess, &mbi, sizeof(mbi)) == sizeof(mbi) &&
            mbi.State == MEM_COMMIT && mbi.AllocationBase == excess) {
            FreeSegment(excess);
        } else {
            CHAOS_IL2CPP_LOG_WARN_M("LOH",
                "sweep_skip_invalid_segment ptr={0} size={1} state={2}",
                reinterpret_cast<void*>(excess),
                static_cast<unsigned long long>(excess->payload_size),
                static_cast<unsigned long>(mbi.State));
        }
    }

    return reclaimed;
}

bool LargeObjectHeap::IsInLOH(const void* ptr) const {
    return FindSegment(ptr) != nullptr;
}

bool LargeObjectHeap::IsMarked(const void* obj) const {
    auto* seg = FindSegment(obj);
    if (seg == nullptr) return false;
    return seg->marked.load(std::memory_order_acquire);
}

// ── LOH Compaction ─────────────────────────────────────────────
//
// LOH compaction consolidates live segments to eliminate
// fragmentation.  It is only effective when compact_mode_ != NONE
// (default is NONE).  Objects are relocated by allocating new
// segments and copying payloads; the caller must fix up all
// references using the returned relocation map.
//
// The compaction strategy:
//   1. Collect all live (marked) segments in list order
//   2. If free segments intersperse live ones → fragment > threshold
//   3. Relocate: allocate new segments, copy live payloads
//   4. Update segment list: replace old fragmented entries
//   5. Return relocation vector for caller reference fix-up
//
// Fragmentation is measured as: free_segments / total_segments
// ON_REQUEST mode compacts when fragmentation > 0 (any at all).
// AUTOMATIC mode compacts when fragmentation > kLohCompactFragThreshold.

static constexpr float kLohCompactFragThreshold = 0.25f;  // 25% free = compact

CHAOS_IL2CPP_SIZE LargeObjectHeap::Compact(std::vector<std::pair<void*, void*>>& out_relocations) {
    if (compact_mode_ == CompactMode::NONE) return 0;

    std::lock_guard<std::mutex> lock(mutex_);

    // Phase 1: Count live vs free segments.
    int total_segments = 0;
    int free_segments = 0;
    for (auto* seg = segment_list_; seg != nullptr; seg = seg->next) {
        total_segments++;
        if (!seg->in_use.load(std::memory_order_acquire)) {
            free_segments++;
        }
    }

    if (total_segments < 2 || free_segments == 0) return 0;

    // Check fragmentation threshold.
    float frag = static_cast<float>(free_segments) / static_cast<float>(total_segments);
    if (compact_mode_ == CompactMode::AUTOMATIC && frag < kLohCompactFragThreshold) {
        CHAOS_IL2CPP_LOG_DEBUG_M("LOH", "compact_skip frag={0:.2f} threshold={1:.2f}",
            frag, kLohCompactFragThreshold);
        return 0;
    }

    // Phase 2: Collect live segments and their payloads.
    struct LiveEntry {
        LohSegment* segment;
        void*       payload;   // Start of object data
        CHAOS_IL2CPP_SIZE size; // Payload size
    };
    std::vector<LiveEntry> live;
    live.reserve(static_cast<size_t>(total_segments - free_segments));

    for (auto* seg = segment_list_; seg != nullptr; seg = seg->next) {
        if (seg->in_use.load(std::memory_order_acquire)) {
            live.push_back({seg,
                reinterpret_cast<char*>(seg) + sizeof(LohSegment),
                seg->payload_size});
        }
    }

    // Phase 3: Check if compaction is actually needed.
    // Only compact if there's interleaving (not just a tail of free segments).
    bool needs_compact = false;
    bool saw_free = false;
    int live_after_free = 0;
    for (auto* seg = segment_list_; seg != nullptr; seg = seg->next) {
        if (!seg->in_use.load(std::memory_order_acquire)) {
            saw_free = true;
        } else if (saw_free) {
            live_after_free++;
        }
    }
    if (live_after_free == 0) return 0;  // All free at tail — no relocation needed.

    // Phase 4: Relocate — allocate new segments and copy live payloads.
    // New segments are allocated from the OS, not from free list, to avoid
    // the fragmented layout.  Old segments are moved to the free list after
    // the copy, ensuring the new layout is contiguous.
    //
    // Since LOH objects are large (≥85KB), we limit per-cycle relocation
    // to avoid excessive STW pause.  The budget is 4MB (roughly 48 segments
    // at minimum LOH size) per compaction cycle.
    constexpr CHAOS_IL2CPP_SIZE kMaxCompactBytes = 4 * 1024 * 1024;
    CHAOS_IL2CPP_SIZE total_relocated = 0;

    out_relocations.clear();
    out_relocations.reserve(live.size());

    for (auto& entry : live) {
        if (total_relocated >= kMaxCompactBytes) break;

        // Allocate a fresh segment for the relocated object.
        auto* new_seg = AllocateSegment(entry.size);
        if (new_seg == nullptr) break;  // OOM during compaction — stop.

        void* new_payload = reinterpret_cast<char*>(new_seg) + sizeof(LohSegment);
        std::memcpy(new_payload, entry.payload, entry.size);

        // Record relocation.
        out_relocations.emplace_back(entry.payload, new_payload);
        total_relocated += entry.size;

        // Mark the old segment as free (not in use).
        entry.segment->in_use.store(false, std::memory_order_release);
        entry.segment->marked.store(false, std::memory_order_relaxed);

        // Update total_allocated_ (new segment adds to it; old one is freed).
        total_allocated_.fetch_add(new_seg->payload_size, std::memory_order_relaxed);
        segment_count_++;

        // Insert the new segment at the head of the live list.
        new_seg->next = segment_list_;
        segment_list_ = new_seg;
    }

    if (out_relocations.empty()) return 0;

    // Phase 5: Sweep old segments to free list (for reuse or OS release).
    // Walk the list and move any !in_use segments to the free list.
    LohSegment** pp = &segment_list_;
    while (*pp != nullptr) {
        LohSegment* seg = *pp;
        if (!seg->in_use.load(std::memory_order_acquire)) {
            *pp = seg->next;
            seg->next = free_segment_list_;
            free_segment_list_ = seg;
            segment_count_--;
        } else {
            pp = &seg->next;
        }
    }

    CHAOS_IL2CPP_LOG_INFO_M("LOH",
        "compact_done relocated_bytes={0} segments_moved={1}",
        static_cast<unsigned long long>(total_relocated),
        static_cast<unsigned long>(out_relocations.size()));

    return total_relocated;
}

}  // namespace chaos::il2cpp::runtime_core