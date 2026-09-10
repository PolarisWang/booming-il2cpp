#ifndef CHAOS_IL2CPP_GC_STATS_H_
#define CHAOS_IL2CPP_GC_STATS_H_

#include <chaos/native_types.h>

#include <atomic>
#include <chrono>
#include <cstdint>

#include "gc_etw.h"

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// GcStats — process-wide GC statistics accumulator
//
// All counters are std::atomic<uint64_t> with memory_order_relaxed for
// thread-safe, low-contention recording.  GcDumpStats() emits GC|-prefixed
// lines to stdout at process exit (see gc_stats.cpp).
//
// Phase 7 additions:
// - GcGetSnapshot(): atomic snapshot of all counters
// - Pause time histogram: 0-1ms / 1-5ms / 5-10ms / 10-50ms / 50-100ms / 100+ms
// - Ring buffer: last 64 GC events (young + full)
// ======================================================================

struct GcStats {
    // ── Young collection ──────────────────────────────────────────
    std::atomic<uint64_t> young_collections{0};
    std::atomic<uint64_t> young_objects_promoted{0};
    std::atomic<uint64_t> young_bytes_promoted{0};
    std::atomic<uint64_t> young_bytes_reclaimed{0};
    std::atomic<uint64_t> young_cards_scanned{0};
    std::atomic<uint64_t> young_pause_ns{0};

    // ── Young GC phase-level timing (cumulative ns across all young GCs) ──
    std::atomic<uint64_t> young_phase1_ns{0};
    std::atomic<uint64_t> young_phase2_ns{0};
    std::atomic<uint64_t> young_phase2b_ns{0};
    std::atomic<uint64_t> young_phase3_ns{0};
    std::atomic<uint64_t> young_phase3b_ns{0};
    std::atomic<uint64_t> young_phase4_ns{0};

    // ── Full collection ───────────────────────────────────────────
    std::atomic<uint64_t> full_collections{0};
    std::atomic<uint64_t> full_pages_collected{0};
    std::atomic<uint64_t> full_objects_marked{0};
    std::atomic<uint64_t> full_bytes_reclaimed{0};
    std::atomic<uint64_t> full_finalizers_run{0};
    std::atomic<CHAOS_IL2CPP_SIZE> finalization_pending_count{0};
    std::atomic<uint64_t> full_pause_ns{0};

    // ── Full GC phase-level timing (cumulative ns across all full GCs) ──
    std::atomic<uint64_t> full_mark_ns{0};
    std::atomic<uint64_t> full_sweep_ns{0};
    std::atomic<uint64_t> full_compact_ns{0};

    // ── Gen1 collection ───────────────────────────────────────────
    std::atomic<uint64_t> gen1_collections{0};
    std::atomic<uint64_t> gen1_objects_promoted{0};
    std::atomic<uint64_t> gen1_bytes_promoted{0};
    std::atomic<uint64_t> gen1_bytes_reclaimed{0};
    std::atomic<uint64_t> gen1_pause_ns{0};

    // ── Last-GC metadata (for GCMemoryInfo) ──────────────────────
    std::atomic<int32_t> last_compacted{0};
    std::atomic<int32_t> last_concurrent{0};
    std::atomic<int32_t> last_gc_generation{1};  // 0=young, 1=gen1, 2=full

    // ── GC sequence number (monotonically increasing) ────────────
    std::atomic<uint64_t> gc_index{0};

    // ── Allocation (process-wide aggregate) ───────────────────────
    std::atomic<uint64_t> alloc_total{0};
    std::atomic<uint64_t> alloc_bytes{0};
    std::atomic<uint64_t> alloc_oversized{0};
};

extern GcStats g_gc_stats;

/// Process start time (steady clock) for PauseTimePercentage computation.
/// Set once at module load in gc_stats.cpp.
extern const std::chrono::steady_clock::time_point g_gc_start_time;

// ── Pause time histogram buckets (in nanoseconds) ───────────────

/// Histogram bucket boundaries (inclusive upper bound, nanoseconds).
static constexpr uint64_t kGcPauseBucketsNs[] = {
    1'000'000,       // 0-1 ms
    5'000'000,       // 1-5 ms
    10'000'000,      // 5-10 ms
    50'000'000,      // 10-50 ms
    100'000'000,     // 50-100 ms
    UINT64_MAX       // 100+ ms
};
static constexpr int kGcPauseBucketCount = 6;

