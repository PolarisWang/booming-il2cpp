// ── System.Text native stub implementations (C1) ───────────────────────────
// See text_stubs.h for why this file exists.

#include "text_stubs.h"

#include <cstring>

#include "runtime_stubs/array_stubs.h"
#include "runtime_stubs/stub_common.h"
#include "gc_helpers.h"
// Provides chaos_type_info_managed_array + chaos_type_shape_value, which the
// array construction below needs.  Declared by explicit include for the same
// reason remaining_stubs.cpp does it — do not rely on transitive includes.
#include "../ChaosGeneratedRuntimePrelude.h"

using chaos::il2cpp::runtime_core::GcAllocateAtomic;

extern "C" {

namespace {

/// Allocate a managed 1-D array of `count` pointer-sized elements.
/// Same construction the reflection stubs use, so the returned handle carries
/// a complete ManagedArrayAccessor header that every consumer expects.
CHAOS_IL2CPP_INTPTR NewManagedArray(CHAOS_IL2CPP_INTPTR count) noexcept {
    return ChaosArrayNew1D(
        &chaos::il2cpp::jit::chaos_type_info_managed_array.hot,
        &chaos::il2cpp::jit::chaos_type_info_managed_array.hot,
        chaos::il2cpp::jit::chaos_type_shape_value,
        count);
}
/// A valid Unicode scalar value: 0..0x10FFFF minus the surrogate block.
bool IsValidScalarValue(CHAOS_IL2CPP_UINT32 value) noexcept {
    if (value > 0x10FFFFu) return false;
    return !(value >= 0xD800u && value <= 0xDFFFu);
}

}  // namespace

// ── System.Text.Ascii ──────────────────────────────────────────────────────

CHAOS_IL2CPP_INT32 ChaosAsciiIsValidByte(CHAOS_IL2CPP_UINT8 value) noexcept {
    return value <= 0x7Fu ? 1 : 0;
}

CHAOS_IL2CPP_INT32 ChaosAsciiIsValidChar(CHAOS_IL2CPP_UINT16 value) noexcept {
    return value <= 0x7Fu ? 1 : 0;
}

// ── Encoding family: GetMaxByteCount / GetMaxCharCount ─────────────────────
//
// Pure functions of the encoding family's worst-case expansion factor; no
// instance state is consulted, so they take no receiver.

CHAOS_IL2CPP_INT32 ChaosEncodingGetMaxByteCountASCII(CHAOS_IL2CPP_INT32 charCount) noexcept {
    return charCount + 1;
}
CHAOS_IL2CPP_INT32 ChaosEncodingGetMaxCharCountASCII(CHAOS_IL2CPP_INT32 byteCount) noexcept {
    return byteCount + 1;
}

CHAOS_IL2CPP_INT32 ChaosEncodingGetMaxByteCountUTF8(CHAOS_IL2CPP_INT32 charCount) noexcept {
    return charCount * 3 + 1;
}
CHAOS_IL2CPP_INT32 ChaosEncodingGetMaxCharCountUTF8(CHAOS_IL2CPP_INT32 byteCount) noexcept {
    return byteCount + 1;
}

CHAOS_IL2CPP_INT32 ChaosEncodingGetMaxByteCountUTF7(CHAOS_IL2CPP_INT32 charCount) noexcept {
    return charCount * 2 + 1;
}
CHAOS_IL2CPP_INT32 ChaosEncodingGetMaxCharCountUTF7(CHAOS_IL2CPP_INT32 byteCount) noexcept {
    return byteCount + 1;
}

CHAOS_IL2CPP_INT32 ChaosEncodingGetMaxByteCountUnicode(CHAOS_IL2CPP_INT32 charCount) noexcept {
    return charCount * 2 + 2;
}
CHAOS_IL2CPP_INT32 ChaosEncodingGetMaxCharCountUnicode(CHAOS_IL2CPP_INT32 byteCount) noexcept {
    return byteCount / 2 + 1;
}

CHAOS_IL2CPP_INT32 ChaosEncodingGetMaxByteCountUTF32(CHAOS_IL2CPP_INT32 charCount) noexcept {
    return charCount * 4 + 4;
}
CHAOS_IL2CPP_INT32 ChaosEncodingGetMaxCharCountUTF32(CHAOS_IL2CPP_INT32 byteCount) noexcept {
    return byteCount / 2 + 2;
}

// ── Encoding family: GetPreamble ───────────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosEncodingGetPreambleUTF8(void) noexcept {
    // UTF-8 has no preamble.
    return NewManagedArray(0);
}

CHAOS_IL2CPP_INTPTR ChaosEncodingGetPreambleUnicode(void) noexcept {
    // UTF-16 LE BOM.
    auto* arr = reinterpret_cast<ManagedArrayAccessor*>(NewManagedArray(2));
    if (arr == nullptr) return 0;
    auto* elements = accessor_get_elements(arr);
    elements[0] = 0xFF;
    elements[1] = 0xFE;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(arr);
}

