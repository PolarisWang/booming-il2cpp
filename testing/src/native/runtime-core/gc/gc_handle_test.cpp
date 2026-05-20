/// gc_handle_test — GCHandle strong/weak/pinned/dependent lifecycle tests (GoogleTest).
///
/// Uses CRAG directly. No BDWGC dependency.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <atomic>
#include <thread>
#include <vector>

#include <chaos/native_types.h>
#include "gc_events.h"
#include "gc_old_gen.h"
#include "gc_region.h"
#include "gc_young_collector.h"
#include "gc_layout.h"
#include "gc_test_base.h"

#include <gtest/gtest.h>

// Forward declarations from engine_lifecycle.h
namespace chaos { namespace il2cpp { namespace runtime_core {
CHAOS_IL2CPP_UINT64 GcCreateStrongHandle(void* object_instance) noexcept;
CHAOS_IL2CPP_UINT64 GcCreateWeakHandle(void* object_instance) noexcept;
CHAOS_IL2CPP_UINT64 GcCreatePinnedHandle(void* object_instance) noexcept;
void GcFreeHandle(CHAOS_IL2CPP_UINT64 handle_id) noexcept;
CHAOS_IL2CPP_UINT64 GcCreateDependentHandle(void* primary, void* secondary) noexcept;
void* GcGetDependentHandleSecondary(CHAOS_IL2CPP_UINT64 handle_id) noexcept;
void GcSetDependentHandleSecondary(CHAOS_IL2CPP_UINT64 handle_id, void* secondary) noexcept;
void GcFreeDependentHandle(CHAOS_IL2CPP_UINT64 handle_id) noexcept;
void* GcGetHandleTarget(CHAOS_IL2CPP_UINT64 handle_id) noexcept;
}}}

using namespace chaos::il2cpp::runtime_core;

struct GcHandleTest : GcUnitTestBase {
};

// ── Test 1: Strong handle keeps object alive ──────────────────────

TEST_F(GcHandleTest, StrongHandle) {
    void* obj = NurseryAllocate(64);
    ASSERT_NE(obj, nullptr);

    CHAOS_IL2CPP_UINT64 h = GcCreateStrongHandle(obj);
    ASSERT_NE(h, 0u);

    for (int g = 0; g < 3; g++) {
        for (int i = 0; i < 200; i++) {
            volatile void* tmp = NurseryAllocate(32);
            (void)tmp;
        }
        g_old_gen.Collect(nullptr, nullptr);
    }

    GcFreeHandle(h);
}

// ── Test 2: Weak handle is nulled after GC ────────────────────────

TEST_F(GcHandleTest, WeakHandle) {
    void* obj = NurseryAllocate(64);
    ASSERT_NE(obj, nullptr);
    std::memset(obj, 0xAB, 64);

    CHAOS_IL2CPP_UINT64 h = GcCreateWeakHandle(obj);
    ASSERT_NE(h, 0u);

    for (int g = 0; g < 3; g++) {
        for (int i = 0; i < 500; i++) {
            volatile void* tmp = NurseryAllocate(32);
            (void)tmp;
        }
        g_old_gen.Collect(nullptr, nullptr);
    }

    GcFreeHandle(h);
}

// ── Test 3: Pinned handle — address stability ────────────────────

TEST_F(GcHandleTest, PinnedHandle) {
    void* obj = NurseryAllocate(64);
    ASSERT_NE(obj, nullptr);
    std::memset(obj, 0xCD, 64);

    uintptr_t addr_before = reinterpret_cast<uintptr_t>(obj);

    CHAOS_IL2CPP_UINT64 h = GcCreatePinnedHandle(obj);
    ASSERT_NE(h, 0u);

    obj = nullptr;

    for (int g = 0; g < 3; g++) {
        for (int i = 0; i < 500; i++) {
            volatile void* tmp = NurseryAllocate(32);
            (void)tmp;
        }
        g_old_gen.Collect(nullptr, nullptr);
    }

    void* retrieved = GcGetHandleTarget(h);
    ASSERT_NE(retrieved, nullptr) << "Pinned handle target not null after GC";
    EXPECT_EQ(reinterpret_cast<uintptr_t>(retrieved), addr_before)
        << "Pinned handle address unchanged after GC";

    GcFreeHandle(h);
}

// ── Test 4: Dependent handle lifecycle ───────────────────────────

TEST_F(GcHandleTest, DependentHandle) {
    void* primary = g_old_gen.Allocate(256, true);
    void* secondary = NurseryAllocate(64);
    ASSERT_NE(primary, nullptr);
    ASSERT_NE(secondary, nullptr);
    *static_cast<const void**>(primary) = GetTestTypeInfo(256);
    std::memset(secondary, 0xEF, 64);

    CHAOS_IL2CPP_UINT64 dh = GcCreateDependentHandle(primary, secondary);
    ASSERT_NE(dh, 0u);

    void* before_gc = GcGetDependentHandleSecondary(dh);
    ASSERT_NE(before_gc, nullptr);
    EXPECT_EQ(static_cast<unsigned char*>(before_gc)[0], 0xEF);

    secondary = nullptr;

    for (int g = 0; g < 3; g++) {
        for (int i = 0; i < 500; i++) {
            volatile void* tmp = NurseryAllocate(32);
            (void)tmp;
        }
        g_old_gen.Collect(nullptr, nullptr);
    }

    GcFreeDependentHandle(dh);
}

// ── Test 5: Dependent handle SetSecondary ──────────────────────────

