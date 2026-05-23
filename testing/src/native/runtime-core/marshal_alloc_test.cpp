// marshal_alloc_test.cpp — Alloc/free/read/write tests for P/Invoke & Marshal.
//
// Tests MarshalAllocHGlobal, MarshalAllocCoTaskMem, MarshalReAllocHGlobal,
// MarshalReAllocCoTaskMem, MarshalFreeHGlobal, MarshalFreeCoTaskMem,
// MarshalZeroFreeCoTaskMemUtf8, and all MarshalRead/MarshalWrite variants.
//
// Each test includes both correctness assertions and a benchmark anchor.

#include "marshal_test_fixture.h"

#include <engine_binding.h>

#include <cstdint>
#include <cstring>

using namespace chaos::il2cpp::runtime_core;

// ════════════════════════════════════════════════════════════════════════════
// B1 — Alloc/Free/Read/Write tests
// ════════════════════════════════════════════════════════════════════════════

class MarshalAllocTest : public MarshalTestFixture {
};

TEST_F(MarshalAllocTest, AllocHGlobal_VariousSizes) {
    auto* rs = GetRuntimeState();
    ASSERT_NE(rs, nullptr);

    // Zero bytes
    auto p0 = MarshalAllocHGlobal(rs, 0);
    EXPECT_NE(p0, 0);
    MarshalFreeHGlobal(rs, p0);

    // Small
    auto p1 = MarshalAllocHGlobal(rs, 1);
    EXPECT_NE(p1, 0);
    *reinterpret_cast<char*>(p1) = 'x';
    EXPECT_EQ(*reinterpret_cast<char*>(p1), 'x');
    MarshalFreeHGlobal(rs, p1);

    // Medium
    auto p64 = MarshalAllocHGlobal(rs, 64);
    EXPECT_NE(p64, 0);
    std::memset(reinterpret_cast<void*>(p64), 0xAB, 64);
    MarshalFreeHGlobal(rs, p64);

    // Large (1 MB)
    auto pLarge = MarshalAllocHGlobal(rs, 1024 * 1024);
    EXPECT_NE(pLarge, 0);
    MarshalFreeHGlobal(rs, pLarge);

    MARSHEL_BENCHMARK_OP("AllocHGlobal_64", 10000,
        auto p = MarshalAllocHGlobal(rs, 64);
        MarshalFreeHGlobal(rs, p);
    );
}

TEST_F(MarshalAllocTest, AllocCoTaskMem_VariousSizes) {
    auto* rs = GetRuntimeState();
    ASSERT_NE(rs, nullptr);

    // Zero bytes
    auto p0 = MarshalAllocCoTaskMem(rs, 0);
    EXPECT_NE(p0, 0);
    MarshalFreeCoTaskMem(rs, p0);

    // Small
    auto p4 = MarshalAllocCoTaskMem(rs, 4);
    EXPECT_NE(p4, 0);
    *reinterpret_cast<int32_t*>(p4) = 42;
    EXPECT_EQ(*reinterpret_cast<int32_t*>(p4), 42);
    MarshalFreeCoTaskMem(rs, p4);

    // Medium
    auto p256 = MarshalAllocCoTaskMem(rs, 256);
    EXPECT_NE(p256, 0);
    std::memset(reinterpret_cast<void*>(p256), 0xCD, 256);
    MarshalFreeCoTaskMem(rs, p256);

    // Large (1 MB)
    auto pLarge = MarshalAllocCoTaskMem(rs, 1024 * 1024);
    EXPECT_NE(pLarge, 0);
    MarshalFreeCoTaskMem(rs, pLarge);

    // Negative size
    auto pNeg = MarshalAllocCoTaskMem(rs, -1);
    EXPECT_EQ(pNeg, 0);

    MARSHEL_BENCHMARK_OP("AllocCoTaskMem_64", 10000,
        auto p = MarshalAllocCoTaskMem(rs, 64);
        MarshalFreeCoTaskMem(rs, p);
    );
}

TEST_F(MarshalAllocTest, ReAllocHGlobal) {
    auto* rs = GetRuntimeState();
    ASSERT_NE(rs, nullptr);

    // Grow: alloc 64 → realloc to 128
    auto p = MarshalAllocHGlobal(rs, 64);
    ASSERT_NE(p, 0);
    std::memset(reinterpret_cast<void*>(p), 0xAA, 64);

    auto pGrown = MarshalReAllocHGlobal(rs, p, 128);
    EXPECT_NE(pGrown, 0);
    // Verify first 64 bytes preserved
    auto* bytes = reinterpret_cast<const uint8_t*>(pGrown);
    for (int i = 0; i < 64; i++) {
        EXPECT_EQ(bytes[i], 0xAA) << "byte " << i << " not preserved after grow";
    }
    MarshalFreeHGlobal(rs, pGrown);

    // Shrink: alloc 256 → realloc to 32
    auto p2 = MarshalAllocHGlobal(rs, 256);
    ASSERT_NE(p2, 0);
    auto pShrunk = MarshalReAllocHGlobal(rs, p2, 32);
    EXPECT_NE(pShrunk, 0);
    MarshalFreeHGlobal(rs, pShrunk);

    MARSHEL_BENCHMARK_OP("ReAllocHGlobal_64to128", 10000,
        auto p = MarshalAllocHGlobal(rs, 64);
        auto p2 = MarshalReAllocHGlobal(rs, p, 128);
        MarshalFreeHGlobal(rs, p2);
    );
}

