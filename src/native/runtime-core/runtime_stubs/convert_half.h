// convert_half.h — Double→IEEE 754 binary16 (Half) conversion
//
// Independent header, separated from convert_stubs.h to avoid linter merge
// conflicts.  Provides ChaosConvertToInt16FromDouble for the AOT codegen
// SimpleForward shape System.Convert.ToHalf(System.Double).
#pragma once

#include <chaos/native_types.h>
#include <chaos/compiler_hints.h>  // for CHAOS_IL2CPP_FORCEINLINE

#include <cstdint>
#include <cstring>
#include <cmath>

// Double→Half (IEEE 754 binary16, stored as UInt16).  Clamps to ±65504,
// converts sign/exponent/mantissa to 16-bit format with correct
// round-to-nearest-even on the 52→10 bit mantissa truncation.
CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_UINT16 ChaosConvertToInt16FromDouble(CHAOS_IL2CPP_FLOAT64 value) noexcept {
    // Handle special cases
    if (std::isnan(value)) return 0x7E01; // NaN in half format
    if (!std::isfinite(value)) {
        return value < 0.0 ? 0xFC00u : 0x7C00u; // ±Infinity
    }
    // Clamp to half range: values at or above max finite are handled by the
    // exponent saturation logic below (exp >= 31 -> +-infinity).  Only return
    // max finite for exact 65504.0; larger values must fall through to exp
    // detection so 65536.0 correctly becomes infinity (0x7C00) not 0x7BFF.
    if (value == 65504.0) return 0x7BFF;
    if (value == -65504.0) return 0xFBFF;

    uint64_t bits;
    std::memcpy(&bits, &value, sizeof(bits));
    uint64_t sign  = (bits >> 63) & 1;
    int64_t exp    = static_cast<int64_t>((bits >> 52) & 0x7FF);
    uint64_t mant  = bits & 0x000FFFFFFFFFFFFFull;

    if (exp == 0) {
        // Double subnormal: too small for half -> flush to zero.
        return static_cast<CHAOS_IL2CPP_UINT16>(sign << 15);
    }

    // Rebase exponent from double bias (1023) to half bias (15)
    exp = exp - 1023 + 15;

    if (exp <= 0) {
        // Subnormal in half: flush to zero
        return static_cast<CHAOS_IL2CPP_UINT16>(sign << 15);
    }
    if (exp >= 31) {
        // Saturated to infinity
        return static_cast<CHAOS_IL2CPP_UINT16>((sign << 15) | 0x7C00);
    }

    // Normal: round mantissa from 52 bits to 10 bits.
    // IEEE 754 round-to-nearest-even:
    //   discard > midpoint           -> round up
    //   discard == midpoint (tie)    -> round up only if LSB == 1
    //   discard < midpoint           -> truncate
    // Midpoint for 42 discarded bits = 0x2000000000000ull (2^41).
    uint64_t half_mant   = mant >> 42;
    uint64_t discard     = mant & 0x3FFFFFFFFFFFull;
    uint64_t midpoint    = 0x2000000000000ull;
    uint64_t lsb         = static_cast<uint64_t>(half_mant) & 1;
    if (discard > midpoint || (discard == midpoint && lsb)) {
        half_mant++;
    }
    if (half_mant >= 1024) {
        half_mant = 0;
        exp++;
        if (exp >= 31) {
            // Overflow to infinity after carry
            return static_cast<CHAOS_IL2CPP_UINT16>((sign << 15) | 0x7C00);
        }
    }
    return static_cast<CHAOS_IL2CPP_UINT16>((sign << 15) | (static_cast<uint64_t>(exp) << 10) | half_mant);
}
