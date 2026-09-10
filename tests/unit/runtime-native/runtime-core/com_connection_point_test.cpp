// com_connection_point_test.cpp — COM connection point bridge tests.
//
// Tests the chaos::il2cpp::runtime_core namespace functions:
//   ComFindConnectionPoints, ComAdvise, ComUnadvise, ComCreateEventSinkCcw.
//
// IMPORTANT: These tests compile against the CURRENT header but LINK against
// the prebuilt chaos_runtime_core.lib. Uses ComCcwOld (mirrors the committed
// struct layout) for test setup to avoid ABI mismatch.
//
// Full connection point functionality requires a CCW with a managed
// runtime. These tests focus on null-safety and edge cases.

#include "marshal_test_fixture.h"

#include <com_connection_point.h>
#include <com_ccw.h>

#include <cstring>

// Local copy of post-industrialization ComCcw layout (mirrors the prebuilt
// chaos_runtime_core.lib). Updated for SmallVector pattern:
// interfaces is now a POINTER, with inline_interfaces[4] after it.
struct ComCcwOld {
    chaos::il2cpp::com_ccw::ComCcwVtbl* vtable;           //  0:  8
    std::atomic<CHAOS_IL2CPP_UINT32> refcount;             //  8:  4
    CHAOS_IL2CPP_UINT64 gc_handle;                         // 16:  8
    void* runtime_state;                                    // 24:  8
    CHAOS_IL2CPP_SIZE interface_count;                      // 32:  8
    CHAOS_IL2CPP_SIZE interface_capacity;                   // 40:  8
    void* outer_unknown;                                    // 48:  8
    bool  is_aggregated;                                    // 56:  1
    chaos::il2cpp::com_ccw::ComCcwInterfaceEntry* interfaces;           // 64:  8 (pointer)
    chaos::il2cpp::com_ccw::ComCcwInterfaceEntry inline_interfaces[4];  // 72: 96
    void* typelib_data;                                     // 168: 8
    void* cp_container;                                     // 176: 8
};
static_assert(sizeof(ComCcwOld) == 184, "ComCcwOld layout mismatch");

using namespace chaos::il2cpp::runtime_core;

// ════════════════════════════════════════════════════════════════════════════
// F3 — COM connection point tests
// ════════════════════════════════════════════════════════════════════════════

class ComConnectionPointTest : public MarshalTestFixture {
protected:
    // Minimal CCW for connection point testing, using committed layout.
    struct TestCcwGuard {
        ComCcwOld ccw;

        void* self() noexcept { return &ccw; }

        TestCcwGuard() {
            std::memset(&ccw, 0, sizeof(ccw));
            ccw.vtable = &s_test_vtbl_;
            ccw.refcount = 1;  // prevent free on Release
            ccw.interface_count = 1;
            ccw.interface_capacity = 4;  // kInlineCcwInterfaces
            ccw.interfaces = ccw.inline_interfaces;  // Point to inline storage
            ccw.inline_interfaces[0].guid = kZeroGuid;
            ccw.inline_interfaces[0].vtable = &s_test_vtbl_;
            ccw.inline_interfaces[0].ccw_ptr = &ccw;
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
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(guard.self()), kTestGuid),
        kCONNECT_E_NOCONNECTION);
}

TEST_F(ComConnectionPointTest, FindConnectionPointsWithCcwIUnknownMatch) {
    // CCW always has IUnknown — zero-GUID should match.
    TestCcwGuard guard;
    EXPECT_EQ(ComFindConnectionPoints(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(guard.self()), kZeroGuid),
        kS_OK);
}

TEST_F(ComConnectionPointTest, FindConnectionPointsWithRegisteredIid) {
    // Register an IID and verify FindConnectionPoints matches it.
    TestCcwGuard guard;
    chaos::il2cpp::com_ccw::ComCcwVtbl mock_vtbl{};
    EXPECT_TRUE(chaos::il2cpp::com_ccw::RegisterCcwInterface(
        guard.self(), kTestGuid, &mock_vtbl));

    EXPECT_EQ(ComFindConnectionPoints(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(guard.self()), kTestGuid),
        kS_OK);
}

TEST_F(ComConnectionPointTest, UnadviseWithCcwNoConnection) {
    // Unadvise on a valid CCW with no prior Advise should return NOCONNECTION.
    TestCcwGuard guard;
    EXPECT_EQ(ComUnadvise(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(guard.self()), kTestGuid, 1),
        kCONNECT_E_NOCONNECTION);
}

TEST_F(ComConnectionPointTest, CreateEventSinkCcwBothNull) {
    // Both delegate and iid are null.
    EXPECT_EQ(ComCreateEventSinkCcw(0, nullptr), 0);
}

// ── Advise/Unadvise lifecycle (DISABLED: needs MemoryDomain) ────────────
// NOTE: These tests require DomainCurrentAllocateTagged (MemoryDomain)
// which is NOT available in the isolated test environment. When a real
// MemoryDomain is set up, remove the DISABLED_ prefix.
// See memory_domain.h for DomainCurrentAllocateTagged requirements.
// Also note: uses void* for cp_container to match ComCcwOld layout.

