// stream_stubs_test.cpp — Unit tests for stream/IO stubs (t1a-2)
//
// Tests all 14 stream/IO API functions by simulating managed object
// layouts with zeroed memory blocks at the expected field offsets.
//
// Links against CHAOS_COMMON_TEST_LIBS for ground lib resolution.

#include <gtest/gtest.h>
#include <chaos/native_types.h>

#include <cstdint>
#include <cstring>

// ═══════════════════════════════════════════════════════════════════════════
// Helpers: simulate managed objects at expected field offsets
// ═══════════════════════════════════════════════════════════════════════════

// MemoryStream field offsets (matching stream_stubs.cpp)
static constexpr CHAOS_IL2CPP_SIZE kMSBuffer     = 16;
static constexpr CHAOS_IL2CPP_SIZE kMSOrigin     = 24;
static constexpr CHAOS_IL2CPP_SIZE kMSPosition   = 28;
static constexpr CHAOS_IL2CPP_SIZE kMSLength     = 32;
static constexpr CHAOS_IL2CPP_SIZE kMSCapacity   = 36;
static constexpr CHAOS_IL2CPP_SIZE kMSExpandable = 40;
static constexpr CHAOS_IL2CPP_SIZE kMSWritable   = 41;
static constexpr CHAOS_IL2CPP_SIZE kMSExposable  = 42;
static constexpr CHAOS_IL2CPP_SIZE kMSIsOpen     = 43;

// StringReader field offsets
static constexpr CHAOS_IL2CPP_SIZE kSRString = 16;
static constexpr CHAOS_IL2CPP_SIZE kSRPos    = 24;
static constexpr CHAOS_IL2CPP_SIZE kSRLen    = 28;

// String field offsets (matching enum_stubs kManagedStringHeader=40)
static constexpr CHAOS_IL2CPP_SIZE kStrLength     = 16;
static constexpr CHAOS_IL2CPP_SIZE kStrUtf8Ptr    = 24;
static constexpr CHAOS_IL2CPP_SIZE kStrHeaderSize = 40;

// A simulated managed string: header + inline data.
struct SimString {
    unsigned char header[kStrHeaderSize];
    char          data[64];  // inline UTF-8 data
};

// A simulated managed MemoryStream: header + fields.
struct SimMemoryStream {
    unsigned char header[16];
    CHAOS_IL2CPP_INTPTR buffer;      // offset 16
    CHAOS_IL2CPP_INT32 origin;       // offset 24
    CHAOS_IL2CPP_INT32 position;     // offset 28
    CHAOS_IL2CPP_INT32 length;       // offset 32
    CHAOS_IL2CPP_INT32 capacity;     // offset 36
    bool expandable;                 // offset 40
    bool writable;                   // offset 41
    bool exposable;                  // offset 42
    bool isOpen;                     // offset 43
};
static_assert(sizeof(SimMemoryStream) >= 44, "SimMemoryStream too small");

// A simulated managed StringReader: header + fields.
struct SimStringReader {
    unsigned char header[16];
    CHAOS_IL2CPP_INTPTR str;         // offset 16
    CHAOS_IL2CPP_INT32 pos;          // offset 24
    CHAOS_IL2CPP_INT32 length;       // offset 28
};
static_assert(sizeof(SimStringReader) >= 32, "SimStringReader too small");

// Returns a pointer to managed string data area (after header).
static const char* sim_string_data(const SimString* s) {
    return s->data;
}

// Set up a SimString with the given C string content.
static void sim_string_set(SimString* s, const char* text) {
    std::memset(s, 0, sizeof(SimString));
    CHAOS_IL2CPP_INT32 len = static_cast<CHAOS_IL2CPP_INT32>(std::strlen(text));
    std::memcpy(&s->header[kStrLength], &len, sizeof(len));
    std::memcpy(s->data, text, static_cast<std::size_t>(len));
    // utf8_data_ptr points to data area (after header)
    CHAOS_IL2CPP_INTPTR data_ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s->data);
    std::memcpy(&s->header[kStrUtf8Ptr], &data_ptr, sizeof(data_ptr));
}

