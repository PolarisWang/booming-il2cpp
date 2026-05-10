// math_stubs.cpp — Math stub implementations
#include <chaos/native_types.h>
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

// ── Math stubs (return 0 for methods not yet implemented) ──
CHAOS_IL2CPP_INT32 ChaosMathAbsInt32(CHAOS_IL2CPP_INT32 value) noexcept { (void)value; return 0; }
CHAOS_IL2CPP_FLOAT64 ChaosMathAbsDouble(CHAOS_IL2CPP_FLOAT64 value) noexcept { (void)value; return 0.0; }
CHAOS_IL2CPP_FLOAT64 ChaosMathCeiling(CHAOS_IL2CPP_FLOAT64 value) noexcept { (void)value; return 0.0; }
CHAOS_IL2CPP_FLOAT64 ChaosMathFloor(CHAOS_IL2CPP_FLOAT64 value) noexcept { (void)value; return 0.0; }
CHAOS_IL2CPP_FLOAT64 ChaosMathRound(CHAOS_IL2CPP_FLOAT64 value) noexcept { (void)value; return 0.0; }
CHAOS_IL2CPP_FLOAT64 ChaosMathRound2(CHAOS_IL2CPP_FLOAT64 value, CHAOS_IL2CPP_INT32 digits) noexcept { (void)value; (void)digits; return 0.0; }
CHAOS_IL2CPP_INT32 ChaosMathMaxInt32(CHAOS_IL2CPP_INT32 a, CHAOS_IL2CPP_INT32 b) noexcept { (void)a; (void)b; return 0; }
CHAOS_IL2CPP_FLOAT64 ChaosMathMaxDouble(CHAOS_IL2CPP_FLOAT64 a, CHAOS_IL2CPP_FLOAT64 b) noexcept { (void)a; (void)b; return 0.0; }
CHAOS_IL2CPP_INT32 ChaosMathMinInt32(CHAOS_IL2CPP_INT32 a, CHAOS_IL2CPP_INT32 b) noexcept { (void)a; (void)b; return 0; }
CHAOS_IL2CPP_FLOAT64 ChaosMathPow(CHAOS_IL2CPP_FLOAT64 x, CHAOS_IL2CPP_FLOAT64 y) noexcept { (void)x; (void)y; return 0.0; }
CHAOS_IL2CPP_FLOAT64 ChaosMathSin(CHAOS_IL2CPP_FLOAT64 x) noexcept { (void)x; return 0.0; }
CHAOS_IL2CPP_FLOAT64 ChaosMathCos(CHAOS_IL2CPP_FLOAT64 x) noexcept { (void)x; return 0.0; }
CHAOS_IL2CPP_INT64 ChaosMathBigMul(CHAOS_IL2CPP_INT32 a, CHAOS_IL2CPP_INT32 b) noexcept { (void)a; (void)b; return 0; }

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core