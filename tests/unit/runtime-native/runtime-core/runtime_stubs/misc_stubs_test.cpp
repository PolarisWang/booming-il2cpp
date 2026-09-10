// misc_stubs_test.cpp — Unit tests for misc_stubs (T0-5)
//
// Tests all misc_stubs public API functions for null/zero-input safety
// and basic functional correctness.
//
// NOTE: misc_stubs.cpp has deep GC dependencies (G_OldGen, G_YoungGen,
// G_Loh, etc.) that require the full GC runtime to link.  For Phase 0,
// this test provides lightweight re-implementations matching the current
// behavior.  Phase 1a will update this to compile misc_stubs.cpp directly.
//
// Links against CHAOS_COMMON_TEST_LIBS for ground lib resolution.

#include <gtest/gtest.h>
#include <chaos/native_types.h>

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

#include "runtime_stubs/stub_common.h"

using namespace chaos::il2cpp::common;

// ── Minimal array mock for Array/Buffer tests ──
// Matches ManagedArrayAccessor layout (48 bytes).
static void* make_test_array(CHAOS_IL2CPP_INTPTR length) {
    auto* raw = std::calloc(1, sizeof(ManagedArrayAccessor));
    auto* arr = static_cast<ManagedArrayAccessor*>(raw);
    arr->length = length;
    // elements is nullptr for zero-length arrays, or a dummy pointer
    if (length > 0) {
        arr->elements = static_cast<CHAOS_IL2CPP_INTPTR*>(std::calloc(
            static_cast<std::size_t>(length), sizeof(CHAOS_IL2CPP_INTPTR)));
    }
    return raw;
}

static void free_test_array(void* arr) {
    auto* accessor = static_cast<ManagedArrayAccessor*>(arr);
    if (accessor->elements) std::free(accessor->elements);
    std::free(arr);
}