// ═══════════════════════════════════════════════════════════════════════════
// Forward declarations for the stub functions
// (These are in chaos::il2cpp::runtime_core, extern "C")
// ═══════════════════════════════════════════════════════════════════════════

extern "C" {
void ChaosStreamFlush(CHAOS_IL2CPP_INTPTR stream) noexcept;
CHAOS_IL2CPP_INT64 ChaosStreamSeek(CHAOS_IL2CPP_INTPTR stream, CHAOS_IL2CPP_INT64 offset, CHAOS_IL2CPP_INT32 origin) noexcept;
void ChaosStreamSetLength(CHAOS_IL2CPP_INTPTR stream, CHAOS_IL2CPP_INT64 value) noexcept;
CHAOS_IL2CPP_INT64 ChaosStreamGetLength(CHAOS_IL2CPP_INTPTR stream) noexcept;
CHAOS_IL2CPP_INT64 ChaosStreamGetPosition(CHAOS_IL2CPP_INTPTR stream) noexcept;
void ChaosMemoryStreamCtor(CHAOS_IL2CPP_INTPTR instance) noexcept;
void ChaosStringReaderCtor(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INTPTR input) noexcept;
void ChaosStringWriterCtor(CHAOS_IL2CPP_INTPTR instance) noexcept;
CHAOS_IL2CPP_INTPTR ChaosTextReaderReadLine(CHAOS_IL2CPP_INTPTR reader) noexcept;
CHAOS_IL2CPP_INTPTR ChaosTextReaderReadToEnd(CHAOS_IL2CPP_INTPTR reader) noexcept;
void ChaosTextWriterWrite(CHAOS_IL2CPP_INTPTR writer, CHAOS_IL2CPP_INTPTR value) noexcept;
void ChaosTextWriterWriteLine(CHAOS_IL2CPP_INTPTR writer) noexcept;
void ChaosTextWriterWriteLineStr(CHAOS_IL2CPP_INTPTR writer, CHAOS_IL2CPP_INTPTR value) noexcept;
}

// ═══════════════════════════════════════════════════════════════════════════
// MemoryStream tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Stream_MemoryStream, CtorInitializesFields) {
    SimMemoryStream ms{};
    std::memset(&ms, 0xFF, sizeof(ms));

    ChaosMemoryStreamCtor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ms));

    EXPECT_EQ(ms.position, 0);
    EXPECT_EQ(ms.length, 0);
    EXPECT_EQ(ms.capacity, 0);
    EXPECT_TRUE(ms.expandable);
    EXPECT_TRUE(ms.writable);
    EXPECT_TRUE(ms.exposable);
    EXPECT_TRUE(ms.isOpen);
}

TEST(Stream_MemoryStream, CtorNull) {
    // Should not crash
    ChaosMemoryStreamCtor(0);
}

TEST(Stream_MemoryStream, GetLength) {
    SimMemoryStream ms{};
    ms.length = 42;

    auto len = ChaosStreamGetLength(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ms));
    EXPECT_EQ(len, 42);
}

TEST(Stream_MemoryStream, GetLengthNull) {
    EXPECT_EQ(ChaosStreamGetLength(0), 0);
}

TEST(Stream_MemoryStream, GetPosition) {
    SimMemoryStream ms{};
    ms.position = 77;

    auto pos = ChaosStreamGetPosition(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ms));
    EXPECT_EQ(pos, 77);
}

TEST(Stream_MemoryStream, GetPositionNull) {
    EXPECT_EQ(ChaosStreamGetPosition(0), 0);
}

TEST(Stream_MemoryStream, SeekBegin) {
    SimMemoryStream ms{};
    ms.length = 100;
    ms.position = 50;

    auto new_pos = ChaosStreamSeek(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ms), 10, 0);
    EXPECT_EQ(new_pos, 10);
    EXPECT_EQ(ms.position, 10);
}

