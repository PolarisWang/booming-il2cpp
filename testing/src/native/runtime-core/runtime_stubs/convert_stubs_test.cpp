// convert_stubs_test.cpp — Unit tests for convert/parse/decimal stubs (t1a-1)
//
// Tests all convert/parse/decimal public API functions with test-harness
// implementations matching the behavior in parse_convert.cpp.
//
// NOTE: The real implementations live in parse_convert.cpp which depends on
// string_table, fmt, runtime_core.h, and exception_helpers.h.  For Phase 1a
// we use test-harness implementations.  Future updates should compile
// parse_convert.cpp directly once the dependency chain is resolved.
//
// Links against CHAOS_COMMON_TEST_LIBS for ground lib resolution.

#include <gtest/gtest.h>
#include <chaos/native_types.h>

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <cerrno>
#include <climits>
#include <limits>
#include <cstdio>

// ═══════════════════════════════════════════════════════════════════════════
// Minimal DecimalCarrier matching runtime_core.h layout
// ═══════════════════════════════════════════════════════════════════════════

// NOTE: This is a simplified version.  The real DecimalCarrier in runtime_core.h
// may have additional fields — this matches the subset used by parse_convert.cpp.
struct DecimalCarrier {
    CHAOS_IL2CPP_UINT32 flags;
    CHAOS_IL2CPP_UINT32 hi32;
    CHAOS_IL2CPP_UINT64 lo64;
};
static_assert(sizeof(DecimalCarrier) == 16, "DecimalCarrier must be 16 bytes");

// ═══════════════════════════════════════════════════════════════════════════
// Test-harness: Convert functions
// ═══════════════════════════════════════════════════════════════════════════

// Null-terminate for C library functions.
static const char* NullTerm(const char* data, CHAOS_IL2CPP_INT32 len) {
    if (len <= 0) return "";
    static thread_local char s_buf[128];
    if (len > static_cast<CHAOS_IL2CPP_INT32>(sizeof(s_buf) - 1))
        len = static_cast<CHAOS_IL2CPP_INT32>(sizeof(s_buf) - 1);
    std::memcpy(s_buf, data, static_cast<std::size_t>(len));
    s_buf[len] = '\0';
    return s_buf;
}

// Helper: create a minimal string container for testing.
struct TestString {
    CHAOS_IL2CPP_INT32 length;
    const char* utf8_data;
    char storage[64];

    TestString(const char* s) {
        length = static_cast<CHAOS_IL2CPP_INT32>(std::strlen(s));
        std::memcpy(storage, s, static_cast<std::size_t>(length));
        utf8_data = storage;
    }
};

// ═══════════════════════════════════════════════════════════════════════════
// Test-harness: Convert::ToBoolean
// ═══════════════════════════════════════════════════════════════════════════

extern "C" CHAOS_IL2CPP_INT32 ChaosConvertToBoolean(CHAOS_IL2CPP_INTPTR value) noexcept;

// Simplified: returns 0 for null/invalid, 1 for "true", 0 for "false"
CHAOS_IL2CPP_INT32 ChaosConvertToBoolean(CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (value == 0 || value < 0x1000) return 0;
    auto* ts = reinterpret_cast<const TestString*>(reinterpret_cast<void*>(value));
    if (ts->length <= 0 || ts->utf8_data == nullptr) return 0;
    const char* s = NullTerm(ts->utf8_data, ts->length);
    if (std::strcmp(s, "true") == 0) return 1;
    if (std::strcmp(s, "false") == 0) return 0;
    return 0;  // Simplified: no FormatException in test harness
}

// ═══════════════════════════════════════════════════════════════════════════
// Test-harness: Convert::ToByte / ToInt16 / ToInt32 / ToInt64
// ═══════════════════════════════════════════════════════════════════════════

