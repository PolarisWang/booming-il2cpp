#include "gc_stats.h"
#include "gc_region.h"
#include "gc_heap.h"
#include "core/gc_alloc_stubs.h"
#include "gc_config.h"

#include <chaos/log.h>

#include <atomic>
#include <cstdlib>
#include <thread>

namespace chaos::il2cpp::runtime_core {

GcStats g_gc_stats;

// Process start time for pause_time_percentage computation.
// Initialized at module load (static init) before any GC activity.
const std::chrono::steady_clock::time_point g_gc_start_time =
    std::chrono::steady_clock::now();

// ── Pause time histogram ───────────────────────────────────────
std::atomic<uint64_t> g_gc_pause_histogram[kGcPauseBucketCount];

// ── GC event ring buffer ──────────────────────────────────────
std::atomic<int> g_gc_event_ring_head{0};
GcEventEntry g_gc_event_ring[kGcEventRingSize] = {};

// ── Snapshot ──────────────────────────────────────────────────

GcSnapshot GcGetSnapshot() noexcept {
    // Flush fast-path TLS counters before reading global stats.
    FlushTlsFastStats();
    GcSnapshot snap;

    // Read all counters with acquire semantics for causal consistency.
    snap.young_collections   = g_gc_stats.young_collections.load(std::memory_order_acquire);
    snap.full_collections    = g_gc_stats.full_collections.load(std::memory_order_acquire);
    snap.young_objects_promoted = g_gc_stats.young_objects_promoted.load(std::memory_order_acquire);
    snap.young_bytes_promoted   = g_gc_stats.young_bytes_promoted.load(std::memory_order_acquire);
    snap.young_bytes_reclaimed  = g_gc_stats.young_bytes_reclaimed.load(std::memory_order_acquire);
    snap.young_cards_scanned    = g_gc_stats.young_cards_scanned.load(std::memory_order_acquire);
    snap.young_phase1_ns   = g_gc_stats.young_phase1_ns.load(std::memory_order_acquire);
    snap.young_phase2_ns   = g_gc_stats.young_phase2_ns.load(std::memory_order_acquire);
    snap.young_phase2b_ns  = g_gc_stats.young_phase2b_ns.load(std::memory_order_acquire);
    snap.young_phase3_ns   = g_gc_stats.young_phase3_ns.load(std::memory_order_acquire);
    snap.young_phase3b_ns  = g_gc_stats.young_phase3b_ns.load(std::memory_order_acquire);
    snap.young_phase4_ns   = g_gc_stats.young_phase4_ns.load(std::memory_order_acquire);
    snap.full_pages_collected   = g_gc_stats.full_pages_collected.load(std::memory_order_acquire);
    snap.full_objects_marked    = g_gc_stats.full_objects_marked.load(std::memory_order_acquire);
    snap.full_bytes_reclaimed   = g_gc_stats.full_bytes_reclaimed.load(std::memory_order_acquire);
    snap.full_finalizers_run    = g_gc_stats.full_finalizers_run.load(std::memory_order_acquire);
    snap.finalization_pending_count = static_cast<int32_t>(
        g_gc_stats.finalization_pending_count.load(std::memory_order_acquire));
    snap.full_mark_ns    = g_gc_stats.full_mark_ns.load(std::memory_order_acquire);
    snap.full_sweep_ns   = g_gc_stats.full_sweep_ns.load(std::memory_order_acquire);
    snap.full_compact_ns = g_gc_stats.full_compact_ns.load(std::memory_order_acquire);
    snap.alloc_total     = g_gc_stats.alloc_total.load(std::memory_order_acquire);
    snap.alloc_bytes     = g_gc_stats.alloc_bytes.load(std::memory_order_acquire);
    snap.alloc_oversized = g_gc_stats.alloc_oversized.load(std::memory_order_acquire);

    // Gen1 counters.
    snap.gen1_collections      = g_gc_stats.gen1_collections.load(std::memory_order_acquire);
    snap.gen1_objects_promoted = g_gc_stats.gen1_objects_promoted.load(std::memory_order_acquire);
    snap.gen1_bytes_promoted   = g_gc_stats.gen1_bytes_promoted.load(std::memory_order_acquire);
    snap.gen1_bytes_reclaimed  = g_gc_stats.gen1_bytes_reclaimed.load(std::memory_order_acquire);

    // GC sequence number and last generation.
    snap.gc_index = g_gc_stats.gc_index.load(std::memory_order_acquire);
    snap.last_gc_generation = g_gc_stats.last_gc_generation.load(std::memory_order_acquire);
    snap.last_trigger_reason = static_cast<int32_t>(G_Scheduler().LastTriggerReason());

    // Derived pause totals.
    snap.young_pause_ns_total = g_gc_stats.young_pause_ns.load(std::memory_order_acquire);
    snap.full_pause_ns_total  = g_gc_stats.full_pause_ns.load(std::memory_order_acquire);
    snap.gen1_pause_ns_total  = g_gc_stats.gen1_pause_ns.load(std::memory_order_acquire);

    snap.young_pause_ns_avg = (snap.young_collections > 0)
        ? snap.young_pause_ns_total / snap.young_collections : 0;
    snap.full_pause_ns_avg = (snap.full_collections > 0)
        ? snap.full_pause_ns_total / snap.full_collections : 0;
    snap.gen1_pause_ns_avg = (snap.gen1_collections > 0)
        ? snap.gen1_pause_ns_total / snap.gen1_collections : 0;

    // Histogram snapshot.
    for (int i = 0; i < kGcPauseBucketCount; i++) {
        snap.pause_histogram[i] = g_gc_pause_histogram[i].load(std::memory_order_acquire);
    }

    return snap;
}

// Register atexit handler via a static initializer.
namespace {
    struct AtExitRegistrar {
        AtExitRegistrar() {
            std::atexit([]() {
                GcDumpStats();
            });
        }
    };
    static AtExitRegistrar s_atexit_registrar;
}

// Logging method for GcDumpStats output.
void GcDumpStats() noexcept {
    // Flush fast-path TLS counters before reading global stats.
    FlushTlsFastStats();
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

        // Phase-level timing profile (cumulative ns).
        CHAOS_IL2CPP_LOG_WRITE_RAW_M(
            "GC|young-phases|p1={0}|p2={1}|p2b={2}|p3={3}|p3b={4}|p4={5}\n",
            g_gc_stats.young_phase1_ns.load(std::memory_order_relaxed),
            g_gc_stats.young_phase2_ns.load(std::memory_order_relaxed),
            g_gc_stats.young_phase2b_ns.load(std::memory_order_relaxed),
            g_gc_stats.young_phase3_ns.load(std::memory_order_relaxed),
            g_gc_stats.young_phase3b_ns.load(std::memory_order_relaxed),
            g_gc_stats.young_phase4_ns.load(std::memory_order_relaxed));
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

        // Full-GC phase-level timing profile (cumulative ns).
        CHAOS_IL2CPP_LOG_WRITE_RAW_M(
            "GC|full-phases|mark={0}|sweep={1}|compact={2}\n",
            g_gc_stats.full_mark_ns.load(std::memory_order_relaxed),
            g_gc_stats.full_sweep_ns.load(std::memory_order_relaxed),
            g_gc_stats.full_compact_ns.load(std::memory_order_relaxed));
    }

