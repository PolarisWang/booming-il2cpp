#ifndef CHAOS_IL2CPP_GC_SCHEDULER_H_
#define CHAOS_IL2CPP_GC_SCHEDULER_H_

#include <atomic>
#include <cstdint>
#include <cstring>
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
//   RecommendedNurserySize() returns [128KB, 1MB].
//   Target = survival_rate * 2 * last_nursery_used, clamped to bounds.
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

    // ── GC rate limiting ──────────────────────────────────────────

    /// Try to claim a "GC slot" — returns true if enough time has passed
    /// since the last GC completed.  Used to prevent safepoint storms:
    /// when multiple threads exhaust their nursery simultaneously, only
    /// the one that claims the slot proceeds; others skip the GC and just
    /// reallocate a fresh nursery.  Call RecordGcCompleted() after the GC
    /// finishes so the next GC can proceed.
    bool TryClaimGcSlot() noexcept;

    /// Record that a GC just completed (sets the rate-limiter timestamp).
    void RecordGcCompleted() noexcept;

    // ── Collection decision ──────────────────────────────────────

    /// Decide what kind of collection is needed right now.
    /// Returns FULL_BGC when the allocation threshold is exceeded and
    /// neither a young GC nor a forced full GC is more urgent.
    /// Returns FULL when BGC is already busy or memory pressure is high.
    GcCollectionKind DecideCollection() const noexcept;

    /// Check whether a full GC has been requested by any thread.
    bool IsFullGcRequested() const noexcept;

    /// Request a full GC from any thread (sets a flag polled at safepoints).
    void RequestFullGc() noexcept;

    // ── Nursery sizing ───────────────────────────────────────────

    /// Recommended nursery size in bytes, based on survival rate.
    /// Clamped to [128KB, 1MB].
    CHAOS_IL2CPP_SIZE RecommendedNurserySize() const noexcept;

    /// Last nursery used bytes (for sizing calculation).
    CHAOS_IL2CPP_SIZE LastNurseryUsed() const noexcept {
        return last_nursery_used_.load(std::memory_order_relaxed);
    }

    // ── Diagnostics ──────────────────────────────────────────────

    /// Quick check: has allocation since last GC exceeded the young threshold?
    /// Non-binding hint — actual decision is made in DecideCollection().
    /// Called from NurseryAllocate fast path to proactively trigger GC before
    /// any thread exhausts its TLS nursery.
    bool ShouldTriggerGc() const noexcept {
        auto nursery_size = last_nursery_used_.load(std::memory_order_relaxed);
        auto threshold = static_cast<CHAOS_IL2CPP_SIZE>(
            kYoungTriggerMultiplier * nursery_size);
        return alloc_since_last_gc_.load(std::memory_order_relaxed) >= threshold;
    }

    double SurvivalRate() const noexcept {
        return BitsToDouble(survival_rate_bits_.load(std::memory_order_relaxed));
    }
    CHAOS_IL2CPP_SIZE TotalAllocatedSinceLastGC() const noexcept;

private:
    // ── Constants ────────────────────────────────────────────────

    static constexpr double kEmaAlpha = 1.0 / 16.0;  // ~6% weight per sample
    static constexpr CHAOS_IL2CPP_SIZE kMinNurserySize = 128 * 1024;     // 128 KB
    static constexpr CHAOS_IL2CPP_SIZE kMaxNurserySize = 1024 * 1024;    // 1 MB
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
    static constexpr float kFullTriggerMultiplier = 4.0f;

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

    // Total bytes allocated since the last full GC.
    std::atomic<CHAOS_IL2CPP_SIZE> alloc_since_last_full_gc_{0};

    // Full GC request flag (set by any thread, checked at safepoint).
    std::atomic<bool> full_gc_requested_{false};

    // Estimated heap size (updated after full GC).
    std::atomic<CHAOS_IL2CPP_SIZE> estimated_heap_size_{0};

    // GC rate limiter: timestamp (steady_clock ns) of the last GC completion.
    // Threads check this before initiating a new GC; if the last GC was too
    // recent, they skip the GC and just reallocate the nursery.  This prevents
    // safepoint storms where 100 threads cascade GC initiations back-to-back.
    // Initialized to 0 (no GC has completed yet — first GC always allowed).
    std::atomic<uint64_t> last_gc_completion_ns_{0};

    /// Minimum interval between GC completions (in nanoseconds).
    /// 500 µs — enough to let all threads resume and make progress before
    /// the next safepoint.  Prevents the cascading safepoint-storm pattern
    /// where thread A's GC completes, thread B immediately starts another
    /// GC before other threads have resumed, causing all 99 threads to spin.
    static constexpr uint64_t kMinGcIntervalNs = 500 * 1000;  // 500 µs
};

/// Process-wide GC scheduler instance.
extern GcScheduler g_gc_scheduler;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_SCHEDULER_H_