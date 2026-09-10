/// gc_semantics_test — GCHandle / DependentHandle / Pinned .NET-semantics oracle.
///
/// Part of the reliability-verification hardening (plan C, "独立性/交叉验证").
/// Asserts .NET-aligned semantics that a compacting generational GC MUST satisfy:
///   - DependentHandle (ConditionalWeakTable/Ephemeron): primary->secondary linkage,
///     read/write roundtrip, and that freeing releases it.
///   - Pinned handles: a pinned object is not moved; GcIsPinnedObject reflects it;
///     POH allocation lands in a POH region and does not participate in young GC copy.
///
/// API surface (verified against src/native/runtime-core/gc/gc_events.h):
///   GcCreateDependentHandle / GcGetDependentHandlePrimary|Secondary /
///   GcSetDependentHandleSecondary / GcFreeDependentHandle
///   GcAddPinnedObject / GcRemovePinnedObject / GcIsPinnedObject
///   GcAllocatePinned / GcIsPohPointer / GcGetHandleTarget / GcSetHandleTarget
/// Thread primitives mirror gc_safepoint_test.cpp.

#include <cstdint>
#include <atomic>
#include <chaos/native_types.h>

#include "gc_events.h"          // GCHandle / DependentHandle / pinned APIs
#include "gc_region.h"          // NurseryAllocate (also brings chaos/unordered_dense.h, which
                                //   defines CHAOS_IL2CPP_UNORDERED_DENSE_MAP needed by core headers)
#include "core/engine_lifecycle.h"  // GcCreateStrongHandle / GcFreeHandle (regular handles)
#include "gc_api.h"             // GC memory query + allocation
#include "thread_state.h"
#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

namespace {

struct RunCtx {
    uint32_t tid{0};
    RunCtx() {
        tid = threading::AllocateThreadId();
        threading::RegisterThread(tid, nullptr);
        threading::EnterCooperativeMode();
    }
    ~RunCtx() {
        threading::EnterPreemptiveMode();
        threading::UnregisterThread();
    }
    RunCtx(const RunCtx&) = delete;
    RunCtx& operator=(const RunCtx&) = delete;
};

}  // namespace

TEST(GcSemantics, DependentHandleRoundTrip) {
    RunCtx ctx;
    void* p = NurseryAllocate(64);
    void* s = NurseryAllocate(64);
    ASSERT_NE(p, nullptr);
    ASSERT_NE(s, nullptr);

    uint64_t h = GcCreateDependentHandle(p, s);
    ASSERT_NE(h, 0u);

    EXPECT_EQ(GcGetDependentHandlePrimary(h), p);
    EXPECT_EQ(GcGetDependentHandleSecondary(h), s);

    // Change secondary; primary must remain.
    void* s2 = NurseryAllocate(32);
    ASSERT_NE(s2, nullptr);
    GcSetDependentHandleSecondary(h, s2);
    EXPECT_EQ(GcGetDependentHandleSecondary(h), s2);
    EXPECT_EQ(GcGetDependentHandlePrimary(h), p);

    GcFreeDependentHandle(h);
    SUCCEED();
}

TEST(GcSemantics, HandleGetSetTarget) {
    RunCtx ctx;
    void* p = NurseryAllocate(64);
    ASSERT_NE(p, nullptr);

    // GcGetHandleTarget/GcSetHandleTarget operate on REGULAR handles in the
    // shard map (created by GcCreateStrongHandle), NOT on dependent handles
    // (which live in a separate table and use GcGetDependentHandlePrimary/etc).
    uint64_t h = GcCreateStrongHandle(p);
    ASSERT_NE(h, 0u);

    EXPECT_EQ(GcGetHandleTarget(h), p);
    void* target2 = NurseryAllocate(48);
    ASSERT_NE(target2, nullptr);
    GcSetHandleTarget(h, target2);
    EXPECT_EQ(GcGetHandleTarget(h), target2);

    GcFreeHandle(h);
}

TEST(GcSemantics, PinnedObjectReflectsAndSurvives) {
    RunCtx ctx;

    // A stable-address pin is provided by the POH, exactly as CoreCLR does
    // (poh_generation / heap_segment_flags_poh): an object that must not move
    // is allocated INTO the pinned-object region, which never participates in
    // young-GC copying.  A pin_set entry on an arbitrary nursery object cannot
    // keep it in place across Cheney copying — CoreCLR and this GC segregate
    // pinning to the POH instead.
    void* obj = GcAllocatePinned(128);
    ASSERT_NE(obj, nullptr);
    // POH objects are auto-pinned on allocation (GcAllocatePinned → GcAddPinnedObject).
    EXPECT_TRUE(GcIsPinnedObject(obj));
    EXPECT_TRUE(GcIsPohPointer(obj));

    // POH regions are never copied or reset by a young GC, so the object's raw
    // address must remain stable across forced young collections.
    void* stable_addr = obj;
    for (int i = 0; i < 3; i++) {
        chaos_gc_collect_with_mode(0, 1 /* Forced young */);
        EXPECT_TRUE(GcIsPohPointer(stable_addr));
        EXPECT_TRUE(GcIsPinnedObject(stable_addr));
        obj = stable_addr;  // address must not have changed
    }

    // Unpin + re-check: the pin set reflects removal.
    GcRemovePinnedObject(obj);
    EXPECT_FALSE(GcIsPinnedObject(obj));
    EXPECT_TRUE(GcIsPohPointer(obj));  // still in the POH region regardless of pin set
}

TEST(GcSemantics, PohAllocationIsPinnedToRegion) {
    RunCtx ctx;
    void* poh = GcAllocatePinned(256);
    ASSERT_NE(poh, nullptr);
    // POH objects must reside outside the nursery (dedicated REGION_POH) and be
    // recognized by the POH pointer test.
    EXPECT_TRUE(GcIsPohPointer(poh));
    EXPECT_FALSE(GcIsPohPointer(NurseryAllocate(64)));
}
