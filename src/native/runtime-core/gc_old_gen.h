#ifndef CHAOS_IL2CPP_GC_OLD_GEN_H_
#define CHAOS_IL2CPP_GC_OLD_GEN_H_

#include <chaos/native_types.h>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <mutex>
#include <vector>

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// MarkSweepOldGen — precise mark-sweep collector for tenured generation
//
// Replaces BDWGC for all old-gen allocations.  Uses page-based virtual
// memory with inline metadata (free lists, mark bitmaps).
//
// == Size classes ==
// Matches the GcBumpCache 20 size classes so promoted objects fit existing
// size-class expectations: 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128,
// 160, 192, 224, 256, 320, 384, 512, 1024, 2048.
//
// == Page layout ==
// [PageHeader | free_list_ptrs (20*8=160B) | mark_bitmap (variable) | payload]
//
// == Mark-sweep ==
// Tri-color (white/grey/black) marking via explicit mark stack.
// Sweep: linear scan per page, coalesce unmarked blocks, return to free lists.
// ======================================================================

static constexpr CHAOS_IL2CPP_SIZE kOldGenPageSize = 64 * 1024;        // 64 KB pages
static constexpr CHAOS_IL2CPP_SIZE kOldGenPagePayload = kOldGenPageSize - 256;  // reserve for header

// Size classes (same as GcBumpCache for compatibility).
static constexpr CHAOS_IL2CPP_SIZE kOldGenSizeClasses[] = {
    16, 24, 32, 40, 48, 56, 64, 80, 96, 112,
    128, 160, 192, 224, 256, 320, 384, 512, 1024, 2048
};
static constexpr int kOldGenNumSizeClasses = 20;

// Maximum inline allocation before going oversized (>32 KB → direct VirtualAlloc).
static constexpr CHAOS_IL2CPP_SIZE kOldGenMaxInline = 32 * 1024;

// Per-page free-list entry (embedded in free memory, no separate allocation).
struct OldGenFreeBlock {
    OldGenFreeBlock* next;
};

// Page descriptor stored at the start of each virtual allocation.
struct OldGenPage {
    OldGenPage*     next;             // link in page list
    CHAOS_IL2CPP_SIZE page_size;      // total page size (including header)
    CHAOS_IL2CPP_SIZE payload_size;   // usable payload bytes
    CHAOS_IL2CPP_SIZE bitmap_bytes;   // mark bitmap size in bytes (aligned)
    bool            scanning;         // true = scanned (contains pointers)
    bool            is_oversized;     // true = single-object oversized page
    std::atomic<bool> in_use;         // true = actively used for allocation

    // Free-list heads (one per size class, indexed by kOldGenSizeClasses).
    OldGenFreeBlock* free_lists[kOldGenNumSizeClasses];

    // Mark bitmap follows immediately after the header at offset sizeof(OldGenPage).
    unsigned char* MarkBitmap() {
        return reinterpret_cast<unsigned char*>(this) + sizeof(OldGenPage);
    }

    // Payload starts after header + bitmap at offset sizeof(OldGenPage) + bitmap_bytes.
    char* Payload() {
        return reinterpret_cast<char*>(this) + sizeof(OldGenPage) + bitmap_bytes;
    }
    const char* Payload() const {
        return reinterpret_cast<const char*>(this) + sizeof(OldGenPage) + bitmap_bytes;
    }
};

// Mark-sweep collector state.
class MarkSweepOldGen {
public:
    MarkSweepOldGen() = default;
    ~MarkSweepOldGen();

    MarkSweepOldGen(const MarkSweepOldGen&) = delete;
    MarkSweepOldGen& operator=(const MarkSweepOldGen&) = delete;

    // ── Initialization ───────────────────────────────────────────

    /// Initialize the old generation with initial address hint.
    /// @param heap_hint  Preferred base address (0 = pick automatically).
    /// @param initial_pages  Number of pages to pre-allocate (0 = on-demand).
    bool Init(uintptr_t heap_hint = 0, int initial_pages = 0);

    /// Returns the base address of the first allocated page.
    uintptr_t HeapBase() const { return heap_base_; }

    // ── Allocation / deallocation ───────────────────────────────

    /// Allocate @a size bytes in the old generation.
    /// @param scanning_required  true if the GC must scan this memory for pointers.
    void* Allocate(CHAOS_IL2CPP_SIZE size, bool scanning_required = true);

    /// Free a pointer previously allocated via Allocate().
    void Free(void* ptr);

