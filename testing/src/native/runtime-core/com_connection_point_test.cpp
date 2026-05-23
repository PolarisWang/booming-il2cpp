// com_connection_point_test.cpp — COM connection point bridge tests.
//
// Tests the chaos::il2cpp::runtime_core namespace functions:
//   ComFindConnectionPoints, ComAdvise, ComUnadvise, ComCreateEventSinkCcw.
//
// Full connection point functionality requires a CCW with a managed
// runtime. These tests focus on null-safety and edge cases.

#include "marshal_test_fixture.h"

#include <com_connection_point.h>
#include <com_ccw.h>

#include <cstring>

using namespace chaos::il2cpp::runtime_core;

// ════════════════════════════════════════════════════════════════════════════
// F3 — COM connection point tests
// ════════════════════════════════════════════════════════════════════════════

class ComConnectionPointTest : public MarshalTestFixture {
protected:
    // Minimal CCW for connection point testing.
    // Stack-allocated to avoid GCHandle dependency.
    struct TestCcwGuard {
        chaos::il2cpp::com_ccw::ComCcw ccw;
        TestCcwGuard() {
            std::memset(&ccw, 0, sizeof(ccw));
            ccw.vtable = &s_test_vtbl_;
            ccw.interface_count = 1;
            ccw.interfaces[0].guid = &kZeroGuid[0];
            ccw.interfaces[0].vtable = &s_test_vtbl_;
            ccw.interfaces[0].ccw_ptr = &ccw;
        }
    };

    static chaos::il2cpp::com_ccw::ComCcwVtbl s_test_vtbl_;
    static const CHAOS_IL2CPP_UINT8 kZeroGuid[16];
    static const CHAOS_IL2CPP_UINT8 kTestGuid[16];
};

chaos::il2cpp::com_ccw::ComCcwVtbl ComConnectionPointTest::s_test_vtbl_ = {
    &chaos::il2cpp::com_ccw::CcwQueryInterface,
    &chaos::il2cpp::com_ccw::CcwAddRef,
    &chaos::il2cpp::com_ccw::CcwRelease,
};

const CHAOS_IL2CPP_UINT8 ComConnectionPointTest::kZeroGuid[16] = {0};
const CHAOS_IL2CPP_UINT8 ComConnectionPointTest::kTestGuid[16] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C
};

// ── ComFindConnectionPoints ──────────────────────────────────────────────

TEST_F(ComConnectionPointTest, FindConnectionPointsNullHandle) {
    const CHAOS_IL2CPP_UINT8 iid[16] = {0};
    EXPECT_EQ(ComFindConnectionPoints(0, iid), kE_POINTER);
}

TEST_F(ComConnectionPointTest, FindConnectionPointsNullIid) {
    EXPECT_EQ(ComFindConnectionPoints(
        static_cast<CHAOS_IL2CPP_INTPTR>(0x1234), nullptr), kE_POINTER);
}

// ── ComAdvise ────────────────────────────────────────────────────────────

TEST_F(ComConnectionPointTest, AdviseNullHandle) {
    const CHAOS_IL2CPP_UINT8 iid[16] = {0};
    uint32_t cookie = 0;
    EXPECT_EQ(ComAdvise(0, iid,
        static_cast<CHAOS_IL2CPP_INTPTR>(0x5678), &cookie), kE_POINTER);
}

TEST_F(ComConnectionPointTest, AdviseNullIid) {
    uint32_t cookie = 0;
    EXPECT_EQ(ComAdvise(static_cast<CHAOS_IL2CPP_INTPTR>(0x1234), nullptr,
        static_cast<CHAOS_IL2CPP_INTPTR>(0x5678), &cookie), kE_POINTER);
}

TEST_F(ComConnectionPointTest, AdviseNullSink) {
    const CHAOS_IL2CPP_UINT8 iid[16] = {0};
    uint32_t cookie = 0;
    EXPECT_EQ(ComAdvise(static_cast<CHAOS_IL2CPP_INTPTR>(0x1234), iid, 0, &cookie),
              kE_POINTER);
}

TEST_F(ComConnectionPointTest, AdviseNullCookie) {
    const CHAOS_IL2CPP_UINT8 iid[16] = {0};
    EXPECT_EQ(ComAdvise(static_cast<CHAOS_IL2CPP_INTPTR>(0x1234), iid,
        static_cast<CHAOS_IL2CPP_INTPTR>(0x5678), nullptr), kE_POINTER);
}

// ── ComUnadvise ──────────────────────────────────────────────────────────

TEST_F(ComConnectionPointTest, UnadviseNullHandle) {
    const CHAOS_IL2CPP_UINT8 iid[16] = {0};
    EXPECT_EQ(ComUnadvise(0, iid, 1), kE_POINTER);
}

TEST_F(ComConnectionPointTest, UnadviseNullIid) {
    EXPECT_EQ(ComUnadvise(static_cast<CHAOS_IL2CPP_INTPTR>(0x1234), nullptr, 1),
              kE_POINTER);
}

// ── ComCreateEventSinkCcw ────────────────────────────────────────────────

TEST_F(ComConnectionPointTest, CreateEventSinkCcwNullDelegate) {
    const CHAOS_IL2CPP_UINT8 iid[16] = {0};
    EXPECT_EQ(ComCreateEventSinkCcw(0, iid), 0);
}

TEST_F(ComConnectionPointTest, CreateEventSinkCcwNullIid) {
    EXPECT_EQ(ComCreateEventSinkCcw(
        static_cast<CHAOS_IL2CPP_INTPTR>(0x1234), nullptr), 0);
}

TEST_F(ComConnectionPointTest, FindConnectionPointsWithCcwNoMatch) {
    // CCW only has IUnknown registered — non-matching IID should return NOCONNECTION.
    TestCcwGuard guard;
    EXPECT_EQ(ComFindConnectionPoints(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&guard.ccw), kTestGuid),
        kCONNECT_E_NOCONNECTION);
}

TEST_F(ComConnectionPointTest, FindConnectionPointsWithCcwIUnknownMatch) {
    // CCW always has IUnknown — zero-GUID should match.
    TestCcwGuard guard;
    EXPECT_EQ(ComFindConnectionPoints(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&guard.ccw), kZeroGuid),
        kS_OK);
}

TEST_F(ComConnectionPointTest, FindConnectionPointsWithRegisteredIid) {
    // Register an IID and verify FindConnectionPoints matches it.
    TestCcwGuard guard;
    chaos::il2cpp::com_ccw::ComCcwVtbl mock_vtbl{};
    EXPECT_TRUE(chaos::il2cpp::com_ccw::RegisterCcwInterface(
        &guard.ccw, kTestGuid, &mock_vtbl));

    EXPECT_EQ(ComFindConnectionPoints(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&guard.ccw), kTestGuid),
        kS_OK);
}

TEST_F(ComConnectionPointTest, UnadviseWithCcwNoConnection) {
    // Unadvise on a valid CCW with no prior Advise should return NOCONNECTION.
    TestCcwGuard guard;
    EXPECT_EQ(ComUnadvise(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&guard.ccw), kTestGuid, 1),
        kCONNECT_E_NOCONNECTION);
}

TEST_F(ComConnectionPointTest, CreateEventSinkCcwBothNull) {
    // Both delegate and iid are null.
    EXPECT_EQ(ComCreateEventSinkCcw(0, nullptr), 0);
}
