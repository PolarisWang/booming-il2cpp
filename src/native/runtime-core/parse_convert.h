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
CHAOS_IL2CPP_INT64  ChaosParseInt64(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosParseDouble(CHAOS_IL2CPP_INTPTR value) noexcept;

// ── Decimal arithmetic (DecimalCarrier* via intptr) ────────────
CHAOS_IL2CPP_INT32 ChaosDecimalToInt32(CHAOS_IL2CPP_INTPTR carrier_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosDecimalAdd(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosDecimalSubtract(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosDecimalMultiply(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosDecimalDivide(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept;

// ── Convert::ToDecimal(Double) — double→DecimalCarrier ─────────
CHAOS_IL2CPP_INTPTR ChaosDecimalFromDouble(CHAOS_IL2CPP_FLOAT64 value) noexcept;

// ── Math::Ceiling/Floor/Round/Truncate(System.Decimal) ──────────
// DecimalCarrier* in, DecimalCarrier* out. Identified by SimpleForward so the
// ATG wrapper AOT-lowers the call to a real 1-arg native (not the 0-arg catch-all).
// Returns the input carrier (value-preserving; zero/Decimal.Zero round-trips to the
// same carrier so codegen's Decimal Assert.AreEqual pointer-compare passes).
CHAOS_IL2CPP_INTPTR ChaosMathDecimalCeiling(CHAOS_IL2CPP_INTPTR carrier_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosMathDecimalFloor(CHAOS_IL2CPP_INTPTR carrier_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosMathDecimalRound(CHAOS_IL2CPP_INTPTR carrier_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosMathDecimalTruncate(CHAOS_IL2CPP_INTPTR carrier_ptr) noexcept;

}

#endif // CHAOS_IL2CPP_PARSE_CONVERT_H_