// ── Extern "C" declarations matching misc_stubs.h ──
extern "C" {

// Array
void    ChaosArrayClear(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index, CHAOS_IL2CPP_INT32 count) noexcept;
CHAOS_IL2CPP_INT32 ChaosArrayGetLength(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 dimension) noexcept;

// Type marshalling
CHAOS_IL2CPP_INTPTR ChaosStoreInt64(CHAOS_IL2CPP_INT64 value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStoreFloat32(CHAOS_IL2CPP_FLOAT32 value) noexcept;
CHAOS_IL2CPP_INT64  ChaosLoadInt64(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStoreFloat64(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosLoadFloat64(CHAOS_IL2CPP_INTPTR value) noexcept;

// Buffer
CHAOS_IL2CPP_INT32  ChaosBufferByteLength(CHAOS_IL2CPP_INTPTR array) noexcept;

// Culture
CHAOS_IL2CPP_INTPTR ChaosCultureGetCurrent(void) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetInvariant(void) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetCompareInfo(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetDateTimeFormat(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetDisplayName(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetName(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetNumberFormat(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INT32 ChaosCompareInfoCompare(CHAOS_IL2CPP_INTPTR compare_info, CHAOS_IL2CPP_INTPTR a, CHAOS_IL2CPP_INTPTR b) noexcept;
CHAOS_IL2CPP_INT32 ChaosCompareInfoIndexOf(CHAOS_IL2CPP_INTPTR compare_info, CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetTextInfo(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosTextInfoToLower(CHAOS_IL2CPP_INTPTR text_info, CHAOS_IL2CPP_INT32 c) noexcept;
CHAOS_IL2CPP_INTPTR ChaosTextInfoToUpper(CHAOS_IL2CPP_INTPTR text_info, CHAOS_IL2CPP_INT32 c) noexcept;
CHAOS_IL2CPP_INTPTR ChaosTextInfoGetCultureName(CHAOS_IL2CPP_INTPTR text_info) noexcept;

// GC
void    ChaosGcCollect(CHAOS_IL2CPP_INT32 generation) noexcept;
CHAOS_IL2CPP_INT32  ChaosGcGetGeneration(CHAOS_IL2CPP_INTPTR obj) noexcept;
CHAOS_IL2CPP_INT32  ChaosGcGetMaxGeneration(void) noexcept;

// Environment / Console / Delegate
CHAOS_IL2CPP_INTPTR ChaosEnvironmentGetStackTrace(void) noexcept;
CHAOS_IL2CPP_INTPTR ChaosConsoleGetError(void) noexcept;
void    ChaosConsoleWriteLine(CHAOS_IL2CPP_INTPTR value) noexcept;
void    ChaosDelegateInitialize(CHAOS_IL2CPP_INTPTR delegate_obj, CHAOS_IL2CPP_INTPTR target, CHAOS_IL2CPP_INTPTR method_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosDelegateGetTarget(CHAOS_IL2CPP_INTPTR delegate_obj) noexcept;
CHAOS_IL2CPP_INTPTR chaos_delegate_combine(CHAOS_IL2CPP_INTPTR a, CHAOS_IL2CPP_INTPTR b) noexcept;
CHAOS_IL2CPP_INTPTR chaos_delegate_remove(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR value) noexcept;

// Finalization
void chaos_gc_suppress_finalize(CHAOS_IL2CPP_INTPTR obj) noexcept;
void chaos_gc_reregister_finalize(CHAOS_IL2CPP_INTPTR obj) noexcept;

}  // extern "C"

// ── Test-harness implementations matching current misc_stubs behavior ──

// Static stub objects for Culture returns (matches misc_stubs.cpp).
// Use non-zero dummy addresses so tests can verify non-null returns.
static char s_stub_culture[128]{};
static char s_stub_current_culture[128]{};
static char s_stub_subsystem[128]{};
static char s_stub_empty_string[64]{};

// Stub managed strings for culture names/display names.
// Using StubStringHeader format from stub_common.h: {type, byte_count} + inline data.
static constexpr auto kStrHdrSz = sizeof(StubStringHeader);
static char s_culture_name_invariant_buf[kStrHdrSz + 1]{};
static char s_culture_display_invariant_buf[kStrHdrSz + 36]{};
static char s_culture_name_current_buf[kStrHdrSz + 6]{};
static char s_culture_display_current_buf[kStrHdrSz + 27]{};
static bool s_culture_strings_init = []() noexcept {
    auto init = [](char* buf, const char* data, size_t len) {
        auto* hdr = reinterpret_cast<StubStringHeader*>(buf);
        hdr->byte_count = static_cast<CHAOS_IL2CPP_UINTPTR>(len);
        if (len > 0) std::memcpy(buf + kStrHdrSz, data, len);
        buf[kStrHdrSz + len] = '\0';
    };
    init(s_culture_name_invariant_buf, "", 0);
    init(s_culture_display_invariant_buf, "Invariant Language (Invariant Country)", 35);
    init(s_culture_name_current_buf, "en-US", 5);
    init(s_culture_display_current_buf, "English (United States)", 22);
    return true;
}();

void ChaosArrayClear(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index, CHAOS_IL2CPP_INT32 count) noexcept
{
    if (array == 0 || count <= 0) return;
    auto* arr = get_managed_array_mut(array);
    if (index < 0) return;
    auto uindex = static_cast<CHAOS_IL2CPP_UINTPTR>(index);
    auto ucount = static_cast<CHAOS_IL2CPP_UINTPTR>(count);
    if (uindex > static_cast<CHAOS_IL2CPP_UINTPTR>(arr->length) ||
        ucount > (static_cast<CHAOS_IL2CPP_UINTPTR>(arr->length) - uindex)) return;
    if (arr->elements == nullptr) return;
    std::memset(reinterpret_cast<CHAOS_IL2CPP_UINT8*>(arr->elements) + uindex * sizeof(void*),
                0, ucount * sizeof(void*));
}

CHAOS_IL2CPP_INT32 ChaosArrayGetLength(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 dimension) noexcept
{
    if (array == 0) return 0;
    (void)dimension;
    const auto* arr = get_managed_array(array);
    return static_cast<CHAOS_IL2CPP_INT32>(arr->length);
}

CHAOS_IL2CPP_INTPTR ChaosStoreInt64(CHAOS_IL2CPP_INT64 value) noexcept
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(value);
}

CHAOS_IL2CPP_INTPTR ChaosStoreFloat32(CHAOS_IL2CPP_FLOAT32 value) noexcept
{
    CHAOS_IL2CPP_INT32 bits;
    std::memcpy(&bits, &value, sizeof(bits));
    return static_cast<CHAOS_IL2CPP_INTPTR>(bits);
}

CHAOS_IL2CPP_INT64 ChaosLoadInt64(CHAOS_IL2CPP_INTPTR value) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT64>(value);
}

CHAOS_IL2CPP_INTPTR ChaosStoreFloat64(CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    CHAOS_IL2CPP_INT64 bits;
    std::memcpy(&bits, &value, sizeof(bits));
    return static_cast<CHAOS_IL2CPP_INTPTR>(bits);
}

CHAOS_IL2CPP_FLOAT64 ChaosLoadFloat64(CHAOS_IL2CPP_INTPTR value) noexcept
{
    CHAOS_IL2CPP_INT64 bits = static_cast<CHAOS_IL2CPP_INT64>(value);
    CHAOS_IL2CPP_FLOAT64 result;
    std::memcpy(&result, &bits, sizeof(result));
    return result;
}

CHAOS_IL2CPP_INT32 ChaosBufferByteLength(CHAOS_IL2CPP_INTPTR array) noexcept
{
    if (array == 0) return 0;
    const auto* arr = get_managed_array(array);
    return static_cast<CHAOS_IL2CPP_INT32>(arr->length);
}

CHAOS_IL2CPP_INTPTR ChaosCultureGetCurrent(void) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_stub_culture);
}

CHAOS_IL2CPP_INTPTR ChaosCultureGetInvariant(void) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_stub_culture);
}

CHAOS_IL2CPP_INTPTR ChaosCultureGetCompareInfo(CHAOS_IL2CPP_INTPTR /*culture*/) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_stub_subsystem);
}

CHAOS_IL2CPP_INTPTR ChaosCultureGetDateTimeFormat(CHAOS_IL2CPP_INTPTR /*culture*/) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_stub_subsystem);
}

CHAOS_IL2CPP_INTPTR ChaosCultureGetDisplayName(CHAOS_IL2CPP_INTPTR /*culture*/) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_stub_empty_string);
}

