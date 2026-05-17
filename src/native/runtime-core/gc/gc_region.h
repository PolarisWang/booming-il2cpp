#ifndef CHAOS_IL2CPP_GC_REGION_H_
#define CHAOS_IL2CPP_GC_REGION_H_

#include <chaos/native_types.h>
#include <chaos/log.h>
#include <chaos/unordered_dense.h>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <mutex>

#include "gc_scheduler.h"
#include "thread_state.h"

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// CRAG: Chaos Region-Aware GC — C0.1 Region Framework
//
// Phase C of the A→B→C allocation roadmap.  The region framework unifies
// all three allocation domains (GC, Domain, Raw) under a single Region
// manager, replacing the three separate allocator paths with a shared
// region-based substrate.
//
// == M0 scope ==
// - Region / RegionKind / RegionId type definitions
// - RegionManager singleton (allocate / free / release-domain)
// - NurseryAllocate inline fast path (bump pointer)
// - DomainAllocate (per-domain region bump)
// - RawAllocate (malloc fallback)
// - RegionAllocate unified entry point
//
// At M0, the old GcBumpCache and BDWGC paths remain active — regions are
// allocated via GC_MALLOC under the hood.  Later stages replace the backend.
// ======================================================================

// ── Region identifier ──────────────────────────────────────────
using RegionId = CHAOS_IL2CPP_UINT32;
constexpr RegionId kRegionIdInvalid = 0u;

// ── Region kind ────────────────────────────────────────────────
// Note: Prefix enum values with REGION_ to avoid collision with
// Windows macros (DOMAIN from <math.h>, RAW from <windows.h>).
enum class RegionKind : CHAOS_IL2CPP_UINT8 {
    REGION_NURSERY       = 0,  // Young generation (bump-pointer, per-thread)
    REGION_TENURED       = 1,  // Old generation (mark-sweep, optionally compacted)
    REGION_DOMAIN        = 2,  // Per-module metadata (bump-pointer, bulk-release on unload)
    REGION_RAW           = 3,  // Raw/temp allocations (malloc-backed)
    REGION_FOH           = 4,  // Freakishly large object heap (>85 KB, BDWGC mark-sweep)
    REGION_POH           = 5,  // Pinned object heap (bump-pointer, no young GC copy)
};

// ── Region structure ───────────────────────────────────────────
// Each contiguous memory span managed by the CRAG system.
struct Region {
    RegionId        id;
    int             table_slot;     // index in RegionManager::region_table_
    RegionKind      kind;
    CHAOS_IL2CPP_UINT32 domain_id;  // 0 = global / no domain
    char*           begin;
    char*           end;
    char*           current;        // bump pointer (NURSERY / DOMAIN)

    struct {
        bool        marked;         // reachable in current GC cycle
        bool        survivor;       // promoted from nursery at least once
    } gc_state;

    Region*         next;           // free-list / GC work-set link
};

// ── Forward declarations ──────────────────────────────────────
class RegionManager;

// ======================================================================
// NurseryAllocate — inline bump-pointer fast path
//
// Each thread in cooperative mode has a TLS nursery region.  Allocation
// is a pointer bump + zero-init, ~10 native instructions.
// ======================================================================

// TLS nursery context — set up when a thread enters cooperative mode.
struct NurseryContext {
    Region* nursery;        // current nursery region (nullptr = none)
    char*   limit;          // nursery->end - kMaxNurseryAlloc, cached for fast check
};

// Maximum single allocation that can be serviced from the nursery.
// Larger allocations go directly to the old gen or FOH.
static constexpr CHAOS_IL2CPP_SIZE kMaxNurseryAlloc = 32 * 1024;  // 32 KB

// POH region size (per pinned-object heap segment).
static constexpr CHAOS_IL2CPP_SIZE kPohRegionSize = 64 * 1024;   // 64 KB

extern thread_local NurseryContext tls_nursery_ctx;

/// Slow path for NurseryAllocate — acquires a new nursery region or
/// falls back to old-gen allocation when the request exceeds capacity.
void* NurseryAllocateSlow(CHAOS_IL2CPP_SIZE size);

/// Slow path for NurseryAllocateAtomic — same as NurseryAllocateSlow but
/// passes scanning_required=false for pointer-free allocations.
void* NurseryAllocateAtomicSlow(CHAOS_IL2CPP_SIZE size);

/// Allocate in the Pinned Object Heap (POH), bypassing nursery.
/// POH objects never participate in young GC copying (they are not in
/// nursery regions).  Allocation is bump-pointer within the current POH
/// region; falls back to allocating a new POH region when exhausted.
/// Returns zeroed memory, nullptr on OOM.
void* PohAllocate(CHAOS_IL2CPP_SIZE size) noexcept;

/// Check if @a ptr falls within any REGION_POH region.
bool IsPohPointer(const void* ptr) noexcept;

/// Release the current TLS nursery back to the region manager's free pool.
/// Called when a thread exits cooperative GC mode.
void TeardownTlsNursery();

