/// com_rcw_test.cpp — RCW (Runtime Callable Wrapper) unit tests
///
/// Exercises: FindOrCreateRcw, ReleaseRcw, QueryInterfaceCached.
/// Uses a mock IUnknown to avoid requiring the actual COM runtime.

#include "com_rcw.h"
#include "com_abi.h"

#include <runtime_abi.h>

#include <gtest/gtest.h>

#include <cstring>
#include <cstdlib>
#include <new>

// ── Mock IUnknown ────────────────────────────────────────────────────────
// A minimal mock COM object with a static IUnknownVtbl that tracks
// AddRef/Release/QueryInterface calls.

namespace {

struct MockComObject {
    // Must be first field (COM convention).
    chaos::il2cpp::com_abi::IUnknownVtbl* vtbl;
    int refcount;
    int qi_call_count;
    int addref_call_count;
    int release_call_count;
};

// Static vtable for the mock.
chaos::il2cpp::com_abi::IUnknownVtbl g_mock_vtbl;

int CHAOS_RUNTIME_ABI_CALL MockQueryInterface(void* self, const void* iid, void** ppv) noexcept {
    auto* obj = static_cast<MockComObject*>(self);
    obj->qi_call_count++;
    // Pretend we support IUnknown: return self.
    *ppv = self;
    obj->refcount++;
    return 0;  // S_OK
}

uint32_t CHAOS_RUNTIME_ABI_CALL MockAddRef(void* self) noexcept {
    auto* obj = static_cast<MockComObject*>(self);
    obj->addref_call_count++;
    return static_cast<uint32_t>(++obj->refcount);
}

uint32_t CHAOS_RUNTIME_ABI_CALL MockRelease(void* self) noexcept {
    auto* obj = static_cast<MockComObject*>(self);
    obj->release_call_count++;
    int remaining = --obj->refcount;
    if (remaining == 0) {
        std::free(obj);
    }
    return static_cast<uint32_t>(remaining);
}

MockComObject* CreateMockObject() {
    // Initialize vtable once.
    static bool vtbl_initialized = false;
    if (!vtbl_initialized) {
        g_mock_vtbl.QueryInterface = MockQueryInterface;
        g_mock_vtbl.AddRef = MockAddRef;
        g_mock_vtbl.Release = MockRelease;
        vtbl_initialized = true;
    }

    auto* obj = static_cast<MockComObject*>(std::malloc(sizeof(MockComObject)));
    if (obj == nullptr) return nullptr;
    obj->vtbl = &g_mock_vtbl;
    obj->refcount = 1;
    obj->qi_call_count = 0;
    obj->addref_call_count = 0;
    obj->release_call_count = 0;
    return obj;
}

}  // anonymous namespace

// ── Tests ────────────────────────────────────────────────────────────────

TEST(ComRcw, FindOrCreateRcw) {
    auto* mock = CreateMockObject();
    ASSERT_NE(mock, nullptr);

    auto* rcw = chaos::il2cpp::com_rcw::FindOrCreateRcw(mock);
    ASSERT_NE(rcw, nullptr);

    // Magic should be set.
    EXPECT_EQ(rcw->magic, chaos::il2cpp::com_rcw::kComRcwMagic);
    EXPECT_EQ(rcw->identity_unknown, mock);
    EXPECT_EQ(rcw->wrapper_refcount, 1u);

    // The mock should have been AddRef'd on creation.
    EXPECT_EQ(mock->addref_call_count, 1);

    // Cleanup: release the RCW.
    chaos::il2cpp::com_rcw::ReleaseRcw(rcw);
}

TEST(ComRcw, FindOrCreateRcwDuplicate) {
    auto* mock = CreateMockObject();
    ASSERT_NE(mock, nullptr);

    auto* rcw1 = chaos::il2cpp::com_rcw::FindOrCreateRcw(mock);
    ASSERT_NE(rcw1, nullptr);
    EXPECT_EQ(rcw1->wrapper_refcount, 1u);

    // Second FindOrCreateRcw for the same pointer should return the same RCW.
    auto* rcw2 = chaos::il2cpp::com_rcw::FindOrCreateRcw(mock);
    ASSERT_NE(rcw2, nullptr);
    EXPECT_EQ(rcw1, rcw2);
    EXPECT_EQ(rcw2->wrapper_refcount, 2u);

    // Release both references.
    chaos::il2cpp::com_rcw::ReleaseRcw(rcw2);
    chaos::il2cpp::com_rcw::ReleaseRcw(rcw1);

    // The mock should have been released once (only when wrapper_refcount
    // reaches 0 on the final ReleaseRcw).  The first ReleaseRcw just
    // decrements wrapper_refcount from 2 to 1 without releasing the COM object.
    EXPECT_EQ(mock->release_call_count, 1);
}

TEST(ComRcw, ReleaseRcwNull) {
    // Should not crash.
    chaos::il2cpp::com_rcw::ReleaseRcw(nullptr);
}

TEST(ComRcw, FindOrCreateRcwNull) {
    auto* rcw = chaos::il2cpp::com_rcw::FindOrCreateRcw(nullptr);
    EXPECT_EQ(rcw, nullptr);
}

TEST(ComRcw, IsComRcwHandle) {
    EXPECT_FALSE(chaos::il2cpp::com_rcw::IsComRcwHandle(0));
}
