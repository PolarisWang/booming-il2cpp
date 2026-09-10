/// eventpipe_hotupdate_test.cpp — EventPipe hot-update bridge test
///
/// Tests the HotupdateEvent callback registration and firing mechanism.
/// Verifies that callbacks receive correct event types and payload data.

#include <hotupdate_events.h>

#include <gtest/gtest.h>

#include <atomic>
#include <cstring>

namespace {

// ── Test callback tracking ───────────────────────────────────────────────

struct CallbackRecord {
    chaos::il2cpp::diagnostics::HotupdateEvent event;
    chaos::il2cpp::diagnostics::HotupdateEventData data;
    bool received = false;
};

void TestCallback(chaos::il2cpp::diagnostics::HotupdateEvent event,
                   const chaos::il2cpp::diagnostics::HotupdateEventData* data,
                   void* user_data) {
    auto* record = static_cast<CallbackRecord*>(user_data);
    record->event = event;
    if (data != nullptr) {
        record->data = *data;
    }
    record->received = true;
}

}  // anonymous namespace

// ── Tests ────────────────────────────────────────────────────────────────

TEST(EventpipeHotupdate, RegisterCallback) {
    CallbackRecord record;
    bool ok = chaos::il2cpp::diagnostics::HotupdateRegisterEventCallback(
        TestCallback, &record);
    EXPECT_TRUE(ok);

    // Fire an event and verify callback was invoked.
    chaos::il2cpp::diagnostics::HotupdateEventData ev_data{};
    ev_data.module_id = 42;
    ev_data.alc_id = 1;
    ev_data.assembly_name = "TestAssembly";
    ev_data.package_id = "TestPackage";
    ev_data.success = true;

    chaos::il2cpp::diagnostics::HotupdateFireEvent(
        chaos::il2cpp::diagnostics::HotupdateEvent::ASSEMBLY_LOADED, ev_data);

    EXPECT_TRUE(record.received);
    EXPECT_EQ(record.event, chaos::il2cpp::diagnostics::HotupdateEvent::ASSEMBLY_LOADED);
    EXPECT_EQ(record.data.module_id, 42u);
    EXPECT_EQ(record.data.alc_id, 1u);
    EXPECT_EQ(record.data.success, true);
}

TEST(EventpipeHotupdate, FireAssemblyUnloaded) {
    CallbackRecord record;
    bool ok = chaos::il2cpp::diagnostics::HotupdateRegisterEventCallback(
        TestCallback, &record);
    EXPECT_TRUE(ok);

    chaos::il2cpp::diagnostics::HotupdateEventData ev_data{};
    ev_data.module_id = 7;
    ev_data.assembly_name = "UnloadedAssembly";

    chaos::il2cpp::diagnostics::HotupdateFireEvent(
        chaos::il2cpp::diagnostics::HotupdateEvent::ASSEMBLY_UNLOADED, ev_data);

    EXPECT_TRUE(record.received);
    EXPECT_EQ(record.event, chaos::il2cpp::diagnostics::HotupdateEvent::ASSEMBLY_UNLOADED);
    EXPECT_EQ(record.data.module_id, 7u);
}

TEST(EventpipeHotupdate, FireMethodReplaced) {
    CallbackRecord record;
    bool ok = chaos::il2cpp::diagnostics::HotupdateRegisterEventCallback(
        TestCallback, &record);
    EXPECT_TRUE(ok);

    chaos::il2cpp::diagnostics::HotupdateEventData ev_data{};
    ev_data.method_token = 0x06000042;
    ev_data.method_name = "MyMethod";

    chaos::il2cpp::diagnostics::HotupdateFireEvent(
        chaos::il2cpp::diagnostics::HotupdateEvent::METHOD_REPLACED, ev_data);

    EXPECT_TRUE(record.received);
    EXPECT_EQ(record.event, chaos::il2cpp::diagnostics::HotupdateEvent::METHOD_REPLACED);
    EXPECT_EQ(record.data.method_token, 0x06000042u);
}

