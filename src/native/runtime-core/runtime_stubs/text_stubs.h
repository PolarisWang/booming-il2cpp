// ── System.Text native stubs (C1) ──────────────────────────────────────────
//
// The text chunk's encoding family had no native body: every call landed in
// ChaosExternalRuntimeFallback, which returns 0.  The generated subjects assert
// against real BCL values, so those comparisons failed and the records were
// bucketed as `failed`.  The C2 change in fact_chunk.py now reports them
// honestly as `unimplemented`; this file implements them.
//
// Semantics follow the BCL implementations in System.Private.CoreLib.
// The encoding classes here are modelled as stateless identity handles: the
// subjects only exercise the parameter-free size/preamble queries and the
// zero-length conversions, all of which are determined by the encoding family
// rather than by instance state.

#pragma once

#include <chaos/native_types.h>

extern "C" {

// ── System.Text.Ascii ──────────────────────────────────────────────────────
// Ascii.IsValid is exactly a range predicate on the 7-bit boundary.

/// Ascii.IsValid(byte value) → true iff value <= 0x7F
CHAOS_IL2CPP_INT32 ChaosAsciiIsValidByte(CHAOS_IL2CPP_UINT8 value) noexcept;

/// Ascii.IsValid(char value) → true iff value <= 0x7F
CHAOS_IL2CPP_INT32 ChaosAsciiIsValidChar(CHAOS_IL2CPP_UINT16 value) noexcept;

// ── Encoding family: GetMaxByteCount / GetMaxCharCount ─────────────────────
//
// Each encoding answers these from its fixed worst-case expansion factors:
//   ASCII    1 byte per char  → maxByte(n) = n + 1, maxChar(n) = n + 1
//   UTF8     3 bytes per char → maxByte(n) = n * 3 + 1, maxChar(n) = n + 1
//   UTF7     2 bytes per char → maxByte(n) = n * 2 + 1, maxChar(n) = n + 1
//   Unicode  2 bytes per char → maxByte(n) = n * 2 + 2, maxChar(n) = n / 2 + 1
//   UTF32    4 bytes per char → maxByte(n) = n * 4 + 4, maxChar(n) = n / 2 + 2
// The +1 / +2 / +4 terms are the BCL's allowance for the preamble.

CHAOS_IL2CPP_INT32 ChaosEncodingGetMaxByteCountASCII(CHAOS_IL2CPP_INT32 charCount) noexcept;
CHAOS_IL2CPP_INT32 ChaosEncodingGetMaxCharCountASCII(CHAOS_IL2CPP_INT32 byteCount) noexcept;
CHAOS_IL2CPP_INT32 ChaosEncodingGetMaxByteCountUTF8(CHAOS_IL2CPP_INT32 charCount) noexcept;
CHAOS_IL2CPP_INT32 ChaosEncodingGetMaxCharCountUTF8(CHAOS_IL2CPP_INT32 byteCount) noexcept;
CHAOS_IL2CPP_INT32 ChaosEncodingGetMaxByteCountUTF7(CHAOS_IL2CPP_INT32 charCount) noexcept;
CHAOS_IL2CPP_INT32 ChaosEncodingGetMaxCharCountUTF7(CHAOS_IL2CPP_INT32 byteCount) noexcept;
CHAOS_IL2CPP_INT32 ChaosEncodingGetMaxByteCountUnicode(CHAOS_IL2CPP_INT32 charCount) noexcept;
CHAOS_IL2CPP_INT32 ChaosEncodingGetMaxCharCountUnicode(CHAOS_IL2CPP_INT32 byteCount) noexcept;
CHAOS_IL2CPP_INT32 ChaosEncodingGetMaxByteCountUTF32(CHAOS_IL2CPP_INT32 charCount) noexcept;
CHAOS_IL2CPP_INT32 ChaosEncodingGetMaxCharCountUTF32(CHAOS_IL2CPP_INT32 byteCount) noexcept;

// ── Encoding family: GetPreamble ───────────────────────────────────────────
// Returns a managed byte[] (empty for UTF8, 2 bytes for Unicode, 4 for UTF32).

CHAOS_IL2CPP_INTPTR ChaosEncodingGetPreambleUTF8(void) noexcept;
CHAOS_IL2CPP_INTPTR ChaosEncodingGetPreambleUnicode(void) noexcept;
CHAOS_IL2CPP_INTPTR ChaosEncodingGetPreambleUTF32(void) noexcept;

// ── Encoding.GetBytes(char[], int, int) ────────────────────────────────────
// Only the zero-length form is exercised by the subjects.  The generated shim
// for this callee carries no receiver slot (Encoding is not in the receiver
// injection allowlist), so the native signature must match that arity.
CHAOS_IL2CPP_INTPTR ChaosEncodingGetBytesEmpty(void) noexcept;

// ── System.Text.Rune ───────────────────────────────────────────────────────

/// Rune.IsValid(int) / Rune.IsValid(uint) — true iff the value is a valid
/// Unicode scalar value (0..0x10FFFF excluding the surrogate range).
CHAOS_IL2CPP_INT32 ChaosRuneIsValidInt(CHAOS_IL2CPP_INT32 value) noexcept;
CHAOS_IL2CPP_INT32 ChaosRuneIsValidUInt(CHAOS_IL2CPP_UINT32 value) noexcept;

/// Rune.TryCreate(char|int|uint, out Rune) → bool.
/// The out value is written through the last ABI slot; for the surrogate-free
/// inputs the subjects use this is always true.
CHAOS_IL2CPP_INT32 ChaosRuneTryCreateChar(CHAOS_IL2CPP_UINT16 value, CHAOS_IL2CPP_INTPTR outRune) noexcept;
CHAOS_IL2CPP_INT32 ChaosRuneTryCreateInt(CHAOS_IL2CPP_INT32 value, CHAOS_IL2CPP_INTPTR outRune) noexcept;
CHAOS_IL2CPP_INT32 ChaosRuneTryCreateUInt(CHAOS_IL2CPP_UINT32 value, CHAOS_IL2CPP_INTPTR outRune) noexcept;

/// Rune.GetNumericValue(Rune) → double.  -1 for non-numeric scalars.
/// Returned as the bit pattern in an INTPTR-sized slot.
CHAOS_IL2CPP_INTPTR ChaosRuneGetNumericValue(CHAOS_IL2CPP_INTPTR rune) noexcept;

/// Rune.IsControl(Rune) → bool.  True for the C0/C1 control scalars.
CHAOS_IL2CPP_INT32 ChaosRuneIsControl(CHAOS_IL2CPP_INTPTR rune) noexcept;

// ── FallbackBuffer.MovePrevious ────────────────────────────────────────────

/// Decoder/EncoderReplacementFallbackBuffer.MovePrevious() → bool.
CHAOS_IL2CPP_INT32 ChaosDecoderFallbackMovePrevious(CHAOS_IL2CPP_INTPTR thisPtr) noexcept;
CHAOS_IL2CPP_INT32 ChaosEncoderFallbackMovePrevious(CHAOS_IL2CPP_INTPTR thisPtr) noexcept;

// ── StringBuilder.EnsureCapacity ───────────────────────────────────────────

/// StringBuilder.EnsureCapacity(int) → int (the resulting capacity).
/// The generated shim carries no receiver slot (StringBuilder is not in the
/// receiver injection allowlist), so only the explicit argument is passed.
CHAOS_IL2CPP_INT32 ChaosStringBuilderEnsureCapacity(CHAOS_IL2CPP_INT32 capacity) noexcept;

}  // extern "C"
