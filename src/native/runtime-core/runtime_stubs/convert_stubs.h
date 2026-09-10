// ── Convert / Parse stub declarations ─────────────────────────────
// String-to-number and number-to-string conversion stubs for
// System.Convert and System.X.Parse families.
#pragma once

#include <chaos/native_types.h>
#include <chaos/compiler_hints.h>
#include <cmath>
#include <limits>

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
CHAOS_IL2CPP_INTPTR    ChaosConvertChangeType(CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INT32 typeCode) noexcept;
CHAOS_IL2CPP_INTPTR    ChaosConvertChangeTypeWithProvider(CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INT32 typeCode, CHAOS_IL2CPP_INTPTR provider) noexcept;

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
// Force-inline to eliminate the static wrapper→helper call chain
// in AOT codegen.  JIT inlines this to a single cvttsd2si instruction;
// inline removes ~3 function calls (StoreFloat64, LoadFloat64, wrapper).
// RaiseManagedException is [[noreturn]], so the overflow path is cold.
CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_INT32 ChaosConvertToInt32FromDouble(CHAOS_IL2CPP_FLOAT64 value) noexcept {
    if (std::isnan(value) ||
        value < static_cast<CHAOS_IL2CPP_FLOAT64>((std::numeric_limits<CHAOS_IL2CPP_INT32>::min)()) ||
        value > static_cast<CHAOS_IL2CPP_FLOAT64>((std::numeric_limits<CHAOS_IL2CPP_INT32>::max)()))
    {
        chaos::il2cpp::runtime_core::RaiseManagedException(
            "System.OverflowException",
            "Value was either too large or too small for an Int32.");
    }
    return static_cast<CHAOS_IL2CPP_INT32>(std::trunc(value));
}
// Double→Half conversion lives in convert_half.h (separate header to
// isolate from linter merge conflicts).
#include "convert_half.h"
// Double→Decimal conversion bridge.
CHAOS_IL2CPP_INTPTR    ChaosDecimalFromDouble(double value) noexcept;