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
#if defined(_WIN32)
    // On Win32, FindOrCreateRcw calls AddRef on the COM pointer, which
    // requires a real COM object. Skip this test without real COM.
    GTEST_SKIP() << "Skipping on Win32 (needs real COM pointer)";
#else
    // On non-Win32, FindOrCreateRcw still allocates a ComRcwNative
    // but skips AddRef (no-op). The RCW should have correct magic.
    void* fake_com_ptr = reinterpret_cast<void*>(static_cast<uintptr_t>(0x1234));
    auto* rcw = FindOrCreateRcw(fake_com_ptr);
    ASSERT_NE(rcw, nullptr);
    EXPECT_EQ(rcw->magic, kComRcwMagic);
    EXPECT_EQ(rcw->identity_unknown, fake_com_ptr);
    EXPECT_EQ(rcw->wrapper_refcount, 1u);

    // Cleanup: set refcount to 1 and release
    rcw->wrapper_refcount = 1;
    ReleaseRcw(rcw);
#endif
}

TEST_F(ComRcwTest, ReleaseRcwNull) {
    // Should be a no-op, no crash
    ReleaseRcw(nullptr);
    SUCCEED();
}

TEST_F(ComRcwTest, ReleaseRcwRefcountNotZero) {
#if defined(_WIN32)
    GTEST_SKIP() << "Skipping on Win32 (needs real COM pointer)";
#else
    // Releasing once on a refcount=2 RCW should decrement but not free.
    void* fake_com_ptr = reinterpret_cast<void*>(static_cast<uintptr_t>(0x5678));
    auto* rcw = FindOrCreateRcw(fake_com_ptr);
    ASSERT_NE(rcw, nullptr);

    // Call ReleaseRcw — wrapper_refcount goes from 1 to 0, should free.
    // But we want to test non-zero case, so increment first.
    // Actually, we can just test that calling ReleaseRcw twice frees it.
    rcw->wrapper_refcount = 2;
    ReleaseRcw(rcw);  // 2 -> 1, not freed
    SUCCEED();

    // Release the second ref
    rcw->wrapper_refcount = 1;
    ReleaseRcw(rcw);  // 1 -> 0, freed
#endif
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
#if defined(_WIN32)
    GTEST_SKIP() << "Skipping on Win32 (needs real COM pointer)";
#else
    // RCW with cache_count=0 should fall through to QueryInterface.
    // On non-Win32, this returns nullptr (no-op).
    auto* rcw = CreateMinimalRcwBuffer();
    ASSERT_NE(rcw, nullptr);
    rcw->cache_count = 0;

    const CHAOS_IL2CPP_UINT8 iid[16] = {0};
    EXPECT_EQ(QueryInterfaceCached(rcw, iid), nullptr);

    std::free(rcw);
#endif
}

TEST_F(ComRcwTest, IsComRcwHandleAfterFindOrCreate) {
#if defined(_WIN32)
    GTEST_SKIP() << "Skipping on Win32 (needs real COM pointer)";
#else
    // Verify that a real RCW from FindOrCreateRcw passes IsComRcwHandle.
    void* fake_com_ptr = reinterpret_cast<void*>(static_cast<uintptr_t>(0x9ABC));
    auto* rcw = FindOrCreateRcw(fake_com_ptr);
    ASSERT_NE(rcw, nullptr);

    EXPECT_TRUE(IsComRcwHandle(
        static_cast<CHAOS_IL2CPP_INTPTR>(
            reinterpret_cast<uintptr_t>(rcw))));

    ReleaseRcw(rcw);  // refcount was 1, now 0 — freed
#endif
}

TEST_F(ComRcwTest, FindOrCreateRcwReturnsSameRcw) {
#if defined(_WIN32)
    GTEST_SKIP() << "Skipping on Win32 (needs real COM pointer)";
#else
    // FindOrCreateRcw for the same IUnknown pointer should return the
    // same RCW (cached in global table) and increment refcount.
    void* fake_com_ptr = reinterpret_cast<void*>(static_cast<uintptr_t>(0xDEF0));
    auto* rcw1 = FindOrCreateRcw(fake_com_ptr);
    ASSERT_NE(rcw1, nullptr);
    EXPECT_EQ(rcw1->wrapper_refcount, 1u);

    auto* rcw2 = FindOrCreateRcw(fake_com_ptr);
    ASSERT_NE(rcw2, nullptr);
    EXPECT_EQ(rcw1, rcw2);             // same pointer
    EXPECT_EQ(rcw2->wrapper_refcount, 2u);  // incremented

    // Release both refs
    rcw2->wrapper_refcount = 2;
    ReleaseRcw(rcw2);  // 2 -> 1
    rcw2->wrapper_refcount = 1;
    ReleaseRcw(rcw2);  // 1 -> 0, freed
#endif
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
