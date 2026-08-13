// com_rcw_test.cpp — RCW creation, caching, release, and QI tests.
//
// Tests the chaos::il2cpp::com_rcw namespace functions:
//   IsComRcwHandle, FindOrCreateRcw, ReleaseRcw, QueryInterfaceCached.
//
// On non-Win32 platforms, the COM ABI functions (AddRef/Release/QI) are
// no-ops, so these tests verify null-safety and structural correctness.
// On Win32, real COM calls are made but require valid COM pointers.

#include "marshal_test_fixture.h"

#include <com_rcw.h>

#include <cstring>

using namespace chaos::il2cpp::com_rcw;

// ════════════════════════════════════════════════════════════════════════════
// F1 — COM RCW tests
// ════════════════════════════════════════════════════════════════════════════

class ComRcwTest : public MarshalTestFixture {
protected:
    // Helper: create a minimal ComRcwNative buffer for IsComRcwHandle tests.
    // Does NOT go through FindOrCreateRcw — just raw memory with magic.
    static ComRcwNative* CreateMinimalRcwBuffer() {
        auto* buf = static_cast<ComRcwNative*>(std::malloc(sizeof(ComRcwNative)));
        if (buf != nullptr) {
            std::memset(buf, 0, sizeof(ComRcwNative));
            buf->magic = kComRcwMagic;
        }
        return buf;
    }
};

// ════════════════════════════════════════════════════════════════════════════
// Fake COM IUnknown — lets the Win32 PalComAddRef/Release/QueryInterface (which
// deref `unknown` as a vtable: [0]=QI, [1]=AddRef, [2]=Release) run in unit
// tests WITHOUT a registered real COM class.  A self-contained 3-slot vtable of
// stubs over a real refcount int; passing THIS as the RCW's IUnknown exercises
// the real Win32 COM ABI path instead of the previous GTEST_SKIP on _WIN32.
// ════════════════════════════════════════════════════════════════════════════
struct FakeIUnknown {
    const void* const* vtbl;   // IUnknown layout: vtbl pointer first
    int32_t refcount;
};
int32_t FakeIUnknownQueryInterface(void*, const void*, void**) { return /*E_NOINTERFACE*/ 0x80004002; }
uint32_t FakeIUnknownAddRef(void* self) noexcept { return static_cast<uint32_t>(++static_cast<FakeIUnknown*>(self)->refcount); }
uint32_t FakeIUnknownRelease(void* self) noexcept {
    int32_t r = --static_cast<FakeIUnknown*>(self)->refcount;
    return r <= 0 ? 0u : static_cast<uint32_t>(r);
}
namespace {
inline const void* const kFakeIUnknownVtbl[3] = {
    reinterpret_cast<const void*>(&FakeIUnknownQueryInterface),
    reinterpret_cast<const void*>(&FakeIUnknownAddRef),
    reinterpret_cast<const void*>(&FakeIUnknownRelease),
};
inline FakeIUnknown MakeFakeIUnknown() { return FakeIUnknown{kFakeIUnknownVtbl, 1}; }
}

TEST_F(ComRcwTest, IsComRcwHandleNull) {
    EXPECT_FALSE(IsComRcwHandle(0));
}

TEST_F(ComRcwTest, IsComRcwHandleValidMagic) {
    auto* rcw = CreateMinimalRcwBuffer();
    ASSERT_NE(rcw, nullptr);

    EXPECT_TRUE(IsComRcwHandle(
        static_cast<CHAOS_IL2CPP_INTPTR>(
            reinterpret_cast<uintptr_t>(rcw))));

    std::free(rcw);
}

TEST_F(ComRcwTest, IsComRcwHandleWrongMagic) {
    auto* rcw = CreateMinimalRcwBuffer();
    ASSERT_NE(rcw, nullptr);
    rcw->magic = 0xDEADBEEFu;  // wrong magic

    EXPECT_FALSE(IsComRcwHandle(
        static_cast<CHAOS_IL2CPP_INTPTR>(
            reinterpret_cast<uintptr_t>(rcw))));

    std::free(rcw);
}

TEST_F(ComRcwTest, FindOrCreateRcwNull) {
    EXPECT_EQ(FindOrCreateRcw(nullptr), nullptr);
}

TEST_F(ComRcwTest, FindOrCreateRcwValid) {
    // A real (fake-but-valid) IUnknown: FindOrCreateRcw calls AddRef on the
    // COM pointer on ALL platforms via PalComAddRef, which needs a dereferable
    // vtable — FakeIUnknown provides one (previously skipped on Win32).
    FakeIUnknown fake = MakeFakeIUnknown();
    auto* rcw = FindOrCreateRcw(&fake);
    ASSERT_NE(rcw, nullptr);
    EXPECT_EQ(rcw->magic, kComRcwMagic);
    EXPECT_EQ(rcw->identity_unknown, static_cast<void*>(&fake));
    EXPECT_EQ(rcw->wrapper_refcount, 1u);
#if defined(_WIN32)
    // Only Win32 PalComAddRef actually increments the COM object's refcount
    // (vtable call); posix is a no-op, so assert only where it is real.
    EXPECT_EQ(fake.refcount, 2);  // initial 1 + PalComAddRef
#endif

    // Cleanup: set refcount to 1 and release (Release calls PalComRelease).
    rcw->wrapper_refcount = 1;
    ReleaseRcw(rcw);
    SUCCEED();
}

