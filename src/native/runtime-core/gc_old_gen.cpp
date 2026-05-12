#include "gc_old_gen.h"

#include <chaos/log.h>
#include <chaos/profile.h>

#include "gc_bit_utils.h"
#include "gc_card_table.h"
#include "gc_layout.h"
#include "gc_region.h"
#include "gc_stats.h"
#include "thread_pool.h"
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

// ── Global instance ────────────────────────────────────────────────
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
}

// ======================================================================
// Init
// ======================================================================

bool MarkSweepOldGen::Init(uintptr_t heap_hint, int initial_pages) {
    (void)heap_hint;  // hint not used yet — VirtualAlloc picks address

    CHAOS_IL2CPP_LOG_INFO_M("OldGen", "init initial_pages={0}", initial_pages);

    for (int i = 0; i < initial_pages; i++) {
        auto* page = AllocatePage(kOldGenPageSize, true);
        if (page == nullptr) {
            CHAOS_IL2CPP_LOG_ERROR_M("OldGen", "failed to allocate initial page {0}", i);
            return false;
        }
    }

    CHAOS_IL2CPP_LOG_INFO_M("OldGen", "init done base=0x{0} pages={1}",
        static_cast<unsigned long long>(heap_base_), page_count_);
    return true;
}

// ======================================================================
// Page management
// ======================================================================

OldGenPage* MarkSweepOldGen::AllocatePage(CHAOS_IL2CPP_SIZE size, bool scanning) {
    CHAOS_IL2CPP_PROFILE_SCOPE("OldGen::AllocatePage");

    // Calculate: header + aligned bitmap + payload.
    CHAOS_IL2CPP_SIZE payload_size = size;
    CHAOS_IL2CPP_SIZE bitmap_bytes = (payload_size / sizeof(void*)) / 8;
    bitmap_bytes = (bitmap_bytes + 15) & ~static_cast<CHAOS_IL2CPP_SIZE>(15);
    CHAOS_IL2CPP_SIZE total_size = sizeof(OldGenPage) + bitmap_bytes + payload_size;

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
    mem->is_oversized = (payload_size > kOldGenMaxInline);

    // Initialize free lists.
    for (int i = 0; i < kOldGenNumSizeClasses; i++) {
        mem->free_lists[i] = nullptr;
    }

    // Clear mark bitmap.
    std::memset(mem->MarkBitmap(), 0, bitmap_bytes);

    // Carve payload into size-class blocks.
    char* payload = mem->Payload();
    CHAOS_IL2CPP_SIZE remaining = payload_size;

    // Ensure at least 2 blocks per small size class for quick allocation.
    for (int sc = 0; sc < kOldGenNumSizeClasses && remaining >= kOldGenSizeClasses[sc]; sc++) {
        CHAOS_IL2CPP_SIZE sc_size = kOldGenSizeClasses[sc];
        int batch = (sc_size <= 256) ? 4 : 1;
        for (int i = 0; i < batch && remaining >= sc_size; i++) {
            auto* block = reinterpret_cast<OldGenFreeBlock*>(payload);
            block->next = mem->free_lists[sc];
            mem->free_lists[sc] = block;
            payload += sc_size;
            remaining -= sc_size;
        }
    }
    // Fill remaining with largest fitting size class.
    while (remaining >= kOldGenSizeClasses[0]) {
        int sc = kOldGenNumSizeClasses - 1;
        while (sc >= 0 && kOldGenSizeClasses[sc] > remaining) sc--;
        if (sc < 0) break;
        auto* block = reinterpret_cast<OldGenFreeBlock*>(payload);
        block->next = mem->free_lists[sc];
        mem->free_lists[sc] = block;
        payload += kOldGenSizeClasses[sc];
        remaining -= kOldGenSizeClasses[sc];
    }

    // Link into page list.
    {
        std::lock_guard<std::mutex> lock(mutex_);
        mem->next = page_list_;
        page_list_ = mem;
        page_count_++;

        if (heap_base_ == 0) {
            heap_base_ = reinterpret_cast<uintptr_t>(mem);
            // Align card table heap_base to the actual first page address.
            GcSetHeapBase(reinterpret_cast<void*>(heap_base_));
        }
    }

    total_allocated_.fetch_add(total_size, std::memory_order_relaxed);
    return mem;
}

