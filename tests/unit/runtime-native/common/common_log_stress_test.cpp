/// common_log_stress_test — Multi-threaded stress test for log.h.
///
/// Verifies that CHAOS_IL2CPP_LOG_* macros are safe under concurrent access
/// from many threads.

#include <gtest/gtest.h>
#include <chaos/common.h>
#include <thread>
#include <vector>

// ═══════════════════════════════════════════════════════════════════════════
// Log stress — many threads writing concurrently
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonLogStress, ConcurrentManyThreads) {
    constexpr int kThreads = 8;
    constexpr int kIterations = 200;
    std::vector<std::thread> threads;
    for (int i = 0; i < kThreads; i++) {
        threads.emplace_back([i]() {
            for (int j = 0; j < kIterations; j++) {
                CHAOS_IL2CPP_LOG_ERROR_M("Stress", "thread {0} iter {1}", i, j);
            }
        });
    }
    for (auto& t : threads) { t.join(); }
    SUCCEED();
}

TEST(CommonLogStress, MixedLevels) {
    // Mixed ERROR/WARN/INFO/DEBUG from multiple threads
    constexpr int kThreads = 4;
    constexpr int kIterations = 50;
    std::vector<std::thread> threads;
    for (int i = 0; i < kThreads; i++) {
        threads.emplace_back([i]() {
            for (int j = 0; j < kIterations; j++) {
                CHAOS_IL2CPP_LOG_ERROR_M("Mixed", "err {0}", i);
                CHAOS_IL2CPP_LOG_WARN_M("Mixed", "warn {0}", i);
                CHAOS_IL2CPP_LOG_INFO_M("Mixed", "info {0}", i);
                CHAOS_IL2CPP_LOG_DEBUG_M("Mixed", "dbg {0}", i);
            }
        });
    }
    for (auto& t : threads) { t.join(); }
    SUCCEED();
}

// ═══════════════════════════════════════════════════════════════════════════
// Raw write stress
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonLogStress, RawWriteConcurrent) {
    constexpr int kThreads = 4;
    constexpr int kIterations = 100;
    std::vector<std::thread> threads;
    for (int i = 0; i < kThreads; i++) {
        threads.emplace_back([i]() {
            for (int j = 0; j < kIterations; j++) {
                CHAOS_IL2CPP_LOG_WRITE_RAW_M("stress raw {0} {1}\n", i, j);
            }
        });
    }
    for (auto& t : threads) { t.join(); }
    SUCCEED();
}
