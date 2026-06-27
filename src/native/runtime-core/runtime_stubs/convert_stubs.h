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
// Double→Half (IEEE 754 binary16, stored as UInt16).  Clamps to ±65504,
// converts sign/exponent/mantissa to 16-bit format.
CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_UINT16 ChaosConvertToInt16FromDouble(CHAOS_IL2CPP_FLOAT64 value) noexcept {
    // Handle special cases
    if (std::isnan(value)) return 0x7E01; // NaN in half format
    if (!std::isfinite(value)) {
        return value < 0.0 ? 0xFC00u : 0x7C00u; // ±Infinity
    }
    // Clamp to half range
    if (value >= 65504.0) return 0x7BFF; // max finite half
    if (value <= -65504.0) return 0xFBFF; // -max finite half
    if (value >= -6.0e-8 && value <= 6.0e-8) return 0; // subnormal → zero

    uint64_t bits;
    std::memcpy(&bits, &value, sizeof(bits));
    uint64_t sign = (bits >> 63) & 1;
    int64_t exp = static_cast<int64_t>((bits >> 52) & 0x7FF) - 1023 + 15;
    uint64_t mant = bits & 0x000FFFFFFFFFFFFFull;

    if (exp <= 0) {
        // Subnormal in half: flush to zero (too small)
        return static_cast<CHAOS_IL2CPP_UINT16>((sign << 15));
    }
    if (exp > 30) {
        // Saturated to infinity
        return static_cast<CHAOS_IL2CPP_UINT16>((sign << 15) | 0x7C00);
    }

    // Normal: round mantissa from 52 bits to 10 bits
    uint64_t half_mant = mant >> 42;
    // Round-to-nearest-even on the discarded bits
    uint64_t discard = mant & 0x3FFFFFFFFFFFull;
    uint64_t lsb = mant >> 42;
    if (discard > 0x1FFFFFFFFFFFFull || (discard == 0x1FFFFFFFFFFFFull && (lsb & 1))) {
        half_mant++;
    }
    if (half_mant >= 1024) {
        half_mant = 0;
        exp++;
    }
    return static_cast<CHAOS_IL2CPP_UINT16>((sign << 15) | (static_cast<uint64_t>(exp) << 10) | half_mant);
}
// Double→Decimal conversion bridge.
CHAOS_IL2CPP_INTPTR    ChaosDecimalFromDouble(double value) noexcept;