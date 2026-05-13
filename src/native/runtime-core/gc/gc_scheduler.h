#ifndef CHAOS_IL2CPP_GC_SCHEDULER_H_
#define CHAOS_IL2CPP_GC_SCHEDULER_H_

#include <chaos/native_types.h>

#include <atomic>
#include <cstdint>

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

namespace chaos::il2cpp::runtime_core {

enum class GcCollectionKind {
    NONE = 0,
    YOUNG = 1,
    FULL = 2,
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
    void RecordFullCollection(uint64_t pause_ns) noexcept;

    // ── Collection decision ──────────────────────────────────────

    /// Decide what kind of collection is needed right now.
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

    double SurvivalRate() const noexcept { return survival_rate_; }
    CHAOS_IL2CPP_SIZE TotalAllocatedSinceLastGC() const noexcept;

private:
    // ── Constants ────────────────────────────────────────────────

    static constexpr double kEmaAlpha = 1.0 / 16.0;  // ~6% weight per sample
    static constexpr CHAOS_IL2CPP_SIZE kMinNurserySize = 128 * 1024;     // 128 KB
    static constexpr CHAOS_IL2CPP_SIZE kMaxNurserySize = 1024 * 1024;    // 1 MB
    static constexpr CHAOS_IL2CPP_SIZE kDefaultNurserySize = 256 * 1024; // 256 KB

    // Young GC trigger: allocation since last young GC exceeds this
    // multiplier × last nursery size.
    static constexpr float kYoungTriggerMultiplier = 1.0f;

    // Full GC trigger: allocation since last full GC exceeds this
    // multiplier × estimated heap size.
    static constexpr float kFullTriggerMultiplier = 4.0f;

    // ── State ────────────────────────────────────────────────────

    // EMA survival rate (0.0 = nothing survives, 1.0 = everything survives).
    double survival_rate_ = 0.5;  // start at 50% (conservative)

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
};

/// Process-wide GC scheduler instance.
extern GcScheduler g_gc_scheduler;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_SCHEDULER_H_