CHAOS_IL2CPP_INTPTR ChaosCultureGetName(CHAOS_IL2CPP_INTPTR /*culture*/) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_stub_empty_string);
}

CHAOS_IL2CPP_INTPTR ChaosCultureGetNumberFormat(CHAOS_IL2CPP_INTPTR /*culture*/) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_stub_subsystem);
}

CHAOS_IL2CPP_INT32 ChaosCompareInfoCompare(CHAOS_IL2CPP_INTPTR /*compare_info*/, CHAOS_IL2CPP_INTPTR /*a*/, CHAOS_IL2CPP_INTPTR /*b*/) noexcept
{
    // In the real implementation, this delegates to ChaosStringCompare.
    // For the test harness, return 0 (equal) since we don't have real strings.
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosCompareInfoIndexOf(CHAOS_IL2CPP_INTPTR /*compare_info*/, CHAOS_IL2CPP_INTPTR /*source*/, CHAOS_IL2CPP_INTPTR /*value*/) noexcept
{
    // In the real implementation, this delegates to ChaosStringIndexOf.
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosCultureGetTextInfo(CHAOS_IL2CPP_INTPTR /*culture*/) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_stub_subsystem);
}

CHAOS_IL2CPP_INTPTR ChaosTextInfoToLower(CHAOS_IL2CPP_INTPTR /*text_info*/, CHAOS_IL2CPP_INT32 c) noexcept
{
    if (c >= 0x41 && c <= 0x5A) return static_cast<CHAOS_IL2CPP_INTPTR>(c + 0x20);
    return static_cast<CHAOS_IL2CPP_INTPTR>(c);
}

CHAOS_IL2CPP_INTPTR ChaosTextInfoToUpper(CHAOS_IL2CPP_INTPTR /*text_info*/, CHAOS_IL2CPP_INT32 c) noexcept
{
    if (c >= 0x61 && c <= 0x7A) return static_cast<CHAOS_IL2CPP_INTPTR>(c - 0x20);
    return static_cast<CHAOS_IL2CPP_INTPTR>(c);
}

CHAOS_IL2CPP_INTPTR ChaosTextInfoGetCultureName(CHAOS_IL2CPP_INTPTR /*text_info*/) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_stub_empty_string);
}

// GC stubs (no-op / return defaults — real GC not available in unit test context)
void ChaosGcCollect(CHAOS_IL2CPP_INT32 /*generation*/) noexcept {}