/// Release the current TLS POH bump context (called on thread detach).
void TeardownTlsPoh() noexcept;

/// Inline bump-pointer allocation from the current thread's nursery.
/// Returns zeroed memory.
///
/// CoreCLR-style fast path: alloc_ptr (nursery->current) starts at nullptr/0.
/// The first call on any thread always falls through to NurseryAllocateSlow,
/// which acquires the first nursery region.  All subsequent calls hit the
/// bump path — no null check needed on the hot path.
inline void* NurseryAllocate(CHAOS_IL2CPP_SIZE size) noexcept {
    size = (size + 7) & ~static_cast<CHAOS_IL2CPP_SIZE>(7);  // 8-byte align

    // Enforce maximum nursery allocation size.
    // Objects larger than kMaxNurseryAlloc must go through the slow path,
    // which routes them to the old generation (size-class or oversized).
    if (size > kMaxNurseryAlloc) [[unlikely]] {
        return NurseryAllocateSlow(size);
    }

    auto* ctx = &tls_nursery_ctx;
    char* ptr = ctx->nursery ? ctx->nursery->current : nullptr;
    char* next = ptr + size;

    if (ptr != nullptr && next <= ctx->nursery->end) [[likely]] {
        ctx->nursery->current = next;
        std::memset(ptr, 0, size);
        g_gc_scheduler.RecordAllocation(size);
        // Proactive GC trigger with rate limiting.
        if (g_gc_scheduler.ShouldTriggerGc()) [[unlikely]] {
            return NurseryAllocateSlow(size);
        }
        // Safepoint poll: check if a GC safepoint is active and confirm it.
        threading::SafepointPoll();
        return ptr;
    }
    // The slow path is never inlined — it's a full function call.
    return NurseryAllocateSlow(size);
}

/// Atomic (pointer-free) variant of NurseryAllocate.
/// Routes to NurseryAllocateAtomicSlow for oversized/old-gen fallback,
/// so large pointer-free allocations skip bitmap scanning.
inline void* NurseryAllocateAtomic(CHAOS_IL2CPP_SIZE size) noexcept {
    size = (size + 7) & ~static_cast<CHAOS_IL2CPP_SIZE>(7);
    if (size > kMaxNurseryAlloc) [[unlikely]] {
        return NurseryAllocateAtomicSlow(size);
    }

    auto* ctx = &tls_nursery_ctx;
    char* ptr = ctx->nursery ? ctx->nursery->current : nullptr;
    char* next = ptr + size;

    if (ptr != nullptr && next <= ctx->nursery->end) [[likely]] {
        ctx->nursery->current = next;
        std::memset(ptr, 0, size);
        g_gc_scheduler.RecordAllocation(size);
        // Safepoint poll for the atomic variant — same rationale as NurseryAllocate.
        threading::SafepointPoll();
        return ptr;
    }
    return NurseryAllocateAtomicSlow(size);
}

// ======================================================================
// DomainAllocate — region-based allocation for module metadata
//
// Each domain gets one or more DOMAIN-kind regions.  Allocation is
// bump-pointer within the domain's current active region.
// ======================================================================

/// Allocate @a size bytes in the DOMAIN region of @a domain_id.
/// Falls back to malloc when no region has space.
/// Returns zeroed memory.
void* DomainAllocate(CHAOS_IL2CPP_UINT32 domain_id, CHAOS_IL2CPP_SIZE size);

// ======================================================================
// RawAllocate — malloc-based allocation for temp/non-GC structures
//
// Simple wrapper.  In later stages this may be
// redirected to a RAW region pool.
// ======================================================================

inline void* RawAllocate(CHAOS_IL2CPP_SIZE size) {
    return CHAOS_IL2CPP_MALLOC(size);
}

inline void RawFree(void* ptr) {
    CHAOS_IL2CPP_FREE(ptr);
}

// ======================================================================
// RegionManager — process-wide region lifecycle manager
//
// Singleton that owns all regions, maintains free-lists,
// and provides bulk-release for domain unload.
//
// Thread-safety: AllocateRegion/FreeRegion are mutex-protected.
// Nursery allocation (per-thread) is lock-free.
// ======================================================================

class RegionManager {
public:
    static constexpr CHAOS_IL2CPP_SIZE kDefaultRegionSize = 256 * 1024;   // 256 KB nursery
    static constexpr CHAOS_IL2CPP_SIZE kDomainRegionSize  = 64 * 1024;    // 64 KB domain
    static constexpr CHAOS_IL2CPP_SIZE kTenuredRegionSize = 1024 * 1024;  // 1 MB tenured

    /// Access the process-wide singleton.
    static RegionManager& Instance() {
        static RegionManager instance;
        return instance;
    }

    // ── Region lifecycle ───────────────────────────────────────

    /// Allocate a new region of the given kind and minimum size.
    /// Returns nullptr on OOM.
    Region* AllocateRegion(RegionKind kind, CHAOS_IL2CPP_SIZE min_size,
                           CHAOS_IL2CPP_UINT32 domain_id = 0);

