#include "gc_stats.h"
#include "gc_region.h"

#include <chaos/log.h>

namespace chaos::il2cpp::runtime_core {

GcStats g_gc_stats;

void GcDumpStats() noexcept {
    // ── Young collection ──────────────────────────────────────────
    uint64_t young_count = g_gc_stats.young_collections.load(std::memory_order_relaxed);
    if (young_count > 0) {
        uint64_t total_ns = g_gc_stats.young_pause_ns.load(std::memory_order_relaxed);
        uint64_t avg_ns = young_count > 0 ? total_ns / young_count : 0;
        CHAOS_IL2CPP_LOG_WRITE_RAW_M(
            "GC|young|collections={0}|promoted_objects={1}|promoted_bytes={2}"
            "|reclaimed={3}|cards={4}|total_pause_ns={5}|avg_pause_ns={6}\n",
            young_count,
            g_gc_stats.young_objects_promoted.load(std::memory_order_relaxed),
            g_gc_stats.young_bytes_promoted.load(std::memory_order_relaxed),
            g_gc_stats.young_bytes_reclaimed.load(std::memory_order_relaxed),
            g_gc_stats.young_cards_scanned.load(std::memory_order_relaxed),
            total_ns,
            avg_ns);
    }

    // ── Full collection ───────────────────────────────────────────
    uint64_t full_count = g_gc_stats.full_collections.load(std::memory_order_relaxed);
    if (full_count > 0) {
        uint64_t total_ns = g_gc_stats.full_pause_ns.load(std::memory_order_relaxed);
        uint64_t avg_ns = full_count > 0 ? total_ns / full_count : 0;
        CHAOS_IL2CPP_LOG_WRITE_RAW_M(
            "GC|full|collections={0}|pages={1}|marked={2}|reclaimed={3}"
            "|finalizers={4}|total_pause_ns={5}|avg_pause_ns={6}\n",
            full_count,
            g_gc_stats.full_pages_collected.load(std::memory_order_relaxed),
            g_gc_stats.full_objects_marked.load(std::memory_order_relaxed),
            g_gc_stats.full_bytes_reclaimed.load(std::memory_order_relaxed),
            g_gc_stats.full_finalizers_run.load(std::memory_order_relaxed),
            total_ns,
            avg_ns);
    }

    // ── Allocation ────────────────────────────────────────────────
    uint64_t alloc_total = g_gc_stats.alloc_total.load(std::memory_order_relaxed);
    if (alloc_total > 0) {
        CHAOS_IL2CPP_LOG_WRITE_RAW_M(
            "GC|alloc|total={0}|bytes={1}|oversized={2}\n",
            alloc_total,
            g_gc_stats.alloc_bytes.load(std::memory_order_relaxed),
            g_gc_stats.alloc_oversized.load(std::memory_order_relaxed));
    }

    // ── Region (sampled live from RegionManager) ──────────────────
    auto& rm = RegionManager::Instance();
    CHAOS_IL2CPP_LOG_WRITE_RAW_M(
        "GC|region|active={0}|total_allocated={1}\n",
        rm.ActiveRegionCount(),
        rm.TotalAllocatedBytes());
}

}  // namespace chaos::il2cpp::runtime_core