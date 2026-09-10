// marshal_string_test.cpp — String marshalling tests for P/Invoke & Marshal.
//
// Tests MarshalStringToWide, MarshalStringToCoTaskMemUtf8,
// MarshalStringToCoTaskMemWide, MarshalStringToHGlobalAnsi,
// MarshalStringToHGlobalUni, MarshalFreeBSTR.
//
// These functions read from a managed String object but only produce native
// buffers (CoTaskMem, HGlobal) — no managed string creation. The managed
// String input is mocked via a manually constructed StringObjectHeader
// (matching the layout in header_layouts.cpp).
//
// Functions that create managed strings (MarshalPtrToStringUtf8, etc.)
// require a functional GC heap and are tested separately.

#include "marshal_test_fixture.h"

#include <engine_binding.h>

#include <cstdint>
#include <cstring>

using namespace chaos::il2cpp::runtime_core;

// ── Mock StringObjectHeader (matches header_layouts.cpp layout) ───────────
//
// struct StringObjectHeader {
//     TypeInfoHandle type;       // 8 bytes
//     CHAOS_IL2CPP_UINTPTR byte_count;  // 8 bytes
// };
// UTF-8 data immediately follows the header at offset 16.

struct MockStringHeader {
    const void* type;
    uintptr_t   byte_count;
};

/// Build a mock managed string in a malloc'd buffer.
/// Returns a pointer that can be passed as void* managed_string.
static void* CreateMockString(const char* utf8, uintptr_t byte_count) {
    // Allocate header + data + NUL
    auto* buf = static_cast<char*>(std::malloc(sizeof(MockStringHeader) + byte_count + 1));
    auto* hdr = reinterpret_cast<MockStringHeader*>(buf);
    hdr->type = nullptr;
    hdr->byte_count = byte_count;
    if (byte_count > 0) {
        std::memcpy(buf + sizeof(MockStringHeader), utf8, byte_count);
    }
    buf[sizeof(MockStringHeader) + byte_count] = '\0';
    return hdr;
}

// ════════════════════════════════════════════════════════════════════════════
// B2 — String marshalling tests
// ════════════════════════════════════════════════════════════════════════════

class MarshalStringTest : public MarshalTestFixture {
};

TEST_F(MarshalStringTest, StringToWide_NullString) {
    EXPECT_EQ(MarshalStringToWide(nullptr, nullptr, 0), 0);
}

TEST_F(MarshalStringTest, StringToWide_EmptyString) {
    auto* s = CreateMockString("", 0);
    EXPECT_EQ(MarshalStringToWide(s, nullptr, 0), 0);
    std::free(s);
}

TEST_F(MarshalStringTest, StringToWide_Ascii) {
    auto* s = CreateMockString("Hello", 5);
    int needed = MarshalStringToWide(s, nullptr, 0);
    ASSERT_GT(needed, 0);

    CHAOS_IL2CPP_UINT16 buf[16];
    int written = MarshalStringToWide(s, buf, 16);
    EXPECT_EQ(written, 5);
    EXPECT_EQ(buf[0], 'H');
    EXPECT_EQ(buf[1], 'e');
    EXPECT_EQ(buf[2], 'l');
    EXPECT_EQ(buf[3], 'l');
    EXPECT_EQ(buf[4], 'o');
    std::free(s);
}

TEST_F(MarshalStringTest, StringToWide_Utf8) {
    // "Hëllö" — UTF-8: H(1) ë(2) l(1) l(1) ö(2) = 7 bytes, 5 code points
    const char utf8[] = "H\xC3\xABll\xC3\xB6";
    auto* s = CreateMockString(utf8, 7);
    int needed = MarshalStringToWide(s, nullptr, 0);
    ASSERT_GT(needed, 0);

    CHAOS_IL2CPP_UINT16 buf[16];
    int written = MarshalStringToWide(s, buf, 16);
    EXPECT_EQ(written, 5);
    EXPECT_EQ(buf[0], 'H');
    EXPECT_EQ(buf[1], 0x00EB);  // ë
    EXPECT_EQ(buf[2], 'l');
    EXPECT_EQ(buf[3], 'l');
    EXPECT_EQ(buf[4], 0x00F6);  // ö
    std::free(s);
}

TEST_F(MarshalStringTest, StringToCoTaskMemUtf8_NullAndEmpty) {
    auto* rs = GetRuntimeState();
    auto* ts = GetThreadState();
    ASSERT_NE(rs, nullptr);
    ASSERT_NE(ts, nullptr);

    // Null managed string → 0
    EXPECT_EQ(MarshalStringToCoTaskMemUtf8(rs, ts, nullptr), 0);

    // Empty string → non-zero allocation
    auto* empty = CreateMockString("", 0);
    auto mem = MarshalStringToCoTaskMemUtf8(rs, ts, empty);
    EXPECT_NE(mem, 0);
    EXPECT_TRUE(MarshalFreeCoTaskMem(rs, mem));
    std::free(empty);
}

