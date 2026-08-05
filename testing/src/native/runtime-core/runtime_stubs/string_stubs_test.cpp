// string_stubs_test.cpp — Unit tests for string_stubs.cpp (T0-1)
//
// Tests all 19 public string API functions:
//   ChaosStringContains, ChaosStringStartsWith, ChaosStringEndsWith
//   ChaosStringIndexOf, ChaosStringCompare
//   ChaosStringJoinSs, ChaosStringSplit
//   ChaosStringSubstring, ChaosStringRemove
//   ChaosStringReplace, ChaosStringToLower, ChaosStringToUpper
//   ChaosStringTrim, ChaosStringFastAllocate
//   ChaosStringFormatArray, ChaosStringFormat1, ChaosStringFormat2
//   ChaosStringAppend, ChaosFormattablestringFactoryCreate
//
// Test strings are created via std::malloc (not GcAllocateAtomic) to avoid
// requiring a fully initialized GC. GC/threading stubs are provided in
// gc_stubs_for_string_tests.cpp (compiled alongside this file).
//
// The string APIs themselves are resolved from chaos_runtime_core.lib.

#include <gtest/gtest.h>
#include <chaos/native_types.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <new>

#include "runtime_stubs/stub_common.h"
#include "runtime_stubs/string_stubs.h"

using namespace chaos::il2cpp::common;

// ═══════════════════════════════════════════════════════════════════════════
// Helpers: create test StubStringHeader objects via std::malloc
// ═══════════════════════════════════════════════════════════════════════════

/// Allocate a managed string header + data containing the given UTF-8 text.
/// Uses std::malloc (not GC) so the test has no GC dependency.
static CHAOS_IL2CPP_INTPTR make_test_string(const char* text) noexcept {
    if (text == nullptr) return 0;
    CHAOS_IL2CPP_UINTPTR len = static_cast<CHAOS_IL2CPP_UINTPTR>(std::strlen(text));
    auto* result = static_cast<StubStringHeader*>(
        std::malloc(sizeof(StubStringHeader) + len + 1));
    if (result == nullptr) return 0;
    result->type = 0;
    result->byte_count = len;
    if (len > 0) std::memcpy(result + 1, text, len);
    reinterpret_cast<char*>(result + 1)[len] = '\0';
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result);
}

/// Allocate an empty managed string (byte_count = 0).
static CHAOS_IL2CPP_INTPTR make_empty_string() noexcept {
    auto* result = static_cast<StubStringHeader*>(
        std::malloc(sizeof(StubStringHeader) + 1));
    if (result == nullptr) return 0;
    result->type = 0;
    result->byte_count = 0;
    reinterpret_cast<char*>(result + 1)[0] = '\0';
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result);
}

/// Allocate a managed array of CHAOS_IL2CPP_INTPTR elements (for JoinSs).
/// Uses the ManagedArrayAccessor contiguous layout (header + inline elements)
/// matching get_managed_array()/accessor_get_elements() in stub_common.h.
static CHAOS_IL2CPP_INTPTR make_test_array(CHAOS_IL2CPP_INTPTR* elems,
                                            CHAOS_IL2CPP_UINTPTR count) noexcept {
    auto* arr = static_cast<ManagedArrayAccessor*>(
        std::malloc(sizeof(ManagedArrayAccessor)
                    + count * sizeof(CHAOS_IL2CPP_INTPTR)));
    if (arr == nullptr) return 0;
    std::memset(arr, 0, sizeof(ManagedArrayAccessor));
    arr->length = static_cast<CHAOS_IL2CPP_INTPTR>(count);
    auto* element_data = accessor_get_elements(arr);
    for (CHAOS_IL2CPP_UINTPTR i = 0; i < count; ++i) element_data[i] = elems[i];
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(arr);
}

/// Allocate a StubArrayHeader-based inline array (for FormatArray, which
/// casts directly to StubArrayHeader* and reads inline elements).
static CHAOS_IL2CPP_INTPTR make_inline_array(CHAOS_IL2CPP_INTPTR* elems,
                                               CHAOS_IL2CPP_UINTPTR count) noexcept {
    auto* arr = static_cast<StubArrayHeader*>(
        std::malloc(sizeof(StubArrayHeader) + count * sizeof(CHAOS_IL2CPP_INTPTR)));
    if (arr == nullptr) return 0;
    arr->element_type = 0;
    arr->length = count;
    auto* dst = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(arr + 1);
    for (CHAOS_IL2CPP_UINTPTR i = 0; i < count; ++i) dst[i] = elems[i];
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(arr);
}

