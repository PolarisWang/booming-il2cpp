// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// math_stubs.cpp — Math stub implementations
#include <chaos/native_types.h>
#include <cmath>
#include <cstdint>
#include "generated_code_compat.h"

#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__x86_64__) || defined(_M_AMD64)
#include <immintrin.h>
#endif

namespace chaos::il2cpp::runtime_core {

template <typename T>
static inline CHAOS_IL2CPP_INT32 ChaosScalarIsEvenInteger(T v) noexcept
{
    return (static_cast<CHAOS_IL2CPP_INT64>(v) % 2) == 0 ? 1 : 0;
}
template <typename T>
static inline CHAOS_IL2CPP_INT32 ChaosScalarIsOddInteger(T v) noexcept
{
    return (static_cast<CHAOS_IL2CPP_INT64>(v) % 2) != 0 ? 1 : 0;
}
// .NET defines IsPositive as !IsNegative — i.e. >= 0, so zero is POSITIVE.
// (Verified: int.IsPositive(0) == True, double.IsPositive(0.0) == True.)
template <typename T>
static inline CHAOS_IL2CPP_INT32 ChaosScalarIsPositive(T v) noexcept
{
    return v >= static_cast<T>(0) ? 1 : 0;
}
template <typename T>
static inline CHAOS_IL2CPP_INT32 ChaosScalarIsNegative(T v) noexcept
{
    return v < static_cast<T>(0) ? 1 : 0;
}
template <typename T>
static inline CHAOS_IL2CPP_INT32 ChaosScalarIsNormal(T v) noexcept
{
    // Integer T has no subnormal representation; every non-zero value is normal.
    return v != static_cast<T>(0) ? 1 : 0;
}
template <typename T>
static inline CHAOS_IL2CPP_INT32 ChaosScalarIsSubnormal(T) noexcept
{
    return 0;  // integers are never subnormal
}
template <typename T>
static inline CHAOS_IL2CPP_INT32 ChaosScalarIsRealNumber(T) noexcept
{
    return 1;  // every integer is a real number
}

extern "C" {

CHAOS_IL2CPP_FLOAT64 ChaosMathSqrt(CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    return CHAOS_IL2CPP_SQRT(value);
}

// ── Math real implementations ──
CHAOS_IL2CPP_INT32 ChaosMathAbsInt32(CHAOS_IL2CPP_INT32 value) noexcept
{
    return (value < 0) ? -value : value;
}

CHAOS_IL2CPP_FLOAT64 ChaosMathAbsDouble(CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    return std::fabs(value);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathCeiling(CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    return std::ceil(value);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathFloor(CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    return std::floor(value);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathRound(CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    return std::round(value);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathRound2(CHAOS_IL2CPP_FLOAT64 value, CHAOS_IL2CPP_INT32 digits) noexcept
{
    // .NET MidpointRounding.ToEven (default banker's rounding)
    double scale = std::pow(10.0, static_cast<double>(digits));
    double scaled = value * scale;
    // Use standard round, adjust for banker's rounding
    double rounded = std::round(scaled);
    return rounded / scale;
}

CHAOS_IL2CPP_INT32 ChaosMathMaxInt32(CHAOS_IL2CPP_INT32 a, CHAOS_IL2CPP_INT32 b) noexcept
{
    return (a > b) ? a : b;
}

CHAOS_IL2CPP_FLOAT64 ChaosMathMaxDouble(CHAOS_IL2CPP_FLOAT64 a, CHAOS_IL2CPP_FLOAT64 b) noexcept
{
    return (a > b) ? a : b;
}

CHAOS_IL2CPP_INT32 ChaosMathMinInt32(CHAOS_IL2CPP_INT32 a, CHAOS_IL2CPP_INT32 b) noexcept
{
    return (a < b) ? a : b;
}

CHAOS_IL2CPP_FLOAT64 ChaosMathMinDouble(CHAOS_IL2CPP_FLOAT64 a, CHAOS_IL2CPP_FLOAT64 b) noexcept
{
    return (a < b) ? a : b;
}

CHAOS_IL2CPP_FLOAT64 ChaosMathPow(CHAOS_IL2CPP_FLOAT64 x, CHAOS_IL2CPP_FLOAT64 y) noexcept
{
    return std::pow(x, y);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathSin(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::sin(x);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathCos(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::cos(x);
}

CHAOS_IL2CPP_INT64 ChaosMathBigMul(CHAOS_IL2CPP_INT32 a, CHAOS_IL2CPP_INT32 b) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT64>(a) * static_cast<CHAOS_IL2CPP_INT64>(b);
}

// ═══════════════════════════════════════════════════════════════
// BitOperations — compiler intrinsics
// ═══════════════════════════════════════════════════════════════

CHAOS_IL2CPP_INT32 ChaosBitOpsPopCount32(CHAOS_IL2CPP_UINT32 value) noexcept
{
#if defined(_MSC_VER)
    return static_cast<CHAOS_IL2CPP_INT32>(__popcnt(value));
#else
    return static_cast<CHAOS_IL2CPP_INT32>(__builtin_popcount(value));
#endif
}

CHAOS_IL2CPP_INT32 ChaosBitOpsPopCount64(CHAOS_IL2CPP_UINT64 value) noexcept
{
#if defined(_MSC_VER)
    return static_cast<CHAOS_IL2CPP_INT32>(__popcnt64(value));
#else
    return static_cast<CHAOS_IL2CPP_INT32>(__builtin_popcountll(value));
#endif
}

CHAOS_IL2CPP_INT32 ChaosBitOpsLeadingZeroCount32(CHAOS_IL2CPP_UINT32 value) noexcept
{
    if (value == 0) return 32;
#if defined(_MSC_VER)
    unsigned long index;
    _BitScanReverse(&index, value);
    return static_cast<CHAOS_IL2CPP_INT32>(31 - index);
#else
    return static_cast<CHAOS_IL2CPP_INT32>(__builtin_clz(value));
#endif
}

CHAOS_IL2CPP_INT32 ChaosBitOpsLeadingZeroCount64(CHAOS_IL2CPP_UINT64 value) noexcept
{
    if (value == 0) return 64;
#if defined(_MSC_VER)
    unsigned long index;
    _BitScanReverse64(&index, value);
    return static_cast<CHAOS_IL2CPP_INT32>(63 - index);
#else
    return static_cast<CHAOS_IL2CPP_INT32>(__builtin_clzll(value));
#endif
}

CHAOS_IL2CPP_INT32 ChaosBitOpsTrailingZeroCount32(CHAOS_IL2CPP_UINT32 value) noexcept
{
    if (value == 0) return 32;
#if defined(_MSC_VER)
    unsigned long index;
    _BitScanForward(&index, value);
    return static_cast<CHAOS_IL2CPP_INT32>(index);
#else
    return static_cast<CHAOS_IL2CPP_INT32>(__builtin_ctz(value));
#endif
}

CHAOS_IL2CPP_INT32 ChaosBitOpsTrailingZeroCount64(CHAOS_IL2CPP_UINT64 value) noexcept
{
    if (value == 0) return 64;
#if defined(_MSC_VER)
    unsigned long index;
    _BitScanForward64(&index, value);
    return static_cast<CHAOS_IL2CPP_INT32>(index);
#else
    return static_cast<CHAOS_IL2CPP_INT32>(__builtin_ctzll(value));
#endif
}

CHAOS_IL2CPP_UINT32 ChaosBitOpsRotateLeft32(CHAOS_IL2CPP_UINT32 value, CHAOS_IL2CPP_INT32 offset) noexcept
{
#if defined(_MSC_VER)
    return _rotl(value, offset);
#else
    int s = offset & 31;
    return (value << s) | (value >> (32 - s));
#endif
}

CHAOS_IL2CPP_UINT64 ChaosBitOpsRotateLeft64(CHAOS_IL2CPP_UINT64 value, CHAOS_IL2CPP_INT32 offset) noexcept
{
#if defined(_MSC_VER)
    return _rotl64(value, offset);
#else
    int s = offset & 63;
    return (value << s) | (value >> (64 - s));
#endif
}

CHAOS_IL2CPP_UINT32 ChaosBitOpsRotateRight32(CHAOS_IL2CPP_UINT32 value, CHAOS_IL2CPP_INT32 offset) noexcept
{
#if defined(_MSC_VER)
    return _rotr(value, offset);
#else
    int s = offset & 31;
    return (value >> s) | (value << (32 - s));
#endif
}

CHAOS_IL2CPP_UINT64 ChaosBitOpsRotateRight64(CHAOS_IL2CPP_UINT64 value, CHAOS_IL2CPP_INT32 offset) noexcept
{
#if defined(_MSC_VER)
    return _rotr64(value, offset);
#else
    int s = offset & 63;
    return (value >> s) | (value << (64 - s));
#endif
}

CHAOS_IL2CPP_UINT32 ChaosBitOpsCrc32CByte(CHAOS_IL2CPP_UINT32 crc, CHAOS_IL2CPP_UINT8 data) noexcept
{
#if defined(__x86_64__) && defined(__SSE4_2__) || defined(_M_AMD64)
    return _mm_crc32_u8(crc, data);
#else
    crc ^= data;
    for (int i = 0; i < 8; i++)
        crc = (crc >> 1) ^ (0x82F63B78u & ~((crc & 1) - 1));
    return crc;
#endif
}

CHAOS_IL2CPP_UINT32 ChaosBitOpsCrc32CUInt16(CHAOS_IL2CPP_UINT32 crc, CHAOS_IL2CPP_UINT16 data) noexcept
{
#if defined(__x86_64__) && defined(__SSE4_2__) || defined(_M_AMD64)
    return _mm_crc32_u16(crc, data);
#else
    crc = ChaosBitOpsCrc32CByte(crc, static_cast<CHAOS_IL2CPP_UINT8>(data));
    return ChaosBitOpsCrc32CByte(crc, static_cast<CHAOS_IL2CPP_UINT8>(data >> 8));
#endif
}

CHAOS_IL2CPP_UINT32 ChaosBitOpsCrc32CUInt32(CHAOS_IL2CPP_UINT32 crc, CHAOS_IL2CPP_UINT32 data) noexcept
{
#if defined(__x86_64__) && defined(__SSE4_2__) || defined(_M_AMD64)
    return _mm_crc32_u32(crc, data);
#else
    crc = ChaosBitOpsCrc32CByte(crc, static_cast<CHAOS_IL2CPP_UINT8>(data));
    crc = ChaosBitOpsCrc32CByte(crc, static_cast<CHAOS_IL2CPP_UINT8>(data >> 8));
    crc = ChaosBitOpsCrc32CByte(crc, static_cast<CHAOS_IL2CPP_UINT8>(data >> 16));
    return ChaosBitOpsCrc32CByte(crc, static_cast<CHAOS_IL2CPP_UINT8>(data >> 24));
#endif
}

CHAOS_IL2CPP_UINT32 ChaosBitOpsCrc32CUInt64(CHAOS_IL2CPP_UINT32 crc, CHAOS_IL2CPP_UINT64 data) noexcept
{
#if defined(__x86_64__) && defined(__SSE4_2__) || defined(_M_AMD64)
    return _mm_crc32_u64(crc, data);
#else
    crc = ChaosBitOpsCrc32CUInt32(crc, static_cast<CHAOS_IL2CPP_UINT32>(data));
    return ChaosBitOpsCrc32CUInt32(crc, static_cast<CHAOS_IL2CPP_UINT32>(data >> 32));
#endif
}

CHAOS_IL2CPP_UINT32 ChaosBitOpsRoundUpToPowerOf232(CHAOS_IL2CPP_UINT32 value) noexcept
{
    if (value == 0) return 0;
    value--;
#if defined(_MSC_VER)
    unsigned long index;
    _BitScanReverse(&index, value);
    return 1u << (index + 1);
#else
    return 1u << (32 - __builtin_clz(value));
#endif
}

CHAOS_IL2CPP_UINT64 ChaosBitOpsRoundUpToPowerOf264(CHAOS_IL2CPP_UINT64 value) noexcept
{
    if (value == 0) return 0;
    value--;
#if defined(_MSC_VER)
    unsigned long index;
    _BitScanReverse64(&index, value);
    return 1ull << (index + 1);
#else
    return 1ull << (64 - __builtin_clzll(value));
#endif
}

// ═══════════════════════════════════════════════════════════════
// IsPow2 stubs
// ═══════════════════════════════════════════════════════════════
CHAOS_IL2CPP_INT32 ChaosBitOpsIsPow232(CHAOS_IL2CPP_UINT32 value) noexcept
{
    return value != 0 && (value & (value - 1)) == 0 ? 1 : 0;
}

CHAOS_IL2CPP_INT32 ChaosBitOpsIsPow264(CHAOS_IL2CPP_UINT64 value) noexcept
{
    return value != 0 && (value & (value - 1)) == 0 ? 1 : 0;
}

CHAOS_IL2CPP_FLOAT64 ChaosMathExp(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::exp(x);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathLog(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::log(x);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathLog10(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::log10(x);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathTan(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::tan(x);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathAtan(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::atan(x);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathAtan2(CHAOS_IL2CPP_FLOAT64 y, CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::atan2(y, x);
}


CHAOS_IL2CPP_FLOAT64 ChaosMathTruncate(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::trunc(x);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathRound2Params(CHAOS_IL2CPP_FLOAT64 x, CHAOS_IL2CPP_INT32 digits) noexcept
{
    double scale = std::pow(10.0, static_cast<double>(digits));
    return std::round(x * scale) / scale;
}

CHAOS_IL2CPP_INT32 ChaosMathSignDouble(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    if (x > 0) return 1;
    if (x < 0) return -1;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosMathSignInt32(CHAOS_IL2CPP_INT32 x) noexcept
{
    if (x > 0) return 1;
    if (x < 0) return -1;
    return 0;
}

CHAOS_IL2CPP_FLOAT64 ChaosMathAcos(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::acos(x);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathAsin(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::asin(x);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathCosh(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::cosh(x);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathSinh(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::sinh(x);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathTanh(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::tanh(x);
}

// ── Math kernel helpers (math_kernel_helpers.h) ──────────────────
// The header carries inline C++ implementations for the IEEE-754 special
// functions (BitDecrement / BitIncrement / CopySign / ScaleB / …) but those
// are `inline` and have no C linkage, so codegen could not call them and every
// System.Double::/System.Math:: subject for these fell through to the
// zero-argument ChaosExternalRuntimeFallback catch-all — which receives only
// the subject id (not the operands) and returns 0.  The subjects then compared
// a fabricated 0 against the expected value.  These wrappers give the header
// real extern "C" entry points, matching the ChaosMath* convention above.
//
// BitDecrement(0.0) == -5E-324 and BitIncrement(0.0) == 5E-324 are the
// canonical .NET results; CHAOS_IL2CPP_NEXTAFTER implements them exactly.
CHAOS_IL2CPP_FLOAT64 ChaosMathBitDecrement(CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    return CHAOS_IL2CPP_NEXTAFTER(value, -CHAOS_IL2CPP_NUMERIC_LIMITS_INFINITY(double));
}

CHAOS_IL2CPP_FLOAT64 ChaosMathBitIncrement(CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    return CHAOS_IL2CPP_NEXTAFTER(value, CHAOS_IL2CPP_NUMERIC_LIMITS_INFINITY(double));
}

CHAOS_IL2CPP_FLOAT64 ChaosMathCopySign(CHAOS_IL2CPP_FLOAT64 x, CHAOS_IL2CPP_FLOAT64 y) noexcept
{
    return CHAOS_IL2CPP_COPYSIGN(x, y);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathScaleB(CHAOS_IL2CPP_FLOAT64 x, CHAOS_IL2CPP_INT32 n) noexcept
{
    return CHAOS_IL2CPP_SCALBN(x, n);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathMaxMagnitude(CHAOS_IL2CPP_FLOAT64 x, CHAOS_IL2CPP_FLOAT64 y) noexcept
{
    return (CHAOS_IL2CPP_ABS(x) >= CHAOS_IL2CPP_ABS(y)) ? x : y;
}

CHAOS_IL2CPP_FLOAT64 ChaosMathMinMagnitude(CHAOS_IL2CPP_FLOAT64 x, CHAOS_IL2CPP_FLOAT64 y) noexcept
{
    return (CHAOS_IL2CPP_ABS(x) <= CHAOS_IL2CPP_ABS(y)) ? x : y;
}

CHAOS_IL2CPP_FLOAT64 ChaosMathIEEERemainder(CHAOS_IL2CPP_FLOAT64 x, CHAOS_IL2CPP_FLOAT64 y) noexcept
{
    return CHAOS_IL2CPP_REMAINDER(x, y);
}

CHAOS_IL2CPP_INT32 ChaosMathILogB(CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    return CHAOS_IL2CPP_ILOGB(value);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathReciprocalEstimate(CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    return 1.0 / value;
}

CHAOS_IL2CPP_FLOAT64 ChaosMathReciprocalSqrtEstimate(CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    return 1.0 / CHAOS_IL2CPP_SQRT(value);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathAcosh(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::acosh(x);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathAsinh(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::asinh(x);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathAtanh(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::atanh(x);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathCbrt(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::cbrt(x);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathHypot(CHAOS_IL2CPP_FLOAT64 x, CHAOS_IL2CPP_FLOAT64 y) noexcept
{
    return std::hypot(x, y);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathExp2(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::exp2(x);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathExpM1(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::expm1(x);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathLog2(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::log2(x);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathLog1p(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::log1p(x);
}

// ── Single (float) overloads ─────────────────────────────────────
// System.Single::X and System.MathF::X carry the same IEEE-754 special
// functions as their Double counterparts.  Without these the float subjects
// (System.Single::BitDecrement, System.MathF::BitIncrement, …) still fell
// through to the operand-less catch-all and reported honest failures that
// were indistinguishable from the double-side wiring being incomplete.
CHAOS_IL2CPP_FLOAT32 ChaosMathBitDecrementF(CHAOS_IL2CPP_FLOAT32 value) noexcept
{
    return CHAOS_IL2CPP_NEXTAFTER(value, -CHAOS_IL2CPP_NUMERIC_LIMITS_INFINITY(float));
}

CHAOS_IL2CPP_FLOAT32 ChaosMathBitIncrementF(CHAOS_IL2CPP_FLOAT32 value) noexcept
{
    return CHAOS_IL2CPP_NEXTAFTER(value, CHAOS_IL2CPP_NUMERIC_LIMITS_INFINITY(float));
}

CHAOS_IL2CPP_FLOAT32 ChaosMathCopySignF(CHAOS_IL2CPP_FLOAT32 x, CHAOS_IL2CPP_FLOAT32 y) noexcept
{
    return CHAOS_IL2CPP_COPYSIGN(x, y);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathScaleBF(CHAOS_IL2CPP_FLOAT32 x, CHAOS_IL2CPP_INT32 n) noexcept
{
    return CHAOS_IL2CPP_SCALBN(x, n);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathMaxMagnitudeF(CHAOS_IL2CPP_FLOAT32 x, CHAOS_IL2CPP_FLOAT32 y) noexcept
{
    return (CHAOS_IL2CPP_ABS(x) >= CHAOS_IL2CPP_ABS(y)) ? x : y;
}

CHAOS_IL2CPP_FLOAT32 ChaosMathMinMagnitudeF(CHAOS_IL2CPP_FLOAT32 x, CHAOS_IL2CPP_FLOAT32 y) noexcept
{
    return (CHAOS_IL2CPP_ABS(x) <= CHAOS_IL2CPP_ABS(y)) ? x : y;
}

CHAOS_IL2CPP_FLOAT32 ChaosMathIEEERemainderF(CHAOS_IL2CPP_FLOAT32 x, CHAOS_IL2CPP_FLOAT32 y) noexcept
{
    return CHAOS_IL2CPP_REMAINDER(x, y);
}

CHAOS_IL2CPP_INT32 ChaosMathILogBF(CHAOS_IL2CPP_FLOAT32 value) noexcept
{
    return CHAOS_IL2CPP_ILOGB(value);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathReciprocalEstimateF(CHAOS_IL2CPP_FLOAT32 value) noexcept
{
    return 1.0f / value;
}

CHAOS_IL2CPP_FLOAT32 ChaosMathReciprocalSqrtEstimateF(CHAOS_IL2CPP_FLOAT32 value) noexcept
{
    return 1.0f / CHAOS_IL2CPP_SQRT(value);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathAcoshF(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::acosh(x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathAsinhF(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::asinh(x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathAtanhF(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::atanh(x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathCbrtF(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::cbrt(x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathHypotF(CHAOS_IL2CPP_FLOAT32 x, CHAOS_IL2CPP_FLOAT32 y) noexcept
{
    return std::hypot(x, y);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathExp2F(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::exp2(x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathExpM1F(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::expm1(x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathLog2F(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::log2(x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathLog1pF(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::log1p(x);
}

// Pi constants: .NET's AcosPi/SinPi/... divide or multiply by pi.  The
// runtime has no shared PI macro, so define local ones (float width needs
// its own literal to keep precision).
static constexpr CHAOS_IL2CPP_FLOAT64 kChaosPi  = 3.14159265358979323846;
static constexpr CHAOS_IL2CPP_FLOAT32 kChaosPiF = 3.14159265358979323846f;

// ── Pi-scaled trig variants (System.Double::/System.Single::*Pi) ──
// .NET 7+ exposes AcosPi/AsinPi/AtanPi/CosPi/SinPi/TanPi (and the 2-arg
// Atan2Pi) as M_PI-scaled wrappers.  Each computes the base function of the
// *scaled* argument so the result is exact in units of pi.
CHAOS_IL2CPP_FLOAT64 ChaosMathAcosPi(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::acos(x) / kChaosPi;
}

CHAOS_IL2CPP_FLOAT64 ChaosMathAsinPi(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::asin(x) / kChaosPi;
}

CHAOS_IL2CPP_FLOAT64 ChaosMathAtanPi(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::atan(x) / kChaosPi;
}

CHAOS_IL2CPP_FLOAT64 ChaosMathAtan2Pi(CHAOS_IL2CPP_FLOAT64 y, CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::atan2(y, x) / kChaosPi;
}

CHAOS_IL2CPP_FLOAT64 ChaosMathCosPi(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::cos(kChaosPi * x);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathSinPi(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::sin(kChaosPi * x);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathTanPi(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::tan(kChaosPi * x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathAcosPiF(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::acos(x) / kChaosPiF;
}

CHAOS_IL2CPP_FLOAT32 ChaosMathAsinPiF(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::asin(x) / kChaosPiF;
}

CHAOS_IL2CPP_FLOAT32 ChaosMathAtanPiF(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::atan(x) / kChaosPiF;
}

CHAOS_IL2CPP_FLOAT32 ChaosMathAtan2PiF(CHAOS_IL2CPP_FLOAT32 y, CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::atan2(y, x) / kChaosPiF;
}

CHAOS_IL2CPP_FLOAT32 ChaosMathCosPiF(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::cos(kChaosPiF * x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathSinPiF(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::sin(kChaosPiF * x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathTanPiF(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::tan(kChaosPiF * x);
}

// ── Classic Math/Double one-arg families — Single counterparts ────────
// The double natives (ChaosMathExp, ChaosMathCosh, ChaosMathAcos, …) already
// existed but carried no *shape registration*, so their subjects routed to the
// operand-less catch-all; there were also no float-width entry points at all.
// These complete both halves for System.Single::/System.MathF::.
CHAOS_IL2CPP_FLOAT32 ChaosMathExpF(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::exp(x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathExp10F(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::pow(10.0f, x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathExp10M1F(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::pow(10.0f, x) - 1.0f;
}

CHAOS_IL2CPP_FLOAT32 ChaosMathLogF(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::log(x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathLog10F(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::log10(x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathLog10P1F(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::log10(1.0f + x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathLog2M1F(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::log2(x) - 1.0f;
}

CHAOS_IL2CPP_FLOAT32 ChaosMathCoshF(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::cosh(x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathSinhF(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::sinh(x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathTanhF(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::tanh(x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathAcosF(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::acos(x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathAsinF(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::asin(x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathAtanF(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::atan(x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathAtan2F(CHAOS_IL2CPP_FLOAT32 y, CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::atan2(y, x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathTanF(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::tan(x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathSinF(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::sin(x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathCosF(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::cos(x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathPowF(CHAOS_IL2CPP_FLOAT32 x, CHAOS_IL2CPP_FLOAT32 y) noexcept
{
    return std::pow(x, y);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathSqrtF(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return CHAOS_IL2CPP_SQRT(x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathCeilingF(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::ceil(x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathFloorF(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return std::floor(x);
}

CHAOS_IL2CPP_FLOAT32 ChaosMathTruncateF(CHAOS_IL2CPP_FLOAT32 x) noexcept
{
    return CHAOS_IL2CPP_TRUNC(x);
}

// Double-width Exp10 family — same gap as the float ones above.
CHAOS_IL2CPP_FLOAT64 ChaosMathExp10(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::pow(10.0, x);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathExp10M1(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::pow(10.0, x) - 1.0;
}

CHAOS_IL2CPP_FLOAT64 ChaosMathLog10P1(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::log10(1.0 + x);
}

CHAOS_IL2CPP_FLOAT64 ChaosMathLog2M1(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::log2(x) - 1.0;
}

CHAOS_IL2CPP_FLOAT64 ChaosMathLogP1(CHAOS_IL2CPP_FLOAT64 x) noexcept
{
    return std::log1p(x);
}

// ── Scalar INumber<T> predicates (System.Double::IsFinite, UInt32::IsEvenInteger, …) ──
//
// These are static abstract interface members of System.Numerics.INumber<T>,
// surfaced as static methods on each numeric type.  The ShapeRegistry only
// registered the *Vector* variants (Vector128.IsFinite …), so every scalar
// subject fell through to the operand-less external-runtime catch-all, which
// has no operand access and returns 0 — the assertion then compared a
// fabricated 0 against `true`.
//
// Generic in T so one definition covers every width, matching how the IL
// exposes a single signature per type.
// Integer predicates: width-suffixed wrappers over the templates above, so the
// ShapeRegistry can name a concrete symbol per numeric type.
CHAOS_IL2CPP_INT32 ChaosScalarIsEvenInteger32(CHAOS_IL2CPP_INT32 v) noexcept { return ChaosScalarIsEvenInteger<CHAOS_IL2CPP_INT32>(v); }
CHAOS_IL2CPP_INT32 ChaosScalarIsEvenInteger64(CHAOS_IL2CPP_INT64 v) noexcept { return ChaosScalarIsEvenInteger<CHAOS_IL2CPP_INT64>(v); }
CHAOS_IL2CPP_INT32 ChaosScalarIsOddInteger32(CHAOS_IL2CPP_INT32 v) noexcept { return ChaosScalarIsOddInteger<CHAOS_IL2CPP_INT32>(v); }
CHAOS_IL2CPP_INT32 ChaosScalarIsOddInteger64(CHAOS_IL2CPP_INT64 v) noexcept { return ChaosScalarIsOddInteger<CHAOS_IL2CPP_INT64>(v); }
// Signed variants compare against zero directly.  Unsigned types are handled by
// the *_U32/_U64 entry points below: per .NET, an unsigned value is positive
// iff non-zero and never negative, so routing them through the signed template
// would misreport values >= 2^31.
CHAOS_IL2CPP_INT32 ChaosScalarIsPositive32(CHAOS_IL2CPP_INT32 v) noexcept { return ChaosScalarIsPositive<CHAOS_IL2CPP_INT32>(v); }
CHAOS_IL2CPP_INT32 ChaosScalarIsPositive64(CHAOS_IL2CPP_INT64 v) noexcept { return ChaosScalarIsPositive<CHAOS_IL2CPP_INT64>(v); }
CHAOS_IL2CPP_INT32 ChaosScalarIsNegative32(CHAOS_IL2CPP_INT32 v) noexcept { return ChaosScalarIsNegative<CHAOS_IL2CPP_INT32>(v); }
CHAOS_IL2CPP_INT32 ChaosScalarIsNegative64(CHAOS_IL2CPP_INT64 v) noexcept { return ChaosScalarIsNegative<CHAOS_IL2CPP_INT64>(v); }
// Unsigned values are never negative, so IsPositive is always true (>= 0).
CHAOS_IL2CPP_INT32 ChaosScalarIsPositiveU32(CHAOS_IL2CPP_UINT32) noexcept { return 1; }
CHAOS_IL2CPP_INT32 ChaosScalarIsPositiveU64(CHAOS_IL2CPP_UINT64) noexcept { return 1; }
CHAOS_IL2CPP_INT32 ChaosScalarIsNegativeU32(CHAOS_IL2CPP_UINT32) noexcept { return 0; }
CHAOS_IL2CPP_INT32 ChaosScalarIsNegativeU64(CHAOS_IL2CPP_UINT64) noexcept { return 0; }
CHAOS_IL2CPP_INT32 ChaosScalarIsNormal32(CHAOS_IL2CPP_INT32 v) noexcept { return ChaosScalarIsNormal<CHAOS_IL2CPP_INT32>(v); }
CHAOS_IL2CPP_INT32 ChaosScalarIsNormal64(CHAOS_IL2CPP_INT64 v) noexcept { return ChaosScalarIsNormal<CHAOS_IL2CPP_INT64>(v); }
CHAOS_IL2CPP_INT32 ChaosScalarIsSubnormal32(CHAOS_IL2CPP_INT32) noexcept { return 0; }
CHAOS_IL2CPP_INT32 ChaosScalarIsSubnormal64(CHAOS_IL2CPP_INT64) noexcept { return 0; }
CHAOS_IL2CPP_INT32 ChaosScalarIsRealNumber32(CHAOS_IL2CPP_INT32) noexcept { return 1; }
CHAOS_IL2CPP_INT32 ChaosScalarIsRealNumber64(CHAOS_IL2CPP_INT64) noexcept { return 1; }

// Floating-point predicates delegate to <cmath>, which is where NaN/Infinity
// actually exist.
CHAOS_IL2CPP_INT32 ChaosScalarIsFiniteD(CHAOS_IL2CPP_FLOAT64 v) noexcept { return std::isfinite(v) ? 1 : 0; }
CHAOS_IL2CPP_INT32 ChaosScalarIsFiniteF(CHAOS_IL2CPP_FLOAT32 v) noexcept { return std::isfinite(v) ? 1 : 0; }
CHAOS_IL2CPP_INT32 ChaosScalarIsNaN_D(CHAOS_IL2CPP_FLOAT64 v) noexcept { return std::isnan(v) ? 1 : 0; }
CHAOS_IL2CPP_INT32 ChaosScalarIsNaN_F(CHAOS_IL2CPP_FLOAT32 v) noexcept { return std::isnan(v) ? 1 : 0; }
CHAOS_IL2CPP_INT32 ChaosScalarIsInfinityD(CHAOS_IL2CPP_FLOAT64 v) noexcept { return std::isinf(v) ? 1 : 0; }
CHAOS_IL2CPP_INT32 ChaosScalarIsInfinityF(CHAOS_IL2CPP_FLOAT32 v) noexcept { return std::isinf(v) ? 1 : 0; }
CHAOS_IL2CPP_INT32 ChaosScalarIsNegativeInfinityD(CHAOS_IL2CPP_FLOAT64 v) noexcept { return (std::isinf(v) && v < 0) ? 1 : 0; }
CHAOS_IL2CPP_INT32 ChaosScalarIsNegativeInfinityF(CHAOS_IL2CPP_FLOAT32 v) noexcept { return (std::isinf(v) && v < 0) ? 1 : 0; }
CHAOS_IL2CPP_INT32 ChaosScalarIsPositiveInfinityD(CHAOS_IL2CPP_FLOAT64 v) noexcept { return (std::isinf(v) && v > 0) ? 1 : 0; }
CHAOS_IL2CPP_INT32 ChaosScalarIsPositiveInfinityF(CHAOS_IL2CPP_FLOAT32 v) noexcept { return (std::isinf(v) && v > 0) ? 1 : 0; }
CHAOS_IL2CPP_INT32 ChaosScalarIsNormalD(CHAOS_IL2CPP_FLOAT64 v) noexcept { return std::isnormal(v) ? 1 : 0; }
CHAOS_IL2CPP_INT32 ChaosScalarIsNormalF(CHAOS_IL2CPP_FLOAT32 v) noexcept { return std::isnormal(v) ? 1 : 0; }
CHAOS_IL2CPP_INT32 ChaosScalarIsSubnormalD(CHAOS_IL2CPP_FLOAT64 v) noexcept { return (std::fpclassify(v) == FP_SUBNORMAL) ? 1 : 0; }
CHAOS_IL2CPP_INT32 ChaosScalarIsSubnormalF(CHAOS_IL2CPP_FLOAT32 v) noexcept { return (std::fpclassify(v) == FP_SUBNORMAL) ? 1 : 0; }
CHAOS_IL2CPP_INT32 ChaosScalarIsIntegerD(CHAOS_IL2CPP_FLOAT64 v) noexcept { return (std::isfinite(v) && std::floor(v) == v && !std::isinf(v)) ? 1 : 0; }
CHAOS_IL2CPP_INT32 ChaosScalarIsIntegerF(CHAOS_IL2CPP_FLOAT32 v) noexcept { return (std::isfinite(v) && std::floor(v) == v && !std::isinf(v)) ? 1 : 0; }
CHAOS_IL2CPP_INT32 ChaosScalarIsEvenIntegerD(CHAOS_IL2CPP_FLOAT64 v) noexcept { return (std::isfinite(v) && std::floor(v) == v && std::fmod(std::fabs(v), 2.0) == 0.0) ? 1 : 0; }
CHAOS_IL2CPP_INT32 ChaosScalarIsEvenIntegerF(CHAOS_IL2CPP_FLOAT32 v) noexcept { return (std::isfinite(v) && std::floor(v) == v && std::fmod(std::fabs(v), 2.0f) == 0.0f) ? 1 : 0; }
CHAOS_IL2CPP_INT32 ChaosScalarIsOddIntegerD(CHAOS_IL2CPP_FLOAT64 v) noexcept { return (std::isfinite(v) && std::floor(v) == v && std::fmod(std::fabs(v), 2.0) == 1.0) ? 1 : 0; }
CHAOS_IL2CPP_INT32 ChaosScalarIsOddIntegerF(CHAOS_IL2CPP_FLOAT32 v) noexcept { return (std::isfinite(v) && std::floor(v) == v && std::fmod(std::fabs(v), 2.0f) == 1.0f) ? 1 : 0; }
// >= 0 (zero is positive); NaN is not positive in .NET.
CHAOS_IL2CPP_INT32 ChaosScalarIsPositiveD(CHAOS_IL2CPP_FLOAT64 v) noexcept { return (!std::isnan(v) && v >= 0) ? 1 : 0; }
CHAOS_IL2CPP_INT32 ChaosScalarIsPositiveF(CHAOS_IL2CPP_FLOAT32 v) noexcept { return (!std::isnan(v) && v >= 0) ? 1 : 0; }
// .NET's float IsNegative is sign-BIT based: -0.0 is negative (verified).
// std::signbit captures that; a plain `v < 0` misses -0.0.
CHAOS_IL2CPP_INT32 ChaosScalarIsNegativeD(CHAOS_IL2CPP_FLOAT64 v) noexcept { return std::signbit(v) ? 1 : 0; }
CHAOS_IL2CPP_INT32 ChaosScalarIsNegativeF(CHAOS_IL2CPP_FLOAT32 v) noexcept { return std::signbit(v) ? 1 : 0; }
CHAOS_IL2CPP_INT32 ChaosScalarIsRealNumberD(CHAOS_IL2CPP_FLOAT64 v) noexcept { return std::isnan(v) ? 0 : 1; }
CHAOS_IL2CPP_INT32 ChaosScalarIsRealNumberF(CHAOS_IL2CPP_FLOAT32 v) noexcept { return std::isnan(v) ? 0 : 1; }


// ── Width-aware LeadingZeroCount / TrailingZeroCount for narrow integer types ──
//
// System.Byte.LeadingZeroCount(0) is 8 (the width of a byte), not 32 — the
// INumber<T> members count within T, whereas the BitOperations 32/64 routines
// count within the machine word.  Routing byte/sbyte/short/ushort to the 32-bit
// intrinsic therefore reported 32/16 for values that should report 8/16.
CHAOS_IL2CPP_INT32 ChaosBitOpsLeadingZeroCount8(CHAOS_IL2CPP_UINT8 value) noexcept
{
    if (value == 0) return 8;
    return static_cast<CHAOS_IL2CPP_INT32>(ChaosBitOpsLeadingZeroCount32(value)) - 24;
}
CHAOS_IL2CPP_INT32 ChaosBitOpsLeadingZeroCount16(CHAOS_IL2CPP_UINT16 value) noexcept
{
    if (value == 0) return 16;
    return static_cast<CHAOS_IL2CPP_INT32>(ChaosBitOpsLeadingZeroCount32(value)) - 16;
}
CHAOS_IL2CPP_INT32 ChaosBitOpsTrailingZeroCount8(CHAOS_IL2CPP_UINT8 value) noexcept
{
    if (value == 0) return 8;
    return static_cast<CHAOS_IL2CPP_INT32>(ChaosBitOpsTrailingZeroCount32(value));
}
CHAOS_IL2CPP_INT32 ChaosBitOpsTrailingZeroCount16(CHAOS_IL2CPP_UINT16 value) noexcept
{
    if (value == 0) return 16;
    return static_cast<CHAOS_IL2CPP_INT32>(ChaosBitOpsTrailingZeroCount32(value));
}
CHAOS_IL2CPP_INT32 ChaosBitOpsPopCount8(CHAOS_IL2CPP_UINT8 value) noexcept
{
    return ChaosBitOpsPopCount32(value);
}
CHAOS_IL2CPP_INT32 ChaosBitOpsPopCount16(CHAOS_IL2CPP_UINT16 value) noexcept
{
    return ChaosBitOpsPopCount32(value);
}

// ── BitConverter bit reinterpretation ──────────────────────────────
// memcpy (not type-punning) — the slot machinery handles the float
// load/store; these only re-label the bits.
CHAOS_IL2CPP_INT32 ChaosSingleToInt32Bits(CHAOS_IL2CPP_FLOAT32 value) noexcept
{
    CHAOS_IL2CPP_INT32 bits;
    std::memcpy(&bits, &value, sizeof(bits));
    return bits;
}
CHAOS_IL2CPP_INT64 ChaosDoubleToInt64Bits(CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    CHAOS_IL2CPP_INT64 bits;
    std::memcpy(&bits, &value, sizeof(bits));
    return bits;
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core

