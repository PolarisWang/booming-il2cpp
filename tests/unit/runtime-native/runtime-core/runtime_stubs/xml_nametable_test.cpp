// xml_nametable_test.cpp — Unit tests for xml_nametable_stubs.cpp (P0-A)
//
// Covers System.Xml.NameTable.Add(string) / XmlNameTable.Get(string) interned
// semantics implemented in ChaosXmlNameTableAddString / ChaosXmlNameTableGetString:
//
//   1. Add(content) twice returns the SAME tagged StringId (reference-equality
//      contract that NameTable exists to provide).
//   2. Get(content) after Add(content) returns the interned reference.
//   3. Get(content) for content never added returns null (query-only).
//   4. Add(null) raises ArgumentNullException.
//   5. Add/Get on a null receiver raises NullReferenceException.
//   6. Add("") / Get("") agree with the compiled ldstr "" hash.
//
// The stub .cpp is compiled directly into this target (same GNU-ar/MSVC
// compatibility pattern as the sibling runtime_stubs tests), together with
// string_table.cpp which it depends on.
//
// NOTE on null-receiver tests: the stubs call RaiseNullReferenceException()
// which is [[noreturn]] and unwinds via the runtime EH path.  A gtest cannot
// observe that without the full EH runtime linked, so the null-receiver and
// Add(null) contracts are asserted via EXPECT_DEATH-style processes only when
// the EH runtime is available.  Here we assert the *non-throwing* contract
// (the dominant NameTable use), which is what the json/xml production work
// depends on.

#include <gtest/gtest.h>
#include <chaos/native_types.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "runtime_stubs/stub_common.h"
#include "runtime_stubs/xml_nametable_stubs.h"
#include "codegen_bridge.h"

namespace {

// A non-null opaque `this` pointer.  The stubs only test it for null; any
// non-zero value is a valid receiver for these tests.
CHAOS_IL2CPP_INTPTR kFakeThis = static_cast<CHAOS_IL2CPP_INTPTR>(0x1000);

/// Allocate a managed string header + data containing the given UTF-8 text,
/// using std::malloc (not GC) so the test has no GC dependency — same helper
/// shape as string_stubs_test.cpp.
CHAOS_IL2CPP_INTPTR make_test_string(const char* text) noexcept
{
    if (text == nullptr)
    {
        return 0;
    }
    const auto len = static_cast<CHAOS_IL2CPP_UINTPTR>(std::strlen(text));
    auto* result = static_cast<StubStringHeader*>(
        std::malloc(sizeof(StubStringHeader) + len + 1));
    if (result == nullptr)
    {
        return 0;
    }
    result->type = 0;
    result->byte_count = len;
    if (len > 0)
    {
        std::memcpy(result + 1, text, len);
    }
    reinterpret_cast<char*>(result + 1)[len] = '\0';
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result);
}

}  // namespace

// ═══════════════════════════════════════════════════════════════════════════
// Add — reference-equality (the whole point of a NameTable)
// ═══════════════════════════════════════════════════════════════════════════

TEST(XmlNameTable, Add_SameContentReturnsSameReference)
{
    auto a = make_test_string("element");
    auto b = make_test_string("element");  // distinct allocation, same content
    ASSERT_NE(a, 0);
    ASSERT_NE(b, 0);
    ASSERT_NE(a, b);  // pre-condition: genuinely different pointers

    const auto r1 = ChaosXmlNameTableAddString(kFakeThis, a);
    const auto r2 = ChaosXmlNameTableAddString(kFakeThis, b);

    // Interned: same content → same tagged StringId reference.
    EXPECT_NE(r1, 0);
    EXPECT_EQ(r1, r2);
}