TEST_F(ComConnectionPointTest, DISABLED_AdviseFullCycle) {
    // Full Advise → Unadvise cycle on a real CCW.
    TestCcwGuard guard;
    CHAOS_IL2CPP_INTPTR ccw_handle = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(guard.self());

    uint32_t cookie = 0;
    EXPECT_EQ(ComAdvise(ccw_handle, kTestGuid,
              static_cast<CHAOS_IL2CPP_INTPTR>(0x9000), &cookie), kS_OK);
    EXPECT_NE(cookie, 0u);
    EXPECT_NE(guard.ccw.cp_container, nullptr);
    SUCCEED();

    EXPECT_EQ(ComUnadvise(ccw_handle, kTestGuid, cookie), kS_OK);
}

TEST_F(ComConnectionPointTest, DISABLED_MultipleAdviseSameIid) {
    TestCcwGuard guard;
    CHAOS_IL2CPP_INTPTR ccw_handle = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(guard.self());

    uint32_t cookie1 = 0, cookie2 = 0;
    EXPECT_EQ(ComAdvise(ccw_handle, kTestGuid,
              static_cast<CHAOS_IL2CPP_INTPTR>(0x9000), &cookie1), kS_OK);
    EXPECT_EQ(ComAdvise(ccw_handle, kTestGuid,
              static_cast<CHAOS_IL2CPP_INTPTR>(0x9001), &cookie2), kS_OK);

    EXPECT_NE(cookie1, cookie2);
    EXPECT_EQ(ComUnadvise(ccw_handle, kTestGuid, cookie1), kS_OK);
    EXPECT_EQ(ComUnadvise(ccw_handle, kTestGuid, cookie2), kS_OK);
}

TEST_F(ComConnectionPointTest, DISABLED_AdviseUnadviseReadvise) {
    TestCcwGuard guard;
    CHAOS_IL2CPP_INTPTR ccw_handle = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(guard.self());

    uint32_t cookie1 = 0;
    EXPECT_EQ(ComAdvise(ccw_handle, kTestGuid,
              static_cast<CHAOS_IL2CPP_INTPTR>(0x9000), &cookie1), kS_OK);
    EXPECT_EQ(ComUnadvise(ccw_handle, kTestGuid, cookie1), kS_OK);

    uint32_t cookie2 = 0;
    EXPECT_EQ(ComAdvise(ccw_handle, kTestGuid,
              static_cast<CHAOS_IL2CPP_INTPTR>(0x9000), &cookie2), kS_OK);
    EXPECT_NE(cookie2, cookie1);
}

TEST_F(ComConnectionPointTest, DISABLED_UnadviseNonexistentCookie) {
    TestCcwGuard guard;
    CHAOS_IL2CPP_INTPTR ccw_handle = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(guard.self());

    uint32_t cookie = 0;
    EXPECT_EQ(ComAdvise(ccw_handle, kTestGuid,
              static_cast<CHAOS_IL2CPP_INTPTR>(0x9000), &cookie), kS_OK);

    EXPECT_EQ(ComUnadvise(ccw_handle, kTestGuid, cookie + 1),
              kCONNECT_E_NOCONNECTION);

    EXPECT_EQ(ComUnadvise(ccw_handle, kTestGuid, cookie), kS_OK);
}

TEST_F(ComConnectionPointTest, DISABLED_AdviseLazilyAllocatesCpContainer) {
    TestCcwGuard guard;
    guard.ccw.cp_container = nullptr;
    CHAOS_IL2CPP_INTPTR ccw_handle = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(guard.self());

    uint32_t cookie = 0;
    EXPECT_EQ(ComAdvise(ccw_handle, kTestGuid,
              static_cast<CHAOS_IL2CPP_INTPTR>(0x9000), &cookie), kS_OK);
    EXPECT_NE(guard.ccw.cp_container, nullptr);
    SUCCEED();

    EXPECT_EQ(ComUnadvise(ccw_handle, kTestGuid, cookie), kS_OK);
}

TEST_F(ComConnectionPointTest, DISABLED_MultipleIidsSeparateLists) {
    TestCcwGuard guard;
    CHAOS_IL2CPP_INTPTR ccw_handle = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(guard.self());

    const CHAOS_IL2CPP_UINT8 iid_a[16] = {1, 0, 0, 0};
    const CHAOS_IL2CPP_UINT8 iid_b[16] = {2, 0, 0, 0};

    uint32_t cookie_a = 0, cookie_b = 0;
    EXPECT_EQ(ComAdvise(ccw_handle, iid_a,
              static_cast<CHAOS_IL2CPP_INTPTR>(0xA000), &cookie_a), kS_OK);
    EXPECT_EQ(ComAdvise(ccw_handle, iid_b,
              static_cast<CHAOS_IL2CPP_INTPTR>(0xB000), &cookie_b), kS_OK);
    EXPECT_NE(guard.ccw.cp_container, nullptr);
    SUCCEED();

    EXPECT_EQ(ComUnadvise(ccw_handle, iid_a, cookie_a), kS_OK);
    EXPECT_EQ(ComUnadvise(ccw_handle, iid_b, cookie_b), kS_OK);
}

TEST_F(ComConnectionPointTest, DISABLED_FindConnectionPointsAfterAllUnadvised) {
    TestCcwGuard guard;
    CHAOS_IL2CPP_INTPTR ccw_handle = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(guard.self());

    uint32_t cookie = 0;
    EXPECT_EQ(ComAdvise(ccw_handle, kTestGuid,
              static_cast<CHAOS_IL2CPP_INTPTR>(0x9000), &cookie), kS_OK);
    EXPECT_EQ(ComUnadvise(ccw_handle, kTestGuid, cookie), kS_OK);

    EXPECT_EQ(ComFindConnectionPoints(ccw_handle, kTestGuid), kS_OK);
}