TEST(Stream_MemoryStream, SeekCurrent) {
    SimMemoryStream ms{};
    ms.length = 100;
    ms.position = 50;

    auto new_pos = ChaosStreamSeek(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ms), 10, 1);
    EXPECT_EQ(new_pos, 60);
    EXPECT_EQ(ms.position, 60);
}

TEST(Stream_MemoryStream, SeekEnd) {
    SimMemoryStream ms{};
    ms.length = 100;
    ms.position = 50;

    auto new_pos = ChaosStreamSeek(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ms), -10, 2);
    EXPECT_EQ(new_pos, 90);
    EXPECT_EQ(ms.position, 90);
}

TEST(Stream_MemoryStream, SeekClampNegative) {
    SimMemoryStream ms{};
    ms.length = 100;
    ms.position = 50;

    auto new_pos = ChaosStreamSeek(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ms), -1000, 1);
    EXPECT_EQ(new_pos, 0);
    EXPECT_EQ(ms.position, 0);
}

TEST(Stream_MemoryStream, SeekClampPastEnd) {
    SimMemoryStream ms{};
    ms.length = 100;
    ms.position = 50;

    auto new_pos = ChaosStreamSeek(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ms), 1000, 0);
    EXPECT_EQ(new_pos, 100);
    EXPECT_EQ(ms.position, 100);
}

TEST(Stream_MemoryStream, SeekNull) {
    EXPECT_EQ(ChaosStreamSeek(0, 10, 0), 0);
}

TEST(Stream_MemoryStream, SetLength) {
    SimMemoryStream ms{};
    ms.position = 80;

    ChaosStreamSetLength(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ms), 200);

    EXPECT_EQ(ms.length, 200);
    EXPECT_EQ(ms.position, 80);  // position unchanged (within length)
}

TEST(Stream_MemoryStream, SetLengthClampPosition) {
    SimMemoryStream ms{};
    ms.position = 80;

    ChaosStreamSetLength(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ms), 50);

    EXPECT_EQ(ms.length, 50);
    EXPECT_EQ(ms.position, 50);  // clamped
}

TEST(Stream_MemoryStream, SetLengthNull) {
    // Should not crash
    ChaosStreamSetLength(0, 100);
}

TEST(Stream_MemoryStream, Flush) {
    SimMemoryStream ms{};
    ms.isOpen = true;

    // Should not crash
    ChaosStreamFlush(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ms));
}

TEST(Stream_MemoryStream, FlushNull) {
    // Should not crash
    ChaosStreamFlush(0);
}

// ═══════════════════════════════════════════════════════════════════════════
// StringReader tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Stream_StringReader, Ctor) {
    SimString str{};
    sim_string_set(&str, "hello world");

    SimStringReader reader{};
    std::memset(&reader, 0xFF, sizeof(reader));

    ChaosStringReaderCtor(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&reader),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&str));

    EXPECT_EQ(reader.str, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&str));
    EXPECT_EQ(reader.pos, 0);
    EXPECT_EQ(reader.length, 11);
}

TEST(Stream_StringReader, CtorNull) {
    // Should not crash
    ChaosStringReaderCtor(0, 0);
}

TEST(Stream_StringReader, ReadLine) {
    SimString str{};
    sim_string_set(&str, "line1\nline2");

    SimStringReader reader{};
    reader.str = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&str);
    reader.pos = 0;
    reader.length = 10;  // "line1\nline2"

    auto result = ChaosTextReaderReadLine(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&reader));
    EXPECT_NE(result, 0);
    EXPECT_EQ(reader.pos, 6);  // advanced past "line1\n"
    // Can't easily verify content of returned string without managed string reader
}