extern "C" {
CHAOS_IL2CPP_UINT8  ChaosConvertToByte(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT16  ChaosConvertToInt16(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT32  ChaosConvertToInt32(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT64  ChaosConvertToInt64(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosConvertToSingle(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosConvertToDouble(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosFormatInt32(CHAOS_IL2CPP_INT32 value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosFormatDouble(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_INT32  ChaosParseInt32(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT64  ChaosParseInt64(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosParseDouble(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT32  ChaosConvertToInt32FromDouble(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosConvertToDecimal(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT32  ChaosDecimalToInt32(CHAOS_IL2CPP_INTPTR carrier_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosDecimalFromDouble(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosDecimalAdd(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosDecimalSubtract(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosDecimalMultiply(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosDecimalDivide(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept;
}

static inline const TestString* PtrToTestString(CHAOS_IL2CPP_INTPTR value) {
    return reinterpret_cast<const TestString*>(reinterpret_cast<void*>(value));
}

// ── strtol-style numeric parsing (simplified — no RaiseManagedException) ──

CHAOS_IL2CPP_UINT8 ChaosConvertToByte(CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (value == 0 || value < 0x1000) return 0;
    auto* ts = PtrToTestString(value);
    if (ts->length <= 0) return 0;
    const char* s = NullTerm(ts->utf8_data, ts->length);
    char* end = nullptr;
    errno = 0;
    unsigned long result = std::strtoul(s, &end, 10);
    if (errno != 0 || end == s || result > 255) return 0;
    return static_cast<CHAOS_IL2CPP_UINT8>(result);
}

CHAOS_IL2CPP_INT16 ChaosConvertToInt16(CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (value == 0 || value < 0x1000) return 0;
    auto* ts = PtrToTestString(value);
    if (ts->length <= 0) return 0;
    const char* s = NullTerm(ts->utf8_data, ts->length);
    char* end = nullptr;
    errno = 0;
    long result = std::strtol(s, &end, 10);
    if (errno != 0 || end == s ||
        result < static_cast<long>(std::numeric_limits<CHAOS_IL2CPP_INT16>::min()) ||
        result > static_cast<long>(std::numeric_limits<CHAOS_IL2CPP_INT16>::max())) return 0;
    return static_cast<CHAOS_IL2CPP_INT16>(result);
}

CHAOS_IL2CPP_INT32 ChaosConvertToInt32(CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (value == 0 || value < 0x1000) return 0;
    auto* ts = PtrToTestString(value);
    if (ts->length <= 0) return 0;
    const char* s = NullTerm(ts->utf8_data, ts->length);
    char* end = nullptr;
    long result = std::strtol(s, &end, 10);
    if (end == s) return 0;
    return static_cast<CHAOS_IL2CPP_INT32>(result);
}

CHAOS_IL2CPP_INT64 ChaosConvertToInt64(CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (value == 0 || value < 0x1000) return 0;
    auto* ts = PtrToTestString(value);
    if (ts->length <= 0) return 0;
    const char* s = NullTerm(ts->utf8_data, ts->length);
    char* end = nullptr;
    errno = 0;
    long long result = std::strtoll(s, &end, 10);
    if (errno != 0 || end == s) return 0;
    return static_cast<CHAOS_IL2CPP_INT64>(result);
}

CHAOS_IL2CPP_FLOAT32 ChaosConvertToSingle(CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (value == 0 || value < 0x1000) return 0.0f;
    auto* ts = PtrToTestString(value);
    if (ts->length <= 0) return 0.0f;
    const char* s = NullTerm(ts->utf8_data, ts->length);
    char* end = nullptr;
    double result = std::strtod(s, &end);
    if (end == s) return 0.0f;
    return static_cast<CHAOS_IL2CPP_FLOAT32>(result);
}

CHAOS_IL2CPP_FLOAT64 ChaosConvertToDouble(CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (value == 0 || value < 0x1000) return 0.0;
    auto* ts = PtrToTestString(value);
    if (ts->length <= 0) return 0.0;
    const char* s = NullTerm(ts->utf8_data, ts->length);
    char* end = nullptr;
    double result = std::strtod(s, &end);
    if (end == s) return 0.0;
    return result;
}

// ── Convert::ToDecimal(String) —→ DecimalCarrier via double ──

CHAOS_IL2CPP_INTPTR ChaosConvertToDecimal(CHAOS_IL2CPP_INTPTR value) noexcept
{
    double d = ChaosConvertToDouble(value);
    return ChaosDecimalFromDouble(d);
}

// ── FormatInt32: fast itoa ──

CHAOS_IL2CPP_INTPTR ChaosFormatInt32(CHAOS_IL2CPP_INT32 value) noexcept
{
    char buf[16];
    char* p = buf + sizeof(buf);
    CHAOS_IL2CPP_UINT32 remaining;
    if (value < 0) {
        remaining = static_cast<CHAOS_IL2CPP_UINT32>(-(value + 1)) + 1u;
    } else {
        remaining = static_cast<CHAOS_IL2CPP_UINT32>(value);
    }
    do {
        *--p = static_cast<char>('0' + (remaining % 10));
        remaining /= 10;
    } while (remaining != 0);
    if (value < 0) *--p = '-';

    // Return a pointer to the static buffer (simplified — real impl uses StringId)
    static thread_local char s_result[16];
    auto len = buf + sizeof(buf) - p;
    std::memcpy(s_result, p, len);
    s_result[len] = '\0';
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_result);
}

// ── FormatDouble: snprintf-based (simplified — real impl uses fmt) ──

CHAOS_IL2CPP_INTPTR ChaosFormatDouble(CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    static thread_local char s_buf[64];
    auto n = std::snprintf(s_buf, sizeof(s_buf), "%g", value);
    if (n < 0 || static_cast<std::size_t>(n) >= sizeof(s_buf)) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_buf);
}

// ── Parse delegations ──

CHAOS_IL2CPP_INT32 ChaosParseInt32(CHAOS_IL2CPP_INTPTR value) noexcept {
    return ChaosConvertToInt32(value);
}

CHAOS_IL2CPP_INT64 ChaosParseInt64(CHAOS_IL2CPP_INTPTR value) noexcept {
    return ChaosConvertToInt64(value);
}

CHAOS_IL2CPP_FLOAT64 ChaosParseDouble(CHAOS_IL2CPP_INTPTR value) noexcept {
    return ChaosConvertToDouble(value);
}

// ── ChaosConvertToInt32FromDouble: truncation with overflow check ──

CHAOS_IL2CPP_INT32 ChaosConvertToInt32FromDouble(CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    if (std::isnan(value) ||
        value < static_cast<CHAOS_IL2CPP_FLOAT64>(std::numeric_limits<CHAOS_IL2CPP_INT32>::min()) ||
        value > static_cast<CHAOS_IL2CPP_FLOAT64>(std::numeric_limits<CHAOS_IL2CPP_INT32>::max())) {
        return 0;  // Simplified: no RaiseManagedException in test harness
    }
    return static_cast<CHAOS_IL2CPP_INT32>(std::trunc(value));
}

// ── Decimal helpers ──

CHAOS_IL2CPP_INT32 ChaosDecimalToInt32(CHAOS_IL2CPP_INTPTR carrier_ptr) noexcept
{
    auto* carrier = reinterpret_cast<const DecimalCarrier*>(carrier_ptr);
    CHAOS_IL2CPP_INT64 result = static_cast<CHAOS_IL2CPP_INT64>(carrier->lo64);
    if (carrier->flags & 0x80000000u) result = -result;
    return static_cast<CHAOS_IL2CPP_INT32>(result);
}

CHAOS_IL2CPP_INTPTR ChaosDecimalFromDouble(CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    thread_local DecimalCarrier s_tls_decimal{};
    auto* out = &s_tls_decimal;
    if (value < 0) {
        out->flags = 0x80000000u;
        out->lo64 = static_cast<CHAOS_IL2CPP_UINT64>(
            static_cast<CHAOS_IL2CPP_INT64>(std::trunc(-value)));
    } else {
        out->flags = 0u;
        out->lo64 = static_cast<CHAOS_IL2CPP_UINT64>(
            static_cast<CHAOS_IL2CPP_INT64>(std::trunc(value)));
    }
    out->hi32 = 0u;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(out);
}

static CHAOS_IL2CPP_INTPTR DecimalFromDoubleResult(double result) {
    auto* out = new DecimalCarrier{};
    if (result < 0) {
        out->flags = 0x80000000u;
        out->lo64 = static_cast<CHAOS_IL2CPP_UINT64>(static_cast<CHAOS_IL2CPP_INT64>(result));
    } else {
        out->flags = 0u;
        out->lo64 = static_cast<CHAOS_IL2CPP_UINT64>(static_cast<CHAOS_IL2CPP_INT64>(result));
    }
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(out);
}

CHAOS_IL2CPP_INTPTR ChaosDecimalAdd(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept
{
    auto* l = reinterpret_cast<const DecimalCarrier*>(left_ptr);
    auto* r = reinterpret_cast<const DecimalCarrier*>(right_ptr);
    double ld = static_cast<double>(static_cast<CHAOS_IL2CPP_INT64>(l->lo64));
    double rd = static_cast<double>(static_cast<CHAOS_IL2CPP_INT64>(r->lo64));
    if (l->flags & 0x80000000u) ld = -ld;
    if (r->flags & 0x80000000u) rd = -rd;
    return DecimalFromDoubleResult(ld + rd);
}

CHAOS_IL2CPP_INTPTR ChaosDecimalSubtract(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept
{
    auto* l = reinterpret_cast<const DecimalCarrier*>(left_ptr);
    auto* r = reinterpret_cast<const DecimalCarrier*>(right_ptr);
    double ld = static_cast<double>(static_cast<CHAOS_IL2CPP_INT64>(l->lo64));
    double rd = static_cast<double>(static_cast<CHAOS_IL2CPP_INT64>(r->lo64));
    if (l->flags & 0x80000000u) ld = -ld;
    if (r->flags & 0x80000000u) rd = -rd;
    return DecimalFromDoubleResult(ld - rd);
}

CHAOS_IL2CPP_INTPTR ChaosDecimalMultiply(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept
{
    auto* l = reinterpret_cast<const DecimalCarrier*>(left_ptr);
    auto* r = reinterpret_cast<const DecimalCarrier*>(right_ptr);
    double ld = static_cast<double>(static_cast<CHAOS_IL2CPP_INT64>(l->lo64));
    double rd = static_cast<double>(static_cast<CHAOS_IL2CPP_INT64>(r->lo64));
    if (l->flags & 0x80000000u) ld = -ld;
    if (r->flags & 0x80000000u) rd = -rd;
    return DecimalFromDoubleResult(ld * rd);
}

CHAOS_IL2CPP_INTPTR ChaosDecimalDivide(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept
{
    auto* l = reinterpret_cast<const DecimalCarrier*>(left_ptr);
    auto* r = reinterpret_cast<const DecimalCarrier*>(right_ptr);
    double ld = static_cast<double>(static_cast<CHAOS_IL2CPP_INT64>(l->lo64));
    double rd = static_cast<double>(static_cast<CHAOS_IL2CPP_INT64>(r->lo64));
    if (l->flags & 0x80000000u) ld = -ld;
    if (r->flags & 0x80000000u) rd = -rd;
    if (rd == 0.0) return 0;
    return DecimalFromDoubleResult(ld / rd);
}

// ═══════════════════════════════════════════════════════════════════════════
// Helper: make a test string pointer
// ═══════════════════════════════════════════════════════════════════════════

static CHAOS_IL2CPP_INTPTR make_test_string(const char* s) {
    auto* ts = new TestString(s);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ts);
}

// ═══════════════════════════════════════════════════════════════════════════
// Convert::ToBoolean tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_ToBoolean, Null) {
    EXPECT_EQ(ChaosConvertToBoolean(0), 0);
}

TEST(Convert_ToBoolean, True) {
    auto s = make_test_string("true");
    EXPECT_EQ(ChaosConvertToBoolean(s), 1);
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToBoolean, False) {
    auto s = make_test_string("false");
    EXPECT_EQ(ChaosConvertToBoolean(s), 0);
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToBoolean, Invalid) {
    auto s = make_test_string("invalid");
    EXPECT_EQ(ChaosConvertToBoolean(s), 0);
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToBoolean, CaseSensitive) {
    auto s = make_test_string("True");
    EXPECT_EQ(ChaosConvertToBoolean(s), 0);  // Case-sensitive, not "true"
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToBoolean, Empty) {
    auto s = make_test_string("");
    EXPECT_EQ(ChaosConvertToBoolean(s), 0);
    delete reinterpret_cast<TestString*>(s);
}

// ═══════════════════════════════════════════════════════════════════════════
// Convert::ToByte tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_ToByte, Null) {
    EXPECT_EQ(ChaosConvertToByte(0), 0);
}

TEST(Convert_ToByte, Valid) {
    auto s = make_test_string("123");
    EXPECT_EQ(ChaosConvertToByte(s), 123);
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToByte, Zero) {
    auto s = make_test_string("0");
    EXPECT_EQ(ChaosConvertToByte(s), 0);
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToByte, MaxValue) {
    auto s = make_test_string("255");
    EXPECT_EQ(ChaosConvertToByte(s), 255);
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToByte, Overflow) {
    auto s = make_test_string("256");
    EXPECT_EQ(ChaosConvertToByte(s), 0);  // Overflow → 0 in test harness
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToByte, Negative) {
    auto s = make_test_string("-1");
    EXPECT_EQ(ChaosConvertToByte(s), 0);  // Overflow
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToByte, Invalid) {
    auto s = make_test_string("abc");
    EXPECT_EQ(ChaosConvertToByte(s), 0);
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToByte, LeadingWhitespace) {
    auto s = make_test_string("  42");
    EXPECT_EQ(ChaosConvertToByte(s), 42);  // strtol skips whitespace
    delete reinterpret_cast<TestString*>(s);
}

// ═══════════════════════════════════════════════════════════════════════════
// Convert::ToInt16 tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_ToInt16, Null) {
    EXPECT_EQ(ChaosConvertToInt16(0), 0);
}

TEST(Convert_ToInt16, Valid) {
    auto s = make_test_string("32767");
    EXPECT_EQ(ChaosConvertToInt16(s), 32767);
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToInt16, Negative) {
    auto s = make_test_string("-32768");
    EXPECT_EQ(ChaosConvertToInt16(s), -32768);
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToInt16, Overflow) {
    auto s = make_test_string("32768");
    EXPECT_EQ(ChaosConvertToInt16(s), 0);
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToInt16, Invalid) {
    auto s = make_test_string("xyz");
    EXPECT_EQ(ChaosConvertToInt16(s), 0);
    delete reinterpret_cast<TestString*>(s);
}

// ═══════════════════════════════════════════════════════════════════════════
// Convert::ToInt32 tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_ToInt32, Null) {
    EXPECT_EQ(ChaosConvertToInt32(0), 0);
}

TEST(Convert_ToInt32, Valid) {
    auto s = make_test_string("1234567");
    EXPECT_EQ(ChaosConvertToInt32(s), 1234567);
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToInt32, Negative) {
    auto s = make_test_string("-999");
    EXPECT_EQ(ChaosConvertToInt32(s), -999);
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToInt32, Zero) {
    auto s = make_test_string("0");
    EXPECT_EQ(ChaosConvertToInt32(s), 0);
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToInt32, MaxValue) {
    auto s = make_test_string("2147483647");
    EXPECT_EQ(ChaosConvertToInt32(s), 2147483647);
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToInt32, MinValue) {
    auto s = make_test_string("-2147483648");
    EXPECT_EQ(ChaosConvertToInt32(s), -2147483648LL);
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToInt32, Invalid) {
    auto s = make_test_string("notanumber");
    EXPECT_EQ(ChaosConvertToInt32(s), 0);
    delete reinterpret_cast<TestString*>(s);
}

// ═══════════════════════════════════════════════════════════════════════════
// Convert::ToInt64 tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_ToInt64, Null) {
    EXPECT_EQ(ChaosConvertToInt64(0), 0);
}

TEST(Convert_ToInt64, LargeNumber) {
    auto s = make_test_string("9223372036854775807");
    EXPECT_EQ(ChaosConvertToInt64(s), 9223372036854775807LL);
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToInt64, Negative) {
    auto s = make_test_string("-9223372036854775807");
    EXPECT_EQ(ChaosConvertToInt64(s), -9223372036854775807LL - 1 + 1);
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToInt64, Overflow) {
    auto s = make_test_string("999999999999999999999");
    EXPECT_EQ(ChaosConvertToInt64(s), 0);  // Overflow → 0 in test harness
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToInt64, Invalid) {
    auto s = make_test_string("bad");
    EXPECT_EQ(ChaosConvertToInt64(s), 0);
    delete reinterpret_cast<TestString*>(s);
}

// ═══════════════════════════════════════════════════════════════════════════
// Convert::ToSingle (float) tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_ToSingle, Null) {
    EXPECT_FLOAT_EQ(ChaosConvertToSingle(0), 0.0f);
}

TEST(Convert_ToSingle, Valid) {
    auto s = make_test_string("3.14");
    EXPECT_FLOAT_EQ(ChaosConvertToSingle(s), 3.14f);
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToSingle, Negative) {
    auto s = make_test_string("-2.5");
    EXPECT_FLOAT_EQ(ChaosConvertToSingle(s), -2.5f);
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToSingle, Invalid) {
    auto s = make_test_string("notfloat");
    EXPECT_FLOAT_EQ(ChaosConvertToSingle(s), 0.0f);
    delete reinterpret_cast<TestString*>(s);
}

// ═══════════════════════════════════════════════════════════════════════════
// Convert::ToDouble tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_ToDouble, Null) {
    EXPECT_DOUBLE_EQ(ChaosConvertToDouble(0), 0.0);
}

TEST(Convert_ToDouble, Valid) {
    auto s = make_test_string("3.14159265358979");
    EXPECT_DOUBLE_EQ(ChaosConvertToDouble(s), 3.14159265358979);
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToDouble, Scientific) {
    auto s = make_test_string("1e10");
    EXPECT_DOUBLE_EQ(ChaosConvertToDouble(s), 1e10);
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToDouble, Negative) {
    auto s = make_test_string("-0.5");
    EXPECT_DOUBLE_EQ(ChaosConvertToDouble(s), -0.5);
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToDouble, Invalid) {
    auto s = make_test_string("bad");
    EXPECT_DOUBLE_EQ(ChaosConvertToDouble(s), 0.0);
    delete reinterpret_cast<TestString*>(s);
}

// ═══════════════════════════════════════════════════════════════════════════
// Convert::ToDecimal (string → DecimalCarrier) tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_ToDecimal, Null) {
    auto result = ChaosConvertToDecimal(0);
    EXPECT_NE(result, 0);  // Returns thread-local DecimalCarrier
    auto* dc = reinterpret_cast<const DecimalCarrier*>(result);
    EXPECT_EQ(dc->lo64, 0u);
}

TEST(Convert_ToDecimal, Positive) {
    auto s = make_test_string("42");
    auto result = ChaosConvertToDecimal(s);
    auto* dc = reinterpret_cast<const DecimalCarrier*>(result);
    EXPECT_EQ(dc->lo64, 42u);
    EXPECT_EQ(dc->flags, 0u);
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ToDecimal, Negative) {
    auto s = make_test_string("-10");
    auto result = ChaosConvertToDecimal(s);
    auto* dc = reinterpret_cast<const DecimalCarrier*>(result);
    EXPECT_EQ(dc->lo64, 10u);
    EXPECT_EQ(dc->flags, 0x80000000u);
    delete reinterpret_cast<TestString*>(s);
}

// ═══════════════════════════════════════════════════════════════════════════
// FormatInt32 tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_FormatInt32, Zero) {
    auto result = ChaosFormatInt32(0);
    EXPECT_STREQ(reinterpret_cast<const char*>(result), "0");
}

TEST(Convert_FormatInt32, Positive) {
    auto result = ChaosFormatInt32(12345);
    EXPECT_STREQ(reinterpret_cast<const char*>(result), "12345");
}

TEST(Convert_FormatInt32, Negative) {
    auto result = ChaosFormatInt32(-42);
    EXPECT_STREQ(reinterpret_cast<const char*>(result), "-42");
}

TEST(Convert_FormatInt32, MaxValue) {
    auto result = ChaosFormatInt32(2147483647);
    EXPECT_STREQ(reinterpret_cast<const char*>(result), "2147483647");
}

TEST(Convert_FormatInt32, MinValue) {
    auto result = ChaosFormatInt32(-2147483648);
    EXPECT_STREQ(reinterpret_cast<const char*>(result), "-2147483648");
}

// ═══════════════════════════════════════════════════════════════════════════
// FormatDouble tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_FormatDouble, Zero) {
    auto result = ChaosFormatDouble(0.0);
    EXPECT_NE(result, 0);
}

TEST(Convert_FormatDouble, Pi) {
    auto result = ChaosFormatDouble(3.14159);
    EXPECT_NE(result, 0);
    auto* s = reinterpret_cast<const char*>(result);
    EXPECT_GT(std::strlen(s), 0u);
}

TEST(Convert_FormatDouble, Negative) {
    auto result = ChaosFormatDouble(-2.5);
    EXPECT_NE(result, 0);
    auto* s = reinterpret_cast<const char*>(result);
    EXPECT_EQ(s[0], '-');
}

// ═══════════════════════════════════════════════════════════════════════════
// Parse delegations (same as Convert equivalents)
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_ParseInt32, Valid) {
    auto s = make_test_string("456");
    EXPECT_EQ(ChaosParseInt32(s), 456);
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ParseInt64, Valid) {
    auto s = make_test_string("789");
    EXPECT_EQ(ChaosParseInt64(s), 789);
    delete reinterpret_cast<TestString*>(s);
}

TEST(Convert_ParseDouble, Valid) {
    auto s = make_test_string("2.718");
    EXPECT_DOUBLE_EQ(ChaosParseDouble(s), 2.718);
    delete reinterpret_cast<TestString*>(s);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosConvertToInt32FromDouble tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_Int32FromDouble, Positive) {
    EXPECT_EQ(ChaosConvertToInt32FromDouble(42.7), 42);  // truncation
}

TEST(Convert_Int32FromDouble, Negative) {
    EXPECT_EQ(ChaosConvertToInt32FromDouble(-3.9), -3);  // truncation toward zero
}

TEST(Convert_Int32FromDouble, Zero) {
    EXPECT_EQ(ChaosConvertToInt32FromDouble(0.0), 0);
}

TEST(Convert_Int32FromDouble, NaN) {
    EXPECT_EQ(ChaosConvertToInt32FromDouble(std::numeric_limits<double>::quiet_NaN()), 0);
}

TEST(Convert_Int32FromDouble, Overflow_TooLarge) {
    EXPECT_EQ(ChaosConvertToInt32FromDouble(1e20), 0);  // Overflow
}

TEST(Convert_Int32FromDouble, Overflow_TooSmall) {
    EXPECT_EQ(ChaosConvertToInt32FromDouble(-1e20), 0);  // Overflow
}

// ═══════════════════════════════════════════════════════════════════════════
// Decimal operations tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_DecimalToInt32, Positive) {
    DecimalCarrier dc{};
    dc.flags = 0;
    dc.lo64 = 42;
    EXPECT_EQ(ChaosDecimalToInt32(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dc)), 42);
}

TEST(Convert_DecimalToInt32, Negative) {
    DecimalCarrier dc{};
    dc.flags = 0x80000000u;
    dc.lo64 = 10;
    EXPECT_EQ(ChaosDecimalToInt32(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dc)), -10);
}

TEST(Convert_DecimalToInt32, Zero) {
    DecimalCarrier dc{};
    EXPECT_EQ(ChaosDecimalToInt32(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dc)), 0);
}

TEST(Convert_DecimalFromDouble, Positive) {
    auto result = ChaosDecimalFromDouble(123.0);
    auto* dc = reinterpret_cast<const DecimalCarrier*>(result);
    EXPECT_EQ(dc->lo64, 123u);
    EXPECT_EQ(dc->flags, 0u);
}

TEST(Convert_DecimalFromDouble, Negative) {
    auto result = ChaosDecimalFromDouble(-50.0);
    auto* dc = reinterpret_cast<const DecimalCarrier*>(result);
    EXPECT_EQ(dc->lo64, 50u);
    EXPECT_EQ(dc->flags, 0x80000000u);
}

TEST(Convert_DecimalFromDouble, Zero) {
    auto result = ChaosDecimalFromDouble(0.0);
    auto* dc = reinterpret_cast<const DecimalCarrier*>(result);
    EXPECT_EQ(dc->lo64, 0u);
    EXPECT_EQ(dc->flags, 0u);
}

TEST(Convert_DecimalFromDouble, Truncation) {
    auto result = ChaosDecimalFromDouble(3.99);
    auto* dc = reinterpret_cast<const DecimalCarrier*>(result);
    EXPECT_EQ(dc->lo64, 3u);  // Truncated, not rounded
}

// ── Decimal arithmetic ──

TEST(Convert_DecimalAdd, Simple) {
    DecimalCarrier a{}, b{};
    a.lo64 = 10; b.lo64 = 20;
    auto result = ChaosDecimalAdd(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&a),
                                   reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&b));
    auto* dc = reinterpret_cast<const DecimalCarrier*>(result);
    EXPECT_EQ(dc->lo64, 30u);
    EXPECT_EQ(dc->flags, 0u);
    delete dc;
}

TEST(Convert_DecimalSubtract, Simple) {
    DecimalCarrier a{}, b{};
    a.lo64 = 30; b.lo64 = 10;
    auto result = ChaosDecimalSubtract(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&a),
                                        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&b));
    auto* dc = reinterpret_cast<const DecimalCarrier*>(result);
    EXPECT_EQ(dc->lo64, 20u);
    delete dc;
}

TEST(Convert_DecimalMultiply, Simple) {
    DecimalCarrier a{}, b{};
    a.lo64 = 6; b.lo64 = 7;
    auto result = ChaosDecimalMultiply(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&a),
                                        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&b));
    auto* dc = reinterpret_cast<const DecimalCarrier*>(result);
    EXPECT_EQ(dc->lo64, 42u);
    delete dc;
}

