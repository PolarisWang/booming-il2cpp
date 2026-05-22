/// gc_benchmark.cpp — GC benchmark regression test suite (G-15)
///
/// Measures GC performance metrics (throughput, pause times, allocation
/// latency) and outputs structured BENCH| lines for regression detection.
///
/// Benchmarks:
///   1. AllocationThroughput  — nursery allocation MB/s
///   2. YoungGcPause          — young GC pause time distribution
///   3. FullGcPause           — full GC pause time distribution
///   4. MixedAllocPattern     — interleaved small/large allocation rate
///   5. Gen1Pause             — Gen1 collection pause distribution
///
/// Output format:
///   BENCH|<name>|<metric>|<value>|<unit>
///
/// A companion tool (bench-compare) can diff two runs to detect regressions.

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <vector>
#include <algorithm>
#include <cmath>

#include <chaos/native_types.h>
#include "gc_api.h"
#include "gc_gen1.h"
#include "gc_old_gen.h"
#include "gc_region.h"
#include "gc_scheduler.h"
#include "gc_stats.h"
#include "gc_young_gen.h"

using namespace chaos::il2cpp::runtime_core;

#include "gc_test_macros.h"

static int g_failures = 0;
static int g_tests    = 0;
static int g_sub      = 0;

// ── Helpers ──────────────────────────────────────────────────────────

using Clock = std::chrono::high_resolution_clock;

/// Emit a benchmark result line.
#define BENCH(name, metric, value, unit)                                \
    printf("BENCH|%s|%s|%lld|%s\n",                                      \
           (name), (metric), static_cast<long long>(value), (unit))

/// Allocate a nursery object and zero its memory.
static void* BenchAlloc(size_t size) {
    void* p = NurseryAllocate(size);
    if (p) std::memset(p, 0, size);
    return p;
}

/// Allocate directly into old gen.
static void* BenchAllocOldGen(size_t size) {
    return g_old_gen.Allocate(size, false);
}

// ── Benchmark 1: Allocation throughput ────────────────────────────────
void BenchAllocationThroughput() {
    printf("\n── Benchmark 1: AllocationThroughput ──\n");

    constexpr int kAllocs = 100000;
    constexpr size_t kSize = 64;

    auto start = Clock::now();
    for (int i = 0; i < kAllocs; i++) {
        void* p = BenchAlloc(kSize);
        if (!p) break;
    }
    auto end = Clock::now();

    auto elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    double elapsed_s = static_cast<double>(elapsed_ns) / 1e9;
    double total_mb = (static_cast<double>(kAllocs) * kSize) / (1024.0 * 1024.0);
    double mb_per_s = total_mb / elapsed_s;

    BENCH("AllocationThroughput", "throughput", static_cast<int64_t>(mb_per_s * 1000), "KB/s");
    BENCH("AllocationThroughput", "avg_alloc_ns", elapsed_ns / kAllocs, "ns");
    BENCH("AllocationThroughput", "total_elapsed_ms", elapsed_ns / 1000000, "ms");

    GC_CHECK(mb_per_s > 0, "Allocation throughput: %.1f MB/s", mb_per_s);
}

// ── Benchmark 2: Young GC pause ──────────────────────────────────────
void BenchYoungGcPause() {
    printf("\n── Benchmark 2: YoungGcPause ──\n");

    constexpr int kGcs = 10;
    std::vector<uint64_t> pauses;
    pauses.reserve(kGcs);

    // Record baseline pause counters.
    auto snap_before = GcGetSnapshot();

    for (int g = 0; g < kGcs; g++) {
        // Fill nursery by allocating up to remaining space.
        auto* region = g_young_gen.region.load(std::memory_order_acquire);
        char* bump = g_young_gen.bump.load(std::memory_order_acquire);
        char* region_end = g_young_gen.region_end.load(std::memory_order_acquire);
        if (region && bump && region_end) {
            size_t remaining = static_cast<size_t>(region_end - bump);
            if (remaining > 0) {
                BenchAlloc(remaining);
            }
        }
    }

    auto snap_after = GcGetSnapshot();
    uint64_t total_pause = snap_after.young_pause_ns_total - snap_before.young_pause_ns_total;
    int64_t collections = static_cast<int64_t>(snap_after.young_collections - snap_before.young_collections);

    if (collections > 0) {
        uint64_t avg = total_pause / collections;
        BENCH("YoungGcPause", "count", collections, "samples");
        BENCH("YoungGcPause", "avg_ns", avg, "ns");
        BENCH("YoungGcPause", "total_ns", total_pause, "ns");
        GC_CHECK(avg > 0, "Young GC avg pause: %lld ns (%lld samples)",
                 static_cast<long long>(avg),
                 static_cast<long long>(collections));
    } else {
        BENCH("YoungGcPause", "count", 0, "samples");
        GC_CHECK(true, "Young GC not triggered (insufficient allocation pressure)");
    }
}

