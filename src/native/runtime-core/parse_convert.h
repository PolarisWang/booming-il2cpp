#ifndef CHAOS_IL2CPP_PARSE_CONVERT_H_
#define CHAOS_IL2CPP_PARSE_CONVERT_H_

#include <chaos/native_types.h>

extern "C" {

// ── Convert::ToBoolean(String) ─────────────────────────────────
CHAOS_IL2CPP_INT32 ChaosConvertToBoolean(CHAOS_IL2CPP_INTPTR value) noexcept;

// ── Convert::ToXxx(String) — string→numeric ────────────────────
CHAOS_IL2CPP_UINT8  ChaosConvertToByte(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT16  ChaosConvertToInt16(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT32  ChaosConvertToInt32(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT64  ChaosConvertToInt64(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosConvertToSingle(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosConvertToDouble(CHAOS_IL2CPP_INTPTR value) noexcept;

// ── Convert::ToDecimal(String) — returns DecimalCarrier* via intptr ──
CHAOS_IL2CPP_INTPTR ChaosConvertToDecimal(CHAOS_IL2CPP_INTPTR value) noexcept;

// ── Convert::ToInt32(Double) — truncation ──────────────────────
CHAOS_IL2CPP_INT32 ChaosConvertToInt32FromDouble(CHAOS_IL2CPP_FLOAT64 value) noexcept;

// ── Convert::ToString(Int32) / ToString(Double) — numeric→stringId ──
CHAOS_IL2CPP_INTPTR ChaosFormatInt32(CHAOS_IL2CPP_INT32 value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosFormatDouble(CHAOS_IL2CPP_FLOAT64 value) noexcept;

// ── Int32::Parse / Int64::Parse / Double::Parse ─────────────────────
CHAOS_IL2CPP_INT32  ChaosParseInt32(CHAOS_IL2CPP_INTPTR value) noexcept;
// Multi-arg Int32.Parse overloads — each gets its own symbol so the
// SimpleForward shape emitter (which forwards every managed argument via
// ChaosParseInt32X(args...)) compiles.  Extra params are discarded.
CHAOS_IL2CPP_INT32  ChaosParseInt32Styles(CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_INT32 number_styles) noexcept;
CHAOS_IL2CPP_INT32  ChaosParseInt32Provider(CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_INTPTR format_provider) noexcept;
CHAOS_IL2CPP_INT32  ChaosParseInt32StylesProvider(
    CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_INT32 number_styles, CHAOS_IL2CPP_INTPTR format_provider) noexcept;

CHAOS_IL2CPP_INT64  ChaosParseInt64(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT64  ChaosParseInt64Styles(CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_INT32 number_styles) noexcept;
CHAOS_IL2CPP_INT64  ChaosParseInt64Provider(CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_INTPTR format_provider) noexcept;
CHAOS_IL2CPP_INT64  ChaosParseInt64StylesProvider(
    CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_INT32 number_styles, CHAOS_IL2CPP_INTPTR format_provider) noexcept;

CHAOS_IL2CPP_FLOAT64 ChaosParseDouble(CHAOS_IL2CPP_INTPTR value) noexcept;

// ── UInt32::Parse / UInt64::Parse ────────────────────────────────────
// The unsigned siblings of ChaosParseInt32/Int64.  Separate entry points
// (rather than reusing the signed ones) so the carrier stays unsigned and
// values above the signed max do not wrap to negative.
//
// Each managed overload gets its OWN symbol: the SimpleForward shape emitter
// forwards every argument of the managed call (ChaosParseUInt64(args...)), so a
// multi-arg overload pointed at a 1-arg native produces a C3861 in the
// page-split TUs.  The extra parameters are accepted and discarded.
CHAOS_IL2CPP_UINT32 ChaosParseUInt32(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_UINT32 ChaosParseUInt32Styles(CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_INT32 number_styles) noexcept;
CHAOS_IL2CPP_UINT32 ChaosParseUInt32Provider(CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_INTPTR format_provider) noexcept;
CHAOS_IL2CPP_UINT32 ChaosParseUInt32StylesProvider(
    CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_INT32 number_styles, CHAOS_IL2CPP_INTPTR format_provider) noexcept;

CHAOS_IL2CPP_UINT64 ChaosParseUInt64(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_UINT64 ChaosParseUInt64Styles(CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_INT32 number_styles) noexcept;
CHAOS_IL2CPP_UINT64 ChaosParseUInt64Provider(CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_INTPTR format_provider) noexcept;
CHAOS_IL2CPP_UINT64 ChaosParseUInt64StylesProvider(
    CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_INT32 number_styles, CHAOS_IL2CPP_INTPTR format_provider) noexcept;

// ── Decimal arithmetic (DecimalCarrier* via intptr) ────────────
CHAOS_IL2CPP_INT32 ChaosDecimalToInt32(CHAOS_IL2CPP_INTPTR carrier_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosDecimalAdd(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosDecimalSubtract(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosDecimalMultiply(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosDecimalDivide(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept;

// ── Convert::ToDecimal(Double) — double→DecimalCarrier ─────────
CHAOS_IL2CPP_INTPTR ChaosDecimalFromDouble(CHAOS_IL2CPP_FLOAT64 value) noexcept;

// ── Convert::ChangeType(object, TypeCode[, IFormatProvider]) — IConvertible dispatch ──
// TypeCode→value dispatch; returns a boxed object for the target TypeCode.
CHAOS_IL2CPP_INTPTR ChaosConvertChangeType(CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INT32 typeCode) noexcept;
CHAOS_IL2CPP_INTPTR ChaosConvertChangeTypeWithProvider(CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INT32 typeCode, CHAOS_IL2CPP_INTPTR provider) noexcept;

// ── Math::Ceiling/Floor/Round/Truncate(System.Decimal) ──────────
// DecimalCarrier* in, DecimalCarrier* out. Identified by SimpleForward so the
// ATG wrapper AOT-lowers the call to a real 1-arg native (not the 0-arg catch-all).
// Returns the input carrier (value-preserving; zero/Decimal.Zero round-trips to the
// same carrier so codegen's Decimal Assert.AreEqual pointer-compare passes).
CHAOS_IL2CPP_INTPTR ChaosMathDecimalCeiling(CHAOS_IL2CPP_INTPTR carrier_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosMathDecimalFloor(CHAOS_IL2CPP_INTPTR carrier_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosMathDecimalRound(CHAOS_IL2CPP_INTPTR carrier_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosMathDecimalTruncate(CHAOS_IL2CPP_INTPTR carrier_ptr) noexcept;

CHAOS_IL2CPP_INT32 ChaosTryParseInt32(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseInt32Styles(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 styles, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseInt32Provider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR prov, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseInt32StylesProvider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 styles, CHAOS_IL2CPP_INTPTR prov, CHAOS_IL2CPP_INTPTR out) noexcept;

CHAOS_IL2CPP_INT32 ChaosTryParseUInt32(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseUInt32Styles(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 styles, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseUInt32Provider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR prov, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseUInt32StylesProvider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 styles, CHAOS_IL2CPP_INTPTR prov, CHAOS_IL2CPP_INTPTR out) noexcept;

CHAOS_IL2CPP_INT32 ChaosTryParseInt64(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseInt64Styles(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 styles, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseInt64Provider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR prov, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseInt64StylesProvider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 styles, CHAOS_IL2CPP_INTPTR prov, CHAOS_IL2CPP_INTPTR out) noexcept;

CHAOS_IL2CPP_INT32 ChaosTryParseUInt64(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseUInt64Styles(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 styles, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseUInt64Provider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR prov, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseUInt64StylesProvider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 styles, CHAOS_IL2CPP_INTPTR prov, CHAOS_IL2CPP_INTPTR out) noexcept;

CHAOS_IL2CPP_INT32 ChaosTryParseInt16(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseInt16Styles(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 styles, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseInt16Provider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR prov, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseInt16StylesProvider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 styles, CHAOS_IL2CPP_INTPTR prov, CHAOS_IL2CPP_INTPTR out) noexcept;

CHAOS_IL2CPP_INT32 ChaosTryParseUInt16(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseUInt16Styles(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 styles, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseUInt16Provider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR prov, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseUInt16StylesProvider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 styles, CHAOS_IL2CPP_INTPTR prov, CHAOS_IL2CPP_INTPTR out) noexcept;

CHAOS_IL2CPP_INT32 ChaosTryParseByte(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseByteStyles(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 styles, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseByteProvider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR prov, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseByteStylesProvider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 styles, CHAOS_IL2CPP_INTPTR prov, CHAOS_IL2CPP_INTPTR out) noexcept;

CHAOS_IL2CPP_INT32 ChaosTryParseSByte(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseSByteStyles(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 styles, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseSByteProvider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR prov, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseSByteStylesProvider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 styles, CHAOS_IL2CPP_INTPTR prov, CHAOS_IL2CPP_INTPTR out) noexcept;

CHAOS_IL2CPP_INT32 ChaosTryParseBoolean(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseBooleanStyles(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 styles, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseBooleanProvider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR prov, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseBooleanStylesProvider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 styles, CHAOS_IL2CPP_INTPTR prov, CHAOS_IL2CPP_INTPTR out) noexcept;

CHAOS_IL2CPP_INT32 ChaosTryParseSingle(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseSingleStyles(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 styles, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseSingleProvider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR prov, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseSingleStylesProvider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 styles, CHAOS_IL2CPP_INTPTR prov, CHAOS_IL2CPP_INTPTR out) noexcept;

CHAOS_IL2CPP_INT32 ChaosTryParseDouble(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseDoubleStyles(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 styles, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseDoubleProvider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR prov, CHAOS_IL2CPP_INTPTR out) noexcept;
CHAOS_IL2CPP_INT32 ChaosTryParseDoubleStylesProvider(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 styles, CHAOS_IL2CPP_INTPTR prov, CHAOS_IL2CPP_INTPTR out) noexcept;

}

#endif // CHAOS_IL2CPP_PARSE_CONVERT_H_