TEST(Convert_DecimalDivide, Simple) {
    DecimalCarrier a{}, b{};
    a.lo64 = 10; b.lo64 = 2;
    auto result = ChaosDecimalDivide(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&a),
                                      reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&b));
    auto* dc = reinterpret_cast<const DecimalCarrier*>(result);
    EXPECT_EQ(dc->lo64, 5u);
    delete dc;
}

TEST(Convert_DecimalDivide, ByZero) {
    DecimalCarrier a{}, b{};
    a.lo64 = 10; b.lo64 = 0;
    auto result = ChaosDecimalDivide(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&a),
                                      reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&b));
    EXPECT_EQ(result, 0);  // Division by zero → null
    (void)result;
}

// ═══════════════════════════════════════════════════════════════════════════
// Test-harness: chaos_convert_tochar_* (convert.cpp coverage)
// ═══════════════════════════════════════════════════════════════════════════
//
// Self-contained implementations matching the behavior in convert.cpp
// without requiring deep runtime dependencies (exception_helpers,
// string_table, etc.).  Functions that raise exceptions in the real impl
// return 0 here (same pattern as ChaosConvertToInt32FromDouble).
//
// Reference: convert.cpp lines 20-208, 18 functions total.

// --- Numeric: always-valid range ---

static CHAOS_IL2CPP_UINT16 H_ToChar_Byte(CHAOS_IL2CPP_INTPTR value) {
    return static_cast<CHAOS_IL2CPP_UINT16>(static_cast<CHAOS_IL2CPP_UINT8>(value & 0xFF));
}