TEST_F(MarshalStringTest, StringToCoTaskMemUtf8_Roundtrip) {
    auto* rs = GetRuntimeState();
    auto* ts = GetThreadState();
    ASSERT_NE(rs, nullptr);
    ASSERT_NE(ts, nullptr);

    const char expected[] = "hello marshal";
    auto* s = CreateMockString(expected, 14);

    auto mem = MarshalStringToCoTaskMemUtf8(rs, ts, s);
    ASSERT_NE(mem, 0);

    // The returned pointer IS the data address (MarshalAllocationHeader
    // precedes it). Read back and compare.
    auto* actual = reinterpret_cast<const char*>(mem);
    EXPECT_EQ(std::memcmp(actual, expected, 14), 0);
    EXPECT_EQ(actual[14], '\0');  // null-terminated

    EXPECT_TRUE(MarshalFreeCoTaskMem(rs, mem));
    std::free(s);
}

TEST_F(MarshalStringTest, StringToCoTaskMemWide_Roundtrip) {
    auto* rs = GetRuntimeState();
    auto* ts = GetThreadState();
    ASSERT_NE(rs, nullptr);
    ASSERT_NE(ts, nullptr);

    auto* s = CreateMockString("ABC", 3);
    auto mem = MarshalStringToCoTaskMemWide(rs, ts, s);
    ASSERT_NE(mem, 0);

    // Data is UTF-16 chars at the returned pointer
    const auto* wide = reinterpret_cast<const CHAOS_IL2CPP_UINT16*>(mem);
    EXPECT_EQ(wide[0], 'A');
    EXPECT_EQ(wide[1], 'B');
    EXPECT_EQ(wide[2], 'C');
    EXPECT_EQ(wide[3], 0);  // null-terminated

    EXPECT_TRUE(MarshalFreeCoTaskMem(rs, mem));
    std::free(s);
}

TEST_F(MarshalStringTest, StringToHGlobalAnsi_Roundtrip) {
    auto* rs = GetRuntimeState();
    auto* ts = GetThreadState();
    ASSERT_NE(rs, nullptr);
    ASSERT_NE(ts, nullptr);

    auto* s = CreateMockString("ansi-test", 9);
    auto mem = MarshalStringToHGlobalAnsi(rs, ts, s);
    ASSERT_NE(mem, 0);

    // ASCII ANSI should match input on Western locales
    const auto* actual = reinterpret_cast<const char*>(mem);
    EXPECT_EQ(std::memcmp(actual, "ansi-test", 9), 0);
    EXPECT_EQ(actual[9], '\0');

    EXPECT_TRUE(MarshalFreeHGlobal(rs, mem));
    std::free(s);
}

TEST_F(MarshalStringTest, StringToHGlobalUni_Roundtrip) {
    auto* rs = GetRuntimeState();
    auto* ts = GetThreadState();
    ASSERT_NE(rs, nullptr);
    ASSERT_NE(ts, nullptr);

    auto* s = CreateMockString("uni-test", 8);
    auto mem = MarshalStringToHGlobalUni(rs, ts, s);
    ASSERT_NE(mem, 0);

    const auto* wide = reinterpret_cast<const CHAOS_IL2CPP_UINT16*>(mem);
    EXPECT_EQ(wide[0], 'u');
    EXPECT_EQ(wide[1], 'n');
    EXPECT_EQ(wide[2], 'i');
    EXPECT_EQ(wide[3], '-');
    EXPECT_EQ(wide[4], 't');
    EXPECT_EQ(wide[5], 'e');
    EXPECT_EQ(wide[6], 's');
    EXPECT_EQ(wide[7], 't');
    EXPECT_EQ(wide[8], 0);

    EXPECT_TRUE(MarshalFreeHGlobal(rs, mem));
    std::free(s);
}

TEST_F(MarshalStringTest, MarshalFreeBSTR_Null) {
    // FreeBSTR with null should be a no-op
    MarshalFreeBSTR(0);
    SUCCEED();
}

TEST_F(MarshalStringTest, StringToCoTaskMemUtf8_BufferCapcity) {
    auto* rs = GetRuntimeState();
    auto* ts = GetThreadState();
    ASSERT_NE(rs, nullptr);
    ASSERT_NE(ts, nullptr);

    // Mock a larger string
    auto* s = CreateMockString("ABCDEFGHIJ", 10);
    auto mem = MarshalStringToCoTaskMemUtf8(rs, ts, s);
    ASSERT_NE(mem, 0);

    const auto* actual = reinterpret_cast<const char*>(mem);
    EXPECT_EQ(std::memcmp(actual, "ABCDEFGHIJ", 10), 0);
    EXPECT_EQ(actual[10], '\0');

    EXPECT_TRUE(MarshalFreeCoTaskMem(rs, mem));
    std::free(s);
}
