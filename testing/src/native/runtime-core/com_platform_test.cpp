// com_platform_test.cpp — COM platform abstraction layer unit tests.
//
// Tests the com_platform.h abstraction layer that wraps Win32 COM API calls.
// On Win32: tests verify null-safety and error handling without real COM.
// On non-Win32: tests verify that stub implementations return E_NOTIMPL/0.

#include "marshal_test_fixture.h"

#include <com_platform.h>

#include <cstring>

using namespace chaos::il2cpp::runtime_core::com_platform;

// ════════════════════════════════════════════════════════════════════════════
// P1 — Platform abstraction tests
// ════════════════════════════════════════════════════════════════════════════

class ComPlatformTest : public MarshalTestFixture {
};

TEST_F(ComPlatformTest, PlatformQueryInterfaceNull) {
    void* ppv = nullptr;
    const CHAOS_IL2CPP_UINT8 iid[16] = {0};
    // Null unknown pointer — should return E_POINTER or E_NOTIMPL.
    auto hr = PlatformQueryInterface(nullptr, iid, &ppv);
    EXPECT_TRUE(hr != 0);  // non-zero = error
}

TEST_F(ComPlatformTest, PlatformQueryInterfaceNullPpv) {
    const CHAOS_IL2CPP_UINT8 iid[16] = {0};
    void* fake_ptr = reinterpret_cast<void*>(static_cast<uintptr_t>(0x1234));
    auto hr = PlatformQueryInterface(fake_ptr, iid, nullptr);
    EXPECT_TRUE(hr != 0);
}

TEST_F(ComPlatformTest, PlatformQueryInterfaceNullIid) {
    void* ppv = nullptr;
    void* fake_ptr = reinterpret_cast<void*>(static_cast<uintptr_t>(0x1234));
    auto hr = PlatformQueryInterface(fake_ptr, nullptr, &ppv);
    EXPECT_TRUE(hr != 0);
}

TEST_F(ComPlatformTest, PlatformAddRefNull) {
    EXPECT_EQ(PlatformAddRef(nullptr), 0u);
}

TEST_F(ComPlatformTest, PlatformReleaseNull) {
    EXPECT_EQ(PlatformRelease(nullptr), 0u);
}

TEST_F(ComPlatformTest, PlatformCoCreateInstanceNullClsid) {
    const CHAOS_IL2CPP_UINT8 iid[16] = {0};
    EXPECT_EQ(PlatformCoCreateInstance(nullptr, iid), nullptr);
}

TEST_F(ComPlatformTest, PlatformCoCreateInstanceNullIid) {
    const CHAOS_IL2CPP_UINT8 clsid[16] = {0};
    EXPECT_EQ(PlatformCoCreateInstance(clsid, nullptr), nullptr);
}

TEST_F(ComPlatformTest, PlatformCoCreateInstanceAggregatedNullOuter) {
    const CHAOS_IL2CPP_UINT8 clsid[16] = {0};
    const CHAOS_IL2CPP_UINT8 iid[16] = {0};
    EXPECT_EQ(PlatformCoCreateInstanceAggregated(clsid, iid, nullptr), nullptr);
}

TEST_F(ComPlatformTest, PlatformSysAllocStringNull) {
    EXPECT_EQ(PlatformSysAllocString(nullptr), nullptr);
}

TEST_F(ComPlatformTest, PlatformSysStringLenNull) {
    EXPECT_EQ(PlatformSysStringLen(nullptr), 0u);
}

TEST_F(ComPlatformTest, PlatformSysFreeStringNull) {
    // Should be a no-op, no crash.
    PlatformSysFreeString(nullptr);
    SUCCEED();
}

TEST_F(ComPlatformTest, PlatformSysAllocStringFreeRoundtrip) {
    // Allocate a BSTR from a UTF-16 string, then free it.
    const CHAOS_IL2CPP_UINT16 test_str[] = {'H', 'e', 'l', 'l', 'o', 0};
    void* bstr = PlatformSysAllocString(test_str);
    if (bstr != nullptr) {
        // Verify length.
        CHAOS_IL2CPP_UINT32 len = PlatformSysStringLen(bstr);
        EXPECT_EQ(len, 5u);

        PlatformSysFreeString(bstr);
        SUCCEED();
    } else {
        // On non-Win32, SysAllocString returns nullptr — that's OK.
        SUCCEED();
    }
}

TEST_F(ComPlatformTest, PlatformSysAllocStringEmpty) {
    const CHAOS_IL2CPP_UINT16 empty_str[] = {0};
    void* bstr = PlatformSysAllocString(empty_str);
    if (bstr != nullptr) {
        CHAOS_IL2CPP_UINT32 len = PlatformSysStringLen(bstr);
        EXPECT_EQ(len, 0u);
        PlatformSysFreeString(bstr);
    } else {
        SUCCEED();
    }
}
