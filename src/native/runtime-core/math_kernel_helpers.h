#ifndef CHAOS_IL2CPP_MATH_KERNEL_HELPERS_H_
#define CHAOS_IL2CPP_MATH_KERNEL_HELPERS_H_

#include <chaos/native_types.h>
#include <cmath>
#include <cstdint>

namespace chaos::il2cpp::math_kernel_helpers {

// Integer templates
template <typename T>
inline T MathAbs(T value) {
    return value < 0 ? -value : value;
}

template <typename T>
inline T MathMax(T a, T b) {
    return a > b ? a : b;
}

template <typename T>
inline T MathMin(T a, T b) {
    return a < b ? a : b;
}

template <typename T>
inline T MathClamp(T value, T min, T max) {
    return value < min ? min : (value > max ? max : value);
}

template <typename T>
inline CHAOS_IL2CPP_INT32 MathSign(T value) {
    if (value > 0) return 1;
    if (value < 0) return -1;
    return 0;
}

// Scalar float/double wrappers
inline double MathTruncate(double value) { return CHAOS_IL2CPP_TRUNC(value); }
inline float  MathTruncate(float  value) { return CHAOS_IL2CPP_TRUNC(value); }

inline double MathRoundToEven(double value) { return CHAOS_IL2CPP_ROUND(value); }
inline float  MathRoundToEven(float  value) { return CHAOS_IL2CPP_ROUND(value); }

inline double MathIEEERemainder(double x, double y) { return CHAOS_IL2CPP_REMAINDER(x, y); }
inline float  MathIEEERemainder(float  x, float  y) { return CHAOS_IL2CPP_REMAINDER(x, y); }

inline CHAOS_IL2CPP_INT32 MathILogB(double value) { return CHAOS_IL2CPP_ILOGB(value); }
inline CHAOS_IL2CPP_INT32 MathILogB(float  value) { return CHAOS_IL2CPP_ILOGB(value); }

inline double MathLog(double a, double b) { return CHAOS_IL2CPP_LOG(a) / CHAOS_IL2CPP_LOG(b); }
inline float  MathLog(float  a, float  b) { return CHAOS_IL2CPP_LOG(a) / CHAOS_IL2CPP_LOG(b); }

inline double MathCopySign(double x, double y) { return CHAOS_IL2CPP_COPYSIGN(x, y); }
inline float  MathCopySign(float  x, float  y) { return CHAOS_IL2CPP_COPYSIGN(x, y); }

inline double MathScaleB(double x, CHAOS_IL2CPP_INT32 n) { return CHAOS_IL2CPP_SCALBN(x, n); }
inline float  MathScaleB(float  x, CHAOS_IL2CPP_INT32 n) { return CHAOS_IL2CPP_SCALBN(x, n); }

inline double MathBitDecrement(double value) { return CHAOS_IL2CPP_NEXTAFTER(value, -CHAOS_IL2CPP_NUMERIC_LIMITS_INFINITY(double)); }
inline float  MathBitDecrement(float  value) { return CHAOS_IL2CPP_NEXTAFTER(value, -CHAOS_IL2CPP_NUMERIC_LIMITS_INFINITY(float)); }

inline double MathBitIncrement(double value) { return CHAOS_IL2CPP_NEXTAFTER(value, CHAOS_IL2CPP_NUMERIC_LIMITS_INFINITY(double)); }
inline float  MathBitIncrement(float  value) { return CHAOS_IL2CPP_NEXTAFTER(value, CHAOS_IL2CPP_NUMERIC_LIMITS_INFINITY(float)); }

inline double MathReciprocalEstimate(double value) { return 1.0 / value; }
inline float  MathReciprocalEstimate(float  value) { return 1.0f / value; }

inline double MathReciprocalSqrtEstimate(double value) { return 1.0 / CHAOS_IL2CPP_SQRT(value); }
inline float  MathReciprocalSqrtEstimate(float  value) { return 1.0f / CHAOS_IL2CPP_SQRT(value); }

// NOTE: Use std::abs + ternary instead of std::fmaxmag/fminmag.
// std::fmaxmag/fminmag are not reliably available across all MSVC versions
// (they require C++17 mode on some toolchains and may be missing entirely on others).
inline double MathMaxMagnitude(double x, double y) { return (CHAOS_IL2CPP_ABS(x) >= CHAOS_IL2CPP_ABS(y)) ? x : y; }
inline float  MathMaxMagnitude(float  x, float  y) { return (CHAOS_IL2CPP_ABS(x) >= CHAOS_IL2CPP_ABS(y)) ? x : y; }

inline double MathMinMagnitude(double x, double y) { return (CHAOS_IL2CPP_ABS(x) <= CHAOS_IL2CPP_ABS(y)) ? x : y; }
inline float  MathMinMagnitude(float  x, float  y) { return (CHAOS_IL2CPP_ABS(x) <= CHAOS_IL2CPP_ABS(y)) ? x : y; }

inline CHAOS_IL2CPP_INT64 MathBigMul(CHAOS_IL2CPP_INT32 a, CHAOS_IL2CPP_INT32 b) {
    return static_cast<CHAOS_IL2CPP_INT64>(a) * static_cast<CHAOS_IL2CPP_INT64>(b);
}

inline CHAOS_IL2CPP_INT64 MathBigMul(CHAOS_IL2CPP_INT64 a, CHAOS_IL2CPP_INT64 b, CHAOS_IL2CPP_INT64* low) {
    using U64 = CHAOS_IL2CPP_UINT64;
    U64 a_lo = static_cast<U64>(a) & 0xFFFFFFFFu;
    U64 a_hi = static_cast<U64>(a) >> 32;
    U64 b_lo = static_cast<U64>(b) & 0xFFFFFFFFu;
    U64 b_hi = static_cast<U64>(b) >> 32;
    U64 lo = a_lo * b_lo;
    U64 t1 = a_lo * b_hi + (lo >> 32);
    U64 t2 = a_hi * b_lo + (t1 & 0xFFFFFFFFu);
    lo = (t2 << 32) | (lo & 0xFFFFFFFFu);
    U64 hi = a_hi * b_hi + (t1 >> 32) + (t2 >> 32);
    *low = static_cast<CHAOS_IL2CPP_INT64>(lo);
    return static_cast<CHAOS_IL2CPP_INT64>(hi);
}

// SinCos (struct return)
struct MathSinCosResult { double Sin; double Cos; };
struct MathSinCosResultF { float  Sin; float  Cos; };

inline MathSinCosResult MathSinCos(double x) {
    double s, c;
    CHAOS_IL2CPP_SINCOS(x, s, c);
    return MathSinCosResult{s, c};
}

inline MathSinCosResultF MathSinCos(float x) {
    return MathSinCosResultF{CHAOS_IL2CPP_SIN(x), CHAOS_IL2CPP_COS(x)};
}

// DivRem helpers
inline CHAOS_IL2CPP_INT32 MathDivRem(CHAOS_IL2CPP_INT32 a, CHAOS_IL2CPP_INT32 b, CHAOS_IL2CPP_INT32* rem) {
    *rem = a % b;
    return a / b;
}

inline CHAOS_IL2CPP_INT64 MathDivRem(CHAOS_IL2CPP_INT64 a, CHAOS_IL2CPP_INT64 b, CHAOS_IL2CPP_INT64* rem) {
    *rem = a % b;
    return a / b;
}

struct MathDivRemByteResult    { CHAOS_IL2CPP_UINT8  Quotient; CHAOS_IL2CPP_UINT8  Remainder; };
struct MathDivRemSByteResult   { CHAOS_IL2CPP_INT8   Quotient; CHAOS_IL2CPP_INT8   Remainder; };
struct MathDivRemInt16Result   { CHAOS_IL2CPP_INT16  Quotient; CHAOS_IL2CPP_INT16  Remainder; };
struct MathDivRemUInt16Result  { CHAOS_IL2CPP_UINT16 Quotient; CHAOS_IL2CPP_UINT16 Remainder; };
struct MathDivRemInt32Result   { CHAOS_IL2CPP_INT32  Quotient; CHAOS_IL2CPP_INT32  Remainder; };
struct MathDivRemUInt32Result  { CHAOS_IL2CPP_UINT32 Quotient; CHAOS_IL2CPP_UINT32 Remainder; };
struct MathDivRemInt64Result   { CHAOS_IL2CPP_INT64  Quotient; CHAOS_IL2CPP_INT64  Remainder; };
struct MathDivRemUInt64Result  { CHAOS_IL2CPP_UINT64 Quotient; CHAOS_IL2CPP_UINT64 Remainder; };
struct MathDivRemIntPtrResult  { CHAOS_IL2CPP_INTPTR  Quotient; CHAOS_IL2CPP_INTPTR  Remainder; };
struct MathDivRemUIntPtrResult { CHAOS_IL2CPP_UINTPTR Quotient; CHAOS_IL2CPP_UINTPTR Remainder; };

inline MathDivRemByteResult    MathDivRemByte(CHAOS_IL2CPP_UINT8 a, CHAOS_IL2CPP_UINT8 b)    { return {static_cast<CHAOS_IL2CPP_UINT8>(a / b), static_cast<CHAOS_IL2CPP_UINT8>(a % b)}; }
inline MathDivRemSByteResult   MathDivRemSByte(CHAOS_IL2CPP_INT8 a, CHAOS_IL2CPP_INT8 b)     { return {static_cast<CHAOS_IL2CPP_INT8>(a / b), static_cast<CHAOS_IL2CPP_INT8>(a % b)}; }
inline MathDivRemInt16Result   MathDivRemInt16(CHAOS_IL2CPP_INT16 a, CHAOS_IL2CPP_INT16 b)   { return {static_cast<CHAOS_IL2CPP_INT16>(a / b), static_cast<CHAOS_IL2CPP_INT16>(a % b)}; }
inline MathDivRemUInt16Result  MathDivRemUInt16(CHAOS_IL2CPP_UINT16 a, CHAOS_IL2CPP_UINT16 b) { return {static_cast<CHAOS_IL2CPP_UINT16>(a / b), static_cast<CHAOS_IL2CPP_UINT16>(a % b)}; }
inline MathDivRemInt32Result   MathDivRemInt32(CHAOS_IL2CPP_INT32 a, CHAOS_IL2CPP_INT32 b)   { return {a / b, a % b}; }
inline MathDivRemUInt32Result  MathDivRemUInt32(CHAOS_IL2CPP_UINT32 a, CHAOS_IL2CPP_UINT32 b) { return {a / b, a % b}; }
inline MathDivRemInt64Result   MathDivRemInt64(CHAOS_IL2CPP_INT64 a, CHAOS_IL2CPP_INT64 b)   { return {a / b, a % b}; }
inline MathDivRemUInt64Result  MathDivRemUInt64(CHAOS_IL2CPP_UINT64 a, CHAOS_IL2CPP_UINT64 b) { return {a / b, a % b}; }
inline MathDivRemIntPtrResult  MathDivRemIntPtr(CHAOS_IL2CPP_INTPTR a, CHAOS_IL2CPP_INTPTR b) { return {a / b, a % b}; }
inline MathDivRemUIntPtrResult MathDivRemUIntPtr(CHAOS_IL2CPP_UINTPTR a, CHAOS_IL2CPP_UINTPTR b) { return {a / b, a % b}; }

}  // namespace chaos::il2cpp::math_kernel_helpers

#endif  // CHAOS_IL2CPP_MATH_KERNEL_HELPERS_H_