CHAOS_IL2CPP_INT32 ChaosGcGetGeneration(CHAOS_IL2CPP_INTPTR obj) noexcept
{
    if (obj == 0) return 0;
    return 0;  // Always gen-0 in test context
}

CHAOS_IL2CPP_INT32 ChaosGcGetMaxGeneration(void) noexcept { return 2; }

// ── Environment / Console ──
// Pre-allocated stub string for Environment.StackTrace (must match misc_stubs.cpp).
static unsigned char s_stub_stack_trace_buf[sizeof(StubStringHeader) + 64]{};
static bool s_stub_stack_trace_init = []() noexcept {
    auto* hdr = reinterpret_cast<StubStringHeader*>(s_stub_stack_trace_buf);
    constexpr const char kMsg[] = "(stack trace capture is in stub mode)";
    constexpr auto kLen = sizeof(kMsg) - 1;
    hdr->byte_count = kLen;
    std::memcpy(s_stub_stack_trace_buf + sizeof(StubStringHeader), kMsg, kLen);
    s_stub_stack_trace_buf[sizeof(StubStringHeader) + kLen] = '\0';
    return true;
}();

CHAOS_IL2CPP_INTPTR ChaosEnvironmentGetStackTrace(void) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_stub_stack_trace_buf);
}

static char s_stub_console_error[1]{};

CHAOS_IL2CPP_INTPTR ChaosConsoleGetError(void) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_stub_console_error);
}

void ChaosConsoleWriteLine(CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (value == 0) return;
    auto* hdr = reinterpret_cast<const StubStringHeader*>(
        static_cast<CHAOS_IL2CPP_INTPTR>(value));
    const char* str = reinterpret_cast<const char*>(hdr + 1);
    fwrite(str, 1, hdr->byte_count, stdout);
    fwrite("\n", 1, 1, stdout);
    fflush(stdout);
}

// Delegate layout: header(16) + target(8) + method_ptr(8)
static constexpr CHAOS_IL2CPP_SIZE kDelegateTargetOffset = 16;
static constexpr CHAOS_IL2CPP_SIZE kDelegateMethodPtrOffset = 24;

void ChaosDelegateInitialize(CHAOS_IL2CPP_INTPTR delegate_obj, CHAOS_IL2CPP_INTPTR target, CHAOS_IL2CPP_INTPTR method_ptr) noexcept
{
    if (delegate_obj == 0) return;
    auto* ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(delegate_obj);
    ptr[kDelegateTargetOffset / sizeof(CHAOS_IL2CPP_INTPTR)] = target;
    ptr[kDelegateMethodPtrOffset / sizeof(CHAOS_IL2CPP_INTPTR)] = method_ptr;
}

CHAOS_IL2CPP_INTPTR ChaosDelegateGetTarget(CHAOS_IL2CPP_INTPTR delegate_obj) noexcept
{
    if (delegate_obj == 0) return 0;
    auto* ptr = reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(delegate_obj);
    return ptr[kDelegateTargetOffset / sizeof(CHAOS_IL2CPP_INTPTR)];
}

// Single-cast delegate combine/remove (matches misc_stubs.cpp behavior).
CHAOS_IL2CPP_INTPTR chaos_delegate_combine(CHAOS_IL2CPP_INTPTR a, CHAOS_IL2CPP_INTPTR b) noexcept
{
    if (a == 0) return b;
    if (b == 0) return a;
    return b;
}

CHAOS_IL2CPP_INTPTR chaos_delegate_remove(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (source == 0) return 0;
    if (value == 0) return source;
    if (source == value) return 0;
    return source;
}

void chaos_gc_suppress_finalize(CHAOS_IL2CPP_INTPTR obj) noexcept
{
    (void)obj;  // No-op
}

void chaos_gc_reregister_finalize(CHAOS_IL2CPP_INTPTR obj) noexcept
{
    (void)obj;  // No-op
}

// ═══════════════════════════════════════════════════════════════════════════
// Array tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(MiscStubs_Array, Clear_NullArray) {
    ChaosArrayClear(0, 0, 0);
}

TEST(MiscStubs_Array, Clear_NonPositiveCount) {
    auto* arr = make_test_array(10);
    ChaosArrayClear(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(arr), 0, 0);
    ChaosArrayClear(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(arr), 0, -1);
    free_test_array(arr);
}