    /// Reallocate a pointer to a new size (preserves min(old_size, new_size) bytes).
    void* Reallocate(void* ptr, CHAOS_IL2CPP_SIZE new_size);

    // ── GC collection ────────────────────────────────────────────

    /// Perform a full mark-sweep collection.
    /// @param root_callback  Called for each root (object pointer) to mark.
    /// @param user_data      Opaque user data for root_callback.
    void Collect(void (*root_callback)(void* obj, void* user_data), void* user_data);

    /// Perform a small incremental GC slice.
    /// Returns true if a full collection was completed.
    bool CollectIncremental();

    // ── Pinned root support ─────────────────────────────────────

    /// Register a pinned root (object that must never be moved/collected).
    void AddPinnedRoot(void* addr, CHAOS_IL2CPP_SIZE size);

    // ── Root scanning helpers ────────────────────────────────────

    /// Scan a memory range [begin, end) for raw pointers into old-gen pages
    /// and mark any found objects as reachable roots for the next collection.
    /// Runs DrainMarkStack() after scanning.
    void ScanRangeForRoots(void* range_begin, void* range_end);

    // ── Thread stack registration (replaces GC_register_my_thread) ──

    /// Register the current thread's stack bounds for root scanning.
    void RegisterThreadStack(void* stack_base, void* stack_limit);

    /// Unregister the current thread's stack.
    void UnregisterThreadStack();

    // ── Finalizer support ────────────────────────────────────────

    /// Register a finalizer callback for @a obj.
    void RegisterFinalizer(void* obj, void (*finalizer)(void*));

    /// Run all pending finalizers for unreachable objects.
    /// Returns the number of finalizers run.
    CHAOS_IL2CPP_SIZE RunFinalizers();

    // ── Diagnostics ─────────────────────────────────────────────

    CHAOS_IL2CPP_SIZE TotalAllocated() const {
        return total_allocated_.load(std::memory_order_relaxed);
    }
    CHAOS_IL2CPP_SIZE TotalPages() const { return page_count_; }

private:
    // ── Page management ─────────────────────────────────────────

    /// Allocate a new page from the OS.
    OldGenPage* AllocatePage(CHAOS_IL2CPP_SIZE size, bool scanning);

    /// Return a page to the OS.
    void FreePage(OldGenPage* page);

    /// Find the page containing @a ptr, or nullptr.
    OldGenPage* FindPage(const void* ptr);

    // ── Allocation helpers ──────────────────────────────────────

    /// Round up to the nearest size class. Returns size class index or -1.
    int SizeClassIndex(CHAOS_IL2CPP_SIZE size) const;

    /// Try to allocate from existing page free lists. Returns nullptr on miss.
    void* TryAllocateFromFreeLists(CHAOS_IL2CPP_SIZE size, int sc_idx);

    // ── GC helpers ──────────────────────────────────────────────

    /// Mark an object as reachable. Returns false if already marked.
    bool MarkObject(void* obj);

    /// Mark all objects on the mark stack (transitive closure).
    void DrainMarkStack();

    /// Sweep a single page: reclaim unmarked blocks.
    CHAOS_IL2CPP_SIZE SweepPage(OldGenPage* page);

    /// Coalesce adjacent free blocks on a page.
    void CoalescePage(OldGenPage* page);

    // ── State ───────────────────────────────────────────────────

    uintptr_t heap_base_ = 0;

    OldGenPage* page_list_ = nullptr;   // singly-linked list of all pages
    int         page_count_ = 0;

    std::mutex mutex_;                  // protects page list + free lists
    std::atomic<CHAOS_IL2CPP_SIZE> total_allocated_{0};

    // Marked-object counter (reset each cycle, used by GcStats).
    std::atomic<uint64_t> marked_count_{0};

    // Mark stack for tri-color marking (vector = stack).
    std::vector<void*> mark_stack_;

    // Pinned roots.
    struct PinnedRoot {
        void* addr;
        CHAOS_IL2CPP_SIZE size;
    };
    std::vector<PinnedRoot> pinned_roots_;

    // Finalizer table: maps object → finalizer callback.
    struct FinalizerEntry {
        void* obj;
        void (*finalizer)(void*);
    };
    std::vector<FinalizerEntry> finalizers_;
};

// Global old-generation instance.  Defined in gc_old_gen.cpp.
extern MarkSweepOldGen g_old_gen;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_OLD_GEN_H_