TEST_F(ComRcwTest, ReleaseRcwNull) {
    // Should be a no-op, no crash
    ReleaseRcw(nullptr);
    SUCCEED();
}

TEST_F(ComRcwTest, ReleaseRcwRefcountNotZero) {
    FakeIUnknown fake = MakeFakeIUnknown();
    auto* rcw = FindOrCreateRcw(&fake);
    ASSERT_NE(rcw, nullptr);

    // Calling ReleaseRcw on wrapper_refcount=2 should decrement, not free.
    rcw->wrapper_refcount = 2;
    ReleaseRcw(rcw);  // 2 -> 1, not freed (RCW still in cache table)
    SUCCEED();

    // Release the second ref: 1 -> 0, freed and erased from table.
    rcw->wrapper_refcount = 1;
    ReleaseRcw(rcw);  // 1 -> 0, freed
    SUCCEED();
}

TEST_F(ComRcwTest, QueryInterfaceCachedNullRcw) {
    const CHAOS_IL2CPP_UINT8 iid[16] = {0};
    EXPECT_EQ(QueryInterfaceCached(nullptr, iid), nullptr);
}

TEST_F(ComRcwTest, QueryInterfaceCachedNullIid) {
    auto* rcw = CreateMinimalRcwBuffer();
    ASSERT_NE(rcw, nullptr);

    EXPECT_EQ(QueryInterfaceCached(rcw, nullptr), nullptr);

    std::free(rcw);
}

TEST_F(ComRcwTest, QueryInterfaceCachedEmptyCache) {
    // RCW with cache_count=0 should fall through to QueryInterface on the
    // identity IUnknown.  FakeIUnknown gives that vtable a dereferable QI stub
    // (no cache hit → returns the stub QI result, nullptr for an E_NOINTERFACE).
    FakeIUnknown fake = MakeFakeIUnknown();
    auto* rcw = CreateMinimalRcwBuffer();
    ASSERT_NE(rcw, nullptr);
    rcw->cache_count = 0;
    rcw->identity_unknown = &fake;

    const CHAOS_IL2CPP_UINT8 iid[16] = {0};
    EXPECT_EQ(QueryInterfaceCached(rcw, iid), nullptr);

    std::free(rcw);
}

TEST_F(ComRcwTest, IsComRcwHandleAfterFindOrCreate) {
    // A real RCW from FindOrCreateRcw must pass IsComRcwHandle.
    FakeIUnknown fake = MakeFakeIUnknown();
    auto* rcw = FindOrCreateRcw(&fake);
    ASSERT_NE(rcw, nullptr);

    EXPECT_TRUE(IsComRcwHandle(
        static_cast<CHAOS_IL2CPP_INTPTR>(
            reinterpret_cast<uintptr_t>(rcw))));

    ReleaseRcw(rcw);  // refcount was 1, now 0 — freed
    SUCCEED();
}

TEST_F(ComRcwTest, FindOrCreateRcwReturnsSameRcw) {
    // FindOrCreateRcw for the same IUnknown pointer should return the
    // same RCW (cached in global table) and increment refcount.
    FakeIUnknown fake = MakeFakeIUnknown();
    // outer fake owns the low refcount the table keys on; AddRef bumps it.
    auto* rcw1 = FindOrCreateRcw(&fake);
    ASSERT_NE(rcw1, nullptr);
    EXPECT_EQ(rcw1->wrapper_refcount, 1u);

    auto* rcw2 = FindOrCreateRcw(&fake);
    ASSERT_NE(rcw2, nullptr);
    EXPECT_EQ(rcw1, rcw2);             // same pointer
    EXPECT_EQ(rcw2->wrapper_refcount, 2u);  // incremented
#if defined(_WIN32)
    // Only the FIRST FindOrCreate (cache miss) calls PalComAddRef on the COM
    // object (posix is a no-op); cache hits bump the RCW count but not the COM
    // refcount, so Win32 sees one AddRef → 1 + 1 = 2.
    EXPECT_EQ(fake.refcount, 2);       // one PalComAddRef
#endif

    // Release both refs
    rcw2->wrapper_refcount = 2;
    ReleaseRcw(rcw2);  // 2 -> 1
    rcw2->wrapper_refcount = 1;
    ReleaseRcw(rcw2);  // 1 -> 0, freed
    SUCCEED();
}

