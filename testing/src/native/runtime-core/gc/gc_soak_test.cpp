/// gc_soak_test.cpp — CRAG GC Arrhenius 72h soak test (G-17)
///
/// Long-running stability test that cycles through allocation phases,
/// takes periodic GcGetSnapshot measurements, and detects performance
/// degradation trends over extended execution.
///
/// Design:
///   - Cycles through 5 phases: AllocBurst → GCHeavy → Mixed → Idle → Promo
///   - Takes a GcSnapshot every kSnapshotIntervalMs and appends to JSONL
///   - Every kCheckInterval snapshots, runs degradation checks:
///       > Pause time trending up (regression slope > threshold)
///       > Free memory not recovering (high watermark not resetting)
///       > GC frequency increasing (more collections per unit work)
///       > Finalizer queue buildup
///   - Exits with error if any degradation check fails beyond tolerance
///   - Reports structured ARRHENIUS| lines and JSONL metrics file
///
/// Usage:
///   chaos_gc_soak_test [--minutes M] [--phases P] [--report file.jsonl]
///
/// Output:
///   ARRHENIUS|<phase>|<elapsed_s>|<metric>|<value>|<unit>
///   JSONL metrics file at artifacts/native-runtime-core-test/reports/

#include <chaos/native_types.h>

#include "gc_region.h"
#include "gc_young_collector.h"
#include "gc_old_gen.h"
#include "gc_stats.h"
#include "gc_api.h"
#include "gc_card_table.h"
#include "gc_gen1.h"
#include "thread_state.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <functional>
#include <thread>
#include <vector>

#include "gc_test_macros.h"

using namespace chaos::il2cpp::runtime_core;

static int g_failures = 0;
static int g_tests    = 0;
static int g_sub      = 0;

// ── ARRHENIUS output macro ───────────────────────────────────────
#define ARRHENIUS(phase, elapsed_s, metric, value, unit)                \
    printf("ARRHENIUS|%s|%lld|%s|%lld|%s\n",                            \
           (phase), static_cast<long long>(elapsed_s),                   \
           (metric), static_cast<long long>(value), (unit))

// ══════════════════════════════════════════════════════════════════════════
// Constants & config
// ══════════════════════════════════════════════════════════════════════════

static constexpr int kDefaultRunMinutes     = 10;   // default: 10 min
static constexpr int kSnapshotIntervalMs    = 5000; // snapshot every 5 s
static constexpr int kCheckInterval         = 12;   // degradation check every 12 snapshots = 60 s

// Phase durations (seconds)
static constexpr int kPhaseAllocBurstSec    = 15;
static constexpr int kPhaseGCHeavySec       = 10;
static constexpr int kPhaseMixedSec         = 20;
static constexpr int kPhaseIdleSec          = 5;
static constexpr int kPhasePromoSec         = 10;

static constexpr int kCycleSec = kPhaseAllocBurstSec + kPhaseGCHeavySec
                               + kPhaseMixedSec + kPhaseIdleSec + kPhasePromoSec;

static int g_run_minutes = kDefaultRunMinutes;

// ══════════════════════════════════════════════════════════════════════════
// Metrics ring — holds the last N snapshot-derived metrics for trend analysis
// ══════════════════════════════════════════════════════════════════════════

struct MetricsPoint {
    int64_t  elapsed_s;
    uint64_t young_avg_pause_ns;
    uint64_t full_avg_pause_ns;
    uint64_t gen1_avg_pause_ns;
    uint64_t young_collections;
    uint64_t full_collections;
    uint64_t gen1_collections;
    double   alloc_mb_s;        // MiB allocated per second since last point
    uint64_t young_freq;        // young collections since last point
    uint64_t finalizer_pending;
    uint64_t active_regions;
};

static constexpr int kMetricsRingSize = 120;   // 120 points × 5 s = 10 min window
static MetricsPoint g_ring[kMetricsRingSize];
static int g_ring_head = 0;
static int g_ring_count = 0;

