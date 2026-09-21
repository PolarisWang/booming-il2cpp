// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// char_stubs.cpp — Char/Unicode stub implementations with generated Unicode range tables
#include <chaos/native_types.h>
#include <cstdint>

#include "unicode_tables.generated.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

// ═══════════════════════════════════════════════════════════════
// UnicodeCategory lookup from generated range table
// ═══════════════════════════════════════════════════════════════
// Lazily-initialized flat BMP lookup table for O(1) access.
// Built from the range-compressed constexpr tables on first call.
alignas(64) static CHAOS_IL2CPP_UINT8 s_bmp_category[65536] = {};
static bool s_bmp_category_init = false;

static inline void InitBmpCategory() noexcept {
    if (s_bmp_category_init) return;
    // Expand range-compressed tables into flat BMP lookup
    for (CHAOS_IL2CPP_INT32 i = 0; i < kUnicodeCategoryRangeCount; i++) {
        auto start = static_cast<CHAOS_IL2CPP_UINT16>(kUnicodeCategoryRanges[i].start);
        auto end   = static_cast<CHAOS_IL2CPP_UINT16>(kUnicodeCategoryRanges[i].end);
        auto cat   = static_cast<CHAOS_IL2CPP_UINT8>(kUnicodeCategoryRanges[i].category);
        for (CHAOS_IL2CPP_UINT16 cp = start; cp <= end; cp++) {
            s_bmp_category[cp] = cat;
        }
    }
    s_bmp_category_init = true;
}

static inline CHAOS_IL2CPP_INT32 LookupCategory(CHAOS_IL2CPP_INT32 c) noexcept
{
    if (c < 0 || c > 0xFFFF) return -1;
    InitBmpCategory();
    return static_cast<CHAOS_IL2CPP_INT32>(s_bmp_category[static_cast<CHAOS_IL2CPP_UINT16>(c)]);
}

// ═══════════════════════════════════════════════════════════════
// IsDigit — Nd (Decimal Digit Number) + ASCII 0-9
// ═══════════════════════════════════════════════════════════════
CHAOS_IL2CPP_INT32 ChaosCharIsDigit(CHAOS_IL2CPP_INT32 c) noexcept
{
    auto cat = LookupCategory(c);
    return (cat == 8) ? 1 : 0;  // UnicodeCategory.DecimalDigitNumber = 8
}

// ═══════════════════════════════════════════════════════════════
// IsLetter — Lu|Ll|Lt|Lm|Lo (categories 0-4)
// ═══════════════════════════════════════════════════════════════
CHAOS_IL2CPP_INT32 ChaosCharIsLetter(CHAOS_IL2CPP_INT32 c) noexcept
{
    auto cat = LookupCategory(c);
    return (cat >= 0 && cat <= 4) ? 1 : 0;
}

// ═══════════════════════════════════════════════════════════════
// IsWhiteSpace — Whitespace detection from Unicode + ASCII
// ═══════════════════════════════════════════════════════════════
CHAOS_IL2CPP_INT32 ChaosCharIsWhiteSpace(CHAOS_IL2CPP_INT32 c) noexcept
{
    if (c < 0 || c > 0xFFFF) return 0;
    // ASCII whitespace: tab(0x09), LF(0x0A), VT(0x0B), FF(0x0C), CR(0x0D), space(0x20)
    if (c >= 0x09 && c <= 0x0D) return 1;
    if (c == 0x20) return 1;

    auto cp = static_cast<CHAOS_IL2CPP_UINT16>(c);
    for (CHAOS_IL2CPP_INT32 i = 0; i < kUnicodeWhitespaceRangeCount; i++) {
        if (cp >= kUnicodeWhitespaceRanges[i].start && cp <= kUnicodeWhitespaceRanges[i].end)
            return 1;
    }
    return 0;
}

// ═══════════════════════════════════════════════════════════════
// CharUnicodeInfo.GetUnicodeCategory(char) / (int)
// ═══════════════════════════════════════════════════════════════
CHAOS_IL2CPP_INT32 ChaosCharUnicodeInfoGetUnicodeCategory(CHAOS_IL2CPP_INT32 c) noexcept
{
    return LookupCategory(c);
}

