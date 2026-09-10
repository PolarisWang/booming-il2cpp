/// common_async_stress_test — Multi-threaded stress test for async.h.

#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/async.h>
#include <atomic>
#include <thread>
#include <vector>

using namespace chaos::il2cpp::common;

namespace {
    std::atomic<int> g_stress_call_count{0};
    CHAOS_IL2CPP_INTPTR StressTaskFn(CHAOS_IL2CPP_INTPTR arg) noexcept {
        g_stress_call_count.fetch_add(1, std::memory_order_relaxed);
        return arg;
    }
}

TEST(CommonAsyncStress, ConcurrentTaskRun) {
    register_async_task_run_fn(StressTaskFn);
    g_stress_call_count.store(0);

    constexpr int kThreads = 8;
    constexpr int kIterations = 100;
    std::vector<std::thread> threads;
    for (int t = 0; t < kThreads; t++) {
        threads.emplace_back([]() {
            for (int i = 0; i < kIterations; i++) {
                async_task_run(static_cast<CHAOS_IL2CPP_INTPTR>(i));
            }
        });
    }
    for (auto& t : threads) { t.join(); }
    EXPECT_EQ(kThreads * kIterations, g_stress_call_count.load());
}

TEST(CommonAsyncStress, RegisterRaces) {
    constexpr int kThreads = 4;
    constexpr int kIterations = 50;
    std::vector<std::thread> threads;
    for (int t = 0; t < kThreads; t++) {
        threads.emplace_back([]() {
            for (int i = 0; i < kIterations; i++) {
                register_async_task_run_fn(StressTaskFn);
                async_task_run(static_cast<CHAOS_IL2CPP_INTPTR>(i));
                register_async_task_run_fn(nullptr);
            }
        });
    }
    for (auto& t : threads) { t.join(); }
    SUCCEED();
}
