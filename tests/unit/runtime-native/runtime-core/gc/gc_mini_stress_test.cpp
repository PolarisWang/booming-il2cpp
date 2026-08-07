/// gc_mini_stress — Minimal multi-threaded nursery stress test (GoogleTest).
///
/// 10 threads, 16 nursery allocs each. Quick validation.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <atomic>
#include <thread>
#include <vector>

#include <chaos/native_types.h>
#include "gc_region.h"
#include "gc_old_gen.h"
#include "gc_scheduler.h"
#include "gc_stats.h"
#include "gc_worker_pool.h"
#include "gc_test_base.h"

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

static constexpr int kThreads = 10;
static constexpr int kAllocs = 16;

TEST_F(GcStressTestBase, MiniStress10x16) {
    StressConfig cfg;
    cfg.thread_count = kThreads;
    cfg.allocs_per_thread = kAllocs;
    cfg.size_distribution = { { 128, 1.0f } };
    cfg.survival_strategy = {
        { SurvivalLevel::GLOBAL,       0.20f },
        { SurvivalLevel::THREAD_LOCAL, 0.50f },
        { SurvivalLevel::EPHEMERAL,    0.30f },
    };
    cfg.magic_pattern = 0xABABABAB;
    cfg.gc_interval = 0;

    int global_count = RunConcurrentAlloc(cfg);
    EXPECT_GT(global_count, 0) << "At least some global survivors expected (20% of "
                               << (kThreads * kAllocs) << " = ~" << (kThreads * kAllocs / 5) << ")";
    VerifyNoCorruption(cfg);
}
