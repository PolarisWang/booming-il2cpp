/// com_connection_point_test.cpp — COM connection point unit tests
///
/// Exercises: ComAdvise, ComUnadvise, ComFindConnectionPoints.

#include <runtime_abi.h>

#include "com_connection_point.h"
#include "com_ccw.h"

#include <gtest/gtest.h>

#include <cstring>
#include <cstdlib>

namespace {

// ── Mock runtime helpers ─────────────────────────────────────────────────

struct MockRuntimeState {
    int dummy;
};

// A minimal managed object.
int g_managed_obj = 0;

// Helper: create a minimal CCW for connection point testing.
chaos::il2cpp::com_ccw::ComCcw* CreateTestCcw() {
    auto handle = chaos::il2cpp::com_ccw::CreateCcw(&g_managed_obj, nullptr);
    if (handle == 0) return nullptr;
    return reinterpret_cast<chaos::il2cpp::com_ccw::ComCcw*>(
        static_cast<void*>(handle));
}

// A test GUID (16 bytes).
const CHAOS_IL2CPP_UINT8 kTestIid[16] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C
};

const CHAOS_IL2CPP_UINT8 kOtherIid[16] = {
    0xCA, 0xFE, 0xBA, 0xBE, 0x10, 0x20, 0x30, 0x40,
    0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0
};

}  // anonymous namespace

// ── Tests ────────────────────────────────────────────────────────────────

TEST(ComConnectionPoint, ComFindConnectionPointsNotFound) {
    auto* ccw = CreateTestCcw();
    ASSERT_NE(ccw, nullptr);

    // No interfaces registered besides IUnknown, so this IID should not match.
    auto result = chaos::il2cpp::runtime_core::ComFindConnectionPoints(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ccw), kTestIid);
    EXPECT_EQ(result, chaos::il2cpp::runtime_core::kCONNECT_E_NOCONNECTION);

    // Cleanup.
    chaos::il2cpp::com_ccw::CcwRelease(ccw);
}

TEST(ComConnectionPoint, ComFindConnectionPointsNullArgs) {
    auto result = chaos::il2cpp::runtime_core::ComFindConnectionPoints(0, kTestIid);
    EXPECT_EQ(result, chaos::il2cpp::runtime_core::kE_POINTER);

    auto* ccw = CreateTestCcw();
    ASSERT_NE(ccw, nullptr);
    result = chaos::il2cpp::runtime_core::ComFindConnectionPoints(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ccw), nullptr);
    EXPECT_EQ(result, chaos::il2cpp::runtime_core::kE_POINTER);

    chaos::il2cpp::com_ccw::CcwRelease(ccw);
}

TEST(ComConnectionPoint, ComAdvise) {
    auto* ccw = CreateTestCcw();
    ASSERT_NE(ccw, nullptr);

    // Register the test IID as a CCW interface so FindConnectionPoints works.
    chaos::il2cpp::com_ccw::ComCcwVtbl mock_vtbl{};
    EXPECT_TRUE(chaos::il2cpp::com_ccw::RegisterCcwInterface(
        ccw, kTestIid, &mock_vtbl));

    // Create a sink CCW for the Advise call.
    auto sink_handle = chaos::il2cpp::com_ccw::CreateCcw(&g_managed_obj, nullptr);
    ASSERT_NE(sink_handle, 0);

    uint32_t cookie = 0;
    auto result = chaos::il2cpp::runtime_core::ComAdvise(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ccw),
        kTestIid,
        sink_handle,
        &cookie);
    EXPECT_EQ(result, chaos::il2cpp::runtime_core::kS_OK);
    EXPECT_NE(cookie, 0u);

    // FindConnectionPoints should still work.
    result = chaos::il2cpp::runtime_core::ComFindConnectionPoints(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ccw), kTestIid);
    EXPECT_EQ(result, chaos::il2cpp::runtime_core::kS_OK);

    // Cleanup: Unadvise.
    result = chaos::il2cpp::runtime_core::ComUnadvise(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ccw), kTestIid, cookie);
    EXPECT_EQ(result, chaos::il2cpp::runtime_core::kS_OK);

    chaos::il2cpp::com_ccw::CcwRelease(ccw);
    // The sink CCW got an extra ref from ComAdvise, so we need to release
    // our reference and let the Unadvise release the connection's ref.
    auto* sink = reinterpret_cast<chaos::il2cpp::com_ccw::ComCcw*>(
        static_cast<void*>(sink_handle));
    chaos::il2cpp::com_ccw::CcwRelease(sink);
}

TEST(ComConnectionPoint, ComAdviseNullArgs) {
    auto* ccw = CreateTestCcw();
    ASSERT_NE(ccw, nullptr);

    uint32_t cookie = 0;

    // null ccw_handle
    auto result = chaos::il2cpp::runtime_core::ComAdvise(0, kTestIid, 1234, &cookie);
    EXPECT_EQ(result, chaos::il2cpp::runtime_core::kE_POINTER);

    // null iid
    result = chaos::il2cpp::runtime_core::ComAdvise(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ccw), nullptr, 1234, &cookie);
    EXPECT_EQ(result, chaos::il2cpp::runtime_core::kE_POINTER);

    // null cookie
    result = chaos::il2cpp::runtime_core::ComAdvise(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ccw), kTestIid, 1234, nullptr);
    EXPECT_EQ(result, chaos::il2cpp::runtime_core::kE_POINTER);

    chaos::il2cpp::com_ccw::CcwRelease(ccw);
}