static CHAOS_IL2CPP_UINT16 H_ToChar_Char(CHAOS_IL2CPP_INTPTR value) {
    return static_cast<CHAOS_IL2CPP_UINT16>(static_cast<uint16_t>(value));
}

static CHAOS_IL2CPP_UINT16 H_ToChar_UInt16(CHAOS_IL2CPP_INTPTR value) {
    return static_cast<CHAOS_IL2CPP_UINT16>(static_cast<uint16_t>(value));
}

// --- Numeric: range-checked (overflow → 0 in test harness) ---

static CHAOS_IL2CPP_UINT16 H_ToChar_Int16(CHAOS_IL2CPP_INTPTR value) {
    if (value < 0) return 0;
    return static_cast<CHAOS_IL2CPP_UINT16>(value);
}

static CHAOS_IL2CPP_UINT16 H_ToChar_Int32(CHAOS_IL2CPP_INTPTR value) {
    if (value < 0 || value > static_cast<CHAOS_IL2CPP_INTPTR>(0xFFFF)) return 0;
    return static_cast<CHAOS_IL2CPP_UINT16>(value);
}

static CHAOS_IL2CPP_UINT16 H_ToChar_Int64(CHAOS_IL2CPP_INTPTR value) {
    if (value < 0 || value > static_cast<CHAOS_IL2CPP_INTPTR>(0xFFFF)) return 0;
    return static_cast<CHAOS_IL2CPP_UINT16>(value);
}

