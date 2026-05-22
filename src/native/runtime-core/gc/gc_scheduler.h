#ifndef CHAOS_IL2CPP_GC_SCHEDULER_H_
#define CHAOS_IL2CPP_GC_SCHEDULER_H_

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <chaos/native_types.h>

namespace chaos::il2cpp::runtime_core {

// V4-M8: Strict-aliasing-safe double↔uint64_t bitcast helpers.
inline uint64_t DoubleToBits(double d) noexcept {
    uint64_t bits;
    std::memcpy(&bits, &d, sizeof(bits));
    return bits;
}
inline double BitsToDouble(uint64_t bits) noexcept {
    double d;
    std::memcpy(&d, &bits, sizeof(d));
    return d;
}

// ======================================================================
// GCCollectionMode — controls how GC.Collect(int, GCCollectionMode) behaves
// ======================================================================

enum class GcCollectionMode : uint8_t {
    DEFAULT   = 0,  ///< Same as Forced for GC.Collect()
    FORCED    = 1,  ///< Immediate blocking collection (default)
    OPTIMIZED = 2,  ///< Let BGC decide; may defer or use concurrent mark
    AGGRESSIVE = 3, ///< Immediate blocking + compact (highest memory pressure)
};

// ======================================================================
// GCLatencyMode — controls GC responsiveness/pause trade-off
// ======================================================================

enum class GcLatencyMode : uint8_t {
    BATCH                = 0,  ///< Maximize throughput; longest pauses
    INTERACTIVE          = 1,  ///< Balanced pauses (default for desktop)
    LOW_LATENCY          = 2,  ///< Short pauses; GC avoids full blocking
    SUSTAINED_LOW_LATENCY= 3,  ///< Sustained low-latency mode
    NO_GC_REGION         = 4,  ///< No GC allowed temporarily (app-managed)
};

// ======================================================================
// BgcScope — controls which generations BGC collects in the next cycle
//
// BGC can collect Gen2 only (default) or Gen1+Gen2 together.  The scope
// is decided by the scheduler at the start of each BGC cycle based on
// current heap pressure, Gen1 occupancy, and survival rates.
// ======================================================================

enum class BgcScope : uint8_t {
    GEN2_ONLY = 0,  ///< BGC marks/sweeps only Gen2 (default, current behavior)
    GEN1_GEN2 = 1,  ///< BGC marks/sweeps both Gen1 and Gen2 together
};

// ======================================================================
// GcScheduler — adaptive GC scheduling with EMA survival-rate tracking
//
// Decides WHEN to run young vs full collections and HOW LARGE the next
// nursery should be, based on observed allocation and promotion rates.
//
// == EMA survival rate ==
// Each young GC records the survival ratio (promoted_bytes / nursery_used).
// The scheduler maintains an exponentially-weighted moving average with
// alpha = 1/16 (~6% weight per sample, last ~16 GCs dominate).
//
// == Collection decision ==
//   DecideCollection() returns one of:
//     NONE  — no GC needed
//     YOUNG — run a young (nursery) collection
//     FULL  — run a full (old-gen) mark-sweep
//
// == Nursery sizing ==
//   RecommendedNurserySize() returns [64KB, 4MB].
//   Target = survival_rate * multiplier * last_nursery_used, clamped to bounds.
//   When survival is low → shrink nursery (less to copy on each GC).
//   When survival is high → grow nursery (buy more time between GCs).
// ======================================================================

enum class GcCollectionKind {
    NONE = 0,
    YOUNG = 1,
    FULL = 2,       // STW full GC (memory pressure / emergency)
    FULL_BGC = 3,   // Background concurrent mark (low-latency)
};

class GcScheduler {
public:
    GcScheduler() = default;
    ~GcScheduler() = default;

    GcScheduler(const GcScheduler&) = delete;
    GcScheduler& operator=(const GcScheduler&) = delete;

    // ── Allocation tracking ──────────────────────────────────────

