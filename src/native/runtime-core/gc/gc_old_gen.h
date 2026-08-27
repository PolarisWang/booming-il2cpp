#ifndef CHAOS_IL2CPP_GC_OLD_GEN_H_
#define CHAOS_IL2CPP_GC_OLD_GEN_H_

#include <chaos/native_types.h>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <mutex>
#include <vector>

#include "gc_card_table.h"
#include "gc_layout.h"
#include "gc_mark_bitmap.h"

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
    128, 160, 192, 224, 256, 320, 384, 512, 1024, 2048,
    3072, 4096, 6144, 8192, 12288, 16384, 24576, 32768
};
static constexpr int kOldGenNumSizeClasses = 28;

// Maximum inline allocation before going oversized (>32 KB → direct VirtualAlloc).
static constexpr CHAOS_IL2CPP_SIZE kOldGenMaxInline = 32 * 1024;

// Per-page free-list entry (embedded in free memory, no separate allocation).
// Layout: [sentinel:8][next:8] = 16 bytes total.
// sentinel stores a TypeInfoHot* from the sentinel registry — MarkObject
// reads this to distinguish free blocks from live objects.  next chains
// blocks in the per-size-class free list.
struct OldGenFreeBlock {
    const void*     sentinel;  // [0] GC sentinel TypeInfo pointer (marks free block)
    OldGenFreeBlock* next;     // [8] free list linkage
};

// Page descriptor stored at the start of each virtual allocation.
struct OldGenPage {
    OldGenPage*     next;             // link in page list
    CHAOS_IL2CPP_SIZE page_size;      // total page size (including header)
    CHAOS_IL2CPP_SIZE payload_size;   // usable payload bytes
    CHAOS_IL2CPP_SIZE bitmap_bytes;   // mark bitmap size in bytes (aligned)
    bool            scanning;         // true = scanned (contains pointers)
    bool            is_oversized;     // true = single-object oversized page
    int8_t          preferred_sc_idx; // size class index for most blocks on this page (-1 = mixed)
    int8_t          numa_node;         // NUMA node this page was allocated on (-1 = unknown)
    bool            young_tenured;     // true = recent survivor promotion (BGC sweep priority)
    std::atomic<bool> in_use;         // true = actively used for allocation
    std::atomic<bool> sweep_lock{false}; // spinlock for concurrent sweep

    // Free-list heads (one per size class, indexed by kOldGenSizeClasses).
    OldGenFreeBlock* free_lists[kOldGenNumSizeClasses];

    // ── In-place demotion (CoreCLR-aligned, GC-N6 #10) ──
    // A "demoted" object is a live gen1-owned object that stays resident in this
    // OLD-GEN page (not copied to the gen1 region).  It keeps its old-gen page
    // mark-bit SET (so sweep/BGC preserve it) while being tracked here so the
    // gen1 collection can scan it, the full GC can root it, and GetRegionGen can
    // classify it as gen1.  This replaces the fragile physically-moving demotion
    // whose DemotionRelocate could leave a stale ref → SEGFAULT.
    // Fixed inline array (worst-case a fragmented 64KB page holds at most a few
    // dozen live objects) — allocation-free, no lifetime/lock-free-array hazards.
    struct DemotedObj {
        char* addr;                // object start within Payload()
        CHAOS_IL2CPP_SIZE size;    // instance size (bytes)
        bool  must_promote;        // survived >=1 gen1 collection (age)
    };
    static constexpr int kMaxDemotedPerPage = 128;
    DemotedObj demoted[kMaxDemotedPerPage];
    std::atomic<int32_t> demoted_count{0};

    // Mark bitmap follows immediately after the header at offset sizeof(OldGenPage).
    unsigned char* MarkBitmap() {
        return reinterpret_cast<unsigned char*>(this) + sizeof(OldGenPage);
    }

    const unsigned char* MarkBitmap() const {
        return reinterpret_cast<const unsigned char*>(this) + sizeof(OldGenPage);
    }