TEST_F(ComRcwTest, ReleaseRcwRefcountAlreadyZero) {
    // ReleaseRcw with wrapper_refcount already 0 should be a no-op
    // (wrapper_refcount underflow protection in ReleaseRcw).
    auto* rcw = CreateMinimalRcwBuffer();
    ASSERT_NE(rcw, nullptr);
    rcw->wrapper_refcount = 0;

    // Should not crash — wrapper_refcount was already 0, so the
    // decrement check (--wrapper_refcount > 0) should short-circuit.
    ReleaseRcw(rcw);

    std::free(rcw);
}

TEST_F(ComRcwTest, IsComRcwHandleRandomPointer) {
    // A stack variable with wrong magic should be detected.
    uint32_t val = 0xDEADBEEFu;
    EXPECT_FALSE(IsComRcwHandle(
        static_cast<CHAOS_IL2CPP_INTPTR>(reinterpret_cast<uintptr_t>(&val))));
}

// ── QueryInterfaceCached cache hit / full-cache paths ────────────────────

TEST_F(ComRcwTest, QueryInterfaceCachedCacheFullNoAdd) {
    // Use FindOrCreateRcw to get a properly allocated RCW, then manually
    // fill the cache to capacity.  FakeIUnknown gives release a valid vtable.
    FakeIUnknown fake = MakeFakeIUnknown();
    auto* rcw = FindOrCreateRcw(&fake);
    ASSERT_NE(rcw, nullptr);

    // Fill all kMaxInterfaceCache entries with distinct IID data.
    // Each entry points to a valid FakeIUnknown so ReleaseRcw's per-entry
    // PalComRelease is safe (a bare integer 0x7000+i would AV on Win32).
    CHAOS_IL2CPP_UINT8 iid_data[kMaxInterfaceCache][16];
    std::memset(iid_data, 0, sizeof(iid_data));
    for (CHAOS_IL2CPP_SIZE i = 0; i < kMaxInterfaceCache; ++i) {
        iid_data[i][0] = static_cast<CHAOS_IL2CPP_UINT8>(i + 1);
        rcw->interface_cache[i].iid = iid_data[i];
        rcw->interface_cache[i].interface_ptr = &fake;   // valid vtable for Release
        rcw->interface_cache[i].refcount = 1;
    }
    rcw->cache_count = kMaxInterfaceCache;

    // Non-matching IID — cache is full, so returns nullptr (no QI fall-through).
    const CHAOS_IL2CPP_UINT8 query_iid[16] = {0xFF, 0, 0, 0};
    void* result = QueryInterfaceCached(rcw, query_iid);
    EXPECT_EQ(result, nullptr);

    // Verify no entry was overwritten.
    for (CHAOS_IL2CPP_SIZE i = 0; i < kMaxInterfaceCache; ++i) {
        EXPECT_EQ(rcw->interface_cache[i].refcount, 1u);
    }

    rcw->wrapper_refcount = 1;
    ReleaseRcw(rcw);
    SUCCEED();
}

TEST_F(ComRcwTest, QueryInterfaceCachedCacheHit) {
    // Populate cache manually, verify cache hit returns the pointer.
    auto* rcw = CreateMinimalRcwBuffer();
    ASSERT_NE(rcw, nullptr);

    const CHAOS_IL2CPP_UINT8 iid0[16] = {1, 0, 0, 0};
    void* fake_iface = reinterpret_cast<void*>(static_cast<uintptr_t>(0x7000));

    rcw->cache_count = 1;
    rcw->interface_cache[0].iid = iid0;
    rcw->interface_cache[0].interface_ptr = fake_iface;
    rcw->interface_cache[0].refcount = 1;

    void* result = QueryInterfaceCached(rcw, iid0);
    EXPECT_EQ(result, fake_iface);
    EXPECT_EQ(rcw->interface_cache[0].refcount, 2u);

    std::free(rcw);
}

TEST_F(ComRcwTest, QueryInterfaceCachedCacheHitSecondEntry) {
    // Verify cache hit works for the second cache entry.
    auto* rcw = CreateMinimalRcwBuffer();
    ASSERT_NE(rcw, nullptr);

    const CHAOS_IL2CPP_UINT8 iid0[16] = {1, 0, 0, 0};
    const CHAOS_IL2CPP_UINT8 iid1[16] = {2, 0, 0, 0};
    void* fake_iface0 = reinterpret_cast<void*>(static_cast<uintptr_t>(0x7000));
    void* fake_iface1 = reinterpret_cast<void*>(static_cast<uintptr_t>(0x8000));

    rcw->cache_count = 2;
    rcw->interface_cache[0].iid = iid0;
    rcw->interface_cache[0].interface_ptr = fake_iface0;
    rcw->interface_cache[0].refcount = 1;
    rcw->interface_cache[1].iid = iid1;
    rcw->interface_cache[1].interface_ptr = fake_iface1;
    rcw->interface_cache[1].refcount = 1;

    void* result = QueryInterfaceCached(rcw, iid1);
    EXPECT_EQ(result, fake_iface1);
    EXPECT_EQ(rcw->interface_cache[1].refcount, 2u);

    std::free(rcw);
}