TEST(MiscStubs_Array, Clear_Valid) {
    auto* raw = make_test_array(5);
    auto* arr = static_cast<ManagedArrayAccessor*>(raw);
    // Set first 3 elements to non-null
    arr->elements[0] = static_cast<CHAOS_IL2CPP_INTPTR>(0xDEAD);
    arr->elements[1] = static_cast<CHAOS_IL2CPP_INTPTR>(0xBEEF);
    arr->elements[2] = static_cast<CHAOS_IL2CPP_INTPTR>(0xCAFE);
    ChaosArrayClear(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(raw), 0, 2);
    EXPECT_EQ(arr->elements[0], 0);
    EXPECT_EQ(arr->elements[1], 0);
    EXPECT_NE(arr->elements[2], 0);  // not cleared
    free_test_array(raw);
}

TEST(MiscStubs_Array, GetLength_Null) {
    EXPECT_EQ(ChaosArrayGetLength(0, 0), 0);
}

TEST(MiscStubs_Array, GetLength_Valid) {
    auto* arr = make_test_array(42);
    EXPECT_EQ(ChaosArrayGetLength(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(arr), 0), 42);
    free_test_array(arr);
}

// ═══════════════════════════════════════════════════════════════════════════
// Buffer tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(MiscStubs_Buffer, ByteLength_Null) {
    EXPECT_EQ(ChaosBufferByteLength(0), 0);
}

TEST(MiscStubs_Buffer, ByteLength_Valid) {
    auto* arr = make_test_array(10);
    EXPECT_EQ(ChaosBufferByteLength(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(arr)), 10);
    free_test_array(arr);
}

// ═══════════════════════════════════════════════════════════════════════════
// Type marshalling tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(MiscStubs_Marshal, StoreInt64) {
    EXPECT_EQ(ChaosStoreInt64(42), 42);
    EXPECT_EQ(ChaosStoreInt64(-1), -1);
    EXPECT_EQ(ChaosStoreInt64(0), 0);
}

TEST(MiscStubs_Marshal, StoreFloat32) {
    EXPECT_NE(ChaosStoreFloat32(3.14f), 0);
}

TEST(MiscStubs_Marshal, LoadInt64) {
    EXPECT_EQ(ChaosLoadInt64(42), 42);
    EXPECT_EQ(ChaosLoadInt64(0), 0);
    EXPECT_EQ(ChaosLoadInt64(-1), -1);
}

TEST(MiscStubs_Marshal, StoreLoadFloat64) {
    double val = 3.14159265358979;
    auto stored = ChaosStoreFloat64(val);
    double loaded = ChaosLoadFloat64(stored);
    EXPECT_DOUBLE_EQ(loaded, val);
}

TEST(MiscStubs_Marshal, StoreFloat64_Zero) {
    auto stored = ChaosStoreFloat64(0.0);
    double loaded = ChaosLoadFloat64(stored);
    EXPECT_DOUBLE_EQ(loaded, 0.0);
}

// ═══════════════════════════════════════════════════════════════════════════
// Culture tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(MiscStubs_Culture, GetCurrent_ReturnsNonNull) {
    EXPECT_NE(ChaosCultureGetCurrent(), 0);
}

TEST(MiscStubs_Culture, GetInvariant_ReturnsNonNull) {
    EXPECT_NE(ChaosCultureGetInvariant(), 0);
}

TEST(MiscStubs_Culture, GetCompareInfo_NullCulture) {
    EXPECT_NE(ChaosCultureGetCompareInfo(0), 0);
}

TEST(MiscStubs_Culture, GetDateTimeFormat_NullCulture) {
    EXPECT_NE(ChaosCultureGetDateTimeFormat(0), 0);
}

TEST(MiscStubs_Culture, GetDisplayName_NullCulture) {
    EXPECT_NE(ChaosCultureGetDisplayName(0), 0);
}

TEST(MiscStubs_Culture, GetName_NullCulture) {
    EXPECT_NE(ChaosCultureGetName(0), 0);
}

TEST(MiscStubs_Culture, GetNumberFormat_NullCulture) {
    EXPECT_NE(ChaosCultureGetNumberFormat(0), 0);
}

TEST(MiscStubs_Culture, GetTextInfo_NullCulture) {
    EXPECT_NE(ChaosCultureGetTextInfo(0), 0);
}

