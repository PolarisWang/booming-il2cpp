// com_ccw_stress_test.cpp — Concurrent CCW AddRef/Release/QI stress tests.
//
// Spawns multiple threads that concurrently manipulate CCW reference counts
// and query interfaces to verify thread safety.

#include "marshal_test_fixture.h"

#include <com_ccw.h>

#include <atomic>
#include <thread>
#include <vector>

using namespace chaos::il2cpp::com_ccw;

// ════════════════════════════════════════════════════════════════════════════
// S2 — CCW stress tests
// ════════════════════════════════════════════════════════════════════════════

class ComCcwStressTest : public MarshalTestFixture {
protected:
    // Local CCW matching the committed ComCcw layout (x64).
    // interface_capacity + interfaces* + inline_interfaces[4] layout.
    struct ComCcwOld {
        ComCcwVtbl* vtable;
        std::atomic<CHAOS_IL2CPP_UINT32> refcount;
        CHAOS_IL2CPP_UINT64 gc_handle;
        void* runtime_state;
        CHAOS_IL2CPP_SIZE interface_count;
        CHAOS_IL2CPP_SIZE interface_capacity;
        void* outer_unknown;
        bool  is_aggregated;
        ComCcwInterfaceEntry* interfaces;
        ComCcwInterfaceEntry inline_interfaces[4];
        const void* typelib_data;
        void* cp_container;
    };

    struct TestCcwGuard {
        ComCcwOld ccw;

        void* self() noexcept { return &ccw; }

        TestCcwGuard() {
            std::memset(&ccw, 0, sizeof(ccw));
            ccw.vtable = &s_test_vtbl_;
            ccw.refcount = 1;  // prevent free on Release
            ccw.interface_count = 1;
            ccw.interface_capacity = 4;
            ccw.interfaces = &ccw.inline_interfaces[0];
            ccw.inline_interfaces[0].guid = &kZeroGuid[0];
            ccw.inline_interfaces[0].vtable = &s_test_vtbl_;
            ccw.inline_interfaces[0].ccw_ptr = &ccw;
        }
    };

    static ComCcwVtbl s_test_vtbl_;
    static const CHAOS_IL2CPP_UINT8 kZeroGuid[16];
};

ComCcwVtbl ComCcwStressTest::s_test_vtbl_ = {
    &CcwQueryInterface,
    &CcwAddRef,
    &CcwRelease,
};

const CHAOS_IL2CPP_UINT8 ComCcwStressTest::kZeroGuid[16] = {0};

TEST_F(ComCcwStressTest, ConcurrentAddRefRelease) {
    // Multiple threads performing AddRef/Release on the same CCW.
    constexpr int kThreadCount = 8;
    constexpr int kOpsPerThread = 10000;
    TestCcwGuard guard;
    void* s = guard.self();
    std::atomic<int> failures{0};

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreadCount; ++t) {
        threads.emplace_back([s, &failures]() {
            for (int i = 0; i < kOpsPerThread; ++i) {
                CcwAddRef(s);
                CcwRelease(s);
            }
        });
    }

    for (auto& th : threads) th.join();

    // After all threads, refcount should be back to 1.
    EXPECT_EQ(guard.ccw.refcount.load(), 1u);
    EXPECT_EQ(failures.load(), 0);
}

TEST_F(ComCcwStressTest, ConcurrentQueryInterface) {
    // Multiple threads QI for the same interface simultaneously.
    constexpr int kThreadCount = 8;
    TestCcwGuard guard;
    void* s = guard.self();
    std::atomic<int> successes{0};

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreadCount; ++t) {
        threads.emplace_back([s, &successes]() {
            void* ppv = nullptr;
            auto hr = CcwQueryInterface(s, ComCcwStressTest::kZeroGuid, &ppv);
            if (hr == kS_OK && ppv != nullptr) {
                successes.fetch_add(1, std::memory_order_relaxed);
                CcwRelease(ppv);
            }
        });
    }

    for (auto& th : threads) th.join();
    EXPECT_EQ(successes.load(), kThreadCount);
}

TEST_F(ComCcwStressTest, ConcurrentMixedAddRefReleaseQi) {
    // Mixed workload: AddRef, Release, QI from all threads.
    constexpr int kThreadCount = 8;
    constexpr int kOpsPerThread = 5000;
    TestCcwGuard guard;
    void* s = guard.self();
    std::atomic<int> errors{0};

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreadCount; ++t) {
        threads.emplace_back([s, &errors, t]() {
            const CHAOS_IL2CPP_UINT8 test_guid[16] = {
                static_cast<CHAOS_IL2CPP_UINT8>(t + 1)
            };
            void* fake_vtbl = reinterpret_cast<void*>(
                static_cast<uintptr_t>(0x5000 + t));
            RegisterCcwInterface(s, test_guid, fake_vtbl);
        });
    }
    for (auto& th : threads) th.join();

    // Now QI for all registered interfaces concurrently.
    for (int t = 0; t < kThreadCount; ++t) {
        threads.emplace_back([s, &errors, t]() {
            const CHAOS_IL2CPP_UINT8 test_guid[16] = {
                static_cast<CHAOS_IL2CPP_UINT8>(t + 1)
            };
            for (int i = 0; i < kOpsPerThread; ++i) {
                void* ppv = nullptr;
                auto hr = CcwQueryInterface(s, test_guid, &ppv);
                if (hr == kS_OK && ppv != nullptr) {
                    CcwRelease(ppv);
                } else {
                    errors.fetch_add(1, std::memory_order_relaxed);
                }
            }
        });
    }

    for (auto& th : threads) th.join();
    EXPECT_EQ(errors.load(), 0);
}

TEST_F(ComCcwStressTest, NullSafetyConcurrent) {
    // Concurrent calls with null pointers must not crash.
    constexpr int kThreadCount = 4;
    constexpr int kIterations = 1000;
    std::atomic<int> done{0};

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreadCount; ++t) {
        threads.emplace_back([&done]() {
            for (int i = 0; i < kIterations; ++i) {
                CcwQueryInterface(nullptr, nullptr, nullptr);
                CcwAddRef(nullptr);
                CcwRelease(nullptr);
                CcwFromInterface(nullptr);
            }
            done.fetch_add(1, std::memory_order_relaxed);
        });
    }

    for (auto& th : threads) th.join();
    EXPECT_EQ(done.load(), kThreadCount);
}