    // Payload starts after header + bitmap at offset sizeof(OldGenPage) + bitmap_bytes.
    char* Payload() {
        return reinterpret_cast<char*>(this) + sizeof(OldGenPage) + bitmap_bytes;
    }
    const char* Payload() const {
        return reinterpret_cast<const char*>(this) + sizeof(OldGenPage) + bitmap_bytes;
    }

    /// Given an interior pointer to within this page's payload, locate the
    /// containing object by backward bitmap scan + TypeInfo validation.
    /// Returns nullptr if no valid containing object is found.
    void* FindObjectContaining(const void* interior_ptr) const;

    // ── In-place demotion helpers (async/reused by gen1 collection + full GC) ──
    /// Add a gen1-owned object resident on this page (addr within Payload()).
    /// Entry is fully written BEFORE demoted_count is published so a concurrent
    /// reader that observes the new count sees a valid entry.  Writers are STW.
    /// Returns false if the inline array is full (demotion is best-effort).
    bool DemoteInPlace(char* addr, CHAOS_IL2CPP_SIZE size, bool must_promote = false) {
        int32_t n = demoted_count.load(std::memory_order_acquire);
        if (n >= kMaxDemotedPerPage) return false;
        demoted[n] = {addr, size, must_promote};
        demoted_count.store(n + 1, std::memory_order_release);
        return true;
    }
    /// Remove the gen1-owned object at exactly @a addr (idempotent).  Returns the
    /// removed entry (addr==nullptr if none).  STW.
    DemotedObj DemoteRemove(char* addr) {
        DemotedObj none{nullptr, 0, false};
        int32_t n = demoted_count.load(std::memory_order_acquire);
        for (int32_t i = 0; i < n; i++) {
            if (demoted[i].addr == addr) {
                DemotedObj e = demoted[i];
                demoted[i] = demoted[n - 1];
                demoted_count.store(n - 1, std::memory_order_release);
                return e;
            }
        }
        return none;
    }
    /// Is @a addr (or an interior pointer into) a gen1-owned object on this page?
    /// Concurrent-safe: reads count once, then only valid entries [0,count).
    bool DemotedContains(const void* ptr) const {
        const auto* cp = static_cast<const char*>(ptr);
        int32_t n = demoted_count.load(std::memory_order_acquire);
        for (int32_t i = 0; i < n; i++) {
            const DemotedObj& e = demoted[i];
            if (e.addr != nullptr && cp >= e.addr &&
                cp <  e.addr + static_cast<ptrdiff_t>(e.size)) {
                return true;
            }
        }
        return false;
    }

    /// Normalize a demoted pointer (which may be an interior pointer into the
    /// object) back to the object's base address @c .addr, or nullptr if @a ptr
    /// is not inside any demoted object on this page.  Unlike DemotedContains
    /// (a bool test), this returns the containing object's start so callers can
    /// key by base address — critical because conservative stack scans record
    /// raw slot values that may be interior pointers, and base/interior address
    /// semantics MUST NOT be mixed when tracking liveness (GC-gen1 Phase 4f).
    char* DemotedBase(const void* ptr) const {
        const auto* cp = static_cast<const char*>(ptr);
        int32_t n = demoted_count.load(std::memory_order_acquire);
        for (int32_t i = 0; i < n; i++) {
            const DemotedObj& e = demoted[i];
            if (e.addr != nullptr && cp >= e.addr &&
                cp <  e.addr + static_cast<ptrdiff_t>(e.size)) {
                return e.addr;
            }
        }
        return nullptr;
    }
};

/// Is @a ptr a gen1-owned object physically resident in an old-gen page (i.e. in
/// this mark-sweep old gen's in-place demoted set)?  O(log n) via FindPage +
/// per-page demoted-array scan.  Used to extend gen1-ness / classification for
/// addresses that are not in the gen1 bump region but are demoted-old-gen.  Only
/// meaningful when @a ptr is inside an old-gen page.
bool IsInDemotedSet(const void* ptr);

/// Base-address variant of IsInDemotedSet: for any pointer (base or interior)
/// inside a gen1-owned in-place demoted old-gen object, return that object's
/// base @c addr; nullptr if not demoted.  Conservative scans only yield interior
/// pointers, but liveness bookkeeping must be keyed on the object base — so all
/// demoted tracking should normalize through this instead of storing raw values.
char* IsInDemotedSetGetBase(const void* ptr);