TEST(EventpipeHotupdate, FirePackageLoadFailed) {
    CallbackRecord record;
    bool ok = chaos::il2cpp::diagnostics::HotupdateRegisterEventCallback(
        TestCallback, &record);
    EXPECT_TRUE(ok);

    chaos::il2cpp::diagnostics::HotupdateEventData ev_data{};
    ev_data.package_id = "BrokenPackage";
    ev_data.success = false;
    ev_data.error_message = "Metadata parse error";

    chaos::il2cpp::diagnostics::HotupdateFireEvent(
        chaos::il2cpp::diagnostics::HotupdateEvent::PACKAGE_LOAD_FAILED, ev_data);

    EXPECT_TRUE(record.received);
    EXPECT_EQ(record.event, chaos::il2cpp::diagnostics::HotupdateEvent::PACKAGE_LOAD_FAILED);
    EXPECT_EQ(record.data.success, false);
}

TEST(EventpipeHotupdate, FireError) {
    CallbackRecord record;
    bool ok = chaos::il2cpp::diagnostics::HotupdateRegisterEventCallback(
        TestCallback, &record);
    EXPECT_TRUE(ok);

    chaos::il2cpp::diagnostics::HotupdateEventData ev_data{};
    ev_data.error_message = "General error";

    chaos::il2cpp::diagnostics::HotupdateFireEvent(
        chaos::il2cpp::diagnostics::HotupdateEvent::ERROR, ev_data);

    EXPECT_TRUE(record.received);
    EXPECT_EQ(record.event, chaos::il2cpp::diagnostics::HotupdateEvent::ERROR);
}

TEST(EventpipeHotupdate, UserDataPassedThrough) {
    int user_data_value = 0;

    auto cb = [](chaos::il2cpp::diagnostics::HotupdateEvent,
                  const chaos::il2cpp::diagnostics::HotupdateEventData*,
                  void* user_data) {
        auto* val = static_cast<int*>(user_data);
        *val = 42;
    };

    bool ok = chaos::il2cpp::diagnostics::HotupdateRegisterEventCallback(cb, &user_data_value);
    EXPECT_TRUE(ok);

    chaos::il2cpp::diagnostics::HotupdateEventData ev_data{};
    chaos::il2cpp::diagnostics::HotupdateFireEvent(
        chaos::il2cpp::diagnostics::HotupdateEvent::ASSEMBLY_LOADED, ev_data);

    EXPECT_EQ(user_data_value, 42);
}

TEST(EventpipeHotupdate, RegisterCallbackFull) {
    // Fill up the callback table by registering until full.
    CallbackRecord records[8];
    int registered = 0;
    for (int i = 0; i < 8; ++i) {
        if (chaos::il2cpp::diagnostics::HotupdateRegisterEventCallback(
                TestCallback, &records[i])) {
            registered++;
        } else {
            break;
        }
    }
    ASSERT_GT(registered, 0) << "At least one callback must register";

    // The next registration should fail (table is now full).
    CallbackRecord extra;
    EXPECT_FALSE(chaos::il2cpp::diagnostics::HotupdateRegisterEventCallback(
        TestCallback, &extra));

    // Fire an event — all registered callbacks should receive it.
    chaos::il2cpp::diagnostics::HotupdateEventData ev_data{};
    ev_data.module_id = 99;
    chaos::il2cpp::diagnostics::HotupdateFireEvent(
        chaos::il2cpp::diagnostics::HotupdateEvent::ASSEMBLY_LOADED, ev_data);

    for (int i = 0; i < registered; ++i) {
        EXPECT_TRUE(records[i].received) << "Callback " << i << " was not invoked";
        EXPECT_EQ(records[i].data.module_id, 99u);
    }
}

TEST(EventpipeHotupdate, RegisterNullCallback) {
    bool ok = chaos::il2cpp::diagnostics::HotupdateRegisterEventCallback(nullptr, nullptr);
    EXPECT_FALSE(ok);
}

TEST(EventpipeHotupdate, FireEventNoCallbacks) {
    // Should not crash when no callbacks are registered.
    chaos::il2cpp::diagnostics::HotupdateEventData ev_data{};
    chaos::il2cpp::diagnostics::HotupdateFireEvent(
        chaos::il2cpp::diagnostics::HotupdateEvent::ASSEMBLY_LOADED, ev_data);
}