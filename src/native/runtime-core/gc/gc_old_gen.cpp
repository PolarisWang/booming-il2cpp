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
#include "gc_scheduler.h"
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

        // Register page with the two-level card table so write barriers
        // can find L2 segments for objects allocated in this page.
        GcRegisterHeapRange(reinterpret_cast<uintptr_t>(mem),
                            reinterpret_cast<uintptr_t>(mem) + total_size);
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

bool MarkSweepOldGen::IsInOldGen(const void* ptr) const {
    if (ptr == nullptr) return false;
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
    auto* page = page_list_;
    while (page != nullptr) {
        uintptr_t start = reinterpret_cast<uintptr_t>(page->Payload());
        uintptr_t end = start + page->payload_size;
        if (addr >= start && addr < end) {
            return true;
        }
        page = page->next;
    }
    return false;
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

    // Check the cached last-used page first — avoids O(n) walk.
    auto* page = last_alloc_page_[sc_idx];
    if (page != nullptr && page->in_use.load(std::memory_order_relaxed)) {
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
        if (page->free_lists[sc_idx] != nullptr && page->in_use.load(std::memory_order_relaxed)) {
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
    // AllocatePage() which takes mutex_ internally — using mutex_ here
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
            CHAOS_IL2CPP_LOG_DEBUG_M("OldGen", "loh_alloc size={0}",
                static_cast<unsigned long long>(size));
            void* loh_ptr = g_loh.Allocate(size);
            if (loh_ptr != nullptr) {
                GcRecordAlloc(size, true);
            }
            return loh_ptr;
        }

        // 32-85 KB: oversized page allocation (existing behavior).
        CHAOS_IL2CPP_LOG_DEBUG_M("OldGen", "oversized_alloc size={0}",
            static_cast<unsigned long long>(size));

        // Check GC pressure before oversized allocation.
        auto decision = g_gc_scheduler.DecideCollection();
        if (decision == GcCollectionKind::FULL) {
            CHAOS_IL2CPP_LOG_DEBUG_M("OldGen", "gc_before_oversized");
            uint32_t gen = threading::RequestGlobalSafepoint();
            Collect(nullptr, nullptr);
            threading::ReleaseGlobalSafepoint(gen);
            // RecordFullCollection is handled inside Collect().
        } else if (decision == GcCollectionKind::YOUNG) {
            // For oversized, we don't run young GC (nursery is irrelevant).
            // The scheduler will trigger young GC on the next normal allocation.
        }

        auto* page = AllocatePage(size, scanning_required);
        if (page == nullptr) return nullptr;
        auto* result = page->Payload();
        std::memset(result, 0, size);
        GcRecordAlloc(size, true);
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
            return result;
        }
    }

    // Try free lists.
    auto* ptr = TryAllocateFromFreeLists(size, sc_idx);
    if (ptr != nullptr) {
        GcRecordAlloc(size, false);
        return ptr;
    }

    // Miss: allocate a new page.
    auto* page = AllocatePage(kOldGenPageSize, scanning_required);
    if (page == nullptr) return nullptr;

    // Retry free list (new page was pre-carved with size-class blocks).
    ptr = TryAllocateFromFreeLists(size, sc_idx);
    if (ptr != nullptr) {
        GcRecordAlloc(size, false);
    }
    return ptr;
}

void MarkSweepOldGen::Free(void* ptr) {
    if (ptr == nullptr) return;

    // Oversized pages: unlink from page list and virtual-free immediately.
    // Take the mutex first to protect the FindPage → page_list_ traversal
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
        VirtualFreePage(page, page->page_size);
        return;
    }

    // Non-oversized: zero the memory and let GC sweep handle reclamation.
    // We do NOT touch the free-list here — adding the block back would
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
    // Free() is not reliable without size tracking — call it for oversized pages
    // where FindPage+Free is safe (immediate VirtualFree of oversized page).
    // For non-oversized, the old block will be reclaimed at next sweep.
    if (page != nullptr && page->is_oversized) {
        Free(ptr);
    }

    return new_ptr;
}

// ======================================================================
// GC Collection — Mark phase
// ======================================================================