static CHAOS_IL2CPP_UINT16 H_ToChar_SByte(CHAOS_IL2CPP_INTPTR value) {
    if (value < 0) return 0;
    return static_cast<CHAOS_IL2CPP_UINT16>(value);
}

static CHAOS_IL2CPP_UINT16 H_ToChar_UInt32(CHAOS_IL2CPP_INTPTR value) {
    if (value > static_cast<CHAOS_IL2CPP_INTPTR>(0xFFFF)) return 0;
    return static_cast<CHAOS_IL2CPP_UINT16>(value);
}

static CHAOS_IL2CPP_UINT16 H_ToChar_UInt64(CHAOS_IL2CPP_INTPTR value) {
    if (value > static_cast<CHAOS_IL2CPP_INTPTR>(0xFFFF)) return 0;
    return static_cast<CHAOS_IL2CPP_UINT16>(value);
}

// --- Throwing overloads (return 0 in test harness) ---

static CHAOS_IL2CPP_UINT16 H_ToChar_Boolean(CHAOS_IL2CPP_INTPTR value) {
    (void)value;
    return 0;  // Real impl: chaos_raise_exception (InvalidCastException)
}

static CHAOS_IL2CPP_UINT16 H_ToChar_DateTime(CHAOS_IL2CPP_INTPTR value) {
    (void)value;
    return 0;  // Real impl: chaos_raise_exception (InvalidCastException)
}

