#include "gc_old_gen.h"

#include <chaos/log.h>
#include <chaos/profile.h>

#include "gc_bit_utils.h"
#include "gc_card_table.h"
#include "gc_events.h"
#include "gc_layout.h"
#include "gc_loh.h"
#include "gc_parallel_mark.h"
#include "gc_region.h"
#include "gc_young_gen.h"
#include "gc_scheduler.h"
#include "gc_stats.h"
#include "gc_worker_pool.h"
#include "thread_pool.h"
#include "memory_domain.h"
#include "thread_state.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <new>
#include <thread>
#include <vector>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #include <intrin.h>
    #define CHAOS_OLDGEN_SPIN_HINT()  _mm_pause()
#else
    #include <sys/mman.h>
    #include <unistd.h>
    #define CHAOS_OLDGEN_SPIN_HINT()  __builtin_ia32_pause()
#endif

// Overhead:  8 bytes (aligned to pointer size).
static constexpr CHAOS_IL2CPP_SIZE kOldGenBlockHeaderSize = sizeof(void*);

namespace chaos::il2cpp::runtime_core {

// ���� Global instance ������������������������������������������������������������������������������������������������
MarkSweepOldGen g_old_gen;

// ======================================================================
// Platform virtual memory helpers
// ======================================================================

static void* VirtualAllocPage(CHAOS_IL2CPP_SIZE size) {
#if defined(_WIN32) || defined(_WIN64)
    auto* ptr = VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (ptr == nullptr) {
        CHAOS_IL2CPP_LOG_ERROR_M("OldGen", "VirtualAlloc failed size={0} err={1}",
            static_cast<unsigned long long>(size), GetLastError());
    }
    return ptr;
#else
    auto* ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        CHAOS_IL2CPP_LOG_ERROR_M("OldGen", "mmap failed size={0}", static_cast<unsigned long long>(size));
        return nullptr;
    }
    return ptr;
#endif
}

static void VirtualFreePage(void* ptr, CHAOS_IL2CPP_SIZE size) {
    if (ptr == nullptr) return;
#if defined(_WIN32) || defined(_WIN64)
    VirtualFree(ptr, 0, MEM_RELEASE);
#else
    munmap(ptr, size);
#endif
}

// ======================================================================
// Construction / Destruction
// ======================================================================

MarkSweepOldGen::~MarkSweepOldGen() {
    auto* page = page_list_;
    while (page != nullptr) {
        auto* next = page->next;
        FreePage(page);
        page = next;
    }
    page_list_ = nullptr;
    page_count_ = 0;

    // Free any deferred oversized pages.
    for (auto* p : deferred_free_pages_) {
        VirtualFreePage(p, p->page_size);
    }
    deferred_free_pages_.clear();

    auto* old_array = page_array_.load(std::memory_order_relaxed);
    if (old_array != nullptr) {
        std::free(old_array->pages);
        delete old_array;
    }
    for (int i = 0; i < kRetiredRingSize; i++) {
        if (retired_ring_[i] != nullptr) {
            std::free(retired_ring_[i]->pages);
            delete retired_ring_[i];
        }
    }
}

// ======================================================================
// Init
// ======================================================================

bool MarkSweepOldGen::Init(uintptr_t heap_hint, int initial_pages) {
    (void)heap_hint;  // hint not used yet �� VirtualAlloc picks address

    CHAOS_IL2CPP_LOG_INFO_M("OldGen", "init initial_pages={0}", initial_pages);

    for (int i = 0; i < initial_pages; i++) {
        auto* page = AllocatePage(kOldGenPageSize, true);
        if (page == nullptr) {
            CHAOS_IL2CPP_LOG_ERROR_M("OldGen", "failed to allocate initial page {0}", i);
            return false;
        }
    }

    RebuildPageArray();

    CHAOS_IL2CPP_LOG_INFO_M("OldGen", "init done base=0x{0} pages={1}",
        static_cast<unsigned long long>(heap_base_), page_count_);
    return true;
}

// ======================================================================
// Page management
// ======================================================================

OldGenPage* MarkSweepOldGen::AllocatePage(CHAOS_IL2CPP_SIZE size, bool scanning, int preferred_sc_idx) {
    CHAOS_IL2CPP_PROFILE_SCOPE("OldGen::AllocatePage");

    // Calculate: header + aligned bitmap + poison guard + payload.
    CHAOS_IL2CPP_SIZE payload_size = size;
    CHAOS_IL2CPP_SIZE bitmap_bytes = (payload_size / sizeof(void*)) / 8;
    bitmap_bytes = (bitmap_bytes + 15) & ~static_cast<CHAOS_IL2CPP_SIZE>(15);
    // Poison guard: 16 bytes of 0xCD after the bitmap to detect overflow writes.
    // The guard is part of bitmap_bytes so Payload() offset correctly skips it.
    constexpr CHAOS_IL2CPP_SIZE kBitmapPoison = 16;
    CHAOS_IL2CPP_SIZE raw_bitmap = bitmap_bytes;
    bitmap_bytes += kBitmapPoison;
    CHAOS_IL2CPP_SIZE total_size = sizeof(OldGenPage) + bitmap_bytes + payload_size;

    // Helper: carve payload into size-class free blocks
    // (used for both new and recycled pages).
    auto carve_free_lists = [](OldGenPage* p, int pref_sc) {
        char* payload = p->Payload();
        CHAOS_IL2CPP_SIZE remaining = p->payload_size;
        CHAOS_IL2CPP_SIZE n_sc = kOldGenNumSizeClasses;

        if (pref_sc >= 0 && pref_sc < n_sc) {
            CHAOS_IL2CPP_SIZE pref_size = kOldGenSizeClasses[pref_sc];
            CHAOS_IL2CPP_SIZE pref_budget = (p->payload_size * 8) / 10;
            while (remaining >= pref_size && pref_budget >= pref_size) {
                auto* block = reinterpret_cast<OldGenFreeBlock*>(payload);
                block->next = p->free_lists[pref_sc];
                block->sentinel = GcLayoutRegistry::Instance().GetSentinelTypeInfo(pref_sc);
                p->free_lists[pref_sc] = block;
                payload += pref_size;
                remaining -= pref_size;
                pref_budget -= pref_size;
            }
        }
        int sc = 0;
        while (remaining >= kOldGenSizeClasses[0]) {
            CHAOS_IL2CPP_SIZE sc_size = kOldGenSizeClasses[sc];
            if (sc_size <= remaining) {
                auto* block = reinterpret_cast<OldGenFreeBlock*>(payload);
                block->next = p->free_lists[sc];
                block->sentinel = GcLayoutRegistry::Instance().GetSentinelTypeInfo(sc);
                p->free_lists[sc] = block;
                payload += sc_size;
                remaining -= sc_size;
            }
            sc = (sc + 1) % kOldGenNumSizeClasses;
        }
    };

    // Try to recycle a pooled page first (avoids VirtualAlloc syscall).
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!page_pool_.empty()) {
            auto* recycled = page_pool_.back();
            page_pool_.pop_back();

            // Re-initialize non-permanent header fields.
            recycled->scanning = scanning;
            recycled->preferred_sc_idx = (preferred_sc_idx >= 0 && preferred_sc_idx < kOldGenNumSizeClasses)
                ? static_cast<int8_t>(preferred_sc_idx) : static_cast<int8_t>(-1);
            recycled->in_use.store(true, std::memory_order_release);

            // Clear bitmap (permanent fields like page_size/payload_size are unchanged).
            GcMarkBitmap(recycled->MarkBitmap(), recycled->bitmap_bytes).Clear();

            // Clear free list heads and re-carve.
            for (int i = 0; i < kOldGenNumSizeClasses; i++)
                recycled->free_lists[i] = nullptr;
            carve_free_lists(recycled, recycled->preferred_sc_idx);

            // Link into page list and rebuild the page array index.
            recycled->next = page_list_;
            page_list_ = recycled;
            page_count_++;
            g_gc_scheduler.RecordPageCountGrowth(1);
            RebuildPageArray();

            return recycled;
        }
    }

    auto* mem = static_cast<OldGenPage*>(VirtualAllocPage(total_size));
    if (mem == nullptr) return nullptr;

    // Initialize page header.
    mem->next = nullptr;
    mem->page_size = total_size;
    mem->payload_size = payload_size;
    mem->bitmap_bytes = bitmap_bytes;
    mem->scanning = scanning;
    mem->in_use.store(true, std::memory_order_release);

    // Mark oversized pages so sweep handles them differently.
    // A page is oversized only when holding a single large object (>32KB).
    // Normal multi-object pages at kOldGenPageSize(64KB) must NOT be oversized.
    mem->is_oversized = (payload_size > kOldGenMaxInline) && (payload_size != kOldGenPageSize);

    // Store preferred size class so MarkObject can mark all slots of each object
    // (the bitmap is 1 bit per 8-byte slot; multi-slot objects need multiple bits).
    mem->preferred_sc_idx = (preferred_sc_idx >= 0 && preferred_sc_idx < kOldGenNumSizeClasses)
        ? static_cast<int8_t>(preferred_sc_idx) : static_cast<int8_t>(-1);

    // Initialize free lists.
    for (int i = 0; i < kOldGenNumSizeClasses; i++) {
        mem->free_lists[i] = nullptr;
    }

    // Clear mark bitmap.
    GcMarkBitmap(mem->MarkBitmap(), bitmap_bytes).Clear();

    // Set poison pattern after the raw bitmap region to detect overflow.
    // In CHECK builds, MarkRange verifies the poison is intact after each
    // marking operation.  Overflow detected �� CHAOS_IL2CPP_ASSERT fires.
#if defined(CHAOS_IL2CPP_DEBUG)
    if (raw_bitmap + kBitmapPoison <= bitmap_bytes) {
        std::memset(mem->MarkBitmap() + raw_bitmap, 0xCD, kBitmapPoison);
    }
#endif

    // Carve payload into size-class free blocks.
    carve_free_lists(mem, preferred_sc_idx);
    // Link into page list.
    {
        std::lock_guard<std::mutex> lock(mutex_);
        mem->next = page_list_;
        page_list_ = mem;
        page_count_++;
        g_gc_scheduler.RecordPageCountGrowth(1);

        if (heap_base_ == 0) {
            heap_base_ = reinterpret_cast<uintptr_t>(mem);
            // Align card table heap_base to the actual first page address.
            GcSetHeapBase(reinterpret_cast<void*>(heap_base_));
        }

        // Register page with the two-level card table so write barriers
        // can find L2 segments for objects allocated in this page.
        GcRegisterHeapRange(reinterpret_cast<uintptr_t>(mem),
                            reinterpret_cast<uintptr_t>(mem) + total_size);
        RebuildPageArray();
    }

    total_allocated_.fetch_add(total_size, std::memory_order_relaxed);
    return mem;
}

void MarkSweepOldGen::FreePage(OldGenPage* page) {
    if (page == nullptr) return;
    VirtualFreePage(page, page->page_size);
}

// ���� Sorted page index (doubly-buffered for lock-free reads) ������������

void MarkSweepOldGen::RebuildPageArray() {
    // Collect all in-use pages.
    int count = 0;
    for (auto* p = page_list_; p != nullptr; p = p->next) {
        if (p->in_use.load(std::memory_order_acquire)) count++;
    }

    auto* new_pages = static_cast<OldGenPage**>(
        std::malloc(static_cast<size_t>(count) * sizeof(OldGenPage*)));
    if (new_pages == nullptr) return;

    int idx = 0;
    for (auto* p = page_list_; p != nullptr; p = p->next) {
        if (p->in_use.load(std::memory_order_acquire)) {
            new_pages[idx++] = p;
        }
    }
    // Sort by page address ascending.
    std::sort(new_pages, new_pages + count,
        [](OldGenPage* a, OldGenPage* b) { return a < b; });

    auto* new_array = new PageArray{new_pages, count};
    auto* old_array = page_array_.exchange(new_array, std::memory_order_acq_rel);

    // Ring-buffer based deferred free: retire the old array and free the
    // array from 2 slots ago.  This gives any in-flight reader that loaded
    // page_array_ before the exchange at least 2 more rebuilds before the
    // array is freed — sufficient for microsecond-range lock-free reads.
    int retire_slot = retired_ring_idx_ % kRetiredRingSize;
    if (retired_ring_[retire_slot] != nullptr) {
        std::free(retired_ring_[retire_slot]->pages);
        delete retired_ring_[retire_slot];
    }
    retired_ring_[retire_slot] = old_array;
    retired_ring_idx_ = (retire_slot + 1) % kRetiredRingSize;
}

OldGenPage* MarkSweepOldGen::FindPage(const void* ptr) const {
    if (ptr == nullptr) return nullptr;
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);

    auto* arr = page_array_.load(std::memory_order_acquire);
    if (arr == nullptr || arr->count == 0) {
        // Fallback: linear scan during initialization.
        auto* page = page_list_;
        while (page != nullptr) {
            uintptr_t start = reinterpret_cast<uintptr_t>(page);
            uintptr_t end = start + page->page_size;
            if (addr >= start && addr < end) return page;
            page = page->next;
        }
        return nullptr;
    }

    // Binary search on sorted page addresses.
    int lo = 0, hi = arr->count - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        auto* page = arr->pages[mid];
        if (page == nullptr) {
            CHAOS_IL2CPP_LOG_WARN("OldGen", "null_page_in_array");
            break;
        }
        uintptr_t start = reinterpret_cast<uintptr_t>(page);
        uintptr_t end = start + page->page_size;
        if (addr < start) {
            hi = mid - 1;
        } else if (addr >= end) {
            lo = mid + 1;
        } else {
            return page;
        }
    }

    // Linear fallback: pages allocated since last RebuildPageArray
    // are not in the binary-search index.  Walk the linked list.
    auto* page = page_list_;
    while (page != nullptr) {
        uintptr_t start = reinterpret_cast<uintptr_t>(page);
        uintptr_t end = start + page->page_size;
        if (addr >= start && addr < end) return page;
        page = page->next;
    }
    return nullptr;
}