    /// Record that @a bytes were allocated (called from GcAllocate).
    /// Thread-safe (atomic counter).
    void RecordAllocation(CHAOS_IL2CPP_SIZE bytes) noexcept;

    /// Record that a GC just completed (sets cooldown counter).
    void RecordGcCompleted() noexcept;

    // ── Collection recording ─────────────────────────────────────

    /// Record a young collection result. Updates EMA survival rate.
    /// @param nursery_used  Bytes used in the nursery before collection.
    /// @param bytes_promoted  Bytes that survived (copied to tenured).
    /// @param pause_ns  STW pause duration in nanoseconds.
    void RecordYoungCollection(CHAOS_IL2CPP_SIZE nursery_used,
                               CHAOS_IL2CPP_SIZE bytes_promoted,
                               uint64_t pause_ns) noexcept;

    /// Record a full collection. Resets allocation counter.
    /// @param total_heap_bytes  Estimated total heap size (old-gen page usage)
    ///        for scheduling the next full GC trigger threshold.
    void RecordFullCollection(CHAOS_IL2CPP_SIZE total_heap_bytes, uint64_t pause_ns = 0) noexcept;

    // ── Gen1 collection recording ──────────────────────────────────

    /// Record a Gen1 collection result. Resets Gen1 budget counter.
    /// @param bytes_promoted  Bytes promoted from Gen1 to Gen2.
    /// @param objects_in_gen1  Total objects that existed in Gen1 before collection
    ///                         (for survival rate computation).
    /// @param pause_ns  STW pause duration in nanoseconds.
    void RecordGen1Collection(CHAOS_IL2CPP_SIZE bytes_promoted,
                              CHAOS_IL2CPP_SIZE objects_in_gen1,
                              uint64_t pause_ns = 0) noexcept;

    /// Record Gen0→Gen1 promotions. Called from young collector when
    /// objects are promoted into the survivor area (Gen1).
    void RecordGen1Allocation(CHAOS_IL2CPP_SIZE bytes) noexcept;

    /// EMA-smoothed Gen1 survival rate as a float in [0, 1].
    /// Used by GcGen1ShouldCollect() to adapt the occupancy threshold.
    float Gen1SurvivalRate() const noexcept {
        return BitsToDouble(
            gen1_survival_rate_bits_.load(std::memory_order_relaxed));
    }

    /// Record page count growth since last full GC.
    /// When page_count grows rapidly without matching reclaim, the GC
    /// scheduler can trigger earlier to prevent unbounded page growth.
    void RecordPageCountGrowth(int delta) noexcept;

    /// Reset page count growth counter (called after full GC).
    void ResetPageCountGrowth() noexcept;

    // ── GC rate limiting ──────────────────────────────────────────

    /// Try to claim a "GC slot" — returns true if enough time has passed
    /// since the last GC completed.  Used to prevent safepoint storms:
    /// when multiple threads exhaust their nursery simultaneously, only
    /// the one that claims the slot proceeds; others skip the GC and just
    /// reallocate a fresh nursery.  Call RecordGcCompleted() after the GC
    /// finishes so the next GC can proceed.
    bool TryClaimGcSlot() noexcept;


    // ── Collection decision ──────────────────────────────────────

    /// Decide what kind of collection is needed right now.
    /// Returns FULL_BGC when the allocation threshold is exceeded and
    /// neither a young GC nor a forced full GC is more urgent.
    /// Returns FULL when BGC is already busy or memory pressure is high.
    GcCollectionKind DecideCollection() const noexcept;

    /// Decide the scope for the next BGC cycle based on current heap state.
    /// Reads Gen1 occupancy, Gen1 survival rate, and Gen2 fragmentation.
    /// Sets bgc_scope_ and returns it.  Called before starting a BGC cycle.
    BgcScope DecideBgcScope() noexcept;

    /// Get the scope for the current/next BGC cycle.
    BgcScope GetBgcScope() const noexcept {
        return static_cast<BgcScope>(bgc_scope_.load(std::memory_order_acquire));
    }