TEST_F(MarshalAllocTest, ReAllocCoTaskMem) {
    auto* rs = GetRuntimeState();
    ASSERT_NE(rs, nullptr);

    // Grow
    auto p = MarshalAllocCoTaskMem(rs, 64);
    ASSERT_NE(p, 0);
    std::memset(reinterpret_cast<void*>(p), 0xBB, 64);

    auto pGrown = MarshalReAllocCoTaskMem(rs, p, 128);
    EXPECT_NE(pGrown, 0);
    auto* bytes = reinterpret_cast<const uint8_t*>(pGrown);
    for (int i = 0; i < 64; i++) {
        EXPECT_EQ(bytes[i], 0xBB) << "byte " << i << " not preserved after CoTaskMem grow";
    }
    MarshalFreeCoTaskMem(rs, pGrown);

    // Shrink
    auto p2 = MarshalAllocCoTaskMem(rs, 256);
    ASSERT_NE(p2, 0);
    auto pShrunk = MarshalReAllocCoTaskMem(rs, p2, 32);
    EXPECT_NE(pShrunk, 0);
    MarshalFreeCoTaskMem(rs, pShrunk);

    MARSHEL_BENCHMARK_OP("ReAllocCoTaskMem_64to128", 10000,
        auto p = MarshalAllocCoTaskMem(rs, 64);
        auto p2 = MarshalReAllocCoTaskMem(rs, p, 128);
        MarshalFreeCoTaskMem(rs, p2);
    );
}

TEST_F(MarshalAllocTest, FreeHGlobal_NullSafe) {
    auto* rs = GetRuntimeState();
    ASSERT_NE(rs, nullptr);

    // Free with null memory should return true (no-op)
    EXPECT_TRUE(MarshalFreeHGlobal(rs, 0));
    // Free with null runtime_state returns false
    EXPECT_FALSE(MarshalFreeHGlobal(nullptr, 0));
}

TEST_F(MarshalAllocTest, FreeCoTaskMem_NullSafe) {
    auto* rs = GetRuntimeState();
    ASSERT_NE(rs, nullptr);

    EXPECT_TRUE(MarshalFreeCoTaskMem(rs, 0));
    EXPECT_FALSE(MarshalFreeCoTaskMem(nullptr, 0));
}

TEST_F(MarshalAllocTest, MarshalZeroFreeCoTaskMemUtf8) {
    auto* rs = GetRuntimeState();
    ASSERT_NE(rs, nullptr);

    // Allocate and write a string
    auto p = MarshalAllocCoTaskMem(rs, 32);
    ASSERT_NE(p, 0);
    std::memcpy(reinterpret_cast<void*>(p), "hello marshal", 14);

    // ZeroFree should zero the memory and free it
    EXPECT_TRUE(MarshalZeroFreeCoTaskMemUtf8(rs, p));

    // Null-safe with valid runtime
    EXPECT_TRUE(MarshalZeroFreeCoTaskMemUtf8(rs, 0));
    // Null runtime returns false
    EXPECT_FALSE(MarshalZeroFreeCoTaskMemUtf8(nullptr, 0));
}

TEST_F(MarshalAllocTest, MarshalReadWrite_AllIntTypes) {
    auto* rs = GetRuntimeState();
    ASSERT_NE(rs, nullptr);

    auto buf = MarshalAllocCoTaskMem(rs, 64);
    ASSERT_NE(buf, 0);

    // Byte roundtrip
    MarshalWriteByte(buf, 0, 0xAB);
    EXPECT_EQ(MarshalReadByte(buf, 0), 0xAB);

    // Int16 roundtrip
    MarshalWriteInt16(buf, 2, static_cast<int16_t>(-32000));
    EXPECT_EQ(MarshalReadInt16(buf, 2), -32000);

    // Int32 roundtrip
    MarshalWriteInt32(buf, 4, 123456789);
    EXPECT_EQ(MarshalReadInt32(buf, 4), 123456789);

    // Int64 roundtrip
    MarshalWriteInt64(buf, 8, 0xDEADBEEFCAFEBABELL);
    EXPECT_EQ(MarshalReadInt64(buf, 8), 0xDEADBEEFCAFEBABELL);

    // IntPtr roundtrip
    MarshalWriteIntPtr(buf, 16, buf);
    EXPECT_EQ(MarshalReadIntPtr(buf, 16), buf);

    MarshalFreeCoTaskMem(rs, buf);

    MARSHEL_BENCHMARK_OP("ReadWriteInt32", 100000,
        MarshalWriteInt32(buf, 0, 42);
        MarshalReadInt32(buf, 0);
    );
}
