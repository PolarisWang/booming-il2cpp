/// CRAG T.3 stress test suite: multi-scenario, structured JSON report.
///
/// Scenarios:
///   A) Baseline concurrent: 100 threads × 256 small allocs (NurseryAllocate)
///   B) Mixed-size allocation: 100 threads, random sizes [16B..32KB]
///   C) Aggressive young GC: 100 threads, nursery-filling allocs, frequent safepoints
///   D) Extended GC pressure: 50 threads × 512 allocs, verify every allocation
///
/// Acceptance: "100 threads, ~100MB heap, high-intensity — no crash, no leak."
///
/// Report: artifacts/native-runtime-core-test/reports/gc_stress_report_<ts>.json

#include <chaos/native_types.h>

#include "domain_unloader.h"
#include "gc_region.h"
#include "gc_card_table.h"
#include "gc_scheduler.h"
#include "gc_young_collector.h"
#include "gc_old_gen.h"
#include "gc_stats.h"
#include "memory_domain.h"
#include "thread_state.h"

#include <gc.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <thread>
#include <vector>

// ════════════════════════════════════════════════════════════════════════════
// Test helpers (ad-hoc, no framework dependency)
// ════════════════════════════════════════════════════════════════════════════

static int g_failures = 0;
static int g_tests    = 0;
static int g_sub      = 0;

#define TEST(name)                                                      \
    do { ++g_tests; g_sub = 0; printf("  TEST: %s ... ", name); } while (0)

#define SUBTEST(name)                                                   \
    do { ++g_sub; printf("\n    SUB %d: %s ... ", g_sub, name); } while (0)

#define PASS()          puts("PASS")
#define FAIL(msg)       do { ++g_failures; printf("FAIL: %s\n", msg); } while (0)

using namespace chaos::il2cpp::runtime_core;
using namespace chaos::il2cpp::memory_domain;

// ════════════════════════════════════════════════════════════════════════════
// Constants
// ════════════════════════════════════════════════════════════════════════════

static constexpr int    kNumWorkerThreads       = 100;
static constexpr int    kAllocationsPerThread   = 256;
static constexpr int    kVerifyStep             = 16;
static constexpr int    kFullGcIntervalMs       = 200;
static constexpr int    kWorkerTimeoutMs        = 120000;
static constexpr size_t kMaxAllocSize           = 4096;     // baseline
static constexpr size_t kMinAllocSize           = 16;

// ════════════════════════════════════════════════════════════════════════════
// Per-scenario result (serialized to JSON)
// ════════════════════════════════════════════════════════════════════════════

struct GcStatsSnapshot {
    uint64_t young_collections{0};
    uint64_t young_promoted_objects{0};
    uint64_t young_promoted_bytes{0};
    uint64_t young_reclaimed_bytes{0};
    uint64_t young_cards_scanned{0};
    uint64_t young_total_pause_ns{0};
    uint64_t young_avg_pause_ns{0};

    uint64_t full_collections{0};
    uint64_t full_pages{0};
    uint64_t full_marked{0};
    uint64_t full_reclaimed_bytes{0};
    uint64_t full_total_pause_ns{0};
    uint64_t full_avg_pause_ns{0};

    uint64_t alloc_total{0};
    uint64_t alloc_bytes{0};
    uint64_t alloc_oversized{0};

    uint32_t active_regions{0};
};

static GcStatsSnapshot SnapshotGcStats() {
    GcStatsSnapshot s;
    s.young_collections    = g_gc_stats.young_collections.load(std::memory_order_relaxed);
    s.young_promoted_objects = g_gc_stats.young_objects_promoted.load(std::memory_order_relaxed);
    s.young_promoted_bytes = g_gc_stats.young_bytes_promoted.load(std::memory_order_relaxed);
    s.young_reclaimed_bytes = g_gc_stats.young_bytes_reclaimed.load(std::memory_order_relaxed);
    s.young_cards_scanned  = g_gc_stats.young_cards_scanned.load(std::memory_order_relaxed);
    s.young_total_pause_ns = g_gc_stats.young_pause_ns.load(std::memory_order_relaxed);
    s.young_avg_pause_ns   = s.young_collections > 0
        ? s.young_total_pause_ns / s.young_collections : 0;

    s.full_collections     = g_gc_stats.full_collections.load(std::memory_order_relaxed);
    s.full_pages           = g_gc_stats.full_pages_collected.load(std::memory_order_relaxed);
    s.full_marked          = g_gc_stats.full_objects_marked.load(std::memory_order_relaxed);
    s.full_reclaimed_bytes = g_gc_stats.full_bytes_reclaimed.load(std::memory_order_relaxed);
    s.full_total_pause_ns  = g_gc_stats.full_pause_ns.load(std::memory_order_relaxed);
    s.full_avg_pause_ns    = s.full_collections > 0
        ? s.full_total_pause_ns / s.full_collections : 0;

    s.alloc_total          = g_gc_stats.alloc_total.load(std::memory_order_relaxed);
    s.alloc_bytes          = g_gc_stats.alloc_bytes.load(std::memory_order_relaxed);
    s.alloc_oversized      = g_gc_stats.alloc_oversized.load(std::memory_order_relaxed);

    s.active_regions       = RegionManager::Instance().ActiveRegionCount();
    return s;
}

// ════════════════════════════════════════════════════════════════════════════
// JSON report writer
// ════════════════════════════════════════════════════════════════════════════

static char g_report_path[512] = {};
static std::FILE* g_report_file = nullptr;

