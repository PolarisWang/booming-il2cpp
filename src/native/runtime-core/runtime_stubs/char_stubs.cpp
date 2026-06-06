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
static inline CHAOS_IL2CPP_INT32 LookupCategory(CHAOS_IL2CPP_INT32 c) noexcept
{
    if (c < 0 || c > 0xFFFF) return -1;  // Only BMP supported
    auto cp = static_cast<CHAOS_IL2CPP_UINT16>(c);
    for (CHAOS_IL2CPP_INT32 i = 0; i < kUnicodeCategoryRangeCount; i++) {
        if (cp >= kUnicodeCategoryRanges[i].start && cp <= kUnicodeCategoryRanges[i].end)
            return static_cast<CHAOS_IL2CPP_INT32>(kUnicodeCategoryRanges[i].category);
    }
    return -1;
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
CHAOS_IL2CPP_FLOAT64 ChaosCharUnicodeInfoGetNumericValue(CHAOS_IL2CPP_INT32 c) noexcept
{
    if (c < 0 || c > 0xFFFF) return -1.0;
    auto cp = static_cast<CHAOS_IL2CPP_UINT16>(c);

    // Check decimal digit table first
    for (CHAOS_IL2CPP_INT32 i = 0; i < kUnicodeDecimalDigitCount; i++) {
        if (kUnicodeDecimalDigitTable[i].codepoint == cp)
            return static_cast<CHAOS_IL2CPP_FLOAT64>(kUnicodeDecimalDigitTable[i].value);
    }

    // Check numeric value table
    for (CHAOS_IL2CPP_INT32 i = 0; i < kUnicodeNumericCount; i++) {
        if (kUnicodeNumericTable[i].codepoint == cp)
            return static_cast<CHAOS_IL2CPP_FLOAT64>(kUnicodeNumericTable[i].value);
    }

    return -1.0;  // No numeric value
}

// ═══════════════════════════════════════════════════════════════
// CharUnicodeInfo.GetDigitValue(char) — ASCII digits + Nd digits
// ═══════════════════════════════════════════════════════════════
CHAOS_IL2CPP_INT32 ChaosCharUnicodeInfoGetDigitValue(CHAOS_IL2CPP_INT32 c) noexcept
{
    if (c < 0 || c > 0xFFFF) return -1;
    auto cp = static_cast<CHAOS_IL2CPP_UINT16>(c);

    for (CHAOS_IL2CPP_INT32 i = 0; i < kUnicodeDecimalDigitCount; i++) {
        if (kUnicodeDecimalDigitTable[i].codepoint == cp)
            return static_cast<CHAOS_IL2CPP_INT32>(kUnicodeDecimalDigitTable[i].value);
    }

    return -1;
}

// ═══════════════════════════════════════════════════════════════
// CharUnicodeInfo.GetDecimalDigitValue(char) — same as GetDigitValue
// ═══════════════════════════════════════════════════════════════
CHAOS_IL2CPP_INT32 ChaosCharUnicodeInfoGetDecimalDigitValue(CHAOS_IL2CPP_INT32 c) noexcept
{
    if (c < 0 || c > 0xFFFF) return -1;
    auto cp = static_cast<CHAOS_IL2CPP_UINT16>(c);

    for (CHAOS_IL2CPP_INT32 i = 0; i < kUnicodeDecimalDigitCount; i++) {
        if (kUnicodeDecimalDigitTable[i].codepoint == cp)
            return static_cast<CHAOS_IL2CPP_INT32>(kUnicodeDecimalDigitTable[i].value);
    }

    return -1;
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