/// Free a test array and its element buffer (allocated by make_test_array).
static void free_test_array(CHAOS_IL2CPP_INTPTR handle) noexcept {
    if (handle == 0) return;
    auto* arr = reinterpret_cast<ManagedArrayAccessor*>(handle);
    std::free(arr);
}

/// Return UTF-8 data from a managed string handle.
static const char* str_data(CHAOS_IL2CPP_INTPTR handle) noexcept {
    if (handle == 0) return nullptr;
    const auto* hdr = reinterpret_cast<const StubStringHeader*>(handle);
    return reinterpret_cast<const char*>(hdr + 1);
}

/// Return byte_count from a managed string handle.
static CHAOS_IL2CPP_UINTPTR str_len(CHAOS_IL2CPP_INTPTR handle) noexcept {
    if (handle == 0) return 0;
    return reinterpret_cast<const StubStringHeader*>(handle)->byte_count;
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosStringContains
// ═══════════════════════════════════════════════════════════════════════════

TEST(StringStubs, Contains_Found) {
    auto str = make_test_string("hello world");
    auto val = make_test_string("world");
    ASSERT_NE(str, 0);
    ASSERT_NE(val, 0);
    EXPECT_EQ(ChaosStringContains(str, val), 1);
}

TEST(StringStubs, Contains_NotFound) {
    auto str = make_test_string("hello world");
    auto val = make_test_string("xyz");
    ASSERT_NE(str, 0);
    ASSERT_NE(val, 0);
    EXPECT_EQ(ChaosStringContains(str, val), 0);
}

TEST(StringStubs, Contains_EmptyHaystack) {
    auto str = make_empty_string();
    auto val = make_test_string("a");
    ASSERT_NE(str, 0);
    ASSERT_NE(val, 0);
    EXPECT_EQ(ChaosStringContains(str, val), 0);
}

TEST(StringStubs, Contains_EmptyNeedle) {
    auto str = make_test_string("abc");
    auto val = make_empty_string();
    ASSERT_NE(str, 0);
    ASSERT_NE(val, 0);
    EXPECT_EQ(ChaosStringContains(str, val), 1);
}

TEST(StringStubs, Contains_NullStr) {
    auto val = make_test_string("a");
    ASSERT_NE(val, 0);
    EXPECT_EQ(ChaosStringContains(0, val), 0);
}

TEST(StringStubs, Contains_NullValue) {
    auto str = make_test_string("a");
    ASSERT_NE(str, 0);
    EXPECT_EQ(ChaosStringContains(str, 0), 0);
}

TEST(StringStubs, Contains_NeedleLongerThanHaystack) {
    auto str = make_test_string("ab");
    auto val = make_test_string("abc");
    ASSERT_NE(str, 0);
    ASSERT_NE(val, 0);
    EXPECT_EQ(ChaosStringContains(str, val), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosStringStartsWith
// ═══════════════════════════════════════════════════════════════════════════

TEST(StringStubs, StartsWith_Yes) {
    auto str = make_test_string("hello world");
    auto val = make_test_string("hello");
    ASSERT_NE(str, 0);
    ASSERT_NE(val, 0);
    EXPECT_EQ(ChaosStringStartsWith(str, val), 1);
}

TEST(StringStubs, StartsWith_No) {
    auto str = make_test_string("hello world");
    auto val = make_test_string("world");
    ASSERT_NE(str, 0);
    ASSERT_NE(val, 0);
    EXPECT_EQ(ChaosStringStartsWith(str, val), 0);
}

TEST(StringStubs, StartsWith_ExactMatch) {
    auto str = make_test_string("abc");
    auto val = make_test_string("abc");
    ASSERT_NE(str, 0);
    ASSERT_NE(val, 0);
    EXPECT_EQ(ChaosStringStartsWith(str, val), 1);
}

TEST(StringStubs, StartsWith_EmptyValue) {
    auto str = make_test_string("abc");
    auto val = make_empty_string();
    ASSERT_NE(str, 0);
    ASSERT_NE(val, 0);
    EXPECT_EQ(ChaosStringStartsWith(str, val), 1);
}

TEST(StringStubs, StartsWith_NullStr) {
    auto val = make_test_string("a");
    ASSERT_NE(val, 0);
    EXPECT_EQ(ChaosStringStartsWith(0, val), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosStringEndsWith
// ═══════════════════════════════════════════════════════════════════════════

TEST(StringStubs, EndsWith_Yes) {
    auto str = make_test_string("hello world");
    auto val = make_test_string("world");
    ASSERT_NE(str, 0);
    ASSERT_NE(val, 0);
    EXPECT_EQ(ChaosStringEndsWith(str, val), 1);
}

TEST(StringStubs, EndsWith_No) {
    auto str = make_test_string("hello world");
    auto val = make_test_string("hello");
    ASSERT_NE(str, 0);
    ASSERT_NE(val, 0);
    EXPECT_EQ(ChaosStringEndsWith(str, val), 0);
}

TEST(StringStubs, EndsWith_ExactMatch) {
    auto str = make_test_string("abc");
    auto val = make_test_string("abc");
    ASSERT_NE(str, 0);
    ASSERT_NE(val, 0);
    EXPECT_EQ(ChaosStringEndsWith(str, val), 1);
}

TEST(StringStubs, EndsWith_NullStr) {
    auto val = make_test_string("a");
    ASSERT_NE(val, 0);
    EXPECT_EQ(ChaosStringEndsWith(0, val), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosStringIndexOf
// ═══════════════════════════════════════════════════════════════════════════

TEST(StringStubs, IndexOf_Found) {
    auto str = make_test_string("hello world");
    auto val = make_test_string("world");
    ASSERT_NE(str, 0);
    ASSERT_NE(val, 0);
    EXPECT_EQ(ChaosStringIndexOf(str, val), 6);
}

TEST(StringStubs, IndexOf_FoundAtStart) {
    auto str = make_test_string("abc");
    auto val = make_test_string("a");
    ASSERT_NE(str, 0);
    ASSERT_NE(val, 0);
    EXPECT_EQ(ChaosStringIndexOf(str, val), 0);
}

TEST(StringStubs, IndexOf_NotFound) {
    auto str = make_test_string("hello");
    auto val = make_test_string("xyz");
    ASSERT_NE(str, 0);
    ASSERT_NE(val, 0);
    EXPECT_EQ(ChaosStringIndexOf(str, val), -1);
}

TEST(StringStubs, IndexOf_EmptyNeedle) {
    auto str = make_test_string("abc");
    auto val = make_empty_string();
    ASSERT_NE(str, 0);
    ASSERT_NE(val, 0);
    EXPECT_EQ(ChaosStringIndexOf(str, val), 0);
}

TEST(StringStubs, IndexOf_NullStr) {
    auto val = make_test_string("a");
    ASSERT_NE(val, 0);
    EXPECT_EQ(ChaosStringIndexOf(0, val), -1);
}

TEST(StringStubs, IndexOf_NullValue) {
    auto str = make_test_string("a");
    ASSERT_NE(str, 0);
    EXPECT_EQ(ChaosStringIndexOf(str, 0), -1);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosStringCompare
// ═══════════════════════════════════════════════════════════════════════════

TEST(StringStubs, Compare_Equal) {
    auto a = make_test_string("abc");
    auto b = make_test_string("abc");
    ASSERT_NE(a, 0);
    ASSERT_NE(b, 0);
    EXPECT_EQ(ChaosStringCompare(a, b), 0);
}

TEST(StringStubs, Compare_ALessThanB) {
    auto a = make_test_string("abc");
    auto b = make_test_string("abd");
    ASSERT_NE(a, 0);
    ASSERT_NE(b, 0);
    EXPECT_LT(ChaosStringCompare(a, b), 0);
}

TEST(StringStubs, Compare_AGreaterThanB) {
    auto a = make_test_string("abd");
    auto b = make_test_string("abc");
    ASSERT_NE(a, 0);
    ASSERT_NE(b, 0);
    EXPECT_GT(ChaosStringCompare(a, b), 0);
}

TEST(StringStubs, Compare_ShorterPrefix) {
    auto a = make_test_string("ab");
    auto b = make_test_string("abc");
    ASSERT_NE(a, 0);
    ASSERT_NE(b, 0);
    EXPECT_LT(ChaosStringCompare(a, b), 0);
}

TEST(StringStubs, Compare_BothNull) {
    EXPECT_EQ(ChaosStringCompare(0, 0), 0);
}

TEST(StringStubs, Compare_ANullBNot) {
    auto b = make_test_string("a");
    ASSERT_NE(b, 0);
    EXPECT_LT(ChaosStringCompare(0, b), 0);
}

TEST(StringStubs, Compare_BNullANot) {
    auto a = make_test_string("a");
    ASSERT_NE(a, 0);
    EXPECT_GT(ChaosStringCompare(a, 0), 0);
}

TEST(StringStubs, Compare_EmptyBoth) {
    auto a = make_empty_string();
    auto b = make_empty_string();
    ASSERT_NE(a, 0);
    ASSERT_NE(b, 0);
    EXPECT_EQ(ChaosStringCompare(a, b), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosStringJoinSs
// ═══════════════════════════════════════════════════════════════════════════

TEST(StringStubs, JoinSs_TwoStrings) {
    auto sep = make_test_string(", ");
    CHAOS_IL2CPP_INTPTR elems[] = { make_test_string("a"), make_test_string("b") };
    auto arr = make_test_array(elems, 2);
    ASSERT_NE(sep, 0);
    ASSERT_NE(arr, 0);
    auto result = ChaosStringJoinSs(sep, arr);
    ASSERT_NE(result, 0);
    EXPECT_EQ(str_len(result), 4);
    EXPECT_STREQ(str_data(result), "a, b");
}

TEST(StringStubs, JoinSs_EmptySeparator) {
    auto sep = make_empty_string();
    CHAOS_IL2CPP_INTPTR elems[] = { make_test_string("a"), make_test_string("b"), make_test_string("c") };
    auto arr = make_test_array(elems, 3);
    ASSERT_NE(sep, 0);
    ASSERT_NE(arr, 0);
    auto result = ChaosStringJoinSs(sep, arr);
    ASSERT_NE(result, 0);
    EXPECT_EQ(str_len(result), 3);
    EXPECT_STREQ(str_data(result), "abc");
}

TEST(StringStubs, JoinSs_SingleElement) {
    auto sep = make_test_string(",");
    CHAOS_IL2CPP_INTPTR elems[] = { make_test_string("only") };
    auto arr = make_test_array(elems, 1);
    ASSERT_NE(sep, 0);
    ASSERT_NE(arr, 0);
    auto result = ChaosStringJoinSs(sep, arr);
    ASSERT_NE(result, 0);
    EXPECT_STREQ(str_data(result), "only");
}

TEST(StringStubs, JoinSs_NullSeparator) {
    CHAOS_IL2CPP_INTPTR elems[] = { make_test_string("a") };
    auto arr = make_test_array(elems, 1);
    ASSERT_NE(arr, 0);
    EXPECT_EQ(ChaosStringJoinSs(0, arr), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosStringSplit
// ═══════════════════════════════════════════════════════════════════════════

TEST(StringStubs, Split_Basic) {
    auto str = make_test_string("a,b,c");
    ASSERT_NE(str, 0);
    auto result = ChaosStringSplit(str, static_cast<CHAOS_IL2CPP_INTPTR>(','));
    ASSERT_NE(result, 0);
    auto* arr = reinterpret_cast<const StubArrayHeader*>(result);
    EXPECT_EQ(arr->length, 3);
    auto* elems = reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(arr + 1);
    EXPECT_STREQ(str_data(elems[0]), "a");
    EXPECT_STREQ(str_data(elems[1]), "b");
    EXPECT_STREQ(str_data(elems[2]), "c");
}

TEST(StringStubs, Split_NoSeparator) {
    auto str = make_test_string("abc");
    ASSERT_NE(str, 0);
    auto result = ChaosStringSplit(str, static_cast<CHAOS_IL2CPP_INTPTR>(','));
    ASSERT_NE(result, 0);
    auto* arr = reinterpret_cast<const StubArrayHeader*>(result);
    EXPECT_EQ(arr->length, 1);
    auto* elems = reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(arr + 1);
    EXPECT_STREQ(str_data(elems[0]), "abc");
}

TEST(StringStubs, Split_NullStr) {
    EXPECT_EQ(ChaosStringSplit(0, static_cast<CHAOS_IL2CPP_INTPTR>(',')), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosStringSubstring
// ═══════════════════════════════════════════════════════════════════════════

TEST(StringStubs, Substring_Normal) {
    auto str = make_test_string("hello world");
    ASSERT_NE(str, 0);
    auto result = ChaosStringSubstring(str, 6, 5);
    ASSERT_NE(result, 0);
    EXPECT_EQ(str_len(result), 5);
    EXPECT_STREQ(str_data(result), "world");
}

TEST(StringStubs, Substring_WholeString) {
    auto str = make_test_string("abc");
    ASSERT_NE(str, 0);
    auto result = ChaosStringSubstring(str, 0, 3);
    ASSERT_NE(result, 0);
    EXPECT_STREQ(str_data(result), "abc");
}

TEST(StringStubs, Substring_ZeroLength) {
    auto str = make_test_string("abc");
    ASSERT_NE(str, 0);
    auto result = ChaosStringSubstring(str, 1, 0);
    ASSERT_NE(result, 0);
    EXPECT_EQ(str_len(result), 0);
}

TEST(StringStubs, Substring_NullStr) {
    EXPECT_EQ(ChaosStringSubstring(0, 0, 5), 0);
}

TEST(StringStubs, Substring_OutOfBounds) {
    auto str = make_test_string("abc");
    ASSERT_NE(str, 0);
    EXPECT_EQ(ChaosStringSubstring(str, 10, 1), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosStringRemove
// ═══════════════════════════════════════════════════════════════════════════

TEST(StringStubs, Remove_Normal) {
    auto str = make_test_string("hello world");
    ASSERT_NE(str, 0);
    auto result = ChaosStringRemove(str, 5, 6);
    ASSERT_NE(result, 0);
    EXPECT_STREQ(str_data(result), "hello");
}

TEST(StringStubs, Remove_Middle) {
    auto str = make_test_string("abcdef");
    ASSERT_NE(str, 0);
    auto result = ChaosStringRemove(str, 2, 2);
    ASSERT_NE(result, 0);
    EXPECT_STREQ(str_data(result), "abef");
}

TEST(StringStubs, Remove_All) {
    auto str = make_test_string("abc");
    ASSERT_NE(str, 0);
    auto result = ChaosStringRemove(str, 0, 3);
    ASSERT_NE(result, 0);
    EXPECT_EQ(str_len(result), 0);
}

TEST(StringStubs, Remove_NullStr) {
    EXPECT_EQ(ChaosStringRemove(0, 0, 5), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosStringReplace
// ═══════════════════════════════════════════════════════════════════════════

TEST(StringStubs, Replace_Normal) {
    auto str = make_test_string("hello world world");
    auto old_val = make_test_string("world");
    auto new_val = make_test_string("there");
    ASSERT_NE(str, 0);
    ASSERT_NE(old_val, 0);
    ASSERT_NE(new_val, 0);
    auto result = ChaosStringReplace(str, old_val, new_val);
    ASSERT_NE(result, 0);
    EXPECT_STREQ(str_data(result), "hello there there");
}

TEST(StringStubs, Replace_NoMatch) {
    auto str = make_test_string("abc");
    auto old_val = make_test_string("x");
    auto new_val = make_test_string("y");
    ASSERT_NE(str, 0);
    ASSERT_NE(old_val, 0);
    ASSERT_NE(new_val, 0);
    auto result = ChaosStringReplace(str, old_val, new_val);
    ASSERT_NE(result, 0);
    EXPECT_STREQ(str_data(result), "abc");
}

TEST(StringStubs, Replace_EmptyOld) {
    auto str = make_test_string("abc");
    auto old_val = make_empty_string();
    auto new_val = make_test_string("x");
    ASSERT_NE(str, 0);
    ASSERT_NE(old_val, 0);
    ASSERT_NE(new_val, 0);
    auto result = ChaosStringReplace(str, old_val, new_val);
    ASSERT_NE(result, 0);
    EXPECT_STREQ(str_data(result), "abc");
}

TEST(StringStubs, Replace_NullInput) {
    auto old_val = make_test_string("a");
    auto new_val = make_test_string("b");
    ASSERT_NE(old_val, 0);
    ASSERT_NE(new_val, 0);
    EXPECT_EQ(ChaosStringReplace(0, old_val, new_val), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosStringToLower
// ═══════════════════════════════════════════════════════════════════════════

TEST(StringStubs, ToLower_Normal) {
    auto str = make_test_string("HELLO World!");
    ASSERT_NE(str, 0);
    auto result = ChaosStringToLower(str);
    ASSERT_NE(result, 0);
    EXPECT_STREQ(str_data(result), "hello world!");
}

TEST(StringStubs, ToLower_AlreadyLower) {
    auto str = make_test_string("hello");
    ASSERT_NE(str, 0);
    auto result = ChaosStringToLower(str);
    ASSERT_NE(result, 0);
    EXPECT_STREQ(str_data(result), "hello");
}

TEST(StringStubs, ToLower_Empty) {
    auto str = make_empty_string();
    ASSERT_NE(str, 0);
    auto result = ChaosStringToLower(str);
    ASSERT_NE(result, 0);
    EXPECT_EQ(str_len(result), 0);
}

TEST(StringStubs, ToLower_Null) {
    EXPECT_EQ(ChaosStringToLower(0), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosStringToUpper
// ═══════════════════════════════════════════════════════════════════════════

TEST(StringStubs, ToUpper_Normal) {
    auto str = make_test_string("Hello World!");
    ASSERT_NE(str, 0);
    auto result = ChaosStringToUpper(str);
    ASSERT_NE(result, 0);
    EXPECT_STREQ(str_data(result), "HELLO WORLD!");
}

TEST(StringStubs, ToUpper_Empty) {
    auto str = make_empty_string();
    ASSERT_NE(str, 0);
    auto result = ChaosStringToUpper(str);
    ASSERT_NE(result, 0);
    EXPECT_EQ(str_len(result), 0);
}

TEST(StringStubs, ToUpper_Null) {
    EXPECT_EQ(ChaosStringToUpper(0), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosStringTrim
// ═══════════════════════════════════════════════════════════════════════════

TEST(StringStubs, Trim_LeadingAndTrailing) {
    auto str = make_test_string("  hello world\t\n");
    ASSERT_NE(str, 0);
    auto result = ChaosStringTrim(str);
    ASSERT_NE(result, 0);
    EXPECT_STREQ(str_data(result), "hello world");
}

TEST(StringStubs, Trim_NoWhitespace) {
    auto str = make_test_string("hello");
    ASSERT_NE(str, 0);
    auto result = ChaosStringTrim(str);
    ASSERT_NE(result, 0);
    EXPECT_STREQ(str_data(result), "hello");
}

TEST(StringStubs, Trim_AllWhitespace) {
    auto str = make_test_string("   \t\n  ");
    ASSERT_NE(str, 0);
    auto result = ChaosStringTrim(str);
    ASSERT_NE(result, 0);
    EXPECT_EQ(str_len(result), 0);
}

TEST(StringStubs, Trim_Empty) {
    auto str = make_empty_string();
    ASSERT_NE(str, 0);
    auto result = ChaosStringTrim(str);
    ASSERT_NE(result, 0);
    EXPECT_EQ(str_len(result), 0);
}

TEST(StringStubs, Trim_Null) {
    EXPECT_EQ(ChaosStringTrim(0), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosStringFastAllocate
// ═══════════════════════════════════════════════════════════════════════════

TEST(StringStubs, FastAllocate_Normal) {
    auto result = ChaosStringFastAllocate(5);
    ASSERT_NE(result, 0);
    EXPECT_EQ(str_len(result), 5);
}

TEST(StringStubs, FastAllocate_Zero) {
    auto result = ChaosStringFastAllocate(0);
    ASSERT_NE(result, 0);
    EXPECT_EQ(str_len(result), 0);
}

TEST(StringStubs, FastAllocate_Negative) {
    EXPECT_EQ(ChaosStringFastAllocate(-1), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosStringFormatArray
// ═══════════════════════════════════════════════════════════════════════════

TEST(StringStubs, FormatArray_NoPlaceholders) {
    auto fmt = make_test_string("hello");
    ASSERT_NE(fmt, 0);
    auto result = ChaosStringFormatArray(fmt, 0);
    ASSERT_NE(result, 0);
    EXPECT_STREQ(str_data(result), "hello");
}

TEST(StringStubs, FormatArray_OneArg) {
    auto fmt = make_test_string("hello {0}");
    CHAOS_IL2CPP_INTPTR elems[] = { make_test_string("world") };
    auto arr = make_inline_array(elems, 1);
    ASSERT_NE(fmt, 0);
    ASSERT_NE(arr, 0);
    auto result = ChaosStringFormatArray(fmt, arr);
    ASSERT_NE(result, 0);
    EXPECT_STREQ(str_data(result), "hello world");
}

TEST(StringStubs, FormatArray_TwoArgs) {
    auto fmt = make_test_string("{0}, {1}!");
    CHAOS_IL2CPP_INTPTR elems[] = { make_test_string("hello"), make_test_string("world") };
    auto arr = make_inline_array(elems, 2);
    ASSERT_NE(fmt, 0);
    ASSERT_NE(arr, 0);
    auto result = ChaosStringFormatArray(fmt, arr);
    ASSERT_NE(result, 0);
    EXPECT_STREQ(str_data(result), "hello, world!");
}

TEST(StringStubs, FormatArray_EscapedBraces) {
    auto fmt = make_test_string("{{hello}}");
    ASSERT_NE(fmt, 0);
    auto result = ChaosStringFormatArray(fmt, 0);
    ASSERT_NE(result, 0);
    EXPECT_STREQ(str_data(result), "{hello}");
}

TEST(StringStubs, FormatArray_NullFormat) {
    EXPECT_EQ(ChaosStringFormatArray(0, 0), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosStringFormat1 / ChaosStringFormat2
// ═══════════════════════════════════════════════════════════════════════════

TEST(StringStubs, Format1) {
    auto fmt = make_test_string("arg={0}");
    auto arg = make_test_string("42");
    ASSERT_NE(fmt, 0);
    ASSERT_NE(arg, 0);
    auto result = ChaosStringFormat1(fmt, arg);
    ASSERT_NE(result, 0);
    EXPECT_STREQ(str_data(result), "arg=42");
}

TEST(StringStubs, Format2) {
    auto fmt = make_test_string("{0}+{1}");
    auto a0 = make_test_string("1");
    auto a1 = make_test_string("2");
    ASSERT_NE(fmt, 0);
    ASSERT_NE(a0, 0);
    ASSERT_NE(a1, 0);
    auto result = ChaosStringFormat2(fmt, a0, a1);
    ASSERT_NE(result, 0);
    EXPECT_STREQ(str_data(result), "1+2");
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosStringAppend
// ═══════════════════════════════════════════════════════════════════════════

TEST(StringStubs, Append_Normal) {
    // Create a builder buffer (empty string, pre-allocated capacity).
    auto builder = ChaosStringFastAllocate(10);
    ASSERT_NE(builder, 0);
    auto* hdr = reinterpret_cast<StubStringHeader*>(builder);
    hdr->byte_count = 0;  // start empty

    auto str = make_test_string("hello");
    ASSERT_NE(str, 0);
    ChaosStringAppend(builder, str, 0);
    EXPECT_EQ(hdr->byte_count, 5);
    EXPECT_STREQ(reinterpret_cast<const char*>(hdr + 1), "hello");
}

TEST(StringStubs, Append_NullBuilder) {
    auto str = make_test_string("hello");
    ASSERT_NE(str, 0);
    ChaosStringAppend(0, str, 0);  // must not crash
}

TEST(StringStubs, Append_NullStr) {
    auto builder = make_empty_string();
    ASSERT_NE(builder, 0);
    ChaosStringAppend(builder, 0, 0);  // must not crash
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosFormattablestringFactoryCreate
// ═══════════════════════════════════════════════════════════════════════════

TEST(StringStubs, FormattableStringFactoryCreate_Normal) {
    auto fmt = make_test_string("hello {0}");
    auto arg = make_test_string("world");
    ASSERT_NE(fmt, 0);
    ASSERT_NE(arg, 0);
    CHAOS_IL2CPP_INTPTR args[] = { arg };
    auto arr = make_test_array(args, 1);
    ASSERT_NE(arr, 0);
    auto result = ChaosFormattablestringFactoryCreate(fmt, arr);
    ASSERT_NE(result, 0);
    // V1 returns format string unchanged.
    EXPECT_STREQ(str_data(result), "hello {0}");
}

TEST(StringStubs, FormattableStringFactoryCreate_NullFormat) {
    EXPECT_EQ(ChaosFormattablestringFactoryCreate(0, 0), 0);
}
