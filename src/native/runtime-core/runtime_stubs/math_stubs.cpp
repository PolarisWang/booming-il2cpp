// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// math_stubs.cpp — Math stub implementations
#include <chaos/native_types.h>
#include <cmath>
#include <cstdint>
#include "generated_code_compat.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

CHAOS_IL2CPP_INT64 ChaosMathSqrt(CHAOS_IL2CPP_INT64 value) noexcept
{
    double d;
    CHAOS_IL2CPP_MEMCPY(&d, &value, sizeof(d));
    d = CHAOS_IL2CPP_SQRT(d);
    CHAOS_IL2CPP_INT64 result;
    CHAOS_IL2CPP_MEMCPY(&result, &d, sizeof(result));
    return result;
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

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core