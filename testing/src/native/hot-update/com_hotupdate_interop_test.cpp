/// com_hotupdate_interop_test.cpp — COM + hot-update interop test
///
/// Verifies that COM CCW/RCW operations are not disrupted by method
/// replacement registration.  This is a smoke-level integration test:
/// if both subsystems initialize and operate without crashing, the test
/// passes.

#include <runtime_abi.h>

#include "com_ccw.h"
#include "method_replacement.h"

#include <gtest/gtest.h>

#include <cstdlib>

namespace {

// AOT symbol stubs (required by chaos_interpreter linkage).
extern "C" const char* kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int kChaosExternalRuntimeCount = 0;
extern "C" void* chaos_il2cpp_aot_hotpatch_module = nullptr;

int g_managed_obj = 0;

void* DummyReplacement() {
    return reinterpret_cast<void*>(0xAAu);
}

}  // anonymous namespace

TEST(ComHotupdateInterop, CreateCcwAfterMethodReplacement) {
    using chaos::il2cpp::method_replacement::Register;
    using chaos::il2cpp::method_replacement::RevertAll;

    RevertAll();

    // Register a method replacement first.
    ASSERT_TRUE(Register(0x06000001u, reinterpret_cast<void*>(&DummyReplacement)));

    // Now create a CCW — should not crash.
    auto handle = chaos::il2cpp::com_ccw::CreateCcw(&g_managed_obj, nullptr);
    EXPECT_NE(handle, 0);
    auto* ccw = reinterpret_cast<chaos::il2cpp::com_ccw::ComCcw*>(
        static_cast<void*>(handle));
    EXPECT_NE(ccw->vtable, nullptr);

    // QI should work.
    const CHAOS_IL2CPP_UINT8 iid_iunknown[16] = {0};
    void* ppv = nullptr;
    auto hr = chaos::il2cpp::com_ccw::CcwQueryInterface(ccw, iid_iunknown, &ppv);
    EXPECT_EQ(hr, chaos::il2cpp::com_ccw::kS_OK);
    EXPECT_EQ(ppv, ccw);

    // AddRef/Release should work.
    uint32_t ref = chaos::il2cpp::com_ccw::CcwAddRef(ccw);
    EXPECT_EQ(ref, 2u);
    ref = chaos::il2cpp::com_ccw::CcwRelease(ccw);
    EXPECT_EQ(ref, 1u);

    // Now register another replacement while CCW exists.
    ASSERT_TRUE(Register(0x06000002u, reinterpret_cast<void*>(&DummyReplacement)));

    // CCW should still be valid.
    ref = chaos::il2cpp::com_ccw::CcwAddRef(ccw);
    EXPECT_EQ(ref, 2u);
    chaos::il2cpp::com_ccw::CcwRelease(ccw);

    // Cleanup.
    RevertAll();
}
