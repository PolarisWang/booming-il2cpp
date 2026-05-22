/// gc_finalizer_integration_test — End-to-end finalizer tests via public ABI.
///
/// Unlike gc_finalizer_test.cpp which calls G_OldGen().RegisterFinalizer()
/// directly, these tests go through the public chaos_gc_register_finalizable()
/// API — the same path used by generated AOT code.  This verifies the
/// full chain: TypeInfo → stable_id → callback lookup → old-gen registration.

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <chaos/native_types.h>
#include <chaos/type_info.h>

#include "gc_helpers.h"
#include "gc_layout.h"
#include "gc_old_gen.h"
#include "gc_region.h"
#include "gc_api.h"
#include "gc_scheduler.h"
#include "gc_stats.h"

using namespace chaos::il2cpp::runtime_core;

// Required by gc_test_macros.h CHECK/FAIL macros.
static int g_failures = 0;
static int g_tests = 0;
static int g_sub = 0;

// Public API for waiting on pending finalizers.
extern "C" void chaos_gc_wait_for_pending_finalizers() noexcept;

#include "gc_test_macros.h"

// ── Test stable_id that won't collide with real types ────────────────
static constexpr uint64_t kTestFinalizerStableId = 0x00000000F1A1E5E3;

// Counter incremented by the test finalizer callback.
static std::atomic<int> g_finalizer_invoke_count{0};
static void* g_last_finalized_obj = nullptr;

static void TestFinalizerCallback(void* obj) {
    printf("    [finalizer invoked] obj=%p\n", obj);
    g_finalizer_invoke_count.fetch_add(1, std::memory_order_release);
    g_last_finalized_obj = obj;
}

// Test TypeInfoHot struct living in static storage.
struct alignas(8) TestFinalizerTypeInfo {
    TypeInfoHot hot;
    uint64_t _padding[4]{};
};

static TestFinalizerTypeInfo g_test_finalizer_type;

// ══════════════════════════════════════════════════════════════════════
// Helpers
// ══════════════════════════════════════════════════════════════════════

static void Setup() {
    g_finalizer_invoke_count.store(0, std::memory_order_relaxed);
    g_last_finalized_obj = nullptr;

    // Set up TypeInfoHot with kTypeInfoHasFinalizer flag.
    std::memset(&g_test_finalizer_type, 0, sizeof(g_test_finalizer_type));
    g_test_finalizer_type.hot.stable_id = kTestFinalizerStableId;
    g_test_finalizer_type.hot.flags = kTypeInfoHasFinalizer;

    // Register the TypeInfo address range.
    auto& registry = GcLayoutRegistry::Instance();
    registry.RegisterTypeInfoRange(
        reinterpret_cast<uintptr_t>(&g_test_finalizer_type),
        reinterpret_cast<uintptr_t>(&g_test_finalizer_type) + sizeof(g_test_finalizer_type));

    // Register a GC layout (64 bytes, pointer-free).
    registry.Register(kTestFinalizerStableId, 64, nullptr, 0);

    // Register the finalizer callback.
    registry.RegisterFinalizerCallback(kTestFinalizerStableId, TestFinalizerCallback);

    printf("  Setup OK: type_info=%p stable_id=0x%llx flags=0x%x\n",
           &g_test_finalizer_type.hot,
           (unsigned long long)kTestFinalizerStableId,
           (unsigned)kTypeInfoHasFinalizer);
}

static void* AllocFinalizableObject() {
    void* obj = g_old_gen.Allocate(64, true);
    if (obj == nullptr) return nullptr;
    std::memset(obj, 0, 64);
    *static_cast<const TypeInfoHot**>(obj) = &g_test_finalizer_type.hot;
    return obj;
}

// ══════════════════════════════════════════════════════════════════════
// Tests
// ══════════════════════════════════════════════════════════════════════

void TestPublicAbiRegistersFinalizer() {
    printf("\n── Test: PublicAbiRegistersFinalizer ──\n");
    Setup();

    void* obj = AllocFinalizableObject();
    CHECK(obj != nullptr, "AllocFinalizableObject");

    printf("  calling chaos_gc_register_finalizable(obj=%p) ...\n", obj);
    chaos_gc_register_finalizable(obj);

    printf("  finalization_pending_count=%d\n",
           g_gc_stats.finalization_pending_count.load());

    // Drop reference and trigger full GC.
    obj = nullptr;
    printf("  calling chaos_gc_collect() ...\n");
    chaos_gc_collect();
    printf("  calling chaos_gc_wait_for_pending_finalizers() ...\n");
    chaos_gc_wait_for_pending_finalizers();

    int count = g_finalizer_invoke_count.load();
    printf("  finalizer_invoke_count=%d\n", count);
    CHECK(count >= 1, "finalizer was called");
}

