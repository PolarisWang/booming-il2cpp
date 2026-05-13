#ifndef CHAOS_IL2CPP_GC_BUMP_CACHE_H_
#define CHAOS_IL2CPP_GC_BUMP_CACHE_H_

#include <chaos/native_types.h>
#include <chaos/profile.h>
#include <chaos/log.h>

#include <cstddef>
#include <cstdio>
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

    /// Dump allocation counters to stderr (in PROFILE format).
    /// Call after a benchmark loop or at thread exit.
    void DumpCounters() const {
        uint64_t total_hits = 0, total_misses = 0, total_bytes = 0;
        for (int i = 0; i < kNumSizeClasses; i++) {
            auto add = [&](const SizeClassState& sc) {
                if (sc.hits == 0 && sc.misses == 0) return;
                std::fprintf(stderr, "ALLOC|size_class=%zu|hits=%llu|misses=%llu|bytes=%llu\n",
                    static_cast<unsigned long long>(kSizeClasses[i]),
                    static_cast<unsigned long long>(sc.hits),
                    static_cast<unsigned long long>(sc.misses),
                    static_cast<unsigned long long>(sc.alloc_bytes));
                total_hits += sc.hits;
                total_misses += sc.misses;
                total_bytes += sc.alloc_bytes;
            };
            add(scan_sc_[i]);
            add(atomic_sc_[i]);
        }
        std::fprintf(stderr, "ALLOC|oversized|count=%llu|bytes=%llu\n",
            static_cast<unsigned long long>(oversized_count_),
            static_cast<unsigned long long>(oversized_bytes_));
        std::fprintf(stderr, "ALLOC|total|hits=%llu|misses=%llu|bytes=%llu|hit_rate=%.2f%%\n",
            static_cast<unsigned long long>(total_hits),
            static_cast<unsigned long long>(total_misses),
            static_cast<unsigned long long>(total_bytes),
            total_hits + total_misses > 0
                ? 100.0 * static_cast<double>(total_hits) / (total_hits + total_misses)
                : 0.0);
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
        uint64_t  hits{};      // Cache hit count
        uint64_t  misses{};    // Cache miss (refill) count
        uint64_t  alloc_bytes{}; // Total bytes allocated via this class
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
    /// Uses constexpr binary search — O(log 20) ≈ 5 comparisons vs 20 for
    /// linear scan, with negligible real-world impact but zero cost to maintain.
    static int SizeClassIndex(CHAOS_IL2CPP_SIZE aligned_size) noexcept {
        static constexpr auto first = kSizeClasses[0];
        static constexpr auto last = kSizeClasses[kNumSizeClasses - 1];
        if (aligned_size < first || aligned_size > last) return -1;

        int lo = 0, hi = kNumSizeClasses - 1;
        while (lo <= hi) {
            int mid = lo + (hi - lo) / 2;
            auto v = kSizeClasses[mid];
            if (v == aligned_size) return mid;
            if (v < aligned_size) lo = mid + 1;
            else                  hi = mid - 1;
        }
        return -1;
    }

    void* AllocateImpl(CHAOS_IL2CPP_SIZE size, bool atomic) {
        CHAOS_IL2CPP_PROFILE_SCOPE("GcAllocateImpl");
        size = AlignUp(size);

        if (size > kMaxInlineSize) {
            oversized_count_++;
            oversized_bytes_ += size;
            return atomic ? GC_MALLOC_ATOMIC(size) : GC_MALLOC(size);
        }

        // Fast path: size-class hit → pop from free list.
        int idx = SizeClassIndex(size);
        if (idx >= 0) {
            SizeClassState& sc = (atomic ? atomic_sc_ : scan_sc_)[idx];
            sc.alloc_bytes += size;
            if (sc.free_list != nullptr) {
                void* ptr = sc.free_list;
                sc.free_list = sc.free_list->next;
                sc.count--;
                sc.hits++;
                return ptr;
            }
            // Miss → refill from bump arena.
            sc.misses++;
            RefillSizeClass(idx, atomic);
            if (sc.free_list != nullptr) {
                void* ptr = sc.free_list;
                sc.free_list = sc.free_list->next;
                sc.count--;
                return ptr;
            }
        }

        // Fallback: bump-allocate directly (size not in any size class).
        CHAOS_IL2CPP_LOG_DEBUG("GcBumpCache", "size_class_miss");
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

    // ── Oversized allocation counters ─────────────────────────
    uint64_t oversized_count_ = 0;
    uint64_t oversized_bytes_ = 0;
};

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_BUMP_CACHE_H_