extern std::atomic<uint64_t> g_gc_pause_histogram[kGcPauseBucketCount];

/// Record a pause time into the histogram.
inline void GcRecordPauseHistogram(uint64_t pause_ns) noexcept {
    for (int i = 0; i < kGcPauseBucketCount; i++) {
        if (pause_ns <= kGcPauseBucketsNs[i]) {
            g_gc_pause_histogram[i].fetch_add(1, std::memory_order_relaxed);
            break;
        }
    }
}

// ── GC event ring buffer ───────────────────────────────────────

/// Maximum events in the ring buffer.
static constexpr int kGcEventRingSize = 64;

/// One ring buffer entry.
struct GcEventEntry {
    bool     is_full_gc;       ///< true = full GC, false = young GC
    uint64_t pause_ns;         ///< Pause time in nanoseconds
    uint64_t objects_processed;///< Objects promoted (young) or marked (full)
    uint64_t bytes_reclaimed;  ///< Bytes freed
};

/// Ring buffer head index (atomic, wraps at kGcEventRingSize).
extern std::atomic<int> g_gc_event_ring_head;

/// Ring buffer entries.
extern GcEventEntry g_gc_event_ring[kGcEventRingSize];

/// Record an event in the ring buffer.
inline void GcRecordEventRing(bool is_full_gc, uint64_t pause_ns,
                               uint64_t objects_processed, uint64_t bytes_reclaimed) noexcept {
    int head = g_gc_event_ring_head.fetch_add(1, std::memory_order_relaxed);
    int idx = head % kGcEventRingSize;
    g_gc_event_ring[idx].is_full_gc = is_full_gc;
    g_gc_event_ring[idx].pause_ns = pause_ns;
    g_gc_event_ring[idx].objects_processed = objects_processed;
    g_gc_event_ring[idx].bytes_reclaimed = bytes_reclaimed;
}

// ── Snapshot structure ─────────────────────────────────────────

/// Atomic snapshot of all GC counters (returned by GcGetSnapshot).
struct GcSnapshot {
    // Counters
    uint64_t young_collections;
    uint64_t full_collections;
    uint64_t young_objects_promoted;
    uint64_t young_bytes_promoted;
    uint64_t young_bytes_reclaimed;
    uint64_t young_cards_scanned;
    // Young GC phase cumulative ns (diagnostic granularity).
    uint64_t young_phase1_ns;
    uint64_t young_phase2_ns;
    uint64_t young_phase2b_ns;
    uint64_t young_phase3_ns;
    uint64_t young_phase3b_ns;
    uint64_t young_phase4_ns;
    uint64_t full_pages_collected;
    uint64_t full_objects_marked;
    uint64_t full_bytes_reclaimed;
    uint64_t full_finalizers_run;
    int32_t finalization_pending_count;
    // Full GC phase cumulative ns (diagnostic granularity).
    uint64_t full_mark_ns;
    uint64_t full_sweep_ns;
    uint64_t full_compact_ns;
    uint64_t alloc_total;
    uint64_t alloc_bytes;
    uint64_t alloc_oversized;

    // ── Gen1 ──
    uint64_t gen1_collections;
    uint64_t gen1_objects_promoted;
    uint64_t gen1_bytes_promoted;
    uint64_t gen1_bytes_reclaimed;

    // Derived
    uint64_t young_pause_ns_total;
    uint64_t full_pause_ns_total;
    uint64_t young_pause_ns_avg;
    uint64_t full_pause_ns_avg;
    uint64_t gen1_pause_ns_total;
    uint64_t gen1_pause_ns_avg;
    uint64_t gc_index;
    int32_t last_gc_generation;

    /// Reason the most recent full GC was triggered (see GcTriggerReason).
    int32_t last_trigger_reason;

    // Histogram
    uint64_t pause_histogram[kGcPauseBucketCount];
};

/// Take an atomic snapshot of all GC counters.
GcSnapshot GcGetSnapshot() noexcept;

// ── GCMemoryInfo snapshot (managed GC.GetGCMemoryInfo() data) ──