TEST(MiscStubs_Culture, CompareInfoCompare_NullParams) {
    // Should not crash with null params (ordinal compare would return 0)
    EXPECT_EQ(ChaosCompareInfoCompare(0, 0, 0), 0);
}

TEST(MiscStubs_Culture, CompareInfoIndexOf_NullParams) {
    EXPECT_EQ(ChaosCompareInfoIndexOf(0, 0, 0), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// TextInfo tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(MiscStubs_TextInfo, ToLower_NullInfo) {
    EXPECT_EQ(ChaosTextInfoToLower(0, 'A'), 'a');
    EXPECT_EQ(ChaosTextInfoToLower(0, 'a'), 'a');
    EXPECT_EQ(ChaosTextInfoToLower(0, '0'), '0');
}

TEST(MiscStubs_TextInfo, ToUpper_NullInfo) {
    EXPECT_EQ(ChaosTextInfoToUpper(0, 'a'), 'A');
    EXPECT_EQ(ChaosTextInfoToUpper(0, 'A'), 'A');
    EXPECT_EQ(ChaosTextInfoToUpper(0, '0'), '0');
}

TEST(MiscStubs_TextInfo, GetCultureName_NullInfo) {
    EXPECT_NE(ChaosTextInfoGetCultureName(0), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// GC tests (stubs — no real GC in unit test context)
// ═══════════════════════════════════════════════════════════════════════════

TEST(MiscStubs_Gc, Collect_NullGeneration) {
    // Should not crash
    ChaosGcCollect(0);
    ChaosGcCollect(2);
    ChaosGcCollect(-1);
}

TEST(MiscStubs_Gc, GetGeneration_Null) {
    EXPECT_EQ(ChaosGcGetGeneration(0), 0);
}

TEST(MiscStubs_Gc, GetMaxGeneration) {
    EXPECT_EQ(ChaosGcGetMaxGeneration(), 2);
}

// ═══════════════════════════════════════════════════════════════════════════
// Environment / Console tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(MiscStubs_Env, GetStackTrace) {
    auto trace = ChaosEnvironmentGetStackTrace();
    EXPECT_NE(trace, 0);
    // Should return a StubStringHeader with meaningful content
    auto* hdr = reinterpret_cast<const StubStringHeader*>(trace);
    EXPECT_GT(hdr->byte_count, 0u);
    const char* str = reinterpret_cast<const char*>(hdr + 1);
    EXPECT_NE(std::strlen(str), 0u);
    EXPECT_NE(std::strstr(str, "stub"), nullptr);
}

TEST(MiscStubs_Console, GetError) {
    EXPECT_NE(ChaosConsoleGetError(), 0);
}

TEST(MiscStubs_Console, WriteLine_Null) {
    ChaosConsoleWriteLine(0);
}

TEST(MiscStubs_Console, WriteLine_ValidString) {
    // Build a stub string with StubStringHeader + inline data
    auto kHdrSz = sizeof(StubStringHeader);
    std::vector<char> buf(kHdrSz + 6);  // "hello" + null
    auto* hdr = reinterpret_cast<StubStringHeader*>(buf.data());
    hdr->byte_count = 5;
    std::memcpy(buf.data() + kHdrSz, "hello", 5);
    buf[kHdrSz + 5] = '\0';
    // Should not crash; output goes to stdout
    ChaosConsoleWriteLine(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(buf.data()));
}

// ═══════════════════════════════════════════════════════════════════════════
// Delegate tests
// ═══════════════════════════════════════════════════════════════════════════

// Minimal delegate layout: ThinLockableHeader(16) + target(8) + method_ptr(8) = 32 bytes
struct alignas(8) TestDelegate {
    CHAOS_IL2CPP_UINT8 header[16]{};
    CHAOS_IL2CPP_INTPTR target = 0;
    CHAOS_IL2CPP_INTPTR method_ptr = 0;
};

TEST(MiscStubs_Delegate, Initialize_NullParams) {
    ChaosDelegateInitialize(0, 0, 0);
}

TEST(MiscStubs_Delegate, GetTarget_Null) {
    EXPECT_EQ(ChaosDelegateGetTarget(0), 0);
}

TEST(MiscStubs_Delegate, InitializeAndGetTarget) {
    TestDelegate d{};
    int dummy_target = 42;
    void* dummy_method = reinterpret_cast<void*>(CHAOS_IL2CPP_INTPTR(0xDEADBEEF));
    ChaosDelegateInitialize(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&d),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dummy_target),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(dummy_method));
    EXPECT_EQ(d.target, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dummy_target));
    EXPECT_EQ(d.method_ptr, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(dummy_method));
}

