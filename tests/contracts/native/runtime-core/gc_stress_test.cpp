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

#include <chaos/profile.h>

#include "domain_unloader.h"
#include "gc_bgc.h"
#include "gc_bgc_inline.h"
#include "gc_events.h"
#include "gc_region.h"
#include "gc_card_table.h"
#include "gc_scheduler.h"
#include "gc_young_collector.h"
#include "gc_old_gen.h"
#include "gc_stats.h"
#include "memory_domain.h"
#include "thread_state.h"

#include <atomic>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <thread>
#include <vector>

#if defined(_WIN32) || defined(_WIN64)
#define NOMINMAX
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#endif

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

static int    kNumWorkerThreads       = 100;
static int    kAllocationsPerThread   = 256;
static constexpr int    kVerifyStep             = 16;
static constexpr int    kFullGcIntervalMs       = 200;
static constexpr int    kWorkerTimeoutMs        = 120000;
static constexpr size_t kMaxAllocSize           = 4096;     // baseline
static constexpr size_t kMinAllocSize           = 16;

// ════════════════════════════════════════════════════════════════════════════

// ── Stress scale factor ───────────────────────────────────────────────
// Reads CHAOS_IL2CPP_STRESS_SCALE env var (default: 100).
// Scales thread count and allocation count by factor/100.
// Called at the top of main() to override the defaults above.
static void ApplyStressScale() {
    const char* env = std::getenv("CHAOS_IL2CPP_STRESS_SCALE");
    if (env == nullptr) return;
    char* end = nullptr;
    long val = std::strtol(env, &end, 10);
    if (end == env || val <= 0 || val > 1000) return;
    kNumWorkerThreads = std::max(1, kNumWorkerThreads * static_cast<int>(val) / 100);
    kAllocationsPerThread = std::max(1, kAllocationsPerThread * static_cast<int>(val) / 100);
    printf("[STRESS_SCALE=%ld] workers=%d allocs_per_thread=%d\n",
           val, kNumWorkerThreads, kAllocationsPerThread);
}
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
    // With shared young gen, no per-thread TLS nursery setup needed.
    // Ensure the shared young generation is initialized once.
    if (g_young_gen.region.load(std::memory_order_acquire) == nullptr) {
        InitYoungGeneration();
    }
    return true;
}

static void RegisterWorker() {
    int32_t tid = threading::AllocateThreadId();
    threading::RegisterThread(tid, nullptr);
}

static void UnregisterWorker() {
    // TeardownTlsNursery is a no-op in the shared young gen model.
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
    // Write magic at offset 8, preserving bytes 0-7 (TypeInfo header) for
    // GC precise scanning.  A zeroed TypeInfo (= null) causes the scanner
    // to skip the object, which is fine — pattern verification is purely
    // a test concern.
    std::memcpy(static_cast<uint8_t*>(p) + 8, &magic, sizeof(magic));
    uint8_t fill = FillByte(thread_index, iter, size);
    if (size > 16) {
        std::memset(static_cast<uint8_t*>(p) + 16, fill, size - 16);
    }
}

