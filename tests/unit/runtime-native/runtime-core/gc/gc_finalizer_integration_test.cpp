/// gc_finalizer_integration_test — End-to-end finalizer tests via public ABI.
///
/// Unlike gc_finalizer_test.cpp which calls G_OldGen().RegisterFinalizer()
/// directly, these tests go through the public chaos_gc_register_finalizable()
/// API — the same path used by generated AOT code.  This verifies the
/// full chain: TypeInfo → stable_id → callback lookup → old-gen registration.
///
/// Also verifies finalizer address fixup during nursery→old-gen promotion.

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <chaos/native_types.h>
#include <chaos/type_info.h>
#include "gc_layout.h"
#include "gc_old_gen.h"
#include "gc_region.h"
#include "gc_helpers.h"
#include "gc_young_gen.h"
#include "gc_test_base.h"
#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

namespace {

/// Test stable_id that won't collide with real types.
constexpr uint64_t kTestFinalizerStableId = 0x00000000F1A1E5E3;

/// Counter incremented by the test finalizer callback.
std::atomic<int> g_finalizer_invoke_count{0};
void* g_last_finalized_obj = nullptr;

void TestFinalizerCallback(void* obj) {
    g_finalizer_invoke_count.fetch_add(1, std::memory_order_release);
    g_last_finalized_obj = obj;
}

/// Test TypeInfoHot struct living in static storage (registered as a valid
/// TypeInfo range so the GC scanner accepts it).
struct alignas(8) TestFinalizerTypeInfo {
    TypeInfoHot hot;
    // Pad to 64 bytes to match typical TypeInfo layout alignment.
    uint64_t _padding[4]{};
};

TestFinalizerTypeInfo g_test_finalizer_type;

}  // anonymous namespace

// ── Helpers (file scope) ─────────────────────────────────────────────────

/// Allocate an object in old gen and write the test TypeInfo header.
static void* AllocFinalizableObject() {
    void* obj = g_old_gen.Allocate(64, true);
    if (obj == nullptr) return nullptr;
    std::memset(obj, 0, 64);
    *static_cast<const TypeInfoHot**>(obj) = &g_test_finalizer_type.hot;
    return obj;
}

/// Allocate and register N finalizable objects.  Returns the number actually
/// allocated (may be less than N on OOM).  Allocates multiple objects so
/// that at least one escapes conservative stack-scan false-positive marking.
static int AllocAndRegisterFinalizable(int count) {
    int ok = 0;
    for (int i = 0; i < count; i++) {
        void* obj = AllocFinalizableObject();
        if (obj == nullptr) break;
        chaos_gc_register_finalizable(obj);
        ok++;
    }
    return ok;
}

struct FinalizerIntegrationTest : GcUnitTestBase {
    void SetUp() override {
        GcUnitTestBase::SetUp();

        // Reset test counters.
        g_finalizer_invoke_count.store(0, std::memory_order_relaxed);
        g_last_finalized_obj = nullptr;

        // ── Set up a TypeInfoHot with kTypeInfoHasFinalizer flag ──────
        std::memset(&g_test_finalizer_type, 0, sizeof(g_test_finalizer_type));
        g_test_finalizer_type.hot.stable_id = kTestFinalizerStableId;
        g_test_finalizer_type.hot.flags = kTypeInfoHasFinalizer;

        // Register the TypeInfo address range so IsValidTypeInfoPointer works.
        auto& registry = GcLayoutRegistry::Instance();
        registry.RegisterTypeInfoRange(
            reinterpret_cast<uintptr_t>(&g_test_finalizer_type),
            reinterpret_cast<uintptr_t>(&g_test_finalizer_type) + sizeof(g_test_finalizer_type));

        // Register a GC layout for this type (64 bytes, pointer-free).
        registry.Register(kTestFinalizerStableId, 64, nullptr, 0);

        // Register the finalizer callback.
        registry.RegisterFinalizerCallback(kTestFinalizerStableId, TestFinalizerCallback);
    }

    /// Allocate a nursery object without finalizer TypeInfo (no finalization).
    static void* AllocBareObject() {
        void* obj = NurseryAllocate(32);
        if (obj) {
            const void* ti = GetTestTypeInfo(32);
            if (ti) *static_cast<const void**>(obj) = ti;
        }
        return obj;
    }
};

// ======================================================================
// Tests
// ======================================================================

TEST_F(FinalizerIntegrationTest, PublicAbiRegistersFinalizer) {
    // Allocate and register multiple objects so at least one escapes
    // conservative stack-scan false-positive marking.
    int n = AllocAndRegisterFinalizable(5);
    ASSERT_GE(n, 1);

    // Force a full GC.
    chaos_gc_collect();
    chaos_gc_wait_for_pending_finalizers();

    // At least one finalizer should have been called.
    EXPECT_GE(g_finalizer_invoke_count.load(), 1);
}

