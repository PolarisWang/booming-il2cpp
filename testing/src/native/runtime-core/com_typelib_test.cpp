// com_typelib_test.cpp — TypeLib (ITypeInfo / ITypeLib) unit tests.
//
// Tests the runtime ITypeInfo/ITypeLib implementations by constructing
// static ComTypeLibData descriptors and verifying that the wrapper methods
// (GetTypeAttr, GetFuncDesc, GetIDsOfNames, etc.) return correct values.
//
// Does NOT require a managed runtime — tests are purely structural.

#include "marshal_test_fixture.h"

#include <com_typelib.h>
#include <com_abi.h>

#include <cstring>

using namespace chaos::il2cpp::runtime_core;

// ════════════════════════════════════════════════════════════════════════════
// T1 — TypeLib tests
// ════════════════════════════════════════════════════════════════════════════

class ComTypeLibTest : public MarshalTestFixture {
protected:
    // Static method data for a simple "Foo(int)" method.
    static const ComParamData s_foo_params[1];
    static const ComMethodData s_methods[2];

    // A minimal ComTypeLibData with IUnknown GUID and two methods.
    static const CHAOS_IL2CPP_UINT8 s_test_guid[16];
    static const ComTypeLibData s_test_lib_data;

    // CCW with typelib_data set (null gc_handle to avoid GCHandle issues).
    // Must match the ComCcw layout (x64).
    // ComCcwInterfaceEntry = 24 bytes, inline_interfaces[4] = 96 bytes.
    struct TestCcwWithTypeLib {
        void* vtable;
        std::atomic<CHAOS_IL2CPP_UINT32> refcount;
        CHAOS_IL2CPP_UINT64 gc_handle;
        void* runtime_state;
        CHAOS_IL2CPP_SIZE interface_count;
        CHAOS_IL2CPP_SIZE interface_capacity;
        void* outer_unknown;
        bool  is_aggregated;
        void* interfaces_ptr;
        void* inline_interfaces[12];  // ComCcwInterfaceEntry[4] = 96 bytes
        const ComTypeLibData* typelib_data;
        void* cp_container;
    };

    // Helper: read the ITypeInfoVtbl from a COM-like ITypeInfo pointer.
    static ITypeInfoVtbl* GetTypeInfoVtbl(void* type_info) {
        if (type_info == nullptr) return nullptr;
        return *static_cast<ITypeInfoVtbl**>(type_info);
    }

    // Helper: read the ITypeLibVtbl from a COM-like ITypeLib pointer.
    static ITypeLibVtbl* GetTypeLibVtbl(void* type_lib) {
        if (type_lib == nullptr) return nullptr;
        return *static_cast<ITypeLibVtbl**>(type_lib);
    }
};

const ComParamData ComTypeLibTest::s_foo_params[1] = {
    {"x", 0x03u, 0x01u}  // VT_I4 (0x03), PARAMFLAG_FIN (0x01)
};

const ComMethodData ComTypeLibTest::s_methods[2] = {
    {"Foo", 1, 1, 3, 0x06000001u, s_foo_params},
    {"Bar", 2, 0, 4, 0x06000002u, nullptr}
};

const CHAOS_IL2CPP_UINT8 ComTypeLibTest::s_test_guid[16] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C
};

const ComTypeLibData ComTypeLibTest::s_test_lib_data = {
    s_test_guid,
    0x123456789ABCDEF0ull,
    "ITestInterface",
    "TestNamespace",
    2,    // method_count
    0,    // is_idispatch
    s_methods
};

TEST_F(ComTypeLibTest, GetComTypeInfoForCcwNull) {
    EXPECT_EQ(GetComTypeInfoForCcw(nullptr), nullptr);
}

TEST_F(ComTypeLibTest, GetComTypeLibForCcwNull) {
    EXPECT_EQ(GetComTypeLibForCcw(nullptr), nullptr);
}

TEST_F(ComTypeLibTest, GetComTypeInfoForCcwNoData) {
    // CCW with typelib_data = nullptr should return nullptr.
    TestCcwWithTypeLib ccw{};
    std::memset(&ccw, 0, sizeof(ccw));
    ccw.typelib_data = nullptr;
    EXPECT_EQ(GetComTypeInfoForCcw(
        reinterpret_cast<chaos::il2cpp::com_ccw::ComCcw*>(&ccw)), nullptr);
}

TEST_F(ComTypeLibTest, GetComTypeLibForCcwNoData) {
    TestCcwWithTypeLib ccw{};
    std::memset(&ccw, 0, sizeof(ccw));
    ccw.typelib_data = nullptr;
    EXPECT_EQ(GetComTypeLibForCcw(
        reinterpret_cast<chaos::il2cpp::com_ccw::ComCcw*>(&ccw)), nullptr);
}