void MarkSweepOldGen::FreePage(OldGenPage* page) {
    if (page == nullptr) return;
    VirtualFreePage(page, page->page_size);
}

OldGenPage* MarkSweepOldGen::FindPage(const void* ptr) {
    if (ptr == nullptr) return nullptr;
    auto* page = page_list_;
    while (page != nullptr) {
        uintptr_t start = reinterpret_cast<uintptr_t>(page);
        uintptr_t end = start + page->page_size;
        uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
        if (addr >= start && addr < end) {
            return page;
        }
        page = page->next;
    }
    return nullptr;
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

    // Walk pages looking for a free-list hit.
    auto* page = page_list_;
    while (page != nullptr) {
        if (page->free_lists[sc_idx] != nullptr && page->in_use.load(std::memory_order_relaxed)) {
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
    if (page_list_ == nullptr) {
        Init(0, 2);
    }

    // Oversized: direct page allocation sized to fit the request.
    if (size > kOldGenMaxInline) {
        CHAOS_IL2CPP_LOG_DEBUG_M("OldGen", "oversized_alloc size={0}",
            static_cast<unsigned long long>(size));
        auto* page = AllocatePage(size, scanning_required);
        if (page == nullptr) return nullptr;
        auto* result = page->Payload();
        std::memset(result, 0, size);
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
        // Now retry the free list — the new page was pre-carved.
        sc_idx = SizeClassIndex(size);
        if (sc_idx < 0) {
            // Still no match (shouldn't happen for aligned < 32KB).
            return page->Payload();
        }
    }

    // Try free lists.
    auto* ptr = TryAllocateFromFreeLists(size, sc_idx);
    if (ptr != nullptr) return ptr;

    // Miss: allocate a new page.
    auto* page = AllocatePage(kOldGenPageSize, scanning_required);
    if (page == nullptr) return nullptr;

    // Retry free list (new page was pre-carved with size-class blocks).
    return TryAllocateFromFreeLists(size, sc_idx);
}

void MarkSweepOldGen::Free(void* ptr) {
    if (ptr == nullptr) return;

    auto* page = FindPage(ptr);
    if (page == nullptr) {
        CHAOS_IL2CPP_LOG_WARN_M("OldGen", "free_unknown_ptr {0}", ptr);
        return;
    }

    // Oversized pages: mark for decommission (will be freed in next Collect Phase 4).
    if (page->is_oversized) {
        page->in_use.store(false, std::memory_order_release);
        return;
    }

    // Compute the block's offset within the page payload to find its size class.
    auto addr = reinterpret_cast<uintptr_t>(ptr);
    auto payload_start = reinterpret_cast<uintptr_t>(page->Payload());
    if (addr < payload_start) return;

    // Walk the carving layout to find the actual block size.
    // The carving is deterministic from AllocatePage: first small batches,
    // then fill with largest fitting size class.
    CHAOS_IL2CPP_SIZE offset = addr - payload_start;
    CHAOS_IL2CPP_SIZE remaining = page->payload_size;
    CHAOS_IL2CPP_SIZE cursor = 0;
    bool found = false;

    for (int sc = 0; sc < kOldGenNumSizeClasses && remaining >= kOldGenSizeClasses[sc]; sc++) {
        CHAOS_IL2CPP_SIZE sc_size = kOldGenSizeClasses[sc];
        int batch = (sc_size <= 256) ? 4 : 1;
        for (int i = 0; i < batch && remaining >= sc_size; i++) {
            if (cursor == offset) {
                // Found the block: return it to the size-class free list.
                std::lock_guard<std::mutex> lock(mutex_);
                auto* block = reinterpret_cast<OldGenFreeBlock*>(ptr);
                block->next = page->free_lists[sc];
                page->free_lists[sc] = block;
                found = true;
                break;
            }
            cursor += sc_size;
            remaining -= sc_size;
        }
        if (found) break;
    }
    if (!found) {
        // Fallback: the block might be in the "fill remaining" tail.
        // Zero the memory and let sweep handle it.
        std::memset(ptr, 0, 64);
    }
}

void* MarkSweepOldGen::Reallocate(void* ptr, CHAOS_IL2CPP_SIZE new_size) {
    if (ptr == nullptr) return Allocate(new_size, true);
    if (new_size == 0) { Free(ptr); return nullptr; }

    // We don't track individual block sizes precisely at C3,
    // so always allocate new + copy (heuristic using page payload as bound).
    auto* new_ptr = Allocate(new_size, true);
    if (new_ptr == nullptr) return nullptr;

    auto* page = FindPage(ptr);
    CHAOS_IL2CPP_SIZE old_size = new_size;
    if (page != nullptr) {
        auto addr = reinterpret_cast<uintptr_t>(ptr);
        auto pstart = reinterpret_cast<uintptr_t>(page->Payload());
        auto remaining = page->payload_size - (addr - pstart);
        old_size = std::min(remaining, new_size);
    }
    std::memcpy(new_ptr, ptr, old_size);

    // For C3, we do NOT free the old pointer (let GC sweep handle it).
    // Free() is not reliable without size tracking.

    return new_ptr;
}

// ======================================================================
// GC Collection — Mark phase
// ======================================================================

bool MarkSweepOldGen::MarkObject(void* obj) {
    if (obj == nullptr) return false;

    auto* page = FindPage(obj);
    if (page == nullptr) return false;

    // Compute slot index within page.
    auto obj_addr = reinterpret_cast<uintptr_t>(obj);
    auto payload_start = reinterpret_cast<uintptr_t>(page->Payload());
    if (obj_addr < payload_start) return false;

    CHAOS_IL2CPP_SIZE offset = obj_addr - payload_start;
    CHAOS_IL2CPP_SIZE slot_idx = offset / sizeof(void*);
    CHAOS_IL2CPP_SIZE byte_idx = slot_idx / 8;
    int bit_idx = static_cast<int>(slot_idx % 8);

    auto* bitmap = page->MarkBitmap();
    unsigned char mask = static_cast<unsigned char>(1u << bit_idx);

    if ((bitmap[byte_idx] & mask) != 0) {
        return false;  // already marked
    }

    bitmap[byte_idx] |= mask;
    marked_count_.fetch_add(1, std::memory_order_relaxed);
    return true;  // newly marked (was white, now grey)
}

void MarkSweepOldGen::DrainMarkStack() {
    auto& layout_registry = GcLayoutRegistry::Instance();

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
            // Not a valid TypeInfo — conservative fallback.
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

        // Valid TypeInfo — look up the precise GC layout.
        auto* hot = static_cast<const TypeInfoHot*>(type_info_ptr);
        uint64_t stable_id = hot->stable_id;
        const auto* layout = layout_registry.Lookup(stable_id);

        if (layout == nullptr || layout->pointer_count == 0) {
            // No layout or pointer-free — skip scanning.
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

// ======================================================================
// GC Collection — Sweep phase
// ======================================================================

CHAOS_IL2CPP_SIZE MarkSweepOldGen::SweepPage(OldGenPage* page) {
    if (page == nullptr) return 0;

    // Oversized pages: single-object.  If nothing is marked, the entire page
    // is garbage — return its size as reclaimed.  Mark it as not-in-use so
    // Collect() Phase 4 can free it.
    if (page->is_oversized) {
        auto* bitmap = page->MarkBitmap();
        bool any_marked = false;
        CHAOS_IL2CPP_SIZE bm_words = page->bitmap_bytes;
        for (CHAOS_IL2CPP_SIZE w = 0; w < bm_words; w++) {
            if (bitmap[w] != 0) { any_marked = true; break; }
        }
        std::memset(bitmap, 0, bm_words);
        if (!any_marked) {
            // Mark for decommission — Collect() Phase 4 will free this page.
            page->in_use.store(false, std::memory_order_release);
            return page->payload_size;
        }
        return 0;
    }

    auto* bitmap = reinterpret_cast<const uint64_t*>(page->MarkBitmap());
    char* payload = page->Payload();
    CHAOS_IL2CPP_SIZE reclaimed = 0;

    // Walk the bitmap 64 bits (one uint64_t word) at a time.
    // Each bit corresponds to one pointer-sized slot (8 bytes).
    CHAOS_IL2CPP_SIZE num_words = page->bitmap_bytes / sizeof(uint64_t);
    CHAOS_IL2CPP_SIZE slot = 0;  // global slot index across the page

    // Fast path: contiguous run of unmarked slots → single free block.
    auto free_run = [&](CHAOS_IL2CPP_SIZE start_slot, CHAOS_IL2CPP_SIZE end_slot) {
        if (start_slot >= end_slot) return;
        CHAOS_IL2CPP_SIZE run_bytes = (end_slot - start_slot) * sizeof(void*);
        reclaimed += run_bytes;

        int sc_idx = SizeClassIndex(run_bytes);
        if (sc_idx >= 0) {
            auto* block = reinterpret_cast<OldGenFreeBlock*>(payload + start_slot * sizeof(void*));
            block->next = page->free_lists[sc_idx];
            page->free_lists[sc_idx] = block;
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
            // Fast path 1: all 64 slots marked — nothing to free.
            slot += 64;
            continue;
        }

        if (word == 0) {
            // Fast path 2: all 64 slots unmarked — one contiguous free block.
            free_run(slot, slot + 64);
            slot += 64;
            continue;
        }

        // Slow path: mixed — iterate only the ZERO bits (unmarked slots).
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

    // Clear bitmap for next cycle.
    std::memset(page->MarkBitmap(), 0, page->bitmap_bytes);

    return reclaimed;
}

void MarkSweepOldGen::CoalescePage(OldGenPage* page) {
    if (page == nullptr || page->is_oversized) return;

    std::lock_guard<std::mutex> lock(mutex_);

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
        // Clear the free list — we'll rebuild it from merged blocks.
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
            // Adjacent — merge.
            last.size += blocks[i].size;
        } else {
            merged.push_back(blocks[i]);
        }
    }

    // Return merged blocks to appropriate size-class free lists.
    for (auto& b : merged) {
        int sc_idx = SizeClassIndex(b.size);
        if (sc_idx >= 0) {
            auto* block = reinterpret_cast<OldGenFreeBlock*>(b.addr);
            block->next = page->free_lists[sc_idx];
            page->free_lists[sc_idx] = block;
        }
        // If no matching size class, leave the block as-is (sweep will handle it).
    }
}

// ======================================================================
// GC Collection — full collect
// ======================================================================

void MarkSweepOldGen::Collect(void (*root_callback)(void* obj, void* user_data), void* user_data) {
    CHAOS_IL2CPP_PROFILE_SCOPE("OldGen::Collect");

    auto pause_start = std::chrono::steady_clock::now();

    CHAOS_IL2CPP_LOG_INFO_M("OldGen", "collect_start page_count={0}", page_count_);

    // Phase 1: Mark roots.
    bool has_roots = (root_callback != nullptr) || !pinned_roots_.empty();

    if (root_callback != nullptr) {
        root_callback(nullptr, user_data);  // signal start (optional)
    }

    // Mark pinned roots.
    for (auto& pr : pinned_roots_) {
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

        // EnumerateThreads takes a C function pointer — use a static
        // helper since g_old_gen is a process-wide global.
        threading::EnumerateThreads(
            [](threading::ManagedThread* thread) -> bool {
                if (thread->nursery_ctx == nullptr) return true;
                auto* nursery = thread->nursery_ctx->nursery;
                if (nursery == nullptr) return true;

                void* nursery_begin = nursery->begin;
                void* nursery_cur   = nursery->current;
                if (nursery_cur > nursery_begin) {
                    g_old_gen.ScanRangeForRoots(nursery_begin, nursery_cur);
                }
                return true;  // continue enumeration
            });

        if (mark_stack_.size() > before_roots) {
            has_roots = true;
        }
        CHAOS_IL2CPP_LOG_DEBUG_M("OldGen", "collect_scanned_nurseries");
    }

    // If no roots were found, skip mark-sweep entirely.
    // Sweeping without roots would free all pages, corrupting live objects
    // in worker threads that the collector has no visibility into.
    if (!has_roots) {
        auto pause_end = std::chrono::steady_clock::now();
        uint64_t pause_ns = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(pause_end - pause_start).count());
        GcRecordFullCollection(
            static_cast<CHAOS_IL2CPP_SIZE>(page_count_),
            0, 0, 0, pause_ns);
        CHAOS_IL2CPP_LOG_INFO_M("OldGen", "collect_skipped_no_roots pause_ns={0}", pause_ns);
        return;
    }

    // Phase 2: Mark transitive closure.

    // DrainMarkStack processes the mark stack (grey objects), scanning their
    // pointer fields and marking referenced objects.  This happens AFTER root
    // marking (Phase 1) so the stack is non-empty when we start.
    DrainMarkStack();

    // Phase 3: Sweep all pages (parallel when beneficial).
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
        // Main thread participates alongside thread pool workers.
        std::atomic<int> next_page{0};
        std::atomic<CHAOS_IL2CPP_SIZE> parallel_reclaimed{0};
        std::atomic<int> workers_done{0};
        int num_workers = std::thread::hardware_concurrency();
        if (num_workers < 2) num_workers = 2;

        auto sweep_worker = [&](bool is_main) {
            CHAOS_IL2CPP_SIZE local_reclaimed = 0;
            while (true) {
                int idx = next_page.fetch_add(1, std::memory_order_relaxed);
                if (idx >= total_pages) break;
                local_reclaimed += SweepPage(pages[static_cast<size_t>(idx)]);
                CoalescePage(pages[static_cast<size_t>(idx)]);
            }
            parallel_reclaimed.fetch_add(local_reclaimed, std::memory_order_relaxed);
            if (!is_main) {
                workers_done.fetch_add(1, std::memory_order_release);
            }
        };

        // Spawn thread pool workers.
        int spawned = 0;
        for (int i = 0; i < num_workers - 1; i++) {
            threading::ThreadPoolQueueUserWorkItem(
                [](void* ctx) {
                    auto* worker_fn = static_cast<decltype(&sweep_worker)>(ctx);
                    (*worker_fn)(false);
                },
                &sweep_worker);
            spawned++;
        }

        // Main thread participates.
        sweep_worker(true);

        // Wait for all spawned workers to finish.
        while (workers_done.load(std::memory_order_acquire) < spawned) {
            CHAOS_OLDGEN_SPIN_HINT();
        }

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
            if (!p->in_use.load(std::memory_order_acquire)) {
                *pp = p->next;
                FreePage(p);
                page_count_--;
            } else {
                pp = &p->next;
            }
        }
    }

    // Phase 5: Run finalizers for unreachable objects.
    CHAOS_IL2CPP_SIZE finalizers_run = RunFinalizers();

    auto pause_end = std::chrono::steady_clock::now();
    uint64_t pause_ns = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(pause_end - pause_start).count());

    CHAOS_IL2CPP_SIZE marked_count = static_cast<CHAOS_IL2CPP_SIZE>(
        marked_count_.exchange(0, std::memory_order_relaxed));

    // Record into GcStats.
    GcRecordFullCollection(
        static_cast<CHAOS_IL2CPP_SIZE>(page_count_),
        marked_count,
        total_reclaimed,
        finalizers_run,
        pause_ns);

    CHAOS_IL2CPP_LOG_INFO_M("OldGen", "collect_done reclaimed={0} pause_ns={1}",
        static_cast<unsigned long long>(total_reclaimed), pause_ns);
}