    // ── Gen1 collection ───────────────────────────────────────────
    uint64_t gen1_count = g_gc_stats.gen1_collections.load(std::memory_order_relaxed);
    if (gen1_count > 0) {
        uint64_t total_ns = g_gc_stats.gen1_pause_ns.load(std::memory_order_relaxed);
        uint64_t avg_ns = gen1_count > 0 ? total_ns / gen1_count : 0;
        CHAOS_IL2CPP_LOG_WRITE_RAW_M(
            "GC|gen1|collections={0}|promoted_objects={1}|promoted_bytes={2}"
            "|reclaimed={3}|total_pause_ns={4}|avg_pause_ns={5}\n",
            gen1_count,
            g_gc_stats.gen1_objects_promoted.load(std::memory_order_relaxed),
            g_gc_stats.gen1_bytes_promoted.load(std::memory_order_relaxed),
            g_gc_stats.gen1_bytes_reclaimed.load(std::memory_order_relaxed),
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

// ── Periodic dump thread ──────────────────────────────────────────
// Spawned on first GC when DumpStatsIntervalSec > 0.  Calls GcDumpStats
// at the configured interval, reading only atomic counters (no locks).
// The thread is preemptive-safe: it never holds GC locks.
namespace {
// (file-local state; all symbols below live inside chaos::il2cpp::runtime_core)
using namespace chaos::il2cpp::runtime_core;

static std::atomic<bool> g_dump_thread_stop{false};
static std::thread g_dump_thread_impl;

void DumpThreadLoop() noexcept {
    uint64_t interval_ns = static_cast<uint64_t>(
        GcConfig().DumpStatsIntervalSec) * 1'000'000'000ULL;
    if (interval_ns == 0) return;

    // Interleave between interval_ns/2 and interval_ns to avoid
    // aligning with system-wide periodic events (scheduling jitter).
    auto half = interval_ns / 2;

    while (!g_dump_thread_stop.load(std::memory_order_acquire)) {
        // Not holding GC lock — safe to call GcDumpStats.
        GcDumpStats();

        // Sleep in two phases: first half-interval, then re-check
        // the stop flag before the second half.  This ensures the
        // thread exits within half-interval + context switch delay
        // when the stop flag is set.
        std::this_thread::sleep_for(std::chrono::nanoseconds(half));
        if (g_dump_thread_stop.load(std::memory_order_acquire)) break;
        std::this_thread::sleep_for(std::chrono::nanoseconds(interval_ns - half));
    }
}
}  // anonymous namespace

namespace chaos::il2cpp::runtime_core {

void StartGcPeriodicDumpThread() noexcept {
    if (GcConfig().DumpStatsIntervalSec <= 0) return;
    // Atomic guard: only one thread starts it.
    static std::atomic<bool> started{false};
    bool expected = false;
    if (!started.compare_exchange_strong(expected, true,
            std::memory_order_acq_rel, std::memory_order_acquire)) {
        return;  // already started
    }
    g_dump_thread_stop.store(false, std::memory_order_release);
    g_dump_thread_impl = std::thread(DumpThreadLoop);
    g_dump_thread_impl.detach();
}

// Stop the periodic dump thread (called from the atexit registrar).
static void StopPeriodicDumpThread() noexcept {
    g_dump_thread_stop.store(true, std::memory_order_release);
    // Detached thread exits within half-interval; it only reads atomics.
}

}  // namespace chaos::il2cpp::runtime_core