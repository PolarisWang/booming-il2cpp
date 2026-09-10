/// gc_safepoint_test — Safepoint protocol unit tests (GoogleTest).

#include <cstdint>
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

#include "thread_state.h"
#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

TEST(SafepointTest, RegisterUnregisterThread) {
    uint32_t tid = threading::AllocateThreadId();
    EXPECT_NE(tid, 0xFFFFFFFFu);

    threading::RegisterThread(tid, nullptr);
    SUCCEED();

    threading::UnregisterThread();
    SUCCEED();
}

TEST(SafepointTest, CooperativePreemptiveTransition) {
    uint32_t tid = threading::AllocateThreadId();
    threading::RegisterThread(tid, nullptr);

    threading::EnterCooperativeMode();
    SUCCEED();

    threading::EnterPreemptiveMode();
    SUCCEED();

    threading::EnterCooperativeMode();
    SUCCEED();

    threading::UnregisterThread();
}

TEST(SafepointTest, SafepointRequestedLifecycle) {
    uint32_t tid = threading::AllocateThreadId();
    threading::RegisterThread(tid, nullptr);

    uint32_t gen = threading::RequestGlobalSafepoint();
    EXPECT_GT(gen, 0u);

    threading::ReleaseGlobalSafepoint(gen);
    SUCCEED();

    threading::UnregisterThread();
}

TEST(SafepointTest, ThreadIdUniqueness) {
    std::vector<uint32_t> ids;
    std::mutex mtx;

    auto worker = [&]() {
        uint32_t tid = threading::AllocateThreadId();
        {
            std::lock_guard<std::mutex> lock(mtx);
            ids.push_back(tid);
        }
        threading::RegisterThread(tid, nullptr);
        threading::UnregisterThread();
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back(worker);
    }
    for (auto& th : threads) th.join();

    bool unique = true;
    for (size_t i = 0; i < ids.size(); i++) {
        for (size_t j = i + 1; j < ids.size(); j++) {
            if (ids[i] == ids[j]) { unique = false; break; }
        }
        if (!unique) break;
    }
    EXPECT_TRUE(unique);
    EXPECT_EQ(ids.size(), 10u);
}
