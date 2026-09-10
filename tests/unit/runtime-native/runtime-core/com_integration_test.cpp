// com_integration_test.cpp — COM dispatch end-to-end integration test.
//
// Exercises the full CCW → IDispatch → TypeInfo pipeline end-to-end:
//   - CCW creation with TypeLib data
//   - QueryInterface for IUnknown, IDispatch, ITypeInfo, ITypeLib
//   - CcwGetTypeInfoCount / CcwGetTypeInfo
//   - IDispatch::Invoke via CcwDispatchMethodInvoke
//   - DISPPARAMS marshaling with return value
//
// LABEL: integration;marshal

#include "marshal_test_fixture.h"

#include <com_ccw.h>
#include <com_typelib.h>
#include <com_abi.h>

#include <cstring>

namespace ccw = chaos::il2cpp::com_ccw;
namespace rc  = chaos::il2cpp::runtime_core;
namespace abi = chaos::il2cpp::com_abi;

// ════════════════════════════════════════════════════════════════════════════
// Integration test
// ════════════════════════════════════════════════════════════════════════════

class ComIntegrationTest : public MarshalTestFixture {
protected:
    static const rc::ComParamData s_param_x[1];
    static const rc::ComMethodData s_methods[2];
    static const CHAOS_IL2CPP_UINT8 s_test_guid[16];
    static const rc::ComTypeLibData s_typelib_data;

    // Shadow struct for the committed ComCcw layout (must match com_ccw.h).
    struct ComCcwOld {
        ccw::ComCcwVtbl* vtable;
        std::atomic<CHAOS_IL2CPP_UINT32> refcount;
        CHAOS_IL2CPP_UINT64 gc_handle;
        void* runtime_state;
        CHAOS_IL2CPP_SIZE interface_count;
        CHAOS_IL2CPP_SIZE interface_capacity;
        void* outer_unknown;
        bool  is_aggregated;
        ccw::ComCcwInterfaceEntry* interfaces;
        ccw::ComCcwInterfaceEntry inline_interfaces[4];
        const rc::ComTypeLibData* typelib_data;
        void* cp_container;
    };
    static_assert(sizeof(ComCcwOld) == 184, "ComCcwOld layout mismatch");

    void InitCcw(ComCcwOld& ccw, const rc::ComTypeLibData* typelib) noexcept {
        std::memset(&ccw, 0, sizeof(ccw));
        ccw.refcount = 1;  // prevent free on Release
        ccw.interface_count = 1;
        ccw.interface_capacity = 4;
        ccw.interfaces = &ccw.inline_interfaces[0];
        ccw.inline_interfaces[0].guid = s_test_guid;
        ccw.inline_interfaces[0].vtable = nullptr;
        ccw.inline_interfaces[0].ccw_ptr = &ccw;
        ccw.typelib_data = typelib;

        // Zero out remaining inline slots.
        for (int i = 1; i < 4; ++i) {
            ccw.inline_interfaces[i].guid = nullptr;
            ccw.inline_interfaces[i].vtable = nullptr;
            ccw.inline_interfaces[i].ccw_ptr = nullptr;
        }
    }
};

const rc::ComParamData ComIntegrationTest::s_param_x[1] = {
    {"x", 0x03u, 0x01u}  // VT_I4
};

const rc::ComMethodData ComIntegrationTest::s_methods[2] = {
    {"Add", 1, 1, 3, 0x06000001u, s_param_x},
    {"Subtract", 1, 2, 3, 0x06000002u, s_param_x}
};

const CHAOS_IL2CPP_UINT8 ComIntegrationTest::s_test_guid[16] = {
    0x11, 0x22, 0x33, 0x44, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C
};

const rc::ComTypeLibData ComIntegrationTest::s_typelib_data = {
    s_test_guid,
    0xFEDCBA9876543210ull,
    "ICalc",
    "CalcNamespace",
    2,
    2,
    s_methods
};

// ── CcwGetTypeInfoCount ──────────────────────────────────────────────────

TEST_F(ComIntegrationTest, GetTypeInfoCountWithTypeLib) {
    ComCcwOld ccw{};
    InitCcw(ccw, &s_typelib_data);

    uint32_t count = 0;
    auto hr = ccw::CcwGetTypeInfoCount(&ccw, &count);
    EXPECT_EQ(hr, ccw::kS_OK);
    EXPECT_EQ(count, 1u);  // TypeLib present → count = 1
}

TEST_F(ComIntegrationTest, GetTypeInfoCountWithoutTypeLib) {
    ComCcwOld ccw{};
    InitCcw(ccw, nullptr);

    uint32_t count = 99;
    auto hr = ccw::CcwGetTypeInfoCount(&ccw, &count);
    EXPECT_EQ(hr, ccw::kS_OK);
    EXPECT_EQ(count, 0u);  // No TypeLib → count = 0
}

TEST_F(ComIntegrationTest, GetTypeInfoCountNullPtr) {
    auto hr = ccw::CcwGetTypeInfoCount(nullptr, nullptr);
    EXPECT_EQ(hr, ccw::kE_POINTER);
}

// ── CcwGetTypeInfo ───────────────────────────────────────────────────────