static CHAOS_IL2CPP_UINT16 H_ToChar_Decimal(CHAOS_IL2CPP_INTPTR value) {
    auto* carrier = reinterpret_cast<const DecimalCarrier*>(value);
    uint32_t scale = (carrier->flags >> 16) & 0xFF;
    CHAOS_IL2CPP_INT64 intVal = static_cast<CHAOS_IL2CPP_INT64>(carrier->lo64);
    for (uint32_t i = 0; i < scale; i++) {
        intVal /= 10;
    }
    if (carrier->flags & 0x80000000u) {
        intVal = -intVal;
    }
    if (intVal < 0 || intVal > 0xFFFF) return 0;
    return static_cast<CHAOS_IL2CPP_UINT16>(intVal);
}

static CHAOS_IL2CPP_UINT16 H_ToChar_Double(CHAOS_IL2CPP_INTPTR value) {
    // value is a bit-cast double (load_float64)
    double d;
    std::memcpy(&d, &value, sizeof(d));
    auto intVal = static_cast<int>(d);
    if (intVal < 0 || intVal > 0xFFFF) return 0;
    return static_cast<CHAOS_IL2CPP_UINT16>(intVal);
}

static CHAOS_IL2CPP_UINT16 H_ToChar_Single(CHAOS_IL2CPP_INTPTR value) {
    CHAOS_IL2CPP_INT32 bits = static_cast<CHAOS_IL2CPP_INT32>(static_cast<int32_t>(value));
    float f;
    std::memcpy(&f, &bits, sizeof(f));
    auto intVal = static_cast<int>(f);
    if (intVal < 0 || intVal > 0xFFFF) return 0;
    return static_cast<CHAOS_IL2CPP_UINT16>(intVal);
}

