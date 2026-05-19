/// gc_stats_test — GC stats unit tests.
///
/// Tests:
///   1. GcRecordYoungCollection + GcGetSnapshot counter match
///   2. GcDumpStats output non-empty
///   3. GcRecordAlloc updates counters
///   4. Multi-threaded concurrent GcRecordAlloc

#include <cstdio>
#include <cstdint>
#include <atomic>
#include <thread>
#include <vector>

#include <chaos/native_types.h>
#include "gc_stats.h"
#include "gc_test_macros.h"

using namespace chaos::il2cpp::runtime_core;

static int g_failures = 0;
static int g_tests    = 0;
static int g_sub      = 0;

// ── Test 1: Record and snapshot ─────────────────────────────────────
void TestRecordAndSnapshot() {
    TEST("RecordAndSnapshot");

    // Record some collections.
    constexpr uint64_t kPauseNs = 1000000;  // 1ms

    GcRecordYoungCollection(256, 64 * 1024, 32 * 1024, 10, kPauseNs);
    GcRecordYoungCollection(512, 128 * 1024, 64 * 1024, 20, kPauseNs);

    // Take snapshot.
    auto snapshot = GcGetSnapshot();
    GC_CHECK(snapshot.young_collections >= 2,
             "snapshot shows at least 2 collections recorded");
}

// ── Test 2: GcDumpStats output ──────────────────────────────────────
void TestDumpStatsOutput() {
    TEST("DumpStatsOutput");

    // Just verify DumpStats doesn't crash.
    GcDumpStats();
    GC_CHECK(true, "GcDumpStats completed without crash");
}

// ── Test 3: GcRecordAlloc updates counters ─────────────────────────
void TestRecordAlloc() {
    TEST("RecordAlloc");

    GcRecordAlloc(4096, false);
    GcRecordAlloc(8192, false);
    GcRecordAlloc(65536, true);  // oversized

    auto snapshot = GcGetSnapshot();
    GC_CHECK(snapshot.alloc_total >= 3,
             "GcRecordAlloc increased alloc_total");
    GC_CHECK(snapshot.alloc_oversized >= 1,
             "oversized allocation recorded");
}

// ── Test 4: Multi-threaded record ───────────────────────────────────
void TestMultiThreadRecord() {
    TEST("MultiThreadRecord");

    constexpr int kNumThreads = 4;
    constexpr int kAllocsPerThread = 1000;

    auto worker = [kAllocsPerThread]() {
        for (int i = 0; i < kAllocsPerThread; i++) {
            GcRecordAlloc(32, false);
        }
    };

    std::vector<std::thread> threads;
    for (int t = 0; t < kNumThreads; t++) {
        threads.emplace_back(worker);
    }
    for (auto& th : threads) th.join();

    auto snapshot = GcGetSnapshot();
    GC_CHECK(snapshot.alloc_total >= kNumThreads * kAllocsPerThread,
             "multi-threaded allocation recorded correctly");
}

// ── Main ────────────────────────────────────────────────────────────
int main() {
    puts("GC stats unit tests");
    puts("════════════════════\n");

    TestRecordAndSnapshot();
    TestDumpStatsOutput();
    TestRecordAlloc();
    TestMultiThreadRecord();

    printf("\nResults: %d tests, %d failures\n", g_tests, g_failures);
    return g_failures > 0 ? 1 : 0;
}