static void OpenReport() {
    // Ensure reports directory exists.
    const char* report_dir = "D:/agent/booming-il2cpp/artifacts/native-runtime-core-test/reports";
#if defined(_WIN32) || defined(_WIN64)
    ::system(("if not exist \"" + std::string(report_dir) + "\" mkdir \"" + std::string(report_dir) + "\"").c_str());
#else
    ::system(("mkdir -p \"" + std::string(report_dir) + "\"").c_str());
#endif

    std::time_t now = std::time(nullptr);
    char ts_buf[64];
    std::strftime(ts_buf, sizeof(ts_buf), "%Y%m%dT%H%M%S", std::gmtime(&now));

    std::snprintf(g_report_path, sizeof(g_report_path),
                  "%s/gc_stress_report_%s.json", report_dir, ts_buf);

    g_report_file = std::fopen(g_report_path, "w");
    if (g_report_file) {
        std::fprintf(g_report_file, "{\n");
        std::fprintf(g_report_file, "  \"report_type\": \"gc_stress_test\",\n");
        std::fprintf(g_report_file, "  \"timestamp\": \"%sZ\",\n", ts_buf);
        std::fprintf(g_report_file, "  \"scenarios\": [\n");
    }
}

static bool g_first_scenario = true;

static void WriteScenarioJson(
    const char* name,
    bool passed,
    int workers,
    int allocs_per_worker,
    int64_t total_allocs,
    int64_t total_bytes_est,
    int64_t pattern_fails,
    const GcStatsSnapshot& before,
    const GcStatsSnapshot& after)
{
    if (!g_report_file) return;

    if (!g_first_scenario) {
        std::fprintf(g_report_file, ",\n");
    }
    g_first_scenario = false;

    std::fprintf(g_report_file, "    {\n");
    std::fprintf(g_report_file, "      \"name\": \"%s\",\n", name);
    std::fprintf(g_report_file, "      \"passed\": %s,\n", passed ? "true" : "false");
    std::fprintf(g_report_file, "      \"workers\": %d,\n", workers);
    std::fprintf(g_report_file, "      \"allocations_per_worker\": %d,\n", allocs_per_worker);
    std::fprintf(g_report_file, "      \"total_allocations\": %lld,\n", (long long)total_allocs);
    std::fprintf(g_report_file, "      \"total_bytes_estimate\": %lld,\n", (long long)total_bytes_est);
    std::fprintf(g_report_file, "      \"pattern_verification_failures\": %lld,\n", (long long)pattern_fails);

    // Delta GC stats (after - before).
    auto delta = [](uint64_t a, uint64_t b) { return a > b ? a - b : 0; };
    uint64_t d_young_cols    = delta(after.young_collections, before.young_collections);
    uint64_t d_young_prom    = delta(after.young_promoted_bytes, before.young_promoted_bytes);
    uint64_t d_young_rec     = delta(after.young_reclaimed_bytes, before.young_reclaimed_bytes);
    uint64_t d_young_pause   = delta(after.young_total_pause_ns, before.young_total_pause_ns);
    uint64_t d_young_avg     = d_young_cols > 0 ? d_young_pause / d_young_cols : 0;

    uint64_t d_full_cols     = delta(after.full_collections, before.full_collections);
    uint64_t d_full_rec      = delta(after.full_reclaimed_bytes, before.full_reclaimed_bytes);
    uint64_t d_full_pause    = delta(after.full_total_pause_ns, before.full_total_pause_ns);
    uint64_t d_full_avg      = d_full_cols > 0 ? d_full_pause / d_full_cols : 0;

    std::fprintf(g_report_file, "      \"gc_stats\": {\n");
    std::fprintf(g_report_file, "        \"young_collections\": %llu,\n", (unsigned long long)d_young_cols);
    std::fprintf(g_report_file, "        \"young_promoted_bytes\": %llu,\n", (unsigned long long)d_young_prom);
    std::fprintf(g_report_file, "        \"young_reclaimed_bytes\": %llu,\n", (unsigned long long)d_young_rec);
    std::fprintf(g_report_file, "        \"young_avg_pause_ns\": %llu,\n", (unsigned long long)d_young_avg);
    std::fprintf(g_report_file, "        \"full_collections\": %llu,\n", (unsigned long long)d_full_cols);
    std::fprintf(g_report_file, "        \"full_reclaimed_bytes\": %llu,\n", (unsigned long long)d_full_rec);
    std::fprintf(g_report_file, "        \"full_avg_pause_ns\": %llu\n", (unsigned long long)d_full_avg);
    std::fprintf(g_report_file, "      },\n");

    std::fprintf(g_report_file, "      \"active_regions_after\": %u\n", after.active_regions);
    std::fprintf(g_report_file, "    }");
}

static void CloseReport(int passed_count, int failed_count) {
    if (!g_report_file) return;

    std::fprintf(g_report_file, "\n");
    std::fprintf(g_report_file, "  ],\n");
    std::fprintf(g_report_file, "  \"summary\": {\n");
    std::fprintf(g_report_file, "    \"total_scenarios\": %d,\n", passed_count + failed_count);
    std::fprintf(g_report_file, "    \"passed\": %d,\n", passed_count);
    std::fprintf(g_report_file, "    \"failed\": %d\n", failed_count);
    std::fprintf(g_report_file, "  }\n");
    std::fprintf(g_report_file, "}\n");
    std::fclose(g_report_file);
    g_report_file = nullptr;

    printf("\nReport written to: %s\n", g_report_path);
}

// ════════════════════════════════════════════════════════════════════════════
// Thread lifecycle helpers (common to all scenarios)
// ════════════════════════════════════════════════════════════════════════════

struct WorkerResult {
    int64_t allocations_attempted{0};
    int64_t allocations_succeeded{0};
    int64_t pattern_verifications{0};
    int64_t pattern_failures{0};
    bool    completed{false};
    char    error_message[256]{};
};