// Finalizer table entry: maps object -> finalizer callback.
struct FinalizerEntry {
    void* obj;
    void (*finalizer)(void*);
};

/// Finalizer entry with timeout tracking for BGC finalizer watchdog.
struct TimedFinalizerEntry {
    void* obj{nullptr};
    void (*finalizer)(void*){nullptr};
    int retry_count{0};
    bool is_dead{false};  ///< true = exceeded max retries, permanently skipped
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

    // ── GC-N8 free-list reuse-rate accounting ─────────────────────
    // Counts allocations served from existing page free-lists (hits) vs fresh
    // page carves (misses).  Sampled & reset at collection time to feed the
    // scheduler's FreeListReuseRate signal (dynamic_tuning Phase-1).
    uint64_t FreeListHits() const noexcept {
        return free_list_hits_.load(std::memory_order_relaxed);
    }
    uint64_t FreeListMisses() const noexcept {
        return free_list_carves_.load(std::memory_order_relaxed);
    }
    void ResetFreeListAccounting() noexcept {
        free_list_hits_.store(0, std::memory_order_relaxed);
        free_list_carves_.store(0, std::memory_order_relaxed);
    }

    /// Free a pointer previously allocated via Allocate().
    void Free(void* ptr);

    /// Reallocate a pointer to a new size (preserves min(old_size, new_size) bytes).
    void* Reallocate(void* ptr, CHAOS_IL2CPP_SIZE new_size);

    // ── GC collection ────────────────────────────────────────────

    /// Perform a full mark-sweep collection.
    /// @param root_callback  Called for each root (object pointer) to mark.
    /// @param user_data      Opaque user data for root_callback.
    void Collect(void (*root_callback)(void* obj, void* user_data), void* user_data);

    /// Perform a full mark-sweep collection (synchronous STW).
    /// At C3 this is a full collect; C3+ will add incremental slices.
    /// Returns true after collection completes.
    bool CollectFull();

    // ── BGC concurrent mark support ─────────────────────────────

    /// Concurrent-safe mark: atomic test-and-set on the mark bitmap.
    /// Returns true if the object was newly marked (first time in this cycle).
    /// Called by the BGC thread during concurrent mark phase.
    bool BgcTryMark(void* obj);

    /// Sweep all pages concurrently (called by BGC thread after re-mark).
    /// Iterates pages one at a time under mutex_, with yields between pages
    /// to avoid starving mutators.
    void BgcSweep();

    /// Compact pages under safepoint after BGC concurrent sweep.
    /// Reuses the mark bitmap left intact by BgcSweep() to decide
    /// fragmentation and plan relocation.  Clears the bitmap after
    /// compaction to prepare for the next BGC cycle.
    void BgcCompact();

    // ── Pinned Object Heap (POH) support ──────────────────────────

    /// Allocate in the Pinned Object Heap.  POH objects bypass young GC
    /// copying and are always treated as live during BGC sweep.
    /// Falls back to PohAllocate() in gc_region.cpp (REGION_POH regions).
    static void* AllocatePinned(CHAOS_IL2CPP_SIZE size) noexcept;

    // ── Pinned root support ─────────────────────────────────────

    /// Register a pinned root (object that must never be moved/collected).
    void AddPinnedRoot(void* addr, CHAOS_IL2CPP_SIZE size);

    // ── Emergency reserve (Finalizer OOM guarantee) ───────────────
    //
    // Pre-allocated memory pool that the finalizer thread can draw from
    // when the GC heap is exhausted.  Prevents finalizer deadlock during
    // OOM conditions (CoreCLR equivalent: gc_heap::emergency_allocation).
    static constexpr CHAOS_IL2CPP_SIZE kEmergencyReserveSize = 64 * 1024;  // 64 KB

    /// Allocate from the emergency reserve (bump-pointer, zeroed).
    /// Returns nullptr if the reserve is exhausted.
    void* AllocateFromEmergencyReserve(CHAOS_IL2CPP_SIZE size) noexcept;