TEST(ComConnectionPoint, ComUnadviseNotFound) {
    auto* ccw = CreateTestCcw();
    ASSERT_NE(ccw, nullptr);

    // Unadvise with no prior Advise should return CONNECT_E_NOCONNECTION.
    auto result = chaos::il2cpp::runtime_core::ComUnadvise(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ccw), kTestIid, 1);
    EXPECT_EQ(result, chaos::il2cpp::runtime_core::kCONNECT_E_NOCONNECTION);

    chaos::il2cpp::com_ccw::CcwRelease(ccw);
}

TEST(ComConnectionPoint, ComAdviseMultipleSinks) {
    auto* ccw = CreateTestCcw();
    ASSERT_NE(ccw, nullptr);

    chaos::il2cpp::com_ccw::ComCcwVtbl mock_vtbl{};
    EXPECT_TRUE(chaos::il2cpp::com_ccw::RegisterCcwInterface(ccw, kTestIid, &mock_vtbl));

    // Create two sink CCWs.
    auto sink1_handle = chaos::il2cpp::com_ccw::CreateCcw(&g_managed_obj, nullptr);
    auto sink2_handle = chaos::il2cpp::com_ccw::CreateCcw(&g_managed_obj, nullptr);
    ASSERT_NE(sink1_handle, 0);
    ASSERT_NE(sink2_handle, 0);

    uint32_t cookie1 = 0, cookie2 = 0;
    EXPECT_EQ(chaos::il2cpp::runtime_core::ComAdvise(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ccw), kTestIid, sink1_handle, &cookie1),
        chaos::il2cpp::runtime_core::kS_OK);
    EXPECT_EQ(chaos::il2cpp::runtime_core::ComAdvise(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ccw), kTestIid, sink2_handle, &cookie2),
        chaos::il2cpp::runtime_core::kS_OK);
    EXPECT_NE(cookie1, cookie2);

    // Unadvise both.
    EXPECT_EQ(chaos::il2cpp::runtime_core::ComUnadvise(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ccw), kTestIid, cookie1),
        chaos::il2cpp::runtime_core::kS_OK);
    EXPECT_EQ(chaos::il2cpp::runtime_core::ComUnadvise(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ccw), kTestIid, cookie2),
        chaos::il2cpp::runtime_core::kS_OK);

    chaos::il2cpp::com_ccw::CcwRelease(ccw);
    auto* s1 = reinterpret_cast<chaos::il2cpp::com_ccw::ComCcw*>(
        static_cast<void*>(sink1_handle));
    auto* s2 = reinterpret_cast<chaos::il2cpp::com_ccw::ComCcw*>(
        static_cast<void*>(sink2_handle));
    chaos::il2cpp::com_ccw::CcwRelease(s1);
    chaos::il2cpp::com_ccw::CcwRelease(s2);
}

TEST(ComConnectionPoint, ComAdviseDifferentIids) {
    auto* ccw = CreateTestCcw();
    ASSERT_NE(ccw, nullptr);

    chaos::il2cpp::com_ccw::ComCcwVtbl mock_vtbl{};
    EXPECT_TRUE(chaos::il2cpp::com_ccw::RegisterCcwInterface(ccw, kTestIid, &mock_vtbl));
    EXPECT_TRUE(chaos::il2cpp::com_ccw::RegisterCcwInterface(ccw, kOtherIid, &mock_vtbl));

    auto sink_handle = chaos::il2cpp::com_ccw::CreateCcw(&g_managed_obj, nullptr);
    ASSERT_NE(sink_handle, 0);

    uint32_t cookie1 = 0, cookie2 = 0;
    EXPECT_EQ(chaos::il2cpp::runtime_core::ComAdvise(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ccw), kTestIid, sink_handle, &cookie1),
        chaos::il2cpp::runtime_core::kS_OK);
    EXPECT_EQ(chaos::il2cpp::runtime_core::ComAdvise(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ccw), kOtherIid, sink_handle, &cookie2),
        chaos::il2cpp::runtime_core::kS_OK);

    EXPECT_EQ(chaos::il2cpp::runtime_core::ComUnadvise(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ccw), kTestIid, cookie1),
        chaos::il2cpp::runtime_core::kS_OK);
    EXPECT_EQ(chaos::il2cpp::runtime_core::ComUnadvise(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ccw), kOtherIid, cookie2),
        chaos::il2cpp::runtime_core::kS_OK);

    chaos::il2cpp::com_ccw::CcwRelease(ccw);
    auto* sink = reinterpret_cast<chaos::il2cpp::com_ccw::ComCcw*>(
        static_cast<void*>(sink_handle));
    chaos::il2cpp::com_ccw::CcwRelease(sink);
}