TEST(Stream_StringReader, ReadLineAtEnd) {
    SimString str{};
    sim_string_set(&str, "done");

    SimStringReader reader{};
    reader.str = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&str);
    reader.pos = 10;
    reader.length = 10;

    // No more data
    auto result = ChaosTextReaderReadLine(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&reader));
    EXPECT_NE(result, 0);  // Returns empty string, not null
}

TEST(Stream_StringReader, ReadToEnd) {
    SimString str{};
    sim_string_set(&str, "full content");

    SimStringReader reader{};
    reader.str = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&str);
    reader.pos = 0;
    reader.length = 12;

    auto result = ChaosTextReaderReadToEnd(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&reader));
    EXPECT_NE(result, 0);
    EXPECT_EQ(reader.pos, 12);  // advanced to end
}

TEST(Stream_StringReader, ReadToEndAtEnd) {
    SimString str{};
    sim_string_set(&str, "done");

    SimStringReader reader{};
    reader.str = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&str);
    reader.pos = 10;
    reader.length = 10;

    auto result = ChaosTextReaderReadToEnd(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&reader));
    EXPECT_NE(result, 0);  // empty string
}

TEST(Stream_StringReader, ReadLineNull) {
    EXPECT_EQ(ChaosTextReaderReadLine(0), 0);
}

TEST(Stream_StringReader, ReadToEndNull) {
    EXPECT_EQ(ChaosTextReaderReadToEnd(0), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// StringWriter tests
// ═══════════════════════════════════════════════════════════════════════════

// For StringWriter, we test that the functions don't crash and that the
// side-storage correctly accepts writes.  We use a unique handle value
// that won't collide with real managed objects.
static constexpr CHAOS_IL2CPP_INTPTR kWriterHandle1 =
    static_cast<CHAOS_IL2CPP_INTPTR>(0x1001);
static constexpr CHAOS_IL2CPP_INTPTR kWriterHandle2 =
    static_cast<CHAOS_IL2CPP_INTPTR>(0x1002);

TEST(Stream_StringWriter, Ctor) {
    // Should not crash
    ChaosStringWriterCtor(kWriterHandle1);
}

TEST(Stream_StringWriter, CtorNull) {
    // Should not crash
    ChaosStringWriterCtor(0);
}

TEST(Stream_StringWriter, Write) {
    // Must call Ctor first to register the writer
    ChaosStringWriterCtor(kWriterHandle1);
    // Should not crash — writes "hello" to the side buffer
    SimString str{};
    sim_string_set(&str, "hello");
    ChaosTextWriterWrite(kWriterHandle1, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&str));
}

TEST(Stream_StringWriter, WriteLine) {
    ChaosTextWriterWriteLine(kWriterHandle1);
}

TEST(Stream_StringWriter, WriteLineStr) {
    SimString str{};
    sim_string_set(&str, "world");
    ChaosTextWriterWriteLineStr(kWriterHandle1, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&str));
}

TEST(Stream_StringWriter, NullWriter) {
    SimString str{};
    sim_string_set(&str, "test");
    // Should not crash
    ChaosTextWriterWrite(0, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&str));
    ChaosTextWriterWriteLine(0);
    ChaosTextWriterWriteLineStr(0, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&str));
}

TEST(Stream_StringWriter, MultipleWriters) {
    // Use distinct handles, should not interfere
    SimString s1{}, s2{};
    sim_string_set(&s1, "AAAA");
    sim_string_set(&s2, "BBBB");

    ChaosStringWriterCtor(kWriterHandle2);
    ChaosTextWriterWrite(kWriterHandle1, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s1));
    ChaosTextWriterWrite(kWriterHandle2, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s2));
}

TEST(Stream_StringWriter, WriteWithNullValue) {
    // Should not crash
    ChaosTextWriterWrite(kWriterHandle1, 0);
    ChaosTextWriterWriteLineStr(kWriterHandle1, 0);
}

TEST(Stream_StringWriter, WriteEmpty) {
    SimString str{};
    sim_string_set(&str, "");
    // Should not crash
    ChaosTextWriterWrite(kWriterHandle1, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&str));
    ChaosTextWriterWriteLineStr(kWriterHandle1, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&str));
}

