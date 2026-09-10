// com_ccw_test.cpp — CCW creation, aggregation, and IUnknown method tests.
//
// Tests the chaos::il2cpp::com_ccw namespace functions:
//   CcwQueryInterface, CcwAddRef, CcwRelease, CcwFromInterface,
//   RegisterCcwInterface, CcwGetTypeInfoCount, CcwGetTypeInfo.
//
// IMPORTANT: These tests compile against the CURRENT header (which has
// struct changes) but LINK against the prebuilt chaos_runtime_core.lib
// (compiled from committed code). To avoid ABI mismatch we use a local
// shadow struct (ComCcwOld) that mirrors the committed layout for test
// setup, then reinterpret_cast to void* for API calls. The prebuilt lib's
// ResolveCcw range-check safely handles direct CCW pointers.
//
// NOTE: All tests start with refcount=1 to prevent the prebuilt CcwRelease
// from attempting to free the stack-allocated CCW when refcount hits 0.

#include "marshal_test_fixture.h"

#include <com_ccw.h>

#include <cstring>

// Local copy of the post-industrialization ComCcw struct layout (mirrors
// the prebuilt chaos_runtime_core.lib). Updated for SmallVector pattern:
// interfaces is now a POINTER, with inline_interfaces[4] after it.
struct ComCcwOld {
    chaos::il2cpp::com_ccw::ComCcwVtbl* vtable;           //  0:  8
    std::atomic<CHAOS_IL2CPP_UINT32> refcount;             //  8:  4
    CHAOS_IL2CPP_UINT64 gc_handle;                         // 16:  8
    void* runtime_state;                                    // 24:  8
    CHAOS_IL2CPP_SIZE interface_count;                      // 32:  8
    CHAOS_IL2CPP_SIZE interface_capacity;                   // 40:  8  ← NEW
    void* outer_unknown;                                    // 48:  8
    bool  is_aggregated;                                    // 56:  1
    // padding 7
    chaos::il2cpp::com_ccw::ComCcwInterfaceEntry* interfaces;           // 64:  8 (pointer)
    chaos::il2cpp::com_ccw::ComCcwInterfaceEntry inline_interfaces[4];  // 72: 96
    void* typelib_data;                                     // 168: 8  ← NEW
    void* cp_container;                                     // 176: 8
};
static_assert(sizeof(ComCcwOld) == 184, "ComCcwOld layout mismatch");

using namespace chaos::il2cpp::com_ccw;

// ════════════════════════════════════════════════════════════════════════════
// F2 — COM CCW tests
// ════════════════════════════════════════════════════════════════════════════

class ComCcwTest : public MarshalTestFixture {
protected:
    /// Stack-allocated shadow CCW using the committed struct layout.
    /// The prebuilt lib accesses fields at committed offsets; our local
    /// ComCcwOld guarantees the correct layout regardless of the current
    /// com_ccw.h header.
    struct TestCcwGuard {
        ComCcwOld ccw;

        /// Returns a void* pointer suitable for passing to IUnknown API
        /// functions (CcwQueryInterface, CcwAddRef, CcwRelease, etc.).
        /// The prebuilt lib's ResolveCcw safely handles direct CCW pointers.
        void* self() noexcept { return &ccw; }

        TestCcwGuard() {
            std::memset(&ccw, 0, sizeof(ccw));
            ccw.vtable = &s_test_vtbl_;
            ccw.refcount = 1;  // Start at 1 to prevent free on Release
            ccw.interface_count = 1;
            ccw.interface_capacity = kInlineCcwInterfaces;
            ccw.interfaces = ccw.inline_interfaces;  // Point to inline storage
            ccw.inline_interfaces[0].guid = s_zero_guid;
            ccw.inline_interfaces[0].vtable = &s_test_vtbl_;
            ccw.inline_interfaces[0].ccw_ptr = &ccw;
        }
    };

    static ComCcwVtbl s_test_vtbl_;
    static const CHAOS_IL2CPP_UINT8 s_zero_guid[16];
};

ComCcwVtbl ComCcwTest::s_test_vtbl_ = {
    &CcwQueryInterface,
    &CcwAddRef,
    &CcwRelease,
};

const CHAOS_IL2CPP_UINT8 ComCcwTest::s_zero_guid[16] = {0};

// ── CcwQueryInterface ───────────────────────────────────────────────────────