bool MarkSweepOldGen::IsInOldGen(const void* ptr) const {
    if (ptr == nullptr) return false;
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);

    auto* arr = page_array_.load(std::memory_order_acquire);
    if (arr == nullptr || arr->count == 0) {
        // Fallback: linear scan during initialization.
        auto* page = page_list_;
        while (page != nullptr) {
            uintptr_t start = reinterpret_cast<uintptr_t>(page->Payload());
            uintptr_t end = start + page->payload_size;
            if (addr >= start && addr < end) return true;
            page = page->next;
        }
        return false;
    }

    int lo = 0, hi = arr->count - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        auto* page = arr->pages[mid];
        if (page == nullptr) break;
        uintptr_t start = reinterpret_cast<uintptr_t>(page->Payload());
        uintptr_t end = start + page->payload_size;
        if (addr < start) {
            hi = mid - 1;
        } else if (addr >= end) {
            lo = mid + 1;
        } else {
            return true;
        }
    }
    // Linear fallback: pages allocated since last RebuildPageArray
    // are not in the binary-search index.
    auto* page = page_list_;
    while (page != nullptr) {
        uintptr_t start = reinterpret_cast<uintptr_t>(page->Payload());
        uintptr_t end = start + page->payload_size;
        if (addr >= start && addr < end) return true;
        page = page->next;
    }
    return false;
}

uint64_t MarkSweepOldGen::DiagCountOxFFBytes() const {
    uint64_t count = 0;
    auto* page = page_list_;
    while (page != nullptr) {
        if (page->in_use.load(std::memory_order_acquire) && !page->is_oversized) {
            auto* payload = reinterpret_cast<const uint8_t*>(page->Payload());
            for (CHAOS_IL2CPP_SIZE i = 0; i < page->payload_size; i++) {
                if (payload[i] == 0xFF) count++;
            }
        }
        page = page->next;
    }
    return count;
}

int MarkSweepOldGen::DiagProtectPayloads() {
    int count = 0;
    auto* page = page_list_;
    while (page != nullptr) {
        if (page->in_use.load(std::memory_order_acquire) && !page->is_oversized) {
            void* payload = page->Payload();
            DWORD old;
            if (VirtualProtect(payload, page->payload_size, PAGE_READONLY, &old)) {
                count++;
            }
        }
        page = page->next;
    }
    return count;
}

void MarkSweepOldGen::DiagUnprotectPayloads() {
    auto* page = page_list_;
    while (page != nullptr) {
        if (page->in_use.load(std::memory_order_acquire) && !page->is_oversized) {
            void* payload = page->Payload();
            DWORD old;
            VirtualProtect(payload, page->payload_size, PAGE_READWRITE, &old);
        }
        page = page->next;
    }
}

bool MarkSweepOldGen::IsMarked(const void* obj) const {
    if (obj == nullptr) return false;
    auto* page = FindPage(obj);
    if (page == nullptr) return false;
    uintptr_t offset = reinterpret_cast<uintptr_t>(obj)
        - reinterpret_cast<uintptr_t>(page->Payload());
    CHAOS_IL2CPP_SIZE slot = offset / sizeof(void*);
    auto bm = GcMarkBitmap(const_cast<unsigned char*>(page->MarkBitmap()),
                            page->bitmap_bytes);
    return bm.TestSlot(slot);
}

void MarkSweepOldGen::AddToMarkStack(void* obj) {
    if (obj == nullptr) return;
    std::lock_guard<std::mutex> lock(mutex_);
    mark_stack_.push_back(obj);
}

// ======================================================================
// Size class helper
// ======================================================================

int MarkSweepOldGen::SizeClassIndex(CHAOS_IL2CPP_SIZE size) const {
    if (size > kOldGenMaxInline) return -1;

    for (int i = 0; i < kOldGenNumSizeClasses; i++) {
        if (kOldGenSizeClasses[i] >= size) return i;
    }
    return -1;
}

// ======================================================================
// Allocation
// ======================================================================

void* MarkSweepOldGen::TryAllocateFromFreeLists(CHAOS_IL2CPP_SIZE size, int sc_idx) {
    if (sc_idx < 0) return nullptr;

    std::lock_guard<std::mutex> lock(mutex_);

    // Check the cached last-used page first �� avoids O(n) walk.
    auto* page = last_alloc_page_[sc_idx];
    if (page != nullptr && page->in_use.load(std::memory_order_relaxed) &&
        !page->sweep_lock.load(std::memory_order_relaxed)) {
        if (page->free_lists[sc_idx] != nullptr) {
            auto* block = page->free_lists[sc_idx];
            page->free_lists[sc_idx] = block->next;
            std::memset(block, 0, size);
            return block;
        }
    }

    // Walk from the head of the page list.
    page = page_list_;
    while (page != nullptr) {
        if (page->free_lists[sc_idx] != nullptr && page->in_use.load(std::memory_order_relaxed) &&
            !page->sweep_lock.load(std::memory_order_relaxed)) {
            // Update the cache for this size class.
            last_alloc_page_[sc_idx] = page;
            auto* block = page->free_lists[sc_idx];
            page->free_lists[sc_idx] = block->next;
            std::memset(block, 0, size);
            return block;
        }
        page = page->next;
    }
    return nullptr;
}

void* MarkSweepOldGen::Allocate(CHAOS_IL2CPP_SIZE size, bool scanning_required) {
    CHAOS_IL2CPP_PROFILE_SCOPE("OldGen::Allocate");

    if (size == 0) return nullptr;

    // Auto-initialize on first use if Init() wasn't called explicitly.
    // Use an atomic flag to prevent concurrent auto-init races.
    // NOTE: init_mutex_ is separate from mutex_ because Init() calls
    // AllocatePage() which takes mutex_ internally �� using mutex_ here
    // would deadlock (std::mutex is non-recursive).
    if (!initialized_.load(std::memory_order_acquire)) {
        std::lock_guard<std::mutex> lock(init_mutex_);
        if (!initialized_.load(std::memory_order_acquire)) {
            Init(0, 2);
            initialized_.store(true, std::memory_order_release);
        }
    }

    // Oversized: route to Large Object Heap or direct page allocation.
    if (size > kOldGenMaxInline) {
        // Route objects > 85 KB to the Large Object Heap (no compaction).
        if (size > kLohThreshold) {
            void* loh_ptr = g_loh.Allocate(size);
            if (loh_ptr != nullptr) {
                GcRecordAlloc(size, true);
                memory_domain::GcTrackDomainAlloc(size);
            }
            return loh_ptr;
        }

        // 32-85 KB: oversized page allocation.

        // Defer full GC to next safepoint instead of blocking synchronously.
        // The page pool can absorb allocation bursts without GC; blocking here
        // penalizes transient oversized allocations with unnecessary STW pauses.
        g_gc_scheduler.RequestFullGc();

        auto* page = AllocatePage(size, scanning_required);
        if (page == nullptr) return nullptr;
        auto* result = page->Payload();
        std::memset(result, 0, size);
        GcRecordAlloc(size, true);
        memory_domain::GcTrackDomainAlloc(size);
        return result;
    }

    // Align up to 8 bytes (matches size classes).
    size = (size + 7) & ~static_cast<CHAOS_IL2CPP_SIZE>(7);

    // Size class index.
    int sc_idx = SizeClassIndex(size);
    if (sc_idx < 0) {
        // Fallback: allocate a new page and carve from it.
        auto* page = AllocatePage(kOldGenPageSize, scanning_required);
        if (page == nullptr) return nullptr;
        sc_idx = SizeClassIndex(size);
        if (sc_idx < 0) {
            // Still no match (shouldn't happen for aligned < 32KB).
            auto* result = page->Payload();
            GcRecordAlloc(size, false);
            memory_domain::GcTrackDomainAlloc(size);
            return result;
        }
    }

    // Try free lists.
    auto* ptr = TryAllocateFromFreeLists(size, sc_idx);
    if (ptr != nullptr) {
        GcRecordAlloc(size, false);
        memory_domain::GcTrackDomainAlloc(size);
        return ptr;
    }

    // Miss: allocate a new page, hinting the missing size class so the
    // carve logic fills most of the page with blocks of this size.
    auto* page = AllocatePage(kOldGenPageSize, scanning_required, sc_idx);
    if (page == nullptr) return nullptr;

    // Pop from the newly carved page's freelist directly, avoiding a full
    // TryAllocateFromFreeLists walk (which would re-acquire mutex_ and scan
    // the entire page_list_ for an entry we know exists on this new page).
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (page->free_lists[sc_idx] != nullptr) {
            auto* block = page->free_lists[sc_idx];
            page->free_lists[sc_idx] = block->next;
            std::memset(block, 0, size);
            last_alloc_page_[sc_idx] = page;
            GcRecordAlloc(size, false);
            memory_domain::GcTrackDomainAlloc(size);
            return block;
        }
    }

    // Fallback: retry free list walk (should be rare — new page was just carved).
    ptr = TryAllocateFromFreeLists(size, sc_idx);
    if (ptr != nullptr) {
        GcRecordAlloc(size, false);
        memory_domain::GcTrackDomainAlloc(size);
    }
    return ptr;
}

void MarkSweepOldGen::Free(void* ptr) {
    if (ptr == nullptr) return;

    // Oversized pages: unlink from page list and virtual-free immediately.
    // Take the mutex first to protect the FindPage �� page_list_ traversal
    // from concurrent page_list_ mutations during GC Collect().
    std::lock_guard<std::mutex> lock(mutex_);

    auto* page = FindPage(ptr);
    if (page == nullptr) {
        CHAOS_IL2CPP_LOG_WARN_M("OldGen", "free_unknown_ptr {0}", ptr);
        return;
    }

    if (page->is_oversized) {
        OldGenPage** pp = &page_list_;
        while (*pp != nullptr) {
            if (*pp == page) {
                *pp = page->next;
                page_count_--;
                break;
            }
            pp = &(*pp)->next;
        }

        // Mark as not-in-use so BgcSweep's in_use pre-check skips this page.
        // Defer the actual VirtualFree to the next safepoint (Collect or
        // BgcSweep Phase 4 free loop) so BgcSweep's page snapshot (taken
        // under mutex_ before the free loop) does not access freed memory.
        page->in_use.store(false, std::memory_order_release);
        deferred_free_pages_.push_back(page);
        RebuildPageArray();
        return;
    }

    // Non-oversized: zero the memory and let GC sweep handle reclamation.
    // We do NOT touch the free-list here �� adding the block back would
    // create a double-free hazard when SweepPage also sees the unmarked
    // bitmap bits and adds it back.  SweepPage is the sole reclamation
    // path for non-oversized blocks.
    //
    // We zero enough to break any stale TypeInfo* reference and prevent
    // accidental pointer retention through the mark phase.  Full-page
    // memset happens at page carve time.
    std::memset(ptr, 0, 64);
}

void* MarkSweepOldGen::Reallocate(void* ptr, CHAOS_IL2CPP_SIZE new_size) {
    if (ptr == nullptr) return Allocate(new_size, true);
    if (new_size == 0) { Free(ptr); return nullptr; }

    // We don't track individual block sizes precisely at C3,
    // so always allocate new + copy (heuristic using page payload as bound).
    auto* new_ptr = Allocate(new_size, true);
    if (new_ptr == nullptr) return nullptr;

    // FindPage needs mutex_ protection to avoid racing with page_list_ mutations.
    CHAOS_IL2CPP_SIZE old_size = new_size;
    bool oversized = false;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto* page = FindPage(ptr);
        if (page != nullptr) {
            auto addr = reinterpret_cast<uintptr_t>(ptr);
            auto pstart = reinterpret_cast<uintptr_t>(page->Payload());
            auto remaining = page->payload_size - (addr - pstart);
            old_size = std::min(remaining, new_size);
            oversized = page->is_oversized;
        }
    }
    std::memcpy(new_ptr, ptr, old_size);

    if (oversized) {
        Free(ptr);
    }

    return new_ptr;
}

// ======================================================================
// GC Collection �� Mark phase
// ======================================================================

/// Compute the size of the size-class block at a given payload offset within a page.
/// Simulates the deterministic carve from AllocatePage. Returns the block size in bytes,
/// or 0 if the offset is invalid. Uses the page's preferred_sc_idx for O(1) fast path
/// in the preferred-class region.
static CHAOS_IL2CPP_SIZE BlockSizeAtPayloadOffset(const OldGenPage* page,
                                                   CHAOS_IL2CPP_SIZE offset) {
    if (offset >= page->payload_size) return 0;
    if (page->is_oversized) return page->payload_size;

    int pref_sc = page->preferred_sc_idx;
    if (pref_sc >= 0 && pref_sc < kOldGenNumSizeClasses) {
        CHAOS_IL2CPP_SIZE pref_size = kOldGenSizeClasses[pref_sc];
        CHAOS_IL2CPP_SIZE pref_region = ((page->payload_size * 8) / 10) / pref_size * pref_size;
        if (offset < pref_region) return pref_size;
    }

    // Round-robin region: simulate the carve from the end of the preferred region.
    CHAOS_IL2CPP_SIZE cursor = 0;
    if (pref_sc >= 0 && pref_sc < kOldGenNumSizeClasses) {
        CHAOS_IL2CPP_SIZE pref_size = kOldGenSizeClasses[pref_sc];
        CHAOS_IL2CPP_SIZE pref_region = ((page->payload_size * 8) / 10) / pref_size * pref_size;
        cursor = pref_region;
    }

    int sc = 0;
    while (cursor + kOldGenSizeClasses[0] <= page->payload_size) {
        CHAOS_IL2CPP_SIZE sc_size = kOldGenSizeClasses[sc];
        if (cursor + sc_size <= page->payload_size) {
            if (offset < cursor + sc_size) return sc_size;
            cursor += sc_size;
        }
        sc = (sc + 1) % kOldGenNumSizeClasses;
    }
    return 0;
}

// AtomicMarkRange replaced by GcMarkBitmap::MarkRange (gc_mark_bitmap.h).