static void PushMetric(const MetricsPoint& pt) {
    g_ring[g_ring_head] = pt;
    g_ring_head = (g_ring_head + 1) % kMetricsRingSize;
    if (g_ring_count < kMetricsRingSize) g_ring_count++;
}

static MetricsPoint GetMetric(int idx) {
    // idx 0 = newest, negative = older
    int offset = (g_ring_head - 1 - idx + kMetricsRingSize) % kMetricsRingSize;
    return g_ring[offset];
}

// ══════════════════════════════════════════════════════════════════════════
// JSONL report writer
// ══════════════════════════════════════════════════════════════════════════

static const char* kReportDir = "D:/agent/booming-il2cpp/artifacts/native-runtime-core-test/reports";
static char g_report_path[512] = {};
static std::FILE* g_report_file = nullptr;

static void OpenReport() {
#if defined(_WIN32) || defined(_WIN64)
    ::system(("if not exist \"" + std::string(kReportDir) + "\" mkdir \"" + std::string(kReportDir) + "\"").c_str());
#endif
    std::time_t now = std::time(nullptr);
    char ts_buf[64];
    std::strftime(ts_buf, sizeof(ts_buf), "%Y%m%dT%H%M%S", std::gmtime(&now));
    std::snprintf(g_report_path, sizeof(g_report_path),
                  "%s/gc_soak_%s.jsonl", kReportDir, ts_buf);
    g_report_file = std::fopen(g_report_path, "w");
    if (g_report_file) {
        std::fprintf(g_report_file, "{\"event\":\"soak_start\",\"duration_min\":%d}\n",
                     g_run_minutes);
    }
}

static void LogSnapshotJson(const char* phase, int64_t elapsed_s,
                             const GcSnapshot& snap, const MetricsPoint& pt) {
    if (!g_report_file) return;

    double young_avg_us = snap.young_collections > 0
        ? snap.young_pause_ns_total / 1000.0 / snap.young_collections : 0.0;
    double full_avg_us  = snap.full_collections > 0
        ? snap.full_pause_ns_total / 1000.0 / snap.full_collections : 0.0;
    double gen1_avg_us  = snap.gen1_collections > 0
        ? snap.gen1_pause_ns_total / 1000.0 / snap.gen1_collections : 0.0;

    std::fprintf(g_report_file,
        "{"
        "\"event\":\"snapshot\","
        "\"elapsed_s\":%lld,"
        "\"phase\":\"%s\","
        "\"young_collections\":%llu,"
        "\"young_avg_pause_us\":%.1f,"
        "\"full_collections\":%llu,"
        "\"full_avg_pause_us\":%.1f,"
        "\"gen1_collections\":%llu,"
        "\"gen1_avg_pause_us\":%.1f,"
        "\"alloc_mb_s\":%.2f,"
        "\"young_freq\":%llu,"
        "\"finalizer_pending\":%llu,"
        "\"active_regions\":%llu"
        "}\n",
        static_cast<long long>(elapsed_s),
        phase,
        static_cast<unsigned long long>(snap.young_collections),
        young_avg_us,
        static_cast<unsigned long long>(snap.full_collections),
        full_avg_us,
        static_cast<unsigned long long>(snap.gen1_collections),
        gen1_avg_us,
        pt.alloc_mb_s,
        static_cast<unsigned long long>(pt.young_freq),
        static_cast<unsigned long long>(pt.finalizer_pending),
        static_cast<unsigned long long>(pt.active_regions));
}

static void LogCheckJson(bool passed, const char* check, double value,
                          double threshold) {
    if (!g_report_file) return;
    std::fprintf(g_report_file,
        "{\"event\":\"check\",\"check\":\"%s\",\"passed\":%s,"
        "\"value\":%.2f,\"threshold\":%.2f}\n",
        check, passed ? "true" : "false", value, threshold);
}

static void CloseReport(int passed, int failed_checks) {
    if (!g_report_file) return;
    std::fprintf(g_report_file,
        "{\"event\":\"soak_end\",\"checks_passed\":%d,\"checks_failed\":%d}\n",
        passed, failed_checks);
    std::fclose(g_report_file);
    g_report_file = nullptr;
    printf("\nSoak report: %s\n", g_report_path);
}

