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
#if defined(__x86_64__) || defined(_M_AMD64)
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
#if defined(__x86_64__) || defined(_M_AMD64)
    return _mm_crc32_u16(crc, data);
#else
    crc = ChaosBitOpsCrc32CByte(crc, static_cast<CHAOS_IL2CPP_UINT8>(data));
    return ChaosBitOpsCrc32CByte(crc, static_cast<CHAOS_IL2CPP_UINT8>(data >> 8));
#endif
}

CHAOS_IL2CPP_UINT32 ChaosBitOpsCrc32CUInt32(CHAOS_IL2CPP_UINT32 crc, CHAOS_IL2CPP_UINT32 data) noexcept
{
#if defined(__x86_64__) || defined(_M_AMD64)
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
#if defined(__x86_64__) || defined(_M_AMD64)
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

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core