bool MarkSweepOldGen::MarkObject(void* obj) {
    if (obj == nullptr) return false;

    auto* page = FindPage(obj);
    if (page == nullptr) {
        return false;
    }

    auto obj_addr = reinterpret_cast<uintptr_t>(obj);
    auto payload_start = reinterpret_cast<uintptr_t>(page->Payload());
    if (obj_addr < payload_start) return false;

    // Align check: address must be pointer-aligned (valid object start).
    if (obj_addr % sizeof(void*) != 0) return false;

    CHAOS_IL2CPP_SIZE offset = obj_addr - payload_start;
    if (offset >= page->payload_size) return false;

    // PrecisionMark: read the TypeInfo pointer from the object's first word
    // to determine the precise instance_size from GcTypeLayout.  This replaces
    // the previous BlockSizeAtPayloadOffset approach which used size-class
    // estimates and caused over-marking when a freed size-class block was
    // reused for a smaller type.
    //
    // Sentinel check: free blocks carry a sentinel TypeInfo pointer written
    // by SweepPage/CoalescePage/AllocatePage.  IsSentinelStableId detects
    // these and returns false — we never mark free blocks as reachable.
    const void* type_info_ptr = *static_cast<const void* const*>(obj);
    if (type_info_ptr == nullptr) return false;

    auto& layout_registry = GcLayoutRegistry::Instance();
    if (!layout_registry.IsValidTypeInfoPointer(type_info_ptr)) return false;

    uint64_t stable_id = layout_registry.ReadStableId(type_info_ptr);
    if (stable_id == 0) return false;

    // Skip sentinel free blocks (stable_id in the reserved sentinel range).
    if (IsSentinelStableId(stable_id)) return false;

    const auto* layout = layout_registry.Lookup(stable_id);
    if (layout == nullptr || layout->instance_size <= 0) return false;

    CHAOS_IL2CPP_SIZE num_slots = layout->instance_size / sizeof(void*);
    if (num_slots < 1) num_slots = 1;

    CHAOS_IL2CPP_SIZE slot_idx = offset / sizeof(void*);

    bool newly_set = GcMarkBitmap(page->MarkBitmap(), page->bitmap_bytes)
                         .MarkRange(slot_idx, num_slots);
    if (newly_set) {
        marked_count_.fetch_add(1, std::memory_order_relaxed);
    }
    return newly_set;
}

void MarkSweepOldGen::DrainMarkStack() {
    auto& layout_registry = GcLayoutRegistry::Instance();

    // Unbounded mark stack: no overflow possible after Phase 3 parallel mark.
    // DrainMarkStack is used only for the finalizer re-mark pass and small-heap
    // sequential collection, both of which have bounded object graphs.
    while (!mark_stack_.empty()) {
        void* obj = mark_stack_.back();
        mark_stack_.pop_back();

        // Use precise GC layout if available.
        auto* page = FindPage(obj);
        if (page == nullptr) continue;

        // Read TypeInfo* from first word.
        const void* type_info_ptr = *static_cast<const void* const*>(obj);
        if (type_info_ptr == nullptr) continue;

        // Check if the first word is a valid TypeInfo pointer and look up layout.
        if (!layout_registry.IsValidTypeInfoPointer(type_info_ptr)) {
            // Not a valid TypeInfo �� conservative fallback.
            CHAOS_IL2CPP_SIZE obj_size = sizeof(void*);
            auto obj_addr = reinterpret_cast<uintptr_t>(obj);
            auto payload_start = reinterpret_cast<uintptr_t>(page->Payload());
            CHAOS_IL2CPP_SIZE offset = static_cast<CHAOS_IL2CPP_SIZE>(obj_addr - payload_start);
            CHAOS_IL2CPP_SIZE payload_remaining = page->payload_size - offset;
            CHAOS_IL2CPP_SIZE max_size = kOldGenSizeClasses[kOldGenNumSizeClasses - 1];
            if (payload_remaining < max_size) max_size = payload_remaining;

            for (CHAOS_IL2CPP_SIZE slot_off = 0; slot_off + sizeof(void*) <= max_size; slot_off += sizeof(void*)) {
                auto* slot = reinterpret_cast<void**>(static_cast<char*>(obj) + slot_off);
                void* ref = *slot;
                if (ref != nullptr && FindPage(ref) != nullptr) {
                    if (MarkObject(ref)) {
                        mark_stack_.push_back(ref);
                    }
                }
            }
            continue;
        }

        // Valid TypeInfo �� look up the precise GC layout.
        uint64_t stable_id = layout_registry.ReadStableId(type_info_ptr);
        if (stable_id == 0) continue;
        const auto* layout = layout_registry.Lookup(stable_id);

        if (layout == nullptr || layout->pointer_count == 0) {
            continue;
        }

        // Precise scan: only the pointer offsets declared in the layout.
        uintptr_t obj_base = reinterpret_cast<uintptr_t>(obj);
        for (uint16_t i = 0; i < layout->pointer_count; i++) {
            uint16_t offset = layout->pointer_offsets[i].offset;
            auto* slot = reinterpret_cast<void**>(obj_base + offset);
            void* ref = *slot;

            if (ref != nullptr && FindPage(ref) != nullptr) {
                if (MarkObject(ref)) {
                    mark_stack_.push_back(ref);
                }
            }
        }
    }
}

namespace {

/// Seed worker 0's deque from the sequential mark stack.
/// Converts each object pointer to a MarkChunk (page_idx, bitmap_word).
static void SeedParallelDeque(ParallelMarkContext* ctx,
                               const std::vector<void*>& mark_stack) {
    auto* w0 = &ctx->workers[0];
    for (void* obj : mark_stack) {
        // Find page index by scanning page array.
        // In practice, objects are typically in recently-allocated pages,
        // so this is faster than it looks (cache hot).
        for (int pi = 0; pi < ctx->page_count; pi++) {
            auto* p = ctx->pages[pi];
            if (p == nullptr) continue;
            uintptr_t p_start = reinterpret_cast<uintptr_t>(p->Payload());
            uintptr_t p_end = p_start + p->payload_size;
            uintptr_t addr = reinterpret_cast<uintptr_t>(obj);
            if (addr >= p_start && addr < p_end) {
                CHAOS_IL2CPP_SIZE offset = addr - p_start;
                CHAOS_IL2CPP_SIZE slot = offset / sizeof(void*);
                uint64_t bit = static_cast<uint64_t>(1) << (slot % 64);
                uint16_t word = static_cast<uint16_t>(slot / 64);
                if (w0->has_pending && w0->pending.page_idx == pi &&
                    w0->pending.word_index == word) {
                    w0->pending.bitmap |= bit;
                } else {
                    FlushPending(w0);
                    w0->pending.page_idx = pi;
                    w0->pending.word_index = word;
                    w0->pending.bitmap = bit;
                    w0->has_pending = true;
                }
                break;
            }
        }
    }
    FlushPending(w0);
}

}  // anonymous namespace

void MarkSweepOldGen::DrainMarkStackParallel(OldGenPage** pages, int page_count) {
    CHAOS_IL2CPP_PROFILE_SCOPE("OldGen::DrainMarkStackParallel");

    if (page_count <= 0 || pages == nullptr) return;

    // Determine worker count.
    int hw_concurrency = static_cast<int>(std::thread::hardware_concurrency());
    if (hw_concurrency < 1) hw_concurrency = 1;

    auto* ctx = InitParallelMarkContext(pages, page_count, hw_concurrency);
    if (ctx == nullptr) {
        // Fall back to sequential.
        CHAOS_IL2CPP_LOG_WARN("OldGen", "parallel_mark_oom_fallback");
        DrainMarkStack();
        return;
    }

    // Seed worker 0's deque from our existing mark stack.
    if (!mark_stack_.empty()) {
        SeedParallelDeque(ctx, mark_stack_);
        mark_stack_.clear();
    }

    // Spawn thread pool workers (1..N-1).
    // Signal all workers to start before dispatching.
    ctx->drain_started.store(true, std::memory_order_release);

    // Use GcWorkerPool for parallel mark (not ThreadPool �� ThreadPool
    // workers are registered managed threads that spin in SafepointPoll
    // and would deadlock when called inside a safepoint).
    GcWorkerPool::Instance().RunWorkers(ctx->worker_count, [ctx](int idx) {
        ParallelMarkWorkerLoop(ctx, idx);
    });

    // Drain any remaining chunks left in deques (workers may have been preempted).
    // As a safety net, process any remaining mark entries sequentially.
    for (int i = 0; i < ctx->worker_count; i++) {
        MarkChunk chunk;
        while (PopChunk(&ctx->workers[i], &chunk)) {
            if (chunk.page_idx >= 0 && chunk.page_idx < page_count) {
                auto* page = pages[chunk.page_idx];
                if (page == nullptr) continue;
                char* payload = page->Payload();
                auto& layout_registry = GcLayoutRegistry::Instance();
                uint64_t word = chunk.bitmap;
                while (word != 0) {
                    int bit = GcCtz64(word);
                    word &= word - 1;
                    CHAOS_IL2CPP_SIZE slot_idx = static_cast<CHAOS_IL2CPP_SIZE>(
                        static_cast<CHAOS_IL2CPP_SIZE>(chunk.word_index) * 64 + bit);
                    void* obj = payload + slot_idx * sizeof(void*);
                    const void* type_info_ptr = *static_cast<const void* const*>(obj);
                    if (type_info_ptr == nullptr) continue;
                    if (!layout_registry.IsValidTypeInfoPointer(type_info_ptr)) continue;
                    uint64_t stable_id = layout_registry.ReadStableId(type_info_ptr);
                    if (stable_id == 0) continue;
                    const auto* layout = layout_registry.Lookup(stable_id);
                    if (layout == nullptr || layout->pointer_count == 0) continue;
                    uintptr_t obj_base = reinterpret_cast<uintptr_t>(obj);
                    for (uint16_t j = 0; j < layout->pointer_count; j++) {
                        uint16_t offset = layout->pointer_offsets[j].offset;
                        auto* slot = reinterpret_cast<void**>(obj_base + offset);
                        void* ref = *slot;
                        if (ref == nullptr) continue;
                        int ref_pi = FindPageIndexByAddr(ctx, reinterpret_cast<uintptr_t>(ref));
                        if (ref_pi < 0) continue;
                        auto* rp = pages[ref_pi];
                        uintptr_t ref_offset = reinterpret_cast<uintptr_t>(ref) -
                            reinterpret_cast<uintptr_t>(rp->Payload());
                        CHAOS_IL2CPP_SIZE ref_slot = ref_offset / sizeof(void*);
                        CHAOS_IL2CPP_SIZE ref_byte = ref_slot / 8;
                        int ref_bit = static_cast<int>(ref_slot % 8);
                        unsigned char ref_mask = static_cast<unsigned char>(1u << ref_bit);
                        if (MarkObject(ref)) {
                            marked_count_.fetch_add(1, std::memory_order_relaxed);
                        }
                    }
                }
            }
        }
    }

    // Reclaim retired GcLayout tables (safe here since STW).
    GcLayoutRegistry::Instance().ReclaimRetiredTables();

    DestroyParallelMarkContext(ctx);

}

void MarkSweepOldGen::HandleReMarkPass() {
    // Iterate GCHandle table and mark all reachable objects as roots,
    // then drain the mark stack to pick up the transitive closure of any
    // objects made reachable by finalizers.
    GcIterateHandleTable([](void* object, void* user_data) {
        auto* self = static_cast<MarkSweepOldGen*>(user_data);
        if (object != nullptr) {
            if (self->MarkObject(object)) {
                self->mark_stack_.push_back(object);
            }
        }
    }, this);
    DrainMarkStack();

    // Re-mark LOH objects reachable from GCHandles.  Finalizers can
    // resurrect LOH objects (e.g., by storing 'this' into a static field
    // or registering a new GCHandle).  Without this re-mark, resurrected
    // LOH objects would be swept by g_loh.Sweep() later in Phase 4.
    if (g_loh.SegmentCount() > 0) {
        GcIterateHandleTable([](void* object, void* user_data) {
            (void)user_data;
            if (object != nullptr) {
                g_loh.MarkObject(object);
            }
        }, nullptr);
    }

    // Phase 3c: Process dependent handles (ConditionalWeakTable).
    // Fixed-point iteration: if primary is alive, keep secondary alive.
    // Runs after finalizer re-mark so resurrected objects are visible.
    int dep_kept = GcProcessDependentHandlesAfterFullGC();
    if (dep_kept > 0) {
        // New objects were marked �� drain the mark stack.
        DrainMarkStack();
        // Re-mark LOH objects reachable from dependent handles.
        if (g_loh.SegmentCount() > 0 && g_loh.Sweep() > 0) {
            // LOH objects were kept alive; no need to subtract from reclaimed.
        }
    }
}

// ======================================================================
// GC Collection �� Sweep phase
// ======================================================================