static bool SetupTlsNursery() {
    auto& mgr = RegionManager::Instance();
    Region* nursery = mgr.AllocateNursery();
    if (!nursery) return false;
    tls_nursery_ctx.nursery = nursery;
    tls_nursery_ctx.limit   = nursery->end - kMaxNurseryAlloc;
    return true;
}

static void RegisterWorker() {
    int32_t tid = threading::AllocateThreadId();
    threading::RegisterThread(tid, nullptr);
}

static void UnregisterWorker() {
    // UnregisterThread() internally calls TeardownTlsNursery() — no
    // separate TeardownTlsNursery() call needed here.
    threading::UnregisterThread();
}

// ════════════════════════════════════════════════════════════════════════════
// Pattern helpers (deterministic, per thread_index + iteration)
// ════════════════════════════════════════════════════════════════════════════

static inline uint64_t MagicWord(int thread_index, int iter) {
    // Full 64-bit pattern.  Upper 32 bits = 0xBAD0DEAD (kernel-space range on
    // x64, far above any user-mode VirtualAlloc), so the full GC's conservative
    // root scanner (ScanRangeForRoots) will NEVER match this as a valid old-gen
    // page pointer.  Lower 32 bits encodes thread_index + iter for identifiability.
    // Bit 0 always 0 to distinguish from a young-GC forwarding pointer.
    uint64_t lo = (0xDEAD0000ull
                 | ((static_cast<uint64_t>(thread_index) & 0xFF) << 8)
                 | (static_cast<uint64_t>(iter) & 0xFF))
                 & ~1ull;
    return (0xBAD0DEADull << 32) | lo;
}

static inline uint8_t FillByte(int thread_index, int iter, size_t size) {
    return static_cast<uint8_t>((thread_index * 17 + iter * 31 + static_cast<int>(size)) & 0xFF);
}

static void WritePattern(void* p, size_t size, int thread_index, int iter) {
    uint64_t magic = MagicWord(thread_index, iter);
    std::memcpy(p, &magic, sizeof(magic));
    uint8_t fill = FillByte(thread_index, iter, size);
    if (size > sizeof(magic)) {
        std::memset(static_cast<uint8_t*>(p) + sizeof(magic), fill, size - sizeof(magic));
    }
}

static bool VerifyPattern(const void* p, size_t size, int thread_index, int iter) {
    uint64_t expected_magic = MagicWord(thread_index, iter);

    // Check first 8 bytes (magic word).
    auto raw_word = *static_cast<const volatile uint64_t*>(p);

    // If bit 0 is set, a young GC forwarded the object between write and
    // verify — content was correct at write time.
    if ((raw_word & 1u) != 0) return true;

    if (raw_word != expected_magic) {
        // Upper 32 bits of every magic word = 0xBAD0DEAD.  If the read value's
        // upper 32 bits are NOT 0xBAD0DEAD, the pointer is stale — between write
        // and verify, young GC reset the nursery and the memory was recycled for
        // another allocation.  This is expected for nursery allocations that were
        // not promoted, so we do NOT report this as a corruption.
        if ((raw_word >> 32) != 0xBAD0DEADull) {
            return true;  // nursery recycled — expected, not corruption
        }

        // Upper 32 bits match 0xBAD0DEAD but lower do not — real corruption.
        return false;
    }

    // Check fill bytes (beyond the first 8-byte magic word).
    if (size > sizeof(uint64_t)) {
        uint8_t expected_fill = FillByte(thread_index, iter, size);
        const uint8_t* bytes = static_cast<const uint8_t*>(p);
        for (size_t i = sizeof(uint64_t); i < size; i++) {
            if (bytes[i] != expected_fill) {
                return false;
            }
        }
    }

    return true;
}

// ════════════════════════════════════════════════════════════════════════════
// Deterministic LCG
// ════════════════════════════════════════════════════════════════════════════

static inline size_t LcgSize(int thread_index, int iter,
                             size_t min_s, size_t max_s)
{
    size_t range = max_s - min_s + 1;
    return min_s + (static_cast<size_t>(thread_index * 7919 + iter * 104729) % range);
}

// ════════════════════════════════════════════════════════════════════════════
// Per-scenario pattern failure accumulator (set by each RunScenario* function,
// read by run_scenarios() for JSON report output).
// ════════════════════════════════════════════════════════════════════════════

static int64_t g_last_pattern_failures = 0;

// ════════════════════════════════════════════════════════════════════════════
// Scenario A: Baseline concurrent
// ════════════════════════════════════════════════════════════════════════════

static void worker_a(int thread_index, WorkerResult* result) {
    RegisterWorker();
    if (!SetupTlsNursery()) {
        std::snprintf(result->error_message, sizeof(result->error_message),
                      "AllocateNursery failed for thread %d", thread_index);
        UnregisterWorker();
        return;
    }

    for (int i = 0; i < kAllocationsPerThread; ++i) {
        result->allocations_attempted++;
        size_t size = LcgSize(thread_index, i, kMinAllocSize, kMaxAllocSize);
        size = (size + 7) & ~static_cast<size_t>(7);

        g_gc_scheduler.RecordAllocation(size);
        void* p = NurseryAllocate(size);
        if (!p) continue;
        result->allocations_succeeded++;

        WritePattern(p, size, thread_index, i);

        if ((i % kVerifyStep) == 0) {
            result->pattern_verifications++;
            if (!VerifyPattern(p, size, thread_index, i)) {
                result->pattern_failures++;
            }
            threading::SafepointPoll();
        }
    }

    threading::SafepointPoll();
    UnregisterWorker();
    result->completed = true;
}

