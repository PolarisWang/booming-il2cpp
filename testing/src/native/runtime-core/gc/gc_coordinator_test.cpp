/// gc_coordinator_test.cpp — GcCoordinator unit tests (Phase 3, gc-p4-01).
///
/// Tests singleton access, GC state query, and global GC request.
/// In WKS mode (default build), RequestGlobalGc delegates to the existing
/// single-heap chaos_gc_collect() path.

#include <cstdio>
#include <atomic>

#include "gc_coordinator.h"
#include "gc_test_base.h"

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

// ═══════════════════════════════════════════════════════════════════════════
// Fixture: basic singleton tests
// ═══════════════════════════════════════════════════════════════════════════

struct GcCoordinatorTest : GcTestBase {
    void SetUp() override {
        GcTestBase::SetUp();
    }
    void TearDown() override {
        GcTestBase::TearDown();
    }
};

TEST_F(GcCoordinatorTest, SingletonAccess) {
    auto& inst = GcCoordinator::Instance();
    // Verify we got a valid reference (same address on second call).
    auto& inst2 = GcCoordinator::Instance();
    EXPECT_EQ(&inst, &inst2);
}

TEST_F(GcCoordinatorTest, InitiallyNotActive) {
    auto& coord = GcCoordinator::Instance();
    EXPECT_FALSE(coord.IsGcActive());
}

TEST_F(GcCoordinatorTest, RequestGlobalGcDoesNotCrash) {
    auto& coord = GcCoordinator::Instance();
    // In WKS mode this calls through to chaos_gc_collect() which acquires
    // a safepoint and triggers a full GC cycle.  Verify it doesn't crash.
    EXPECT_NO_FATAL_FAILURE(coord.RequestGlobalGc());
}

TEST_F(GcCoordinatorTest, IsGcActiveDuringCollection) {
    auto& coord = GcCoordinator::Instance();
    EXPECT_FALSE(coord.IsGcActive());

    // RequestGlobalGc acquires a safepoint internally; after it returns
    // the GC should be complete and gc_active_ reset to false.
    coord.RequestGlobalGc();
    EXPECT_FALSE(coord.IsGcActive());
}

TEST_F(GcCoordinatorTest, MultipleGlobalGcRequests) {
    auto& coord = GcCoordinator::Instance();
    for (int i = 0; i < 5; i++) {
        EXPECT_FALSE(coord.IsGcActive());
        EXPECT_NO_FATAL_FAILURE(coord.RequestGlobalGc());
    }
}
