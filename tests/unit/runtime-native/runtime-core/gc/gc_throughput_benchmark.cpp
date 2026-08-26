/// gc_throughput_benchmark.cpp — Native GC throughput benchmark (GoogleTest).
///
/// 4 benchmarks:
///   1. NurseryAllocateThroughput  — 10K nursery allocations, ns/allocation
///   2. OldGenAllocateThroughput   — 1K old-gen allocations, ns/allocation
///   3. YoungGcPauseUnderLoad      — 10 young GC cycles, min/max/avg pause
///   4. BgcLatency                 — old-gen allocation latency, concurrent vs. idle
///
/// Run with:
///   test_gc_throughput_benchmark --gtest_filter="*Throughput*"
///
/// All benchmarks print [BENCH] lines for post-processing.

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <thread>
#include <vector>

#include <chaos/native_types.h>
#include "gc_bgc.h"
#include "gc_old_gen.h"
#include "gc_region.h"
#include "gc_scheduler.h"
#include "gc_test_base.h"
#include "gc_young_collector.h"
#include "gc_young_gen.h"
#include "thread_state.h"

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

// ── Helpers ─────────────────────────────────────────────────────────────

/// Wait for a BGC phase with timeout.
static bool WaitForBgcPhase(BgcPhase phase, int timeout_ms = 60000) {
    auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
    while (std::chrono::steady_clock::now() < deadline) {
        if (BgcController::Instance().Phase() == phase) return true;
        if (!BgcController::Instance().IsBusy()) {
            return (phase == BgcPhase::IDLE);
        }
        std::this_thread::yield();
    }
    return false;
}

/// Run a complete BGC cycle (start → concurrent mark → remark → sweep → compact).
/// Blocks until the cycle finishes. Returns false if interrupted.
static bool RunBgcCycleWithRetry() {
    for (int attempt = 0; attempt < 5; attempt++) {
        while (BgcController::Instance().IsBusy())
            std::this_thread::sleep_for(std::chrono::milliseconds(1));

        uint32_t gen = threading::RequestGlobalSafepoint();
        BgcController::Instance().StartBgcCycle();
        threading::ReleaseGlobalSafepoint(gen);

        if (!WaitForBgcPhase(BgcPhase::REMARK_NEEDED, 120000)) {
            auto phase = BgcController::Instance().Phase();
            if (phase == BgcPhase::IDLE || !BgcController::Instance().IsBusy())
                continue;
            return false;
        }

        gen = threading::RequestGlobalSafepoint();
        BgcController::Instance().StwRemark();
        BgcController::Instance().StartConcurrentSweep();
        threading::ReleaseGlobalSafepoint(gen);

        if (!WaitForBgcPhase(BgcPhase::COMPACT_NEEDED, 120000)) {
            auto phase = BgcController::Instance().Phase();
            if (phase == BgcPhase::IDLE || !BgcController::Instance().IsBusy())
                continue;
            return false;
        }

        gen = threading::RequestGlobalSafepoint();
        BgcController::Instance().StwCompact();
        threading::ReleaseGlobalSafepoint(gen);

        BgcController::Instance().WaitForCycleComplete();
        return true;
    }
    return false;
}

// ═══════════════════════════════════════════════════════════════════════════
// Fixture: extends GcBenchTestBase with BGC support for the latency test
// ═══════════════════════════════════════════════════════════════════════════

struct ThroughputBenchTest : GcBenchTestBase {
    static std::atomic<bool> s_bgc_started;

    void SetUp() override {
        GcBenchTestBase::SetUp();
        if (!s_bgc_started.load(std::memory_order_acquire)) {
            BgcController::Instance().Start();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            s_bgc_started.store(true, std::memory_order_release);
        }
    }
};

std::atomic<bool> ThroughputBenchTest::s_bgc_started{false};