TEST_F(ComCcwTest, QueryInterfaceNullSelf) {
    void* ppv = nullptr;
    const CHAOS_IL2CPP_UINT8 iid[16] = {0};
    EXPECT_EQ(CcwQueryInterface(nullptr, iid, &ppv), kE_POINTER);
    EXPECT_EQ(ppv, nullptr);
}

TEST_F(ComCcwTest, QueryInterfaceNullPpv) {
    const CHAOS_IL2CPP_UINT8 iid[16] = {0};
    EXPECT_EQ(CcwQueryInterface(reinterpret_cast<void*>(0x1234), iid, nullptr),
              kE_POINTER);
}

TEST_F(ComCcwTest, QueryInterfaceNullIid) {
    TestCcwGuard guard;
    void* ppv = nullptr;
    EXPECT_EQ(CcwQueryInterface(guard.self(), nullptr, &ppv), kE_POINTER);
}

TEST_F(ComCcwTest, QueryInterfaceNoMatch) {
    TestCcwGuard guard;
    const CHAOS_IL2CPP_UINT8 nonmatching_iid[16] = {
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    void* ppv = nullptr;
    EXPECT_EQ(CcwQueryInterface(guard.self(), nonmatching_iid, &ppv),
              kE_NOINTERFACE);
    EXPECT_EQ(ppv, nullptr);
}

TEST_F(ComCcwTest, QueryInterfaceRegisteredInterface) {
    TestCcwGuard guard;
    void* s = guard.self();
    const CHAOS_IL2CPP_UINT8 test_guid[16] = {
        0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C
    };
    void* fake_vtbl = reinterpret_cast<void*>(static_cast<uintptr_t>(0x4000));
    ASSERT_TRUE(RegisterCcwInterface(s, test_guid, fake_vtbl));

    void* ppv = nullptr;
    auto hr = CcwQueryInterface(s, test_guid, &ppv);
    EXPECT_EQ(hr, kS_OK);
    EXPECT_NE(ppv, nullptr);

    // QI increments refcount from 1 to 2 — release back to 1.
    CcwRelease(s);
}

TEST_F(ComCcwTest, QueryInterfaceNoMatchNonZeroGuid) {
    TestCcwGuard guard;
    const CHAOS_IL2CPP_UINT8 unknown_guid[16] = {
        0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88,
        0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00
    };
    void* ppv = nullptr;
    EXPECT_EQ(CcwQueryInterface(guard.self(), unknown_guid, &ppv),
              kE_NOINTERFACE);
    EXPECT_EQ(ppv, nullptr);
}

// ── CcwAddRef ───────────────────────────────────────────────────────────────

TEST_F(ComCcwTest, AddRefNull) {
    EXPECT_EQ(CcwAddRef(nullptr), 0u);
}

TEST_F(ComCcwTest, AddRefReleaseCycle) {
    TestCcwGuard guard;
    void* s = guard.self();
    // Start at refcount = 1 (to prevent free on reaching 0)
    EXPECT_EQ(CcwAddRef(s), 2u);
    EXPECT_EQ(CcwAddRef(s), 3u);
    EXPECT_EQ(CcwRelease(s), 2u);
    EXPECT_EQ(CcwAddRef(s), 3u);
    EXPECT_EQ(CcwRelease(s), 2u);
    EXPECT_EQ(CcwRelease(s), 1u);
}

TEST_F(ComCcwTest, AddRefValid) {
    TestCcwGuard guard;
    EXPECT_EQ(CcwAddRef(guard.self()), 2u);
}

// ── CcwRelease ──────────────────────────────────────────────────────────────

TEST_F(ComCcwTest, ReleaseNull) {
    EXPECT_EQ(CcwRelease(nullptr), 0u);
}

// ── CcwFromInterface ────────────────────────────────────────────────────────

TEST_F(ComCcwTest, FromInterfaceNull) {
    EXPECT_EQ(CcwFromInterface(nullptr), nullptr);
}

// ── RegisterCcwInterface ────────────────────────────────────────────────────

TEST_F(ComCcwTest, RegisterInterfaceNullCcw) {
    const CHAOS_IL2CPP_UINT8 guid[16] = {0};
    void* vtable = reinterpret_cast<void*>(static_cast<uintptr_t>(0x1000));
    EXPECT_FALSE(RegisterCcwInterface(nullptr, guid, vtable));
}

TEST_F(ComCcwTest, RegisterInterfaceNullGuid) {
    TestCcwGuard guard;
    EXPECT_FALSE(RegisterCcwInterface(guard.self(), nullptr,
        reinterpret_cast<void*>(0x1000)));
}

TEST_F(ComCcwTest, RegisterInterfaceNullVtable) {
    TestCcwGuard guard;
    const CHAOS_IL2CPP_UINT8 guid[16] = {0};
    EXPECT_FALSE(RegisterCcwInterface(guard.self(), guid, nullptr));
}

TEST_F(ComCcwTest, RegisterInterfaceValid) {
    TestCcwGuard guard;
    void* s = guard.self();
    const CHAOS_IL2CPP_UINT8 guid[16] = {1, 2, 3, 4};
    void* vtable = reinterpret_cast<void*>(static_cast<uintptr_t>(0x2000));

    EXPECT_TRUE(RegisterCcwInterface(s, guid, vtable));

    // Verify via QI.
    void* ppv = nullptr;
    EXPECT_EQ(CcwQueryInterface(s, guid, &ppv), kS_OK);
    EXPECT_NE(ppv, nullptr);
    CcwRelease(s);
}

TEST_F(ComCcwTest, RegisterInterfaceFull) {
    // kMaxCcwInterfaces = 256 (post-industrialization). Slot 0 = IUnknown,
    // so 3 more can be registered (fits in 4-slot inline_interfaces).
    // The full-capacity path is tested via dynamic allocation in integration.
    TestCcwGuard guard;
    void* s = guard.self();

    const CHAOS_IL2CPP_UINT8 guids[3][16] = {
        {1}, {2}, {3}
    };
    void* fake_vtbl = reinterpret_cast<void*>(0x3000);
    for (uint32_t i = 0; i < 3; ++i) {
        EXPECT_TRUE(RegisterCcwInterface(s, guids[i], fake_vtbl));
    }

    // Slots 1-3 still reachable via QI.
    for (uint32_t i = 0; i < 3; ++i) {
        void* ppv = nullptr;
        EXPECT_EQ(CcwQueryInterface(s, guids[i], &ppv), kS_OK);
        EXPECT_NE(ppv, nullptr);
        CcwRelease(s);
    }
}

TEST_F(ComCcwTest, RegisterInterfaceMaxThenQI) {
    // Register 3 interfaces, QI for each, then QI for unregistered IID.
    TestCcwGuard guard;
    void* s = guard.self();

    const CHAOS_IL2CPP_UINT8 guids[3][16] = {
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    };
    for (uint32_t i = 0; i < 3; ++i) {
        EXPECT_TRUE(RegisterCcwInterface(s, guids[i],
            reinterpret_cast<void*>(static_cast<uintptr_t>(0x3000 + i))));
    }

    for (uint32_t i = 0; i < 3; ++i) {
        void* ppv = nullptr;
        EXPECT_EQ(CcwQueryInterface(s, guids[i], &ppv), kS_OK);
        EXPECT_NE(ppv, nullptr);
        CcwRelease(s);
    }

    const CHAOS_IL2CPP_UINT8 unknown[16] = {
        0xFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    void* ppv = nullptr;
    EXPECT_EQ(CcwQueryInterface(s, unknown, &ppv), kE_NOINTERFACE);
}

// ── CreateCcw ───────────────────────────────────────────────────────────────

TEST_F(ComCcwTest, CreateCcwNullObject) {
    EXPECT_EQ(CreateCcw(nullptr, nullptr), 0);
    EXPECT_EQ(CreateCcw(nullptr, reinterpret_cast<void*>(0x1234)), 0);
}

// ── CreateCcwAggregated ─────────────────────────────────────────────────────

TEST_F(ComCcwTest, CreateCcwAggregatedNullObject) {
    EXPECT_EQ(CreateCcwAggregated(nullptr, nullptr, nullptr), 0);
}

TEST_F(ComCcwTest, CreateCcwAggregatedNullOuter) {
    EXPECT_EQ(CreateCcwAggregated(reinterpret_cast<void*>(0x3000),
                                   reinterpret_cast<void*>(0x4000), nullptr), 0);
}

// ── DestroyCcw ──────────────────────────────────────────────────────────────

TEST_F(ComCcwTest, DestroyCcwNull) {
    DestroyCcw(nullptr);
    SUCCEED();
}

// ── Aggregation tests ───────────────────────────────────────────────────────

/// Minimal mock IUnknown for testing aggregation delegation.
struct MockOuterUnknown {
    void* vtable;
    int qi_call_count;
    int addref_call_count;
    int release_call_count;
    CHAOS_IL2CPP_UINT32 refcount;
};

CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL MockOuterQi(
    void* self, const void* /*iid*/, void** ppv) noexcept {
    auto* mock = static_cast<MockOuterUnknown*>(self);
    mock->qi_call_count++;
    *ppv = self;
    mock->refcount++;
    return kS_OK;
}

CHAOS_IL2CPP_UINT32 CHAOS_RUNTIME_ABI_CALL MockOuterAddRef(void* self) noexcept {
    auto* mock = static_cast<MockOuterUnknown*>(self);
    mock->addref_call_count++;
    return ++mock->refcount;
}

CHAOS_IL2CPP_UINT32 CHAOS_RUNTIME_ABI_CALL MockOuterRelease(void* self) noexcept {
    auto* mock = static_cast<MockOuterUnknown*>(self);
    mock->release_call_count++;
    return --mock->refcount;
}

static ComCcwVtbl s_mock_outer_vtbl = {
    &MockOuterQi,
    &MockOuterAddRef,
    &MockOuterRelease,
};

TEST_F(ComCcwTest, AggregatedCcwDelegatesQueryInterface) {
    MockOuterUnknown outer{};
    outer.vtable = &s_mock_outer_vtbl;
    outer.refcount = 1;

    TestCcwGuard guard;
    guard.ccw.outer_unknown = &outer;
    guard.ccw.is_aggregated = true;
    void* s = guard.self();

    void* ppv = nullptr;
    const CHAOS_IL2CPP_UINT8 some_iid[16] = {1};
    auto hr = CcwQueryInterface(s, some_iid, &ppv);
    EXPECT_EQ(hr, kS_OK);
    EXPECT_EQ(ppv, &outer);
    EXPECT_GE(outer.qi_call_count, 1);
}

TEST_F(ComCcwTest, AggregatedCcwDelegatesAddRef) {
    MockOuterUnknown outer{};
    outer.vtable = &s_mock_outer_vtbl;
    outer.refcount = 1;

    TestCcwGuard guard;
    guard.ccw.outer_unknown = &outer;
    guard.ccw.is_aggregated = true;

    EXPECT_EQ(CcwAddRef(guard.self()), 2u);
    EXPECT_EQ(outer.addref_call_count, 1);
}

TEST_F(ComCcwTest, AggregatedCcwDelegatesRelease) {
    MockOuterUnknown outer{};
    outer.vtable = &s_mock_outer_vtbl;
    outer.refcount = 2;

    TestCcwGuard guard;
    guard.ccw.outer_unknown = &outer;
    guard.ccw.is_aggregated = true;

    EXPECT_EQ(CcwRelease(guard.self()), 1u);
    EXPECT_EQ(outer.release_call_count, 1);
}

TEST_F(ComCcwTest, AggregatedCcwIUnknownIdentityReturnsOuter) {
    MockOuterUnknown outer{};
    outer.vtable = &s_mock_outer_vtbl;
    outer.refcount = 1;

    TestCcwGuard guard;
    guard.ccw.outer_unknown = &outer;
    guard.ccw.is_aggregated = true;
    void* s = guard.self();

    void* ppv = nullptr;
    const CHAOS_IL2CPP_UINT8 iunknown_iid[16] = {0};
    auto hr = CcwQueryInterface(s, iunknown_iid, &ppv);
    EXPECT_EQ(hr, kS_OK);
    EXPECT_EQ(ppv, &outer);
}

TEST_F(ComCcwTest, NonAggregatedCcwDoesNotDelegateAddRef) {
    TestCcwGuard guard;
    guard.ccw.is_aggregated = false;
    guard.ccw.outer_unknown = nullptr;

    EXPECT_EQ(CcwAddRef(guard.self()), 2u);
}

// ── CcwGetTypeInfoCount ─────────────────────────────────────────────────────

TEST_F(ComCcwTest, GetTypeInfoCountNullPctinfo) {
    EXPECT_EQ(CcwGetTypeInfoCount(reinterpret_cast<void*>(0x1234), nullptr),
              kE_POINTER);
}

// ── CcwGetTypeInfo ──────────────────────────────────────────────────────────

TEST_F(ComCcwTest, GetTypeInfoNullPpTInfo) {
    EXPECT_EQ(CcwGetTypeInfo(reinterpret_cast<void*>(0x1234), 0, 0, nullptr),
              kE_POINTER);
}