void TestReachableObjectNotFinalized() {
    printf("\n── Test: ReachableObjectNotFinalized ──\n");
    Setup();

    void* obj = AllocFinalizableObject();
    CHECK(obj != nullptr, "AllocFinalizableObject");

    chaos_gc_register_finalizable(obj);

    // Pin the object to simulate a managed static field root.
    g_old_gen.AddPinnedRoot(obj, 64);

    // Trigger full GC — the pinned root keeps the object alive,
    // so the finalizer should NOT be called.
    chaos_gc_collect();
    chaos_gc_wait_for_pending_finalizers();

    int count = g_finalizer_invoke_count.load();
    printf("  finalizer_invoke_count=%d (should be 0)\n", count);
    CHECK(count == 0, "reachable object not finalized");
}

void TestFinalizerSurvivesPromotion() {
    printf("\n── Test: FinalizerSurvivesPromotion ──\n");
    Setup();

    // Alloc and register.
    void* obj = AllocFinalizableObject();
    CHECK(obj != nullptr, "AllocFinalizableObject");
    chaos_gc_register_finalizable(obj);

    // Pin and trigger GC — verifies finalizer registration
    // survives the collect/re-register cycle.
    g_old_gen.AddPinnedRoot(obj, 64);
    chaos_gc_collect();
    chaos_gc_wait_for_pending_finalizers();
    int count = g_finalizer_invoke_count.load();
    CHECK(count == 0, "finalizer not called while object reachable");

    // Second GC cycle — same pinned root, registration still intact.
    chaos_gc_collect();
    chaos_gc_wait_for_pending_finalizers();
    count = g_finalizer_invoke_count.load();
    CHECK(count == 0, "finalizer registration survives second GC");

    printf("  finalizer survived %d GC cycles without being called\n", 2);
}

void TestSuppressAndReRegister() {
    printf("\n── Test: SuppressAndReRegister ──\n");
    Setup();

    void* obj = AllocFinalizableObject();
    CHECK(obj != nullptr, "AllocFinalizableObject");
    chaos_gc_register_finalizable(obj);

    // Suppress.
    chaos_gc_suppress_finalize(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(obj));

    obj = nullptr;
    chaos_gc_collect();
    chaos_gc_wait_for_pending_finalizers();

    int count = g_finalizer_invoke_count.load();
    printf("  after suppress, finalizer_invoke_count=%d (should be 0)\n", count);
    CHECK(count == 0, "suppressed finalizer not called");

    // Re-register with a new object.
    obj = AllocFinalizableObject();
    CHECK(obj != nullptr, "AllocFinalizableObject 2");
    chaos_gc_register_finalizable(obj);
    chaos_gc_reregister_finalize(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(obj));

    obj = nullptr;
    chaos_gc_collect();
    chaos_gc_wait_for_pending_finalizers();

    count = g_finalizer_invoke_count.load();
    printf("  after re-register, finalizer_invoke_count=%d (should be >= 1)\n", count);
    CHECK(count >= 1, "re-registered finalizer called");
}

void TestMultipleFinalizableObjects() {
    printf("\n── Test: MultipleFinalizableObjects ──\n");
    Setup();

    constexpr int kCount = 100;
    void* objs[kCount];

    for (int i = 0; i < kCount; i++) {
        objs[i] = AllocFinalizableObject();
        CHECK(objs[i] != nullptr, "AllocFinalizableObject");
        chaos_gc_register_finalizable(objs[i]);
    }

    // Drop all references.
    std::memset(objs, 0, sizeof(objs));

    chaos_gc_collect();
    chaos_gc_wait_for_pending_finalizers();

    int count = g_finalizer_invoke_count.load();
    printf("  finalizer_invoke_count=%d (should be >= 1)\n", count);
    CHECK(count >= 1, "at least one finalizer ran");
    CHECK(count <= kCount, "not more than kCount finalizers ran");
}

void TestNoFalsePositiveForRegularObjects() {
    printf("\n── Test: NoFalsePositiveForRegularObjects ──\n");
    Setup();

    // Allocate old-gen and write a TypeInfo pointer WITHOUT kTypeInfoHasFinalizer.
    void* obj = g_old_gen.Allocate(64, true);
    CHECK(obj != nullptr, "old_gen.Allocate(64)");
    std::memset(obj, 0, 64);
    auto& registry = GcLayoutRegistry::Instance();
    *static_cast<const void**>(obj) = registry.GetSentinelTypeInfo(0);

    chaos_gc_register_finalizable(obj);

    obj = nullptr;
    chaos_gc_collect();
    chaos_gc_wait_for_pending_finalizers();

    int count = g_finalizer_invoke_count.load();
    printf("  finalizer_invoke_count=%d (should be 0)\n", count);
    CHECK(count == 0, "no false positive finalizer call");
}

// ══════════════════════════════════════════════════════════════════════
// Main
// ══════════════════════════════════════════════════════════════════════

int main() {
    TestPublicAbiRegistersFinalizer();
    TestReachableObjectNotFinalized();
    TestFinalizerSurvivesPromotion();
    TestSuppressAndReRegister();
    TestMultipleFinalizableObjects();
    TestNoFalsePositiveForRegularObjects();

    printf("\n═══════════════════════════════════════════\n");
    if (g_failures == 0) {
        printf("  ALL TESTS PASSED\n");
    } else {
        printf("  %d TEST(S) FAILED\n", g_failures);
    }
    printf("═══════════════════════════════════════════\n");
    return g_failures;
}