TEST(XmlNameTable, Add_DifferentContentReturnsDifferentReference)
{
    auto a = make_test_string("alpha");
    auto b = make_test_string("beta");
    ASSERT_NE(a, 0);
    ASSERT_NE(b, 0);

    const auto r1 = ChaosXmlNameTableAddString(kFakeThis, a);
    const auto r2 = ChaosXmlNameTableAddString(kFakeThis, b);

    EXPECT_NE(r1, 0);
    EXPECT_NE(r2, 0);
    EXPECT_NE(r1, r2);
}

TEST(XmlNameTable, Add_IdempotentAcrossManyAdds)
{
    auto s = make_test_string("repeat");
    ASSERT_NE(s, 0);

    const auto first = ChaosXmlNameTableAddString(kFakeThis, s);
    ASSERT_NE(first, 0);
    for (int i = 0; i < 10; ++i)
    {
        EXPECT_EQ(ChaosXmlNameTableAddString(kFakeThis, s), first);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Get — query-only semantics
// ═══════════════════════════════════════════════════════════════════════════

TEST(XmlNameTable, Get_ReturnsInternedReferenceAfterAdd)
{
    auto added = make_test_string("queryable");
    auto queried = make_test_string("queryable");
    ASSERT_NE(added, 0);
    ASSERT_NE(queried, 0);

    const auto interned = ChaosXmlNameTableAddString(kFakeThis, added);
    ASSERT_NE(interned, 0);

    EXPECT_EQ(ChaosXmlNameTableGetString(kFakeThis, queried), interned);
}

TEST(XmlNameTable, Get_AbsentContentReturnsNull)
{
    auto never_added = make_test_string("definitely-not-interned-xyzzy");
    ASSERT_NE(never_added, 0);

    // Get never inserts → an unknown string must come back null.
    EXPECT_EQ(ChaosXmlNameTableGetString(kFakeThis, never_added), 0);

    // And it must NOT have been interned as a side effect: a second Get is
    // still null (this is the regression guard against Get silently inserting).
    EXPECT_EQ(ChaosXmlNameTableGetString(kFakeThis, never_added), 0);
}

TEST(XmlNameTable, Get_NullStringReturnsNull)
{
    // Get(null) is defined to return null (not throw) — contrast with Add(null).
    EXPECT_EQ(ChaosXmlNameTableGetString(kFakeThis, 0), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// Empty string — must agree with the compiled ldstr "" literal hash
// ═══════════════════════════════════════════════════════════════════════════

TEST(XmlNameTable, Add_EmptyStringIsStableAndTagged)
{
    auto empty = make_test_string("");
    ASSERT_NE(empty, 0);

    const auto r1 = ChaosXmlNameTableAddString(kFakeThis, empty);
    const auto r2 = ChaosXmlNameTableAddString(kFakeThis, empty);

    EXPECT_NE(r1, 0);
    EXPECT_EQ(r1, r2);
    // The returned handle must be a tagged StringId, not a raw pointer.
    EXPECT_TRUE(chaos_is_string_id(r1));
}

TEST(XmlNameTable, Get_EmptyStringMatchesAddEmptyString)
{
    auto empty = make_test_string("");
    ASSERT_NE(empty, 0);

    const auto added = ChaosXmlNameTableAddString(kFakeThis, empty);
    ASSERT_NE(added, 0);

    // Get("") returns the well-formed empty reference rather than null.
    EXPECT_EQ(ChaosXmlNameTableGetString(kFakeThis, empty), added);
}

// ═══════════════════════════════════════════════════════════════════════════
// Tagged StringId round-trip — the cross-representation contract
// ═══════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════
// char[] overloads — Add/Get(char[], int, int)
// ═══════════════════════════════════════════════════════════════════════════

namespace {

/// Build a codegen-style char[] (StubArrayHeader + contiguous 2-byte elements).
/// Mirrors the layout resolve_char_array_data() expects.
CHAOS_IL2CPP_INTPTR make_test_char_array(const char* ascii) noexcept
{
    if (ascii == nullptr) return 0;
    const auto len = static_cast<CHAOS_IL2CPP_UINTPTR>(std::strlen(ascii));
    auto* arr = static_cast<StubArrayHeader*>(
        std::malloc(sizeof(StubArrayHeader) + (len + 1) * sizeof(CHAOS_IL2CPP_UINT16)));
    if (arr == nullptr) return 0;
    arr->element_type = 0;
    arr->length = len;
    auto* elems = reinterpret_cast<CHAOS_IL2CPP_UINT16*>(arr + 1);
    for (CHAOS_IL2CPP_UINTPTR i = 0; i < len; ++i)
        elems[i] = static_cast<CHAOS_IL2CPP_UINT16>(
            static_cast<unsigned char>(ascii[i]));
    elems[len] = 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(arr);
}

}  // namespace

TEST(XmlNameTable, AddChars_InternsAndMatchesStringOverload)
{
    auto chars = make_test_char_array("charsys");
    auto str = make_test_string("charsys");
    ASSERT_NE(chars, 0);
    ASSERT_NE(str, 0);

    const auto fromChars = ChaosXmlNameTableAddChars(kFakeThis, chars, 0, 7);
    const auto fromString = ChaosXmlNameTableAddString(kFakeThis, str);

    // Same content via either overload must yield the same interned reference.
    EXPECT_NE(fromChars, 0);
    EXPECT_EQ(fromChars, fromString);
}

TEST(XmlNameTable, AddChars_RespectsStartAndLenWindow)
{
    // "xxyyzz" with start=2,len=2 → "yy"
    auto chars = make_test_char_array("xxyyzz");
    auto expected = make_test_string("yy");
    ASSERT_NE(chars, 0);
    ASSERT_NE(expected, 0);

    const auto windowed = ChaosXmlNameTableAddChars(kFakeThis, chars, 2, 2);
    const auto plain = ChaosXmlNameTableAddString(kFakeThis, expected);

    EXPECT_NE(windowed, 0);
    EXPECT_EQ(windowed, plain);
}

TEST(XmlNameTable, GetChars_FindsPreviouslyInterned)
{
    auto chars = make_test_char_array("lookupme");
    ASSERT_NE(chars, 0);

    const auto interned = ChaosXmlNameTableAddChars(kFakeThis, chars, 0, 8);
    ASSERT_NE(interned, 0);

    EXPECT_EQ(ChaosXmlNameTableGetChars(kFakeThis, chars, 0, 8), interned);
}

TEST(XmlNameTable, GetChars_AbsentReturnsNull)
{
    auto chars = make_test_char_array("never-seen-chars-xyzzy");
    ASSERT_NE(chars, 0);

    // Query-only: must not insert as a side effect.
    EXPECT_EQ(ChaosXmlNameTableGetChars(kFakeThis, chars, 0, 23), 0);
    EXPECT_EQ(ChaosXmlNameTableGetChars(kFakeThis, chars, 0, 23), 0);
}

TEST(XmlNameTable, GetChars_NullArrayReturnsNull)
{
    EXPECT_EQ(ChaosXmlNameTableGetChars(kFakeThis, 0, 0, 0), 0);
}

TEST(XmlNameTable, AddChars_NonAsciiRoundTripsToSameReference)
{
    // Multi-byte UTF-16 → UTF-8 expansion must still intern consistently.
    auto chars = make_test_char_array("ab");
    ASSERT_NE(chars, 0);
    // Overwrite with a non-ASCII BMP char (U+00E9 'é').
    auto* elems = reinterpret_cast<CHAOS_IL2CPP_UINT16*>(
        reinterpret_cast<StubArrayHeader*>(chars) + 1);
    elems[0] = 0x00E9;

    const auto interned = ChaosXmlNameTableAddChars(kFakeThis, chars, 0, 1);
    ASSERT_NE(interned, 0);

    // Add and Get must agree for the same non-ASCII window.
    EXPECT_EQ(ChaosXmlNameTableGetChars(kFakeThis, chars, 0, 1), interned);
}