bool MarkSweepOldGen::CollectIncremental() {
    // At C3, incremental collection is a full collect.
    Collect(nullptr, nullptr);
    return true;
}

// ======================================================================
// Thread stack registration (replaces GC_register_my_thread)
// ======================================================================

struct ThreadStackInfo {
    uintptr_t stack_base;
    uintptr_t stack_limit;  // low address (stack grows downward)
};

// Per-thread stack info for root scanning during GC.
// At C3, this is a simple registry.  A production version would use
// TLS for O(1) lookup instead of a lock-protected map.
static std::mutex s_thread_stack_mutex;
static std::vector<ThreadStackInfo> s_thread_stacks;

void MarkSweepOldGen::RegisterThreadStack(void* stack_base, void* stack_limit) {
    ThreadStackInfo info;
    info.stack_base = reinterpret_cast<uintptr_t>(stack_base);
    info.stack_limit = reinterpret_cast<uintptr_t>(stack_limit);

    std::lock_guard<std::mutex> lock(s_thread_stack_mutex);
    s_thread_stacks.push_back(info);
}

void MarkSweepOldGen::UnregisterThreadStack() {
    // Find and remove the current thread's stack entry.
    // For C3, we match by approximate stack address (current frame pointer).
    // A production version would use TLS.
#if defined(_M_IX86) || defined(__i386__) || defined(_M_X64) || defined(__x86_64__)
    void* frame_ptr = _AddressOfReturnAddress();
#else
    void* frame_ptr = nullptr;
#endif
    uintptr_t approx = reinterpret_cast<uintptr_t>(frame_ptr);

    std::lock_guard<std::mutex> lock(s_thread_stack_mutex);
    for (size_t i = 0; i < s_thread_stacks.size(); i++) {
        if (approx >= s_thread_stacks[i].stack_limit &&
            approx <= s_thread_stacks[i].stack_base) {
            s_thread_stacks.erase(s_thread_stacks.begin() + static_cast<ptrdiff_t>(i));
            return;
        }
    }
}