    /// Check whether a full GC has been requested by any thread.
    bool IsFullGcRequested() const noexcept;

    /// Request a full GC from any thread (sets a flag polled at safepoints).
    void RequestFullGc() noexcept;

    // ── GCCollectionMode / GCLatencyMode ─────────────────────────

    /// Set the collection mode for the next explicit GC.Collect() call.
    void SetCollectionMode(GcCollectionMode mode) noexcept {
        collection_mode_.store(mode, std::memory_order_release);
    }

    /// Get the current collection mode.
    GcCollectionMode GetCollectionMode() const noexcept {
        return collection_mode_.load(std::memory_order_acquire);
    }

    /// Set the GC latency mode (controls pause vs throughput trade-off).
    void SetLatencyMode(GcLatencyMode mode) noexcept {
        latency_mode_.store(mode, std::memory_order_release);
    }

    /// Get the current GC latency mode.
    GcLatencyMode GetLatencyMode() const noexcept {
        return latency_mode_.load(std::memory_order_acquire);
    }

    // ── Nursery sizing ───────────────────────────────────────────

    /// Recommended nursery size in bytes, based on survival rate.
    /// Clamped to [64KB, 4MB].
    CHAOS_IL2CPP_SIZE RecommendedNurserySize() const noexcept;

    /// Last nursery used bytes (for sizing calculation).
    CHAOS_IL2CPP_SIZE LastNurseryUsed() const noexcept {
        return last_nursery_used_.load(std::memory_order_relaxed);
    }

    // ── Diagnostics ──────────────────────────────────────────────

    /// Quick check: has allocation since last GC exceeded the young threshold
    /// AND is the cooldown counter exhausted?
    /// Non-binding hint — actual decision is made in DecideCollection().
    /// Called from NurseryAllocate fast path to proactively trigger GC before
    /// any thread exhausts its TLS nursery.
    /// The cooldown prevents GC storms by skipping checks for ~N allocations
    /// after each GC completion.  With 100 threads, the global allocation
    /// counter exceeds the 512KB threshold after ~3 allocs/thread.  Without the
    /// cooldown, EVERY subsequent fast-path allocation would route to the slow
    /// path, creating a GC storm.
    bool ShouldTriggerGc() const noexcept {
        auto nursery_size = last_nursery_used_.load(std::memory_order_relaxed);
        auto threshold = static_cast<CHAOS_IL2CPP_SIZE>(
            kYoungTriggerMultiplier * nursery_size);
        if (alloc_since_last_gc_.load(std::memory_order_relaxed) < threshold)
            return false;
        // Cooldown: skip triggering if the cooldown counter is above zero.
        return gc_cooldown_skips_.load(std::memory_order_acquire) == 0;
    }

    /// Get the scheduler-recommended promotion age threshold.
    /// Computed from Gen1 EMA survival rate and pause-time cost.
    /// Set by RecordGen1Collection, read by young collector's Phase 4.
    int GetRecommendedPromotionAge() const noexcept {
        return scheduler_recommended_threshold_.load(std::memory_order_acquire);
    }

    /// Recommended Gen1 area size in bytes (formerly RecommendedSurvivorSize).
    /// Computed from EMA of promoted bytes per young GC and Gen1 survival rate.
    /// Clamped to [kMinGen1Size, kMaxGen1Size].
    CHAOS_IL2CPP_SIZE RecommendedGen1Size() const noexcept;

    // ── BGC monitoring accessors ──────────────────────────────────

    /// Number of BGC cycles that chose GEN1_GEN2 scope.
    uint32_t BgcGen1Gen2Count() const noexcept {
        return bgc_gen1_gen2_count_.load(std::memory_order_relaxed);
    }
    /// Number of BGC cycles that chose GEN2_ONLY scope.
    uint32_t BgcGen2OnlyCount() const noexcept {
        return bgc_gen2_only_count_.load(std::memory_order_relaxed);
    }
    /// Total bytes promoted from Gen1 by BGC StwCompact.
    CHAOS_IL2CPP_SIZE BgcGen1PromoteBytes() const noexcept {
        return bgc_gen1_promote_bytes_.load(std::memory_order_relaxed);
    }
    /// Total bytes kept in Gen1 by BGC (survival above threshold).
    CHAOS_IL2CPP_SIZE BgcGen1KeepBytes() const noexcept {
        return bgc_gen1_keep_bytes_.load(std::memory_order_relaxed);
    }