// --- Object overloads (unbox int32 payload) ---

static CHAOS_IL2CPP_UINT16 H_ToChar_Object(CHAOS_IL2CPP_INTPTR value) {
    auto* slots = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(value);
    CHAOS_IL2CPP_INTPTR payload = slots[1];
    return H_ToChar_Int32(payload);
}

static CHAOS_IL2CPP_UINT16 H_ToChar_ObjectProvider(CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_INTPTR provider) {
    (void)provider;
    auto* slots = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(value);
    CHAOS_IL2CPP_INTPTR payload = slots[1];
    return H_ToChar_Int32(payload);
}

// Boxed Int32 layout: PureTypeHeader (8B: type_info) + payload (8B) = 16B
struct BoxedInt32 {
    CHAOS_IL2CPP_INTPTR type_info;
    CHAOS_IL2CPP_INTPTR payload;
};

// ═══════════════════════════════════════════════════════════════════════════
// chaos_convert_tochar_byte tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_ToChar_Byte, Zero) {
    EXPECT_EQ(H_ToChar_Byte(0), 0);
}

TEST(Convert_ToChar_Byte, MaxValue) {
    EXPECT_EQ(H_ToChar_Byte(255), 255);
}

TEST(Convert_ToChar_Byte, AboveMaxTruncates) {
    EXPECT_EQ(H_ToChar_Byte(256), 0);  // & 0xFF truncates to 0
}

TEST(Convert_ToChar_Byte, NegativeTruncates) {
    EXPECT_EQ(H_ToChar_Byte(-1), 255);  // & 0xFF truncates
}

// ═══════════════════════════════════════════════════════════════════════════
// chaos_convert_tochar_char tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_ToChar_Char, Zero) {
    EXPECT_EQ(H_ToChar_Char(0), 0);
}

TEST(Convert_ToChar_Char, ValidValue) {
    EXPECT_EQ(H_ToChar_Char(65), 65);  // 'A'
}

TEST(Convert_ToChar_Char, MaxValue) {
    EXPECT_EQ(H_ToChar_Char(0xFFFF), 0xFFFF);
}

TEST(Convert_ToChar_Char, BeyondMaxTruncates) {
    EXPECT_EQ(H_ToChar_Char(0x10000), 0);  // uint16_t truncation
}

// ═══════════════════════════════════════════════════════════════════════════
// chaos_convert_tochar_uint16 tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_ToChar_UInt16, Zero) {
    EXPECT_EQ(H_ToChar_UInt16(0), 0);
}

TEST(Convert_ToChar_UInt16, MaxValue) {
    EXPECT_EQ(H_ToChar_UInt16(0xFFFF), 0xFFFF);
}

TEST(Convert_ToChar_UInt16, BeyondMaxTruncates) {
    EXPECT_EQ(H_ToChar_UInt16(0x10000), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// chaos_convert_tochar_int16 tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_ToChar_Int16, Zero) {
    EXPECT_EQ(H_ToChar_Int16(0), 0);
}

TEST(Convert_ToChar_Int16, Positive) {
    EXPECT_EQ(H_ToChar_Int16(65), 65);
}

TEST(Convert_ToChar_Int16, NegativeOverflow) {
    EXPECT_EQ(H_ToChar_Int16(-1), 0);  // Overflow → 0 in harness
}

// ═══════════════════════════════════════════════════════════════════════════
// chaos_convert_tochar_int32 tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_ToChar_Int32, Zero) {
    EXPECT_EQ(H_ToChar_Int32(0), 0);
}

TEST(Convert_ToChar_Int32, Positive) {
    EXPECT_EQ(H_ToChar_Int32(65), 65);
}

TEST(Convert_ToChar_Int32, MaxValue) {
    EXPECT_EQ(H_ToChar_Int32(0xFFFF), 0xFFFF);
}

TEST(Convert_ToChar_Int32, NegativeOverflow) {
    EXPECT_EQ(H_ToChar_Int32(-1), 0);
}