bool MarkSweepOldGen::MarkObject(void* obj) {
    if (obj == nullptr) return false;

    auto* page = FindPage(obj);
    if (page == nullptr) return false;

    auto obj_addr = reinterpret_cast<uintptr_t>(obj);
    auto payload_start = reinterpret_cast<uintptr_t>(page->Payload());
    if (obj_addr < payload_start) return false;

    CHAOS_IL2CPP_SIZE offset = obj_addr - payload_start;
    CHAOS_IL2CPP_SIZE slot_idx = offset / sizeof(void*);
    CHAOS_IL2CPP_SIZE byte_idx = slot_idx / 8;
    int bit_idx = static_cast<int>(slot_idx % 8);

    auto* bitmap = page->MarkBitmap();
    unsigned char mask = static_cast<unsigned char>(1u << bit_idx);

    // Atomic mark: safe for concurrent access from parallel mark workers.
    // Returns true if the bit was newly set (was 0, now 1).
#if defined(_MSC_VER) && !defined(__clang__)
    auto prev = _InterlockedOr8(reinterpret_cast<volatile char*>(&bitmap[byte_idx]),
                                 static_cast<char>(mask));
    if ((static_cast<unsigned char>(prev) & mask) != 0) {
        return false;  // already marked
    }
#else
    auto prev = __atomic_fetch_or(&bitmap[byte_idx], mask, __ATOMIC_RELAXED);
    if ((prev & mask) != 0) {
        return false;  // already marked
    }
#endif

    marked_count_.fetch_add(1, std::memory_order_relaxed);
    return true;  // newly marked (was white, now grey)
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
                if (w0->has_pending && w0->pending.page_idx == pi &&
                    (slot / 64) == 0) {  // word 0 only for simplicity
                    w0->pending.bitmap |= bit;
                } else {
                    FlushPending(w0);
                    w0->pending.page_idx = pi;
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
    int spawned = 0;
    for (int i = 1; i < ctx->worker_count; i++) {
        threading::ThreadPoolQueueUserWorkItem(
            [](void* param) {
                auto p = static_cast<std::pair<ParallelMarkContext*, int>*>(param);
                ParallelMarkWorkerLoop(p->first, p->second);
                delete p;
            },
            new std::pair<ParallelMarkContext*, int>(ctx, i));
        spawned++;
    }

    // Signal all workers to start.
    ctx->drain_started.store(true, std::memory_order_release);

    // Worker 0 participates as a parallel worker.
    if (ctx->worker_count >= 1) {
        ParallelMarkWorkerLoop(ctx, 0);
    }

    // Wait for all spawned workers to finish.
    while (ctx->active_workers.load(std::memory_order_acquire) > 0) {
        CHAOS_OLDGEN_SPIN_HINT();
    }

    // Drain any remaining chunks left in deques (workers may have been preempted).
    // As a safety net, process any remaining mark entries sequentially.
    for (int i = 0; i < ctx->worker_count; i++) {
        MarkChunk chunk;
        while (PopChunk(&ctx->workers[i], &chunk)) {
            if (chunk.page_idx >= 0 && chunk.page_idx < page_count) {
                auto* page = pages[chunk.page_idx];
                char* payload = page->Payload();
                auto& layout_registry = GcLayoutRegistry::Instance();
                uint64_t word = chunk.bitmap;
                while (word != 0) {
                    int bit = GcCtz64(word);
                    word &= word - 1;
                    void* obj = payload + static_cast<CHAOS_IL2CPP_SIZE>(bit) * sizeof(void*);
                    const void* type_info_ptr = *static_cast<const void* const*>(obj);
                    if (type_info_ptr == nullptr) continue;
                    if (!layout_registry.IsValidTypeInfoPointer(type_info_ptr)) continue;
                    auto* hot = static_cast<const TypeInfoHot*>(type_info_ptr);
                    uint64_t stable_id = hot->stable_id;
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

    CHAOS_IL2CPP_LOG_DEBUG("OldGen", "parallel_mark_done");
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

        // Split the run into size-class blocks from largest to smallest,
        // so that every byte of reclaimed memory ends up on a free list.
        auto* cursor = payload + start_slot * sizeof(void*);
        CHAOS_IL2CPP_SIZE remaining = run_bytes;
        while (remaining > 0) {
            int sc_idx = SizeClassIndex(remaining);
            if (sc_idx < 0) {
                // No single size class fits the remainder — carve off
                // the largest size class and continue with the rest.
                sc_idx = kOldGenNumSizeClasses - 1;
            }
            CHAOS_IL2CPP_SIZE sc_size = kOldGenSizeClasses[sc_idx];
            if (sc_size > remaining) {
                // Last-chance fallback: skip one slot and retry so we
                // don't leak non-size-class-aligned remainders.
                sc_size = sizeof(void*);
                sc_idx = SizeClassIndex(sc_size);
                if (sc_idx < 0) break;
            }
            auto* block = reinterpret_cast<OldGenFreeBlock*>(cursor);
            block->next = page->free_lists[sc_idx];
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

    // CoalescePage only touches this page's own free lists — no other thread
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
                sc_size = sizeof(void*);
                sc_idx = SizeClassIndex(sc_size);
                if (sc_idx < 0) break;
            }
            auto* block = reinterpret_cast<OldGenFreeBlock*>(cursor);
            block->next = page->free_lists[sc_idx];
            page->free_lists[sc_idx] = block;
            cursor += sc_size;
            remaining -= sc_size;
        }
    }
}

// ======================================================================
// GC Collection — full collect
// ======================================================================

/// Per-page relocation plan entry for compaction.
struct CompactPlanEntry {
    void*   old_addr;      ///< Current object address
    void*   new_addr;      ///< New (compacted) object address
    CHAOS_IL2CPP_SIZE size;  ///< Object size in bytes
};

/// Per-page compaction plan.
struct CompactPlan {
    std::vector<CompactPlanEntry> entries;
    CHAOS_IL2CPP_SIZE live_bytes{0};
    CHAOS_IL2CPP_SIZE saved_bytes{0};
};

float MarkSweepOldGen::PageFragmentation(const OldGenPage* page) const {
    if (page == nullptr || page->is_oversized) return 0.0f;
    auto* bitmap = reinterpret_cast<const uint64_t*>(page->MarkBitmap());
    CHAOS_IL2CPP_SIZE num_words = page->bitmap_bytes / sizeof(uint64_t);
    CHAOS_IL2CPP_SIZE marked_slots = 0;
    for (CHAOS_IL2CPP_SIZE w = 0; w < num_words; w++) {
        marked_slots += static_cast<CHAOS_IL2CPP_SIZE>(GcPopCount64(bitmap[w]));
    }
    CHAOS_IL2CPP_SIZE live = marked_slots * sizeof(void*);
    float ratio = 1.0f - (static_cast<float>(live) / static_cast<float>(page->payload_size));
    return (ratio < 0.0f) ? 0.0f : ratio;
}

MarkSweepOldGen::CompactMode MarkSweepOldGen::DecideCompactMode() {
    // Check fragmentation across all pages.  If any size class has >30%
    // fragmentation on average, trigger compaction.
    // For C4, we use a simple threshold: if ANY page has >30% fragmentation.
    std::lock_guard<std::mutex> lock(mutex_);
    auto* p = page_list_;
    while (p != nullptr) {
        if (p->in_use.load(std::memory_order_acquire) && !p->is_oversized) {
            float frag = PageFragmentation(p);
            if (frag > 0.30f) {
                return CompactMode::COMPACT;
            }
        }
        p = p->next;
    }
    return CompactMode::NONE;
}

CHAOS_IL2CPP_SIZE MarkSweepOldGen::PlanPageCompaction(OldGenPage* page,
                                                        CHAOS_IL2CPP_SIZE* out_live_bytes) {
    if (page == nullptr || page->is_oversized) {
        if (out_live_bytes) *out_live_bytes = 0;
        return 0;
    }

    auto* bitmap = reinterpret_cast<const uint64_t*>(page->MarkBitmap());
    char* payload = page->Payload();
    CHAOS_IL2CPP_SIZE num_words = page->bitmap_bytes / sizeof(uint64_t);
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
        // Scan all set bits in this word using a local copy that we
        // progressively clear.  We must NOT re-read bitmap[w] each
        // iteration (that would re-process the first set bit forever).
        uint64_t remaining = word;
        while (remaining != 0) {
            int bit = GcCtz64(remaining);
            remaining &= remaining - 1;
            CHAOS_IL2CPP_SIZE obj_slot = w * 64 + static_cast<CHAOS_IL2CPP_SIZE>(bit);
            if (obj_slot >= num_slots) break;
            void* obj = payload + obj_slot * sizeof(void*);

            // Determine object size from TypeInfo layout.
            CHAOS_IL2CPP_SIZE obj_size = sizeof(void*);
            const void* type_info_ptr = *static_cast<const void* const*>(obj);
            if (type_info_ptr != nullptr) {
                auto& registry = GcLayoutRegistry::Instance();
                if (registry.IsValidTypeInfoPointer(type_info_ptr)) {
                    auto* hot = static_cast<const TypeInfoHot*>(type_info_ptr);
                    uint64_t stable_id = hot->stable_id;
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

    if (marked.empty()) {
        if (out_live_bytes) *out_live_bytes = 0;
        return 0;
    }

    // Plan: compact objects to start of page payload, preserving order.
    // Pinned objects are skipped (they stay in place and force gaps).
    std::vector<CompactPlanEntry> plan;
    plan.reserve(marked.size());
    char* cursor = payload;
    CHAOS_IL2CPP_SIZE total_live = 0;

    for (auto& mo : marked) {
        plan.push_back({mo.addr, cursor, mo.size});
        cursor += mo.size;
        total_live += mo.size;
    }

    CHAOS_IL2CPP_SIZE saved = static_cast<CHAOS_IL2CPP_SIZE>(
        reinterpret_cast<uintptr_t>(cursor) - reinterpret_cast<uintptr_t>(payload));
    saved = page->payload_size - saved;
    // saved is actually the total live bytes packed into a smaller space.
    // The actual saving is: payload_size - total_live.
    if (out_live_bytes) *out_live_bytes = total_live;
    return page->payload_size - total_live;
}

void MarkSweepOldGen::RelocatePage(OldGenPage* page) {
    if (page == nullptr || page->is_oversized) return;
    // Relocate all old-gen pointer references to point to compacted addresses.
    // Walk every pointer-aligned slot in every old-gen page; if the slot value
    // points to an object in this page that was moved, update the slot.
    // This runs under STW so no concurrent access concerns.
    //
    // Build a sorted old→new address map from the mark bitmap using precise
    // object sizes (from GcLayout).  Then use it for O(log N) lookup per slot.

    char* payload = page->Payload();
    uintptr_t page_start = reinterpret_cast<uintptr_t>(payload);
    uintptr_t page_end = page_start + page->payload_size;
    CHAOS_IL2CPP_SIZE num_slots = page->payload_size / sizeof(void*);
    CHAOS_IL2CPP_SIZE num_words = page->bitmap_bytes / sizeof(uint64_t);
    auto* bitmap = reinterpret_cast<const uint64_t*>(page->MarkBitmap());

    // Build sorted vector of (old_addr, new_addr) for all marked objects.
    struct AddrPair { uintptr_t old_addr; uintptr_t new_addr; };
    std::vector<AddrPair> addr_map;
    addr_map.reserve(1024);

    uintptr_t compact_cursor = page_start;
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
            uintptr_t obj_addr = page_start + obj_slot * sizeof(void*);

            // Determine object size from TypeInfo layout.
            CHAOS_IL2CPP_SIZE obj_size = sizeof(void*);
            const void* type_info_ptr = *reinterpret_cast<void* const*>(obj_addr);
            if (type_info_ptr != nullptr) {
                auto& registry = GcLayoutRegistry::Instance();
                if (registry.IsValidTypeInfoPointer(type_info_ptr)) {
                    auto* hot = static_cast<const TypeInfoHot*>(type_info_ptr);
                    uint64_t stable_id = hot->stable_id;
                    const auto* layout = registry.Lookup(stable_id);
                    if (layout != nullptr && layout->instance_size > 0) {
                        obj_size = layout->instance_size;
                    }
                }
            }

            addr_map.push_back({obj_addr, compact_cursor});
            compact_cursor += obj_size;
        }
        slot = (w + 1) * 64;
    }

    if (addr_map.empty()) return;

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

            // Binary search in addr_map for this old address.
            auto it = std::lower_bound(addr_map.begin(), addr_map.end(), val_addr,
                [](const AddrPair& p, uintptr_t addr) { return p.old_addr < addr; });
            if (it != addr_map.end() && it->old_addr == val_addr) {
                *slot_ptr = reinterpret_cast<void*>(it->new_addr);
            }
        }
    }
}

void MarkSweepOldGen::CompactPage(OldGenPage* page) {
    if (page == nullptr || page->is_oversized) return;
    // Compact marked objects to the start of the payload.
    // Walk all marked objects and memmove them to their compacted positions.
    auto* bitmap = reinterpret_cast<const uint64_t*>(page->MarkBitmap());
    char* payload = page->Payload();
    CHAOS_IL2CPP_SIZE num_slots = page->payload_size / sizeof(void*);
    CHAOS_IL2CPP_SIZE num_words = page->bitmap_bytes / sizeof(uint64_t);

    // Collect all marked objects with their sizes.
    struct MovedObj {
        void* old_addr;
        void* new_addr;
        CHAOS_IL2CPP_SIZE size;
    };
    std::vector<MovedObj> to_move;
    to_move.reserve(1024);

    char* compact_cursor = payload;
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
                    auto* hot = static_cast<const TypeInfoHot*>(type_info_ptr);
                    uint64_t stable_id = hot->stable_id;
                    const auto* layout = registry.Lookup(stable_id);
                    if (layout != nullptr && layout->instance_size > 0) {
                        obj_size = layout->instance_size;
                    }
                }
            }

            to_move.push_back({obj, compact_cursor, obj_size});
            compact_cursor += obj_size;
        }
        slot = (w + 1) * 64;
    }

    // Perform the memmove from last to first (safe for overlapping regions
    // when compacting toward lower addresses).
    for (auto it = to_move.rbegin(); it != to_move.rend(); ++it) {
        if (it->new_addr != it->old_addr) {
            std::memmove(it->new_addr, it->old_addr, it->size);
        }
    }

    // Rebuild free lists from remaining space after compact_cursor.
    CHAOS_IL2CPP_SIZE remaining_bytes = page->payload_size -
        static_cast<CHAOS_IL2CPP_SIZE>(compact_cursor - payload);

    // Clear existing free lists.
    for (int i = 0; i < kOldGenNumSizeClasses; i++) {
        page->free_lists[i] = nullptr;
    }

    // Add remaining space as free blocks, split by size class.
    char* free_cursor = compact_cursor;
    while (remaining_bytes > 0) {
        int sc_idx = SizeClassIndex(remaining_bytes);
        if (sc_idx < 0) sc_idx = kOldGenNumSizeClasses - 1;
        CHAOS_IL2CPP_SIZE sc_size = kOldGenSizeClasses[sc_idx];
        if (sc_size > remaining_bytes) {
            sc_size = sizeof(void*);
            sc_idx = SizeClassIndex(sc_size);
            if (sc_idx < 0) break;
        }
        auto* block = reinterpret_cast<OldGenFreeBlock*>(free_cursor);
        block->next = page->free_lists[sc_idx];
        page->free_lists[sc_idx] = block;
        free_cursor += sc_size;
        remaining_bytes -= sc_size;
    }

    CHAOS_IL2CPP_LOG_DEBUG_M("OldGen", "compacted_page objects={0} free_after={1}",
        static_cast<unsigned long long>(to_move.size()),
        static_cast<unsigned long long>(remaining_bytes));
}

