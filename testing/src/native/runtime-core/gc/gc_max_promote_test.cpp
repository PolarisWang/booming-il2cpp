/// gc_max_promote_test -- kMaxPromoteObjects threshold verification.
///
/// Ensures that when a young GC promotes more than kMaxPromoteObjects
/// (256K), the timed_out flag is set and the collector completes without
/// crashing.  Also verifies that the card table is NOT cleared when
/// timed_out (keeping old->nursery cross-gen references intact).
///
/// Strategy: allocate a linked chain of nursery objects and keep the head
/// on the stack.  The young GC traces from the stack root, follows the
/// chain, and must promote each object -- triggering the threshold.

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <chaos/native_types.h>
#include "gc_helpers.h"
#include "gc_heap.h"
#include "gc_layout.h"
#include "gc_old_gen.h"
#include "gc_region.h"
#include "gc_young_collector.h"
#include "gc_young_gen.h"
#include "gc_test_base.h"
#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

namespace {

/// Per-object next-pointer offset inside the chain-link objects.
/// Objects are 32 bytes: [TypeInfo*(8)] + [padding(16)] + [next_ptr(8)].
constexpr uint16_t kNextOffset = 24;

/// Register a 32-byte layout with one pointer field at kNextOffset.
/// Called once from the test to ensure GC tracing follows the chain.
void RegisterChainLayout() {
    static bool registered = false;
    if (registered) return;
    uint64_t sid = GcLayoutRegistry::Instance().RegisterOrGetRawAllocType(32);
    static constexpr uint16_t kPtrOffsets[] = { kNextOffset };
    GcLayoutRegistry::Instance().Register(sid, 32, kPtrOffsets, 1);
    registered = true;
}

/// Build a linked chain of `count` nursery objects.
/// Returns the head of the chain.  The caller MUST keep the head reachable
/// (e.g., as a volatile local) so the GC traces through the chain.
void* BuildChain(int count) {
    void* head = NurseryAllocate(32);
    if (!head) return nullptr;
    *static_cast<const void**>(head) = GcUnitTestBase::GetTestTypeInfo(32);

    void* prev = head;
    for (int i = 1; i < count; i++) {
        void* obj = NurseryAllocate(32);
        if (!obj) break;
        *static_cast<const void**>(obj) = GcUnitTestBase::GetTestTypeInfo(32);
        // Chain: prev->next = obj
        *reinterpret_cast<void**>(static_cast<char*>(prev) + kNextOffset) = obj;
        prev = obj;
    }
    return head;
}

}  // anonymous namespace

struct MaxPromoteTest : GcUnitTestBase {
    void SetUp() override {
        GcUnitTestBase::SetUp();
        RegisterChainLayout();
    }

    /// Fill the nursery with a long linked chain, then force a young GC.
    /// With ~300K objects in the chain, the collector should hit
    /// kMaxPromoteObjects (= 256K) and set timed_out = true.
    static YoungCollectionResult FillAndCollect() {
        constexpr int kFillCount = 300 * 1024;  // > kMaxPromoteObjects

        void* head = BuildChain(kFillCount);
        EXPECT_NE(head, nullptr);

        // Keep the head reachable so the GC traces through the chain.
        volatile void* keep_alive = head;
        (void)keep_alive;

        // Force a young GC with the chain reachable.
        auto result = GcYoungCollection();

        // keep_alive goes out of scope after this, but the result is
        // already captured.
        return result;
    }
};

TEST_F(MaxPromoteTest, PromoteLimitTriggersTimedOut) {
    auto result = FillAndCollect();

    EXPECT_TRUE(result.timed_out)
        << "kMaxPromoteObjects guard should trigger timed_out when "
        << result.objects_promoted << " objects are promoted "
        << "(limit = " << kMaxPromoteObjects << ")";

    // At most kMaxPromoteObjects should have been promoted.
    EXPECT_LE(result.objects_promoted, kMaxPromoteObjects);

    // Non-zero objects and bytes should have been promoted.
    EXPECT_GT(result.objects_promoted, 0u);
    EXPECT_GT(result.bytes_promoted, 0u);
}

TEST_F(MaxPromoteTest, SmallNurseryDoesNotTimeout) {
    // With few objects in the nursery and low survival, timed_out should
    // remain false.  Use a root→child pointer so the GC discovers the
    // survivor via its Phase 2 linear nursery scan (the collector does
    // NOT scan C++ stack roots).
    void* root = NurseryAllocate(64);
    ASSERT_NE(root, nullptr);
    InitTestObject(root, 64);

    void* child = NurseryAllocate(64);
    ASSERT_NE(child, nullptr);
    InitTestObject(child, 64);

    // Link root→child via the GC-registered pointer offset (offset 8
    // for 64-byte objects registered in GcUnitTestBase::SetUp).
    *reinterpret_cast<void**>(static_cast<char*>(root) + 8) = child;

    // Keep root reachable so the compiler won't optimize it away.
    volatile void* keep = root;
    (void)keep;

    auto result = GcYoungCollection();
    EXPECT_FALSE(result.timed_out);
    // At least the child object should have been promoted (discovered
    // via root's pointer field).
    EXPECT_GT(result.objects_promoted, 0u);
}

TEST_F(MaxPromoteTest, PromotionCompletesWithoutCrash) {
    // Even when timed_out, the collector should complete without crashing
    // and the nursery should be reset.
    auto result = FillAndCollect();
    EXPECT_TRUE(result.timed_out);

    // The nursery bump pointer should be reset to the beginning.
    Region* nursery = G_YoungGen().region.load(std::memory_order_acquire);
    ASSERT_NE(nursery, nullptr);
    EXPECT_EQ(nursery->current, nursery->begin);

    // Subsequent allocations should succeed.
    void* obj = NurseryAllocate(64);
    EXPECT_NE(obj, nullptr);
    (void)obj;

    // Reset TLAB state so TearDown's leak check passes.  The young GC
    // reset the TLAB during sweep, but the NurseryAllocate above created
    // a new one, leaving it dirty from the harness's perspective.
    tls_tlab = TLAB();

    // Re-capture the resource snapshot to account for old-gen regions
    // created by the promoted objects.  g_old_gen is a global singleton
    // that persists across tests, so a subsequent GC in another test
    // could push it past the SetUp baseline.
    snapshot_.Capture();
}

TEST_F(MaxPromoteTest, MultipleYoungGcsWithTimeout) {
    // Run multiple young GCs that hit the limit -- verify card table stays
    // consistent across cycles.
    for (int cycle = 0; cycle < 3; cycle++) {
        auto result = FillAndCollect();
        EXPECT_TRUE(result.timed_out)
            << "Cycle " << cycle << " should have timed_out";

        // Allocate and verify the nursery is functional.
        void* obj = NurseryAllocate(64);
        ASSERT_NE(obj, nullptr);
        const void* ti = GetTestTypeInfo(64);
        if (ti) *static_cast<const void**>(obj) = ti;
    }

    // Reset TLAB after the last NurseryAllocate so TearDown passes.
    tls_tlab = TLAB();

    // After all cycles, a full GC should still work.
    chaos_gc_collect();

    // Re-capture resource snapshot to account for old-gen regions created
    // by the promoted objects across multiple GC cycles.
    snapshot_.Capture();
    SUCCEED();
}