    /// Record BGC scope decision.  Increments the corresponding counter.
    void RecordBgcScopeDecision(BgcScope scope) noexcept;

    /// Record bytes promoted from Gen1 by BGC StwCompact.
    void RecordBgcGen1Promote(CHAOS_IL2CPP_SIZE bytes) noexcept;

    /// Record bytes kept in Gen1 by BGC (not promoted, left for Young GC).
    void RecordBgcGen1Keep(CHAOS_IL2CPP_SIZE bytes) noexcept;

    /// Force BGC scope (for testing).  Allows tests to exercise GEN1_GEN2
    /// paths without setting up precise heap conditions for the heuristic.
    void SetBgcScopeForTest(BgcScope scope) noexcept {
        bgc_scope_.store(static_cast<uint8_t>(scope), std::memory_order_release);
    }

    // ── External memory pressure ──────────────────────────────

    /// Notify scheduler of external (unmanaged) memory pressure.
    /// Called from chaos_gc_add_memory_pressure.  Accumulates in a
    /// process-wide counter that feeds into DecideCollection().
    void AddExternalMemoryPressure(CHAOS_IL2CPP_INT64 bytes) noexcept;

    /// Notify scheduler that external memory has been freed.
    /// Called from chaos_gc_remove_memory_pressure.
    void RemoveExternalMemoryPressure(CHAOS_IL2CPP_INT64 bytes) noexcept;

    /// Current outstanding external memory pressure in bytes.
    CHAOS_IL2CPP_INT64 ExternalMemoryPressure() const noexcept {
        return external_memory_pressure_.load(std::memory_order_relaxed);
    }

    // ── Full GC notification (GC.RegisterForFullGCNotification / WaitForFullGC*) ──

    /// Enable full GC notifications for the calling thread.
    /// After this call, WaitForFullGcApproach / WaitForFullGcComplete
    /// will block until a full GC cycle starts/completes.
    void EnableFullGcNotification() noexcept;

    /// Disable full GC notifications.
    void DisableFullGcNotification() noexcept;

    /// Signal that a full GC is about to start (called from Collect()).
    /// Wakes any thread blocked in WaitForFullGcApproach.
    void SignalFullGcApproach() noexcept;

    /// Signal that a full GC has completed (called from Collect()).
    /// Wakes any thread blocked in WaitForFullGcComplete.
    void SignalFullGcComplete() noexcept;

    /// Block until a full GC approach is signaled, or timeout_ms elapses.
    /// Returns true if approach was signaled, false on timeout.
    bool WaitForFullGcApproach(int32_t timeout_ms) noexcept;

    /// Block until a full GC completion is signaled, or timeout_ms elapses.
    /// Returns true if complete was signaled, false on timeout.
    bool WaitForFullGcComplete(int32_t timeout_ms) noexcept;

    /// Check whether full GC notification is currently enabled.
    bool IsNotificationEnabled() const noexcept {
        return fullgc_notification_enabled_.load(std::memory_order_acquire);
    }

    // ── Survivor sizing constants ──────────────────────────────────
    /// Minimum Gen1 area: 4 MB.  Below this, Gen1 filtering is too
    /// constrained and objects promote to Gen2 too quickly.
    static constexpr CHAOS_IL2CPP_SIZE kMinGen1Size = 4 * 1024 * 1024;   // 4 MB
    /// Maximum Gen1 area: 32 MB.  Above this, the virtual address space
    /// consumed by the independent Gen1 region becomes excessive relative
    /// to its filtering benefit.
    static constexpr CHAOS_IL2CPP_SIZE kMaxGen1Size = 32 * 1024 * 1024;  // 32 MB
    /// Default Gen1 area (current fixed size at init time).
    static constexpr CHAOS_IL2CPP_SIZE kDefaultGen1Size = 8 * 1024 * 1024; // 8 MB