    /// Reset the emergency reserve bump pointer (called after GC).
    /// Only resets if the reserve was activated during the cycle.
    void ReplenishEmergencyReserve() noexcept;

    /// Pre-allocate the emergency reserve during init.
    bool InitEmergencyReserve() noexcept;

    /// Initialize the emergency reserve with externally-provided memory
    /// (for testing with known addresses).
    void InitEmergencyReserveForTest(void* base, CHAOS_IL2CPP_SIZE size) noexcept;

    /// Check if the emergency reserve has available space.
    bool HasEmergencyReserveSpace() const noexcept {
        return emergency_reserve_base_ != nullptr &&
               emergency_reserve_current_.load(std::memory_order_acquire) <
                   emergency_reserve_base_ + emergency_reserve_size_;
    }

    // ── Root scanning helpers ────────────────────────────────────

    /// Try to mark a root from a stack slot value: read the pointer at @a addr
    /// and if it points to an old-gen page, mark it as a root for the next
    /// full GC collection.  Returns true if the pointer was in old-gen and
    /// was newly marked.
    /// This is the callback target for GcScanAllThreadRoots during full GC.
    bool TryMarkRoot(void* addr);

    /// Scan a memory range [begin, end) for raw pointers into old-gen pages
    /// and mark any found objects as reachable roots for the next collection.
    /// Runs DrainMarkStack() after scanning.
    void ScanRangeForRoots(void* range_begin, void* range_end);

    /// Mark every in-place-demoted object (gen1-owned, physically resident in an
    /// old-gen page) as a root so a full GC never sweeps a still-gen1-owned object.
    /// CoreCLR-aligned in-place demotion (GC-N6 #10).  Runs during the full-GC
    /// mark root phase, then DrainMarkStack() closes over each demoted object's
    /// transitive graph.
    void ScanInPlaceDemotedRoots();