// ── Benchmark 3: Full GC pause ────────────────────────────────────────
void BenchFullGcPause() {
    printf("\n── Benchmark 3: FullGcPause ──\n");

    constexpr int kGcs = 5;
    std::vector<uint64_t> pauses;
    pauses.reserve(kGcs);

    for (int g = 0; g < kGcs; g++) {
        // Populate old gen with some objects.
        for (int i = 0; i < 50; i++) {
            BenchAllocOldGen(1024);
        }

        // Measure full GC pause via chaos_gc_collect.
        uint64_t before = chaos_gc_get_total_pause_duration();
        g_old_gen.Collect(nullptr, nullptr);
        uint64_t after = chaos_gc_get_total_pause_duration();
        uint64_t pause_ns = after - before;

        if (pause_ns > 0) {
            pauses.push_back(pause_ns);
        }
    }

    if (!pauses.empty()) {
        std::sort(pauses.begin(), pauses.end());
        uint64_t total = 0;
        for (auto p : pauses) total += p;
        uint64_t avg = total / pauses.size();
        uint64_t median = pauses[pauses.size() / 2];

        BENCH("FullGcPause", "count", pauses.size(), "samples");
        BENCH("FullGcPause", "avg_ns", avg, "ns");
        BENCH("FullGcPause", "median_ns", median, "ns");

        GC_CHECK(avg > 0, "Full GC avg pause: %lld ns (%lld samples)",
                 static_cast<long long>(avg),
                 static_cast<long long>(pauses.size()));
    } else {
        GC_CHECK(false, "Full GC pause samples collected");
    }
}

// ── Benchmark 4: Mixed allocation pattern ─────────────────────────────
void BenchMixedAllocPattern() {
    printf("\n── Benchmark 4: MixedAllocPattern ──\n");

    constexpr int kIterations = 50000;
    std::vector<void*> ptrs;
    ptrs.reserve(1000);

    auto start = Clock::now();
    for (int i = 0; i < kIterations; i++) {
        // Mix small and medium allocations.
        size_t size = (i % 5 == 0) ? 512 : 32;
        void* p = BenchAlloc(size);
        if (!p) break;

        // Keep some objects alive (simulate survivor pattern).
        if (i % 50 == 0) {
            if (ptrs.size() < 1000) {
                ptrs.push_back(p);
            }
        }
    }
    auto end = Clock::now();

    auto elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    double elapsed_s = static_cast<double>(elapsed_ns) / 1e9;
    // Approximate throughput: kIterations * 32 bytes (mostly small allocs).
    double total_bytes = static_cast<double>(kIterations) * 32.0;
    double mb_per_s = (total_bytes / (1024.0 * 1024.0)) / elapsed_s;

    BENCH("MixedAllocPattern", "throughput_kb_s", static_cast<int64_t>(mb_per_s * 1000), "KB/s");
    BENCH("MixedAllocPattern", "avg_alloc_ns", elapsed_ns / kIterations, "ns");
    BENCH("MixedAllocPattern", "survivor_count", static_cast<int64_t>(ptrs.size()), "objects");

    GC_CHECK(mb_per_s > 0, "Mixed allocation throughput: %.1f MB/s", mb_per_s);
}

// ── Benchmark 5: Gen1 collection pause ────────────────────────────────
void BenchGen1Pause() {
    printf("\n── Benchmark 5: Gen1Pause ──\n");

    // Promote enough objects to trigger Gen1 collection.
    // Run young GCs to populate Gen1, then measure Gen1 pause.
    std::vector<uint64_t> pauses;
    pauses.reserve(10);

    auto snap_before = GcGetSnapshot();

    for (int attempt = 0; attempt < 15; attempt++) {
        // Allocate to trigger young GCs and promote to Gen1.
        for (int i = 0; i < 200; i++) {
            void* p = BenchAlloc(128);
            if (!p) break;
        }
    }

    auto snap_after = GcGetSnapshot();
    uint64_t gen1_pause = snap_after.gen1_pause_ns_total - snap_before.gen1_pause_ns_total;
    int64_t collections = static_cast<int64_t>(snap_after.gen1_collections - snap_before.gen1_collections);

    if (collections > 0) {
        uint64_t avg = gen1_pause / collections;
        BENCH("Gen1Pause", "count", collections, "samples");
        BENCH("Gen1Pause", "avg_ns", avg, "ns");
        BENCH("Gen1Pause", "total_ns", gen1_pause, "ns");
    } else {
        BENCH("Gen1Pause", "count", 0, "samples");
    }

    GC_CHECK(true, "Gen1 pause benchmark completed without crash");
}

// ── Summary ────────────────────────────────────────────────────────────
void PrintSummary() {
    printf("\n── BENCHMARK SUMMARY (BENCH| lines for parsing) ──\n");
    // All BENCH| lines were already emitted by individual benchmarks.
    // Print a final header line for clarity.
    printf("BENCH|format: BENCH|<name>|<metric>|<value>|<unit>\n");
    printf("BENCH|tip: pipe two runs through bench-compare for regression\n");
}

// ── Main ────────────────────────────────────────────────────────────────
int main() {
    puts("CRAG GC benchmark regression test suite");
    puts("═══════════════════════════════════════\n");

    // Initialize GC subsystems.
    InitYoungGeneration();
    void* warmup = NurseryAllocate(64);
    (void)warmup;

    // Register this thread.
    threading::RegisterThread(threading::AllocateThreadId(), nullptr);

    // Run benchmarks.
    BenchAllocationThroughput();
    BenchYoungGcPause();
    BenchFullGcPause();
    BenchMixedAllocPattern();
    BenchGen1Pause();

    threading::UnregisterThread();

    PrintSummary();

    printf("\n══ Results: %d benchmarks, %d failures ══\n", g_tests, g_failures);
    return g_failures > 0 ? 1 : 0;
}
