#ifndef CHAOS_IL2CPP_PARSE_CONVERT_H_
#define CHAOS_IL2CPP_PARSE_CONVERT_H_

#include <chaos/native_types.h>

extern "C" {

// ── Convert::ToBoolean(String) ─────────────────────────────────
CHAOS_IL2CPP_INT32 ChaosConvertToBoolean(CHAOS_IL2CPP_INTPTR value);

// ── Convert::ToXxx(String) — string→numeric ────────────────────
CHAOS_IL2CPP_UINT8  ChaosConvertToByte(CHAOS_IL2CPP_INTPTR value);
CHAOS_IL2CPP_INT16  ChaosConvertToInt16(CHAOS_IL2CPP_INTPTR value);
CHAOS_IL2CPP_INT32  ChaosConvertToInt32(CHAOS_IL2CPP_INTPTR value);
CHAOS_IL2CPP_INT64  ChaosConvertToInt64(CHAOS_IL2CPP_INTPTR value);
CHAOS_IL2CPP_FLOAT32 ChaosConvertToSingle(CHAOS_IL2CPP_INTPTR value);
CHAOS_IL2CPP_FLOAT64 ChaosConvertToDouble(CHAOS_IL2CPP_INTPTR value);

// ── Convert::ToDecimal(String) — returns DecimalCarrier* via intptr ──
CHAOS_IL2CPP_INTPTR ChaosConvertToDecimal(CHAOS_IL2CPP_INTPTR value);

// ── Convert::ToInt32(Double) — truncation ──────────────────────
CHAOS_IL2CPP_INT32 ChaosConvertToInt32FromDouble(CHAOS_IL2CPP_FLOAT64 value);

// ── Convert::ToString(Int32) / ToString(Double) — numeric→stringId ──
CHAOS_IL2CPP_INTPTR ChaosFormatInt32(CHAOS_IL2CPP_INT32 value);
CHAOS_IL2CPP_INTPTR ChaosFormatDouble(CHAOS_IL2CPP_FLOAT64 value);

// ── Int32::Parse / Int64::Parse / Double::Parse ─────────────────────
CHAOS_IL2CPP_INT32  ChaosParseInt32(CHAOS_IL2CPP_INTPTR value);
CHAOS_IL2CPP_INT64  ChaosParseInt64(CHAOS_IL2CPP_INTPTR value);
CHAOS_IL2CPP_FLOAT64 ChaosParseDouble(CHAOS_IL2CPP_INTPTR value);

// ── Decimal arithmetic (DecimalCarrier* via intptr) ────────────
CHAOS_IL2CPP_INT32 ChaosDecimalToInt32(CHAOS_IL2CPP_INTPTR carrier_ptr);
CHAOS_IL2CPP_INTPTR ChaosDecimalAdd(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr);
CHAOS_IL2CPP_INTPTR ChaosDecimalSubtract(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr);
CHAOS_IL2CPP_INTPTR ChaosDecimalMultiply(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr);
CHAOS_IL2CPP_INTPTR ChaosDecimalDivide(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr);

// ── Convert::ToDecimal(Double) — double→DecimalCarrier ─────────
CHAOS_IL2CPP_INTPTR ChaosDecimalFromDouble(CHAOS_IL2CPP_FLOAT64 value);

}

#endif // CHAOS_IL2CPP_PARSE_CONVERT_H_