// ══════════════════════════════════════════════════════════════════════════
// Cyclic phases
// ══════════════════════════════════════════════════════════════════════════

/// Phase 1: AllocBurst — rapid small+medium allocation
static void PhaseAllocBurst(int64_t elapsed_s, std::vector<void*>& keep) {
    ARRHENIUS("AllocBurst", elapsed_s, "start", 0, "");
    constexpr int kAllocs = 50000;
    for (int i = 0; i < kAllocs; i++) {
        size_t size = (i % 3 == 0) ? 512 : 32;
        void* p = NurseryAllocate(size);
        if (!p) break;
        if (i % 100 == 0 && keep.size() < 200) {
            keep.push_back(p);
        }
    }
    ARRHENIUS("AllocBurst", elapsed_s, "end", kAllocs, "allocs");
}

/// Phase 2: GCHeavy — trigger GCs actively
static void PhaseGCHeavy(int64_t elapsed_s) {
    ARRHENIUS("GCHeavy", elapsed_s, "start", 0, "");
    // Allocate and discard rapidly to trigger young GCs.
    constexpr int kRounds = 20;
    for (int r = 0; r < kRounds; r++) {
        for (int i = 0; i < 5000; i++) {
            void* p = NurseryAllocate(128);
            if (!p) break;
        }
    }
    // Trigger a full GC to stress full collection path.
    g_old_gen.Collect(nullptr, nullptr);
    ARRHENIUS("GCHeavy", elapsed_s, "end", kRounds, "rounds");
}

/// Phase 3: Mixed — interleaved allocation patterns + survivors
static void PhaseMixed(int64_t elapsed_s, std::vector<void*>& keep) {
    ARRHENIUS("Mixed", elapsed_s, "start", 0, "");
    constexpr int kIterations = 30000;
    for (int i = 0; i < kIterations; i++) {
        size_t size = (i % 5 == 0) ? 1024 : 64;
        void* p = NurseryAllocate(size);
        if (!p) continue;
        // Keep ~500 objects alive to create survivor pressure.
        if (i % 60 == 0 && keep.size() < 500) {
            keep.push_back(p);
        }
    }
    ARRHENIUS("Mixed", elapsed_s, "end", kIterations, "allocs");
}

/// Phase 4: Idle — minimal allocation, let GC catch up
static void PhaseIdle(int64_t elapsed_s) {
    ARRHENIUS("Idle", elapsed_s, "start", 0, "");
    // Just a few allocations to keep the thread active.
    for (int i = 0; i < 100; i++) {
        NurseryAllocate(16);
    }
    ARRHENIUS("Idle", elapsed_s, "end", 0, "");
}

/// Phase 5: Promo — old-gen allocation to exercise promotion + full GC
static void PhasePromo(int64_t elapsed_s) {
    ARRHENIUS("Promo", elapsed_s, "start", 0, "");
    constexpr int kAllocs = 2000;
    for (int i = 0; i < kAllocs; i++) {
        void* p = g_old_gen.Allocate(256, false);
        if (!p) break;
    }
    ARRHENIUS("Promo", elapsed_s, "end", kAllocs, "allocs");
}

// ══════════════════════════════════════════════════════════════════════════
// Degradation checks
// ══════════════════════════════════════════════════════════════════════════

/// Simple linear regression slope over the last N metrics points.
/// Returns the slope of `get_y(i)` over x = 0..N-1.
static double ComputeSlope(int n, std::function<double(int)> get_y) {
    if (n < 3) return 0.0;
    double sum_x = 0.0, sum_y = 0.0, sum_xy = 0.0, sum_xx = 0.0;
    for (int i = 0; i < n; i++) {
        double x = static_cast<double>(i);
        double y = get_y(i);
        sum_x  += x;
        sum_y  += y;
        sum_xy += x * y;
        sum_xx += x * x;
    }
    double denom = n * sum_xx - sum_x * sum_x;
    if (std::fabs(denom) < 1e-9) return 0.0;
    return (n * sum_xy - sum_x * sum_y) / denom;
}

