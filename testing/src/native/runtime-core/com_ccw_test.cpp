// com_ccw_test.cpp — CCW creation, aggregation, and IUnknown method tests.
//
// Tests the chaos::il2cpp::com_ccw namespace functions:
//   CcwQueryInterface, CcwAddRef, CcwRelease, CcwFromInterface,
//   CreateCcw, CreateCcwAggregated, DestroyCcw, RegisterCcwInterface,
//   CcwGetTypeInfoCount, CcwGetTypeInfo.
//
// Full CCW creation requires GcHandleNew from the ABI, which needs a
// managed runtime. These tests focus on null-safety and edge cases.

#include "marshal_test_fixture.h"

#include <com_ccw.h>

#include <cstring>

using namespace chaos::il2cpp::com_ccw;

// ════════════════════════════════════════════════════════════════════════════
// F2 — COM CCW tests
// ════════════════════════════════════════════════════════════════════════════

class ComCcwTest : public MarshalTestFixture {
protected:
    /// Helper: creates a minimal stack-allocated ComCcw for testing.
    /// RegisterCcwInterface and QI paths need a valid CCW pointer.
    struct TestCcwGuard {
        ComCcw ccw;
        TestCcwGuard() {
            std::memset(&ccw, 0, sizeof(ccw));
            ccw.vtable = &s_test_vtbl_;
            ccw.interface_count = 1;
            ccw.interfaces[0].guid = &kZeroGuid[0];
            ccw.interfaces[0].vtable = &s_test_vtbl_;
            ccw.interfaces[0].ccw_ptr = &ccw;
        }
    };

private:
    static ComCcwVtbl s_test_vtbl_;
    static const CHAOS_IL2CPP_UINT8 kZeroGuid[16];
};

ComCcwVtbl ComCcwTest::s_test_vtbl_ = {
    &CcwQueryInterface,
    &CcwAddRef,
    &CcwRelease,
};

const CHAOS_IL2CPP_UINT8 ComCcwTest::kZeroGuid[16] = {0};

// ── CcwQueryInterface ────────────────────────────────────────────────────

TEST_F(ComCcwTest, QueryInterfaceNullSelf) {
    void* ppv = nullptr;
    const CHAOS_IL2CPP_UINT8 iid[16] = {0};
    EXPECT_EQ(CcwQueryInterface(nullptr, iid, &ppv), kE_POINTER);
    EXPECT_EQ(ppv, nullptr);
}

TEST_F(ComCcwTest, QueryInterfaceNullPpv) {
    // ppv check happens before ResolveCcw, so self can be any non-null value.
    const CHAOS_IL2CPP_UINT8 iid[16] = {0};
    EXPECT_EQ(CcwQueryInterface(reinterpret_cast<void*>(0x1234), iid, nullptr),
              kE_POINTER);
}

TEST_F(ComCcwTest, QueryInterfaceNullIid) {
    // After the self/ppv null check, ResolveCcw is called, which needs a
    // valid ComCcw to avoid pointer-arithmetic crash.
    TestCcwGuard guard;
    void* ppv = nullptr;
    EXPECT_EQ(CcwQueryInterface(&guard.ccw, nullptr, &ppv), kE_POINTER);
}