TEST(Convert_ToChar_Int32, TooLargeOverflow) {
    EXPECT_EQ(H_ToChar_Int32(0x10000), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// chaos_convert_tochar_int64 tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_ToChar_Int64, Zero) {
    EXPECT_EQ(H_ToChar_Int64(0), 0);
}

TEST(Convert_ToChar_Int64, Positive) {
    EXPECT_EQ(H_ToChar_Int64(65), 65);
}

TEST(Convert_ToChar_Int64, NegativeOverflow) {
    EXPECT_EQ(H_ToChar_Int64(-1), 0);
}

TEST(Convert_ToChar_Int64, TooLargeOverflow) {
    EXPECT_EQ(H_ToChar_Int64(0x10000), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// chaos_convert_tochar_sbyte tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_ToChar_SByte, Zero) {
    EXPECT_EQ(H_ToChar_SByte(0), 0);
}

TEST(Convert_ToChar_SByte, Positive) {
    EXPECT_EQ(H_ToChar_SByte(127), 127);
}

TEST(Convert_ToChar_SByte, NegativeOverflow) {
    EXPECT_EQ(H_ToChar_SByte(-1), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// chaos_convert_tochar_uint32 tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_ToChar_UInt32, Zero) {
    EXPECT_EQ(H_ToChar_UInt32(0), 0);
}

TEST(Convert_ToChar_UInt32, MaxValue) {
    EXPECT_EQ(H_ToChar_UInt32(0xFFFF), 0xFFFF);
}

TEST(Convert_ToChar_UInt32, TooLargeOverflow) {
    EXPECT_EQ(H_ToChar_UInt32(0x10000), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// chaos_convert_tochar_uint64 tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_ToChar_UInt64, Zero) {
    EXPECT_EQ(H_ToChar_UInt64(0), 0);
}

TEST(Convert_ToChar_UInt64, MaxValue) {
    EXPECT_EQ(H_ToChar_UInt64(0xFFFF), 0xFFFF);
}

TEST(Convert_ToChar_UInt64, TooLargeOverflow) {
    EXPECT_EQ(H_ToChar_UInt64(0x10000), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// chaos_convert_tochar_boolean tests (always throws)
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_ToChar_Boolean, AlwaysThrows) {
    EXPECT_EQ(H_ToChar_Boolean(0), 0);
    EXPECT_EQ(H_ToChar_Boolean(1), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// chaos_convert_tochar_datetime tests (always throws)
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_ToChar_DateTime, AlwaysThrows) {
    EXPECT_EQ(H_ToChar_DateTime(0), 0);
    EXPECT_EQ(H_ToChar_DateTime(42), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// chaos_convert_tochar_decimal tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_ToChar_Decimal, Positive) {
    DecimalCarrier dc{};
    dc.lo64 = 65;
    dc.flags = 0;
    EXPECT_EQ(H_ToChar_Decimal(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dc)), 65);
}

TEST(Convert_ToChar_Decimal, NegativeOverflow) {
    DecimalCarrier dc{};
    dc.lo64 = 10;
    dc.flags = 0x80000000u;
    EXPECT_EQ(H_ToChar_Decimal(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dc)), 0);  // Negative -10 < 0
}

TEST(Convert_ToChar_Decimal, ScaleDivision) {
    DecimalCarrier dc{};
    dc.lo64 = 1230;  // 123.0 with scale 1
    dc.flags = 0x00010000u;  // scale = 1
    EXPECT_EQ(H_ToChar_Decimal(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dc)), 123);
}

// ═══════════════════════════════════════════════════════════════════════════
// chaos_convert_tochar_double tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_ToChar_Double, Zero) {
    CHAOS_IL2CPP_FLOAT64 val = 0.0;
    CHAOS_IL2CPP_INTPTR bits = 0;
    std::memcpy(&bits, &val, sizeof(bits));
    EXPECT_EQ(H_ToChar_Double(bits), 0);
}

TEST(Convert_ToChar_Double, Positive) {
    CHAOS_IL2CPP_INTPTR bits = 0;
    CHAOS_IL2CPP_FLOAT64 val = 65.0;
    std::memcpy(&bits, &val, sizeof(bits));
    EXPECT_EQ(H_ToChar_Double(bits), 65);
}

TEST(Convert_ToChar_Double, NegativeOverflow) {
    CHAOS_IL2CPP_INTPTR bits = 0;
    CHAOS_IL2CPP_FLOAT64 val = -1.0;
    std::memcpy(&bits, &val, sizeof(bits));
    EXPECT_EQ(H_ToChar_Double(bits), 0);
}

TEST(Convert_ToChar_Double, TooLargeOverflow) {
    CHAOS_IL2CPP_INTPTR bits = 0;
    CHAOS_IL2CPP_FLOAT64 val = 1e5;
    std::memcpy(&bits, &val, sizeof(bits));
    EXPECT_EQ(H_ToChar_Double(bits), 0);  // 100000 > 0xFFFF
}

// ═══════════════════════════════════════════════════════════════════════════
// chaos_convert_tochar_single tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_ToChar_Single, Zero) {
    CHAOS_IL2CPP_INTPTR bits = 0;
    float val = 0.0f;
    int32_t raw;
    std::memcpy(&raw, &val, sizeof(raw));
    bits = raw;
    EXPECT_EQ(H_ToChar_Single(bits), 0);
}

TEST(Convert_ToChar_Single, Positive) {
    int32_t raw;
    float val = 65.0f;
    std::memcpy(&raw, &val, sizeof(raw));
    EXPECT_EQ(H_ToChar_Single(raw), 65);
}

TEST(Convert_ToChar_Single, NegativeOverflow) {
    int32_t raw;
    float val = -1.0f;
    std::memcpy(&raw, &val, sizeof(raw));
    EXPECT_EQ(H_ToChar_Single(raw), 0);
}

TEST(Convert_ToChar_Single, TooLargeOverflow) {
    int32_t raw;
    float val = 1e5f;
    std::memcpy(&raw, &val, sizeof(raw));
    EXPECT_EQ(H_ToChar_Single(raw), 0);  // 100000 > 0xFFFF
}

// ═══════════════════════════════════════════════════════════════════════════
// chaos_convert_tochar_object / object_provider tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Convert_ToChar_Object, ValidPayload) {
    BoxedInt32 boxed{};
    boxed.type_info = 0xDEAD;  // dummy type_info, not used by impl
    boxed.payload = 65;
    EXPECT_EQ(H_ToChar_Object(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&boxed)), 65);
}

TEST(Convert_ToChar_Object, MaxPayload) {
    BoxedInt32 boxed{};
    boxed.payload = 0xFFFF;
    EXPECT_EQ(H_ToChar_Object(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&boxed)), 0xFFFF);
}

TEST(Convert_ToChar_Object, OverflowPayload) {
    BoxedInt32 boxed{};
    boxed.payload = 0x10000;
    EXPECT_EQ(H_ToChar_Object(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&boxed)), 0);
}

TEST(Convert_ToChar_ObjectProvider, DelegatesToInt32) {
    BoxedInt32 boxed{};
    boxed.payload = 65;
    EXPECT_EQ(H_ToChar_ObjectProvider(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&boxed), 0), 65);
}
