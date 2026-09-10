// ep_gc_bridge_test.cpp — EventPipe GC bridge unit tests
//
// Tests the EpGcBridgeInitialize/Shutdown lifecycle and verifies that
// the callback registration handles all GC event types without crashing.
// Uses GcFireEvent to simulate GC events through the registered callback.

#include <gtest/gtest.h>
#include <eventpipe/ep_gc_bridge.h>
#include <gc/gc_events.h>

namespace chaos::il2cpp::runtime_core {

// Reset the global event callback table — exposed for testing.
// GcRegisterEventCallback uses atomic CAS on global slots, so stale
// registrations from previous tests would cause double-registration
// failures.
inline void GcTestResetCallbacks() noexcept {
    for (int i = 0; i < kMaxGcEventCallbacks; ++i) {
        g_gc_event_slots[i].callback.store(nullptr, std::memory_order_release);
        g_gc_event_slots[i].user_data = nullptr;
    }
}

}  // namespace chaos::il2cpp::runtime_core

namespace chaos::il2cpp::diagnostics {
namespace {

TEST(EpGcBridgeTest, InitializeAndShutdown) {
    using namespace chaos::il2cpp::runtime_core;

    // Initialize should succeed (registers the EpGcCallback).
    EpGcBridgeInitialize();

    // Double-init should be idempotent.
    EpGcBridgeInitialize();

    // Shutdown — currently doesn't unregister callbacks, but should be safe.
    EpGcBridgeShutdown();

    // Clean slate for next test.
    GcTestResetCallbacks();
}

TEST(EpGcBridgeTest, GcEventFiringDoesNotCrash) {
    using namespace chaos::il2cpp::runtime_core;

    EpGcBridgeInitialize();

    // Fire each GC event type through GcFireEvent.  The registered
    // callback should handle all of them without crashing.
    GcFireEvent(GcEvent::GC_START);
    GcFireEvent(GcEvent::GC_END);
    GcFireEvent(GcEvent::GC_YOUNG_START);
    GcFireEvent(GcEvent::GC_YOUNG_DONE);
    GcFireEvent(GcEvent::GC_FULL_START);
    GcFireEvent(GcEvent::GC_FULL_DONE);
    GcFireEvent(GcEvent::GC_OOM);
    GcFireEvent(GcEvent::GC_GEN1_COLLECT);

    // Internal phase markers should be silently ignored.
    GcFireEvent(GcEvent::MARK_DONE);
    GcFireEvent(GcEvent::SWEEP_DONE);
    GcFireEvent(GcEvent::COMPACT_DONE);

    EpGcBridgeShutdown();
    GcTestResetCallbacks();
}

TEST(EpGcBridgeTest, CustomCallbackAlongsideBridge) {
    using namespace chaos::il2cpp::runtime_core;

    // The bridge registers exactly one callback.
    EpGcBridgeInitialize();

    // Register a second callback alongside the bridge.
    int called = 0;
    bool reg_ok = GcRegisterEventCallback(
        [](GcEvent, void* ctx) { ++(*static_cast<int*>(ctx)); }, &called);
    EXPECT_TRUE(reg_ok);

    // Fire an event: both callbacks should fire.
    GcFireEvent(GcEvent::GC_OOM);
    EXPECT_EQ(called, 1);

    EpGcBridgeShutdown();
    GcTestResetCallbacks();
}

}  // namespace
}  // namespace chaos::il2cpp::diagnostics