TEST_F(ComCcwTest, QueryInterfaceNoMatch) {
    // QI for an IID not in the registered interfaces should return
    // E_NOINTERFACE. The zero-GUID entry matches only zero IID.
    TestCcwGuard guard;
    void* ppv = nullptr;
    const CHAOS_IL2CPP_UINT8 nonmatching_iid[16] = {
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    EXPECT_EQ(CcwQueryInterface(&guard.ccw, nonmatching_iid, &ppv),
              kE_NOINTERFACE);
    EXPECT_EQ(ppv, nullptr);
}

TEST_F(ComCcwTest, QueryInterfaceRegisteredInterface) {
    // Register a non-IUnknown interface and QI for it.
    // Must return &entry.vtable (the interface identity pointer).
    TestCcwGuard guard;
    const CHAOS_IL2CPP_UINT8 test_guid[16] = {
        0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C
    };
    void* fake_vtbl = reinterpret_cast<void*>(static_cast<uintptr_t>(0x4000));
    ASSERT_TRUE(RegisterCcwInterface(&guard.ccw, test_guid, fake_vtbl));

    void* ppv = nullptr;
    auto hr = CcwQueryInterface(&guard.ccw, test_guid, &ppv);
    EXPECT_EQ(hr, kS_OK);
    EXPECT_EQ(ppv, &guard.ccw.interfaces[1].vtable);

    // Refcount should have increased (from 0 to 1 after QI).
    EXPECT_EQ(guard.ccw.refcount.load(), 1u);
}

TEST_F(ComCcwTest, QueryInterfaceNoMatchNonZeroGuid) {
    // Ensure QI for a never-registered GUID returns E_NOINTERFACE.
    TestCcwGuard guard;
    const CHAOS_IL2CPP_UINT8 unknown_guid[16] = {
        0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88,
        0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00
    };
    void* ppv = nullptr;
    EXPECT_EQ(CcwQueryInterface(&guard.ccw, unknown_guid, &ppv),
              kE_NOINTERFACE);
    EXPECT_EQ(ppv, nullptr);
}

// ── CcwAddRef ────────────────────────────────────────────────────────────

TEST_F(ComCcwTest, AddRefNull) {
    EXPECT_EQ(CcwAddRef(nullptr), 0u);
}

TEST_F(ComCcwTest, AddRefReleaseCycle) {
    // Multiple AddRef/Release cycles staying above 0.
    TestCcwGuard guard;
    guard.ccw.refcount = 1;

    EXPECT_EQ(CcwAddRef(&guard.ccw), 2u);
    EXPECT_EQ(CcwAddRef(&guard.ccw), 3u);
    EXPECT_EQ(CcwRelease(&guard.ccw), 2u);
    EXPECT_EQ(CcwAddRef(&guard.ccw), 3u);
    EXPECT_EQ(CcwRelease(&guard.ccw), 2u);
    EXPECT_EQ(CcwRelease(&guard.ccw), 1u);
    // Refcount is back to 1 — CCW should still be valid.
    EXPECT_EQ(guard.ccw.refcount.load(), 1u);
}

TEST_F(ComCcwTest, AddRefValid) {
    TestCcwGuard guard;
    guard.ccw.refcount = 5;
    EXPECT_EQ(CcwAddRef(&guard.ccw), 6u);  // returns old+1
    EXPECT_EQ(guard.ccw.refcount.load(), 6u);
}

// ── CcwRelease ───────────────────────────────────────────────────────────

TEST_F(ComCcwTest, ReleaseNull) {
    EXPECT_EQ(CcwRelease(nullptr), 0u);
}

// ── CcwFromInterface ─────────────────────────────────────────────────────

TEST_F(ComCcwTest, FromInterfaceNull) {
    EXPECT_EQ(CcwFromInterface(nullptr), nullptr);
}

TEST_F(ComCcwTest, FromInterfaceViaEntry) {
    // When given &entry.vtable, CcwFromInterface should recover the CCW.
    TestCcwGuard guard;
    auto* iface_ptr = &guard.ccw.interfaces[0].vtable;
    auto* recovered = CcwFromInterface(iface_ptr);
    EXPECT_EQ(recovered, &guard.ccw);
}

TEST_F(ComCcwTest, FromInterfaceDirectPointer) {
    // When given the CCW base pointer (not &entry.vtable),
    // ResolveCcw's fallback path should return the CCW directly.
    TestCcwGuard guard;
    auto* recovered = CcwFromInterface(
        reinterpret_cast<void*>(&guard.ccw.interfaces[0].vtable));
    EXPECT_EQ(recovered, &guard.ccw);
}

// ── RegisterCcwInterface ─────────────────────────────────────────────────

TEST_F(ComCcwTest, RegisterInterfaceNullCcw) {
    const CHAOS_IL2CPP_UINT8 guid[16] = {0};
    void* vtable = reinterpret_cast<void*>(static_cast<uintptr_t>(0x1000));
    EXPECT_FALSE(RegisterCcwInterface(nullptr, guid, vtable));
}

TEST_F(ComCcwTest, RegisterInterfaceNullGuid) {
    TestCcwGuard guard;
    EXPECT_FALSE(RegisterCcwInterface(&guard.ccw, nullptr,
        reinterpret_cast<void*>(0x1000)));
}

TEST_F(ComCcwTest, RegisterInterfaceNullVtable) {
    TestCcwGuard guard;
    const CHAOS_IL2CPP_UINT8 guid[16] = {0};
    EXPECT_FALSE(RegisterCcwInterface(&guard.ccw, guid, nullptr));
}

TEST_F(ComCcwTest, RegisterInterfaceValid) {
    TestCcwGuard guard;
    const CHAOS_IL2CPP_UINT8 guid[16] = {1, 2, 3, 4};
    void* vtable = reinterpret_cast<void*>(static_cast<uintptr_t>(0x2000));

    EXPECT_TRUE(RegisterCcwInterface(&guard.ccw, guid, vtable));
    EXPECT_EQ(guard.ccw.interface_count, 2u);
    EXPECT_EQ(guard.ccw.interfaces[1].guid, guid);
    EXPECT_EQ(guard.ccw.interfaces[1].vtable, vtable);
    EXPECT_EQ(guard.ccw.interfaces[1].ccw_ptr, &guard.ccw);
}

TEST_F(ComCcwTest, RegisterInterfaceFull) {
    // Fill all kMaxCcwInterfaces slots, then verify the next register fails.
    TestCcwGuard guard;
    // ccw already has interface_count = 1 (IUnknown slot at index 0).
    // Fill 3 more to reach kMaxCcwInterfaces = 4.
    const CHAOS_IL2CPP_UINT8 guids[3][16] = {};
    void* fake_vtbl = reinterpret_cast<void*>(0x3000);
    for (uint32_t i = 0; i < 3; ++i) {
        EXPECT_TRUE(RegisterCcwInterface(&guard.ccw, guids[i], fake_vtbl));
    }
    EXPECT_EQ(guard.ccw.interface_count, 4u);

    // Next register should fail.
    const CHAOS_IL2CPP_UINT8 extra_guid[16] = {};
    EXPECT_FALSE(RegisterCcwInterface(&guard.ccw, extra_guid, fake_vtbl));
    EXPECT_EQ(guard.ccw.interface_count, 4u);  // unchanged
}

TEST_F(ComCcwTest, RegisterInterfaceMaxThenQI) {
    // Fill all slots, then QI for each registered IID should succeed.
    TestCcwGuard guard;
    const CHAOS_IL2CPP_UINT8 guids[3][16] = {
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    };
    for (uint32_t i = 0; i < 3; ++i) {
        EXPECT_TRUE(RegisterCcwInterface(&guard.ccw, guids[i],
            reinterpret_cast<void*>(static_cast<uintptr_t>(0x3000 + i))));
    }

    // QI for each registered interface should succeed.
    for (uint32_t i = 0; i < 3; ++i) {
        void* ppv = nullptr;
        auto hr = CcwQueryInterface(&guard.ccw, guids[i], &ppv);
        EXPECT_EQ(hr, kS_OK);
        EXPECT_NE(ppv, nullptr);
    }

    // QI for an unregistered non-zero GUID should still fail.
    const CHAOS_IL2CPP_UINT8 unknown[16] = {
        0xFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    void* ppv = nullptr;
    EXPECT_EQ(CcwQueryInterface(&guard.ccw, unknown, &ppv), kE_NOINTERFACE);
}

// ── CreateCcw ────────────────────────────────────────────────────────────

TEST_F(ComCcwTest, CreateCcwNullObject) {
    EXPECT_EQ(CreateCcw(nullptr, nullptr), 0);
    EXPECT_EQ(CreateCcw(nullptr, reinterpret_cast<void*>(0x1234)), 0);
}

// ── CreateCcwAggregated ──────────────────────────────────────────────────

TEST_F(ComCcwTest, CreateCcwAggregatedNullObject) {
    EXPECT_EQ(CreateCcwAggregated(nullptr, nullptr, nullptr), 0);
    EXPECT_EQ(CreateCcwAggregated(nullptr, reinterpret_cast<void*>(0x1000),
                                   reinterpret_cast<void*>(0x2000)), 0);
}

TEST_F(ComCcwTest, CreateCcwAggregatedNullOuter) {
    EXPECT_EQ(CreateCcwAggregated(reinterpret_cast<void*>(0x3000),
                                   reinterpret_cast<void*>(0x4000), nullptr), 0);
}

// ── DestroyCcw ───────────────────────────────────────────────────────────

TEST_F(ComCcwTest, DestroyCcwNull) {
    // Should be a no-op, no crash
    DestroyCcw(nullptr);
    SUCCEED();
}

// ── CcwGetTypeInfoCount ──────────────────────────────────────────────────

TEST_F(ComCcwTest, GetTypeInfoCountNullPctinfo) {
    EXPECT_EQ(CcwGetTypeInfoCount(reinterpret_cast<void*>(0x1234), nullptr),
              kE_POINTER);
}

// ── CcwGetTypeInfo ───────────────────────────────────────────────────────

TEST_F(ComCcwTest, GetTypeInfoNullPpTInfo) {
    EXPECT_EQ(CcwGetTypeInfo(reinterpret_cast<void*>(0x1234), 0, 0, nullptr),
              kE_POINTER);
}
