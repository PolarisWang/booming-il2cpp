#ifndef CHAOS_IL2CPP_GC_STATS_H_
#define CHAOS_IL2CPP_GC_STATS_H_

#include <chaos/native_types.h>

#include <atomic>
#include <chrono>
#include <cstdint>

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

    // ── Full collection ───────────────────────────────────────────
    std::atomic<uint64_t> full_collections{0};
    std::atomic<uint64_t> full_pages_collected{0};
    std::atomic<uint64_t> full_objects_marked{0};
    std::atomic<uint64_t> full_bytes_reclaimed{0};
    std::atomic<uint64_t> full_finalizers_run{0};
    std::atomic<CHAOS_IL2CPP_SIZE> finalization_pending_count{0};
    std::atomic<uint64_t> full_pause_ns{0};

    // ── Allocation (process-wide aggregate) ───────────────────────
    std::atomic<uint64_t> alloc_total{0};
    std::atomic<uint64_t> alloc_bytes{0};
    std::atomic<uint64_t> alloc_oversized{0};
};

extern GcStats g_gc_stats;

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
    uint64_t full_pages_collected;
    uint64_t full_objects_marked;
    uint64_t full_bytes_reclaimed;
    uint64_t full_finalizers_run;
    int32_t finalization_pending_count;
    uint64_t alloc_total;
    uint64_t alloc_bytes;
    uint64_t alloc_oversized;

    // Derived
    uint64_t young_pause_ns_total;
    uint64_t full_pause_ns_total;
    uint64_t young_pause_ns_avg;
    uint64_t full_pause_ns_avg;

    // Histogram
    uint64_t pause_histogram[kGcPauseBucketCount];
};

/// Take an atomic snapshot of all GC counters.
GcSnapshot GcGetSnapshot() noexcept;

// ── GCMemoryInfo snapshot (managed GC.GetGCMemoryInfo() data) ──

/// Mirrors the key fields of System.GC.GCMemoryInfo (readonly struct).
/// Populated by chaos_gc_get_memory_info().  Not ABI-safe for direct return;
/// the codegen layer translates the fields into the managed struct layout.
struct GcMemoryInfoNative {
    int64_t high_memory_load_threshold_bytes;  ///< Threshold before GC considers memory high
    int64_t memory_load_bytes;                 ///< Current estimated memory load
    int64_t total_available_memory_bytes;      ///< Total available to the GC
    int64_t heap_size_bytes;                   ///< Current total managed heap size
    int64_t fragmented_bytes;                  ///< Memory lost to fragmentation
    int64_t total_committed_bytes;             ///< Total committed (not just heap)
    int64_t promoted_bytes;                    ///< Bytes promoted in last GC
    int32_t generation;                        ///< 0=young, 1=old, 2=LOH
    int32_t finalization_pending_count;        ///< Objects awaiting finalization
    int32_t compacted;                         ///< 1 if last GC was compacting
    int32_t concurrent;                        ///< 1 if last GC was concurrent (BGC)
};

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
    GcRecordPauseHistogram(pause_ns);
    GcRecordEventRing(false, pause_ns, objects_promoted, bytes_reclaimed);
}

inline void GcRecordFullCollection(
    CHAOS_IL2CPP_SIZE pages_collected,
    CHAOS_IL2CPP_SIZE objects_marked,
    CHAOS_IL2CPP_SIZE bytes_reclaimed,
    CHAOS_IL2CPP_SIZE finalizers_run,
    uint64_t pause_ns) noexcept
{
    g_gc_stats.full_collections.fetch_add(1, std::memory_order_relaxed);
    g_gc_stats.full_pages_collected.fetch_add(pages_collected, std::memory_order_relaxed);
    g_gc_stats.full_objects_marked.fetch_add(objects_marked, std::memory_order_relaxed);
    g_gc_stats.full_bytes_reclaimed.fetch_add(bytes_reclaimed, std::memory_order_relaxed);
    g_gc_stats.full_finalizers_run.fetch_add(finalizers_run, std::memory_order_relaxed);
    g_gc_stats.full_pause_ns.fetch_add(pause_ns, std::memory_order_relaxed);
    GcRecordPauseHistogram(pause_ns);
    GcRecordEventRing(true, pause_ns, objects_marked, bytes_reclaimed);
}

inline void GcRecordAlloc(CHAOS_IL2CPP_SIZE bytes, bool oversized) noexcept {
    g_gc_stats.alloc_total.fetch_add(1, std::memory_order_relaxed);
    g_gc_stats.alloc_bytes.fetch_add(bytes, std::memory_order_relaxed);
    if (oversized) {
        g_gc_stats.alloc_oversized.fetch_add(1, std::memory_order_relaxed);
    }
}

// ── Dump ─────────────────────────────────────────────────────────

void GcDumpStats() noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_STATS_H_