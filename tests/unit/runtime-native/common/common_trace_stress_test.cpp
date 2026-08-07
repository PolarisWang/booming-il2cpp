/// common_trace_stress_test — Multi-threaded stress test for trace.h ring buffer.

#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/trace.h>
#include <thread>
#include <vector>

TEST(CommonTraceStress, ConcurrentTraceWrites) {
    constexpr int kThreads = 4;
    constexpr int kIterations = 50;
    std::vector<std::thread> threads;
    for (int i = 0; i < kThreads; i++) {
        threads.emplace_back([i]() {
            for (int j = 0; j < kIterations; j++) {
                CHAOS_IL2CPP_LOG_TRACE("stress", "write", "t={0} i={1}", i, j);
            }
        });
    }
    for (auto& t : threads) { t.join(); }

    // Flush should not crash
    CHAOS_IL2CPP_LOG_TRACE_FLUSH("test_trace_stress.tmp");
    std::remove("test_trace_stress.tmp");
    SUCCEED();
}
