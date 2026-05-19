// ── Convert / Parse stub declarations ─────────────────────────────
// String-to-number and number-to-string conversion stubs for
// System.Convert and System.X.Parse families.
#pragma once

#include <chaos/native_types.h>

// ── Convert string to primitive ────────────────────────────────────
// Each takes a managed string pointer (or string_id), parses its UTF-8
// content as the target type, and returns the converted value.
CHAOS_IL2CPP_INT32     ChaosConvertToBoolean(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_UINT8     ChaosConvertToByte(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INTPTR    ChaosConvertToDecimal(CHAOS_IL2CPP_INTPTR str) noexcept;
double                 ChaosConvertToDouble(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INT16     ChaosConvertToInt16(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INT32     ChaosConvertToInt32(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INT64     ChaosConvertToInt64(CHAOS_IL2CPP_INTPTR str) noexcept;
float                  ChaosConvertToSingle(CHAOS_IL2CPP_INTPTR str) noexcept;

// ── Parse string to primitive ──────────────────────────────────────
// Direct parse entry points for Int32.Parse, Int64.Parse, Double.Parse.
double                 ChaosParseDouble(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INT32     ChaosParseInt32(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INT64     ChaosParseInt64(CHAOS_IL2CPP_INTPTR str) noexcept;

// ── Format primitive to string ─────────────────────────────────────
// Returns a newly allocated managed string (or 0 on OOM).
CHAOS_IL2CPP_INTPTR    ChaosFormatDouble(double value) noexcept;
CHAOS_IL2CPP_INTPTR    ChaosFormatInt32(CHAOS_IL2CPP_INT32 value) noexcept;

// ── Decimal helpers ────────────────────────────────────────────────
CHAOS_IL2CPP_INT32     ChaosDecimalToInt32(CHAOS_IL2CPP_INTPTR dec) noexcept;

// ── Convert from Double ─────────────────────────────────────────────
// Double→Int32 truncation bridge (cvttsd2si equivalent).
CHAOS_IL2CPP_INT32     ChaosConvertToInt32FromDouble(double value) noexcept;
// Double→Decimal conversion bridge.
CHAOS_IL2CPP_INTPTR    ChaosDecimalFromDouble(double value) noexcept;