    /// Young-collection EMA survival rate (nursery→survivor promotion).
    double SurvivalRate() const noexcept {
        return BitsToDouble(survival_rate_bits_.load(std::memory_order_relaxed));
    }
    CHAOS_IL2CPP_SIZE TotalAllocatedSinceLastGC() const noexcept;

    // ── Old-gen fragmentation ───────────────────────────────────

    /// Set old-gen fragmentation ratio [0..1000] (fixed-point, *1000).
    /// Called from RecordYoungCollection for nursery sizing.
    void SetOldGenFragmentation(float frag) noexcept {
        uint32_t fp = static_cast<uint32_t>(frag * 1000.0f);
        if (fp > 1000) fp = 1000;
        old_gen_fragmentation_fp_.store(fp, std::memory_order_release);
    }

    /// Get old-gen fragmentation ratio [0.0, 1.0].
    float OldGenFragmentation() const noexcept {
        return static_cast<float>(
            old_gen_fragmentation_fp_.load(std::memory_order_acquire)) / 1000.0f;
    }

private:
    std::atomic<uint32_t> old_gen_fragmentation_fp_{0};

    // ── Constants ────────────────────────────────────────────────

    static constexpr double kEmaAlpha = 1.0 / 16.0;  // ~6% weight per sample
    static constexpr CHAOS_IL2CPP_SIZE kMinNurserySize = 64 * 1024;      // 64 KB
    static constexpr CHAOS_IL2CPP_SIZE kMaxNurserySize = 4 * 1024 * 1024; // 4 MB
    static constexpr CHAOS_IL2CPP_SIZE kDefaultNurserySize = 256 * 1024; // 256 KB

    // Young GC trigger: allocation since last young GC exceeds this
    // multiplier × last nursery size.
    // Increased from 1.0 to 2.0 to reduce GC frequency — fewer safepoint
    // cycles and less overhead from BFS worklist / nursery scanning.
    // Trade-off: longer per-GC pauses (more objects to scan), but net
    // throughput is higher with reduced safepoint serialization.
    static constexpr float kYoungTriggerMultiplier = 2.0f;

    // Full GC trigger: allocation since last full GC exceeds this
    // multiplier × estimated heap size.
    // Reduced from 4.0x to 2.0x for more frequent BGC cycles, keeping
    // old-gen dead objects from accumulating between collections.
    static constexpr float kFullTriggerMultiplier = 2.0f;

    // High-pressure threshold: when alloc_since_last_full_gc exceeds
    // this multiplier × estimated_heap_size, the scheduler tightens
    // young GC triggers to reduce old-gen allocation rate.  Set higher
    // than kFullTriggerMultiplier so that normal full GCs fire first;
    // this only kicks in when the full GC is blocked or deferred.
    static constexpr float kHighPressureTriggerMultiplier = 3.0f;

    // When page_count has grown by this many pages since the last full GC
    // without a collection, trigger FULL_BGC.  Prevents unbounded page list
    // growth when the allocation-rate-based trigger (kFullTriggerMultiplier)
    // is too slow to react to rapid page allocation bursts.
    // Reduced from 32 to 16 for earlier BGC intervention.
    static constexpr int kMaxPageGrowthThreshold = 16;

    // ── State ────────────────────────────────────────────────────

    // V4-M8: survival_rate_ stored as atomic<uint64_t> (bitcast from double)
    // to avoid data races between RecordYoungCollection (writer, under safepoint)
    // and RecommendedNurserySize (reader, any thread, no safepoint).
    // Start at 0 (BitsToDouble(0) == 0.0, which naturally produces default nursery).
    std::atomic<uint64_t> survival_rate_bits_{0};