static int RunDegradationChecks(int64_t elapsed_s) {
    int failed = 0;
    int n = std::min(g_ring_count, 10);  // last 10 points (~50 s window)

    // Check 1: Young GC pause trending up
    double young_slope = ComputeSlope(n, [](int i) {
        return static_cast<double>(GetMetric(i).young_avg_pause_ns);
    });
    // Threshold: slope > 500 ns/sample (500 ns increase per 5 s interval)
    if (young_slope > 500.0) {
        ARRHENIUS("CHECK", elapsed_s, "young_pause_trend", young_slope, "ns/sample");
        GC_CHECK(young_slope <= 500.0,
                 "Young pause slope %.0f ns/sample (threshold: 500)", young_slope);
        failed++;
    } else {
        LogCheckJson(true, "young_pause_trend", young_slope, 500.0);
    }

    // Check 2: GC frequency increasing (more young collections per interval)
    double freq_slope = ComputeSlope(n, [](int i) {
        return static_cast<double>(GetMetric(i).young_freq);
    });
    if (freq_slope > 10.0) {
        ARRHENIUS("CHECK", elapsed_s, "gc_freq_trend", freq_slope, "collections/sample");
        GC_CHECK(freq_slope <= 10.0,
                 "GC freq slope %.1f cols/sample (threshold: 10)", freq_slope);
        failed++;
    } else {
        LogCheckJson(true, "gc_freq_trend", freq_slope, 10.0);
    }

    // Check 3: Finalizer pending count not growing unbounded
    uint64_t max_pending = 0;
    for (int i = 0; i < n; i++) {
        max_pending = std::max(max_pending, GetMetric(i).finalizer_pending);
    }
    if (max_pending > 10000) {
        ARRHENIUS("CHECK", elapsed_s, "finalizer_pending", max_pending, "objects");
        GC_CHECK(max_pending <= 10000,
                 "Finalizer pending %llu (threshold: 10000)",
                 static_cast<unsigned long long>(max_pending));
        failed++;
    } else {
        LogCheckJson(true, "finalizer_pending", static_cast<double>(max_pending), 10000.0);
    }

    // Check 4: Active regions not growing monotonically (possible leak/leak)
    double region_slope = ComputeSlope(n, [](int i) {
        return static_cast<double>(GetMetric(i).active_regions);
    });
    if (region_slope > 5.0) {
        ARRHENIUS("CHECK", elapsed_s, "region_growth", region_slope, "regions/sample");
        GC_CHECK(region_slope <= 5.0,
                 "Region growth slope %.1f regions/sample (threshold: 5)", region_slope);
        failed++;
    } else {
        LogCheckJson(true, "region_growth", region_slope, 5.0);
    }

    // Check 5: Full GC pause not degrading
    double full_slope = ComputeSlope(n, [](int i) {
        return static_cast<double>(GetMetric(i).full_avg_pause_ns);
    });
    if (full_slope > 2000.0) {
        ARRHENIUS("CHECK", elapsed_s, "full_pause_trend", full_slope, "ns/sample");
        GC_CHECK(full_slope <= 2000.0,
                 "Full pause slope %.0f ns/sample (threshold: 2000)", full_slope);
        failed++;
    } else {
        LogCheckJson(true, "full_pause_trend", full_slope, 2000.0);
    }

    return failed;
}

// ══════════════════════════════════════════════════════════════════════════
// Main soak loop
// ══════════════════════════════════════════════════════════════════════════

static const char* kPhaseNames[] = {
    "AllocBurst", "GCHeavy", "Mixed", "Idle", "Promo"
};
static constexpr int kPhaseCount = 5;

/// Phase durations (nanoseconds)
static constexpr int64_t PhaseDurationNs(int phase_idx) {
    switch (phase_idx) {
        case 0: return static_cast<int64_t>(kPhaseAllocBurstSec) * 1000000000LL;
        case 1: return static_cast<int64_t>(kPhaseGCHeavySec) * 1000000000LL;
        case 2: return static_cast<int64_t>(kPhaseMixedSec) * 1000000000LL;
        case 3: return static_cast<int64_t>(kPhaseIdleSec) * 1000000000LL;
        case 4: return static_cast<int64_t>(kPhasePromoSec) * 1000000000LL;
        default: return 1000000000LL;
    }
}