    /// Scan dirty cards across all old-gen pages for young GC.
    /// Calls @a callback(card_index, card_start, card_end) for each dirty card
    /// found in any old-gen page.  This replaces the previous incorrect approach
    /// of scanning nursery address range (which never contains dirty cards).
    template <typename Fn>
    void ScanDirtyCardsInPages(Fn&& callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto* page = page_list_; page != nullptr; page = page->next) {
            if (!page->in_use.load(std::memory_order_acquire)) continue;
            uintptr_t page_start = reinterpret_cast<uintptr_t>(page->Payload());
            uintptr_t page_end = page_start + page->payload_size;
            ScanDirtyCards(page_start, page_end, std::forward<Fn>(callback));
        }
    }

    /// Batched variant: groups consecutive dirty cards into ranges.
    /// Calls @a callback(range_start, range_end) for each dirty range,
    /// reducing per-card callback overhead.  @a dirty_card_count is
    /// updated with the total number of dirty cards found (not ranges).
    template <typename Fn>
    void ScanDirtyCardsInPagesBatched(CHAOS_IL2CPP_SIZE* dirty_card_count,
                                       Fn&& callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto* page = page_list_; page != nullptr; page = page->next) {
            if (!page->in_use.load(std::memory_order_acquire)) continue;
            uintptr_t page_start = reinterpret_cast<uintptr_t>(page->Payload());
            uintptr_t page_end = page_start + page->payload_size;
            ScanDirtyCardsBatched(page_start, page_end, dirty_card_count, callback);
        }
    }

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

    /// Collect dead finalizable entries using the current mark bitmap.
    /// Used by BGC after concurrent sweep to identify objects that became
    /// unreachable during the BGC cycle.  The mark bitmap is still valid
    /// because BgcSweep preserves it (clear_bitmap=false).
    /// Returns entries whose finalizers should be invoked, with the
    /// finalizer_ list atomically cleared.
    std::vector<FinalizerEntry> CollectDeadFinalizables();

    // ── Diagnostics ─────────────────────────────────────────────

    CHAOS_IL2CPP_SIZE TotalAllocated() const {
        return total_allocated_.load(std::memory_order_relaxed);
    }
    CHAOS_IL2CPP_SIZE TotalPages() const { return page_count_; }

    /// Number of decommissioned 100%-free pages currently held in the reusable
    /// pool.  Read-only diagnostic accessor (locks mutex_); never valid while a
    /// collection is in progress.  Used by tests to lock pool recycle/trim
    /// behavior (plan-v6 M3/T5).
    int PoolPageCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return static_cast<int>(page_pool_.size());
    }

    /// True if the reusable page pool is over its capacity cap
    /// (kMaxPoolSize = 16).  Under provisional (high memory-pressure) mode the
    /// scheduler forces a collection so a sweep trims the pool back to cap and
    /// releases retained physical memory instead of letting it sit idle.
    bool IsPoolOversized() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return page_pool_.size() > static_cast<size_t>(kMaxPoolSize);
    }

    /// Monotonic count of Non-oversized Free() calls since the last Reset/
    /// read.  Used by DecideCollection to nudge a sweep when many normal frees
    /// have not yet been swept back into the reusable pool (M3/T5 FIX-1).
    CHAOS_IL2CPP_SIZE FreelistReleaseCount() const noexcept {
        return freelist_release_count_.load(std::memory_order_relaxed);
    }

    /// Reset the normal-free counter (called when a collection reclaims pages).
    void ResetFreelistReleaseCount() noexcept {
        freelist_release_count_.store(0, std::memory_order_relaxed);
    }

    // ── Page index (sorted array for O(log n) lookup) ───────────

    /// Sorted page array for O(log n) FindPage/IsInOldGen.
    /// Rebuilt under mutex_ after each page_list_ mutation.
    /// Readers: atomic load, then read-only (pages are stable once published).
    struct PageArray {
        OldGenPage** pages;  // sorted by page address ascending
        int count;
    };

    /// Get the sorted page array for iteration (lock-free).
    const PageArray* GetPageArray() const {
        return page_array_.load(std::memory_order_acquire);
    }

    /// DIAG: count how many bytes in old-gen payloads equal 0xFF.
    /// Used to detect unexpected writes during BGC root set population.
    uint64_t DiagCountOxFFBytes() const;

    /// Check if @a ptr falls within any old-gen page's payload range.
    /// Used by stress test deferred verification to confirm GC promotion.
    bool IsInOldGen(const void* ptr) const;

    /// DIAG: VirtualProtect all page payloads as PAGE_READONLY.
    /// Returns the number of ranges protected.  Call DiagUnprotectPayloads() to restore.
    /// Used to catch unexpected writes during BGC.
    int DiagProtectPayloads();

    /// DIAG: Restore PAGE_READWRITE on all page payloads.
    void DiagUnprotectPayloads();

    /// Check if an object in old-gen is marked (reachable in current GC cycle).
    /// Returns false if the object is not in old-gen or not marked.
    bool IsMarked(const void* obj) const;

    /// Mark a memory range [begin, end) as containing young-tenured objects
    /// (recently promoted from survivor).  BGC sweep prioritizes these pages
    /// for earlier reclamation.
    void MarkYoungTenuredRange(uintptr_t begin, uintptr_t end);

    /// Clear young_tenured flags on all pages (called after BGC sweep
    /// processes them).  Pages whose objects survived become mature tenured.
    void ClearYoungTenuredFlags();

    /// Estimate overall fragmentation ratio [0.0, 1.0] across all old-gen pages.
    /// 0.0 = no fragmentation, 1.0 = fully empty.
    /// Walks the page list (O(n) at page count), updates cached value.
    /// Used by the scheduler to adjust nursery sizing: high fragmentation →
    /// shrink nursery to reduce promotion rate, giving old gen time to compact.
    float OverallFragmentation() const;

    /// Mark an object as reachable. Returns false if already marked.
    /// Public: called by DependentHandle/weak-handle processing during full GC.
    bool MarkObject(void* obj);

    /// Add an object to the parallel mark stack for transitive closure.
    /// Used by DependentHandle fixed-point iteration during full GC.
    void AddToMarkStack(void* obj);

    /// Find the page containing @a ptr, or nullptr.
    OldGenPage* FindPage(const void* ptr) const;