CHAOS_IL2CPP_SIZE MarkSweepOldGen::SweepPage(OldGenPage* page, bool clear_bitmap) {
    if (page == nullptr) return 0;

    // Acquire sweep_lock (per-page spinlock for concurrent sweep).
    int spin_count = 0;
    while (page->sweep_lock.load(std::memory_order_relaxed) ||
           page->sweep_lock.exchange(true, std::memory_order_acquire)) {
        CHAOS_OLDGEN_SPIN_HINT();
        spin_count++;
        if (spin_count > 100000000) {
            printf("  SWEEP_DEADLOCK: page=%p spin_count=%d sweep_lock=%d ptr=0x%x\n",
                   (void*)page, spin_count,
                   page->sweep_lock.load(std::memory_order_relaxed) ? 1 : 0,
                   *(int*)page);
            break;
        }
    }

    // After acquiring sweep_lock, check in_use.  A concurrent Free()
    // for an oversized page may have set in_use = false and is waiting
    // for sweep_lock to be released so it can VirtualFree.  If we
    // detect !in_use here, skip the page �� the memory will be freed
    // as soon as we release the lock.
    if (!page->in_use.load(std::memory_order_acquire)) {
        page->sweep_lock.store(false, std::memory_order_release);
        return 0;
    }

    // Oversized pages: single-object.  If nothing is marked, the entire page
    // is garbage �� return its size as reclaimed.  Mark it as not-in-use so
    // Collect() Phase 4 can free it.
    if (page->is_oversized) {
        auto bm = GcMarkBitmap(page->MarkBitmap(), page->bitmap_bytes);
        bool any_marked = bm.AnySet();
        if (clear_bitmap) {
            bm.Clear();
        }
        if (!any_marked) {
            page->sweep_lock.store(false, std::memory_order_release);
            // Mark for decommission �� Collect() Phase 4 will free this page.
            page->in_use.store(false, std::memory_order_release);
            return page->payload_size;
        }
        page->sweep_lock.store(false, std::memory_order_release);
        return 0;
    }

    // Clear all free lists so that free blocks added by SweepPage's
    // free_run lambda form clean null-terminated chains.  During BGC
    // concurrent mark, mutator allocations may strip middle entries
    // from these chains via head-insertion/removal, leaving stale
    // next pointers at the tail.  Without this clear, CoalescePage
    // can follow a corrupted link into an infinite loop.
    for (int i = 0; i < kOldGenNumSizeClasses; i++) {
        page->free_lists[i] = nullptr;
    }

    char* payload = page->Payload();
    CHAOS_IL2CPP_SIZE reclaimed = 0;

    // Walk the bitmap 64 bits (one uint64_t word) at a time.
    // Each bit corresponds to one pointer-sized slot (8 bytes).
    auto bm = GcMarkBitmap(page->MarkBitmap(), page->bitmap_bytes);
    auto* bitmap = bm.Words();
    CHAOS_IL2CPP_SIZE num_words = bm.WordCount();
    // Cap to actual payload slots (bitmap_bytes includes poison guard).
    CHAOS_IL2CPP_SIZE max_sweep_slots = page->payload_size / sizeof(void*);
    if (num_words * 64 > max_sweep_slots) num_words = (max_sweep_slots + 63) / 64;
    CHAOS_IL2CPP_SIZE slot = 0;  // global slot index across the page

    // Fast path: contiguous run of unmarked slots �� single free block.
    auto free_run = [&](CHAOS_IL2CPP_SIZE start_slot, CHAOS_IL2CPP_SIZE end_slot) {
        if (start_slot >= end_slot) return;
        CHAOS_IL2CPP_SIZE run_bytes = (end_slot - start_slot) * sizeof(void*);
        reclaimed += run_bytes;

        // Split the run into size-class blocks from largest to smallest,
        // so that every byte of reclaimed memory ends up on a free list.
        auto* cursor = payload + start_slot * sizeof(void*);
        CHAOS_IL2CPP_SIZE remaining = run_bytes;
        while (remaining > 0) {
            int sc_idx = SizeClassIndex(remaining);
            if (sc_idx < 0) {
                // No single size class fits the remainder �� carve off
                // the largest size class and continue with the rest.
                sc_idx = kOldGenNumSizeClasses - 1;
            }
            CHAOS_IL2CPP_SIZE sc_size = kOldGenSizeClasses[sc_idx];
            if (sc_size > remaining) {
                // Last-chance fallback: skip one slot and retry so we
                // don't leak non-size-class-aligned remainders.
                // Minimum block = sizeof(OldGenFreeBlock) = 16 bytes.
                if (remaining < sizeof(OldGenFreeBlock)) break;
                sc_size = sizeof(OldGenFreeBlock);
                sc_idx = SizeClassIndex(sc_size);
                if (sc_idx < 0) break;
            }
            auto* block = reinterpret_cast<OldGenFreeBlock*>(cursor);
            block->next = page->free_lists[sc_idx];
            block->sentinel = GcLayoutRegistry::Instance().GetSentinelTypeInfo(sc_idx);
            page->free_lists[sc_idx] = block;
            cursor += sc_size;
            remaining -= sc_size;
        }
    };

    for (CHAOS_IL2CPP_SIZE w = 0; w < num_words; w++) {
        // Prefetch the next bitmap word and corresponding payload cache line.
        if (w + 1 < num_words) {
            CHAOS_IL2CPP_PREFETCH_READ(&bitmap[w + 1]);
        }
        CHAOS_IL2CPP_PREFETCH_READ(payload + slot * sizeof(void*));

        uint64_t word = bitmap[w];

        if (word == UINT64_MAX) {
            // Fast path 1: all 64 slots marked �� nothing to free.
            slot += 64;
            continue;
        }

        if (word == 0) {
            // Fast path 2: all 64 slots unmarked �� one contiguous free block.
            free_run(slot, slot + 64);
            slot += 64;
            continue;
        }

        // Slow path: mixed �� iterate only the ZERO bits (unmarked slots).
        // ~word has 1-bits at unmarked positions, so ForEachSetBit on ~word.
        CHAOS_IL2CPP_SIZE word_base = slot;
        uint64_t unmarked = ~word;

        while (unmarked != 0) {
            int bit = GcCtz64(unmarked);
            CHAOS_IL2CPP_SIZE unmarked_slot = word_base + static_cast<CHAOS_IL2CPP_SIZE>(bit);

            // Find the end of this unmarked run.
            unmarked &= ~(static_cast<uint64_t>(1) << bit);  // clear current bit
            CHAOS_IL2CPP_SIZE run_end_slot = unmarked_slot + 1;

            while (unmarked != 0) {
                int next_bit = GcCtz64(unmarked);
                CHAOS_IL2CPP_SIZE next_unmarked = word_base + static_cast<CHAOS_IL2CPP_SIZE>(next_bit);
                if (next_unmarked != run_end_slot) break;  // non-contiguous
                run_end_slot = next_unmarked + 1;
                unmarked &= ~(static_cast<uint64_t>(1) << next_bit);
            }

            free_run(unmarked_slot, run_end_slot);
        }

        slot += 64;
    }

    // Clear bitmap for next cycle (unless preserved for BGC compaction).
    // Preserve bitmap for the current GC cycle's finalizer and
    // weak-handle processing.  Cleared at the start of the next
    // Collect() mark phase (see Collect()).
    // GcMarkBitmap(page->MarkBitmap(), page->bitmap_bytes).Clear();

        page->sweep_lock.store(false, std::memory_order_release);
    return reclaimed;
}

void MarkSweepOldGen::CoalescePage(OldGenPage* page) {
    if (page == nullptr || page->is_oversized) return;

    // CoalescePage only touches this page's own free lists �� no other thread
    // accesses them during the STW sweep phase, so no mutex is needed.
    // (During parallel sweep, each page is processed by one worker.)

    // Walk all free lists and merge adjacent blocks.
    // Strategy: for each size class, collect all free blocks,
    // sort by address, then merge contiguous blocks into larger size classes.
    //
    // For C3, we use a simple approach: collect all free block addresses,
    // sort them, then merge adjacent ones that sum to a valid size class.

    // Gather all free blocks with their addresses.
    struct BlockAddr {
        char* addr;
        CHAOS_IL2CPP_SIZE size;
    };
    std::vector<BlockAddr> blocks;
    blocks.reserve(256);

    for (int sc = 0; sc < kOldGenNumSizeClasses; sc++) {
        CHAOS_IL2CPP_SIZE sc_size = kOldGenSizeClasses[sc];
        auto* fb = page->free_lists[sc];
        while (fb != nullptr) {
            blocks.push_back({reinterpret_cast<char*>(fb), sc_size});
            fb = fb->next;
        }
        // Clear the free list �� we'll rebuild it from merged blocks.
        page->free_lists[sc] = nullptr;
    }

    if (blocks.empty()) return;

    // Sort by address.
    std::sort(blocks.begin(), blocks.end(),
        [](const BlockAddr& a, const BlockAddr& b) { return a.addr < b.addr; });

    // Merge adjacent blocks.
    std::vector<BlockAddr> merged;
    merged.reserve(blocks.size());
    merged.push_back(blocks[0]);

    for (size_t i = 1; i < blocks.size(); i++) {
        BlockAddr& last = merged.back();
        char* expected_next = last.addr + last.size;
        if (blocks[i].addr == expected_next) {
            // Adjacent �� merge.
            last.size += blocks[i].size;
        } else {
            merged.push_back(blocks[i]);
        }
    }

    // Return merged blocks to appropriate size-class free lists.
    // Split any merged block that doesn't exactly match a size class.
    for (auto& b : merged) {
        char* cursor = b.addr;
        CHAOS_IL2CPP_SIZE remaining = b.size;
        while (remaining > 0) {
            int sc_idx = SizeClassIndex(remaining);
            if (sc_idx < 0) {
                sc_idx = kOldGenNumSizeClasses - 1;
            }
            CHAOS_IL2CPP_SIZE sc_size = kOldGenSizeClasses[sc_idx];
            if (sc_size > remaining) {
                if (remaining < sizeof(OldGenFreeBlock)) break;
                sc_size = sizeof(OldGenFreeBlock);
                sc_idx = SizeClassIndex(sc_size);
                if (sc_idx < 0) break;
            }
            auto* block = reinterpret_cast<OldGenFreeBlock*>(cursor);
            block->next = page->free_lists[sc_idx];
            block->sentinel = GcLayoutRegistry::Instance().GetSentinelTypeInfo(sc_idx);
            page->free_lists[sc_idx] = block;
            cursor += sc_size;
            remaining -= sc_size;
        }
    }
}

// ======================================================================
// GC Collection �� full collect
// ======================================================================

float MarkSweepOldGen::PageFragmentation(const OldGenPage* page) const {
    if (page == nullptr || page->is_oversized) return 0.0f;
    auto bm = GcMarkBitmap(const_cast<unsigned char*>(page->MarkBitmap()),
                            page->bitmap_bytes);
    auto* bitmap = bm.Words();
    CHAOS_IL2CPP_SIZE num_words = bm.WordCount();
    CHAOS_IL2CPP_SIZE max_bm_slots = page->payload_size / sizeof(void*);
    if (num_words * 64 > max_bm_slots) num_words = (max_bm_slots + 63) / 64;
    CHAOS_IL2CPP_SIZE marked_slots = 0;
    for (CHAOS_IL2CPP_SIZE w = 0; w < num_words; w++) {
        marked_slots += static_cast<CHAOS_IL2CPP_SIZE>(GcPopCount64(bitmap[w]));
    }
    CHAOS_IL2CPP_SIZE live = marked_slots * sizeof(void*);
    float ratio = 1.0f - (static_cast<float>(live) / static_cast<float>(page->payload_size));
    return (ratio < 0.0f) ? 0.0f : ratio;
}

MarkSweepOldGen::CompactMode MarkSweepOldGen::DecideCompactMode() {
    std::lock_guard<std::mutex> lock(mutex_);

    int candidate_pages = 0;
    int total_pages = 0;
    CHAOS_IL2CPP_SIZE total_live = 0;
    CHAOS_IL2CPP_SIZE total_payload = 0;

    auto* p = page_list_;
    while (p != nullptr) {
        if (p->in_use.load(std::memory_order_acquire) && !p->is_oversized) {
            total_pages++;
            total_payload += p->payload_size;
            float frag = PageFragmentation(p);
            if (frag > 0.30f) {
                candidate_pages++;
                // Estimate live bytes from mark bitmap.
                auto bm3 = GcMarkBitmap(p->MarkBitmap(), p->bitmap_bytes);
                auto* bitmap = bm3.Words();
                CHAOS_IL2CPP_SIZE num_words = bm3.WordCount();
                CHAOS_IL2CPP_SIZE max_bm_slots = p->payload_size / sizeof(void*);
                if (num_words * 64 > max_bm_slots) num_words = (max_bm_slots + 63) / 64;
                CHAOS_IL2CPP_SIZE marked_slots = 0;
                for (CHAOS_IL2CPP_SIZE w = 0; w < num_words; w++) {
                    marked_slots += static_cast<CHAOS_IL2CPP_SIZE>(GcPopCount64(bitmap[w]));
                }
                total_live += marked_slots * sizeof(void*);
            }
        }
        p = p->next;
    }

    // V5: No live data means compaction has nothing to compact.
    if (total_live == 0) return CompactMode::NONE;
    // Skip compaction when live data is trivially small — the 5-phase
    // CrossPageCompact overhead exceeds the benefit for a few KB of movement.
    static constexpr CHAOS_IL2CPP_SIZE kMinCrossCompactBytes = 4096;
    if (total_live < kMinCrossCompactBytes) return CompactMode::NONE;
    if (candidate_pages == 0) return CompactMode::NONE;

    // Cross-page compaction if global fragmentation > 40% (most pages
    // have some waste) or there are enough candidate pages to make
    // evacuation worthwhile.
    float global_frag = (total_payload > 0)
        ? 1.0f - (static_cast<float>(total_live) / static_cast<float>(total_payload))
        : 0.0f;

    if (global_frag > kCrossPageFragThreshold || candidate_pages >= total_pages / 2) {
        return CompactMode::CROSS_PAGE;
    }

    // Otherwise, per-page compaction for individually fragmented pages.
    return CompactMode::COMPACT;
}

CHAOS_IL2CPP_SIZE MarkSweepOldGen::PlanPageCompaction(OldGenPage* page,
                                                        CompactPlan& out_plan) {
    out_plan.entries.clear();
    out_plan.live_bytes = 0;
    out_plan.saved_bytes = 0;

    if (page == nullptr || page->is_oversized) return 0;

    auto bm4 = GcMarkBitmap(page->MarkBitmap(), page->bitmap_bytes);
    auto* bitmap = bm4.Words();
    char* payload = page->Payload();
    CHAOS_IL2CPP_SIZE num_words = bm4.WordCount();
    CHAOS_IL2CPP_SIZE max_bm_slots = page->payload_size / sizeof(void*);
    if (num_words * 64 > max_bm_slots) num_words = (max_bm_slots + 63) / 64;
    CHAOS_IL2CPP_SIZE num_slots = page->payload_size / sizeof(void*);

    // First pass: collect all marked object addresses using precise layouts.
    struct MarkedObj {
        void* addr;
        CHAOS_IL2CPP_SIZE size;
    };
    std::vector<MarkedObj> marked;
    marked.reserve(1024);

    CHAOS_IL2CPP_SIZE slot = 0;
    while (slot < num_slots) {
        CHAOS_IL2CPP_SIZE w = slot / 64;
        if (w >= num_words) break;
        uint64_t word = bitmap[w];
        if (word == 0) {
            slot = (w + 1) * 64;
            continue;
        }
        uint64_t remaining = word;
        while (remaining != 0) {
            int bit = GcCtz64(remaining);
            remaining &= remaining - 1;
            CHAOS_IL2CPP_SIZE obj_slot = w * 64 + static_cast<CHAOS_IL2CPP_SIZE>(bit);
            if (obj_slot >= num_slots) break;
            void* obj = payload + obj_slot * sizeof(void*);

            CHAOS_IL2CPP_SIZE obj_size = sizeof(void*);
            const void* type_info_ptr = *static_cast<const void* const*>(obj);
            if (type_info_ptr != nullptr) {
                auto& registry = GcLayoutRegistry::Instance();
                if (registry.IsValidTypeInfoPointer(type_info_ptr)) {
                    uint64_t stable_id = registry.ReadStableId(type_info_ptr);
                    const auto* layout = registry.Lookup(stable_id);
                    if (layout != nullptr && layout->instance_size > 0) {
                        obj_size = layout->instance_size;
                    }
                }
            }

            marked.push_back({obj, obj_size});
        }
        slot = (w + 1) * 64;
    }

    if (marked.empty()) return 0;

    // Skip pages containing pinned objects �� pinned objects must never
    // be relocated, and mixing pinned + non-pinned on the same page
    // breaks the linear compaction cursor assumption.
    if (!pinned_compact_skip_.empty()) {
        for (auto& mo : marked) {
            for (auto& pr : pinned_compact_skip_) {
                if (pr.addr == mo.addr) {
                    return 0;  // skip this page entirely
                }
            }
        }
    }

    // Plan: compact objects to start of page payload, preserving order.
    out_plan.entries.reserve(marked.size());
    char* cursor = payload;
    CHAOS_IL2CPP_SIZE total_live = 0;

    for (auto& mo : marked) {
        out_plan.entries.push_back({mo.addr, cursor, mo.size});
        cursor += mo.size;
        total_live += mo.size;
    }

    out_plan.live_bytes = total_live;
    out_plan.saved_bytes = page->payload_size - total_live;
    return out_plan.saved_bytes;
}

