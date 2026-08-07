/// gc_stress_scenarios.cpp — Multi-threaded GC stress tests (GoogleTest).
///
/// Scenarios:
///   ScenarioS_100Threads_Mixed   — 100 threads × 50 allocs, mixed sizes, mixed survival
///   ScenarioS_Light_4Threads     — 4 threads × 1000 allocs, for quick validation
///
/// These are labelled "stress" in CTest and excluded from the default
/// test run.  Run with:
///   ctest -L stress
///   test_gc_stress --gtest_filter="*ScenarioS*"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <atomic>
#include <thread>
#include <vector>

#include <chaos/native_types.h>
#include "gc_gen1.h"
#include "gc_loh.h"
#include "gc_old_gen.h"
#include "gc_region.h"
#include "gc_young_gen.h"
#include "gc_young_collector.h"
#include "gc_test_base.h"

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

// ═══════════════════════════════════════════════════════════════════════════
// Scenario S — Light: 4 threads × 1000 allocs
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(GcStressTestBase, ScenarioS_Light_4Threads) {
    StressConfig cfg;
    cfg.thread_count = 4;
    cfg.allocs_per_thread = 1000;
    cfg.size_distribution = {
        { 64,   0.40f },
        { 512,  0.30f },
        { 4096, 0.20f },
        { 85 * 1024, 0.10f },
    };
    cfg.survival_strategy = {
        { SurvivalLevel::GLOBAL,       0.20f },
        { SurvivalLevel::THREAD_LOCAL, 0.50f },
        { SurvivalLevel::EPHEMERAL,    0.30f },
    };
    cfg.magic_pattern = 0xDEADBEEF;

    int global_count = RunConcurrentAlloc(cfg);
    printf("  [Stress] Light: %d global survivors across %d threads x %d allocs\n",
           global_count, cfg.thread_count, cfg.allocs_per_thread);

    EXPECT_GT(global_count, 0) << "At least some global survivors expected";
    VerifyNoCorruption(cfg);
}

// ═══════════════════════════════════════════════════════════════════════════
// Scenario S — Full: 100 threads × 50 allocs
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(GcStressTestBase, ScenarioS_100Threads_MixedSizes) {
    StressConfig cfg;
    cfg.thread_count = 100;
    cfg.allocs_per_thread = 50;
    cfg.size_distribution = {
        { 64,      0.40f },
        { 512,     0.30f },
        { 4096,    0.20f },
        { 85 * 1024, 0.10f },
    };
    cfg.survival_strategy = {
        { SurvivalLevel::GLOBAL,       0.20f },
        { SurvivalLevel::THREAD_LOCAL, 0.50f },
        { SurvivalLevel::EPHEMERAL,    0.30f },
    };
    cfg.magic_pattern = 0xCAFEBABE;

    int global_count = RunConcurrentAlloc(cfg);
    printf("  [Stress] Full: %d global survivors across %d threads x %d allocs\n",
           global_count, cfg.thread_count, cfg.allocs_per_thread);

    EXPECT_GT(global_count, 0) << "At least some global survivors expected";
    VerifyNoCorruption(cfg);

    SUCCEED();
}

// ═══════════════════════════════════════════════════════════════════════════
// Scenario S — Gen1 Focused: fill survivor, then concurrent pressure
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(GcStressTestBase, ScenarioS_Gen1Pressure) {
    // Fill survivor to ~50% with keep-alive objects.
    FillSurvivorTo(0.50f);

    StressConfig cfg;
    cfg.thread_count = 8;
    cfg.allocs_per_thread = 200;
    cfg.size_distribution = {
        { 64,  0.50f },
        { 256, 0.30f },
        { 1024, 0.20f },
    };
    cfg.survival_strategy = {
        { SurvivalLevel::EPHEMERAL, 1.0f },  // All ephemeral — lots of Gen1 churn
    };
    cfg.magic_pattern = 0xBEEFCAFE;

    RunConcurrentAlloc(cfg);
    VerifyNoCorruption(cfg);

    // After all ephemeral objects die, Gen1 should have collected.
    GcGen1Collection();

    SUCCEED();
}