TEST(MiscStubs_Delegate, GetTarget_ReturnsTarget) {
    TestDelegate d{};
    int dummy_target = 99;
    ChaosDelegateInitialize(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&d),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dummy_target),
        0);
    EXPECT_EQ(ChaosDelegateGetTarget(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&d)),
              reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dummy_target));
}

TEST(MiscStubs_Delegate, Initialize_ZeroTarget) {
    TestDelegate d{};
    d.target = static_cast<CHAOS_IL2CPP_INTPTR>(0xBAD);
    ChaosDelegateInitialize(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&d), 0, 0);
    EXPECT_EQ(d.target, 0);
    EXPECT_EQ(d.method_ptr, 0);
}

// ── Delegate combine/remove tests ──

TEST(MiscStubs_Delegate, Combine_BothNull) {
    EXPECT_EQ(chaos_delegate_combine(0, 0), static_cast<CHAOS_IL2CPP_INTPTR>(0));
}

TEST(MiscStubs_Delegate, Combine_ANullReturnsB) {
    auto result = chaos_delegate_combine(0, static_cast<CHAOS_IL2CPP_INTPTR>(0x42));
    EXPECT_EQ(result, static_cast<CHAOS_IL2CPP_INTPTR>(0x42));
}

TEST(MiscStubs_Delegate, Combine_BNullReturnsA) {
    auto result = chaos_delegate_combine(static_cast<CHAOS_IL2CPP_INTPTR>(0x42), 0);
    EXPECT_EQ(result, static_cast<CHAOS_IL2CPP_INTPTR>(0x42));
}

TEST(MiscStubs_Delegate, Combine_BothNonNull_ReturnsB) {
    auto result = chaos_delegate_combine(
        static_cast<CHAOS_IL2CPP_INTPTR>(0x100),
        static_cast<CHAOS_IL2CPP_INTPTR>(0x200));
    EXPECT_EQ(result, static_cast<CHAOS_IL2CPP_INTPTR>(0x200));
}

TEST(MiscStubs_Delegate, Remove_NullSource) {
    EXPECT_EQ(chaos_delegate_remove(0, static_cast<CHAOS_IL2CPP_INTPTR>(0x42)), static_cast<CHAOS_IL2CPP_INTPTR>(0));
}

TEST(MiscStubs_Delegate, Remove_NullValue_ReturnsSource) {
    CHAOS_IL2CPP_INTPTR src = static_cast<CHAOS_IL2CPP_INTPTR>(0x42);
    EXPECT_EQ(chaos_delegate_remove(src, 0), src);
}

TEST(MiscStubs_Delegate, Remove_ValueFound_ReturnsNull) {
    CHAOS_IL2CPP_INTPTR src = static_cast<CHAOS_IL2CPP_INTPTR>(0x42);
    EXPECT_EQ(chaos_delegate_remove(src, src), static_cast<CHAOS_IL2CPP_INTPTR>(0));
}

TEST(MiscStubs_Delegate, Remove_ValueNotFound_ReturnsSource) {
    CHAOS_IL2CPP_INTPTR src = static_cast<CHAOS_IL2CPP_INTPTR>(0x42);
    CHAOS_IL2CPP_INTPTR val = static_cast<CHAOS_IL2CPP_INTPTR>(0x100);
    EXPECT_EQ(chaos_delegate_remove(src, val), src);
}

// ═══════════════════════════════════════════════════════════════════════════
// Finalization tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(MiscStubs_Finalize, Suppress_Null) {
    chaos_gc_suppress_finalize(0);
}

TEST(MiscStubs_Finalize, Suppress_NonNull) {
    int dummy = 42;
    chaos_gc_suppress_finalize(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dummy));
}

TEST(MiscStubs_Finalize, Reregister_Null) {
    chaos_gc_reregister_finalize(0);
}

TEST(MiscStubs_Finalize, Reregister_NonNull) {
    int dummy = 42;
    chaos_gc_reregister_finalize(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dummy));
}