void MarkSweepOldGen::RelocatePage(OldGenPage* page, const CompactPlan& plan) {
    if (page == nullptr || page->is_oversized || plan.entries.empty()) return;

    // Build sorted old��new address map from CompactPlan.
    struct AddrPair { uintptr_t old_addr; uintptr_t new_addr; };
    std::vector<AddrPair> addr_map;
    addr_map.reserve(plan.entries.size());
    for (auto& e : plan.entries) {
        addr_map.push_back({
            reinterpret_cast<uintptr_t>(e.old_addr),
            reinterpret_cast<uintptr_t>(e.new_addr)
        });
    }

    char* payload = page->Payload();
    uintptr_t page_start = reinterpret_cast<uintptr_t>(payload);
    uintptr_t page_end = page_start + page->payload_size;

    // Walk all old-gen pages to find pointers into this page's objects.
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto* src = page_list_; src != nullptr; src = src->next) {
        if (!src->in_use.load(std::memory_order_acquire)) continue;
        char* src_payload = src->Payload();
        CHAOS_IL2CPP_SIZE src_slots = src->payload_size / sizeof(void*);

        for (CHAOS_IL2CPP_SIZE s = 0; s < src_slots; s++) {
            auto* slot_ptr = reinterpret_cast<void**>(src_payload + s * sizeof(void*));
            void* val = *slot_ptr;
            if (val == nullptr) continue;
            uintptr_t val_addr = reinterpret_cast<uintptr_t>(val);
            if (val_addr < page_start || val_addr >= page_end) continue;

            auto it = std::lower_bound(addr_map.begin(), addr_map.end(), val_addr,
                [](const AddrPair& p, uintptr_t addr) { return p.old_addr < addr; });
            if (it != addr_map.end() && it->old_addr == val_addr) {
                *slot_ptr = reinterpret_cast<void*>(it->new_addr);
            }
        }
    }
}

void MarkSweepOldGen::CompactPage(OldGenPage* page, const CompactPlan& plan) {
    if (page == nullptr || page->is_oversized || plan.entries.empty()) return;

    char* payload = page->Payload();

    // Perform the memmove from last to first (safe for overlapping regions
    // when compacting toward lower addresses).
    for (auto it = plan.entries.rbegin(); it != plan.entries.rend(); ++it) {
        if (it->new_addr != it->old_addr) {
            std::memmove(it->new_addr, it->old_addr, it->size);
        }
    }

    // Rebuild free lists from remaining space after compact_cursor.
    char* compact_end = reinterpret_cast<char*>(plan.entries.back().new_addr) +
                        plan.entries.back().size;
    // Use the compact cursor as the first live end boundary.
    // Since entries are packed at the start, compute end from last entry end.
    CHAOS_IL2CPP_SIZE remaining_bytes = page->payload_size -
        static_cast<CHAOS_IL2CPP_SIZE>(compact_end - payload);

    // Clear existing free lists.
    for (int i = 0; i < kOldGenNumSizeClasses; i++) {
        page->free_lists[i] = nullptr;
    }

    // Add remaining space as free blocks, split by size class.
    char* free_cursor = compact_end;
    while (remaining_bytes > 0) {
        int sc_idx = SizeClassIndex(remaining_bytes);
        if (sc_idx < 0) sc_idx = kOldGenNumSizeClasses - 1;
        CHAOS_IL2CPP_SIZE sc_size = kOldGenSizeClasses[sc_idx];
        if (sc_size > remaining_bytes) {
            if (remaining_bytes < sizeof(OldGenFreeBlock)) break;
            sc_size = sizeof(OldGenFreeBlock);
            sc_idx = SizeClassIndex(sc_size);
            if (sc_idx < 0) break;
        }
        auto* block = reinterpret_cast<OldGenFreeBlock*>(free_cursor);
        block->next = page->free_lists[sc_idx];
        block->sentinel = GcLayoutRegistry::Instance().GetSentinelTypeInfo(sc_idx);
        page->free_lists[sc_idx] = block;
        free_cursor += sc_size;
        remaining_bytes -= sc_size;
    }

}

// ======================================================================
// Parallel compaction (Phase 4 �� GcWorkerPool)
// ======================================================================

CHAOS_IL2CPP_SIZE MarkSweepOldGen::ParallelCompactPages() {
    // Phase 0: Collect fragmented pages (sequential, under mutex).
    struct PagePlan { OldGenPage* page; CompactPlan plan; };
    std::vector<PagePlan> page_plans;
    CHAOS_IL2CPP_SIZE total_live = 0;
    CHAOS_IL2CPP_SIZE total_payload = 0;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto* p = page_list_;
        while (p != nullptr) {
            if (p->in_use.load(std::memory_order_acquire) && !p->is_oversized) {
                if (PageFragmentation(p) > 0.30f) {
                    page_plans.push_back({p, CompactPlan()});
                    total_payload += p->payload_size;
                }
            }
            p = p->next;
        }
    }
    int total_compact = static_cast<int>(page_plans.size());
    if (total_compact == 0) return 0;
    if (total_compact == 1) {
        // Single page: use sequential path (avoids worker pool overhead).
        auto& pp = page_plans[0];
        CHAOS_IL2CPP_SIZE saved = PlanPageCompaction(pp.page, pp.plan);
        RelocatePage(pp.page, pp.plan);
        CompactPage(pp.page, pp.plan);
        return saved;
    }

    // Cap workers.
    int hw_conc = static_cast<int>(std::thread::hardware_concurrency());
    int max_workers = std::min(hw_conc, total_compact);
    if (max_workers > GcWorkerPool::kMaxWorkers) max_workers = GcWorkerPool::kMaxWorkers;
    if (max_workers < 2) max_workers = 2;

    // Phase 1: Plan all pages in parallel (page-local reads only).
    {
        std::atomic<int> plan_idx{0};
        GcWorkerPool::Instance().RunWorkers(max_workers, [&](int) {
            while (true) {
                int idx = plan_idx.fetch_add(1, std::memory_order_relaxed);
                if (idx >= total_compact) break;
                PlanPageCompaction(page_plans[static_cast<size_t>(idx)].page,
                                    page_plans[static_cast<size_t>(idx)].plan);
            }
        });
    }

    // Phase 2: Compact all pages in parallel (page-local memmove + free list rebuild).
    {
        std::atomic<int> compact_idx{0};
        GcWorkerPool::Instance().RunWorkers(max_workers, [&](int) {
            while (true) {
                int idx = compact_idx.fetch_add(1, std::memory_order_relaxed);
                if (idx >= total_compact) break;
                CompactPage(page_plans[static_cast<size_t>(idx)].page,
                            page_plans[static_cast<size_t>(idx)].plan);
            }
        });
    }

    // Phase 3: Build global old��new address map from all plans.
    struct AddrPair { uintptr_t old_addr; uintptr_t new_addr; };
    std::vector<AddrPair> addr_map;
    {
        CHAOS_IL2CPP_SIZE total_entries = 0;
        for (auto& pp : page_plans) total_entries += pp.plan.entries.size();
        if (total_entries == 0) return 0;
        addr_map.reserve(total_entries);
        for (auto& pp : page_plans) {
            total_live += pp.plan.live_bytes;
            for (auto& e : pp.plan.entries) {
                addr_map.push_back({
                    reinterpret_cast<uintptr_t>(e.old_addr),
                    reinterpret_cast<uintptr_t>(e.new_addr)
                });
            }
        }
    }
    // Sort for binary search used in relocation.
    std::sort(addr_map.begin(), addr_map.end(),
        [](const AddrPair& a, const AddrPair& b) { return a.old_addr < b.old_addr; });

    // Phase 4: Build page array once, relocate all pages' slots in parallel.
    std::vector<OldGenPage*> all_pages;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        all_pages.reserve(static_cast<size_t>(page_count_));
        for (auto* p = page_list_; p != nullptr; p = p->next) {
            all_pages.push_back(p);
        }
    }
    int total_pages = static_cast<int>(all_pages.size());

    {
        std::atomic<int> page_idx{0};
        GcWorkerPool::Instance().RunWorkers(max_workers, [&](int) {
            while (true) {
                int idx = page_idx.fetch_add(1, std::memory_order_relaxed);
                if (idx >= total_pages) break;
                auto* src = all_pages[static_cast<size_t>(idx)];
                if (!src->in_use.load(std::memory_order_acquire)) continue;

                char* src_payload = src->Payload();
                CHAOS_IL2CPP_SIZE src_slots = src->payload_size / sizeof(void*);

                for (CHAOS_IL2CPP_SIZE s = 0; s < src_slots; s++) {
                    auto* slot_ptr = reinterpret_cast<void**>(src_payload + s * sizeof(void*));
                    void* val = *slot_ptr;
                    if (val == nullptr) continue;
                    uintptr_t val_addr = reinterpret_cast<uintptr_t>(val);

                    auto it = std::lower_bound(addr_map.begin(), addr_map.end(), val_addr,
                        [](const AddrPair& p, uintptr_t addr) { return p.old_addr < addr; });
                    if (it != addr_map.end() && it->old_addr == val_addr) {
                        *slot_ptr = reinterpret_cast<void*>(it->new_addr);
                    }
                }
            }
        });
    }

    CHAOS_IL2CPP_LOG_INFO_M("OldGen", "parallel_compact_done pages={0} objects={1} saved={2}",
        total_compact,
        static_cast<unsigned long long>(addr_map.size()),
        static_cast<unsigned long long>(total_payload > total_live ? total_payload - total_live : 0));

    return total_payload > total_live ? total_payload - total_live : 0;
}
// ======================================================================

void MarkSweepOldGen::PlanPageEvacuation(OldGenPage* page, CompactPlan& out_plan) {
    if (page == nullptr || page->is_oversized) return;

    auto bm5 = GcMarkBitmap(page->MarkBitmap(), page->bitmap_bytes);
    auto* bitmap = bm5.Words();
    char* payload = page->Payload();
    CHAOS_IL2CPP_SIZE num_words = bm5.WordCount();
    CHAOS_IL2CPP_SIZE max_bm_slots = page->payload_size / sizeof(void*);
    if (num_words * 64 > max_bm_slots) num_words = (max_bm_slots + 63) / 64;
    CHAOS_IL2CPP_SIZE num_slots = page->payload_size / sizeof(void*);

    CHAOS_IL2CPP_SIZE slot = 0;
    while (slot < num_slots) {
        CHAOS_IL2CPP_SIZE w = slot / 64;
        if (w >= num_words) break;
        uint64_t word = bitmap[w];
        if (word == 0) {
            slot = (w + 1) * 64;
            continue;
        }
        uint64_t remaining = word;
        while (remaining != 0) {
            int bit = GcCtz64(remaining);
            remaining &= remaining - 1;
            CHAOS_IL2CPP_SIZE obj_slot = w * 64 + static_cast<CHAOS_IL2CPP_SIZE>(bit);
            if (obj_slot >= num_slots) break;
            void* obj = payload + obj_slot * sizeof(void*);

            // Determine object size from type info.
            // Skip slots that are NOT the start of an object (interior
            // slots like left/right pointers or pattern bytes).  Each
            // object is marked via MarkRange(instance_size/sizeof(void*)),
            // which sets ALL slots of the object.  Only the first slot
            // has a valid TypeInfo pointer; interior slots have data fields.
            // Processing every set bit as a separate object creates
            // overlapping compact-plan entries that race in the parallel
            // Phase 4 memcpy/memset (one worker zeros source data that
            // another is about to read).
            CHAOS_IL2CPP_SIZE obj_size = sizeof(void*);
            const void* type_info_ptr = *static_cast<const void* const*>(obj);
            if (type_info_ptr == nullptr) continue;
            auto& registry = GcLayoutRegistry::Instance();
            if (!registry.IsValidTypeInfoPointer(type_info_ptr)) continue;
            uint64_t stable_id = registry.ReadStableId(type_info_ptr);
            const auto* layout = registry.Lookup(stable_id);
            if (layout != nullptr && layout->instance_size > 0) {
                obj_size = layout->instance_size;
            }

            // Advance past interior slots of this object so the inner
            // bitmap loop doesn't create duplicate evacuation entries.
            if (obj_size > sizeof(void*)) {
                CHAOS_IL2CPP_SIZE obj_slots = obj_size / sizeof(void*);
                for (CHAOS_IL2CPP_SIZE s = 1; s < obj_slots; s++) {
                    CHAOS_IL2CPP_SIZE skip_slot = obj_slot + s;
                    if (skip_slot >= num_slots) break;
                    CHAOS_IL2CPP_SIZE skip_bit = skip_slot % 64;
                    CHAOS_IL2CPP_SIZE skip_word = skip_slot / 64;
                    if (skip_word != w) break; // crosses word boundary
                    remaining &= ~(static_cast<uint64_t>(1) << skip_bit);
                }
            }

            // Skip pinned objects �� must not be relocated.
            if (!pinned_compact_skip_.empty()) {
                bool is_pinned = false;
                for (auto& pr : pinned_compact_skip_) {
                    if (pr.addr == obj) { is_pinned = true; break; }
                }
                if (is_pinned) continue;
            }

            // Allocate target space in old-gen (under STW, no concurrent frees).
            // This is safe during Phase 4b because sweep already ran and
            // free lists are populated.
            void* new_addr = Allocate(obj_size, true);
            if (new_addr != nullptr) {
                out_plan.entries.push_back({obj, new_addr, obj_size});
            }
        }
        slot = (w + 1) * 64;
    }
}