    /// Return a region to the free pool.
    void FreeRegion(RegionId id);

    /// Release all regions owned by @a domain_id.
    /// Used during hot-update domain unload.
    void ReleaseDomainRegions(CHAOS_IL2CPP_UINT32 domain_id);

    /// Return the first region of kind REGION_POH (for PohAllocate fallback).
    /// Returns nullptr if no POH region exists yet.
    Region* GetFirstPohRegion() const;

    /// Return the total number of POH regions (for sweep iteration).
    int GetPohRegionCount() const;

    // ── Nursery management ─────────────────────────────────────

    /// Allocate a fresh nursery for the calling thread.
    Region* AllocateNursery();

    /// Allocate a nursery of the given size (for adaptive sizing).
    Region* AllocateNurseryOfSize(CHAOS_IL2CPP_SIZE size);

    /// Promote all live objects in the current nursery (called by
    /// young collection).  Returns the number of bytes promoted.
    CHAOS_IL2CPP_SIZE PromoteNursery(Region* nursery);

    // ── Diagnostics ────────────────────────────────────────────

    CHAOS_IL2CPP_UINT32 ActiveRegionCount() const {
        return region_count_;
    }

    CHAOS_IL2CPP_UINT64 TotalAllocatedBytes() const {
        return total_allocated_bytes_.load(std::memory_order_relaxed);
    }

    /// Dump region table to stderr (PROFILE format).
    void DumpRegions() const;

    /// Check if @a ptr falls within any region owned by @a domain_id.
    /// Used by the domain unloader to identify cross-domain references.
    bool IsInDomain(CHAOS_IL2CPP_UINT32 domain_id, const void* ptr) const;

    /// Check if @a ptr falls within any REGION_NURSERY region.
    /// Used by IsInNursery to detect cross-thread nursery pointers.
    /// Lock-free: reads a fixed-size array of nursery range slots with
    /// atomic loads.  Writers (AllocateRegion/FreeRegion) publish updates
    /// via atomic store with release ordering.
    bool IsNurseryPointer(const void* ptr) const;

    /// Check if @a ptr falls within any REGION_POH region.
    /// Lock-free (reads poh_slots_ array with atomic loads).
    /// Used by IsPohPointer to detect pinned-object-heap membership.
    bool IsPohPointer(const void* ptr) const;

private:
    RegionManager() = default;
    ~RegionManager() = default;
    RegionManager(const RegionManager&) = delete;
    RegionManager& operator=(const RegionManager&) = delete;

    /// Lock-free nursery range slot.
    /// begin >= end means the slot is invalid/freed.
    struct NurseryRangeSlot {
        std::atomic<uintptr_t> begin{0};
        std::atomic<uintptr_t> end{0};
    };
    static constexpr int kMaxNurserySlots = 512;

    mutable NurseryRangeSlot nursery_slots_[kMaxNurserySlots]{};
    mutable std::atomic<int> nursery_slot_count_{0};

    // Lock-free POH range slot (same design as nursery_slots_).
    struct PohRangeSlot {
        std::atomic<uintptr_t> begin{0};
        std::atomic<uintptr_t> end{0};
    };
    static constexpr int kMaxPohSlots = 64;
    mutable PohRangeSlot poh_slots_[kMaxPohSlots]{};
    mutable std::atomic<int> poh_slot_count_{0};

    /// Add a nursery range or find a reusable slot.
    void AddNurseryRange(uintptr_t begin, uintptr_t end);

    /// Remove a nursery range (mark as invalid).
    void RemoveNurseryRange(uintptr_t begin, uintptr_t end);

    /// Add a POH range for lock-free IsPohPointer.
    void AddPohRange(uintptr_t begin, uintptr_t end);

    /// Remove a POH range.
    void RemovePohRange(uintptr_t begin, uintptr_t end);

    /// Grow the region table to accommodate a new region.
    /// Returns the index of the new slot, or -1 on OOM.
    int AllocSlot();

    Region* region_table_ = nullptr;
    CHAOS_IL2CPP_UINT32 region_count_ = 0;
    CHAOS_IL2CPP_UINT32 region_capacity_ = 0;

    Region* free_list_ = nullptr;         // recycled regions
    int free_list_size_ = 0;             // current free list length (for trimming)

    std::atomic<CHAOS_IL2CPP_UINT64> total_allocated_bytes_{0};

    mutable std::mutex mutex_;

    // O(1) index: region id → region_table_ slot number.
    CHAOS_IL2CPP_UNORDERED_DENSE_MAP(RegionId, CHAOS_IL2CPP_INT32) region_index_;

    // Domain index: domain_id → vector of region_table_ slot numbers.
    // Enables O(k) ReleaseDomainRegions and IsInDomain (k = regions per domain).
    CHAOS_IL2CPP_UNORDERED_DENSE_MAP(CHAOS_IL2CPP_UINT32, CHAOS_IL2CPP_VECTOR(CHAOS_IL2CPP_INT32)) domain_regions_;
};

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_REGION_H_