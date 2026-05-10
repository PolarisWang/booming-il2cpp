#ifndef CHAOS_IL2CPP_GC_BUMP_CACHE_H_
#define CHAOS_IL2CPP_GC_BUMP_CACHE_H_

#include <chaos/native_types.h>

#include <cstddef>
#include <new>

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// GcBumpCache — thread-local size-class + bump arena for GC allocations.
//
// Phase B of the A→B→C allocation roadmap.  Builds on the Phase A bump
// arena with per-size-class thread-local free lists.  Each thread caches
// a small batch of commonly requested sizes; popping from the free list
// is cheaper than bumping and reduces fragmentation within the arena.
//
// == Design ==
// - 20 size classes covering 16 → 2048 bytes (typical object sizes)
// - Each size class has a small thread-local free list (pop = O(1))
// - When a free list is empty, a new batch is carved from the bump arena
// - Sizes that don't match a size class fall through to the bump arena
// - Oversized (> 32 KB) bypasses the arena entirely → direct GC_MALLOC
// - All pages are GC-allocated; the GC reclaims them on collection.
//   No explicit free is needed (conservative BDWGC).
//
// == Thread-safety ==
// NOT safe for cross-thread use — one instance per thread.
// ======================================================================
class GcBumpCache {
public:
    static constexpr CHAOS_IL2CPP_SIZE kPageSize = 64 * 1024;
    static constexpr CHAOS_IL2CPP_SIZE kMaxInlineSize = kPageSize / 2;  // 32 KB
    static constexpr int kBatchCount = 8;  // Blocks per batch from arena

    GcBumpCache() noexcept = default;
    GcBumpCache(const GcBumpCache&) = delete;
    GcBumpCache& operator=(const GcBumpCache&) = delete;

    ~GcBumpCache() noexcept {
        // GC-managed; no explicit teardown needed.
        scan_page_ = nullptr;
        atomic_page_ = nullptr;
    }

    void* Allocate(CHAOS_IL2CPP_SIZE size) {
        return AllocateImpl(size, /*atomic*/ false);
    }

    void* AllocateAtomic(CHAOS_IL2CPP_SIZE size) {
        return AllocateImpl(size, /*atomic*/ true);
    }

private:
    // ── Size class configuration ──────────────────────────────
    // Covers typical .NET managed object sizes (header + fields).
    // Each entry: { size, batch_bytes }
    static constexpr CHAOS_IL2CPP_SIZE kSizeClasses[] = {
        16, 24, 32, 40, 48, 56, 64, 80, 96, 112,
        128, 160, 192, 224, 256, 320, 384, 512, 1024, 2048
    };
    static constexpr int kNumSizeClasses = 20;

    // Free-list node embedded in free blocks (singly-linked).
    struct FreeNode {
        FreeNode* next;
    };

    // Per-size-class state.
    struct SizeClassState {
        FreeNode* free_list;   // Thread-local free list head
        int       count;       // Items currently in free_list
    };

    // ── Bump arena page header ────────────────────────────────
    struct Page {
        Page* next;
        CHAOS_IL2CPP_SIZE offset;
    };

    // ── Helpers ───────────────────────────────────────────────
    static CHAOS_IL2CPP_SIZE AlignUp(CHAOS_IL2CPP_SIZE size) {
        return (size + 7) & ~static_cast<CHAOS_IL2CPP_SIZE>(7);
    }

    /// Return the size-class index for a given aligned size, or -1.
    static int SizeClassIndex(CHAOS_IL2CPP_SIZE aligned_size) {
        for (int i = 0; i < kNumSizeClasses; i++) {
            if (kSizeClasses[i] == aligned_size) {
                return i;
            }
        }
        return -1;
    }

    void* AllocateImpl(CHAOS_IL2CPP_SIZE size, bool atomic) {
        size = AlignUp(size);

        if (size > kMaxInlineSize) {
            return atomic ? GC_MALLOC_ATOMIC(size) : GC_MALLOC(size);
        }

        // Fast path: size-class hit → pop from free list.
        int idx = SizeClassIndex(size);
        if (idx >= 0) {
            SizeClassState& sc = (atomic ? atomic_sc_ : scan_sc_)[idx];
            if (sc.free_list != nullptr) {
                void* ptr = sc.free_list;
                sc.free_list = sc.free_list->next;
                sc.count--;
                return ptr;
            }
            // Miss → refill from bump arena.
            RefillSizeClass(idx, atomic);
            if (sc.free_list != nullptr) {
                void* ptr = sc.free_list;
                sc.free_list = sc.free_list->next;
                sc.count--;
                return ptr;
            }
        }

        // Fallback: bump-allocate directly.
        return BumpAllocate(size, atomic);
    }

    /// Carve a batch of same-size blocks from the bump arena.
    void RefillSizeClass(int idx, bool atomic) {
        CHAOS_IL2CPP_SIZE block_size = kSizeClasses[idx];
        CHAOS_IL2CPP_SIZE batch_bytes = block_size * kBatchCount;

        void* chunk = BumpAllocate(batch_bytes, atomic);
        if (chunk == nullptr) return;

        SizeClassState& sc = (atomic ? atomic_sc_ : scan_sc_)[idx];

        // Chain the batch into a free list.
        char* cursor = static_cast<char*>(chunk);
        for (int i = 0; i < kBatchCount; i++) {
            auto* node = reinterpret_cast<FreeNode*>(cursor);
            node->next = sc.free_list;
            sc.free_list = node;
            sc.count++;
            cursor += block_size;
        }
    }

    /// Bump-allocate from the current arena page (Phase A path).
    void* BumpAllocate(CHAOS_IL2CPP_SIZE size, bool atomic) {
        Page*& page = atomic ? atomic_page_ : scan_page_;
        if (page == nullptr || page->offset + size > kPageSize) {
            page = AllocatePage(atomic);
        }
        void* ptr = static_cast<char*>(static_cast<void*>(page + 1)) + page->offset;
        page->offset += size;
        return ptr;
    }

    Page* AllocatePage(bool atomic) {
        void* raw = atomic
            ? GC_MALLOC_ATOMIC(sizeof(Page) + kPageSize)
            : GC_MALLOC(sizeof(Page) + kPageSize);

        auto* page = static_cast<Page*>(raw);
        page->next = atomic ? atomic_page_ : scan_page_;
        page->offset = 0;
        CHAOS_IL2CPP_MEMSET(static_cast<char*>(raw) + sizeof(Page), 0, kPageSize);
        return page;
    }

    // ── Per-thread state ──────────────────────────────────────
    SizeClassState scan_sc_[kNumSizeClasses] = {};
    SizeClassState atomic_sc_[kNumSizeClasses] = {};
    Page* scan_page_ = nullptr;
    Page* atomic_page_ = nullptr;
};

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_BUMP_CACHE_H_