void MarkSweepOldGen::GlobalRelocate(
    const std::vector<CompactPlanEntry>& entries,
    OldGenPage* page_list) {
    if (entries.empty()) return;

    // Build sorted old→new address map.  The entries from PlanPageEvacuation
    // are NOT guaranteed to be in sorted order, and std::lower_bound below
    // requires a sorted range — sort here explicitly.
    struct AddrPair { uintptr_t old_addr; uintptr_t new_addr; };
    std::vector<AddrPair> addr_map;
    addr_map.reserve(entries.size());
    for (auto& e : entries) {
        addr_map.push_back({
            reinterpret_cast<uintptr_t>(e.old_addr),
            reinterpret_cast<uintptr_t>(e.new_addr)
        });
    }
    std::sort(addr_map.begin(), addr_map.end(),
        [](const AddrPair& a, const AddrPair& b) { return a.old_addr < b.old_addr; });

    // Walk all old-gen pages once and update pointers.
    for (auto* src = page_list; src != nullptr; src = src->next) {
        if (!src->in_use.load(std::memory_order_acquire)) continue;
        char* src_payload = src->Payload();
        CHAOS_IL2CPP_SIZE src_slots = src->payload_size / sizeof(void*);

        for (CHAOS_IL2CPP_SIZE s = 0; s < src_slots; s++) {
            auto* slot_ptr = reinterpret_cast<void**>(src_payload + s * sizeof(void*));
            void* val = *slot_ptr;
            if (val == nullptr) continue;
            uintptr_t val_addr = reinterpret_cast<uintptr_t>(val);

            auto it = std::lower_bound(addr_map.begin(), addr_map.end(), val_addr,
                [](const AddrPair& p, uintptr_t addr) { return p.old_addr < addr; });
            if (it != addr_map.end() && it->old_addr == val_addr) {
                *slot_ptr = reinterpret_cast<void*>(it->new_addr);
            }
        }
    }
}

void MarkSweepOldGen::RelocateRoots(const std::vector<CompactPlanEntry>& entries) {
    if (entries.empty()) return;

    // Build old→new address map sorted by old_addr for binary search.
    struct AddrPair { uintptr_t old_addr; uintptr_t new_addr; };
    std::vector<AddrPair> addr_map;
    addr_map.reserve(entries.size());
    for (auto& e : entries) {
        addr_map.push_back({
            reinterpret_cast<uintptr_t>(e.old_addr),
            reinterpret_cast<uintptr_t>(e.new_addr)
        });
    }
    std::sort(addr_map.begin(), addr_map.end(),
        [](const AddrPair& a, const AddrPair& b) { return a.old_addr < b.old_addr; });

    // Scan all thread stacks and update pointers that reference moved objects.
    threading::GcScanAllThreadRoots(
        [](void* root_addr, bool /*is_interior*/, void* user_data) {
            if (root_addr == nullptr) return;
            auto& map = *static_cast<std::vector<AddrPair>*>(user_data);
            uintptr_t val = *static_cast<uintptr_t*>(root_addr);
            if (val == 0) return;

            auto it = std::lower_bound(map.begin(), map.end(), val,
                [](const AddrPair& p, uintptr_t addr) { return p.old_addr < addr; });
            if (it != map.end() && it->old_addr == val) {
                *static_cast<void**>(root_addr) = reinterpret_cast<void*>(it->new_addr);
            }
        },
        &addr_map);

    // GcScanAllThreadRoots above skips the current (calling) thread's own
    // stack.  If the calling thread holds managed object references in stack
    // locals (e.g., the thread that triggered a full GC), those references
    // would NOT be relocated, leaving dangling pointers to evacuated pages
    // that BgcSweep subsequently decommissions.
    //
    // Scan the current thread's own stack conservatively using the Windows
    // TEB to cover this case.
    auto* teb = reinterpret_cast<NT_TIB*>(__readgsqword(0x30));
    uintptr_t self_stack_limit = reinterpret_cast<uintptr_t>(teb->StackLimit);
    uintptr_t self_stack_base  = reinterpret_cast<uintptr_t>(teb->StackBase);
    uintptr_t self_aligned_start = (self_stack_limit + sizeof(void*) - 1)
        & ~static_cast<uintptr_t>(sizeof(void*) - 1);
    uintptr_t self_aligned_end   = self_stack_base
        & ~static_cast<uintptr_t>(sizeof(void*) - 1);

    for (uintptr_t slot = self_aligned_start; slot < self_aligned_end; slot += sizeof(void*)) {
        auto* val_ptr = reinterpret_cast<void**>(slot);
        if (*val_ptr == nullptr) continue;
        uintptr_t val = reinterpret_cast<uintptr_t>(*val_ptr);
        if (val < g_heap_base) continue;

        auto it = std::lower_bound(addr_map.begin(), addr_map.end(), val,
            [](const AddrPair& p, uintptr_t addr) { return p.old_addr < addr; });
        if (it != addr_map.end() && it->old_addr == val) {
            *val_ptr = reinterpret_cast<void*>(it->new_addr);
        }
    }
}

void MarkSweepOldGen::CrossPageCompact() {
    // No outer mutex_ here: CrossPageCompact runs under STW (all mutators and
    // BGC are suspended), so no concurrent page_list_ or free-list access.
    // PlanPageEvacuation calls Allocate → TryAllocateFromFreeLists which takes
    // mutex_ internally — holding it here would deadlock.

    // Phase 1: Select source pages (most fragmented).
    struct PageScore { OldGenPage* page; float frag; };
    std::vector<PageScore> candidates;
    candidates.reserve(static_cast<size_t>(page_count_));

    for (auto* p = page_list_; p != nullptr; p = p->next) {
        if (!p->in_use.load(std::memory_order_acquire) || p->is_oversized) continue;
        float frag = PageFragmentation(p);
        if (frag > kCrossPageFragThreshold) {
            candidates.push_back({p, frag});
        }
    }
    if (candidates.empty()) {
        return;
    }
    std::sort(candidates.begin(), candidates.end(),
        [](const PageScore& a, const PageScore& b) { return a.frag > b.frag; });

    // Dynamic evacuation budget: at least 512KB, at most 10% of total
    // old-gen allocation or 4MB max.  Scales with heap size so large
    // heaps compact more per cycle without excessive STW pause.
    CHAOS_IL2CPP_SIZE total_heap = TotalAllocated();
    CHAOS_IL2CPP_SIZE evacuation_budget = std::max<CHAOS_IL2CPP_SIZE>(
        512 * 1024,
        std::min<CHAOS_IL2CPP_SIZE>(
            static_cast<CHAOS_IL2CPP_SIZE>(total_heap * 0.1f),
            4 * 1024 * 1024));

    // Select source pages up to budget.
    std::vector<OldGenPage*> source_pages;
    CHAOS_IL2CPP_SIZE total_evacuate = 0;
    int src_idx = 0;
    for (auto& c : candidates) {
        // Skip pages containing pinned objects.
        if (!pinned_compact_skip_.empty()) {
            char* payload = c.page->Payload();
            char* payload_end = payload + c.page->payload_size;
            bool has_pinned = false;
            for (auto& pr : pinned_compact_skip_) {
                auto* addr = static_cast<char*>(pr.addr);
                if (addr >= payload && addr < payload_end) {
                    has_pinned = true;
                    break;
                }
            }
            if (has_pinned) continue;
        }
        source_pages.push_back(c.page);
        auto bm6 = GcMarkBitmap(c.page->MarkBitmap(), c.page->bitmap_bytes);
        auto* bitmap = bm6.Words();
        CHAOS_IL2CPP_SIZE num_words = bm6.WordCount();
        CHAOS_IL2CPP_SIZE max_bm_slots = c.page->payload_size / sizeof(void*);
        if (num_words * 64 > max_bm_slots) num_words = (max_bm_slots + 63) / 64;
        CHAOS_IL2CPP_SIZE marked_slots = 0;
        for (CHAOS_IL2CPP_SIZE w = 0; w < num_words; w++) {
            marked_slots += static_cast<CHAOS_IL2CPP_SIZE>(GcPopCount64(bitmap[w]));
        }
        total_evacuate += marked_slots * sizeof(void*);
        if (total_evacuate >= evacuation_budget) break;
    }

    // Phase 2: Build evacuation plan.
    CompactPlan global_plan;
    for (auto* src : source_pages) {
        PlanPageEvacuation(src, global_plan);
    }
    if (global_plan.entries.empty()) return;

    CHAOS_IL2CPP_LOG_INFO_M("OldGen",
        "cross_page_compact pages={0} objects={1}",
        static_cast<unsigned>(source_pages.size()),
        static_cast<unsigned long long>(global_plan.entries.size()));

    // Phase 3: Global relocation.
    GlobalRelocate(global_plan.entries, page_list_);

    // Phase 4: Copy objects from source pages in parallel.
    {
        int hw = static_cast<int>(std::thread::hardware_concurrency());
        int n_workers = std::min(hw, GcWorkerPool::kMaxWorkers);
        if (n_workers < 2) n_workers = 2;
        std::atomic<int> copy_idx{0};
        int total_copy = static_cast<int>(global_plan.entries.size());
        GcWorkerPool::Instance().RunWorkers(n_workers, [&](int) {
            while (true) {
                int idx = copy_idx.fetch_add(1, std::memory_order_relaxed);
                if (idx >= total_copy) break;
                auto& e = global_plan.entries[static_cast<size_t>(idx)];
                if (e.new_addr != e.old_addr) {
                    std::memcpy(e.new_addr, e.old_addr, e.size);
                    std::memset(e.old_addr, 0, e.size);
                    MarkObject(e.new_addr);
                }
            }
        });
    }

    // Phase 5: Sweep evacuated source pages in parallel.
    {
        int hw = static_cast<int>(std::thread::hardware_concurrency());
        int n_workers = std::min(hw, GcWorkerPool::kMaxWorkers);
        if (n_workers < 2) n_workers = 2;
        std::atomic<int> sweep_idx{0};
        int total_pages = static_cast<int>(source_pages.size());
        GcWorkerPool::Instance().RunWorkers(n_workers, [&](int) {
            while (true) {
                int idx = sweep_idx.fetch_add(1, std::memory_order_relaxed);
                if (idx >= total_pages) break;
                auto* src = source_pages[static_cast<size_t>(idx)];
                GcMarkBitmap(src->MarkBitmap(), src->bitmap_bytes).Clear();
                for (int i = 0; i < kOldGenNumSizeClasses; i++) {
                    src->free_lists[i] = nullptr;
                }
                char* payload = src->Payload();
                CHAOS_IL2CPP_SIZE remaining = src->payload_size;
                char* cursor = payload;
                while (remaining > 0) {
                    int sc_idx = SizeClassIndex(remaining);
                    if (sc_idx < 0) sc_idx = kOldGenNumSizeClasses - 1;
                    CHAOS_IL2CPP_SIZE sc_size = kOldGenSizeClasses[sc_idx];
                    if (sc_size > remaining) {
                        if (remaining < sizeof(OldGenFreeBlock)) break;
                        sc_size = sizeof(OldGenFreeBlock);
                        sc_idx = SizeClassIndex(sc_size);
                        if (sc_idx < 0) break;
                    }
                    auto* block = reinterpret_cast<OldGenFreeBlock*>(cursor);
                    block->next = src->free_lists[sc_idx];
                    block->sentinel = GcLayoutRegistry::Instance().GetSentinelTypeInfo(sc_idx);
                    src->free_lists[sc_idx] = block;
                    cursor += sc_size;
                    remaining -= sc_size;
                }
            }
        });
    }

    // Phase 6: Relocate thread stack roots after compaction.
    // Without this step, thread stack variables that point to evacuated
    // objects contain stale (old) addresses, and the next mutator access
    // will dereference freed (0xFF-filled) memory.
    RelocateRoots(global_plan.entries);

    CHAOS_IL2CPP_LOG_INFO_M("OldGen",
        "cross_page_compact_done bytes={0}",
        static_cast<unsigned long long>(total_evacuate));
}

void MarkSweepOldGen::Collect(void (*root_callback)(void* obj, void* user_data), void* user_data) {
    CHAOS_IL2CPP_PROFILE_SCOPE("OldGen::Collect");

    auto pause_start = std::chrono::steady_clock::now();

    CHAOS_IL2CPP_LOG_INFO_M("OldGen", "collect_start page_count={0}", page_count_);

    // V4-H3: Snapshot pinned_roots_ under mutex to avoid data race with
    // AddPinnedRoot (which pushes under the same mutex).  Iterating the
    // vector without locking is UB if a concurrent push_back triggers
    // reallocation �� the iterator becomes dangling.
    std::vector<PinnedRoot> pinned_snapshot;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pinned_snapshot = pinned_roots_;
    }

#if defined(CHAOS_IL2CPP_DEBUG)
    // Sanity: after snapshot, no other thread holds mutex_ for writes.
    // The snapshot is our consistent view for the entire collection.