/// Execute the active phase.  Called exactly once per phase transition.
static void ExecutePhase(int phase_idx, int64_t elapsed_s, int cycle,
                          std::vector<void*>& keep)
{
    ARRHENIUS("CYCLE", elapsed_s, "cycle_start", cycle, "");
    switch (phase_idx) {
        case 0: PhaseAllocBurst(elapsed_s, keep); break;
        case 1: PhaseGCHeavy(elapsed_s);           break;
        case 2: PhaseMixed(elapsed_s, keep);       break;
        case 3: PhaseIdle(elapsed_s);              break;
        case 4: PhasePromo(elapsed_s);             break;
    }

    // Free some kept objects every 3 cycles for memory churn.
    if (!keep.empty() && (cycle % 3) == 0 && phase_idx == 4) {
        size_t free_count = std::min(keep.size(), size_t(100));
        keep.resize(keep.size() - free_count);
    }
}

// ══════════════════════════════════════════════════════════════════════════
// Main
// ══════════════════════════════════════════════════════════════════════════

int main(int argc, char** argv) {
    // Parse --minutes argument.
    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "--minutes") == 0 && i + 1 < argc) {
            g_run_minutes = std::atoi(argv[++i]);
            if (g_run_minutes < 1) g_run_minutes = 1;
            if (g_run_minutes > 4320) g_run_minutes = 4320;  // max 72h
        }
    }

    puts("CRAG GC Arrhenius Soak Test (G-17)");
    puts("═══════════════════════════════════\n");
    printf("  Duration: %d min\n", g_run_minutes);
    printf("  Cycle:    %d s/phase × 5 %s\n", kCycleSec / kPhaseCount, "phases");
    printf("  Check:    every %d snapshots\n", kCheckInterval);
    printf("\n");

    // Init GC subsystems.
    InitYoungGeneration();
    void* warmup = NurseryAllocate(64);
    (void)warmup;
    threading::RegisterThread(threading::AllocateThreadId(), nullptr);
    threading::EnterCooperativeMode();

    // Open JSONL report.
    OpenReport();

    // Tracked allocations (simulate survivor objects across phases).
    std::vector<void*> keep;
    keep.reserve(1000);

    GcSnapshot prev_snap = GcGetSnapshot();
    uint64_t prev_alloc_bytes = g_gc_stats.alloc_bytes.load(std::memory_order_relaxed);
    int snapshot_count = 0;
    int total_checks_failed = 0;
    int total_checks_passed = 0;
    int cycle = 0;
    int phase_idx = 0;
    bool phase_executed = false;   // true once current phase has run

    auto t_start = std::chrono::steady_clock::now();
    auto t_next_snap = t_start + std::chrono::milliseconds(kSnapshotIntervalMs);
    auto t_next_phase_change = t_start + std::chrono::nanoseconds(PhaseDurationNs(0));
    int64_t total_run_ns = static_cast<int64_t>(g_run_minutes) * 60 * 1000000000LL;

    printf("  Phase  | Elapsed |  Snapshots  | Checks\n");
    printf("  -------+---------+-------------+-------\n");

    while (true) {
        auto t_now = std::chrono::steady_clock::now();
        int64_t elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
            t_now - t_start).count();

        if (elapsed_ns >= total_run_ns) break;

        int64_t elapsed_s = elapsed_ns / 1000000000LL;

        // ── Phase transition: execute current phase once ────────────
        if (!phase_executed) {
            ExecutePhase(phase_idx, elapsed_s, cycle, keep);
            phase_executed = true;
        }

        // ── Phase timeout: advance to next phase ────────────────────
        if (elapsed_ns >= std::chrono::duration_cast<std::chrono::nanoseconds>(
                t_next_phase_change - t_start).count()) {
            phase_idx = (phase_idx + 1) % kPhaseCount;
            if (phase_idx == 0) cycle++;
            phase_executed = false;
            t_next_phase_change = t_now + std::chrono::nanoseconds(
                PhaseDurationNs(phase_idx));
        }

        // ── Snapshot interval ───────────────────────────────────────
        if (t_now >= t_next_snap) {
            GcSnapshot snap = GcGetSnapshot();
            uint64_t curr_alloc_bytes = g_gc_stats.alloc_bytes.load(std::memory_order_relaxed);
            uint64_t alloc_delta = curr_alloc_bytes > prev_alloc_bytes
                ? curr_alloc_bytes - prev_alloc_bytes : 0;
            double elapsed_snap_s = static_cast<double>(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    t_now - t_start).count()) / 1000.0;
            double alloc_mb_s = alloc_delta / (1024.0 * 1024.0) /
                (kSnapshotIntervalMs / 1000.0);

            uint64_t young_delta = snap.young_collections - prev_snap.young_collections;

            MetricsPoint pt;
            pt.elapsed_s = static_cast<int64_t>(elapsed_snap_s);
            pt.young_avg_pause_ns = snap.young_collections > 0
                ? snap.young_pause_ns_total / snap.young_collections : 0;
            pt.full_avg_pause_ns = snap.full_collections > 0
                ? snap.full_pause_ns_total / snap.full_collections : 0;
            pt.gen1_avg_pause_ns = snap.gen1_collections > 0
                ? snap.gen1_pause_ns_total / snap.gen1_collections : 0;
            pt.young_collections  = snap.young_collections;
            pt.full_collections   = snap.full_collections;
            pt.gen1_collections   = snap.gen1_collections;
            pt.alloc_mb_s         = alloc_mb_s;
            pt.young_freq         = young_delta;
            pt.finalizer_pending  = snap.finalization_pending_count;
            pt.active_regions     = RegionManager::Instance().ActiveRegionCount();

            PushMetric(pt);
            LogSnapshotJson(kPhaseNames[phase_idx], static_cast<int64_t>(elapsed_snap_s),
                           snap, pt);

            // Degradation check every kCheckInterval snapshots.
            if (snapshot_count > 0 && (snapshot_count % kCheckInterval) == 0) {
                int f = RunDegradationChecks(static_cast<int64_t>(elapsed_snap_s));
                total_checks_failed += f;
                total_checks_passed += (5 - f);
                printf("  %-7s | %7lld | %6d/%3dc | %d/%d\n",
                       kPhaseNames[phase_idx],
                       static_cast<long long>(elapsed_snap_s),
                       snapshot_count, cycle,
                       f, 5);
            } else {
                printf("  %-7s | %7lld | %6d/%3dc | -\n",
                       kPhaseNames[phase_idx],
                       static_cast<long long>(elapsed_snap_s),
                       snapshot_count, cycle);
            }

            prev_snap = snap;
            prev_alloc_bytes = curr_alloc_bytes;
            snapshot_count++;
            t_next_snap = t_now + std::chrono::milliseconds(kSnapshotIntervalMs);
        }

        // Sleep for a short interval instead of busy-waiting.
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    threading::UnregisterThread();

    auto t_end = std::chrono::steady_clock::now();
    double total_s = std::chrono::duration_cast<std::chrono::milliseconds>(
        t_end - t_start).count() / 1000.0;

    printf("\n══ Soak Results ══\n");
    printf("  Duration:       %.0f s (%.1f min)\n", total_s, total_s / 60.0);
    printf("  Cycles:         %d\n", cycle);
    printf("  Snapshots:      %d\n", snapshot_count);
    printf("  Checks passed:  %d\n", total_checks_passed);
    printf("  Checks failed:  %d\n", total_checks_failed);

    CloseReport(total_checks_passed, total_checks_failed);

    ARRHENIUS("SUMMARY", static_cast<int64_t>(total_s), "passed", total_checks_passed, "");
    ARRHENIUS("SUMMARY", static_cast<int64_t>(total_s), "failed", total_checks_failed, "");

    printf("\n══ Results: %d checks pass, %d checks fail ══\n",
           total_checks_passed, total_checks_failed);
    return total_checks_failed > 0 ? 1 : 0;
}
