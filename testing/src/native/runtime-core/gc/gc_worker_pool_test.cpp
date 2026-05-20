/// gc_worker_pool_test — GcWorkerPool smoke test (GoogleTest).

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <atomic>

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