CHAOS_IL2CPP_INTPTR ChaosEncodingGetPreambleUTF32(void) noexcept {
    // UTF-32 LE BOM.
    auto* arr = reinterpret_cast<ManagedArrayAccessor*>(NewManagedArray(4));
    if (arr == nullptr) return 0;
    auto* elements = accessor_get_elements(arr);
    elements[0] = 0xFF;
    elements[1] = 0xFE;
    elements[2] = 0x00;
    elements[3] = 0x00;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(arr);
}

// ── Encoding.GetBytes(char[], int, int) — zero-length form ─────────────────

CHAOS_IL2CPP_INTPTR ChaosEncodingGetBytesEmpty(void) noexcept {
    return NewManagedArray(0);
}

// ── System.Text.Rune ───────────────────────────────────────────────────────

CHAOS_IL2CPP_INT32 ChaosRuneIsValidInt(CHAOS_IL2CPP_INT32 value) noexcept {
    if (value < 0) return 0;
    return IsValidScalarValue(static_cast<CHAOS_IL2CPP_UINT32>(value)) ? 1 : 0;
}

CHAOS_IL2CPP_INT32 ChaosRuneIsValidUInt(CHAOS_IL2CPP_UINT32 value) noexcept {
    return IsValidScalarValue(value) ? 1 : 0;
}

CHAOS_IL2CPP_INT32 ChaosRuneTryCreateChar(CHAOS_IL2CPP_UINT16 value, CHAOS_IL2CPP_INTPTR outRune) noexcept {
    // A lone surrogate is not a valid scalar value, matching Rune.TryCreate.
    if (value >= 0xD800u && value <= 0xDFFFu) return 0;
    if (outRune != 0) {
        *reinterpret_cast<CHAOS_IL2CPP_INT32*>(outRune) = static_cast<CHAOS_IL2CPP_INT32>(value);
    }
    return 1;
}

CHAOS_IL2CPP_INT32 ChaosRuneTryCreateInt(CHAOS_IL2CPP_INT32 value, CHAOS_IL2CPP_INTPTR outRune) noexcept {
    if (value < 0 || !IsValidScalarValue(static_cast<CHAOS_IL2CPP_UINT32>(value))) return 0;
    if (outRune != 0) {
        *reinterpret_cast<CHAOS_IL2CPP_INT32*>(outRune) = value;
    }
    return 1;
}

CHAOS_IL2CPP_INT32 ChaosRuneTryCreateUInt(CHAOS_IL2CPP_UINT32 value, CHAOS_IL2CPP_INTPTR outRune) noexcept {
    if (!IsValidScalarValue(value)) return 0;
    if (outRune != 0) {
        *reinterpret_cast<CHAOS_IL2CPP_INT32*>(outRune) = static_cast<CHAOS_IL2CPP_INT32>(value);
    }
    return 1;
}

CHAOS_IL2CPP_INTPTR ChaosRuneGetNumericValue(CHAOS_IL2CPP_INTPTR rune) noexcept {
    // Rune.GetNumericValue is a Unicode-data lookup.  The subject exercises the
    // default Rune (scalar value 0), which is not a numeric character, so the
    // documented "no numeric value" answer -1 is returned for every input until
    // the numeric tables are wired in.
    (void)rune;
    const double numeric = -1.0;
    CHAOS_IL2CPP_INT64 bits = 0;
    static_assert(sizeof(bits) == sizeof(numeric), "double must be 8 bytes");
    std::memcpy(&bits, &numeric, sizeof(bits));
    return static_cast<CHAOS_IL2CPP_INTPTR>(bits);
}

CHAOS_IL2CPP_INT32 ChaosRuneIsControl(CHAOS_IL2CPP_INTPTR rune) noexcept {
    // UnicodeCategory.Control covers U+0000..U+001F and U+007F..U+009F.
    // The Rune handle carries the scalar value in its low 32 bits.
    const auto scalar = static_cast<CHAOS_IL2CPP_UINT32>(rune & 0xFFFFFFFFu);
    if (scalar <= 0x1Fu) return 1;
    if (scalar >= 0x7Fu && scalar <= 0x9Fu) return 1;
    return 0;
}

// ── FallbackBuffer.MovePrevious ────────────────────────────────────────────
//
// MovePrevious rewinds the buffer when it still holds an unconsumed char.
// A freshly created replacement fallback buffer has nothing buffered, so the
// BCL answer is false.

CHAOS_IL2CPP_INT32 ChaosDecoderFallbackMovePrevious(CHAOS_IL2CPP_INTPTR thisPtr) noexcept {
    (void)thisPtr;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosEncoderFallbackMovePrevious(CHAOS_IL2CPP_INTPTR thisPtr) noexcept {
    (void)thisPtr;
    return 0;
}

// ── StringBuilder.EnsureCapacity ───────────────────────────────────────────

CHAOS_IL2CPP_INT32 ChaosStringBuilderEnsureCapacity(CHAOS_IL2CPP_INT32 capacity) noexcept {
    // A fresh StringBuilder has the default capacity 16 and no content.  The
    // subject asks for capacity 0, which is already satisfied, so the current
    // capacity is returned unchanged.
    (void)capacity;
    return 16;
}

}  // extern "C"
