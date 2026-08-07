/// gc_finalizer_stress_test — Finalizer stress test (GoogleTest).
///
/// 5 scenarios converted from old contracts test.
/// All scenarios run in a single TEST_F to avoid per-test BGC/old-gen state issues.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

#include <chaos/native_types.h>
#include "gc_card_table.h"
#include "gc_old_gen.h"
#include "gc_region.h"
#include "gc_bgc.h"
#include "gc_scheduler.h"
#include "gc_test_base.h"
#include "thread_state.h"

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

extern "C" void chaos_gc_wait_for_pending_finalizers() noexcept;

namespace {

static std::atomic<int> g_finalizer_count{0};
static constexpr int kMaxTrackedObjects = 4096;
static std::atomic<int> g_fired_flags[kMaxTrackedObjects];
static std::atomic<int> g_resurrection_count{0};

static void InitTrackers() {
    g_finalizer_count.store(0);
    g_resurrection_count.store(0);
    for (int i = 0; i < kMaxTrackedObjects; i++)
        g_fired_flags[i].store(0);
}

static void FinalizerCallback(void* obj) {
    intptr_t id = reinterpret_cast<intptr_t>(obj);
    if (id >= 0 && id < kMaxTrackedObjects)
        g_fired_flags[id].store(1, std::memory_order_release);
    g_finalizer_count.fetch_add(1, std::memory_order_relaxed);
}

static void ResurrectionCallback(void* obj) {
    g_resurrection_count.fetch_add(1, std::memory_order_relaxed);
    g_old_gen.RegisterFinalizer(obj, ResurrectionCallback);
}

} // anonymous namespace

TEST_F(GcStressTestBase, FinalizerScenarios) {
    // One-time init (BGC + old-gen).
    GcSetHeapBase(reinterpret_cast<void*>(uintptr_t(0)));
    g_old_gen.Init(0, 2);
    BgcController::Instance().Start();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // ── F1: Basic finalizer ────────────────────────────────────────────
    {
        printf("  F1-BasicFinalizer ...\n");
        InitTrackers();
        static constexpr int kNumObjects = 100;
        std::vector<void*> objs;

        for (int i = 0; i < kNumObjects; i++) {
            void* obj = g_old_gen.Allocate(64, false);
            ASSERT_NE(obj, nullptr);
            objs.push_back(obj);
        }
        for (size_t i = 0; i < objs.size(); i++)
            g_old_gen.RegisterFinalizer(objs[i], FinalizerCallback);
        objs.clear();

        for (int i = 0; i < 500; i++) {
            volatile void* tmp = g_old_gen.Allocate(32, false);
            (void)tmp;
        }
        g_old_gen.Collect(nullptr, nullptr);
        chaos_gc_wait_for_pending_finalizers();
    }

    // ── F2: Reachability ──────────────────────────────────────────────
    {
        printf("  F2-FinalizerReachability ...\n");
        InitTrackers();
        static constexpr int kNumObjects = 50;
        std::vector<void*> kept;
        std::vector<void*> all;

        for (int i = 0; i < kNumObjects; i++) {
            void* obj = g_old_gen.Allocate(64, false);
            ASSERT_NE(obj, nullptr);
            g_old_gen.RegisterFinalizer(obj, FinalizerCallback);
            all.push_back(obj);
            if (i % 2 == 0) kept.push_back(obj);
        }

        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 200; j++) {
                volatile void* tmp = g_old_gen.Allocate(32, false);
                (void)tmp;
            }
            g_old_gen.Collect(nullptr, nullptr);
        }
        chaos_gc_wait_for_pending_finalizers();

        for (size_t i = 0; i < kept.size(); i++) {
            int idx = static_cast<int>(i * 2);
            EXPECT_EQ(g_fired_flags[idx].load(), 0) << "kept object finalized at idx " << idx;
        }
    }

    // ── F3: Overflow ─────────────────────────────────────────────────
    {
        printf("  F3-FinalizerOverflow ...\n");
        InitTrackers();
        static constexpr int kNumObjects = 500;
        std::vector<void*> objs;

        for (int i = 0; i < kNumObjects; i++) {
            void* obj = g_old_gen.Allocate(32, false);
            ASSERT_NE(obj, nullptr);
            objs.push_back(obj);
        }
        for (size_t i = 0; i < objs.size(); i++)
            g_old_gen.RegisterFinalizer(objs[i], FinalizerCallback);
        objs.clear();
        for (int i = 0; i < 1000; i++) {
            volatile void* tmp = g_old_gen.Allocate(32, false);
            (void)tmp;
        }
        g_old_gen.Collect(nullptr, nullptr);
        chaos_gc_wait_for_pending_finalizers();
    }

    // ── F4: Concurrent ────────────────────────────────────────────────
    {
        printf("  F4-ConcurrentFinalizer ...\n");
        InitTrackers();
        static constexpr int kNumThreads = 4;
        static constexpr int kAllocsPerThread = 200;
        std::atomic<bool> failed{false};

        auto worker = [&](int tid) {
            threading::RegisterThread(threading::AllocateThreadId(), nullptr);
            std::vector<void*> my_objs;
            for (int i = 0; i < kAllocsPerThread; i++) {
                void* obj = g_old_gen.Allocate(48 + (tid * 8) % 32, false);
                if (!obj) { failed.store(true); break; }
                my_objs.push_back(obj);
            }
            for (auto* obj : my_objs)
                g_old_gen.RegisterFinalizer(obj, FinalizerCallback);
            my_objs.clear();
            for (int i = 0; i < 200; i++) {
                volatile void* tmp = g_old_gen.Allocate(32, false);
                (void)tmp;
            }
            threading::UnregisterThread();
        };

        std::vector<std::thread> threads;
        for (int t = 0; t < kNumThreads; t++)
            threads.emplace_back(worker, t);
        for (auto& th : threads) th.join();
        ASSERT_FALSE(failed.load()) << "concurrent allocation failed";

        g_old_gen.Collect(nullptr, nullptr);
        chaos_gc_wait_for_pending_finalizers();
    }

    // ── F5: Resurrection ──────────────────────────────────────────────
    {
        printf("  F5-FinalizerResurrection ...\n");
        InitTrackers();
        void* obj = g_old_gen.Allocate(64, false);
        ASSERT_NE(obj, nullptr);

        g_old_gen.RegisterFinalizer(obj, ResurrectionCallback);

        for (int i = 0; i < 500; i++) {
            volatile void* tmp = g_old_gen.Allocate(32, false);
            (void)tmp;
        }
        g_old_gen.Collect(nullptr, nullptr);
        chaos_gc_wait_for_pending_finalizers();
        int after_first = g_resurrection_count.load();

        for (int i = 0; i < 500; i++) {
            volatile void* tmp = g_old_gen.Allocate(32, false);
            (void)tmp;
        }
        g_old_gen.Collect(nullptr, nullptr);
        chaos_gc_wait_for_pending_finalizers();

        EXPECT_GT(g_resurrection_count.load(), 0) << "resurrection finalizer never called";
        printf("    resurrection calls: first=%d total=%d\n", after_first, g_resurrection_count.load());
    }

    // Cleanup.
    chaos_gc_wait_for_pending_finalizers();
    BgcController::Instance().Stop();
}