private:
    // ── Page management ─────────────────────────────────────────

    /// Allocate a new page from the OS.
    /// @param preferred_sc_idx  If >= 0, carve most of the page for this size class.
    OldGenPage* AllocatePage(CHAOS_IL2CPP_SIZE size, bool scanning,
                             int preferred_sc_idx = -1);

    /// Return a page to the OS.
    void FreePage(OldGenPage* page);

    /// Rebuild the sorted page index from page_list_.
    /// Caller MUST hold mutex_.
    void RebuildPageArray();

public:
    // ── Demotion support accessors ────────────────────────────────
    // Exposed for gc_demotion.cpp (STW full-GC only — single threaded).

    /// Return the head of the page list.  Safe only when mutex_ is held.
    OldGenPage* PageList() const { return page_list_; }

    /// Return the page list mutex for STW scanning.
    std::mutex& PageMutex() { return mutex_; }

private:

    // ── Allocation helpers ──────────────────────────────────────

    /// Round up to the nearest size class. Returns size class index or -1.
    int SizeClassIndex(CHAOS_IL2CPP_SIZE size) const;

    /// Try to allocate from existing page free lists. Returns nullptr on miss.
    void* TryAllocateFromFreeLists(CHAOS_IL2CPP_SIZE size, int sc_idx);

    // ── GC helpers ──────────────────────────────────────────────

    /// Mark all objects on the mark stack (transitive closure).
    /// Sequential — used for small mark sets and finalizer re-mark.
    void DrainMarkStack();

    /// Parallel transitive closure marking using chunked work-stealing.
    /// Replaces DrainMarkStack() for the primary full-GC mark phase.
    /// @param pages     Snapshot of all pages.
    /// @param page_count Number of pages.
    void DrainMarkStackParallel(OldGenPage** pages, int page_count);

    /// Sweep a single page: reclaim unmarked blocks.
    /// @param clear_bitmap  If true, clear the mark bitmap after sweeping.
    ///                      Pass false during BGC sweep to preserve bitmap
    ///                      for subsequent compaction planning.
    CHAOS_IL2CPP_SIZE SweepPage(OldGenPage* page, bool clear_bitmap = true);

    /// Coalesce adjacent free blocks on a page.
    void CoalescePage(OldGenPage* page);

    /// Reclaim retired GcLayout tables (safe during STW).
    void ReclaimGcLayoutTables() {
        GcLayoutRegistry::Instance().ReclaimRetiredTables();
    }

    /// Phase 3b re-mark pass: iterate GCHandle table and mark reachable objects.
    /// Must be called from within Collect() after RunFinalizers().
    void HandleReMarkPass();

    /// Compute fragmentation ratio for a page: 1.0 - (live_bytes / payload_size).
    float PageFragmentation(const OldGenPage* page) const;

    // ── Compaction support (Phase 4) ─────────────────────────────

    /// Compaction mode for a full-GC cycle.
    enum class CompactMode : uint8_t {
        NONE,          ///< No compaction (standard mark-sweep)
        COMPACT,       ///< Compact each page (plan → relocate → compact)
        CROSS_PAGE     ///< Cross-page evacuation (evacuate fragmented pages)
    };

    /// Per-object relocation plan entry.
    struct CompactPlanEntry {
        void*   old_addr;      ///< Current object address
        void*   new_addr;      ///< New (compacted) object address
        CHAOS_IL2CPP_SIZE size;  ///< Object size in bytes
    };

    /// Per-page compaction plan produced by a single bitmap walk.
    struct CompactPlan {
        std::vector<CompactPlanEntry> entries;
        CHAOS_IL2CPP_SIZE live_bytes{0};
        CHAOS_IL2CPP_SIZE saved_bytes{0};
    };

    /// Compact fragmented pages in parallel using GcWorkerPool.
    /// Three-phase approach:
    ///   Phase 1 (parallel): Plan page compaction for each fragmented page.
    ///   Phase 2 (parallel): Compact each planned page (memmove + free list rebuild).
    ///   Phase 3 (parallel): Walk all pages' slots and relocate pointers.
    /// Must be called at safepoint (no concurrent mutators).
    CHAOS_IL2CPP_SIZE ParallelCompactPages();

    /// Plan compaction for a single page: compute new addresses.
    /// Fills @a out_plan with entries mapping old_addr → new_addr for each
    /// marked object.  @return Number of bytes that would be saved.
    CHAOS_IL2CPP_SIZE PlanPageCompaction(OldGenPage* page,
                                          CompactPlan& out_plan);

    /// Relocate all pointer references in old-gen pages to use compacted
    /// addresses from @a plan.
    void RelocatePage(OldGenPage* page, const CompactPlan& plan);

    /// Compact a page: memmove objects to their planned positions in @a plan.
    void CompactPage(OldGenPage* page, const CompactPlan& plan);

    /// Check fragmentation ratio and decide compaction mode.
    CompactMode DecideCompactMode();

    // ── Cross-page compaction (Phase 4b) ──────────────────────────

    static constexpr float kCrossPageFragThreshold = 0.30f;     // min frag to evacuate
    /// Floor for dynamic evacuation budget: minimum 1 MB per cycle.
    /// Actual budget = max(1MB, min(total_heap * 10%, 8MB)).
    /// Scaled with heap size so large heaps compact more per cycle.
    /// Increased from 512KB for more aggressive fragmentation reduction.
    static constexpr CHAOS_IL2CPP_SIZE kMaxCrossPageCompactBytes = 1024 * 1024;

    /// Run cross-page compaction: evacuate fragmented pages by moving
    /// live objects to free space on other pages, then freeing source pages.
    void CrossPageCompact();

    /// Build evacuation plan for a single source page.
    void PlanPageEvacuation(OldGenPage* page, CompactPlan& out_plan);

    /// Global relocation: walk all pages once and update pointers using
    /// a consolidated old→new address map from @a entries.
    static void GlobalRelocate(const std::vector<CompactPlanEntry>& entries,
                               OldGenPage* page_list);

    /// Relocate thread stack roots after compaction.
    /// Scans all thread stacks and updates any pointer that matches a
    /// compacted object's old address to its new address.
    /// Must be called at safepoint (all mutators suspended).
    static void RelocateRoots(const std::vector<CompactPlanEntry>& entries);

    // ── Page index (sorted array for O(log n) lookup) ────────────