void MarkSweepOldGen::Collect(void (*root_callback)(void* obj, void* user_data), void* user_data) {
    CHAOS_IL2CPP_PROFILE_SCOPE("OldGen::Collect");

    auto pause_start = std::chrono::steady_clock::now();

    CHAOS_IL2CPP_LOG_INFO_M("OldGen", "collect_start page_count={0}", page_count_);

    // Fire GC_START event.
    GcFireEvent(GcEvent::GC_START);

    // V4-H3: Snapshot pinned_roots_ under mutex to avoid data race with
    // AddPinnedRoot (which pushes under the same mutex).  Iterating the
    // vector without locking is UB if a concurrent push_back triggers
    // reallocation — the iterator becomes dangling.
    std::vector<PinnedRoot> pinned_snapshot;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pinned_snapshot = pinned_roots_;
    }

#if defined(CHAOS_IL2CPP_DEBUG)
    // Sanity: after snapshot, no other thread holds mutex_ for writes.
    // The snapshot is our consistent view for the entire collection.
    CHAOS_IL2CPP_LOG_DEBUG_M("OldGen", "collect pinned_roots_snapshot=%zu", pinned_snapshot.size());
#endif

    // Phase 1: Mark roots.
    bool has_roots = (root_callback != nullptr) || !pinned_snapshot.empty();

    if (root_callback != nullptr) {
        root_callback(nullptr, user_data);  // signal start (optional)
    }

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

        // EnumerateThreads takes a C function pointer — use a static
        // helper since g_old_gen is a process-wide global.
        threading::EnumerateThreads(
            [](threading::ManagedThread* thread) -> bool {
                if (thread->nursery_ctx == nullptr) return true;
                auto* nursery = thread->nursery_ctx->nursery;
                if (nursery == nullptr) return true;

                // V4-M3: Snapshot nursery range under safepoint, then use it.
                // The thread whose nursery we're scanning is paused at
                // SafepointPoll, so TeardownTlsNursery cannot run concurrently.
                // But capture begin/cur to a local struct anyway so we don't
                // read nursery fields after a potential context switch.
                struct { void* begin; void* cur; } snap = {
                    nursery->begin,
                    nursery->current
                };
                if (snap.cur > snap.begin) {
                    g_old_gen.ScanRangeForRoots(snap.begin, snap.cur);
                }
                return true;  // continue enumeration
            });

        if (mark_stack_.size() > before_roots) {
            has_roots = true;
        }
        CHAOS_IL2CPP_LOG_DEBUG_M("OldGen", "collect_scanned_nurseries");
    }

    // Scan ALL thread stacks as conservative roots.  This catches old-gen
    // object references that live only in thread-local variables (stack slots)
    // and are NOT in any TLS nursery — without this, those objects would be
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
        CHAOS_IL2CPP_LOG_DEBUG_M("OldGen", "collect_scanned_thread_stacks");
    }

    // Phase 2: Mark transitive closure.

    // Mark LOH roots (in pinned roots and thread stacks) when LOH is active.
    // The strategy is simple: after old-gen root marking, any LOH-reachable
    // objects found in roots get their LOH mark bit set.  LOH objects are not
    // transitively scanned (LOH objects are large and rarely point to other
    // LOH objects — memcpy of 85 KB+ is not worth the fragmentation savings).
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
        // Snapshot page list into array for parallel mark.
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
        // Main thread participates alongside thread pool workers.
        // Cap workers to min(hardware_concurrency, total_pages) to avoid
        // oversubscribing high-core-count machines with few pages to sweep.
        std::atomic<int> next_page{0};
        std::atomic<CHAOS_IL2CPP_SIZE> parallel_reclaimed{0};
        std::atomic<int> workers_done{0};
        int max_workers = std::min(static_cast<int>(std::thread::hardware_concurrency()), total_pages);
        if (max_workers < 2) max_workers = 2;

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
        for (int i = 0; i < max_workers - 1; i++) {
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
                // Invalidate last_alloc_page_ entries that point to this page.
                for (int i = 0; i < kOldGenNumSizeClasses; i++) {
                    if (last_alloc_page_[i] == p) {
                        last_alloc_page_[i] = nullptr;
                    }
                }
                FreePage(p);
                page_count_--;
            } else {
                pp = &p->next;
            }
        }
    }

    // Fire SWEEP_DONE event.
    GcFireEvent(GcEvent::SWEEP_DONE);

    // Sweep the Large Object Heap.
    CHAOS_IL2CPP_SIZE loh_reclaimed = 0;
    if (g_loh.SegmentCount() > 0) {
        loh_reclaimed = g_loh.Sweep();
        if (loh_reclaimed > 0) {
            CHAOS_IL2CPP_LOG_DEBUG_M("OldGen", "loh_sweep reclaimed={0}",
                static_cast<unsigned long long>(loh_reclaimed));
        }
    }
    total_reclaimed += loh_reclaimed;

    // Phase 4b: Compaction (when fragmentation exceeds threshold).
    CompactMode compact_mode = DecideCompactMode();
    if (compact_mode == CompactMode::COMPACT) {
        CHAOS_IL2CPP_LOG_INFO_M("OldGen", "compact_mode_enabled");
        CHAOS_IL2CPP_SIZE total_saved = 0;
        std::lock_guard<std::mutex> lock(mutex_);
        auto* p = page_list_;
        while (p != nullptr) {
            if (p->in_use.load(std::memory_order_acquire) && !p->is_oversized) {
                float frag = PageFragmentation(p);
                if (frag > 0.30f) {
                    // Plan → Relocate → Compact for this page.
                    CHAOS_IL2CPP_SIZE saved = PlanPageCompaction(p, nullptr);
                    RelocatePage(p);
                    CompactPage(p);
                    total_saved += saved;
                }
            }
            p = p->next;
        }
        CHAOS_IL2CPP_LOG_INFO_M("OldGen", "compact_done saved_bytes={0}",
            static_cast<unsigned long long>(total_saved));
        GcFireEvent(GcEvent::COMPACT_DONE);
    }

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

    // Fire GC_END event.
    GcFireEvent(GcEvent::GC_END);
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
// Using thread_local for O(1) lookup — replaces the old lock-protected vector
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
    CHAOS_IL2CPP_LOG_DEBUG_M("OldGen", "add_pinned_root {0} size={1}", addr,
        static_cast<unsigned long long>(size));
}