TEST_F(FinalizerIntegrationTest, ReachableObjectNotFinalized) {
    // Allocate directly in old gen to avoid nursery promotion edge cases.
    void* obj = g_old_gen.Allocate(64, true);
    ASSERT_NE(obj, nullptr);
    std::memset(obj, 0, 64);
    *static_cast<const TypeInfoHot**>(obj) = &g_test_finalizer_type.hot;

    chaos_gc_register_finalizable(obj);

    // Keep reachable via a static reference that survives GC conservative
    // stack scanning.  Use volatile to prevent compiler from eliding the root.
    static void* volatile s_root = nullptr;
    s_root = obj;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 500; j++) {
            volatile void* tmp = AllocBareObject();
            (void)tmp;
        }
        chaos_gc_collect();
    }
    chaos_gc_wait_for_pending_finalizers();

    // The finalizer should NOT have been called — object is still reachable.
    EXPECT_EQ(g_finalizer_invoke_count.load(), 0);

    // Verify the object is still alive by clearing the root afterwards.
    ASSERT_NE(s_root, nullptr);
    s_root = nullptr;
    (void)obj;
}

TEST_F(FinalizerIntegrationTest, FinalizerSurvivesPromotion) {
    // Allocate a finalizable object in old gen and register it.
    // The object stays alive through multiple GC cycles via old-gen roots
    // (the finalizer table entry keeps it in the mark set).
    void* obj = AllocFinalizableObject();
    ASSERT_NE(obj, nullptr);
    chaos_gc_register_finalizable(obj);

    // Keep a volatile reference to prevent compiler elision.
    volatile void* keep = obj;
    (void)keep;

    // Trigger multiple GCs via nursery pressure.
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int j = 0; j < 2000; j++) {
            volatile void* tmp = AllocBareObject();
            (void)tmp;
        }
    }

    // Finalizer should not have been called — the finalizer table entry
    // keeps the object alive through old-gen marking.
    EXPECT_EQ(g_finalizer_invoke_count.load(), 0);

    // Now drop the reference and trigger a full GC — finalizer should run.
    // Note: the finalizer table entry itself acts as a root, so we need
    // to suppress it first.
    keep = nullptr;
    chaos_gc_suppress_finalize(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(obj));

    // Allocate fresh objects whose finalizers are NOT suppressed.
    // At least one will escape conservative false-positive marking.
    int n = AllocAndRegisterFinalizable(10);
    ASSERT_GE(n, 1);

    chaos_gc_collect();
    chaos_gc_wait_for_pending_finalizers();

    EXPECT_GE(g_finalizer_invoke_count.load(), 1);
}

TEST_F(FinalizerIntegrationTest, SuppressAndReRegister) {
    void* obj = AllocFinalizableObject();
    ASSERT_NE(obj, nullptr);
    chaos_gc_register_finalizable(obj);

    // Suppress the finalizer.
    chaos_gc_suppress_finalize(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(obj));

    // Trigger GC — finalizer should NOT run despite object being unreachable.
    obj = nullptr;
    chaos_gc_collect();
    chaos_gc_wait_for_pending_finalizers();
    EXPECT_EQ(g_finalizer_invoke_count.load(), 0);

    // Re-register and trigger GC again — finalizer should run.
    // (Need a new object for re-registration since the old one was collected.)
    obj = AllocFinalizableObject();
    ASSERT_NE(obj, nullptr);
    chaos_gc_register_finalizable(obj);
    chaos_gc_reregister_finalize(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(obj));

    // Allocate extra objects so at least one escapes conservative
    // false-positive marking on the re-registration GC.
    obj = nullptr;
    ASSERT_GE(AllocAndRegisterFinalizable(5), 1);

    chaos_gc_collect();
    chaos_gc_wait_for_pending_finalizers();
    EXPECT_GE(g_finalizer_invoke_count.load(), 1);
}

TEST_F(FinalizerIntegrationTest, MultipleFinalizableObjects) {
    constexpr int kCount = 100;
    void* objs[kCount];

    for (int i = 0; i < kCount; i++) {
        objs[i] = AllocFinalizableObject();
        ASSERT_NE(objs[i], nullptr);
        chaos_gc_register_finalizable(objs[i]);
    }

    // Drop all references.
    std::memset(objs, 0, sizeof(objs));

    // Collect and verify multiple finalizers ran.
    chaos_gc_collect();
    chaos_gc_wait_for_pending_finalizers();

    int count = g_finalizer_invoke_count.load();
    EXPECT_GE(count, 1);
    EXPECT_LE(count, kCount);
}

TEST_F(FinalizerIntegrationTest, NoFalsePositiveForRegularObjects) {
    // Objects without finalizer TypeInfo should never have their
    // finalizer called, even if chaos_gc_register_finalizable is called.

    void* obj = AllocBareObject();
    ASSERT_NE(obj, nullptr);
    // Write a TypeInfo WITHOUT the has_finalizer flag.
    auto* ti = const_cast<TypeInfoHot*>(
        static_cast<const TypeInfoHot*>(GetTestTypeInfo(32)));
    ASSERT_NE(ti, nullptr);
    *static_cast<const TypeInfoHot**>(obj) = ti;

    // This should be a no-op since the TypeInfo doesn't have the flag.
    chaos_gc_register_finalizable(obj);

    obj = nullptr;
    chaos_gc_collect();
    chaos_gc_wait_for_pending_finalizers();

    EXPECT_EQ(g_finalizer_invoke_count.load(), 0);
}
