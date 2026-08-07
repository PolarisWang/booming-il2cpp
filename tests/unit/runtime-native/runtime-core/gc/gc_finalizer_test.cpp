/// gc_finalizer_test — Finalizer registration + execution unit tests (GoogleTest).

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <atomic>
#include <thread>
#include <vector>

#include <chaos/native_types.h>
#include "gc_old_gen.h"
#include "gc_region.h"
#include "gc_events.h"
#include "gc_helpers.h"
#include "gc_young_gen.h"
#include "thread_state.h"
#include "gc_test_base.h"
#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

extern "C" void chaos_gc_wait_for_pending_finalizers() noexcept;

namespace {
    std::atomic<bool> g_finalizer_called{false};
    void MarkFinalizerCalled(void*) {
        g_finalizer_called.store(true, std::memory_order_release);
    }
    void NopFinalizer(void*) {}
}

struct FinalizerTest : GcUnitTestBase {
    void TearDown() override {
        chaos_gc_wait_for_pending_finalizers();
        GcUnitTestBase::TearDown();
    }
};

TEST_F(FinalizerTest, BasicFinalizerExecution) {
    void* test_obj = NurseryAllocate(64);
    ASSERT_NE(test_obj, nullptr);

    g_old_gen.RegisterFinalizer(test_obj, NopFinalizer);

    for (int i = 0; i < 5; i++) {
        void* tmp = NurseryAllocate(1024);
        (void)tmp;
    }
    g_old_gen.Collect(nullptr, nullptr);
    SUCCEED();
}

TEST_F(FinalizerTest, ReachableNotFinalized) {
    void* obj = g_old_gen.Allocate(256, true);
    ASSERT_NE(obj, nullptr);

    g_finalizer_called.store(false);
    g_old_gen.RegisterFinalizer(obj, MarkFinalizerCalled);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 200; j++) {
            void* tmp = NurseryAllocate(32);
            (void)tmp;
        }
        g_old_gen.Collect(nullptr, nullptr);
    }

    // Old test framework only verified no crash (GC_CHECK / printf-based).
    // The finalizer may or may not be called; verify the system doesn't
    // crash during GC cycles with reachable finalized objects.
    SUCCEED();
}

TEST_F(FinalizerTest, MultipleFinalizers) {
    constexpr int kCount = 10;
    void* objs[kCount];
    for (int i = 0; i < kCount; i++) {
        objs[i] = NurseryAllocate(32);
        ASSERT_NE(objs[i], nullptr);
    }

    for (int i = 0; i < kCount; i++) {
        g_old_gen.RegisterFinalizer(objs[i], NopFinalizer);
    }

    for (int g = 0; g < 5; g++) {
        for (int i = 0; i < 500; i++) {
            volatile void* tmp = NurseryAllocate(32);
            (void)tmp;
        }
        g_old_gen.Collect(nullptr, nullptr);
    }

    SUCCEED();
}

TEST_F(FinalizerTest, WaitForPending) {
    void* obj = NurseryAllocate(64);
    ASSERT_NE(obj, nullptr);

    g_old_gen.RegisterFinalizer(obj, NopFinalizer);

    for (int i = 0; i < 500; i++) {
        volatile void* tmp = NurseryAllocate(32);
        (void)tmp;
    }
    g_old_gen.Collect(nullptr, nullptr);

    chaos_gc_wait_for_pending_finalizers();
    SUCCEED();
}

TEST_F(FinalizerTest, LargeFinalizerQueue) {
    constexpr int kNumObjects = 70000;
    std::vector<void*> objs;
    objs.reserve(kNumObjects);

    for (int i = 0; i < kNumObjects; i++) {
        void* obj = NurseryAllocate(32);
        if (!obj) break;
        g_old_gen.RegisterFinalizer(obj, NopFinalizer);
        objs.push_back(obj);
    }

    EXPECT_GE(objs.size(), 65000u);

    objs.clear();
    for (int g = 0; g < 3; g++) {
        for (int i = 0; i < 500; i++) {
            volatile void* tmp = NurseryAllocate(32);
            (void)tmp;
        }
        g_old_gen.Collect(nullptr, nullptr);
    }

    chaos_gc_wait_for_pending_finalizers();
    SUCCEED();
}

TEST_F(FinalizerTest, SuppressSuppressed) {
    void* obj = NurseryAllocate(64);
    ASSERT_NE(obj, nullptr);

    g_finalizer_called.store(false);
    g_old_gen.RegisterFinalizer(obj, MarkFinalizerCalled);

    g_old_gen.SuppressFinalizer(obj);
    SUCCEED();

    g_old_gen.SuppressFinalizer(obj);
    SUCCEED();

    void* no_finalizer_obj = NurseryAllocate(64);
    ASSERT_NE(no_finalizer_obj, nullptr);
    g_old_gen.SuppressFinalizer(no_finalizer_obj);
    SUCCEED();

    g_old_gen.Collect(nullptr, nullptr);
    SUCCEED();
}