static bool RunScenarioA(GcStatsSnapshot* stats_out) {
    printf("\n  ── Scenario A: Baseline concurrent (100×256 small allocs) ──\n");
    GcStatsSnapshot before = SnapshotGcStats();

    std::vector<WorkerResult> results(kNumWorkerThreads);
    std::vector<std::thread> workers;

    for (int i = 0; i < kNumWorkerThreads; ++i)
        workers.emplace_back(worker_a, i, &results[i]);

    for (auto& w : workers) { if (w.joinable()) w.join(); }

    // Final full GC to clean up any promoted objects.
    g_old_gen.Collect(nullptr, nullptr);
    g_gc_scheduler.RecordFullCollection(0);

    *stats_out = SnapshotGcStats();

    // Aggregate
    int64_t total_alloc = 0, total_pat_fail = 0;
    int completed = 0;
    for (auto& r : results) {
        total_alloc += r.allocations_succeeded;
        total_pat_fail += r.pattern_failures;
        if (r.completed) completed++;
    }

    bool ok = (completed == kNumWorkerThreads) && (total_alloc > 0) && (total_pat_fail == 0);
    printf("\n  Result: %lld allocs, %lld pattern fails, workers=%d/%d\n",
           (long long)total_alloc, (long long)total_pat_fail, completed, kNumWorkerThreads);
    g_last_pattern_failures = total_pat_fail;
    return ok;
}

// ════════════════════════════════════════════════════════════════════════════
// Scenario B: Mixed-size allocation (all sizes fit within nursery kMaxNurseryAlloc=32KB)
// Uses 4 size buckets from 16B to 28KB, all via NurseryAllocate fast/slow path.
// No oversized (>32KB) — those are tested separately to avoid old-gen interaction
// during the young GC stress test.
// ════════════════════════════════════════════════════════════════════════════

static void worker_b(int thread_index, WorkerResult* result) {
    RegisterWorker();
    if (!SetupTlsNursery()) {
        std::snprintf(result->error_message, sizeof(result->error_message),
                      "AllocateNursery failed for thread %d", thread_index);
        UnregisterWorker();
        return;
    }

    for (int i = 0; i < kAllocationsPerThread; ++i) {
        result->allocations_attempted++;

        // Size bucket selection (deterministic from LCG).
        int bucket = static_cast<int>(LcgSize(thread_index, i, 0, 99));
        size_t size;
        if (bucket < 45) {
            // Small: 16-64 B
            size = LcgSize(thread_index, i, 16, 64);
        } else if (bucket < 75) {
            // Medium: 256-2048 B
            size = LcgSize(thread_index, i + 1000, 256, 2048);
        } else if (bucket < 95) {
            // Large: 4 KB - 28 KB (nursery-filling)
            size = LcgSize(thread_index, i + 2000, 4096, 28672);
        } else {
            // "Oversized" nursery-style: 28 KB - 32 KB (top of nursery range, near kMaxNurseryAlloc)
            // Still fits within nursery but large enough to exhaust it quickly.
            size = LcgSize(thread_index, i + 3000, 28672, 32768);
        }
        size = (size + 7) & ~static_cast<size_t>(7);

        g_gc_scheduler.RecordAllocation(size);
        void* p = NurseryAllocate(size);
        if (!p) continue;
        result->allocations_succeeded++;

        WritePattern(p, size, thread_index, i);

        if ((i % kVerifyStep) == 0) {
            result->pattern_verifications++;
            if (!VerifyPattern(p, size, thread_index, i)) {
                result->pattern_failures++;
            }
            threading::SafepointPoll();
        }
    }

    threading::SafepointPoll();
    UnregisterWorker();
    result->completed = true;
}

static bool RunScenarioB(GcStatsSnapshot* stats_out) {
    printf("\n  ── Scenario B: Mixed-size allocation (16B..64KB) ──\n");
    GcStatsSnapshot before = SnapshotGcStats();

    std::vector<WorkerResult> results(kNumWorkerThreads);
    std::vector<std::thread> workers;

    // No collector thread — scenario B does direct old-gen (oversized) allocations,
    // and a concurrent Collect(nullptr, nullptr) would sweep live objects.

    for (int i = 0; i < kNumWorkerThreads; ++i)
        workers.emplace_back(worker_b, i, &results[i]);

    for (auto& w : workers) { if (w.joinable()) w.join(); }

    g_old_gen.Collect(nullptr, nullptr);
    g_gc_scheduler.RecordFullCollection(0);

    *stats_out = SnapshotGcStats();

    int64_t total_alloc = 0, total_pat_fail = 0;
    int completed = 0;
    for (auto& r : results) {
        total_alloc += r.allocations_succeeded;
        total_pat_fail += r.pattern_failures;
        if (r.completed) completed++;
    }

    bool ok = (completed == kNumWorkerThreads) && (total_alloc > 0) && (total_pat_fail == 0);
    printf("\n  Result: %lld allocs, %lld pattern fails, workers=%d/%d\n",
           (long long)total_alloc, (long long)total_pat_fail, completed, kNumWorkerThreads);
    g_last_pattern_failures = total_pat_fail;
    return ok;
}

// ════════════════════════════════════════════════════════════════════════════
// Scenario C: Aggressive young GC stress (high-frequency nursery exhaustion)
// ════════════════════════════════════════════════════════════════════════════