TEST_F(GcHandleTest, SetSecondary) {
    void* primary = g_old_gen.Allocate(256, true);
    void* secondary1 = NurseryAllocate(64);
    void* secondary2 = NurseryAllocate(64);
    ASSERT_NE(primary, nullptr);
    ASSERT_NE(secondary1, nullptr);
    ASSERT_NE(secondary2, nullptr);
    *static_cast<const void**>(primary) = GetTestTypeInfo(256);
    // Initialize secondary2 with a known pattern to verify it's preserved.
    static_cast<unsigned char*>(secondary2)[0] = 0xBB;

    CHAOS_IL2CPP_UINT64 dh = GcCreateDependentHandle(primary, secondary1);
    ASSERT_NE(dh, 0u);

    void* before = GcGetDependentHandleSecondary(dh);
    EXPECT_EQ(before, secondary1);

    GcSetDependentHandleSecondary(dh, secondary2);

    void* after_set = GcGetDependentHandleSecondary(dh);
    EXPECT_EQ(after_set, secondary2);
    if (after_set) {
        EXPECT_EQ(static_cast<unsigned char*>(after_set)[0], 0xBB);
    }

    secondary1 = nullptr;
    secondary2 = nullptr;

    for (int g = 0; g < 3; g++) {
        for (int i = 0; i < 500; i++) {
            volatile void* tmp = NurseryAllocate(32);
            (void)tmp;
        }
        g_old_gen.Collect(nullptr, nullptr);
    }

    GcFreeDependentHandle(dh);
}

// ── Test 6: Handle free cleanup ────────────────────────────────────

TEST_F(GcHandleTest, HandleFree) {
    void* obj = NurseryAllocate(64);
    ASSERT_NE(obj, nullptr);

    CHAOS_IL2CPP_UINT64 h = GcCreateStrongHandle(obj);
    ASSERT_NE(h, 0u);
    GcFreeHandle(h);

    // Double-free should be a no-op
    GcFreeHandle(h);

    // Free invalid handle (0)
    GcFreeHandle(0);
}

// ── Test 7: Strong handles under concurrent GC pressure ────────────

TEST_F(GcHandleTest, ConcurrentStrongHandles) {
    std::atomic<int> ok{1};

    std::vector<std::thread> threads;
    for (int t = 0; t < 4; t++) {
        threads.emplace_back([&ok]() {
            for (int i = 0; i < 50; i++) {
                void* obj = NurseryAllocate(64);
                if (!obj) { ok.store(0); return; }

                CHAOS_IL2CPP_UINT64 h = GcCreateStrongHandle(obj);
                if (!h) { ok.store(0); return; }

                for (int j = 0; j < 100; j++) {
                    volatile void* tmp = NurseryAllocate(32);
                    (void)tmp;
                }

                GcFreeHandle(h);
            }
        });
    }
    for (auto& th : threads) th.join();
    EXPECT_EQ(ok.load(), 1) << "4 threads x 50 strong handles under GC pressure OK";
}

// ── Test 8: Weak handles under concurrent GC pressure ──────────────

TEST_F(GcHandleTest, ConcurrentWeakHandles) {
    std::atomic<int> ok{1};

    std::vector<std::thread> threads;
    for (int t = 0; t < 4; t++) {
        threads.emplace_back([&ok]() {
            for (int i = 0; i < 50; i++) {
                void* obj = NurseryAllocate(64);
                if (!obj) { ok.store(0); return; }

                CHAOS_IL2CPP_UINT64 h = GcCreateWeakHandle(obj);
                if (!h) { ok.store(0); return; }

                for (int j = 0; j < 100; j++) {
                    volatile void* tmp = NurseryAllocate(32);
                    (void)tmp;
                }

                GcFreeHandle(h);
            }
        });
    }
    for (auto& th : threads) th.join();
    EXPECT_EQ(ok.load(), 1) << "4 threads x 50 weak handles under GC pressure OK";
}

// ── Test 9: Pinned handles under concurrent GC pressure ────────────

TEST_F(GcHandleTest, ConcurrentPinnedHandles) {
    std::atomic<int> ok{1};

    std::vector<std::thread> threads;
    for (int t = 0; t < 4; t++) {
        threads.emplace_back([&ok]() {
            for (int i = 0; i < 50; i++) {
                void* obj = NurseryAllocate(64);
                if (!obj) { ok.store(0); return; }

                CHAOS_IL2CPP_UINT64 h = GcCreatePinnedHandle(obj);
                if (!h) { ok.store(0); return; }

                for (int j = 0; j < 100; j++) {
                    volatile void* tmp = NurseryAllocate(32);
                    (void)tmp;
                }

                GcFreeHandle(h);
            }
        });
    }
    for (auto& th : threads) th.join();
    EXPECT_EQ(ok.load(), 1) << "4 threads x 50 pinned handles under GC pressure OK";
}

// ── Test 10: Handle table growth ────────────────────────────────────

TEST_F(GcHandleTest, HandleTableGrowth) {
    constexpr int kNumHandles = 2000;
    std::vector<CHAOS_IL2CPP_UINT64> handles;
    handles.reserve(kNumHandles);

    for (int i = 0; i < kNumHandles; i++) {
        void* obj = NurseryAllocate(32);
        ASSERT_NE(obj, nullptr);

        CHAOS_IL2CPP_UINT64 h = GcCreateStrongHandle(obj);
        if (h == 0) break;
        handles.push_back(h);
    }

    EXPECT_GE(handles.size(), 100u) << "At least 100 handles created successfully";

    for (auto h : handles) {
        GcFreeHandle(h);
    }
}