// ═══════════════════════════════════════════════════════════════════════════
// Benchmark 1: Nursery allocation throughput
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(ThroughputBenchTest, NurseryAllocateThroughput) {
    static constexpr int kCount = 10000;
    static constexpr uint32_t kSize = 64;

    std::vector<void*> objs;
    objs.reserve(kCount);

    uint64_t start = Rdtsc();
    for (int i = 0; i < kCount; i++) {
        void* p = NurseryAllocate(kSize);
        ASSERT_NE(p, nullptr) << "nursery OOM at iteration " << i;
        InitTestObject(p, kSize);
        objs.push_back(p);
    }
    uint64_t end = Rdtsc();
    uint64_t total_ns = RdtscToNs(end - start);
    uint64_t per_obj_ns = total_ns / kCount;

    RecordMetric("NurseryAllocate/Total", total_ns);
    RecordMetric("NurseryAllocate/PerObject", per_obj_ns);

    // Volatile sink to prevent optimiser from discarding allocations.
    volatile void* sink = objs[0];
    (void)sink;

    printf("  [Bench] NurseryAllocate: %d objects x %u bytes = %llu total ns, "
           "%llu ns/obj\n", kCount, kSize,
           static_cast<unsigned long long>(total_ns),
           static_cast<unsigned long long>(per_obj_ns));
}

// ═══════════════════════════════════════════════════════════════════════════
// Benchmark 2: Old-gen allocation throughput
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(ThroughputBenchTest, OldGenAllocateThroughput) {
    static constexpr int kCount = 1000;
    static constexpr CHAOS_IL2CPP_SIZE kSize = 64;

    std::vector<void*> objs;
    objs.reserve(kCount);

    uint64_t start = Rdtsc();
    for (int i = 0; i < kCount; i++) {
        void* p = g_old_gen.Allocate(kSize, true);
        ASSERT_NE(p, nullptr) << "old-gen OOM at iteration " << i;
        InitTestObject(p, kSize);
        objs.push_back(p);
    }
    uint64_t end = Rdtsc();
    uint64_t total_ns = RdtscToNs(end - start);
    uint64_t per_obj_ns = total_ns / kCount;

    RecordMetric("OldGenAllocate/Total", total_ns);
    RecordMetric("OldGenAllocate/PerObject", per_obj_ns);

    volatile void* sink = objs[0];
    (void)sink;

    printf("  [Bench] OldGenAllocate: %d objects x %llu bytes = %llu total ns, "
           "%llu ns/obj\n", kCount,
           static_cast<unsigned long long>(kSize),
           static_cast<unsigned long long>(total_ns),
           static_cast<unsigned long long>(per_obj_ns));
}

// ═══════════════════════════════════════════════════════════════════════════
// Benchmark 3: Young GC pause under allocation load
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(ThroughputBenchTest, YoungGcPauseUnderLoad) {
    static constexpr int kGcCycles = 10;
    static constexpr int kAllocsPerCycle = 500;
    static constexpr uint32_t kSize = 64;

    std::vector<uint64_t> pauses_ns;
    pauses_ns.reserve(kGcCycles);

    for (int c = 0; c < kGcCycles; c++) {
        // Allocate nursery objects to create collection pressure.
        for (int i = 0; i < kAllocsPerCycle; i++) {
            void* p = NurseryAllocate(kSize);
            ASSERT_NE(p, nullptr);
            InitTestObject(p, kSize);
        }

        // Force young GC and measure pause.
        uint64_t start = Rdtsc();
        uint32_t gen = threading::RequestGlobalSafepoint();
        GcYoungCollection();
        threading::ReleaseGlobalSafepoint(gen);
        uint64_t end = Rdtsc();

        uint64_t ns = RdtscToNs(end - start);
        pauses_ns.push_back(ns);
        printf("  [Bench] YoungGcPause[%d]: %llu ns\n", c,
               static_cast<unsigned long long>(ns));
    }

    // Compute min/max/avg.
    uint64_t min_ns = *std::min_element(pauses_ns.begin(), pauses_ns.end());
    uint64_t max_ns = *std::max_element(pauses_ns.begin(), pauses_ns.end());
    uint64_t sum = 0;
    for (auto n : pauses_ns) sum += n;
    uint64_t avg_ns = sum / pauses_ns.size();

    RecordMetric("YoungGcPause/Min", min_ns);
    RecordMetric("YoungGcPause/Max", max_ns);
    RecordMetric("YoungGcPause/Avg", avg_ns);

    printf("  [Bench] YoungGcPause: %d cycles, min=%llu ns, max=%llu ns, "
           "avg=%llu ns\n", kGcCycles,
           static_cast<unsigned long long>(min_ns),
           static_cast<unsigned long long>(max_ns),
           static_cast<unsigned long long>(avg_ns));

    // Machine-parseable percentile line for the gc.perf.yaml `young_gc_wks`
    // baseline (pause_seconds.young_gc_wks → P50/P95/P99, tol 50%).
    EmitPercentiles("young_gc_wks", pauses_ns);
}

