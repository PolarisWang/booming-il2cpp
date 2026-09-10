/// com_ccw_test.cpp — CCW (COM Callable Wrapper) unit tests
///
/// Exercises: CreateCcw, QueryInterface, AddRef, Release, RegisterCcwInterface,
/// CreateCcwAggregated, DestroyCcw.

#include <runtime_abi.h>

#include "com_ccw.h"

#include <gtest/gtest.h>

#include <cstring>
#include <cstdlib>

// ── Mock RuntimeState for GCHandle simulation ────────────────────────────

namespace {

struct MockRuntimeState {
    int dummy;
};

// Track GCHandle operations.
int g_gc_handle_new_count = 0;
int g_gc_handle_free_count = 0;
int g_gc_handle_get_count = 0;

}  // anonymous namespace

// ── Tests ────────────────────────────────────────────────────────────────

TEST(ComCcw, CreateCcw) {
    int managed_object = 42;
    MockRuntimeState state;

    auto handle = chaos::il2cpp::com_ccw::CreateCcw(&managed_object, &state);
    EXPECT_NE(handle, 0);

    // The handle should point to a valid ComCcw.
    auto* ccw = reinterpret_cast<chaos::il2cpp::com_ccw::ComCcw*>(
        reinterpret_cast<void*>(handle));
    EXPECT_NE(ccw->vtable, nullptr);
    EXPECT_EQ(ccw->refcount.load(), 1u);
    EXPECT_EQ(ccw->interface_count, static_cast<CHAOS_IL2CPP_SIZE>(1));
    EXPECT_EQ(ccw->cp_container, nullptr);

    // IUnknown interface should be registered.
    EXPECT_NE(ccw->interfaces[0].vtable, nullptr);
    EXPECT_EQ(ccw->interfaces[0].ccw_ptr, ccw);

    // Release: AddRef first, then Release twice to trigger cleanup.
    ccw->refcount.fetch_add(1);
    uint32_t remaining = ccw->refcount.fetch_sub(2) - 2;
    // refcount was 1+1=2, subtract 2 => 0
    // Since CreateCcw sets refcount=1, then we added 1 => 2, then sub 2 => 0
    // CcwRelease should free the CCW when refcount hits 0.
    // But our test doesn't call the actual Release function — we handle manually.
    // We'll test the actual QI/AddRef/Release functions separately.

    // For now, just verify the CCW can be used.
    EXPECT_EQ(handle, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ccw));
}

TEST(ComCcw, CreateCcwNullManagedObject) {
    MockRuntimeState state;
    auto handle = chaos::il2cpp::com_ccw::CreateCcw(nullptr, &state);
    EXPECT_EQ(handle, 0);
}

TEST(ComCcw, CcwQueryInterface) {
    int managed_object = 42;
    MockRuntimeState state;

    auto handle = chaos::il2cpp::com_ccw::CreateCcw(&managed_object, &state);
    ASSERT_NE(handle, 0);
    auto* ccw = reinterpret_cast<chaos::il2cpp::com_ccw::ComCcw*>(
        reinterpret_cast<void*>(handle));

    // QI for IUnknown (all-zeroes IID).
    void* ppv = nullptr;
    const CHAOS_IL2CPP_UINT8 iid_iunknown[16] = {0};
    auto hr = chaos::il2cpp::com_ccw::CcwQueryInterface(ccw, iid_iunknown, &ppv);

    EXPECT_EQ(hr, chaos::il2cpp::com_ccw::kS_OK);
    EXPECT_EQ(ppv, ccw);  // IUnknown identity is the CCW itself.
    EXPECT_EQ(ccw->refcount.load(), 2u);  // refcount should have increased.

    // Release the ref from QI.
    chaos::il2cpp::com_ccw::CcwRelease(ccw);
    EXPECT_EQ(ccw->refcount.load(), 1u);
}