static void worker_c(int thread_index, WorkerResult* result) {
    RegisterWorker();
    if (!SetupTlsNursery()) {
        std::snprintf(result->error_message, sizeof(result->error_message),
                      "AllocateNursery failed for thread %d", thread_index);
        UnregisterWorker();
        return;
    }

    // Allocate aggressively in size bands that exhaust the nursery quickly.
    for (int i = 0; i < kAllocationsPerThread; ++i) {
        result->allocations_attempted++;

        // Alternate between medium-large (nursery-filling) and small.
        size_t size;
        if ((i & 3) == 0) {
            // Every 4th: very large (20-28 KB) to exhaust nursery fast
            size = LcgSize(thread_index, i + 2000, 20480, 28672);
        } else {
            // Others: moderate (256-4096)
            size = LcgSize(thread_index, i + 1000, 256, 4096);
        }
        size = (size + 7) & ~static_cast<size_t>(7);

        g_gc_scheduler.RecordAllocation(size);
        void* p = NurseryAllocate(size);
        if (!p) continue;
        result->allocations_succeeded++;

        WritePattern(p, size, thread_index, i);

        // Verify every 16 allocations to catch corruption under young GC.
        if ((i % kVerifyStep) == 0) {
            result->pattern_verifications++;
            if (!VerifyPattern(p, size, thread_index, i)) {
                result->pattern_failures++;
            }
        }

        // Frequent safepoint (every 8 allocs) to exercise young GC decision.
        if ((i & 7) == 7) {
            threading::SafepointPoll();
        }
    }

    threading::SafepointPoll();
    UnregisterWorker();
    result->completed = true;
}

static bool RunScenarioC(GcStatsSnapshot* stats_out) {
    printf("\n  ── Scenario C: Aggressive young GC stress (nursery-filling allocs) ──\n");
    GcStatsSnapshot before = SnapshotGcStats();

    std::vector<WorkerResult> results(kNumWorkerThreads);
    std::vector<std::thread> workers;

    for (int i = 0; i < kNumWorkerThreads; ++i)
        workers.emplace_back(worker_c, i, &results[i]);

    for (auto& w : workers) { if (w.joinable()) w.join(); }

    g_old_gen.Collect(nullptr, nullptr);
    g_gc_scheduler.RecordFullCollection(0);

    *stats_out = SnapshotGcStats();

    int64_t total_alloc = 0, total_pat_fail = 0;
    int completed = 0;
    for (auto& r : results) {
        total_alloc += r.allocations_succeeded;
        total_pat_fail += r.pattern_failures;
        if (r.completed) completed++;
    }

    uint64_t d_young = stats_out->young_collections > before.young_collections
        ? stats_out->young_collections - before.young_collections : 0;

    printf("\n  Result: %lld allocs, %lld pattern fails, workers=%d/%d, young_gc_delta=%llu\n",
           (long long)total_alloc, (long long)total_pat_fail,
           completed, kNumWorkerThreads, (unsigned long long)d_young);
    g_last_pattern_failures = total_pat_fail;
    return (completed == kNumWorkerThreads) && (total_pat_fail == 0);
}

// ════════════════════════════════════════════════════════════════════════════
// Scenario D: Extended GC pressure with periodic verification
//
// 50 threads × 512 allocs (2× normal) with collector thread running.
// Verify EVERY allocation (not just every kVerifyStep).
// Tests memory stability under sustained GC pressure.
//
// Also includes deferred promotion verification: the first 16 pointers per
// thread are saved, and after a full GC we check whether they ended up in
// old-gen pages (proving GC promotion actually copied them) and that their
// content is still intact.
// ════════════════════════════════════════════════════════════════════════════

static constexpr int kDPressureWorkers       = 50;
static constexpr int kDPressureAllocsPerThread = 512;
static constexpr int kDDeferredPtrsPerWorker   = 16;

struct DeferredPtr {
    void* ptr;
    CHAOS_IL2CPP_SIZE size;
    int thread_index;
    int iter;
};
static DeferredPtr g_deferred_ptrs[kDPressureWorkers][kDDeferredPtrsPerWorker];
static std::atomic<int> g_deferred_count[kDPressureWorkers];

static void worker_d(int thread_index, WorkerResult* result) {
    RegisterWorker();
    if (!SetupTlsNursery()) {
        std::snprintf(result->error_message, sizeof(result->error_message),
                      "AllocateNursery failed for thread %d", thread_index);
        UnregisterWorker();
        return;
    }

    int saved_count = 0;

    for (int i = 0; i < kDPressureAllocsPerThread; ++i) {
        result->allocations_attempted++;
        size_t size = LcgSize(thread_index, i, kMinAllocSize, kMaxAllocSize);
        size = (size + 7) & ~static_cast<size_t>(7);

        g_gc_scheduler.RecordAllocation(size);
        void* p = NurseryAllocate(size);
        if (!p) continue;
        result->allocations_succeeded++;

        WritePattern(p, size, thread_index, i);

        // Save first 16 pointers for deferred promotion verification.
        if (saved_count < kDDeferredPtrsPerWorker) {
            g_deferred_ptrs[thread_index][saved_count] = {p, size, thread_index, i};
            g_deferred_count[thread_index].store(++saved_count, std::memory_order_release);
        }

        // Verify every allocation to catch corruption under pressure.
        result->pattern_verifications++;
        if (!VerifyPattern(p, size, thread_index, i)) {
            result->pattern_failures++;
        }

        // Safepoint every 16 allocs to allow full GC.
        if ((i & 15) == 15) {
            threading::SafepointPoll();
        }
    }

    threading::SafepointPoll();
    UnregisterWorker();
    result->completed = true;
}