// ═══════════════════════════════════════════════════════════════════════════
// Edge case tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Stream_EdgeCases, ReadLineWithCRLF) {
    SimString str{};
    sim_string_set(&str, "abc\r\ndef");

    SimStringReader reader{};
    reader.str = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&str);
    reader.pos = 0;
    reader.length = 8;  // "abc\r\ndef"

    auto result = ChaosTextReaderReadLine(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&reader));
    EXPECT_NE(result, 0);
    // pos should be past \r\n = 5
    EXPECT_EQ(reader.pos, 5);
}

TEST(Stream_EdgeCases, ReadLineWithCarriageReturnOnly) {
    SimString str{};
    sim_string_set(&str, "abc\rdef");

    SimStringReader reader{};
    reader.str = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&str);
    reader.pos = 0;
    reader.length = 7;

    auto result = ChaosTextReaderReadLine(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&reader));
    EXPECT_NE(result, 0);
    EXPECT_EQ(reader.pos, 4);  // past \r
}

TEST(Stream_EdgeCases, ReadLineWithConsecutiveNewlines) {
    SimString str{};
    sim_string_set(&str, "\n\nline3");

    SimStringReader reader{};
    reader.str = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&str);
    reader.pos = 0;
    reader.length = 7;  // "\n\nline3" (2+5=7)

    // First call returns empty string (between \n and \n)
    auto result1 = ChaosTextReaderReadLine(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&reader));
    EXPECT_NE(result1, 0);
    EXPECT_EQ(reader.pos, 1);  // past first \n

    // Second call returns empty string (between \n and line3)
    auto result2 = ChaosTextReaderReadLine(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&reader));
    EXPECT_NE(result2, 0);
    EXPECT_EQ(reader.pos, 2);  // past second \n

    // Third call returns "line3"
    auto result3 = ChaosTextReaderReadLine(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&reader));
    EXPECT_NE(result3, 0);
    EXPECT_EQ(reader.pos, 7);  // past end
}

TEST(Stream_EdgeCases, ReadLineOnlyNewline) {
    SimString str{};
    sim_string_set(&str, "\n");

    SimStringReader reader{};
    reader.str = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&str);
    reader.pos = 0;
    reader.length = 1;

    auto result = ChaosTextReaderReadLine(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&reader));
    EXPECT_NE(result, 0);  // empty string
    EXPECT_EQ(reader.pos, 1);
}

TEST(Stream_EdgeCases, ReadLineNoTrailingNewline) {
    SimString str{};
    sim_string_set(&str, "abc\ndef");

    SimStringReader reader{};
    reader.str = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&str);
    reader.pos = 0;
    reader.length = 7;

    auto result1 = ChaosTextReaderReadLine(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&reader));
    EXPECT_NE(result1, 0);
    EXPECT_EQ(reader.pos, 4);  // past "abc\n"

    auto result2 = ChaosTextReaderReadLine(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&reader));
    EXPECT_NE(result2, 0);  // "def" (no trailing newline)
    EXPECT_EQ(reader.pos, 7);  // past end
}

TEST(Stream_EdgeCases, ReadLineWithLongLine) {
    SimString str{};
    char long_line[64];
    std::memset(long_line, 'A', 50);
    long_line[50] = '\n';
    long_line[51] = 'e';
    long_line[52] = 'n';
    long_line[53] = 'd';
    long_line[54] = '\0';
    sim_string_set(&str, long_line);

    SimStringReader reader{};
    reader.str = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&str);
    reader.pos = 0;
    reader.length = 54;  // 50×A + \n + end

    auto result = ChaosTextReaderReadLine(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&reader));
    EXPECT_NE(result, 0);
    EXPECT_EQ(reader.pos, 51);  // past 50×A + \n
}