TEST(ComCcw, CcwQueryInterfaceNullArgs) {
    int managed_object = 42;
    MockRuntimeState state;
    auto handle = chaos::il2cpp::com_ccw::CreateCcw(&managed_object, &state);
    ASSERT_NE(handle, 0);
    auto* ccw = reinterpret_cast<chaos::il2cpp::com_ccw::ComCcw*>(
        reinterpret_cast<void*>(handle));

    // null ppv
    const CHAOS_IL2CPP_UINT8 iid[16] = {0};
    auto hr = chaos::il2cpp::com_ccw::CcwQueryInterface(nullptr, iid, nullptr);
    EXPECT_EQ(hr, chaos::il2cpp::com_ccw::kE_POINTER);

    // null self
    void* ppv = nullptr;
    hr = chaos::il2cpp::com_ccw::CcwQueryInterface(nullptr, iid, &ppv);
    EXPECT_EQ(hr, chaos::il2cpp::com_ccw::kE_POINTER);

    // null iid
    hr = chaos::il2cpp::com_ccw::CcwQueryInterface(ccw, nullptr, &ppv);
    EXPECT_EQ(hr, chaos::il2cpp::com_ccw::kE_POINTER);
}

TEST(ComCcw, CcwAddRef) {
    int managed_object = 42;
    MockRuntimeState state;
    auto handle = chaos::il2cpp::com_ccw::CreateCcw(&managed_object, &state);
    ASSERT_NE(handle, 0);
    auto* ccw = reinterpret_cast<chaos::il2cpp::com_ccw::ComCcw*>(
        reinterpret_cast<void*>(handle));

    uint32_t prev = chaos::il2cpp::com_ccw::CcwAddRef(ccw);
    EXPECT_EQ(prev, 2u);  // was 1, now 2

    prev = chaos::il2cpp::com_ccw::CcwAddRef(ccw);
    EXPECT_EQ(prev, 3u);
}

TEST(ComCcw, CcwAddRefNull) {
    uint32_t result = chaos::il2cpp::com_ccw::CcwAddRef(nullptr);
    EXPECT_EQ(result, 0u);
}

TEST(ComCcw, CcwRelease) {
    int managed_object = 42;
    MockRuntimeState state;
    auto handle = chaos::il2cpp::com_ccw::CreateCcw(&managed_object, &state);
    ASSERT_NE(handle, 0);
    auto* ccw = reinterpret_cast<chaos::il2cpp::com_ccw::ComCcw*>(
        reinterpret_cast<void*>(handle));

    // AddRef to 2, then Release back to 1.
    chaos::il2cpp::com_ccw::CcwAddRef(ccw);
    uint32_t remaining = chaos::il2cpp::com_ccw::CcwRelease(ccw);
    EXPECT_EQ(remaining, 1u);

    // Not yet 0, so CCW should still be valid.
    EXPECT_NE(ccw->vtable, nullptr);
}

TEST(ComCcw, CcwReleaseNull) {
    uint32_t result = chaos::il2cpp::com_ccw::CcwRelease(nullptr);
    EXPECT_EQ(result, 0u);
}

TEST(ComCcw, RegisterCcwInterface) {
    int managed_object = 42;
    MockRuntimeState state;
    auto handle = chaos::il2cpp::com_ccw::CreateCcw(&managed_object, &state);
    ASSERT_NE(handle, 0);
    auto* ccw = reinterpret_cast<chaos::il2cpp::com_ccw::ComCcw*>(
        reinterpret_cast<void*>(handle));

    // Register a mock interface.
    CHAOS_IL2CPP_UINT8 mock_guid[16] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
    };
    chaos::il2cpp::com_ccw::ComCcwVtbl mock_vtbl{};
    bool ok = chaos::il2cpp::com_ccw::RegisterCcwInterface(
        ccw, mock_guid, &mock_vtbl);
    EXPECT_TRUE(ok);
    EXPECT_EQ(ccw->interface_count, static_cast<CHAOS_IL2CPP_SIZE>(2));
    EXPECT_EQ(std::memcmp(ccw->interfaces[1].guid, mock_guid, 16), 0);
    EXPECT_EQ(ccw->interfaces[1].vtable, &mock_vtbl);
    EXPECT_EQ(ccw->interfaces[1].ccw_ptr, ccw);

    // QI for the registered interface should succeed.
    void* ppv = nullptr;
    auto hr = chaos::il2cpp::com_ccw::CcwQueryInterface(
        ccw, mock_guid, &ppv);
    EXPECT_EQ(hr, chaos::il2cpp::com_ccw::kS_OK);
    EXPECT_EQ(ppv, &ccw->interfaces[1].vtable);
}

