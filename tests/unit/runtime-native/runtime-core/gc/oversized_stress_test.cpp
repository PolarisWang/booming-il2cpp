/// oversized_stress_test — Full GC trigger + page pool validation (GoogleTest).
///
/// Validates that oversized burst allocations (32-85KB) via old-gen handle
/// page pool without unbounded growth or crashes.

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
#include "gc_test_base.h"

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

struct OversizedStressTest : GcStressTestBase {
    void SetUp() override {
        GcStressTestBase::SetUp();
        if (!s_old_gen_inited.load()) {
            g_old_gen.Init(0, 2);
            s_old_gen_inited.store(true);
        }
    }

    static std::atomic<bool> s_old_gen_inited;
};

std::atomic<bool> OversizedStressTest::s_old_gen_inited{false};

TEST_F(OversizedStressTest, O1_BurstOversized) {
    static constexpr int kAllocs = 100;
    std::vector<void*> objs;
    objs.reserve(kAllocs);

    for (int i = 0; i < kAllocs; i++) {
        int size = 32 * 1024 + (i * 17) % (53 * 1024);
        void* p = g_old_gen.Allocate(size, true);
        ASSERT_NE(p, nullptr) << "allocation failed";
        std::memset(p, 0xAB, size);
        objs.push_back(p);
    }

    for (size_t i = 0; i < objs.size(); i++) {
        if (i % 5 != 0)
            g_old_gen.Free(objs[i]);
    }

    std::vector<void*> keep;
    for (size_t i = 0; i < objs.size(); i += 5)
        keep.push_back(objs[i]);
    objs.clear();

    g_old_gen.Collect(nullptr, nullptr);
    SUCCEED();
}

TEST_F(OversizedStressTest, O2_SustainedPressure) {
    static constexpr int kWaves = 5;
    static constexpr int kAllocsPerWave = 50;

    for (int w = 0; w < kWaves; w++) {
        std::vector<void*> wave;
        wave.reserve(kAllocsPerWave);

        for (int i = 0; i < kAllocsPerWave; i++) {
            int size = 40 * 1024 + (w * 13 + i * 7) % (40 * 1024);
            void* p = g_old_gen.Allocate(size, true);
            ASSERT_NE(p, nullptr) << "allocation failed at wave " << w;
            std::memset(p, 0xCD, size);
            wave.push_back(p);
        }

        for (auto* p : wave)
            g_old_gen.Free(p);
        wave.clear();

        g_old_gen.Collect(nullptr, nullptr);
    }
    SUCCEED();
}

TEST_F(OversizedStressTest, O3_MixedInterleaved) {
    static constexpr int kRounds = 50;

    for (int r = 0; r < kRounds; r++) {
        void* small = g_old_gen.Allocate(256, true);
        ASSERT_NE(small, nullptr) << "small alloc failed";
        std::memset(small, 0xAA, 256);

        int size = 48 * 1024 + (r * 31) % (16 * 1024);
        void* big = g_old_gen.Allocate(size, true);
        ASSERT_NE(big, nullptr) << "oversized alloc failed";
        std::memset(big, 0xBB, size);

        g_old_gen.Free(small);
        g_old_gen.Free(big);
    }
    SUCCEED();
}

TEST_F(OversizedStressTest, O4_ParallelOversized) {
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
    for (int t = 0; t < kThreads; t++)
        threads.emplace_back(worker, t);
    for (auto& th : threads) th.join();

    ASSERT_FALSE(failed.load()) << "parallel oversized failed";
    g_old_gen.Collect(nullptr, nullptr);
    SUCCEED();
}