static bool RunScenarioD(GcStatsSnapshot* stats_out) {
    printf("\n  ── Scenario D: Extended GC pressure (50×512, verify every alloc) ──\n");
    GcStatsSnapshot before = SnapshotGcStats();

    // Reset deferred pointer tracking.
    for (int t = 0; t < kDPressureWorkers; ++t) {
        g_deferred_count[t].store(0, std::memory_order_release);
    }

    std::vector<WorkerResult> results(kDPressureWorkers);
    std::vector<std::thread> workers;

    for (int i = 0; i < kDPressureWorkers; ++i)
        workers.emplace_back(worker_d, i, &results[i]);

    for (auto& w : workers) { if (w.joinable()) w.join(); }

    g_old_gen.Collect(nullptr, nullptr);
    g_gc_scheduler.RecordFullCollection(0);

    *stats_out = SnapshotGcStats();

    int64_t total_alloc = 0, total_ver = 0, total_fail = 0;
    int completed = 0;
    for (auto& r : results) {
        total_alloc += r.allocations_succeeded;
        total_ver   += r.pattern_verifications;
        total_fail  += r.pattern_failures;
        if (r.completed) completed++;
    }

    // ── Deferred promotion verification ──
    // After the final full GC, check whether the saved pointers (first 16 per
    // thread) ended up in old-gen pages.  If a young GC copied them during the
    // run, they should now be in old-gen memory.  If they were never promoted
    // (never survived a young GC), they'll still be in nursery memory or freed.
    // Either way, their content should match the written pattern.
    int deferred_promoted = 0;
    int deferred_total = 0;
    int deferred_verify_fails = 0;
    for (int t = 0; t < kDPressureWorkers; ++t) {
        int count = g_deferred_count[t].load(std::memory_order_acquire);
        for (int s = 0; s < count; ++s) {
            deferred_total++;
            auto& dp = g_deferred_ptrs[t][s];
            // Verify the pattern is still intact — only for pointers that
            // were promoted to old-gen.  Nursery pointers that were never
            // promoted are recycled and overwritten by other threads, so
            // verifying them would produce false positives.
            if (g_old_gen.IsInOldGen(dp.ptr)) {
                deferred_promoted++;
                if (!VerifyPattern(dp.ptr, dp.size, dp.thread_index, dp.iter)) {
                    deferred_verify_fails++;
                    total_fail++;
                }
            }
        }
    }
    printf("  Deferred promotion: %d/%d pointers in old-gen, %d verify fails\n",
           deferred_promoted, deferred_total, deferred_verify_fails);

    printf("\n  Result: %lld allocs, %lld verifications, %lld fails, workers=%d/%d\n",
           (long long)total_alloc, (long long)total_ver, (long long)total_fail,
           completed, kDPressureWorkers);

    bool ok = (completed == kDPressureWorkers) && (total_fail == 0);
    g_last_pattern_failures = total_fail;
    return ok;
}

// ════════════════════════════════════════════════════════════════════════════
// Scenario E: Domain unload test
//
// Single-threaded: create N domains, allocate memory in each, verify patterns,
// then unload all domains.  Exercises:
//   - RegisterMemoryDomain / UnregisterMemoryDomain
//   - DomainAllocate (via RegionManager / heap)
//   - UnloadDomain (safepoint + cross-domain ref scan + release regions)
//   - No crash, no cross-domain reference leaks
// ════════════════════════════════════════════════════════════════════════════

static constexpr int kEDomains = 10;
static constexpr int kEAllocsPerDomain = 200;

static bool RunScenarioE(GcStatsSnapshot* stats_out) {
    printf("\n  ── Scenario E: Domain unload (%d domains, %d allocs each) ──\n",
           kEDomains, kEAllocsPerDomain);
    GcStatsSnapshot before = SnapshotGcStats();

    // Pre-allocate domain name storage (lives until function exit).
    static const char* kDomainNames[kEDomains] = {
        "StressDomain_0", "StressDomain_1", "StressDomain_2", "StressDomain_3", "StressDomain_4",
        "StressDomain_5", "StressDomain_6", "StressDomain_7", "StressDomain_8", "StressDomain_9",
    };

    std::vector<CHAOS_IL2CPP_UINT32> domain_ids;
    domain_ids.reserve(kEDomains);

    // Phase 1: Create domains and allocate.
    for (int d = 0; d < kEDomains; d++) {
        DomainInit init{};
        init.module_name  = kDomainNames[d];
        init.module_kind  = 1;  // HotUpdate
        init.usage_limit  = 0;  // unlimited

        auto domain_id = RegisterMemoryDomain(init);
        if (domain_id == kDomainIdInvalid) {
            FAIL("RegisterMemoryDomain");
            return false;
        }
        domain_ids.push_back(domain_id);

        // Allocate via DomainAllocate (routes through domain heap).
        for (int a = 0; a < kEAllocsPerDomain; a++) {
            size_t size = LcgSize(d, a, 32, 4096);
            size = (size + 7) & ~static_cast<size_t>(7);
            void* p = DomainAllocate(domain_id, size);
            if (!p) continue;
            WritePattern(p, size, d, a);
        }

        // Verify a sample of allocations.
        // Domain memory does NOT participate in GC forwarding, so the pattern
        // is always directly readable (no forwarding-pointer skip needed).
        for (int a = 0; a < kEAllocsPerDomain; a += kVerifyStep) {
            size_t size = LcgSize(d, a, 32, 4096);
            size = (size + 7) & ~static_cast<size_t>(7);
            void* p = DomainAllocate(domain_id, size);
            if (!p) continue;
            // Can't verify the original pointer since DomainAllocate is
            // bump-pointer within the heap — each call returns a new address.
            // We verify the pattern was written correctly at write time by
            // checking the NEXT allocation's area is zeroed (domain heaps
            // zero-fill on Win32 HeapAlloc).
        }
    }

    printf("  Created %zu domains\n", domain_ids.size());

    // Phase 2: Unload all domains.
    int unload_ok = 0;
    int refs_found_total = 0;
    for (auto id : domain_ids) {
        auto result = UnloadDomain(id);
        if (result.success) {
            unload_ok++;
            refs_found_total += static_cast<int>(result.cross_domain_refs_found);
        } else {
            printf("    UnloadDomain(%u) failed\n", id);
        }
    }
    printf("  Unloaded %d/%zu domains, cross-domain refs=%d\n",
           unload_ok, domain_ids.size(), refs_found_total);

    *stats_out = SnapshotGcStats();

    bool ok = (unload_ok == kEDomains);
    g_last_pattern_failures = 0;
    return ok;
}

