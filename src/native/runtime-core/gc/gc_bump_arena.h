#ifndef CHAOS_IL2CPP_GC_BUMP_ARENA_H_
#define CHAOS_IL2CPP_GC_BUMP_ARENA_H_

// ╔══════════════════════════════════════════════════════════════════════╗
// ║ LEGACY — superseded by GcBumpCache (gc_bump_cache.h).              ║
// ║                                                                    ║
// ║ Phase A of the A→B→C allocation roadmap.  GcBumpCache (Phase B)   ║
// ║ wraps a bump arena with per-size-class TLS free lists and is the   ║
// ║ active GC allocation path as of 2026-05.  No native code includes  ║
// ║ this header — retained only as a reference.                        ║
// ╚══════════════════════════════════════════════════════════════════════╝

#include <chaos/native_types.h>

#include <cstddef>
#include <new>

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// GcBumpArena — thread-local bump-pointer arena for GC allocations.
//
// Phase A of the A→B→C allocation roadmap.  Each thread gets a 64 KB
// bump arena; small-to-medium allocations just advance a pointer (no
// malloc call).  When the arena page is exhausted, a new page is
// allocated via GC_MALLOC / GC_MALLOC_ATOMIC.
//
// All pages are themselves GC-allocated so the collector can still
// trace through them (conservative scanning).  No individual free is
// needed — pages are reclaimed by the GC when the thread dies or
// when the collector runs a full cycle.
//
// Oversized allocations (> kMaxInlineSize) bypass the arena entirely
// and delegate directly to the underlying GC allocator.
//
// Usage:
//   static thread_local GcBumpArena s_arena;
//   void* ptr = s_arena.Allocate(size);
//   void* ptr2 = s_arena.AllocateAtomic(size);  // pointer-free variant
//
// Thread-safety: NOT safe for cross-thread use (per-thread arena).
// ======================================================================
class GcBumpArena {
public:
    /// Default page size for each bump arena chunk.
    static constexpr CHAOS_IL2CPP_SIZE kPageSize = 64 * 1024;       // 64 KB

    /// Allocations larger than this threshold bypass the arena and go
    /// directly to GC_MALLOC / GC_MALLOC_ATOMIC.
    static constexpr CHAOS_IL2CPP_SIZE kMaxInlineSize = kPageSize / 2;  // 32 KB

    GcBumpArena() noexcept = default;

    GcBumpArena(const GcBumpArena&) = delete;
    GcBumpArena& operator=(const GcBumpArena&) = delete;

    ~GcBumpArena() noexcept {
        // Pages are GC-managed — no explicit free needed.
        // The GC will reclaim them when the thread dies or on the
        // next full collection cycle.  We simply reset the linked list.
        scan_page_ = nullptr;
        atomic_page_ = nullptr;
    }

    /// Allocate @a size bytes of GC-scanned memory (objects with pointers).
    /// Returns zeroed memory.
    void* Allocate(CHAOS_IL2CPP_SIZE size) {
        return AllocateImpl(size, /*atomic*/ false);
    }

    /// Allocate @a size bytes of GC-UNS-scanned memory (pointer-free data,
    /// e.g. string UTF-8 bytes).  Returns zeroed memory.
    void* AllocateAtomic(CHAOS_IL2CPP_SIZE size) {
        return AllocateImpl(size, /*atomic*/ true);
    }

private:
    // Per-page header embedded at the start of each GC-allocated chunk.
    struct Page {
        Page* next;          ///< Link to previous page (single-linked list).
        CHAOS_IL2CPP_SIZE offset;  ///< Current bump offset within this page.
        // Data follows immediately after the header.
    };

    static CHAOS_IL2CPP_SIZE AlignUp(CHAOS_IL2CPP_SIZE size) {
        // Align to 8 bytes (guarantees natural alignment for all
        // heap object headers and pointer fields).
        return (size + 7) & ~static_cast<CHAOS_IL2CPP_SIZE>(7);
    }

    void* AllocateImpl(CHAOS_IL2CPP_SIZE size, bool atomic) {
        size = AlignUp(size);

        if (size > kMaxInlineSize) {
            // Oversized: bypass arena, allocate directly.
            return atomic ? GC_MALLOC_ATOMIC(size) : GC_MALLOC(size);
        }

        Page*& page = atomic ? atomic_page_ : scan_page_;
        if (page == nullptr || page->offset + size > kPageSize) {
            page = AllocatePage(atomic);
        }

        // Location = page header + sizeof(Page) + page->offset
        // The page data starts at (page + 1), which is 8-byte aligned
        // because Page has a pointer (8B) + size_t (8B) = 16B.
        void* ptr = static_cast<char*>(static_cast<void*>(page + 1)) + page->offset;
        page->offset += size;
        return ptr;
    }

    Page* AllocatePage(bool atomic) {
        // Allocate one contiguous block: Page header + kPageSize of payload.
        // For the scanned arena we use GC_MALLOC (scanned), for the
        // atomic arena we use GC_MALLOC_ATOMIC (not scanned).
        void* raw = atomic
            ? GC_MALLOC_ATOMIC(sizeof(Page) + kPageSize)
            : GC_MALLOC(sizeof(Page) + kPageSize);

        auto* page = static_cast<Page*>(raw);
        page->next = atomic ? atomic_page_ : scan_page_;
        page->offset = 0;

        // Zero the payload area (GC_MALLOC already zeroes, but be explicit
        // about the page data region for clarity).
        CHAOS_IL2CPP_MEMSET(static_cast<char*>(raw) + sizeof(Page), 0, kPageSize);

        return page;
    }

    // Two independent page chains: one for scanned (pointer-bearing)
    // allocations, one for atomic (pointer-free) allocations.
    Page* scan_page_ = nullptr;
    Page* atomic_page_ = nullptr;
};

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_BUMP_ARENA_H_