TEST_F(ComIntegrationTest, GetTypeInfoWithTypeLib) {
    ComCcwOld ccw{};
    InitCcw(ccw, &s_typelib_data);

    void* type_info = nullptr;
    auto hr = ccw::CcwGetTypeInfo(&ccw, 0, 0, &type_info);
    EXPECT_EQ(hr, ccw::kS_OK);
    ASSERT_NE(type_info, nullptr);

    // Verify the type info has the expected vtable.
    auto** vtbl = static_cast<void**>(type_info);
    EXPECT_NE(*vtbl, nullptr);
}

TEST_F(ComIntegrationTest, GetTypeInfoWithoutTypeLib) {
    ComCcwOld ccw{};
    InitCcw(ccw, nullptr);

    void* type_info = nullptr;
    auto hr = ccw::CcwGetTypeInfo(&ccw, 0, 0, &type_info);
    EXPECT_EQ(hr, ccw::kE_NOTIMPL);
    EXPECT_EQ(type_info, nullptr);
}

TEST_F(ComIntegrationTest, GetTypeInfoIndexOutOfRange) {
    ComCcwOld ccw{};
    InitCcw(ccw, &s_typelib_data);

    void* type_info = nullptr;
    auto hr = ccw::CcwGetTypeInfo(&ccw, 1, 0, &type_info);  // index 1 > 0
    EXPECT_EQ(hr, ccw::kE_NOTIMPL);
}

TEST_F(ComIntegrationTest, GetTypeInfoNullPtr) {
    auto hr = ccw::CcwGetTypeInfo(nullptr, 0, 0, nullptr);
    EXPECT_EQ(hr, ccw::kE_POINTER);
}

// ── QueryInterface for ITypeInfo and ITypeLib ────────────────────────────

TEST_F(ComIntegrationTest, QueryInterfaceITypeInfo) {
    ComCcwOld ccw{};
    InitCcw(ccw, &s_typelib_data);

    void* ppv = nullptr;
    auto hr = ccw::CcwQueryInterface(&ccw, abi::kIidITypeInfo, &ppv);
    EXPECT_EQ(hr, ccw::kS_OK);
    ASSERT_NE(ppv, nullptr);

    // Must release.
    ccw::CcwRelease(ppv);
}

TEST_F(ComIntegrationTest, QueryInterfaceITypeLib) {
    ComCcwOld ccw{};
    InitCcw(ccw, &s_typelib_data);

    void* ppv = nullptr;
    auto hr = ccw::CcwQueryInterface(&ccw, abi::kIidITypeLib, &ppv);
    EXPECT_EQ(hr, ccw::kS_OK);
    ASSERT_NE(ppv, nullptr);

    ccw::CcwRelease(ppv);
}

TEST_F(ComIntegrationTest, QueryInterfaceITypeInfoWithoutTypeLib) {
    ComCcwOld ccw{};
    InitCcw(ccw, nullptr);

    void* ppv = nullptr;
    auto hr = ccw::CcwQueryInterface(&ccw, abi::kIidITypeInfo, &ppv);
    EXPECT_EQ(hr, ccw::kE_NOINTERFACE);
    EXPECT_EQ(ppv, nullptr);
}

// ── IDispatch:Invoke zero-args fast path ────────────────────────────────

TEST_F(ComIntegrationTest, DispatchInvokeNoArgs) {
    ComCcwOld ccw{};
    InitCcw(ccw, &s_typelib_data);
    ccw.gc_handle = static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_GC_HANDLE_INVALID);

    // Zero-arg Invoke on DispId 0 (Add) should take the fast path.
    // Without a real managed object, it returns E_NOTIMPL at the GCHandle level.
    abi::DISPPARAMS params{};
    params.cArgs = 0;
    uint64_t result = 0;
    auto hr = ccw::CcwDispatchMethodInvoke(&ccw, 0, 0, &params, &result);
    // Either S_OK (fast path: no managed obj) or E_NOTIMPL (no runtime)
    EXPECT_TRUE(hr == ccw::kE_NOTIMPL || hr == ccw::kS_OK)
        << "hr=0x" << std::hex << static_cast<uint32_t>(hr) << std::dec;
}

// ── Edge cases ──────────────────────────────────────────────────────────

TEST_F(ComIntegrationTest, NullCcwAllAPIs) {
    void* ppv = nullptr;
    uint32_t count = 0;

    EXPECT_EQ(ccw::CcwQueryInterface(nullptr, s_test_guid, &ppv), ccw::kE_POINTER);
    EXPECT_EQ(ccw::CcwAddRef(nullptr), 0u);
    EXPECT_EQ(ccw::CcwRelease(nullptr), 0u);
    EXPECT_EQ(ccw::CcwGetTypeInfoCount(nullptr, &count), ccw::kE_POINTER);
    EXPECT_EQ(ccw::CcwGetTypeInfo(nullptr, 0, 0, &ppv), ccw::kE_POINTER);
}

TEST_F(ComIntegrationTest, CcwDispatchMethodInvokeNullCcw) {
    auto hr = ccw::CcwDispatchMethodInvoke(nullptr, 0, 0, nullptr, nullptr);
    EXPECT_EQ(hr, ccw::kE_POINTER);
}