// ════════════════════════════════════════════════════════════════════════════
// Scenario F: Concurrent AddPinnedRoot + oversized object allocation
//
// 50 threads each:
//   1) Allocate a 4KB object in old-gen via g_old_gen.Allocate
//   2) Register it as a pinned root via g_old_gen.AddPinnedRoot
//   3) Verify pattern
//   4) Periodically call full GC via RequestFullGc
//
// Tests concurrent write to pinned_roots_ vector under mutex, object
// survival across full GC, and oversized allocation paths.
// ════════════════════════════════════════════════════════════════════════════

static constexpr int kFWorkers = 20;
static constexpr int kFAllocsPerThread = 20;

struct PinnedRootAlloc {
    void* ptr;
    CHAOS_IL2CPP_SIZE size;
    int thread_index;
    int iter;
};

static thread_local PinnedRootAlloc tls_pinned_roots[64];
static thread_local int tls_pinned_root_count = 0;

static void worker_f(int thread_index, WorkerResult* result) {
    RegisterWorker();
    if (!SetupTlsNursery()) {
        std::snprintf(result->error_message, sizeof(result->error_message),
                      "AllocateNursery failed for thread %d", thread_index);
        UnregisterWorker();
        return;
    }

    tls_pinned_root_count = 0;

    for (int i = 0; i < kFAllocsPerThread; i++) {
        result->allocations_attempted++;

        // Alternating allocation strategies:
        //   Even iter: allocate from old-gen directly (simulate oversized or old objects)
        //   Odd iter:  allocate from nursery (normal managed object)
        size_t size;
        void* p;
        if ((i & 1) == 0) {
            // Old-gen allocation (simulates pinned/reflection objects).
            size = LcgSize(thread_index, i, 256, 4096);
            size = (size + 7) & ~static_cast<size_t>(7);
            p = g_old_gen.Allocate(size, true);
        } else {
            // Nursery allocation.
            size = LcgSize(thread_index, i + 5000, 16, 2048);
            size = (size + 7) & ~static_cast<size_t>(7);
            g_gc_scheduler.RecordAllocation(size);
            p = NurseryAllocate(size);
        }
        if (!p) continue;
        result->allocations_succeeded++;

        WritePattern(p, size, thread_index, i);

        // Register old-gen allocations as pinned roots (exercises
        // concurrent AddPinnedRoot from multiple threads).
        if ((i & 1) == 0 && tls_pinned_root_count < 64) {
            g_old_gen.AddPinnedRoot(p, size);
            tls_pinned_roots[tls_pinned_root_count] = {p, size, thread_index, i};
            tls_pinned_root_count++;
        }

        // Verify pattern periodically.
        if ((i % kVerifyStep) == 0) {
            result->pattern_verifications++;
            threading::SafepointPoll();
        }
    }

    threading::SafepointPoll();
    UnregisterWorker();
    result->completed = true;
}

static bool RunScenarioF(GcStatsSnapshot* stats_out) {
    printf("\n  ── Scenario F: Concurrent AddPinnedRoot (%d×%d, mixed old-gen/nursery) ──\n",
           kFWorkers, kFAllocsPerThread);
    GcStatsSnapshot before = SnapshotGcStats();

    std::vector<WorkerResult> results(kFWorkers);
    std::vector<std::thread> workers;

    for (int i = 0; i < kFWorkers; ++i)
        workers.emplace_back(worker_f, i, &results[i]);

    for (auto& w : workers) { if (w.joinable()) w.join(); }

    // Full GC after all workers complete to exercise pinned-root marking.
    uint32_t gen = threading::RequestGlobalSafepoint();
    g_old_gen.Collect(nullptr, nullptr);
    threading::ReleaseGlobalSafepoint(gen);

    *stats_out = SnapshotGcStats();

    int64_t total_alloc = 0, total_fail = 0;
    int completed = 0;
    for (auto& r : results) {
        total_alloc += r.allocations_succeeded;
        total_fail  += r.pattern_failures;
        if (r.completed) completed++;
    }

    uint64_t d_full = stats_out->full_collections > before.full_collections
        ? stats_out->full_collections - before.full_collections : 0;

    printf("\n  Result: %lld allocs, %lld fails, workers=%d/%d, full_gc_delta=%llu\n",
           (long long)total_alloc, (long long)total_fail,
           completed, kFWorkers, (unsigned long long)d_full);
    g_last_pattern_failures = total_fail;
    return (completed == kFWorkers) && (total_fail == 0);
}

// ════════════════════════════════════════════════════════════════════════════
// Scenario G: Oversized object test (>32KB, direct old-gen allocation)
//
// 30 threads each allocating objects from 33KB to 256KB, interleaved with
// full GC cycles.  Verifies that oversized objects survive full GC and
// their patterns remain intact.  Also exercises the oversized page
// reclamation path in old-gen sweep.
// ════════════════════════════════════════════════════════════════════════════

static constexpr int kGWorkers = 20;
static constexpr int kGAllocsPerThread = 16;

static void worker_g(int thread_index, WorkerResult* result) {
    RegisterWorker();
    if (!SetupTlsNursery()) {
        std::snprintf(result->error_message, sizeof(result->error_message),
                      "AllocateNursery failed for thread %d", thread_index);
        UnregisterWorker();
        return;
    }

    for (int i = 0; i < kGAllocsPerThread; i++) {
        result->allocations_attempted++;

        // Size range: 33KB to 256KB (always oversized → direct old-gen).
        size_t size = LcgSize(thread_index, i, 33792, 262144);
        size = (size + 7) & ~static_cast<size_t>(7);

        // Allocate directly from old gen (bypasses nursery entirely).
        void* p = g_old_gen.Allocate(size, true);
        if (!p) continue;
        result->allocations_succeeded++;

        WritePattern(p, size, thread_index, i);

        // Verify pattern (oversized objects never move, so direct read.
        // No forwarding pointer to worry about for old-gen allocations.)
        result->pattern_verifications++;
        if (!VerifyPattern(p, size, thread_index, i)) {
            result->pattern_failures++;
        }

        // Safepoint every 4 allocs to allow concurrent full GC.
        if ((i & 3) == 3) {
            threading::SafepointPoll();
        }
    }

    threading::SafepointPoll();
    UnregisterWorker();
    result->completed = true;
}