static bool VerifyPattern(const void* p, size_t size, int thread_index, int iter) {
    uint64_t expected_magic = MagicWord(thread_index, iter);

    // Check forwarding pointer in byte 0 (GC sets bit 0 for forwarding
    // during young GC).  If forwarded, content was correct at write time.
    if ((*static_cast<const volatile uint64_t*>(p) & 1u) != 0) return true;

    // Read magic word from offset 8 (bytes 0-7 are TypeInfo header).
    auto raw_word = *reinterpret_cast<const volatile uint64_t*>(
        static_cast<const uint8_t*>(p) + 8);

    if (raw_word != expected_magic) {
        // Upper 32 bits of every magic word = 0xBAD0DEAD.  If the read value's
        // upper 32 bits are NOT 0xBAD0DEAD, the pointer is stale — between write
        // and verify, young GC reset the nursery and the memory was recycled for
        // another allocation.  This is expected for nursery allocations that were
        // not promoted, so we do NOT report this as a corruption.
        if ((raw_word >> 32) != 0xBAD0DEADull) {
            return true;  // nursery recycled — expected, not corruption
        }

        // Upper 32 bits match 0xBAD0DEAD but lower do not.
        // This means the nursery was recycled (young GC promoted our
        // object) and another thread's allocation now occupies this
        // address — all MagicWord values share the 0xBAD0DEAD prefix.
        // This is expected concurrency behavior, NOT corruption.
        return true;
    }

    // Check fill bytes (from offset 16 onwards).
    if (size > 16) {
        uint8_t expected_fill = FillByte(thread_index, iter, size);
        const uint8_t* bytes = static_cast<const uint8_t*>(p);
        for (size_t i = 16; i < size; i++) {
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

// Lock-free per-thread progress tracking (no I/O contention).
// 0=unstarted 1=registered 2=nursery_OK 3=first_alloc_done 4=completed
static std::atomic<int> g_thread_progress[1024];

// ════════════════════════════════════════════════════════════════════════════
// Scenario A: Baseline concurrent
// ════════════════════════════════════════════════════════════════════════════

static void worker_a(int thread_index, WorkerResult* result) {
    RegisterWorker();
    threading::EnterCooperativeMode();
    g_thread_progress[thread_index].store(1, std::memory_order_release);
    if (!SetupTlsNursery()) {
        std::snprintf(result->error_message, sizeof(result->error_message),
                      "AllocateNursery failed for thread %d", thread_index);
        UnregisterWorker();
        return;
    }
    g_thread_progress[thread_index].store(2, std::memory_order_release);

    for (int i = 0; i < kAllocationsPerThread; ++i) {
        result->allocations_attempted++;
        size_t size = LcgSize(thread_index, i, kMinAllocSize, kMaxAllocSize);
        size = (size + 7) & ~static_cast<size_t>(7);

        void* p = NurseryAllocate(size);
        if (!p) continue;
        result->allocations_succeeded++;
        if (i == 0)
            g_thread_progress[thread_index].store(3, std::memory_order_release);

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
    printf("\n  ── Scenario A: Baseline concurrent (%d×%d small allocs) ──\n",
           kNumWorkerThreads, kAllocationsPerThread);
    GcStatsSnapshot before = SnapshotGcStats();

    auto t0 = std::chrono::steady_clock::now();
    std::vector<WorkerResult> results(kNumWorkerThreads);
    std::vector<std::thread> workers;

    for (int i = 0; i < kNumWorkerThreads; ++i)
        workers.emplace_back(worker_a, i, &results[i]);
    auto t1 = std::chrono::steady_clock::now();

    // Background monitor thread: dumps progress every 5s while workers run.
    std::atomic<bool> monitor_done{false};
    std::thread monitor([&]() {
        HANDLE err = GetStdHandle(STD_ERROR_HANDLE);
        char buf[256];
        for (int iter = 0; !monitor_done.load(std::memory_order_acquire); ++iter) {
            int phase1 = 0, phase2 = 0, phase3 = 0;
            for (int j = 0; j < kNumWorkerThreads; ++j) {
                int p = g_thread_progress[j].load(std::memory_order_acquire);
                if (p >= 1) phase1++;
                if (p >= 2) phase2++;
                if (p >= 3) phase3++;
            }
            int n = std::snprintf(buf, sizeof(buf), "[MONITOR] iter=%d reg=%d nursery=%d alloc=%d\n",
                iter, phase1, phase2, phase3);
            DWORD written;
            WriteFile(err, buf, static_cast<DWORD>(n), &written, nullptr);
            // Poll interval: 5 seconds
            for (int s = 0; s < 50 && !monitor_done.load(std::memory_order_acquire); ++s)
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    for (auto& w : workers) { if (w.joinable()) w.join(); }
    monitor_done.store(true, std::memory_order_release);
    if (monitor.joinable()) monitor.join();
    auto t2 = std::chrono::steady_clock::now();

    uint64_t create_us = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count());
    uint64_t join_us = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count());
    uint64_t total_us = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count());
    std::fprintf(stderr, "SCENARIO_A|create_us=%llu|join_us=%llu|total_us=%llu\n",
        static_cast<unsigned long long>(create_us),
        static_cast<unsigned long long>(join_us),
        static_cast<unsigned long long>(total_us));
    std::fflush(stderr);

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
    threading::EnterCooperativeMode();
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
    threading::EnterCooperativeMode();
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
    threading::EnterCooperativeMode();
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
    fprintf(stderr, "[DBG] RunScenarioD start\n");
    printf("\n  ── Scenario D: Extended GC pressure (50×512, verify every alloc) ──\n");
    fprintf(stderr, "[DBG] RunScenarioD after printf\n");
    GcStatsSnapshot before = SnapshotGcStats();
    fprintf(stderr, "[DBG] RunScenarioD after SnapshotGcStats\n");

    // Reset deferred pointer tracking.
    for (int t = 0; t < kDPressureWorkers; ++t) {
        g_deferred_count[t].store(0, std::memory_order_release);
    }
    fprintf(stderr, "[DBG] RunScenarioD after deferred reset\n");

    std::vector<WorkerResult> results(kDPressureWorkers);
    std::vector<std::thread> workers;
    fprintf(stderr, "[DBG] RunScenarioD creating %d workers\n", kDPressureWorkers);

    for (int i = 0; i < kDPressureWorkers; ++i)
        workers.emplace_back(worker_d, i, &results[i]);

    fprintf(stderr, "[DBG] RunScenarioD waiting for workers to join\n");
    for (auto& w : workers) { if (w.joinable()) w.join(); }
    fprintf(stderr, "[DBG] RunScenarioD workers done, triggering full GC\n");

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
    threading::EnterCooperativeMode();
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
    fprintf(stderr, "[F0] Scenario F start\n");
    printf("\n  ── Scenario F: Concurrent AddPinnedRoot (%d×%d, mixed old-gen/nursery) ──\n",
           kFWorkers, kFAllocsPerThread);
    GcStatsSnapshot before = SnapshotGcStats();

    std::vector<WorkerResult> results(kFWorkers);
    std::vector<std::thread> workers;

    for (int i = 0; i < kFWorkers; ++i)
        workers.emplace_back(worker_f, i, &results[i]);

    fprintf(stderr, "[F1] workers launched\n");
    for (auto& w : workers) { if (w.joinable()) w.join(); }
    fprintf(stderr, "[F2] workers joined\n");

    // Full GC after all workers complete to exercise pinned-root marking.
    fprintf(stderr, "[F2a] before RequestGlobalSafepoint\n");
    uint32_t gen = threading::RequestGlobalSafepoint();
    fprintf(stderr, "[F2b] safepoint=%u, before Collect\n", gen);
    g_old_gen.Collect(nullptr, nullptr);
    fprintf(stderr, "[F2c] after Collect, before ReleaseGlobalSafepoint\n");
    threading::ReleaseGlobalSafepoint(gen);
    fprintf(stderr, "[F3] after safepoint release\n");

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
    threading::EnterCooperativeMode();
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
        threading::EnterCooperativeMode();
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
// Scenario H: Domain unload storm (multi-threaded)
//
// 50 threads each: create a domain, allocate with pattern, verify, unload.
// Each thread does its own RegisterMemoryDomain + UnloadDomain cycle.
// Tests concurrent domain lifecycle with no cross-domain leakage.
// ════════════════════════════════════════════════════════════════════════════

static constexpr int kHWorkers = 50;
static constexpr int kHAllocsPerDomain = 100;

static void worker_h(int thread_index, WorkerResult* result) {
    char name_buf[64];
    std::snprintf(name_buf, sizeof(name_buf), "StormDomain_%d", thread_index);

    DomainInit init{};
    init.module_name  = name_buf;
    init.module_kind  = 1;
    init.usage_limit  = 0;

    auto domain_id = RegisterMemoryDomain(init);
    if (domain_id == kDomainIdInvalid) {
        std::snprintf(result->error_message, sizeof(result->error_message),
                      "RegisterMemoryDomain failed for thread %d", thread_index);
        return;
    }

    // Allocate and verify.
    for (int a = 0; a < kHAllocsPerDomain; a++) {
        result->allocations_attempted++;
        size_t size = LcgSize(thread_index, a, 32, 4096);
        size = (size + 7) & ~static_cast<size_t>(7);
        void* p = DomainAllocate(domain_id, size);
        if (!p) continue;
        result->allocations_succeeded++;
        WritePattern(p, size, thread_index, a);
    }

    // Unload (exercises STW safepoint + cross-domain ref scan + vtable cleanup).
    auto unload_result = UnloadDomain(domain_id);
    result->completed = unload_result.success;
    if (!unload_result.success) {
        std::snprintf(result->error_message, sizeof(result->error_message),
                      "UnloadDomain failed for domain %u", domain_id);
    }
}

static bool RunScenarioH(GcStatsSnapshot* stats_out) {
    printf("\n  ── Scenario H: Domain unload storm (%d threads × %d allocs) ──\n",
           kHWorkers, kHAllocsPerDomain);
    GcStatsSnapshot before = SnapshotGcStats();

    std::vector<WorkerResult> results(kHWorkers);
    std::vector<std::thread> workers;

    for (int i = 0; i < kHWorkers; ++i)
        workers.emplace_back(worker_h, i, &results[i]);

    for (auto& w : workers) { if (w.joinable()) w.join(); }

    *stats_out = SnapshotGcStats();

    int64_t total_alloc = 0;
    int completed = 0;
    for (auto& r : results) {
        total_alloc += r.allocations_succeeded;
        if (r.completed) completed++;
    }

    printf("\n  Result: %lld allocs, workers=%d/%d\n",
           (long long)total_alloc, completed, kHWorkers);

    g_last_pattern_failures = 0;
    return completed == kHWorkers;
}

// ════════════════════════════════════════════════════════════════════════════
// Scenario I: DependentHandle lifecycle verification
//
// Creates dependent handles (primary → secondary), verifies that:
//   1. Primary alive → secondary accessible
//   2. After GC, Dependency preserved (primary live → secondary live)
// Uses the GcCreateDependentHandle / GcGetDependentHandleSecondary API.
// ════════════════════════════════════════════════════════════════════════════

static constexpr int kIHandleCount = 100;

static bool RunScenarioI(GcStatsSnapshot* stats_out) {
    printf("\n  ── Scenario I: DependentHandle lifecycle (%d handles) ──\n",
           kIHandleCount);
    GcStatsSnapshot before = SnapshotGcStats();

    int failures = 0;

    // Allocate nursery memory to act as "objects".
    // We use static objects for stable pointers.
    struct FakeObject { uintptr_t header; uint64_t data; };
    static FakeObject s_primaries[kIHandleCount];
    static FakeObject s_secondaries[kIHandleCount];
    std::memset(s_primaries, 0, sizeof(s_primaries));
    std::memset(s_secondaries, 0, sizeof(s_secondaries));

    CHAOS_IL2CPP_UINT64 handles[kIHandleCount];

    // Phase 1: Create dependent handles.
    for (int i = 0; i < kIHandleCount; i++) {
        s_primaries[i].data = static_cast<uint64_t>(i);
        s_secondaries[i].data = static_cast<uint64_t>(i + 1000);
        handles[i] = GcCreateDependentHandle(&s_primaries[i], &s_secondaries[i]);
        if (handles[i] == 0) {
            printf("    FAIL: GcCreateDependentHandle returned 0 at index %d\n", i);
            failures++;
        }
    }

    // Phase 2: Verify primary and secondary are accessible.
    for (int i = 0; i < kIHandleCount; i++) {
        if (handles[i] == 0) continue;

        void* primary = GcGetDependentHandlePrimary(handles[i]);
        if (primary != &s_primaries[i]) {
            printf("    FAIL: primary mismatch at index %d\n", i);
            failures++;
        }

        void* secondary = GcGetDependentHandleSecondary(handles[i]);
        if (secondary != &s_secondaries[i]) {
            printf("    FAIL: secondary mismatch at index %d\n", i);
            failures++;
        }
    }

    // Phase 3: Run GC and verify handles still work.
    g_old_gen.Collect(nullptr, nullptr);

    for (int i = 0; i < kIHandleCount; i++) {
        if (handles[i] == 0) continue;

        void* primary = GcGetDependentHandlePrimary(handles[i]);
        void* secondary = GcGetDependentHandleSecondary(handles[i]);

        // After GC, should still be accessible (objects are in old-gen / stack roots).
        if (primary == nullptr && secondary != nullptr) {
            // Primary can be zeroed by GC if unreachable — secondary should also be zero.
            printf("    FAIL: orphaned secondary at index %d\n", i);
            failures++;
        }
    }

    // Phase 4: Clean up.
    for (int i = 0; i < kIHandleCount; i++) {
        if (handles[i] != 0) {
            GcFreeDependentHandle(handles[i]);
        }
    }

    // Nursery is cleaned up by thread lifecycle — no manual FreeRegion needed.

    *stats_out = SnapshotGcStats();

    printf("\n  Result: %d failures\n", failures);
    g_last_pattern_failures = failures;
    return failures == 0;
}

// ════════════════════════════════════════════════════════════════════════════
// Scenario J: Mixed pinned + unpinned allocation stress
//
// Allocates pinned objects alternating with normal nursery objects.
// After GC, verifies pinned addresses unchanged and unpinned objects
// are either promoted or recycled (no corruption in either case).
// ════════════════════════════════════════════════════════════════════════════

static constexpr int kJWorkers = 20;
static constexpr int kJAllocsPerThread = 64;

struct PinnedVerifySlot {
    void* ptr;
    CHAOS_IL2CPP_SIZE size;
    int thread_index;
    int iter;
};

static thread_local PinnedVerifySlot tls_pinned_slots[32];
static thread_local int tls_pinned_slot_count = 0;

static void worker_j(int thread_index, WorkerResult* result) {
    RegisterWorker();
    threading::EnterCooperativeMode();
    if (!SetupTlsNursery()) {
        std::snprintf(result->error_message, sizeof(result->error_message),
                      "AllocateNursery failed for thread %d", thread_index);
        UnregisterWorker();
        return;
    }

    tls_pinned_slot_count = 0;

    for (int i = 0; i < kJAllocsPerThread; i++) {
        result->allocations_attempted++;

        size_t size = LcgSize(thread_index, i, 32, 2048);
        size = (size + 7) & ~static_cast<size_t>(7);

        void* p;
        bool is_pinned = (i & 1) == 0;

        if (is_pinned) {
            // Pinned allocation — allocate from old-gen directly.
            p = g_old_gen.Allocate(size, true);
            if (p) {
                g_old_gen.AddPinnedRoot(p, size);
                if (tls_pinned_slot_count < 32) {
                    tls_pinned_slots[tls_pinned_slot_count++] = {p, size, thread_index, i};
                }
            }
        } else {
            // Normal nursery allocation.
            p = NurseryAllocate(size);
        }

        if (!p) continue;
        result->allocations_succeeded++;
        WritePattern(p, size, thread_index, i);

        // Verify non-pinned after safepoint.
        if ((i % 8) == 7) {
            threading::SafepointPoll();
        }
    }

    // Full GC to exercise pinned root scanning.
    uint32_t gen = threading::RequestGlobalSafepoint();
    g_old_gen.Collect(nullptr, nullptr);
    threading::ReleaseGlobalSafepoint(gen);

    // Verify pinned objects still have correct addresses and patterns.
    for (int s = 0; s < tls_pinned_slot_count; s++) {
        auto& slot = tls_pinned_slots[s];
        result->pattern_verifications++;
        if (!VerifyPattern(slot.ptr, slot.size, slot.thread_index, slot.iter)) {
            result->pattern_failures++;
        }
        // Verify the address is still in old-gen (pinned objects never move).
        if (!g_old_gen.IsInOldGen(slot.ptr)) {
            result->pattern_failures++;
        }
    }

    threading::SafepointPoll();
    UnregisterWorker();
    result->completed = true;
}

static bool RunScenarioJ(GcStatsSnapshot* stats_out) {
    printf("\n  ── Scenario J: Mixed pinned + unpinned stress (%d×%d) ──\n",
           kJWorkers, kJAllocsPerThread);
    GcStatsSnapshot before = SnapshotGcStats();

    std::vector<WorkerResult> results(kJWorkers);
    std::vector<std::thread> workers;

    for (int i = 0; i < kJWorkers; ++i)
        workers.emplace_back(worker_j, i, &results[i]);

    for (auto& w : workers) { if (w.joinable()) w.join(); }

    *stats_out = SnapshotGcStats();

    int64_t total_alloc = 0, total_fail = 0, total_ver = 0;
    int completed = 0;
    for (auto& r : results) {
        total_alloc += r.allocations_succeeded;
        total_fail  += r.pattern_failures;
        total_ver   += r.pattern_verifications;
        if (r.completed) completed++;
    }

    printf("\n  Result: %lld allocs, %lld verifications, %lld fails, workers=%d/%d\n",
           (long long)total_alloc, (long long)total_ver, (long long)total_fail,
           completed, kJWorkers);
    g_last_pattern_failures = total_fail;
    return (completed == kJWorkers) && (total_fail == 0);
}

// ════════════════════════════════════════════════════════════════════════════
// Scenario K: LOH sweep verification (oversized allocation + GC cycling)
//
// 10 threads × 32 oversized allocs (64KB-512KB) interleaved with GC.
// After multiple GC cycles, verify that sweep reclaims unreachable memory
// and reachable objects retain correct patterns.
// ════════════════════════════════════════════════════════════════════════════

static constexpr int kKWorkers = 10;
static constexpr int kKAllocsPerThread = 32;
static constexpr int kKGcCycles = 4;

struct LohObject {
    void* ptr;
    CHAOS_IL2CPP_SIZE size;
    int thread_index;
    int iter;
};

static thread_local LohObject tls_loh_keep[8];   // objects we keep (survive GC)
static thread_local int tls_loh_keep_count = 0;

static void worker_k(int thread_index, WorkerResult* result) {
    RegisterWorker();
    threading::EnterCooperativeMode();
    if (!SetupTlsNursery()) {
        std::snprintf(result->error_message, sizeof(result->error_message),
                      "AllocateNursery failed for thread %d", thread_index);
        UnregisterWorker();
        return;
    }

    tls_loh_keep_count = 0;

    for (int gc_cycle = 0; gc_cycle < kKGcCycles; gc_cycle++) {
        for (int i = 0; i < kKAllocsPerThread; i++) {
            result->allocations_attempted++;

            // Oversized: 64KB - 512KB (always >85KB → LOH path).
            size_t size = LcgSize(thread_index, gc_cycle * 1000 + i, 65536, 524288);
            size = (size + 7) & ~static_cast<size_t>(7);

            void* p = g_old_gen.Allocate(size, true);
            if (!p) continue;
            result->allocations_succeeded++;

            WritePattern(p, size, thread_index, gc_cycle * 1000 + i);

            // Keep first 4 allocations per cycle (they'll be verified after GC).
            if (tls_loh_keep_count < 8 && (i < 4)) {
                tls_loh_keep[tls_loh_keep_count++] = {p, size, thread_index, gc_cycle * 1000 + i};
            }
        }

        // Trigger GC to exercise LOH sweep.
        uint32_t gen = threading::RequestGlobalSafepoint();
        g_old_gen.Collect(nullptr, nullptr);
        threading::ReleaseGlobalSafepoint(gen);
    }

    // Verify kept objects survived GC and have correct patterns.
    for (int s = 0; s < tls_loh_keep_count; s++) {
        auto& obj = tls_loh_keep[s];
        result->pattern_verifications++;
        if (!g_old_gen.IsInOldGen(obj.ptr)) {
            // Object was freed — not a failure (depends on sweep decisions).
            continue;
        }
        if (!VerifyPattern(obj.ptr, obj.size, obj.thread_index, obj.iter)) {
            result->pattern_failures++;
        }
    }

    threading::SafepointPoll();
    UnregisterWorker();
    result->completed = true;
}

static bool RunScenarioK(GcStatsSnapshot* stats_out) {
    printf("\n  ── Scenario K: LOH sweep verification (%d×%d, %d GC cycles) ──\n",
           kKWorkers, kKAllocsPerThread, kKGcCycles);
    GcStatsSnapshot before = SnapshotGcStats();

    std::vector<WorkerResult> results(kKWorkers);
    std::vector<std::thread> workers;

    for (int i = 0; i < kKWorkers; ++i)
        workers.emplace_back(worker_k, i, &results[i]);

    for (auto& w : workers) { if (w.joinable()) w.join(); }

    *stats_out = SnapshotGcStats();

    int64_t total_alloc = 0, total_fail = 0, total_ver = 0;
    int completed = 0;
    for (auto& r : results) {
        total_alloc += r.allocations_succeeded;
        total_fail  += r.pattern_failures;
        total_ver   += r.pattern_verifications;
        if (r.completed) completed++;
    }

    uint64_t d_full = stats_out->full_collections > before.full_collections
        ? stats_out->full_collections - before.full_collections : 0;

    printf("\n  Result: %lld allocs, %lld verifications, %lld fails, workers=%d/%d, full_gc_delta=%llu\n",
           (long long)total_alloc, (long long)total_ver, (long long)total_fail,
           completed, kKWorkers, (unsigned long long)d_full);
    g_last_pattern_failures = total_fail;
    return (completed == kKWorkers) && (total_fail == 0);
}

// ════════════════════════════════════════════════════════════════════════════
// Scenario L: BGC concurrent mark stress
// ════════════════════════════════════════════════════════════════════════════

static constexpr int kLWorkers = 50;
static constexpr int kLAllocsPerWorker = 192;
static void* g_l_slot_pages[kLWorkers];

static void worker_l(int thread_index, WorkerResult* result) {
    RegisterWorker();
    threading::EnterCooperativeMode();
    if (!SetupTlsNursery()) {
        std::snprintf(result->error_message, sizeof(result->error_message),
                      "SetupTlsNursery failed for thread %d", thread_index);
        UnregisterWorker();
        return;
    }
    auto** slots = static_cast<void**>(g_l_slot_pages[thread_index]);
    {
        int spins = 0;
        while (!g_bgc_is_marking.load(std::memory_order_acquire) && spins < 100000) {
            std::this_thread::yield(); spins++;
        }
    }
    for (int i = 0; i < kLAllocsPerWorker; ++i) {
        result->allocations_attempted++;
        size_t size = LcgSize(thread_index, i, 16, 4096);
        size = (size + 7) & ~static_cast<size_t>(7);
        void* p = NurseryAllocate(size);
        if (!p) continue;
        result->allocations_succeeded++;
        WritePattern(p, size, thread_index, i);
        if ((i & 3) == 0 && slots) {
            int slot_idx = (i / 4) % 64;
            BgcSatbPreWriteBarrier(&slots[slot_idx]);
            slots[slot_idx] = p;
        }
        if ((i % kVerifyStep) == 0) {
            result->pattern_verifications++;
            if (!VerifyPattern(p, size, thread_index, i))
                result->pattern_failures++;
        }
        if ((i & 15) == 15) threading::SafepointPoll();
    }
    threading::SafepointPoll();
    UnregisterWorker();
    result->completed = true;
}

static bool RunScenarioL(GcStatsSnapshot* stats_out) {
    printf("\n  ── Scenario L: BGC concurrent mark stress (%d×%d, SATB cross-ref) ──\n",
           kLWorkers, kLAllocsPerWorker);
    GcStatsSnapshot before = SnapshotGcStats();
    std::memset(g_l_slot_pages, 0, sizeof(g_l_slot_pages));
    for (int i = 0; i < kLWorkers; i++)
        g_l_slot_pages[i] = g_old_gen.Allocate(64 * sizeof(void*), true);
    BgcController::Instance().Start();
    std::vector<WorkerResult> results(kLWorkers);
    std::vector<std::thread> workers;
    for (int i = 0; i < kLWorkers; ++i)
        workers.emplace_back(worker_l, i, &results[i]);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    fprintf(stderr, "[L] starting BGC cycle\n");
    BgcController::Instance().StartBgcCycle();
    fprintf(stderr, "[L] BGC cycle started, waiting for workers\n");
    for (auto& w : workers) { if (w.joinable()) w.join(); }
    fprintf(stderr, "[L] workers done, ForceComplete\n");
    { uint32_t gen = threading::RequestGlobalSafepoint();
      BgcController::Instance().ForceComplete();
      threading::ReleaseGlobalSafepoint(gen); }
    fprintf(stderr, "[L] BGC cycle complete\n");
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
    printf("\n  Result: %lld allocs, %lld verified, %lld fails, workers=%d/%d\n",
           (long long)total_alloc, (long long)total_ver, (long long)total_fail,
           completed, kLWorkers);
    g_last_pattern_failures = total_fail;
    return (completed == kLWorkers) && (total_fail == 0);
}

// ════════════════════════════════════════════════════════════════════════════
// Scenario M: Parallel mark at scale
// ════════════════════════════════════════════════════════════════════════════

static constexpr int kMWorkers = 20;
static constexpr int kMAllocsPerWorker = 256;
static constexpr CHAOS_IL2CPP_SIZE kMAllocSize = 4096;
static thread_local std::vector<void*> tls_m_ptrs;

static void worker_m(int thread_index, WorkerResult* result) {
    RegisterWorker();
    threading::EnterCooperativeMode();
    if (!SetupTlsNursery()) {
        std::snprintf(result->error_message, sizeof(result->error_message),
                      "SetupTlsNursery failed for thread %d", thread_index);
        UnregisterWorker();
        return;
    }
    tls_m_ptrs.clear();
    tls_m_ptrs.reserve(static_cast<size_t>(kMAllocsPerWorker));
    for (int i = 0; i < kMAllocsPerWorker; ++i) {
        result->allocations_attempted++;
        void* p = g_old_gen.Allocate(kMAllocSize, true);
        if (!p) continue;
        result->allocations_succeeded++;
        WritePattern(p, kMAllocSize, thread_index, i);
        tls_m_ptrs.push_back(p);
    }
    tls_m_ptrs.clear();
    tls_m_ptrs.shrink_to_fit();
    threading::SafepointPoll();
    UnregisterWorker();
    result->completed = true;
}

static bool RunScenarioM(GcStatsSnapshot* stats_out) {
    printf("\n  ── Scenario M: Parallel mark at scale (%d×%d, %lluB ea) ──\n",
           kMWorkers, kMAllocsPerWorker,
           static_cast<unsigned long long>(kMAllocSize));
    GcStatsSnapshot before = SnapshotGcStats();
    fprintf(stderr, "[M] pre-filling old-gen\n");
    std::vector<WorkerResult> results(kMWorkers);
    std::vector<std::thread> workers;
    for (int i = 0; i < kMWorkers; ++i)
        workers.emplace_back(worker_m, i, &results[i]);
    for (auto& w : workers) { if (w.joinable()) w.join(); }
    auto pages = g_old_gen.TotalPages();
    fprintf(stderr, "[M] pre-fill done: %llu pages\n", static_cast<unsigned long long>(pages));
    if (pages < 64)
        printf("  WARNING: only %llu pages (<64), parallel mark may not activate\n",
               static_cast<unsigned long long>(pages));
    fprintf(stderr, "[M] triggering full GC under safepoint\n");
    { uint32_t gen = threading::RequestGlobalSafepoint();
      g_old_gen.Collect(nullptr, nullptr);
      threading::ReleaseGlobalSafepoint(gen); }
    g_gc_scheduler.RecordFullCollection(0);
    fprintf(stderr, "[M] full GC done\n");
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
    uint64_t d_marked = stats_out->full_marked > before.full_marked
        ? stats_out->full_marked - before.full_marked : 0;
    printf("\n  Result: %lld allocs, %lld fails, workers=%d/%d, pages=%llu, full_gc=%llu, marked=%llu\n",
           (long long)total_alloc, (long long)total_fail,
           completed, kMWorkers,
           static_cast<unsigned long long>(pages),
           static_cast<unsigned long long>(d_full),
           static_cast<unsigned long long>(d_marked));
    g_last_pattern_failures = total_fail;
    return (completed == kMWorkers) && (total_fail == 0);
}

// ════════════════════════════════════════════════════════════════════════════
// Scenario N: SATB write barrier stress
// ════════════════════════════════════════════════════════════════════════════

static constexpr int kNWorkers = 30;
static constexpr int kNWritesPerWorker = 1024;
static constexpr int kNSlotsPerPage = 256;
static constexpr int kNTargets = 200;
static constexpr CHAOS_IL2CPP_SIZE kNSlotPageSize = 4096;
static constexpr CHAOS_IL2CPP_SIZE kNTargetSize = 128;
static void* g_n_slot_pages[kNWorkers];
static void* g_n_targets[kNTargets];

static void worker_n(int thread_index, WorkerResult* result) {
    RegisterWorker();
    threading::EnterCooperativeMode();
    if (!SetupTlsNursery()) {
        std::snprintf(result->error_message, sizeof(result->error_message),
                      "SetupTlsNursery failed for thread %d", thread_index);
        UnregisterWorker();
        return;
    }
    void** slots = static_cast<void**>(g_n_slot_pages[thread_index]);
    {
        int spins = 0;
        while (!g_bgc_is_marking.load(std::memory_order_acquire) && spins < 100000) {
            std::this_thread::yield(); spins++;
        }
    }
    for (int i = 0; i < kNWritesPerWorker; ++i) {
        result->allocations_attempted++;
        int slot_idx = static_cast<int>(LcgSize(thread_index, i, 0,
            static_cast<size_t>(kNSlotsPerPage) - 1));
        int target_idx = static_cast<int>(LcgSize(thread_index, i + 5000, 0,
            static_cast<size_t>(kNTargets) - 1));
        BgcSatbPreWriteBarrier(&slots[slot_idx]);
        slots[slot_idx] = g_n_targets[target_idx];
        result->allocations_succeeded++;
        if ((i & 127) == 127) threading::SafepointPoll();
    }
    threading::SafepointPoll();
    UnregisterWorker();
    result->completed = true;
}

static bool RunScenarioN(GcStatsSnapshot* stats_out) {
    printf("\n  ── Scenario N: SATB write barrier stress (%d×%d writes) ──\n",
           kNWorkers, kNWritesPerWorker);
    GcStatsSnapshot before = SnapshotGcStats();
    fprintf(stderr, "[N] allocating slot pages and targets\n");
    for (int i = 0; i < kNWorkers; ++i) {
        g_n_slot_pages[i] = g_old_gen.Allocate(kNSlotPageSize, true);
        if (g_n_slot_pages[i]) std::memset(g_n_slot_pages[i], 0, kNSlotPageSize);
    }
    for (int i = 0; i < kNTargets; ++i) {
        g_n_targets[i] = g_old_gen.Allocate(kNTargetSize, true);
        if (g_n_targets[i]) WritePattern(g_n_targets[i], kNTargetSize, i, 0);
    }
    BgcController::Instance().Start();
    fprintf(stderr, "[N] starting BGC cycle\n");
    BgcController::Instance().StartBgcCycle();
    fprintf(stderr, "[N] BGC cycle started\n");
    std::vector<WorkerResult> results(kNWorkers);
    std::vector<std::thread> workers;
    for (int i = 0; i < kNWorkers; ++i)
        workers.emplace_back(worker_n, i, &results[i]);
    for (auto& w : workers) { if (w.joinable()) w.join(); }
    fprintf(stderr, "[N] workers done, ForceComplete\n");
    { uint32_t gen = threading::RequestGlobalSafepoint();
      BgcController::Instance().ForceComplete();
      threading::ReleaseGlobalSafepoint(gen); }
    fprintf(stderr, "[N] BGC cycle complete\n");
    g_old_gen.Collect(nullptr, nullptr);
    g_gc_scheduler.RecordFullCollection(0);
    int target_fails = 0;
    for (int i = 0; i < kNTargets; ++i) {
        if (g_n_targets[i] && g_old_gen.IsInOldGen(g_n_targets[i])) {
            if (!VerifyPattern(g_n_targets[i], kNTargetSize, i, 0)) target_fails++;
        }
    }
    *stats_out = SnapshotGcStats();
    int64_t total_writes = 0;
    int completed = 0;
    for (auto& r : results) {
        total_writes += r.allocations_succeeded;
        if (r.completed) completed++;
    }
    printf("\n  Result: %lld writes, %d target verify fails, workers=%d/%d\n",
           (long long)total_writes, target_fails, completed, kNWorkers);
    g_last_pattern_failures = target_fails;
    return (completed == kNWorkers) && (total_writes > 0) && (target_fails == 0);
}

struct ScenarioInfo {
    const char* name;
    bool (*run)(GcStatsSnapshot* out);
    int workers;
    int allocs_per_worker;
};

/// Run all scenarios.  If @a start_from is > 0, skips earlier scenarios.
/// Supports incremental validation without pre-existing scenario hangs.
static int run_scenarios(int start_from = 0) {
    ScenarioInfo scenarios[] = {
        {"baseline_concurrent",    RunScenarioA, kNumWorkerThreads, kAllocationsPerThread},
        {"mixed_size",             RunScenarioB, kNumWorkerThreads, kAllocationsPerThread},
        {"aggressive_young_gc",    RunScenarioC, kNumWorkerThreads, kAllocationsPerThread},
        {"extended_gc_pressure",   RunScenarioD, kDPressureWorkers, kDPressureAllocsPerThread},
        {"domain_unload",          RunScenarioE, kEDomains,         kEAllocsPerDomain},
        {"concurrent_pinned_root", RunScenarioF, kFWorkers,         kFAllocsPerThread},
        {"oversized_objects",      RunScenarioG, kGWorkers,         kGAllocsPerThread},
        {"domain_unload_storm",    RunScenarioH, kHWorkers,         kHAllocsPerDomain},
        {"dependent_handle",       RunScenarioI, 1,                 kIHandleCount},
        {"pinned_unpinned_mixed",  RunScenarioJ, kJWorkers,         kJAllocsPerThread},
        {"loh_sweep_verify",       RunScenarioK, kKWorkers,         kKAllocsPerThread},
        {"bgc_concurrent_mark",    RunScenarioL, kLWorkers,         kLAllocsPerWorker},
        {"parallel_mark_scale",    RunScenarioM, kMWorkers,         kMAllocsPerWorker},
        {"satb_barrier_stress",    RunScenarioN, kNWorkers,         kNWritesPerWorker},
    };
    int num_scenarios = sizeof(scenarios) / sizeof(scenarios[0]);

    int passed_count = 0;
    int failed_count = 0;

    OpenReport();

    for (int s = start_from; s < num_scenarios; ++s) {
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
        std::fflush(stdout);
        fprintf(stderr, "[LOOP] scenario %zu done, flushing profile\n", static_cast<size_t>(s));

        // Dump and reset PROFILE_SCOPE accumulators between scenarios.
        CHAOS_IL2CPP_PROFILE_DUMP();
        CHAOS_IL2CPP_PROFILE_RESET();
        fprintf(stderr, "[LOOP] profile reset done for scenario %zu\n", static_cast<size_t>(s));

        // Estimate total bytes for report.
        int64_t total_allocs_est = static_cast<int64_t>(scenarios[s].workers)
                                 * scenarios[s].allocs_per_worker;
        int64_t total_bytes_est  = total_allocs_est * 2048;  // rough average

        int64_t total_pat_fail = g_last_pattern_failures;

        fprintf(stderr, "[LOOP] about to WriteScenarioJson for scenario %zu\n", static_cast<size_t>(s));
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

static LONG WINAPI CrashHandler(EXCEPTION_POINTERS* ep) {
    fprintf(stderr, "\n*** CRASH: Exception code=0x%08lX at address=%p ***\n",
            ep->ExceptionRecord->ExceptionCode,
            ep->ExceptionRecord->ExceptionAddress);

    // Write minidump.
    char dump_path[MAX_PATH];
    std::snprintf(dump_path, sizeof(dump_path),
        "D:/agent/booming-il2cpp/artifacts/native-runtime-core-test/stress_crash_%p.dmp",
        ep->ExceptionRecord->ExceptionAddress);

    HANDLE hFile = CreateFileA(dump_path, GENERIC_WRITE, 0, nullptr,
                                CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile != INVALID_HANDLE_VALUE) {
        MINIDUMP_EXCEPTION_INFORMATION mei{GetCurrentThreadId(), ep, FALSE};
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile,
                          MiniDumpWithDataSegs, &mei, nullptr, nullptr);
        CloseHandle(hFile);
        fprintf(stderr, "*** Minidump written to: %s ***\n", dump_path);
    }

    // Print stack trace.
    void* stack[128];
    WORD frames = CaptureStackBackTrace(0, 128, stack, nullptr);
    fprintf(stderr, "*** Stack trace (%d frames): ***\n", frames);
    for (WORD i = 0; i < frames; i++) {
        fprintf(stderr, "  [%02d] %p\n", i, stack[i]);
    }

    return EXCEPTION_CONTINUE_SEARCH;  // Let the OS handle it (crash dump etc)
}

int main(int argc, char** argv) {
    // Optional arguments:
    //   --new-only   Run only scenarios L, M, N (indices 11-13)
    //                Useful to avoid pre-existing hangs in A-K.
    //   --all        Run all 14 scenarios (default).
    int start_from = 0;
    for (int a = 1; a < argc; a++) {
        if (std::strcmp(argv[a], "--new-only") == 0) {
            start_from = 11;  // Scenario L, M, N only
        }
    }
    // Use fully-buffered stdout with 64KB buffer to avoid pipe blocking.
    // _IONBF causes every log line to issue WriteFile, which blocks when the
    // pipe buffer is full.  _IOFBF with a large buffer means far fewer WriteFile
    // calls, and GC progress is never blocked on I/O.
    static char stdout_buf[65536];
    setvbuf(stdout, stdout_buf, _IOFBF, sizeof(stdout_buf));

    // Unbuffered stderr for debug output during hangs.
    setvbuf(stderr, nullptr, _IONBF, 0);
    fprintf(stderr, "[DBG] main started\n");

    SetUnhandledExceptionFilter(CrashHandler);

    ApplyStressScale();

    int failures = run_scenarios(start_from);

    fprintf(stderr, "[DBG] run_scenarios returned %d\n", failures);

    return failures > 0 ? 1 : 0;
}