TEST(ComCcw, RegisterCcwInterfaceNullArgs) {
    CHAOS_IL2CPP_UINT8 guid[16] = {0};
    chaos::il2cpp::com_ccw::ComCcwVtbl vtbl{};
    EXPECT_FALSE(chaos::il2cpp::com_ccw::RegisterCcwInterface(nullptr, guid, &vtbl));
    EXPECT_FALSE(chaos::il2cpp::com_ccw::RegisterCcwInterface(&vtbl, nullptr, &vtbl));
    EXPECT_FALSE(chaos::il2cpp::com_ccw::RegisterCcwInterface(&vtbl, guid, nullptr));
}

TEST(ComCcw, RegisterCcwInterfaceFull) {
    int managed_object = 42;
    MockRuntimeState state;
    auto handle = chaos::il2cpp::com_ccw::CreateCcw(&managed_object, &state);
    ASSERT_NE(handle, 0);
    auto* ccw = reinterpret_cast<chaos::il2cpp::com_ccw::ComCcw*>(
        reinterpret_cast<void*>(handle));

    // Fill all interface slots.
    CHAOS_IL2CPP_UINT8 guids[3][16] = {};
    guids[0][0] = 0x10;
    guids[1][0] = 0x20;
    guids[2][0] = 0x30;
    chaos::il2cpp::com_ccw::ComCcwVtbl vtbls[3]{};

    EXPECT_TRUE(chaos::il2cpp::com_ccw::RegisterCcwInterface(ccw, guids[0], &vtbls[0]));
    EXPECT_TRUE(chaos::il2cpp::com_ccw::RegisterCcwInterface(ccw, guids[1], &vtbls[1]));
    EXPECT_TRUE(chaos::il2cpp::com_ccw::RegisterCcwInterface(ccw, guids[2], &vtbls[2]));
    // 4th should fail (kMaxCcwInterfaces = 4, but slot 0 is IUnknown).
    EXPECT_FALSE(chaos::il2cpp::com_ccw::RegisterCcwInterface(ccw, guids[0], &vtbls[0]));
}

TEST(ComCcw, CreateCcwAggregated) {
    int managed_object = 42;
    MockRuntimeState state;

    // Create an outer CCW to serve as the controlling IUnknown.
    auto outer_handle = chaos::il2cpp::com_ccw::CreateCcw(&managed_object, &state);
    ASSERT_NE(outer_handle, 0);
    auto* outer = reinterpret_cast<chaos::il2cpp::com_ccw::ComCcw*>(
        reinterpret_cast<void*>(outer_handle));

    auto handle = chaos::il2cpp::com_ccw::CreateCcwAggregated(
        &managed_object, &state, outer);
    EXPECT_NE(handle, 0);
    auto* ccw = reinterpret_cast<chaos::il2cpp::com_ccw::ComCcw*>(
        reinterpret_cast<void*>(handle));
    EXPECT_TRUE(ccw->is_aggregated);
    EXPECT_EQ(ccw->outer_unknown, outer);

    // Cleanup: destroy the aggregated CCW, then release outer.
    chaos::il2cpp::com_ccw::DestroyCcw(ccw);
    chaos::il2cpp::com_ccw::CcwRelease(outer);  // release outer's initial ref
}

TEST(ComCcw, DestroyCcwNull) {
    // Should not crash.
    chaos::il2cpp::com_ccw::DestroyCcw(nullptr);
}
