/// gc_heap_manager_test.cpp — GcHeapManager unit tests (Phase 3, gc-p4-01).
///
/// Tests singleton access, initialization, and thread-to-heap mapping.
///
/// In WKS mode (default build, CHAOS_IL2CPP_GC_SERVER=0), Initialize()
/// sets heap_count_ to 1 and does NOT allocate the per-heap array. Only
/// basic lifecycle and thread-mapping tests are valid.
///
/// In Server GC mode (CHAOS_IL2CPP_GC_SERVER=1), per-heap array access
/// and iteration are also tested.

#include <cstdio>
#include <atomic>

#include "gc_heap_manager.h"
#include "gc_test_base.h"

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

// ═══════════════════════════════════════════════════════════════════════════
// Fixture
// ═══════════════════════════════════════════════════════════════════════════

struct GcHeapManagerTest : GcTestBase {
    void SetUp() override {
        GcTestBase::SetUp();
    }
    void TearDown() override {
        GcTestBase::TearDown();
    }
};

// ═══════════════════════════════════════════════════════════════════════════
// Singleton
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(GcHeapManagerTest, SingletonAccess) {
    auto& inst = GcHeapManager::Instance();
    auto& inst2 = GcHeapManager::Instance();
    EXPECT_EQ(&inst, &inst2);
}

// ═══════════════════════════════════════════════════════════════════════════
// Lifecycle — WKS mode always produces exactly 1 heap
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(GcHeapManagerTest, InitializeDefault) {
    auto& mgr = GcHeapManager::Instance();
    mgr.Initialize(0);  // auto-detect
    EXPECT_GE(mgr.HeapCount(), 1);
}

TEST_F(GcHeapManagerTest, HeapForCurrentThread) {
    auto& mgr = GcHeapManager::Instance();
    mgr.Initialize(0);

    int heap_id = mgr.HeapForCurrentThread();
    // In WKS mode always returns 0.
    EXPECT_EQ(heap_id, 0);
}

#if CHAOS_IL2CPP_GC_SERVER
// ── Server GC tests (require heap array allocation) ─────────────────

TEST_F(GcHeapManagerTest, InitializeExplicitCount) {
    auto& mgr = GcHeapManager::Instance();
    mgr.Initialize(2);
    EXPECT_EQ(mgr.HeapCount(), 2);
}

TEST_F(GcHeapManagerTest, GetHeapById) {
    auto& mgr = GcHeapManager::Instance();
    mgr.Initialize(2);

    GcHeapContext& h0 = mgr.GetHeap(0);
    EXPECT_EQ(h0.heap_id, 0);

    GcHeapContext& h1 = mgr.GetHeap(1);
    EXPECT_EQ(h1.heap_id, 1);
}

TEST_F(GcHeapManagerTest, ForEachHeapVisitsAll) {
    auto& mgr = GcHeapManager::Instance();
    mgr.Initialize(3);

    std::atomic<int> visited{0};
    mgr.ForEachHeap([&visited](int /*id*/, GcHeapContext& /*heap*/) {
        visited.fetch_add(1);
    });
    EXPECT_EQ(visited.load(), 3);
}

TEST_F(GcHeapManagerTest, ForEachHeapProvidesValidHeapIds) {
    auto& mgr = GcHeapManager::Instance();
    mgr.Initialize(3);

    std::atomic<bool> ids_ok{true};
    mgr.ForEachHeap([&ids_ok](int id, GcHeapContext& heap) {
        if (id != heap.heap_id) {
            ids_ok.store(false);
        }
    });
    EXPECT_TRUE(ids_ok.load());
}

// GC-N9 (=M3B): runtime dynamic heap-count adjustment (grow / shrink).
TEST_F(GcHeapManagerTest, AdjustHeapCountGrowShrink) {
    auto& mgr = GcHeapManager::Instance();
    mgr.Initialize(2);
    EXPECT_EQ(mgr.HeapCount(), 2);

    // Grow 2 → 4: heap count reflects the new target and existing ids preserved.
    EXPECT_TRUE(mgr.AdjustHeapCount(4));
    EXPECT_EQ(mgr.HeapCount(), 4);
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(mgr.GetHeap(i).heap_id, i) << "heap id preserved on grow";
    }

    // No-op when already at target.
    EXPECT_FALSE(mgr.AdjustHeapCount(4));

    // Shrink 4 → 1: bounded to >= 1, surviving heap still id 0.
    EXPECT_TRUE(mgr.AdjustHeapCount(1));
    EXPECT_EQ(mgr.HeapCount(), 1);
    EXPECT_EQ(mgr.GetHeap(0).heap_id, 0);

    // Below-min input clamps to 1; already at 1 → no-op returns false.
    EXPECT_FALSE(mgr.AdjustHeapCount(-5));

    // Oversized input clamps to the cap and does not exceed the bound.
    EXPECT_TRUE(mgr.AdjustHeapCount(2000));  // clamped to kMaxServerHeaps
    EXPECT_LE(mgr.HeapCount(), 64);
    EXPECT_FALSE(mgr.AdjustHeapCount(2000));  // at cap → unchanged → no-op
}
#endif  // CHAOS_IL2CPP_GC_SERVER