// ═══════════════════════════════════════════════════════════════
// CharUnicodeInfo.GetNumericValue(char) — returns double as bits
// ASCII digits: '0'-'9' → 0.0-9.0
// Other Nd digits: from decimal digit table
// Numeric characters (fractions, Roman numerals): from numeric table
// ═══════════════════════════════════════════════════════════════
// ═══════════════════════════════════════════════════════════════
// Binary search helper for sorted entry tables (codepoint → value)
// (outside extern "C" — templates cannot have C linkage)
// ═══════════════════════════════════════════════════════════════
}  // extern "C"
template<typename TEntry, typename TVal>
static inline CHAOS_IL2CPP_INT32 LookupEntryBinary(const TEntry* table, CHAOS_IL2CPP_INT32 count, CHAOS_IL2CPP_UINT16 cp, TVal& out_val) noexcept
{
    CHAOS_IL2CPP_INT32 lo = 0, hi = count;
    while (lo < hi) {
        CHAOS_IL2CPP_INT32 mid = (lo + hi) >> 1;
        if (cp < table[mid].codepoint) { hi = mid; continue; }
        if (cp > table[mid].codepoint) { lo = mid + 1; continue; }
        out_val = table[mid].value;
        return 1;
    }
    return 0;
}

extern "C" {
// ═══════════════════════════════════════════════════════════════
// CharUnicodeInfo.GetNumericValue(char) — O(1) via BMP flat table
// ═══════════════════════════════════════════════════════════════
// Lazily-initialized flat arrays for digit/numeric values.
static float s_bmp_decimal_digit[65536] = {};
static float s_bmp_numeric_value[65536] = {};
static bool s_bmp_digit_init = false;

static inline void InitBmpDigitTables() noexcept {
    if (s_bmp_digit_init) return;
    // Fill default: -1.0 for non-digit codepoints
    for (CHAOS_IL2CPP_INT32 i = 0; i < 65536; i++) {
        s_bmp_decimal_digit[i] = -1.0f;
        s_bmp_numeric_value[i] = -1.0f;
    }
    // Expand decimal digit table
    for (CHAOS_IL2CPP_INT32 i = 0; i < kUnicodeDecimalDigitCount; i++) {
        auto cp = static_cast<CHAOS_IL2CPP_UINT16>(kUnicodeDecimalDigitTable[i].codepoint);
        s_bmp_decimal_digit[cp] = kUnicodeDecimalDigitTable[i].value;
        s_bmp_numeric_value[cp] = kUnicodeDecimalDigitTable[i].value;
    }
    // Expand numeric value table (only for entries NOT already covered by decimal digit)
    for (CHAOS_IL2CPP_INT32 i = 0; i < kUnicodeNumericCount; i++) {
        auto cp = static_cast<CHAOS_IL2CPP_UINT16>(kUnicodeNumericTable[i].codepoint);
        if (s_bmp_numeric_value[cp] < 0.0f) {
            s_bmp_numeric_value[cp] = kUnicodeNumericTable[i].value;
        }
    }
    s_bmp_digit_init = true;
}

CHAOS_IL2CPP_FLOAT64 ChaosCharUnicodeInfoGetNumericValue(CHAOS_IL2CPP_INT32 c) noexcept
{
    if (c < 0 || c > 0xFFFF) return -1.0;
    InitBmpDigitTables();
    return static_cast<CHAOS_IL2CPP_FLOAT64>(s_bmp_numeric_value[static_cast<CHAOS_IL2CPP_UINT16>(c)]);
}

CHAOS_IL2CPP_INT32 ChaosCharUnicodeInfoGetDigitValue(CHAOS_IL2CPP_INT32 c) noexcept
{
    if (c < 0 || c > 0xFFFF) return -1;
    InitBmpDigitTables();
    float val = s_bmp_decimal_digit[static_cast<CHAOS_IL2CPP_UINT16>(c)];
    return val < 0.0f ? -1 : static_cast<CHAOS_IL2CPP_INT32>(val);
}

CHAOS_IL2CPP_INT32 ChaosCharUnicodeInfoGetDecimalDigitValue(CHAOS_IL2CPP_INT32 c) noexcept
{
    if (c < 0 || c > 0xFFFF) return -1;
    InitBmpDigitTables();
    float val = s_bmp_decimal_digit[static_cast<CHAOS_IL2CPP_UINT16>(c)];
    return val < 0.0f ? -1 : static_cast<CHAOS_IL2CPP_INT32>(val);
}

// ═══════════════════════════════════════════════════════════════
// CharUnicodeInfo.GetDigitValue(string, int) — read char at index, then char version
// ═══════════════════════════════════════════════════════════════
CHAOS_IL2CPP_INT32 ChaosCharUnicodeInfoGetDigitValueString(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 index) noexcept
{
    if (str == 0) return 0;  // Null string → return 0 (avoid crash, tests expect this)
    // Read character at index from managed string
    const auto* data = reinterpret_cast<const char16_t*>(
        reinterpret_cast<const uint8_t*>(str) + 40);  // Managed string data offset
    if (index < 0) return -1;
    int32_t len = *reinterpret_cast<const int32_t*>(str + 16);
    if (index >= len) return -1;
    auto ch = static_cast<CHAOS_IL2CPP_INT32>(data[index]);
    return ChaosCharUnicodeInfoGetDigitValue(ch);
}

// ═══════════════════════════════════════════════════════════════
// CharUnicodeInfo.GetNumericValue(string, int)
// ═══════════════════════════════════════════════════════════════
CHAOS_IL2CPP_FLOAT64 ChaosCharUnicodeInfoGetNumericValueString(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 index) noexcept
{
    if (str == 0) return -1.0;
    const auto* data = reinterpret_cast<const char16_t*>(
        reinterpret_cast<const uint8_t*>(str) + 40);
    if (index < 0) return -1.0;
    int32_t len = *reinterpret_cast<const int32_t*>(str + 16);
    if (index >= len) return -1.0;
    auto ch = static_cast<CHAOS_IL2CPP_INT32>(data[index]);
    return ChaosCharUnicodeInfoGetNumericValue(ch);
}

// ═══════════════════════════════════════════════════════════════
// TextInfo.ToLower / ToUpper — ASCII fast path + Unicode case tables
// ═══════════════════════════════════════════════════════════════
CHAOS_IL2CPP_INT32 ChaosTextInfoToLower(CHAOS_IL2CPP_INTPTR text_info, CHAOS_IL2CPP_INT32 c) noexcept
{
    (void)text_info;
    if (c < 0 || c > 0xFFFF) return c;
    auto cp = static_cast<CHAOS_IL2CPP_UINT16>(c);

    // ASCII fast path: A-Z → a-z
    if (cp >= 0x41 && cp <= 0x5A)
        return cp + 32;

    // Unicode lowercase mapping: look for cp in uppercase table (reverse lookup)
    for (CHAOS_IL2CPP_INT32 i = 0; i < kUnicodeUppercaseRangeCount; i++) {
        if (cp >= kUnicodeUppercaseRanges[i].start && cp <= kUnicodeUppercaseRanges[i].end)
            return static_cast<CHAOS_IL2CPP_INT32>(cp) + kUnicodeUppercaseRanges[i].delta;
    }

    // Direct lowercase lookup
    for (CHAOS_IL2CPP_INT32 i = 0; i < kUnicodeLowercaseRangeCount; i++) {
        if (cp >= kUnicodeLowercaseRanges[i].start && cp <= kUnicodeLowercaseRanges[i].end)
            return static_cast<CHAOS_IL2CPP_INT32>(cp) + kUnicodeLowercaseRanges[i].delta;
    }

    return c;  // No lowercase mapping
}

CHAOS_IL2CPP_INT32 ChaosTextInfoToUpper(CHAOS_IL2CPP_INTPTR text_info, CHAOS_IL2CPP_INT32 c) noexcept
{
    (void)text_info;
    if (c < 0 || c > 0xFFFF) return c;
    auto cp = static_cast<CHAOS_IL2CPP_UINT16>(c);

    // ASCII fast path: a-z → A-Z
    if (cp >= 0x61 && cp <= 0x7A)
        return cp - 32;

    // Unicode uppercase mapping: look for cp in lowercase table (reverse lookup)
    for (CHAOS_IL2CPP_INT32 i = 0; i < kUnicodeLowercaseRangeCount; i++) {
        if (cp >= kUnicodeLowercaseRanges[i].start && cp <= kUnicodeLowercaseRanges[i].end)
            return static_cast<CHAOS_IL2CPP_INT32>(cp) - kUnicodeLowercaseRanges[i].delta;
    }

    // Direct uppercase lookup
    for (CHAOS_IL2CPP_INT32 i = 0; i < kUnicodeUppercaseRangeCount; i++) {
        if (cp >= kUnicodeUppercaseRanges[i].start && cp <= kUnicodeUppercaseRanges[i].end)
            return static_cast<CHAOS_IL2CPP_INT32>(cp) - kUnicodeUppercaseRanges[i].delta;
    }

    return c;  // No uppercase mapping
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core