    // Last nursery used bytes for sizing calculation.
    std::atomic<CHAOS_IL2CPP_SIZE> last_nursery_used_{kDefaultNurserySize};

    // Total bytes allocated by the application since the last collection.
    std::atomic<CHAOS_IL2CPP_SIZE> alloc_since_last_gc_{0};

    /// EMA of bytes promoted per young GC (nursery→survivor).
    /// Updated by RecordYoungCollection, used by RecommendedSurvivorSize
    /// to estimate how much survivor space is needed between Gen1 collections.
    std::atomic<CHAOS_IL2CPP_SIZE> avg_promoted_bytes_{0};

    // Total bytes allocated since the last full GC.
    std::atomic<CHAOS_IL2CPP_SIZE> alloc_since_last_full_gc_{0};

    // ── Gen1 EMA survival tracking ──────────────────────────────────
    /// EMA-smoothed Gen1 survival rate (survivor→Gen2 promotion).
    /// Updated by RecordGen1Collection using the same kEmaAlpha as the
    /// young-collection EMA.  Used for promotion age tuning.
    std::atomic<uint64_t> gen1_survival_rate_bits_{0};

    /// Running total of Gen1 pause time (ns).  Accumulated by
    /// RecordGen1Collection for computing avg pause per promoted byte.
    std::atomic<uint64_t> gen1_total_pause_ns_{0};

    /// Running total of bytes promoted from Gen1→Gen2.  Paired with
    /// gen1_total_pause_ns_ to compute pause-time cost per byte.
    std::atomic<CHAOS_IL2CPP_SIZE> gen1_total_promoted_bytes_{0};

    /// Pause-time tolerance: maximum ns per promoted byte before the
    /// scheduler nudges the promotion age threshold up (fewer promotions
    /// per Gen1 collection = shorter pause).  Tuned for the mark-sweep +
    /// memcpy cost of ~5-10 ns/byte in the common case.
    static constexpr uint64_t kGen1MaxNsPerByte = 10;  // 10 ns/byte

    /// Minimum promotion age threshold (fastest promotion to Gen2).
    static constexpr int kGen1MinPromotionAge = 1;
    /// Maximum promotion age threshold (longest filtering in Gen1).
    static constexpr int kGen1MaxPromotionAge = 12;

    /// Scheduler-recommended promotion age threshold.
    /// Computed from Gen1 EMA survival rate + pause-time cost.
    /// Written by RecordGen1Collection, read by the young collector
    /// (gc_young_collector.cpp Phase 4) instead of computing locally.
    std::atomic<int> scheduler_recommended_threshold_{1};

    // Net page count growth since last full GC (new pages - reclaimed).
    // Tracked via RecordPageCountGrowth(), reset in RecordFullCollection().
    std::atomic<int> page_count_growth_{0};

    // Full GC request flag (set by any thread, checked at safepoint).
    std::atomic<bool> full_gc_requested_{false};

    // Estimated heap size (updated after full GC).
    std::atomic<CHAOS_IL2CPP_SIZE> estimated_heap_size_{kDefaultNurserySize};

    // Cooldown: skip during N RecordAllocation calls after each GC.
    // Set to kCooldownAllocations by RecordGcCompleted(); decremented
    // by RecordAllocation().  ShouldTriggerGc returns false while >0.
    // With kCooldownAllocations=256 and ~2KB avg allocation, the 512KB
    // threshold takes 256 allocs.  The cooldown of 256 allocs means
    // ShouldTriggerGc re-activates just as the counter re-crosses the
    // threshold, creating a natural ~1 GC per nursery-cycle cadence.
    static constexpr int kCooldownAllocations = 256;
    std::atomic<int> gc_cooldown_skips_{0};

    // GC rate limiter: timestamp (steady_clock ns) of the last GC completion.
    // Threads check this before initiating a new GC; if the last GC was too
    // recent, they skip the GC and just reallocate the nursery.  This prevents
    // safepoint storms where 100 threads cascade GC initiations back-to-back.
    // Initialized to 0 (no GC has completed yet — first GC always allowed).
    std::atomic<uint64_t> last_gc_completion_ns_{0};