public:

    // ── Finalizer suppression support ──────────────────────────
    /// Suppress finalization for @a obj.  Called from GC.SuppressFinalize.
    void SuppressFinalizer(void* obj);

    /// Re-register finalization for @a obj.  Called from GC.ReRegisterForFinalize.
    void ReRegisterFinalizer(void* obj);

    // ── State ───────────────────────────────────────────────────

    uintptr_t heap_base_ = 0;

    OldGenPage* page_list_ = nullptr;   // singly-linked list of all pages
    int         page_count_ = 0;

    mutable std::mutex mutex_;                  // protects page list + free lists
    std::atomic<CHAOS_IL2CPP_SIZE> total_allocated_{0};

    // Auto-init guard: true after Init() completes.
    std::atomic<bool> initialized_{false};

    // Separate mutex for auto-init (Allocate holds init_mutex_, then Init()
    // calls AllocatePage which internally takes mutex_, so we cannot use
    // mutex_ for the init guard).
    std::mutex init_mutex_;

    // Marked-object counter (reset each cycle, used by GcStats).
    std::atomic<uint64_t> marked_count_{0};

    // Mark stack for tri-color marking (vector = stack).
    // Used for sequential mark (finalizer re-mark) and as the seed source
    // for parallel mark.  Grows dynamically (no hard bound).
    std::vector<void*> mark_stack_;

    // Atomic page array for lock-free FindPage/IsInOldGen.
    // Written under mutex_, read with atomic load.
    std::atomic<PageArray*> page_array_{nullptr};

    // Retired page arrays kept alive forever (never freed).
    // RebuildPageArray exchanges the current page_array_ with a new one and
    // appends the old array to this vector.  Any concurrent reader that loaded
    // the old array pointer before the exchange can still safely dereference
    // it because the underlying memory is never freed.
    // Memory overhead: ~8 bytes per page + 16 bytes per array per rebuild,
    // amortized over process lifetime (GC collects at most ~1 Hz).
    std::vector<PageArray*> retired_arrays_;

    // Oversized pages freed by Free() but deferred to safepoint for
    // VirtualFree.  BgcSweep snapshots page_list_ under mutex_ and may
    // sweep stale entries — deferring the VirtualFree prevents access
    // to freed memory in the BgcSweep path.
    std::vector<OldGenPage*> deferred_free_pages_;

    // Pool of 100%-free normal pages that have been decommissioned for reuse.
    // Instead of VirtualFree, these are kept alive and recycled by AllocatePage
    // on the next allocation that needs a fresh page — avoids a system call.
    // Protected by mutex_.
    // Trimmed to kMaxPoolSize entries at the end of each BgcSweep() to prevent
    // unbounded memory retention after allocation spikes.
    // Pages in the pool are MEM_DECOMMIT'd to release physical pages while
    // keeping the virtual address range reserved.  On pop, the page is
    // MEM_COMMIT'd before reuse.
    struct PoolEntry {
        OldGenPage* page;
        CHAOS_IL2CPP_SIZE page_size;      // saved before decommit for recommit
        CHAOS_IL2CPP_SIZE payload_size;   // preserved across decommit (zeroed by OS)
        CHAOS_IL2CPP_SIZE bitmap_bytes;   // preserved across decommit
        int8_t        numa_node;          // NUMA node this page was allocated on (-1 = unknown)
    };
    static constexpr int kMaxPoolSize = 16;
    std::vector<PoolEntry> page_pool_;

    // Monotonic count of normal (non-oversized) Free() calls.  Read by the
    // scheduler (M3/T5 FIX-1) to decide when to nudge a sweep so fully-free
    // pages get pooled; reset when a collection reclaims them.
    std::atomic<CHAOS_IL2CPP_SIZE> freelist_release_count_{0};

    // Pool pages (100%-free normal pages) deferred from BgcSweep Phase 4b
    // to BgcCompact (STW safepoint).  BgcSweep runs concurrently with
    // mutators that may have allocated from a page's free list after
    // SweepPage rebuilt it.  Deferring the MEM_DECOMMIT to BgcCompact
    // prevents the mutator from writing to decommitted memory.
    // Same reasoning as deferred_free_pages_ — see BgcSweep Phase 4b.
    std::vector<PoolEntry> deferred_pool_pages_;

    // Per-size-class last-used-page cache (avoids O(n) page_list walk).
    OldGenPage* last_alloc_page_[kOldGenNumSizeClasses]{};

    // GC-N8 reuse-rate counters (incremented on the alloc path; sampled at
    // collection time).  Relaxed — lightweight telemetry, not correctness.
    std::atomic<uint64_t> free_list_hits_{0};
    std::atomic<uint64_t> free_list_carves_{0};

    // Pinned roots.
    struct PinnedRoot {
        void* addr;
        CHAOS_IL2CPP_SIZE size;
    };
    std::vector<PinnedRoot> pinned_roots_;

    // Pinned roots snapshot for compaction — set at start of Collect(),
    // cleared after compaction.  Pages containing pinned objects are
    // excluded from per-page compaction; pinned objects are excluded
    // from cross-page evacuation.
    std::vector<PinnedRoot> pinned_compact_skip_;

    // Finalizer table: maps object → finalizer callback.
    // Uses namespace-level FinalizerEntry (defined above).
    // Suppressed finalizers (objects with GC.SuppressFinalize called).
    std::vector<void*> suppressed_finalizers_;

    std::vector<FinalizerEntry> finalizers_;

    // ── Emergency reserve state ──────────────────────────────────

    /// Base address of the pre-allocated emergency reserve.
    char* emergency_reserve_base_{nullptr};

    /// Total size of the emergency reserve in bytes.
    CHAOS_IL2CPP_SIZE emergency_reserve_size_{0};

    /// Bump-pointer into the emergency reserve (atomic for thread safety).
    std::atomic<char*> emergency_reserve_current_{nullptr};

    /// True if the emergency reserve was used since the last replenish.
    std::atomic<bool> emergency_reserve_activated_{false};
};

// Global old-generation instance.  Defined in gc_old_gen.cpp.
extern MarkSweepOldGen g_old_gen;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_OLD_GEN_H_