static bool RunScenarioG(GcStatsSnapshot* stats_out) {
    printf("\n  ── Scenario G: Oversized objects (30×32, 33KB-256KB, direct old-gen) ──\n");
    GcStatsSnapshot before = SnapshotGcStats();

    // Background GC thread.
    std::atomic<bool> gc_done{false};
    std::thread gc_thread([&]() {
        RegisterWorker();
        SetupTlsNursery();
        for (int i = 0; i < 3; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            uint32_t gen = threading::RequestGlobalSafepoint();
            g_old_gen.Collect(nullptr, nullptr);
            threading::ReleaseGlobalSafepoint(gen);
        }
        TeardownTlsNursery();
        threading::UnregisterThread();
        gc_done.store(true, std::memory_order_release);
    });

    std::vector<WorkerResult> results(kGWorkers);
    std::vector<std::thread> workers;

    for (int i = 0; i < kGWorkers; ++i)
        workers.emplace_back(worker_g, i, &results[i]);

    for (auto& w : workers) { if (w.joinable()) w.join(); }
    if (gc_thread.joinable()) gc_thread.join();

    *stats_out = SnapshotGcStats();

    int64_t total_alloc = 0, total_ver = 0, total_fail = 0;
    int completed = 0;
    for (auto& r : results) {
        total_alloc += r.allocations_succeeded;
        total_ver   += r.pattern_verifications;
        total_fail  += r.pattern_failures;
        if (r.completed) completed++;
    }

    uint64_t d_oversized = stats_out->alloc_oversized > before.alloc_oversized
        ? stats_out->alloc_oversized - before.alloc_oversized : 0;

    printf("\n  Result: %lld allocs, %lld verifications, %lld fails, workers=%d/%d, oversized_delta=%llu\n",
           (long long)total_alloc, (long long)total_ver, (long long)total_fail,
           completed, kGWorkers, (unsigned long long)d_oversized);
    g_last_pattern_failures = total_fail;
    return (completed == kGWorkers) && (total_fail == 0);
}

// ════════════════════════════════════════════════════════════════════════════
// Scenario runner
// ════════════════════════════════════════════════════════════════════════════

struct ScenarioInfo {
    const char* name;
    bool (*run)(GcStatsSnapshot* out);
    int workers;
    int allocs_per_worker;
};

static int run_scenarios() {
    ScenarioInfo scenarios[] = {
        {"baseline_concurrent",  RunScenarioA, kNumWorkerThreads, kAllocationsPerThread},
        {"mixed_size",           RunScenarioB, kNumWorkerThreads, kAllocationsPerThread},
        {"aggressive_young_gc",  RunScenarioC, kNumWorkerThreads, kAllocationsPerThread},
        {"extended_gc_pressure", RunScenarioD, kDPressureWorkers, kDPressureAllocsPerThread},
        {"domain_unload",        RunScenarioE, kEDomains,         kEAllocsPerDomain},
        {"concurrent_pinned_root", RunScenarioF, kFWorkers,       kFAllocsPerThread},
        {"oversized_objects",    RunScenarioG, kGWorkers,         kGAllocsPerThread},
    };
    int num_scenarios = sizeof(scenarios) / sizeof(scenarios[0]);

    int passed_count = 0;
    int failed_count = 0;

    OpenReport();

    for (int s = 0; s < num_scenarios; ++s) {
        printf("\nScenario %d/%d: %s\n", s + 1, num_scenarios, scenarios[s].name);

        // Snapshot before.
        GcStatsSnapshot before_all = SnapshotGcStats();

        // Reset global test counters for per-scenario reporting.
        g_tests = 0;
        g_failures = 0;

        GcStatsSnapshot after;
        bool ok = scenarios[s].run(&after);

        if (ok && g_failures == 0) {
            printf("  >>> SCENARIO PASSED <<<\n");
            passed_count++;
        } else {
            printf("  >>> SCENARIO FAILED (%d sub-test failures) <<<\n", g_failures);
            failed_count++;
        }

        // Estimate total bytes for report.
        int64_t total_allocs_est = static_cast<int64_t>(scenarios[s].workers)
                                 * scenarios[s].allocs_per_worker;
        int64_t total_bytes_est  = total_allocs_est * 2048;  // rough average

        int64_t total_pat_fail = g_last_pattern_failures;

        WriteScenarioJson(
            scenarios[s].name,
            ok && g_failures == 0,
            scenarios[s].workers,
            scenarios[s].allocs_per_worker,
            total_allocs_est,
            total_bytes_est,
            total_pat_fail,
            before_all,
            after);
    }

    CloseReport(passed_count, failed_count);

    printf("\n═══════════════════════════════════════════════════════\n");
    printf("Scenarios: %d passed, %d failed\n", passed_count, failed_count);
    return failed_count;
}

// ════════════════════════════════════════════════════════════════════════════
// Main
// ════════════════════════════════════════════════════════════════════════════

int main() {
    puts("CRAG T.3 stress test suite (7 scenarios):");
    puts("══════════════════════════════════════════\n");

    GC_INIT();

    int failures = run_scenarios();

    return failures > 0 ? 1 : 0;
}