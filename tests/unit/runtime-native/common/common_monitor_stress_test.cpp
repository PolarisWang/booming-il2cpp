/// common_monitor_stress_test — Multi-threaded stress test for monitor.h.

#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/monitor.h>
#include <thread>
#include <vector>

using namespace chaos::il2cpp::common;

TEST(CommonMonitorStress, ConcurrentAccess) {
    constexpr int kThreads = 4;
    constexpr int kIterations = 50;
    int dummy = 0;
    CHAOS_IL2CPP_INTPTR handle = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dummy);

    std::vector<std::thread> threads;
    for (int i = 0; i < kThreads; i++) {
        threads.emplace_back([handle]() {
            for (int j = 0; j < kIterations; j++) {
                auto& entry = require_monitor_runtime_entry(handle);
                (void)entry;
            }
        });
    }
    for (auto& t : threads) { t.join(); }
    SUCCEED();
}

TEST(CommonMonitorStress, ConcurrentDifferentHandles) {
    constexpr int kThreads = 4;
    constexpr int kIterations = 50;
    int dummies[8] = {};

    std::vector<std::thread> threads;
    for (int i = 0; i < kThreads; i++) {
        threads.emplace_back([&dummies, i]() {
            CHAOS_IL2CPP_INTPTR h = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dummies[i]);
            for (int j = 0; j < kIterations; j++) {
                auto& entry = require_monitor_runtime_entry(h);
                (void)entry;
            }
        });
    }
    for (auto& t : threads) { t.join(); }
    SUCCEED();
}