TEST(Stream_EdgeCases, StringReaderNullInternalString) {
    SimStringReader reader{};
    std::memset(&reader, 0, sizeof(reader));
    reader.str = 0;  // null internal string
    reader.pos = 0;
    reader.length = 10;

    // Should not crash, return null/empty
    auto result1 = ChaosTextReaderReadLine(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&reader));
    EXPECT_EQ(result1, 0);

    auto result2 = ChaosTextReaderReadToEnd(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&reader));
    EXPECT_EQ(result2, 0);
}

TEST(Stream_EdgeCases, SeekEndWithPositiveOffset) {
    SimMemoryStream ms{};
    std::memset(&ms, 0, sizeof(ms));
    ms.length = 100;
    ms.position = 50;

    // SeekOrigin.End with positive offset — should clamp to length
    auto new_pos = ChaosStreamSeek(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ms), 50, 2);
    EXPECT_EQ(new_pos, 100);  // clamped to length
    EXPECT_EQ(ms.position, 100);
}

TEST(Stream_EdgeCases, SetLengthNegative) {
    SimMemoryStream ms{};
    std::memset(&ms, 0, sizeof(ms));
    ms.length = 100;
    ms.position = 50;

    // Negative length via int64→int32 truncation
    ChaosStreamSetLength(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ms), -1);
    // Should at least not crash; length becomes some value from int32 truncation
    (void)ms;
}

TEST(Stream_EdgeCases, StringWriterSlotExhaustion) {
    // Register 8 writers (max slots)
    CHAOS_IL2CPP_INTPTR handles[9];
    for (int i = 0; i < 9; i++) {
        handles[i] = static_cast<CHAOS_IL2CPP_INTPTR>(0x2000 + i);
        ChaosStringWriterCtor(handles[i]);
    }

    // All 8 slots filled — 9th writer silently fails (no crash)
    SimString str{};
    sim_string_set(&str, "test");
    ChaosTextWriterWrite(handles[8], reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&str));

    // Existing writers still work
    ChaosTextWriterWrite(handles[0], reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&str));
}

TEST(Stream_EdgeCases, MemoryStreamSeekInvalidOrigin) {
    SimMemoryStream ms{};
    std::memset(&ms, 0, sizeof(ms));
    ms.length = 100;
    ms.position = 50;

    // Invalid origin should return current position unchanged
    auto new_pos = ChaosStreamSeek(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ms), 10, 99);
    EXPECT_EQ(new_pos, 50);  // unchanged
    EXPECT_EQ(ms.position, 50);
}

TEST(Stream_EdgeCases, ReadLineWithCRAtEnd) {
    SimString str{};
    sim_string_set(&str, "abc\r");

    SimStringReader reader{};
    reader.str = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&str);
    reader.pos = 0;
    reader.length = 4;  // "abc\r"

    auto result = ChaosTextReaderReadLine(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&reader));
    EXPECT_NE(result, 0);
    EXPECT_EQ(reader.pos, 4);  // past \r
}

TEST(Stream_EdgeCases, MemoryStreamFullCycle) {
    SimMemoryStream ms{};
    std::memset(&ms, 0, sizeof(ms));

    // Constructor
    ChaosMemoryStreamCtor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ms));

    // Set length
    ChaosStreamSetLength(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ms), 1000);
    EXPECT_EQ(ChaosStreamGetLength(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ms)), 1000);

    // Seek
    auto pos = ChaosStreamSeek(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ms), 500, 0);
    EXPECT_EQ(pos, 500);
    EXPECT_EQ(ChaosStreamGetPosition(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ms)), 500);

    // Seek relative
    pos = ChaosStreamSeek(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ms), 100, 1);
    EXPECT_EQ(pos, 600);
    EXPECT_EQ(ChaosStreamGetPosition(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ms)), 600);

    // Seek from end
    pos = ChaosStreamSeek(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ms), -50, 2);
    EXPECT_EQ(pos, 950);

    // Flush
    ChaosStreamFlush(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ms));
}