// ── Root scanning ─────────────────────────────────────────

bool MarkSweepOldGen::TryMarkRoot(void* addr) {
    // addr comes from GcScanAllThreadRoots: it is the address of a stack slot.
    // Read the VALUE at that slot — if it points to old-gen, mark it.
    if (addr == nullptr) return false;
    auto val = *reinterpret_cast<void**>(addr);
    if (val == nullptr) return false;
    auto* page = FindPage(val);
    if (page != nullptr && page->in_use.load(std::memory_order_acquire)) {
        if (MarkObject(val)) {
            mark_stack_.push_back(val);
            return true;
        }
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

    CHAOS_IL2CPP_SIZE ran = 0;
    for (auto& entry : to_run) {
        if (entry.finalizer == nullptr) continue;

        // Check if the object is still reachable (marked in bitmap).
        // RunFinalizers is called after DrainMarkStack, so all reachable
        // objects have their mark-bit set.  Skip the finalizer for any
        // object that is still marked — it's still alive.
        bool unreachable = true;

        // Check if object is in old-gen page.
        auto* page = FindPage(entry.obj);
        if (page != nullptr && !page->is_oversized) {
            uintptr_t obj_addr = reinterpret_cast<uintptr_t>(entry.obj);
            uintptr_t payload_start = reinterpret_cast<uintptr_t>(page->Payload());
            if (obj_addr >= payload_start) {
                CHAOS_IL2CPP_SIZE offset = obj_addr - payload_start;
                CHAOS_IL2CPP_SIZE slot_idx = offset / sizeof(void*);
                CHAOS_IL2CPP_SIZE byte_idx = slot_idx / 8;
                int bit_idx = static_cast<int>(slot_idx % 8);
                auto* bitmap = page->MarkBitmap();
                if (bitmap[byte_idx] & (static_cast<unsigned char>(1u << bit_idx))) {
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
            CHAOS_IL2CPP_LOG_DEBUG_M("OldGen", "run_finalizer obj={0}", entry.obj);
            entry.finalizer(entry.obj);
            ran++;
        } else {
            // Object is still reachable — re-register the finalizer for
            // the next GC cycle.
            RegisterFinalizer(entry.obj, entry.finalizer);
        }
    }

    return ran;
}

}  // namespace chaos::il2cpp::runtime_core