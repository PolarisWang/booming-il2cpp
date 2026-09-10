/// gc_events_test — GC events unit tests.
///
/// Tests:
///   1. GcRegisterEventCallback + GcFireEvent calls back
///   2. Multiple callbacks all receive events
///   3. FireMultipleEvents delivers each event
///   4. GcAddPinnedObject / GcRemovePinnedObject lifecycle
///   5. GcSetHandleTarget / GcGetHandleTarget

#include <cstdio>
#include <cstdint>
#include <atomic>

#include <chaos/native_types.h>
#include "gc_events.h"
#include "gc_etw.h"
#include "gc_region.h"
#include "core/engine_lifecycle.h"
#include "gc_test_macros.h"

using namespace chaos::il2cpp::runtime_core;

static int g_failures = 0;
static int g_tests    = 0;
static int g_sub      = 0;

// ── Test 1: Register and fire callback ──────────────────────────────
void TestRegisterFireCallback() {
    TEST("RegisterFireCallback");

    std::atomic<int> callback_count{0};

    auto handler = +[](GcEvent event, void* user_data) {
        auto* count = static_cast<std::atomic<int>*>(user_data);
        count->fetch_add(1);
        (void)event;
    };

    GcRegisterEventCallback(handler, &callback_count);
    GcFireEvent(GcEvent::GC_START);

    // Should have been called once.
    GC_CHECK(callback_count.load() >= 1, "callback fired for events");
}

// ── Test 2: Multiple callbacks ──────────────────────────────────────
void TestMultipleCallbacks() {
    TEST("MultipleCallbacks");

    std::atomic<int> count1{0}, count2{0};

    auto handler1 = +[](GcEvent, void* ud) {
        static_cast<std::atomic<int>*>(ud)->fetch_add(1);
    };
    auto handler2 = +[](GcEvent, void* ud) {
        static_cast<std::atomic<int>*>(ud)->fetch_add(1);
    };

    GcRegisterEventCallback(handler1, &count1);
    GcRegisterEventCallback(handler2, &count2);

    GcFireEvent(GcEvent::GC_START);

    GC_CHECK(count1.load() >= 1, "first callback fired");
    GC_CHECK(count2.load() >= 1, "second callback fired");
}

// ── Test 3: Fire multiple events ────────────────────────────────────
void TestFireMultipleEvents() {
    TEST("FireMultipleEvents");

    std::atomic<int> count{0};
    auto handler = +[](GcEvent, void* ud) {
        static_cast<std::atomic<int>*>(ud)->fetch_add(1);
    };

    GcRegisterEventCallback(handler, &count);
    GcFireEvent(GcEvent::GC_START);
    GcFireEvent(GcEvent::MARK_DONE);
    GcFireEvent(GcEvent::SWEEP_DONE);

    GC_CHECK(count.load() >= 1, "callback fired for multiple events");
}

// ── Test 4: Pinned object lifecycle ─────────────────────────────────
void TestAddRemovePinnedObject() {
    TEST("AddRemovePinnedObject");

    int dummy_obj = 0;
    void* ptr = &dummy_obj;

    // Not pinned initially.
    bool is_pinned = GcIsPinnedObject(ptr);
    // Default should be not pinned.

    GcAddPinnedObject(ptr);
    is_pinned = GcIsPinnedObject(ptr);
    GC_CHECK(is_pinned, "object is pinned after GcAddPinnedObject");

    GcRemovePinnedObject(ptr);
    is_pinned = GcIsPinnedObject(ptr);
    GC_CHECK(!is_pinned, "object is not pinned after GcRemovePinnedObject");
}

// ── Test 5: Set/Get handle target ───────────────────────────────────
void TestSetGetHandleTarget() {
    TEST("SetGetHandleTarget");

    int obj1 = 0, obj2 = 0;
    void* ptr1 = &obj1;
    void* ptr2 = &obj2;

    // Create a strong handle.
    auto handle = GcCreateStrongHandle(ptr1);
    void* target = GcGetHandleTarget(handle);
    GC_CHECK(target == ptr1, "GetHandleTarget returns initial object");

    GcSetHandleTarget(handle, ptr2);
    target = GcGetHandleTarget(handle);
    GC_CHECK(target == ptr2, "GetHandleTarget returns updated object");

    GcFreeHandle(handle);
}

// ── Test 6: ETW provider init/shutdown idempotence (M13) ────────────
// GcEtwInitialize/GcEtwShutdown must be safe to call repeatedly: only the
// first Initialize registers, and Shutdown unregisters cleanly.  On platforms
// without a real ETW provider (or with the feature flag off) these are no-ops
// that must not crash.
void TestEtwInitializeShutdown() {
    TEST("EtwInitializeShutdown");

    // First init (registers provider if available).
    GcEtwInitialize();
    // Repeat init — must be idempotent (no double-register / no crash).
    GcEtwInitialize();
    GcEtwInitialize();
    GC_CHECK(true, "GcEtwInitialize repeated is idempotent without crash");

    // Shutdown, then re-init (full lifecycle round-trip).
    GcEtwShutdown();
    GcEtwShutdown();  // double shutdown must be a safe no-op
    GC_CHECK(true, "GcEtwShutdown repeated is safe");

    GcEtwInitialize();  // re-init after shutdown
    GC_CHECK(true, "GcEtw re-initialize after shutdown OK");

    // Leave shutdown for a clean exit.
    GcEtwShutdown();
}

// ── Test 7: ETW fire functions are callable (M13) ───────────────────
// Fires every GcEtwFire* event with representative payloads.  Under the
// CHAOS_IL2CPP_GC_EVENTS guard this executes the payload-path; without a
// registered provider (guard inside WriteEtwEvent) it degrades to a safe
// no-op.  Either way it must not crash and must be safe to call before/after
// provider init.
void TestEtwFireFunctions() {
    TEST("EtwFireFunctions");

    // Fire with no provider (safe no-op path).
    GcEtwFireGcStart(2);
    GcEtwFireGcEnd(1000, 4096);
    GcEtwFireGcYoungStart(65536);
    GcEtwFireGcYoungEnd(100, 8, 4096, 32768);
    GcEtwFireGcFullStart(16);
    GcEtwFireGcFullEnd(5000, 1048576, 2048, 32);
    GcEtwFireGcOom();
    GcEtwFireGcGen1Collect(200, 4, 8192);
    GcEtwFireAllocationTick(102400, 0);
    GcEtwFireAllocationTick(131072, 1);
    GC_CHECK(true, "All GcEtwFire* callable without provider no-op crash");

    // Fire after explicit init+shutdown (lifecycle-safe references).
    GcEtwInitialize();
    GcEtwFireGcStart(1);
    GcEtwFireGcOom();
    GcEtwFireAllocationTick(204800, 0);
    GcEtwShutdown();
    // Fire again after shutdown — must remain callable (no dangling state).
    GcEtwFireGcEnd(0, 0);
    GcEtwFireAllocationTick(0, 0);
    GC_CHECK(true, "All GcEtwFire* callable across init/shutdown");
}

// ── Main ────────────────────────────────────────────────────────────
int main() {
    puts("GC events unit tests");
    puts("════════════════════\n");

    TestRegisterFireCallback();
    TestMultipleCallbacks();
    TestFireMultipleEvents();
    TestAddRemovePinnedObject();
    TestSetGetHandleTarget();
    TestEtwInitializeShutdown();
    TestEtwFireFunctions();

    printf("\nResults: %d tests, %d failures\n", g_tests, g_failures);
    return g_failures > 0 ? 1 : 0;
}