#endif

    // Phase 1: Mark roots.
    bool has_roots = (root_callback != nullptr) || !pinned_snapshot.empty();

    if (root_callback != nullptr) {
        root_callback(nullptr, user_data);  // signal start (optional)
    }

    // Clear all page bitmaps for this cycle's mark phase.
    // The bitmaps were preserved across sweep so that finalizer, weak-handle,
    // and dependent-handle processing could query IsMarked() after sweep.
    // Now we reset them for a fresh mark pass.
    {
        std::lock_guard<std::mutex> lock(mutex_);
        int seen = 0;
        for (auto* p = page_list_; p != nullptr; p = p->next) {
            seen++;
            if (p->in_use.load(std::memory_order_acquire)) {
                GcMarkBitmap(p->MarkBitmap(), p->bitmap_bytes).Clear();
            }
        }
    }
    marked_count_.store(0, std::memory_order_relaxed);

    // Mark pinned roots from snapshot (no lock needed).
    for (auto& pr : pinned_snapshot) {
        if (MarkObject(pr.addr)) {
            mark_stack_.push_back(pr.addr);
            has_roots = true;
        }
    }


    // Scan ALL registered threads' TLS nurseries as root sets.
    // This ensures full GC sees nursery roots from every worker thread,
    // not just the calling thread.  Previously, only the calling thread's
    // TLS nursery was scanned, which could miss live references from
    // worker threads to old-gen objects, causing premature collection.
    {
        size_t before_roots = mark_stack_.size();

                // EnumerateThreads lambda �� scan shared young generation TLAB pointers.
        threading::EnumerateThreads(
            [](threading::ManagedThread* thread) -> bool {
                // Scan the shared young generation region directly.
                // All threads share one young region; scan [begin, current).
                Region* young_region = g_young_gen.region.load(std::memory_order_acquire);
                if (young_region == nullptr) return true;
                if (young_region->current > young_region->begin) {
                    g_old_gen.ScanRangeForRoots(
                        young_region->begin, young_region->current);
                }
                return true;
            });
        if (mark_stack_.size() > before_roots) {
            has_roots = true;
        }
    }

    // Scan ALL thread stacks as conservative roots.  This catches old-gen
    // object references that live only in thread-local variables (stack slots)
    // and are NOT in any TLS nursery �� without this, those objects would be
    // reclaimed by the sweep, causing use-after-free.
    {
        size_t before_roots = mark_stack_.size();
        threading::GcScanAllThreadRoots(
            [](void* root_addr, bool /*is_interior*/, void* user_data) {
                auto* self = static_cast<MarkSweepOldGen*>(user_data);
                self->TryMarkRoot(root_addr);
            },
            this);
        if (mark_stack_.size() > before_roots) {
            has_roots = true;
        }
    }

    // Scan POH regions as conservative roots.
    // POH objects bypass young GC (never copied), so they must be
    // preserved during full GC mark.
    {
        auto& rm = RegionManager::Instance();
        int poh_count = rm.GetPohRegionCount();
        if (poh_count > 0) {
            size_t before_roots = mark_stack_.size();
            for (Region* r = rm.GetFirstPohRegion(); r != nullptr;
                 r = rm.GetNextPohRegion(r)) {
                if (r->current > r->begin) {
                    ScanRangeForRoots(r->begin, r->current);
                }
            }
            if (mark_stack_.size() > before_roots) {
                has_roots = true;
            }
        }
    }

    // Phase 2: Mark transitive closure.

    // Mark LOH roots (in pinned roots and thread stacks) when LOH is active.
    // The strategy is simple: after old-gen root marking, any LOH-reachable
    // objects found in roots get their LOH mark bit set.  LOH objects are not
    // transitively scanned (LOH objects are large and rarely point to other
    // LOH objects �� memcpy of 85 KB+ is not worth the fragmentation savings).
    if (g_loh.SegmentCount() > 0) {
        // Mark LOH objects found in pinned roots.
        for (auto& pr : pinned_snapshot) {
            if (g_loh.IsInLOH(pr.addr)) {
                g_loh.MarkObject(pr.addr);
            }
        }

        // The thread stack scanning in Phase 1 (GcScanAllThreadRoots) already
        // registered all root pointers.  Those that happen to be LOH objects
        // are handled by the pinned root scan above and will not be freed by
        // LOH Sweep() since we mark reachable segments here.
    }

    // When there are enough pages, use parallel mark with chunked work-stealing.
    // For small heaps (1 page), sequential is faster (no dispatch overhead).
    constexpr int kMinPagesForParallelMark = 2;

    if (page_count_ >= kMinPagesForParallelMark) {
        std::vector<OldGenPage*> pages;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            pages.reserve(static_cast<size_t>(page_count_));
            auto* p = page_list_;
            int page_idx = 0;
            while (p != nullptr) {
                pages.push_back(p);
                p = p->next;
                page_idx++;
            }
        }
        DrainMarkStackParallel(pages.data(), static_cast<int>(pages.size()));
    } else {
        // Sequential mark for small heaps.
        DrainMarkStack();
    }

    // Fire MARK_DONE event.
        GcFireEvent(GcEvent::MARK_DONE);

    // Phase 3: Run finalizers for unreachable objects.
    // IMPORTANT: This must happen BEFORE Sweep (Phase 4).  The finalizer
    // callback receives the object pointer; if we sweep first, the object's
    // memory is reclaimed (back on free list or VirtualFree'd), and the
    // finalizer would read freed memory (use-after-free).
    CHAOS_IL2CPP_SIZE finalizers_run = RunFinalizers();


    // Phase 3b: Re-mark pass for finalizer-reachable objects.
    // Finalizers can resurrect objects (make them reachable again by storing
    // 'this' into a static field, registering a new GCHandle, etc.).
    // Without a re-mark pass, these resurrected objects would be swept as
    // garbage (use-after-free).  We re-scan GCHandle roots and drain the
    // mark stack to pick up any newly-reachable objects.  This matches
    // CoreCLR behavior where finalization is treated as a GC root for the
    // subsequent mark pass.
        HandleReMarkPass();

    // Phase 3c: Process weak handles (GCHandle.Weak / WeakReference).
    // Must happen AFTER finalization (so WeakTrackResurrection handles are
    // preserved if the object was resurrected) and BEFORE sweep (the mark
    // bitmap must still be valid to determine which objects are dead).
    //
    // GcCollectDeadWeakHandles scans the handle table using the old-gen
    // mark bitmap to find dead objects.  GcProcessCollectedWeakHandles
    // nulls the corresponding handles (except WeakTrackResurrection, which
    // survives one cycle).
    {
        std::vector<std::pair<uint64_t, void*>> dead_weak_handles;
        GcCollectDeadWeakHandles(dead_weak_handles);
        if (!dead_weak_handles.empty()) {
            GcProcessCollectedWeakHandles(dead_weak_handles);
        }
    }

    // Phase 4: Sweep all pages (parallel when beneficial).
    CHAOS_IL2CPP_SIZE total_reclaimed = 0;
    int total_pages = page_count_;

    if (total_pages >= 2) {
        // Build page pointer array under lock.
        std::vector<OldGenPage*> pages;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            pages.reserve(static_cast<size_t>(total_pages));
            auto* p = page_list_;
            while (p != nullptr) {
                pages.push_back(p);
                p = p->next;
            }
        }

        // Parallel sweep: dispatch pages via atomic index.
        // Main thread participates alongside GcWorkerPool threads.
        // Cap workers to min(hardware_concurrency, total_pages) to avoid
        // oversubscribing high-core-count machines with few pages to sweep.
        std::atomic<int> next_page{0};
        std::atomic<CHAOS_IL2CPP_SIZE> parallel_reclaimed{0};
        int max_workers = std::min(static_cast<int>(std::thread::hardware_concurrency()), total_pages);
        if (max_workers < 2) max_workers = 2;

        GcWorkerPool::Instance().RunWorkers(max_workers, [&](int idx) {
            (void)idx;
            CHAOS_IL2CPP_SIZE local_reclaimed = 0;
            while (true) {
                int idx2 = next_page.fetch_add(1, std::memory_order_relaxed);
                if (idx2 >= total_pages) break;
                local_reclaimed += SweepPage(pages[static_cast<size_t>(idx2)]);
                CoalescePage(pages[static_cast<size_t>(idx2)]);
            }
            parallel_reclaimed.fetch_add(local_reclaimed, std::memory_order_relaxed);
        });

        total_reclaimed = parallel_reclaimed.load(std::memory_order_relaxed);
    } else {
        // Sequential sweep (single page).
        auto* page = page_list_;
        while (page != nullptr) {
            CHAOS_IL2CPP_SIZE reclaimed = SweepPage(page);
            CoalescePage(page);
            total_reclaimed += reclaimed;
            page = page->next;
        }
    }

    // Phase 4: Free decommissioned pages (oversized pages that were fully garbage).
    {
        std::lock_guard<std::mutex> lock(mutex_);


        OldGenPage** pp = &page_list_;
        while (*pp != nullptr) {
            OldGenPage* p = *pp;
            bool in_use = p->in_use.load(std::memory_order_acquire);
            bool should_pool = (in_use && !p->is_oversized
                                && PageFragmentation(p) >= 1.0f);
            if (should_pool || !in_use) {
                *pp = p->next;
                // Invalidate last_alloc_page_ entries that point to this page.
                for (int i = 0; i < kOldGenNumSizeClasses; i++) {
                    if (last_alloc_page_[i] == p) {
                        last_alloc_page_[i] = nullptr;
                    }
                }
                if (should_pool) {
                    page_pool_.push_back(p);
                } else {
                    FreePage(p);
                }
                page_count_--;
            } else {
                pp = &p->next;
            }
        }

        // Free deferred pages (oversized pages freed by external Free()
        // but deferred to safepoint to avoid racing with BgcSweep).
        for (auto* p : deferred_free_pages_) {
            VirtualFreePage(p, p->page_size);
        }
        deferred_free_pages_.clear();
    }
    RebuildPageArray();

    // Fire SWEEP_DONE event.
    GcFireEvent(GcEvent::SWEEP_DONE);

    // Sweep the Large Object Heap.
    CHAOS_IL2CPP_SIZE loh_reclaimed = 0;
    if (g_loh.SegmentCount() > 0) {
        loh_reclaimed = g_loh.Sweep();
        if (loh_reclaimed > 0) {
        }
    }
    total_reclaimed += loh_reclaimed;

    // LOH compaction (opt-in, controlled by CompactMode).
    // Relocates live LOH segments to reduce fragmentation.
    {
        std::vector<std::pair<void*, void*>> loh_relocs;
        if (g_loh.Compact(loh_relocs) > 0) {
            CHAOS_IL2CPP_LOG_INFO_M("OldGen", "loh_compact relocations={0}",
                static_cast<unsigned long>(loh_relocs.size()));

            // Build sorted old��new address map for reference fix-up.
            struct LohAddrPair { uintptr_t old_addr; uintptr_t new_addr; };
            std::vector<LohAddrPair> addr_map;
            addr_map.reserve(loh_relocs.size());
            for (auto& r : loh_relocs) {
                addr_map.push_back({
                    reinterpret_cast<uintptr_t>(r.first),
                    reinterpret_cast<uintptr_t>(r.second)});
            }
            std::sort(addr_map.begin(), addr_map.end(),
                [](const LohAddrPair& a, const LohAddrPair& b) {
                    return a.old_addr < b.old_addr;
                });

            // Fix up old-gen page references.
            for (auto* p = page_list_; p != nullptr; p = p->next) {
                if (!p->in_use.load(std::memory_order_acquire)) continue;
                char* payload = p->Payload();
                CHAOS_IL2CPP_SIZE num_slots = p->payload_size / sizeof(void*);
                for (CHAOS_IL2CPP_SIZE s = 0; s < num_slots; s++) {
                    auto* slot = reinterpret_cast<void**>(payload + s * sizeof(void*));
                    void* val = *slot;
                    if (val == nullptr) continue;
                    uintptr_t val_addr = reinterpret_cast<uintptr_t>(val);
                    auto it = std::lower_bound(addr_map.begin(), addr_map.end(), val_addr,
                        [](const LohAddrPair& p, uintptr_t addr) { return p.old_addr < addr; });
                    if (it != addr_map.end() && it->old_addr == val_addr) {
                        *slot = reinterpret_cast<void*>(it->new_addr);
                    }
                }
            }

            // Fix up GCHandles.
            GcRelocateHandles(loh_relocs);
        }
    }

    // Phase 4b: Compaction (when fragmentation exceeds threshold).
    // Transfer pinned snapshot to compaction skip list so PlanPageCompaction
    // and PlanPageEvacuation can exclude pinned objects from relocation.
    pinned_compact_skip_ = pinned_roots_;
    CompactMode compact_mode = DecideCompactMode();

    if (compact_mode == CompactMode::CROSS_PAGE) {
        CHAOS_IL2CPP_LOG_INFO_M("OldGen", "cross_page_compact_mode_enabled");
        CrossPageCompact();
        GcFireEvent(GcEvent::COMPACT_DONE);
    } else if (compact_mode == CompactMode::COMPACT) {
        CHAOS_IL2CPP_LOG_INFO_M("OldGen", "compact_mode_enabled");
        CHAOS_IL2CPP_SIZE total_saved = ParallelCompactPages();
        CHAOS_IL2CPP_LOG_INFO_M("OldGen", "compact_done saved_bytes={0}",
            static_cast<unsigned long long>(total_saved));
        GcFireEvent(GcEvent::COMPACT_DONE);
    }

    // Diagnostic: page bitmap after sweep
    // (currently disabled �� kept for future debugging use)
    // Clear compaction pin skip list �� no longer needed and the
    // snapshot reference would be dangling next cycle.
    pinned_compact_skip_.clear();

    // Phase 5: Record results.
    auto pause_end = std::chrono::steady_clock::now();
    uint64_t pause_ns = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(pause_end - pause_start).count());

    CHAOS_IL2CPP_SIZE marked_count = static_cast<CHAOS_IL2CPP_SIZE>(
        marked_count_.exchange(0, std::memory_order_relaxed));

    // Record into GcStats.
    CHAOS_IL2CPP_SIZE total_heap_bytes = static_cast<CHAOS_IL2CPP_SIZE>(page_count_) * kOldGenPageSize;
    GcRecordFullCollection(
        static_cast<CHAOS_IL2CPP_SIZE>(page_count_),
        marked_count,
        total_reclaimed,
        finalizers_run,
        pause_ns);

    // Record into scheduler with actual heap size for full GC trigger decisions.
    g_gc_scheduler.RecordFullCollection(total_heap_bytes, pause_ns);

    CHAOS_IL2CPP_LOG_INFO_M("OldGen", "collect_done reclaimed={0} pause_ns={1}",
        static_cast<unsigned long long>(total_reclaimed), pause_ns);

    // Fire GC_FULL_DONE event.
    GcFireEvent(GcEvent::GC_FULL_DONE);
}

