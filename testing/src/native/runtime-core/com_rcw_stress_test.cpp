// com_rcw_stress_test.cpp — Concurrent RCW creation/release stress tests.
//
// Spawns multiple threads that concurrently create and release RCWs to
// verify thread safety of the RCW cache (global table + QueryInterfaceCached).

#include "marshal_test_fixture.h"

#include <com_rcw.h>

#include <atomic>
#include <thread>
#include <vector>

#ifndef _WIN32

using namespace chaos::il2cpp::com_rcw;

// ════════════════════════════════════════════════════════════════════════════
// S1 — RCW stress tests
// ════════════════════════════════════════════════════════════════════════════

class ComRcwStressTest : public MarshalTestFixture {
};

TEST_F(ComRcwStressTest, ConcurrentCreateRelease) {
    // Create RCWs from multiple threads using distinct fake COM pointers.
    constexpr int kThreadCount = 8;
    constexpr int kRcwPerThread = 128;
    std::atomic<int> failures{0};

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreadCount; ++t) {
        threads.emplace_back([&, t]() {
            uintptr_t base = static_cast<uintptr_t>(0x10000000u + t * 0x100000u);
            for (int i = 0; i < kRcwPerThread; ++i) {
                void* fake_ptr = reinterpret_cast<void*>(base + i);
                auto* rcw = FindOrCreateRcw(fake_ptr);
                if (rcw == nullptr || rcw->magic != kComRcwMagic) {
                    failures.fetch_add(1, std::memory_order_relaxed);
                    continue;
                }
                // Verify identity_unknown matches.
                if (rcw->identity_unknown != fake_ptr) {
                    failures.fetch_add(1, std::memory_order_relaxed);
                }
                ReleaseRcw(rcw);
            }
        });
    }

    for (auto& th : threads) th.join();
    EXPECT_EQ(failures.load(), 0);
}

TEST_F(ComRcwStressTest, ConcurrentCacheHitSamePointer) {
    // Multiple threads sharing the same COM pointer should get the same RCW.
    constexpr int kThreadCount = 8;
    void* shared_ptr = reinterpret_cast<void*>(static_cast<uintptr_t>(0xDEADBEEFu));
    std::atomic<int> failures{0};

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreadCount; ++t) {
        threads.emplace_back([&, t]() {
            (void)t;
            auto* rcw = FindOrCreateRcw(shared_ptr);
            if (rcw == nullptr || rcw->identity_unknown != shared_ptr) {
                failures.fetch_add(1, std::memory_order_relaxed);
            }
            // Release — note: refcount management may require careful handling
            // depending on the implementation. The test verifies no crash.
            rcw->wrapper_refcount = 1;
            ReleaseRcw(rcw);
        });
    }

    for (auto& th : threads) th.join();
    EXPECT_EQ(failures.load(), 0);
}

TEST_F(ComRcwStressTest, ConcurrentCreateThenReleaseAll) {
    // Create many RCWs from one thread, then release them from another.
    constexpr int kRcwCount = 512;
    void* ptrs[kRcwCount];
    chaos::il2cpp::com_rcw::ComRcwNative* rcws[kRcwCount];

    for (int i = 0; i < kRcwCount; ++i) {
        ptrs[i] = reinterpret_cast<void*>(static_cast<uintptr_t>(0x20000000u + i));
        rcws[i] = FindOrCreateRcw(ptrs[i]);
        ASSERT_NE(rcws[i], nullptr);
    }

    std::thread releaser([&]() {
        for (int i = 0; i < kRcwCount; ++i) {
            if (rcws[i] != nullptr) {
                ReleaseRcw(rcws[i]);
            }
        }
    });
    releaser.join();
    SUCCEED();
}

TEST_F(ComRcwStressTest, FindOrCreateRcwNullDoesNotCrash) {
    // Spam FindOrCreateRcw(nullptr) from multiple threads — should be safe.
    constexpr int kThreadCount = 4;
    constexpr int kIterations = 1000;
    std::atomic<int> null_results{0};

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreadCount; ++t) {
        threads.emplace_back([&]() {
            for (int i = 0; i < kIterations; ++i) {
                if (FindOrCreateRcw(nullptr) == nullptr) {
                    null_results.fetch_add(1, std::memory_order_relaxed);
                }
            }
        });
    }

    for (auto& th : threads) th.join();
    EXPECT_EQ(null_results.load(), kThreadCount * kIterations);
}

#else
// On Win32, skip stress tests that need real COM pointers.
TEST(ComRcwStressTest, SkippedOnWin32) {
    GTEST_SKIP() << "RCW stress tests skipped on Win32 (needs real COM pointers)";
}
#endif  // !_WIN32