void MarkSweepOldGen::AddPinnedRoot(void* addr, CHAOS_IL2CPP_SIZE size) {
    std::lock_guard<std::mutex> lock(mutex_);
    pinned_roots_.push_back({addr, size});
    CHAOS_IL2CPP_LOG_DEBUG_M("OldGen", "add_pinned_root {0} size={1}", addr,
        static_cast<unsigned long long>(size));
}

// ── Root scanning ─────────────────────────────────────────

void MarkSweepOldGen::ScanRangeForRoots(void* range_begin, void* range_end) {
    if (range_begin == nullptr || range_end == nullptr) return;
    if (range_begin >= range_end) return;

    uintptr_t begin = reinterpret_cast<uintptr_t>(range_begin);
    uintptr_t end   = reinterpret_cast<uintptr_t>(range_end);

    for (uintptr_t slot = begin; slot + sizeof(void*) <= end; slot += sizeof(void*)) {
        auto val = *reinterpret_cast<void**>(slot);
        if (val == nullptr) continue;
        auto* page = FindPage(val);
        if (page != nullptr && page->in_use.load(std::memory_order_acquire)) {
            if (MarkObject(val)) {
                mark_stack_.push_back(val);
            }
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
    CHAOS_IL2CPP_LOG_DEBUG_M("OldGen", "register_finalizer obj={0}", obj);
}

CHAOS_IL2CPP_SIZE MarkSweepOldGen::RunFinalizers() {
    std::vector<FinalizerEntry> to_run;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        to_run.swap(finalizers_);
    }

    for (auto& entry : to_run) {
        CHAOS_IL2CPP_LOG_DEBUG_M("OldGen", "run_finalizer obj={0}", entry.obj);
        if (entry.finalizer) {
            entry.finalizer(entry.obj);
        }
    }

    return to_run.size();
}

}  // namespace chaos::il2cpp::runtime_core