bool MarkSweepOldGen::CollectFull() {
    // At C3, incremental collection is a full collect.
    // C3+ will replace this with incremental slicing.
    Collect(nullptr, nullptr);
    return true;
}

// ======================================================================
// Thread stack registration (replaces GC_register_my_thread)
// ======================================================================

// Per-thread stack info for root scanning during GC.
// Using thread_local for O(1) lookup �� replaces the old lock-protected vector
// and _AddressOfReturnAddress heuristic matching.
struct ThreadStackInfo {
    uintptr_t stack_base;
    uintptr_t stack_limit;  // low address (stack grows downward)
};
static thread_local ThreadStackInfo tls_thread_stack{0, 0};

void MarkSweepOldGen::RegisterThreadStack(void* stack_base, void* stack_limit) {
    tls_thread_stack.stack_base = reinterpret_cast<uintptr_t>(stack_base);
    tls_thread_stack.stack_limit = reinterpret_cast<uintptr_t>(stack_limit);
}

void MarkSweepOldGen::UnregisterThreadStack() {
    tls_thread_stack.stack_base = 0;
    tls_thread_stack.stack_limit = 0;
}

void MarkSweepOldGen::AddPinnedRoot(void* addr, CHAOS_IL2CPP_SIZE size) {
    std::lock_guard<std::mutex> lock(mutex_);
    pinned_roots_.push_back({addr, size});
}

// ���� Root scanning ����������������������������������������������������������������������������������

bool MarkSweepOldGen::TryMarkRoot(void* addr) {
    // addr comes from GcScanAllThreadRoots: it is the address of a stack slot.
    // Read the VALUE at that slot �� if it points to old-gen, mark it.
    if (addr == nullptr) return false;
    auto val = *reinterpret_cast<void**>(addr);
    if (val == nullptr) return false;

    // FindPage before IsValidManagedObject: FindPage is safe for arbitrary
    // values (numeric range comparison only, no pointer dereference).
    auto* page = FindPage(val);
    if (page == nullptr || !page->in_use.load(std::memory_order_acquire)) return false;

    // Now safe to read the first word (memory is within a valid heap page).
    if (!IsValidManagedObject(val)) return false;

    if (MarkObject(val)) {
        mark_stack_.push_back(val);
        return true;
    }
    return false;
}

void MarkSweepOldGen::ScanRangeForRoots(void* range_begin, void* range_end) {
    if (range_begin == nullptr || range_end == nullptr) return;
    if (range_begin >= range_end) return;

    uintptr_t begin = reinterpret_cast<uintptr_t>(range_begin);
    uintptr_t end   = reinterpret_cast<uintptr_t>(range_end);

    for (uintptr_t slot = begin; slot + sizeof(void*) <= end; slot += sizeof(void*)) {
        auto val = *reinterpret_cast<void**>(slot);
        if (val == nullptr) continue;

        // FindPage before IsValidManagedObject: FindPage only does numeric
        // range comparison (no pointer dereference) so it is safe for
        // arbitrary stack values including kernel-space addresses like
        // 0xBAD0DEAD... (from test pattern magic words).
        auto* page = FindPage(val);
        if (page == nullptr || !page->in_use.load(std::memory_order_acquire)) continue;

        // Now safe to read the first word (memory is within a valid heap page).
        if (!IsValidManagedObject(val)) continue;

        if (MarkObject(val)) {
            mark_stack_.push_back(val);
        }
    }
    DrainMarkStack();
}

// ======================================================================
// Finalizer support
// ======================================================================

void MarkSweepOldGen::RegisterFinalizer(void* obj, void (*finalizer)(void*)) {
    if (obj == nullptr || finalizer == nullptr) return;

    std::lock_guard<std::mutex> lock(mutex_);
    finalizers_.push_back({obj, finalizer});
    g_gc_stats.finalization_pending_count.fetch_add(1, std::memory_order_relaxed);
}

CHAOS_IL2CPP_SIZE MarkSweepOldGen::RunFinalizers() {
    std::vector<FinalizerEntry> to_run;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        to_run.swap(finalizers_);
        // Reset pending counter; RegisterFinalizer calls below will
        // re-add entries for objects that survived (still reachable).
        g_gc_stats.finalization_pending_count.store(0, std::memory_order_relaxed);
    }

    CHAOS_IL2CPP_SIZE ran = 0;
    for (auto& entry : to_run) {

        // Skip suppressed finalizers.
        if (std::find(suppressed_finalizers_.begin(), suppressed_finalizers_.end(),
                      entry.obj) != suppressed_finalizers_.end()) {
            continue;
        }

        // Check if the object is still reachable (marked in bitmap).
        // RunFinalizers is called after DrainMarkStack, so all reachable
        // objects have their mark-bit set.  Skip the finalizer for any
        // object that is still marked �� it's still alive.
        bool unreachable = true;

        // Check if object is in old-gen page.
        auto* page = FindPage(entry.obj);
        if (page != nullptr && !page->is_oversized) {
            uintptr_t obj_addr = reinterpret_cast<uintptr_t>(entry.obj);
            uintptr_t payload_start = reinterpret_cast<uintptr_t>(page->Payload());
            if (obj_addr >= payload_start) {
                CHAOS_IL2CPP_SIZE offset = obj_addr - payload_start;
                CHAOS_IL2CPP_SIZE slot_idx = offset / sizeof(void*);
                auto bm7 = GcMarkBitmap(page->MarkBitmap(), page->bitmap_bytes);
                if (bm7.TestSlot(slot_idx)) {
                    unreachable = false;
                }
            }
        } else if (g_loh.IsInLOH(entry.obj)) {
            // LOH object: check its segment-level mark bit.
            // LOH segments have a single atomic<bool> marked field; if it's
            // still set, the object is reachable and the finalizer is skipped.
            if (g_loh.IsMarked(entry.obj)) {
                unreachable = false;
            }
        }
        // For oversized pages or if we can't determine reachability,
        // conservatively assume unreachable and run the finalizer.

        if (unreachable) {
            entry.finalizer(entry.obj);
            ran++;
        } else {
            // Object is still reachable �� re-register the finalizer for
            // the next GC cycle.
            RegisterFinalizer(entry.obj, entry.finalizer);
        }
    }

    return ran;
}

std::vector<FinalizerEntry> MarkSweepOldGen::CollectDeadFinalizables() {
    std::vector<FinalizerEntry> dead_entries;
    std::vector<FinalizerEntry> live_entries;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& entry : finalizers_) {
            if (entry.finalizer == nullptr) continue;
            // Skip suppressed finalizers.
            if (std::find(suppressed_finalizers_.begin(), suppressed_finalizers_.end(),
                          entry.obj) != suppressed_finalizers_.end()) {
                live_entries.push_back(entry);  // keep in finalizers_ but don't run
                continue;
            }
            bool unreachable = true;
            auto* page = FindPage(entry.obj);
            if (page != nullptr && !page->is_oversized) {
                uintptr_t obj_addr = reinterpret_cast<uintptr_t>(entry.obj);
                uintptr_t payload_start = reinterpret_cast<uintptr_t>(page->Payload());
                if (obj_addr >= payload_start) {
                    CHAOS_IL2CPP_SIZE offset = obj_addr - payload_start;
                    CHAOS_IL2CPP_SIZE slot_idx = offset / sizeof(void*);
                    auto bm8 = GcMarkBitmap(page->MarkBitmap(), page->bitmap_bytes);
                    if (bm8.TestSlot(slot_idx)) {
                        unreachable = false;
                    }
                }
            } else if (g_loh.IsInLOH(entry.obj)) {
                if (g_loh.IsMarked(entry.obj)) {
                    unreachable = false;
                }
            }
            if (unreachable) dead_entries.push_back(entry);
            else live_entries.push_back(entry);
        }
        finalizers_.swap(live_entries);
        g_gc_stats.finalization_pending_count.store(finalizers_.size(), std::memory_order_relaxed);
    }
    return dead_entries;
}

// ���� Finalizer suppression support ��������������������������������������������������

void MarkSweepOldGen::SuppressFinalizer(void* obj) {
    if (obj == nullptr) return;
    std::lock_guard<std::mutex> lock(mutex_);
    // Only add if not already suppressed (linear scan �� suppressed set is tiny).
    if (std::find(suppressed_finalizers_.begin(), suppressed_finalizers_.end(),
                  obj) == suppressed_finalizers_.end()) {
        suppressed_finalizers_.push_back(obj);
    }
}

void MarkSweepOldGen::ReRegisterFinalizer(void* obj) {
    if (obj == nullptr) return;
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = std::find(suppressed_finalizers_.begin(), suppressed_finalizers_.end(),
                        obj);
    if (it != suppressed_finalizers_.end()) {
        suppressed_finalizers_.erase(it);
    }
}

// ���� BGC concurrent-safe mark ��������������������������������������������������������������������������

bool MarkSweepOldGen::BgcTryMark(void* obj) {
    // Delegate to MarkObject which already uses atomic bitmap operations.
    // The concurrent BGC thread and STW parallel mark can safely interleave
    // on the same bitmap because test-and-set is atomic.
    return MarkObject(obj);
}

// ���� BGC concurrent sweep ����������������������������������������������������������������������������������

void MarkSweepOldGen::BgcSweep() {
    // Snapshot page list under mutex.
    std::vector<OldGenPage*> pages;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pages.reserve(static_cast<size_t>(page_count_));
        auto* p = page_list_;
        while (p != nullptr) {
            pages.push_back(p);
            p = p->next;
        }
    }


    // Parallel sweep via GcWorkerPool.
    // Restored after BGC hang investigation — see git log for details.
    // Each page has independent free_lists and bitmap; SweepPage + CoalescePage
    // are thread-safe on disjoint pages. clear_bitmap=false preserves mark
    // bitmap for DecideCompactMode which runs later in BgcCompact().
    {
        int total_pages = static_cast<int>(pages.size());
        for (auto* page : pages) {
            if (!page->in_use.load(std::memory_order_acquire)) continue;
            SweepPage(page, false);
            CoalescePage(page);
        }
    }

    // Phase 4b: Free decommissioned oversized pages (marked !in_use by SweepPage).
    // IMPORTANT: Do NOT VirtualFree here — BgcSweep runs concurrently with
    // mutators that may still hold stale page_array_ references to these pages.
    // Defer VirtualFree to BgcCompact (under STW safepoint) where no concurrent
    // readers exist.
    {
        std::lock_guard<std::mutex> lock(mutex_);


        OldGenPage** pp = &page_list_;
        while (*pp != nullptr) {
            OldGenPage* p = *pp;
            bool in_use = p->in_use.load(std::memory_order_acquire);
            bool should_pool = (in_use && !p->is_oversized
                                && PageFragmentation(p) >= 1.0f);
            if (should_pool || !in_use) {
                *pp = p->next;
                for (int i = 0; i < kOldGenNumSizeClasses; i++) {
                    if (last_alloc_page_[i] == p) {
                        last_alloc_page_[i] = nullptr;
                    }
                }
                if (should_pool) {
                    page_pool_.push_back(p);
                } else {
                    // Defer VirtualFree to BgcCompact (STW safepoint).
                    deferred_free_pages_.push_back(p);
                }
                page_count_--;
            } else {
                pp = &p->next;
            }
        }

        // Deferred pages are freed in BgcCompact (under STW safepoint).
        // Not here — mutators may still reference freed pages via stale
        // page_array_ entries.

        // Rebuild under mutex_ to prevent TOCTOU: mutators (AllocatePage/Free)
        // can modify page_list_ between count and fill loops inside
        // RebuildPageArray, causing heap buffer overflow on the new_pages array.
        RebuildPageArray();
    }

    // Reclaim retired GcLayout tables (safe during BGC sweep since the
    // BGC thread is the only concurrent GC activity; mutators are running
    // but their cache-line-stale GcTypeLayout pointers are still valid).
    GcLayoutRegistry::Instance().ReclaimRetiredTables();

    // Phase 5: Sweep the Large Object Heap.
    // LOH segments that were not marked during concurrent mark are freed.
    g_loh.Sweep();

}

// ���� BGC concurrent compaction ������������������������������������������������������������������������

void MarkSweepOldGen::BgcCompact() {

    // Transfer pinned roots to compaction skip list so PlanPageEvacuation
    // excludes them from cross-page relocation (same as Collect() does).
    pinned_compact_skip_ = pinned_roots_;

    // Phase 1: Decide compaction mode (reads mark bitmap preserved by BgcSweep).
    CompactMode compact_mode = DecideCompactMode();

    if (compact_mode == CompactMode::CROSS_PAGE) {
        CHAOS_IL2CPP_LOG_INFO("BGC", "cross_page_compact_mode_enabled");
        CrossPageCompact();
        GcFireEvent(GcEvent::COMPACT_DONE);
    } else if (compact_mode == CompactMode::COMPACT) {
        CHAOS_IL2CPP_LOG_INFO("BGC", "compact_mode_enabled");
        CHAOS_IL2CPP_SIZE total_saved = ParallelCompactPages();
        CHAOS_IL2CPP_LOG_INFO_M("BGC", "compact_done saved_bytes={0}",
            static_cast<unsigned long long>(total_saved));
        GcFireEvent(GcEvent::COMPACT_DONE);
    }

    // Phase 2: Clear all page mark bitmaps (preserved by BgcSweep's
    // SweepPage(..., false) call).  Must be cleared before the next BGC
    // cycle's BgcTryMark, which uses atomic test-and-set and would
    // incorrectly report stale bits as "already marked".
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto* p = page_list_;
        while (p != nullptr) {
            if (p->in_use.load(std::memory_order_acquire)) {
                GcMarkBitmap(p->MarkBitmap(), p->bitmap_bytes).Clear();
            }
            p = p->next;
        }
    }

    // Phase 3: Free deferred pages collected during BgcSweep.
    // BgcCompact runs under STW safepoint — no mutators are running, so
    // it is safe to VirtualFree pages that were unlinked from page_list_
    // by BgcSweep's Phase 4b but deferred to avoid use-after-free through
    // stale page_array_ entries in concurrent mutators.
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto* p : deferred_free_pages_) {
            VirtualFreePage(p, p->page_size);
        }
        deferred_free_pages_.clear();
    }

    // Clear compaction skip list (snapshot of pinned_roots_ taken above).
    pinned_compact_skip_.clear();

}

}  // namespace chaos::il2cpp::runtime_core