    /// Minimum interval between GC completions (in nanoseconds).
    /// 50 ms — reduces GC frequency from every TLAB-pool exhaustion
    /// (~500µs under heavy multi-threaded allocation) to at most
    /// 20 Hz, allowing threads to batch multiple TLAB claims before
    /// triggering a safepoint.  CoreCLR uses a similar cooldown to
    /// prevent cascading safepoint storms at high thread counts.
    static constexpr uint64_t kMinGcIntervalNs = 50 * 1000 * 1000;  // 50 ms

    // ── External memory pressure state ─────────────────────────

    /// Process-wide external memory pressure counter.
    /// Accumulated by AddMemoryPressure, decremented by RemoveMemoryPressure.
    std::atomic<CHAOS_IL2CPP_INT64> external_memory_pressure_{0};

    /// Minimum absolute threshold for external memory pressure triggering.
    /// Below this, external pressure alone won't trigger a full GC.
    static constexpr CHAOS_IL2CPP_INT64 kMinExternalPressureThreshold = 256 * 1024 * 1024;  // 256 MB

    /// External pressure ratio: when external pressure exceeds this fraction
    /// of the estimated managed heap size, it contributes to the full-GC
    /// trigger decision.  50% = external pressure equal to half the managed
    /// heap is treated as significant enough to warrant a GC.
    static constexpr float kExternalPressureRatio = 0.5f;

    // ── GCCollectionMode / GCLatencyMode state ───────────────────

    /// Collection mode for the next explicit GC.Collect() call.
    std::atomic<GcCollectionMode> collection_mode_{GcCollectionMode::DEFAULT};

    /// Latency mode (controls GC responsiveness).
    std::atomic<GcLatencyMode> latency_mode_{GcLatencyMode::INTERACTIVE};

    // ── BGC scope state ──────────────────────────────────────────

    /// Cached BGC scope for the current/next BGC cycle.
    /// Set by DecideBgcScope(), read by StartBgcCycle() and BGC worker paths.
    std::atomic<uint8_t> bgc_scope_{static_cast<uint8_t>(BgcScope::GEN2_ONLY)};

    // ── BGC monitoring counters ──────────────────────────────────

    /// Number of BGC cycles that chose GEN1_GEN2 scope.
    std::atomic<uint32_t> bgc_gen1_gen2_count_{0};
    /// Number of BGC cycles that chose GEN2_ONLY scope.
    std::atomic<uint32_t> bgc_gen2_only_count_{0};
    /// Total bytes promoted from Gen1 by BGC StwCompact.
    std::atomic<CHAOS_IL2CPP_SIZE> bgc_gen1_promote_bytes_{0};
    /// Total bytes kept in Gen1 by BGC StwCompact.
    std::atomic<CHAOS_IL2CPP_SIZE> bgc_gen1_keep_bytes_{0};

    // ── Full GC notification state ──────────────────────────────────
    /// When true, SignalFullGcApproach/Complete will notify waiters.
    std::atomic<bool> fullgc_notification_enabled_{false};

    /// Signaled by SignalFullGcApproach before the GC cycle starts.
    /// Sat under notification_mutex_ — use approach_cv_ for blocking wait.
    bool fullgc_approach_signaled_{false};

    /// Signaled by SignalFullGcComplete after the GC cycle ends.
    /// Sat under notification_mutex_ — use complete_cv_ for blocking wait.
    bool fullgc_complete_signaled_{false};

    /// Mutex protecting notification flags and CV wait.
    mutable std::mutex notification_mutex_;

    /// Condition variable for approach waiters.
    mutable std::condition_variable approach_cv_;

    /// Condition variable for complete waiters.
    mutable std::condition_variable complete_cv_;
};

/// Process-wide GC scheduler instance.
extern GcScheduler g_gc_scheduler;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_SCHEDULER_H_