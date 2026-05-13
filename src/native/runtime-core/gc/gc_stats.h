#ifndef CHAOS_IL2CPP_GC_STATS_H_
#define CHAOS_IL2CPP_GC_STATS_H_

#include <chaos/native_types.h>

#include <atomic>
#include <cstdint>

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// GcStats — process-wide GC statistics accumulator
//
// All counters are std::atomic<uint64_t> with memory_order_relaxed for
// thread-safe, low-contention recording.  GcDumpStats() emits GC|-prefixed
// lines to stdout at process exit (see gc_stats.cpp).
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
    std::atomic<uint64_t> full_pause_ns{0};

    // ── Allocation (process-wide aggregate) ───────────────────────
    std::atomic<uint64_t> alloc_total{0};
    std::atomic<uint64_t> alloc_bytes{0};
    std::atomic<uint64_t> alloc_oversized{0};
};

extern GcStats g_gc_stats;

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