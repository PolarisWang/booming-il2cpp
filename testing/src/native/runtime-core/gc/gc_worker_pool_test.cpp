/// gc_worker_pool_test — GcWorkerPool smoke test (GoogleTest).
///
/// Tests basic execution, pool reuse, dynamic sizing, and max-workers
/// configuration.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <atomic>
#include <thread>

#include <chaos/native_types.h>
#include "gc_region.h"
#include "gc_old_gen.h"
#include "gc_scheduler.h"
#include "gc_stats.h"
#include "gc_worker_pool.h"
#include "gc_test_base.h"

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

struct GcWorkerPoolTest : GcTestBase {
};

TEST_F(GcWorkerPoolTest, BasicRun) {
    auto& pool = GcWorkerPool::Instance();
    std::atomic<int> count{0};
    pool.RunWorkers(4, [&count](int idx) {
        count.fetch_add(1);
        printf("  worker %d ran\n", idx);
    });
    EXPECT_GE(count.load(), 1);
}

TEST_F(GcWorkerPoolTest, ReusePool) {
    auto& pool = GcWorkerPool::Instance();
    std::atomic<int> count{0};
    pool.RunWorkers(3, [&count](int idx) {
        count.fetch_add(1);
    });
    EXPECT_GE(count.load(), 1);
}

TEST_F(GcWorkerPoolTest, DefaultMaxWorkers) {
    int def = GcWorkerPool::DefaultMaxWorkers();
    EXPECT_GE(def, 1);
    int hc = static_cast<int>(std::thread::hardware_concurrency());
    if (hc > 1) {
        EXPECT_EQ(def, hc - 1);
    } else {
        EXPECT_EQ(def, 1);
    }
}

TEST_F(GcWorkerPoolTest, SetAndGetMaxWorkers) {
    auto& pool = GcWorkerPool::Instance();
    int saved = pool.GetMaxWorkers();
    EXPECT_GE(saved, 1);

    pool.SetMaxWorkers(2);
    EXPECT_EQ(pool.GetMaxWorkers(), 2);

    // Clamp to minimum of 1.
    pool.SetMaxWorkers(0);
    EXPECT_EQ(pool.GetMaxWorkers(), 1);

    // Restore
    pool.SetMaxWorkers(saved);
    EXPECT_EQ(pool.GetMaxWorkers(), saved);
}

TEST_F(GcWorkerPoolTest, SetMaxWorkersCapsNewWorkers) {
    auto& pool = GcWorkerPool::Instance();

    // Set max to 2 (caller + 2 pool max).
    pool.SetMaxWorkers(2);
    std::atomic<int> count{0};
    pool.RunWorkers(6, [&count](int idx) {
        count.fetch_add(1);
    });
    EXPECT_GE(count.load(), 1);

    // Restore to default.
    pool.SetMaxWorkers(GcWorkerPool::DefaultMaxWorkers());
}

TEST_F(GcWorkerPoolTest, MultipleRounds) {
    auto& pool = GcWorkerPool::Instance();
    for (int round = 0; round < 5; round++) {
        std::atomic<int> count{0};
        pool.RunWorkers(3, [&count, round](int idx) {
            count.fetch_add(1);
        });
        EXPECT_GE(count.load(), 1);
    }
}

TEST_F(GcWorkerPoolTest, SingleWorkerNoPool) {
    auto& pool = GcWorkerPool::Instance();
    bool called = false;
    pool.RunWorkers(1, [&called](int idx) {
        EXPECT_EQ(idx, 0);
        called = true;
    });
    EXPECT_TRUE(called);
}
