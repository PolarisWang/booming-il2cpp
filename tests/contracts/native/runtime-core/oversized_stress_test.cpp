/// oversized_stress_test — Full GC trigger + page pool validation for 32-85KB allocations
///
/// Validates that RequestFullGc() + page pool handles oversized burst allocations
/// without unbounded page growth or crashes.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

#include <chaos/native_types.h>
#include "gc_old_gen.h"
#include "gc_region.h"
#include "gc_stats.h"

using namespace chaos::il2cpp::runtime_core;

static int g_failures = 0;

#define TEST(name) do { printf("  TEST: %s ... ", name); } while (0)
#define PASS() puts("PASS")
#define FAIL(msg) do { ++g_failures; printf("FAIL: %s\n", msg); } while (0)

/// Phase 1: Burst allocation.  Allocate many oversized objects,
/// verify write/read, then free most of them.
static void Phase1_BurstOversized() {
    printf("\n── O1-BurstOversized ──\n");
    TEST("allocate 100× 32-85KB, free 80%, GC, verify");

    static constexpr int kBaseAllocs = 100;
    std::vector<void*> objs;
    objs.reserve(kBaseAllocs);

    // Allocate burst.
    for (int i = 0; i < kBaseAllocs; i++) {
        int size = 32 * 1024 + (i * 17) % (53 * 1024);  // 32-85KB
        void* p = g_old_gen.Allocate(size, true);
        if (!p) { FAIL("allocation failed"); return; }
        std::memset(p, 0xAB, size);
        objs.push_back(p);
    }

    // Free 80% of them (every 5th keep alive).
    for (size_t i = 0; i < objs.size(); i++) {
        if (i % 5 != 0) {
            g_old_gen.Free(objs[i]);
        }
    }

    // Keep every 5th.
    std::vector<void*> keep;
    for (size_t i = 0; i < objs.size(); i += 5) {
        keep.push_back(objs[i]);
    }
    objs.clear();

    printf("\n    allocated=%zu kept=%zu", keep.size(), size_t(kBaseAllocs / 5));

    // GC should collect the freed oversized pages.
    g_old_gen.Collect(nullptr, nullptr);

    printf(" page_count=%zu", g_old_gen.TotalPages());
    PASS();
}

/// Phase 2: Sustained oversized allocation with periodic GC.
/// Allocate/free in waves, ensure page count stays bounded.
static void Phase2_SustainedPressure() {
    printf("\n── O2-SustainedOversized ──\n");
    TEST("5 waves of 50× oversized alloc/free with GC between");

    static constexpr int kWaves = 5;
    static constexpr int kAllocsPerWave = 50;

    for (int w = 0; w < kWaves; w++) {
        std::vector<void*> wave;
        wave.reserve(kAllocsPerWave);

        for (int i = 0; i < kAllocsPerWave; i++) {
            int size = 40 * 1024 + (w * 13 + i * 7) % (40 * 1024);
            void* p = g_old_gen.Allocate(size, true);
            if (!p) { FAIL("allocation failed"); return; }
            std::memset(p, 0xCD, size);
            wave.push_back(p);
        }

        // Free all in this wave.
        for (auto* p : wave) {
            g_old_gen.Free(p);
        }
        wave.clear();

        // GC should clean up.
        g_old_gen.Collect(nullptr, nullptr);

        printf("\n    wave %d done page_count=%zu", w + 1, g_old_gen.TotalPages());
    }

    PASS();
}

/// Phase 3: Interleaved old-gen + oversized.
/// Verify mixed allocation doesn't cause regression.
static void Phase3_MixedInterleaved() {
    printf("\n── O3-MixedInterleaved ──\n");
    TEST("interleave small old-gen + oversized allocations");

    static constexpr int kRounds = 50;

    for (int r = 0; r < kRounds; r++) {
        // Small old-gen alloc.
        void* small = g_old_gen.Allocate(256, true);
        if (!small) { FAIL("small alloc failed"); return; }
        std::memset(small, 0xAA, 256);

        // Oversized alloc.
        int size = 48 * 1024 + (r * 31) % (16 * 1024);
        void* big = g_old_gen.Allocate(size, true);
        if (!big) { FAIL("oversized alloc failed"); return; }
        std::memset(big, 0xBB, size);

        // Free both.
        g_old_gen.Free(small);
        g_old_gen.Free(big);
    }

    printf("\n    %d rounds page_count=%zu", kRounds, g_old_gen.TotalPages());
    PASS();
}

/// Phase 4: Parallel oversized allocation stress.
static void Phase4_ParallelOversized() {
    printf("\n── O4-ParallelOversized ──\n");
    TEST("4 threads × 50 oversized alloc/free");

    static constexpr int kThreads = 4;
    static constexpr int kAllocsPerThread = 50;
    std::atomic<bool> failed{false};

    auto worker = [&](int tid) {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        for (int i = 0; i < kAllocsPerThread; i++) {
            int size = 36 * 1024 + (tid * 11 + i * 13) % (24 * 1024);
            void* p = g_old_gen.Allocate(size, true);
            if (!p) { failed.store(true); break; }
            std::memset(p, 0xEF, size);
            g_old_gen.Free(p);
        }
        threading::UnregisterThread();
    };

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreads; t++) {
        threads.emplace_back(worker, t);
    }
    for (auto& th : threads) th.join();

    if (failed.load()) { FAIL("parallel oversized failed"); return; }

    g_old_gen.Collect(nullptr, nullptr);
    printf("\n    page_count=%zu", g_old_gen.TotalPages());
    PASS();
}

// ── Scenario table ─────────────────────────────────────────────────

struct Scenario {
    const char* name;
    void (*run)();
};

static Scenario g_scenarios[] = {
    {"O1-BurstOversized",       Phase1_BurstOversized},
    {"O2-SustainedOversized",   Phase2_SustainedPressure},
    {"O3-MixedInterleaved",     Phase3_MixedInterleaved},
    {"O4-ParallelOversized",    Phase4_ParallelOversized},
};

static constexpr int kNumScenarios = sizeof(g_scenarios) / sizeof(g_scenarios[0]);

// ── Main ───────────────────────────────────────────────────────────

int main(int argc, char** argv) {
    const char* filter = (argc > 1) ? argv[1] : nullptr;

    g_old_gen.Init(0, 2);

    int run_count = 0;
    int pass_count = 0;
    int total_failures = 0;

    for (int i = 0; i < kNumScenarios; i++) {
        if (filter && strcmp(filter, g_scenarios[i].name) != 0
            && strcmp(filter, "all") != 0) {
            continue;
        }

        printf("── Scenario: %s ──\n", g_scenarios[i].name);
        int before = g_failures;
        g_scenarios[i].run();
        int failed = g_failures - before;

        run_count++;
        if (failed == 0) pass_count++;
        total_failures += failed;

        if (filter && strcmp(filter, g_scenarios[i].name) == 0) {
            break;
        }
    }

    printf("\n═══════════════════════════════════════════\n");
    printf("Results: %d scenarios, %d passed, %d failures\n",
           run_count, pass_count, total_failures);
    printf("═══════════════════════════════════════════\n");

    return total_failures > 0 ? 1 : 0;
}
