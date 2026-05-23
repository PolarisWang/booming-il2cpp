// com_dispatch_test.cpp — IDispatch Invoke unit tests.

#include "marshal_test_fixture.h"

#include <com_ccw.h>
#include <com_typelib.h>
#include <com_abi.h>

#include <cstring>

namespace ccw = chaos::il2cpp::com_ccw;
namespace rc  = chaos::il2cpp::runtime_core;
namespace abi = chaos::il2cpp::com_abi;

// ════════════════════════════════════════════════════════════════════════════
// D1 — IDispatch Invoke tests
// ════════════════════════════════════════════════════════════════════════════

class ComDispatchTest : public MarshalTestFixture {
protected:
    static const rc::ComParamData s_param_x[1];
    static const rc::ComMethodData s_method;

    static const CHAOS_IL2CPP_UINT8 s_test_guid[16];
    static const rc::ComTypeLibData s_typelib_data;

    // Shadow struct matching src:native/runtime-core/com_ccw.h ComCcw (x64).
    // Must be kept in sync with the real struct layout.
    // ComCcwInterfaceEntry = 24 bytes (guid* 8 + vtable* 8 + ccw_ptr* 8).
    // inline_interfaces[4] = 96 bytes.
    struct ComCcwOld {
        void* vtable;
        std::atomic<CHAOS_IL2CPP_UINT32> refcount;
        CHAOS_IL2CPP_UINT64 gc_handle;
        void* runtime_state;
        CHAOS_IL2CPP_SIZE interface_count;
        CHAOS_IL2CPP_SIZE interface_capacity;
        void* outer_unknown;
        bool  is_aggregated;
        void* interfaces_ptr;
        void* inline_interfaces[12];  // ComCcwInterfaceEntry[4] = 96 bytes = 12 void*
        const rc::ComTypeLibData* typelib_data;
        void* cp_container;
    };
};

const rc::ComParamData ComDispatchTest::s_param_x[1] = {
    {"x", 0x03u, 0x01u}
};

const rc::ComMethodData ComDispatchTest::s_method = {
    "Add", 1, 1, 3, 0x06000001u, s_param_x
};

const CHAOS_IL2CPP_UINT8 ComDispatchTest::s_test_guid[16] = {
    0x11, 0x22, 0x33, 0x44, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C
};

const rc::ComTypeLibData ComDispatchTest::s_typelib_data = {
    s_test_guid,
    0xFEDCBA9876543210ull,
    "ICalc",
    "CalcNamespace",
    1,
    1,
    &s_method
};

TEST_F(ComDispatchTest, CcwDispatchMethodInvokeNullCcw) {
    abi::DISPPARAMS params{};
    int64_t result = 0;
    EXPECT_EQ(ccw::CcwDispatchMethodInvoke(nullptr, 0, 1, &params, &result),
              ccw::kE_POINTER);
}

TEST_F(ComDispatchTest, CcwDispatchMethodInvokeNoTypeLib) {
    ComCcwOld ccw{};
    std::memset(&ccw, 0, sizeof(ccw));
    ccw.typelib_data = nullptr;

    abi::DISPPARAMS params{};
    int64_t result = 0;
    EXPECT_EQ(ccw::CcwDispatchMethodInvoke(&ccw, 0, 0, &params, &result),
              ccw::kDISP_E_MEMBERNOTFOUND);
}

TEST_F(ComDispatchTest, CcwDispatchMethodInvokeDispIdOutOfRange) {
    ComCcwOld ccw{};
    std::memset(&ccw, 0, sizeof(ccw));
    ccw.typelib_data = &s_typelib_data;

    abi::DISPPARAMS params{};
    int64_t result = 0;
    EXPECT_EQ(ccw::CcwDispatchMethodInvoke(&ccw, 0, -1, &params, &result),
              ccw::kDISP_E_MEMBERNOTFOUND);
}

TEST_F(ComDispatchTest, CcwDispatchMethodInvokeDispIdTooHigh) {
    ComCcwOld ccw{};
    std::memset(&ccw, 0, sizeof(ccw));
    ccw.typelib_data = &s_typelib_data;

    abi::DISPPARAMS params{};
    int64_t result = 0;
    EXPECT_EQ(ccw::CcwDispatchMethodInvoke(&ccw, 0, 100, &params, &result),
              ccw::kDISP_E_MEMBERNOTFOUND);
}

TEST_F(ComDispatchTest, CcwDispatchMethodInvokeNullParams) {
    ComCcwOld ccw{};
    std::memset(&ccw, 0, sizeof(ccw));
    ccw.typelib_data = &s_typelib_data;
    ccw.gc_handle = static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_GC_HANDLE_INVALID);

    int64_t result = 0;
    auto hr = ccw::CcwDispatchMethodInvoke(&ccw, 0, 0, nullptr, &result);
    EXPECT_TRUE(hr == ccw::kE_NOTIMPL || hr == ccw::kS_OK)
        << "hr=0x" << std::hex << static_cast<uint32_t>(hr) << std::dec;
}

TEST_F(ComDispatchTest, CcwDispatchMethodInvokeEmptyParams) {
    ComCcwOld ccw{};
    std::memset(&ccw, 0, sizeof(ccw));
    ccw.typelib_data = &s_typelib_data;
    ccw.gc_handle = static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_GC_HANDLE_INVALID);

    abi::DISPPARAMS params{};
    params.cArgs = 0;
    params.cNamedArgs = 0;
    params.rgvarg = nullptr;

    int64_t result = 0;
    auto hr = ccw::CcwDispatchMethodInvoke(&ccw, 0, 0, &params, &result);
    EXPECT_TRUE(hr == ccw::kE_NOTIMPL || hr == ccw::kS_OK)
        << "hr=0x" << std::hex << static_cast<uint32_t>(hr) << std::dec;
}

TEST_F(ComDispatchTest, CcwDispatchMethodInvokeWithArgsNoRuntime) {
    ComCcwOld ccw{};
    std::memset(&ccw, 0, sizeof(ccw));
    ccw.typelib_data = &s_typelib_data;
    ccw.gc_handle = static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_GC_HANDLE_INVALID);

    uint8_t variant_buf[16] = {};
    uint16_t vt_i4 = 0x03u;
    std::memcpy(variant_buf, &vt_i4, sizeof(uint16_t));
    int32_t val = 42;
    std::memcpy(variant_buf + 8, &val, sizeof(val));

    abi::DISPPARAMS params{};
    params.cArgs = 1;
    params.rgvarg = reinterpret_cast<void*>(&variant_buf[0]);

    int64_t result = 0;
    auto hr = ccw::CcwDispatchMethodInvoke(&ccw, 0, 0, &params, &result);
    EXPECT_TRUE(hr == ccw::kE_NOTIMPL || hr == ccw::kS_OK || hr == 0x80020009)
        << "hr=0x" << std::hex << static_cast<uint32_t>(hr) << std::dec;
}

TEST_F(ComDispatchTest, ZeroResultBufferOk) {
    ComCcwOld ccw{};
    std::memset(&ccw, 0, sizeof(ccw));
    ccw.typelib_data = &s_typelib_data;
    ccw.gc_handle = static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_GC_HANDLE_INVALID);

    int64_t result = 0;
    auto hr = ccw::CcwDispatchMethodInvoke(&ccw, 0, 0, nullptr, &result);
    EXPECT_TRUE(hr == ccw::kE_NOTIMPL || hr == ccw::kS_OK)
        << "hr=0x" << std::hex << static_cast<uint32_t>(hr) << std::dec;
}