// ═══════════════════════════════════════════════════════════════════════════
// Benchmark 4: Allocation latency during BGC concurrent mark vs. idle
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(ThroughputBenchTest, BgcLatency) {
    static constexpr int kAllocsPerPhase = 200;
    static constexpr CHAOS_IL2CPP_SIZE kAllocSize = 64;

    // ── Phase 1: allocate during BGC idle ─────────────────────────────
    ASSERT_FALSE(BgcController::Instance().IsBusy())
        << "BGC should be idle at start";

    std::vector<uint64_t> idle_latencies;
    idle_latencies.reserve(kAllocsPerPhase);

    for (int i = 0; i < kAllocsPerPhase; i++) {
        uint64_t t0 = Rdtsc();
        void* p = g_old_gen.Allocate(kAllocSize, true);
        ASSERT_NE(p, nullptr);
        InitTestObject(p, kAllocSize);
        uint64_t t1 = Rdtsc();
        idle_latencies.push_back(RdtscToNs(t1 - t0));
    }

    // ── Phase 2: start BGC and allocate during concurrent mark ────────
    {
        uint32_t gen = threading::RequestGlobalSafepoint();
        BgcController::Instance().StartBgcCycle();
        threading::ReleaseGlobalSafepoint(gen);
    }

    ASSERT_TRUE(WaitForBgcPhase(BgcPhase::CONCURRENT_MARK))
        << "BGC did not enter concurrent mark";

    std::vector<uint64_t> concurrent_latencies;
    concurrent_latencies.reserve(kAllocsPerPhase);

    for (int i = 0; i < kAllocsPerPhase; i++) {
        uint64_t t0 = Rdtsc();
        void* p = g_old_gen.Allocate(kAllocSize, true);
        ASSERT_NE(p, nullptr);
        InitTestObject(p, kAllocSize);
        uint64_t t1 = Rdtsc();
        concurrent_latencies.push_back(RdtscToNs(t1 - t0));
    }

    // ── Phase 3: complete BGC cycle ───────────────────────────────────
    ASSERT_TRUE(WaitForBgcPhase(BgcPhase::REMARK_NEEDED));

    {
        uint32_t gen = threading::RequestGlobalSafepoint();
        BgcController::Instance().StwRemark();
        BgcController::Instance().StartConcurrentSweep();
        threading::ReleaseGlobalSafepoint(gen);
    }

    ASSERT_TRUE(WaitForBgcPhase(BgcPhase::COMPACT_NEEDED));

    {
        uint32_t gen = threading::RequestGlobalSafepoint();
        BgcController::Instance().StwCompact();
        threading::ReleaseGlobalSafepoint(gen);
    }

    BgcController::Instance().WaitForCycleComplete();

    // ── Analyse latencies ─────────────────────────────────────────────
    auto compute_stats = [](const std::vector<uint64_t>& vals,
                            const char* label) {
        uint64_t min_ns = *std::min_element(vals.begin(), vals.end());
        uint64_t max_ns = *std::max_element(vals.begin(), vals.end());
        uint64_t sum = 0;
        for (auto n : vals) sum += n;
        uint64_t avg_ns = sum / vals.size();
        printf("  [Bench] BgcLatency/%s: min=%llu ns, max=%llu ns, avg=%llu ns\n",
               label,
               static_cast<unsigned long long>(min_ns),
               static_cast<unsigned long long>(max_ns),
               static_cast<unsigned long long>(avg_ns));
        return avg_ns;
    };

    uint64_t idle_avg = compute_stats(idle_latencies, "Idle");
    uint64_t concurrent_avg = compute_stats(concurrent_latencies, "ConcurrentMark");

    RecordMetric("BgcLatency/IdleAvg", idle_avg);
    RecordMetric("BgcLatency/ConcurrentMarkAvg", concurrent_avg);

    // Machine-parseable concurrent-mark latency percentiles for the
    // gc.perf.yaml `bgc_mark_slice` baseline (P95/P99 only, tol 50%).  The
    // per-allocation latency during the concurrent-mark phase is the practical
    // proxy for the concurrent-mark slice cost on this workload.
    EmitPercentiles("bgc_mark_slice", concurrent_latencies, /*p95_only=*/false,
                    /*p95p99_only=*/true);

    // The concurrent-mark path may be slightly slower (extra barrier work),
    // but should not be pathological.
    printf("  [Bench] Concurrent/Idle ratio: %.2fx\n",
           static_cast<double>(concurrent_avg) / static_cast<double>(idle_avg));
}