/// Mirrors the framework pack System.GCMemoryInfoData field layout.
/// Populated by chaos_gc_get_memory_info().  The native function receives
/// the managed object reference and computes the interior pointer
/// (past the MethodTable*) so the write goes directly to offset 0.
struct GcMemoryInfoNative {
    int64_t high_memory_load_threshold_bytes;  // 0
    int64_t total_available_memory_bytes;       // 8
    int64_t memory_load_bytes;                  // 16
    int64_t heap_size_bytes;                    // 24
    int64_t fragmented_bytes;                   // 32
    int64_t total_committed_bytes;              // 40
    int64_t promoted_bytes;                     // 48
    int64_t pinned_objects_count;               // 56  — always 0
    int64_t finalization_pending_count;         // 64
    int64_t index;                              // 72  — always 0
    int32_t generation;                         // 80
    int32_t pause_time_percentage;              // 84  — always 0
    uint8_t compacted;                          // 88
    uint8_t concurrent;                         // 89
    uint8_t _generationInfo_padding[6];         // 90-95 — pad to 96

    // ── _generationInfo inline array (5 × GCGenerationInfo, 32 bytes each = 160 bytes) ──
    // Matches the BCL GCMemoryInfoData._generationInfo field layout.
    // Each entry: SizeBeforeBytes(0), SizeAfterBytes(8), FragBeforeBytes(16), FragAfterBytes(24)
    int64_t gen0_size_before;                   // 96
    int64_t gen0_size_after;                    // 104
    int64_t gen0_frag_before;                   // 112
    int64_t gen0_frag_after;                    // 120
    int64_t gen1_size_before;                   // 128
    int64_t gen1_size_after;                    // 136
    int64_t gen1_frag_before;                   // 144
    int64_t gen1_frag_after;                    // 152
    int64_t gen2_size_before;                   // 160
    int64_t gen2_size_after;                    // 168
    int64_t gen2_frag_before;                   // 176
    int64_t gen2_frag_after;                    // 184
    int64_t gen3_size_before;                   // 192  — always 0 (no gen3 in CRAG)
    int64_t gen3_size_after;                    // 200
    int64_t gen3_frag_before;                   // 208
    int64_t gen3_frag_after;                    // 216
    int64_t loh_size_before;                    // 224
    int64_t loh_size_after;                     // 232
    int64_t loh_frag_before;                    // 240
    int64_t loh_frag_after;                     // 248
};  // 256 bytes

// ── Inline record helpers (hot-path, header for max inlining) ─────

inline void GcRecordYoungCollection(
    CHAOS_IL2CPP_SIZE objects_promoted,
    CHAOS_IL2CPP_SIZE bytes_promoted,
    CHAOS_IL2CPP_SIZE bytes_reclaimed,
    CHAOS_IL2CPP_SIZE cards_scanned,
    uint64_t pause_ns) noexcept
{
    g_gc_stats.young_collections.fetch_add(1, std::memory_order_relaxed);
    g_gc_stats.young_objects_promoted.fetch_add(objects_promoted, std::memory_order_relaxed);
    g_gc_stats.young_bytes_promoted.fetch_add(bytes_promoted, std::memory_order_relaxed);
    g_gc_stats.young_bytes_reclaimed.fetch_add(bytes_reclaimed, std::memory_order_relaxed);
    g_gc_stats.young_cards_scanned.fetch_add(cards_scanned, std::memory_order_relaxed);
    g_gc_stats.young_pause_ns.fetch_add(pause_ns, std::memory_order_relaxed);
    g_gc_stats.gc_index.fetch_add(1, std::memory_order_relaxed);
    g_gc_stats.last_gc_generation.store(0, std::memory_order_relaxed);
    GcRecordPauseHistogram(pause_ns);
    GcRecordEventRing(false, pause_ns, objects_promoted, bytes_reclaimed);
}

/// Accumulate young-GC phase-level pause times into GcStats (diagnostic
/// granularity).  Called once per young GC alongside GcRecordYoungCollection;
/// the phase values are already computed in the collector's PhaseTime struct.
/// Kept separate from the hot GcRecordYoungCollection to avoid adding
/// cache-line pressure to that path.
inline void GcRecordYoungPhaseTimes(
    uint64_t phase1_ns, uint64_t phase2_ns, uint64_t phase2b_ns,
    uint64_t phase3_ns, uint64_t phase3b_ns, uint64_t phase4_ns) noexcept
{
    g_gc_stats.young_phase1_ns.fetch_add(phase1_ns, std::memory_order_relaxed);
    g_gc_stats.young_phase2_ns.fetch_add(phase2_ns, std::memory_order_relaxed);
    g_gc_stats.young_phase2b_ns.fetch_add(phase2b_ns, std::memory_order_relaxed);
    g_gc_stats.young_phase3_ns.fetch_add(phase3_ns, std::memory_order_relaxed);
    g_gc_stats.young_phase3b_ns.fetch_add(phase3b_ns, std::memory_order_relaxed);
    g_gc_stats.young_phase4_ns.fetch_add(phase4_ns, std::memory_order_relaxed);
}

