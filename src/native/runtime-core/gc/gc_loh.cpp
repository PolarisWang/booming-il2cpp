#include "gc_loh.h"

#include <chaos/log.h>

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
            seg->marked.store(false, std::memory_order_relaxed);
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

    seg->next = segment_list_;
    segment_list_ = seg;
    segment_count_++;
    total_allocated_.fetch_add(seg->payload_size, std::memory_order_relaxed);

    void* payload = reinterpret_cast<char*>(seg) + sizeof(LohSegment);
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
        FreeSegment(excess);
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

}  // namespace chaos::il2cpp::runtime_core