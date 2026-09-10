#ifndef CHAOS_IL2CPP_GC_YOUNG_GEN_H_
#define CHAOS_IL2CPP_GC_YOUNG_GEN_H_

#include <chaos/native_types.h>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace chaos::il2cpp::runtime_core {

// Forward declarations (full definitions in gc_region.h).
struct Region;

// ======================================================================
// Shared Young Generation
//
// Replaces per-thread TLS nurseries with a single process-wide young
// generation region.  Each thread allocates from a 64 KB Thread-Local
// Allocation Buffer (TLAB) carved from the shared region via atomic
// fetch_add.  TLAB exhaustion = claim a new TLAB (no GC).  Only when
// the entire young region is exhausted does GcYoungCollection() fire.
// ======================================================================

// ── Constants ────────────────────────────────────────────────

/// Default size of the shared young generation region.
/// Set to 16 MB to accommodate 256 concurrent TLABs (64 KB each)
/// before triggering a young GC.
static constexpr CHAOS_IL2CPP_SIZE kDefaultYoungRegionSize = 64 * 1024 * 1024;  // 64 MB

/// Default TLAB size per thread.
/// Each thread carves a 64 KB buffer from the shared young region.
/// TLAB exhaustion claims another 64 KB (no GC).
static constexpr CHAOS_IL2CPP_SIZE kDefaultTlabSize = 64 * 1024;  // 64 KB

/// Maximum single allocation serviced from a TLAB.
/// Allocations larger than this bypass TLAB and go directly to old gen.
/// This is a runtime-latchable value (not constexpr): it defaults to the
/// historical 32 KB constant but is overwritten from GcConfig().MaxTlabAlloc
/// during GC init, so the env/API-driven CHAOS_GC_MaxTlabAlloc knob actually
/// drives the allocation hot path.  Every consumer reads a plain machine load.
inline CHAOS_IL2CPP_SIZE kMaxTlabAlloc = 32 * 1024;  // 32 KB

// ── TLAB (Thread-Local Allocation Buffer) ────────────────────

/// Per-thread allocation buffer carved from the shared young region.
struct TLAB {
    char* start{nullptr};        // Beginning of this TLAB
    char* current{nullptr};      // Current bump pointer within TLAB
    char* end{nullptr};          // End of TLAB (exclusive)
    char* start_scan{nullptr};   // For young GC Phase 2 scanning
    char* current_scan{nullptr}; // Current scan position within TLAB
};

// ── Young Generation Region ──────────────────────────────────

/// Shared young generation state.
/// A single process-wide young region (kind=REGION_NURSERY, domain_id=0).
/// Threads carve TLABs from this region atomically.
struct YoungGeneration {
    /// The shared young region (nullptr = uninitialized).
    std::atomic<Region*> region{nullptr};

    /// Bump pointer for TLAB carving.
    std::atomic<char*> bump{nullptr};

    /// End of the young region (exclusive).
    std::atomic<char*> region_end{nullptr};

    // ── Gen1 survivor space (independent REGION_GEN1 region) ──
    /// The Gen1 survivor region (nullptr = uninitialized / not allocated).
    /// Objects that survive one young GC are copied here via promotion_age_threshold.
    /// Replaces the old fixed 8 MB survivor area within the nursery region.
    std::atomic<Region*> gen1_region{nullptr};

    /// Gen1 bump pointer for atomic allocation during promotion.
    /// Separate from Region::current because current is non-atomic and
    /// Gen1 allocation needs CAS for multi-threaded promotion.
    std::atomic<char*> gen1_bump{nullptr};

    /// Cached Gen1 end pointer (exclusive) for fast bounds check.
    char* gen1_end{nullptr};

    /// End of the compacted survivor area from the last Gen1 collection.
    /// Objects below this boundary have survived at least one Gen1 collection
    /// and will be promoted to Gen2 on the next collection ("old" objects).
    /// Objects at or above this boundary are freshly promoted from Gen0
    /// ("new" objects) and will be compacted in Gen1 for another cycle.
    /// Initially nullptr (no previous collection — all objects are "new").
    /// Updated by GcGen1Collection after compaction.
    char* gen1_prev_compact_end{nullptr};

    /// Promotion age threshold: number of young GCs an object survives
    /// in the survivor area before being promoted to old gen.
    /// Dynamic (set by scheduler based on EMA survival rate):
    ///   1 = current behavior: every survivor object promotes on next GC
    ///   2+ = objects survive N cycles in survivor before promotion
    ///   - Low survival rate → higher threshold (filter more aggressively)
    ///   - High survival rate → lower threshold (promote faster)
    std::atomic<int> promotion_age_threshold_{2};

    /// Young GC counter, incremented each young GC cycle.
    /// Used by the dynamic promotion threshold to decide drain intervals.
    std::atomic<int> young_gc_count_{0};

    // ── Safepoint-Bypass (SPB) emergency allocation reserve ──────
    /// Size of the emergency TLAB pool carved from the nursery end.
    /// 256 KB = ~1.5% of the default 16 MB nursery, sufficient for 4
    /// threads × 128 KB emergency TLABs to ride through BGC pauses.
    static constexpr CHAOS_IL2CPP_SIZE kEmergencyTlabSize = 256 * 1024;  // 256 KB

    /// Start of the emergency reserve (nursery end - kEmergencyTlabSize).
    /// The young GC skips this region by using region_end = emergency_start.
    char* emergency_start{nullptr};

    /// Emergency bump pointer — CAS-claimed, lock-free.
    /// Threads in safepoint-bypass mode claim TLABs from this pointer
    /// instead of blocking on WaitForSingleObject(INFINITE).
    std::atomic<char*> emergency_bump{nullptr};
};

// ── Global state ─────────────────────────────────────────────

/// The shared young generation (process-wide).
extern YoungGeneration g_young_gen;

/// This thread's current TLAB.
extern thread_local TLAB tls_tlab;

/// Per-thread adaptive TLAB size.  Initialized to kDefaultTlabSize and
/// adjusted after each young GC based on utilization:
///   >75% utilization → double (up to 256 KB)
///   <25% utilization → halve (down to 16 KB)
///   otherwise → keep
extern thread_local CHAOS_IL2CPP_SIZE tls_tlab_size;

// ── Lifecycle ────────────────────────────────────────────────

/// Initialize the shared young generation.
/// Allocates a single REGION_NURSERY region via RegionManager.
void InitYoungGeneration() noexcept;

/// Destroy the shared young generation.
/// Frees the young region back to RegionManager.
void DestroyYoungGeneration() noexcept;

// ── TLAB allocation ──────────────────────────────────────────

/// Atomically claim a TLAB from the shared young region.
/// Uses CAS on g_young_gen.bump to carve kDefaultTlabSize.
/// Returns an empty TLAB (all fields nullptr) when the region is exhausted.
TLAB TlabClaimFromYoungGen() noexcept;

// ── GC trigger ───────────────────────────────────────────────

// Forward declaration for FlushTlsAllocCounter
struct GcScheduler;

/// Flush the TLS allocation counter to the scheduler.
inline void FlushTlsAllocCounter() noexcept {
    extern thread_local CHAOS_IL2CPP_SIZE tls_alloc_since_last_gc;
    extern GcScheduler g_gc_scheduler;
    if (tls_alloc_since_last_gc > 0) {
        g_gc_scheduler.RecordAllocation(tls_alloc_since_last_gc);
        tls_alloc_since_last_gc = 0;
    }
}

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_YOUNG_GEN_H_