inline void GcRecordFullCollection(
    CHAOS_IL2CPP_SIZE pages_collected,
    CHAOS_IL2CPP_SIZE objects_marked,
    CHAOS_IL2CPP_SIZE bytes_reclaimed,
    CHAOS_IL2CPP_SIZE finalizers_run,
    uint64_t pause_ns,
    int32_t compacted = 0,
    int32_t concurrent = 0,
    uint64_t mark_ns = 0,
    uint64_t sweep_ns = 0,
    uint64_t compact_ns = 0) noexcept
{
    g_gc_stats.full_collections.fetch_add(1, std::memory_order_relaxed);
    g_gc_stats.full_pages_collected.fetch_add(pages_collected, std::memory_order_relaxed);
    g_gc_stats.full_objects_marked.fetch_add(objects_marked, std::memory_order_relaxed);
    g_gc_stats.full_bytes_reclaimed.fetch_add(bytes_reclaimed, std::memory_order_relaxed);
    g_gc_stats.full_finalizers_run.fetch_add(finalizers_run, std::memory_order_relaxed);
    g_gc_stats.full_pause_ns.fetch_add(pause_ns, std::memory_order_relaxed);
    g_gc_stats.full_mark_ns.fetch_add(mark_ns, std::memory_order_relaxed);
    g_gc_stats.full_sweep_ns.fetch_add(sweep_ns, std::memory_order_relaxed);
    g_gc_stats.full_compact_ns.fetch_add(compact_ns, std::memory_order_relaxed);
    g_gc_stats.last_compacted.store(compacted, std::memory_order_relaxed);
    g_gc_stats.last_concurrent.store(concurrent, std::memory_order_relaxed);
    g_gc_stats.gc_index.fetch_add(1, std::memory_order_relaxed);
    g_gc_stats.last_gc_generation.store(2, std::memory_order_relaxed);
    GcRecordPauseHistogram(pause_ns);
    GcRecordEventRing(true, pause_ns, objects_marked, bytes_reclaimed);
}

inline void GcRecordGen1Collection(
    CHAOS_IL2CPP_SIZE objects_promoted,
    CHAOS_IL2CPP_SIZE bytes_promoted,
    CHAOS_IL2CPP_SIZE bytes_reclaimed,
    uint64_t pause_ns) noexcept
{
    g_gc_stats.gen1_collections.fetch_add(1, std::memory_order_relaxed);
    g_gc_stats.gen1_objects_promoted.fetch_add(objects_promoted, std::memory_order_relaxed);
    g_gc_stats.gen1_bytes_promoted.fetch_add(bytes_promoted, std::memory_order_relaxed);
    g_gc_stats.gen1_bytes_reclaimed.fetch_add(bytes_reclaimed, std::memory_order_relaxed);
    g_gc_stats.gen1_pause_ns.fetch_add(pause_ns, std::memory_order_relaxed);
    g_gc_stats.gc_index.fetch_add(1, std::memory_order_relaxed);
    g_gc_stats.last_gc_generation.store(1, std::memory_order_relaxed);
    GcRecordPauseHistogram(pause_ns);
    GcRecordEventRing(false, pause_ns, objects_promoted, bytes_reclaimed);
}

inline void GcRecordAlloc(CHAOS_IL2CPP_SIZE bytes, bool oversized) noexcept {
    g_gc_stats.alloc_total.fetch_add(1, std::memory_order_relaxed);
    g_gc_stats.alloc_bytes.fetch_add(bytes, std::memory_order_relaxed);
    if (oversized) {
        g_gc_stats.alloc_oversized.fetch_add(1, std::memory_order_relaxed);
    }
#if defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
    GcEtwRecordAlloc(bytes);
#endif
}

// ── Dump ─────────────────────────────────────────────────────────

void GcDumpStats() noexcept;

/// Start the periodic GC-diagnostics dump thread if DumpStatsIntervalSec > 0.
/// No-op if interval is 0 (default) or the thread is already running.
/// Called once from InitYoungGeneration.  The thread reads only atomic
/// counters and never holds GC locks.
void StartGcPeriodicDumpThread() noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_STATS_H_