/// gc_events_test — GC events unit tests (GoogleTest).
///
/// Tests:
///   1. GcRegisterEventCallback + GcFireEvent calls back
///   2. Multiple callbacks all receive events
///   3. FireMultipleEvents delivers each event
///   4. GcAddPinnedObject / GcRemovePinnedObject lifecycle
///   5. GcSetHandleTarget / GcGetHandleTarget

#include <cstdint>
#include <atomic>

#include <chaos/native_types.h>
#include "gc_events.h"
#include "gc_region.h"
#include "core/engine_lifecycle.h"

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

// Reset all event callback slots between tests to avoid use-after-scope
// from stale handler pointers pointing to previous test's stack locals.
void ResetGcEventCallbacks() {
    for (int i = 0; i < kMaxGcEventCallbacks; i++) {
        g_gc_event_slots[i].callback.store(nullptr, std::memory_order_release);
        g_gc_event_slots[i].user_data = nullptr;
    }
}

// ── Test 1: Register and fire callback ──────────────────────────────────────

TEST(GcEvents, RegisterFireCallback) {
    ResetGcEventCallbacks();
    std::atomic<int> callback_count{0};

    auto handler = +[](GcEvent event, void* user_data) {
        auto* count = static_cast<std::atomic<int>*>(user_data);
        count->fetch_add(1);
        (void)event;
    };

    GcRegisterEventCallback(handler, &callback_count);
    GcFireEvent(GcEvent::GC_START);

    EXPECT_GE(callback_count.load(), 1) << "callback fired for events";
}

// ── Test 2: Multiple callbacks ──────────────────────────────────────────────

TEST(GcEvents, MultipleCallbacks) {
    ResetGcEventCallbacks();
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

    EXPECT_GE(count1.load(), 1) << "first callback fired";
    EXPECT_GE(count2.load(), 1) << "second callback fired";
}

// ── Test 3: Fire multiple events ────────────────────────────────────────────

TEST(GcEvents, FireMultipleEvents) {
    ResetGcEventCallbacks();
    std::atomic<int> count{0};
    auto handler = +[](GcEvent, void* ud) {
        static_cast<std::atomic<int>*>(ud)->fetch_add(1);
    };

    GcRegisterEventCallback(handler, &count);
    GcFireEvent(GcEvent::GC_START);
    GcFireEvent(GcEvent::MARK_DONE);
    GcFireEvent(GcEvent::SWEEP_DONE);

    EXPECT_GE(count.load(), 1) << "callback fired for multiple events";
}

// ── Test 4: Pinned object lifecycle ─────────────────────────────────────────

TEST(GcEvents, AddRemovePinnedObject) {
    int dummy_obj = 0;
    void* ptr = &dummy_obj;

    GcAddPinnedObject(ptr);
    bool is_pinned = GcIsPinnedObject(ptr);
    EXPECT_TRUE(is_pinned) << "object is pinned after GcAddPinnedObject";

    GcRemovePinnedObject(ptr);
    is_pinned = GcIsPinnedObject(ptr);
    EXPECT_FALSE(is_pinned) << "object is not pinned after GcRemovePinnedObject";
}

// ── Test 5: Set/Get handle target ───────────────────────────────────────────

TEST(GcEvents, SetGetHandleTarget) {
    int obj1 = 0, obj2 = 0;
    void* ptr1 = &obj1;
    void* ptr2 = &obj2;

    auto handle = GcCreateStrongHandle(ptr1);
    void* target = GcGetHandleTarget(handle);
    EXPECT_EQ(target, ptr1) << "GetHandleTarget returns initial object";

    GcSetHandleTarget(handle, ptr2);
    target = GcGetHandleTarget(handle);
    EXPECT_EQ(target, ptr2) << "GetHandleTarget returns updated object";

    GcFreeHandle(handle);
}