TEST_F(ComTypeLibTest, GetComTypeInfoForCcwWithData) {
    // CCW with valid typelib_data should return a non-null ITypeInfo.
    TestCcwWithTypeLib ccw{};
    std::memset(&ccw, 0, sizeof(ccw));
    ccw.typelib_data = &s_test_lib_data;

    void* type_info = GetComTypeInfoForCcw(
        reinterpret_cast<chaos::il2cpp::com_ccw::ComCcw*>(&ccw));
    ASSERT_NE(type_info, nullptr);
    auto* vtbl = GetTypeInfoVtbl(type_info);
    ASSERT_NE(vtbl, nullptr);

    // GetTypeAttr should return a valid TYPEATTR.
    void* type_attr = nullptr;
    EXPECT_EQ(vtbl->GetTypeAttr(type_info, &type_attr), 0);
    ASSERT_NE(type_attr, nullptr);

    auto* attr = static_cast<ComTypeAttr*>(type_attr);
    EXPECT_EQ(attr->cFuncs, 2);
    EXPECT_EQ(attr->typekind, 0); // TKIND_INTERFACE
    vtbl->ReleaseTypeAttr(type_info, type_attr);

    // GetFuncDesc for method 0 (Foo).
    void* func_desc = nullptr;
    EXPECT_EQ(vtbl->GetFuncDesc(type_info, 0, &func_desc), 0);
    ASSERT_NE(func_desc, nullptr);
    auto* fd = static_cast<ComFuncDesc*>(func_desc);
    EXPECT_EQ(fd->memid, 1);
    EXPECT_EQ(fd->cParams, 1);
    vtbl->ReleaseFuncDesc(type_info, func_desc);

    // Release the type info.
    vtbl->Release(type_info);
}

TEST_F(ComTypeLibTest, GetComTypeLibForCcwWithData) {
    // CCW with valid typelib_data should return a non-null ITypeLib.
    TestCcwWithTypeLib ccw{};
    std::memset(&ccw, 0, sizeof(ccw));
    ccw.typelib_data = &s_test_lib_data;

    void* type_lib = GetComTypeLibForCcw(
        reinterpret_cast<chaos::il2cpp::com_ccw::ComCcw*>(&ccw));
    ASSERT_NE(type_lib, nullptr);
    auto* vtbl = GetTypeLibVtbl(type_lib);
    ASSERT_NE(vtbl, nullptr);

    vtbl->Release(type_lib);
}

TEST_F(ComTypeLibTest, TypeLibGetTypeInfoOfGuid) {
    // GetTypeInfoOfGuid should match the GUID from ComTypeLibData.
    TestCcwWithTypeLib ccw{};
    std::memset(&ccw, 0, sizeof(ccw));
    ccw.typelib_data = &s_test_lib_data;

    void* type_lib = GetComTypeLibForCcw(
        reinterpret_cast<chaos::il2cpp::com_ccw::ComCcw*>(&ccw));
    ASSERT_NE(type_lib, nullptr);
    auto* vtbl = GetTypeLibVtbl(type_lib);
    ASSERT_NE(vtbl, nullptr);

    // Lookup by the test GUID — should return the type info.
    void* found_info = nullptr;
    EXPECT_EQ(vtbl->GetTypeInfoOfGuid(type_lib, s_test_guid, &found_info), 0);
    EXPECT_NE(found_info, nullptr);

    // Lookup by an unknown GUID — should return error.
    const CHAOS_IL2CPP_UINT8 unknown_guid[16] = {0xFF};
    void* not_found = nullptr;
    EXPECT_NE(vtbl->GetTypeInfoOfGuid(type_lib, unknown_guid, &not_found), 0);
    EXPECT_EQ(not_found, nullptr);

    if (found_info != nullptr) {
        auto* info_vtbl = GetTypeInfoVtbl(found_info);
        if (info_vtbl) info_vtbl->Release(found_info);
    }
    vtbl->Release(type_lib);
}

TEST_F(ComTypeLibTest, TypeLibGetTypeInfo) {
    TestCcwWithTypeLib ccw{};
    std::memset(&ccw, 0, sizeof(ccw));
    ccw.typelib_data = &s_test_lib_data;

    void* type_lib = GetComTypeLibForCcw(
        reinterpret_cast<chaos::il2cpp::com_ccw::ComCcw*>(&ccw));
    ASSERT_NE(type_lib, nullptr);
    auto* vtbl = GetTypeLibVtbl(type_lib);
    ASSERT_NE(vtbl, nullptr);

    // GetTypeInfo(0) should succeed.
    void* info0 = nullptr;
    EXPECT_EQ(vtbl->GetTypeInfo(type_lib, 0, &info0), 0);
    EXPECT_NE(info0, nullptr);

    // GetTypeInfo(1) should succeed (0-based index for second type).
    void* info1 = nullptr;
    EXPECT_EQ(vtbl->GetTypeInfo(type_lib, 1, &info1), 0);
    EXPECT_NE(info1, nullptr);

    if (info0) { auto* v = GetTypeInfoVtbl(info0); if (v) v->Release(info0); }
    if (info1) { auto* v = GetTypeInfoVtbl(info1); if (v) v->Release(info1); }
    vtbl->Release(type_lib);
}
