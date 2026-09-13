#ifndef CHAOS_IL2CPP_HALF_CLASSIFICATION_H_
#define CHAOS_IL2CPP_HALF_CLASSIFICATION_H_

#include <chaos/native_types.h>

namespace chaos::il2cpp::runtime_core {

bool HalfIsFinite(CHAOS_IL2CPP_UINT16 value);
bool HalfIsEvenInteger(CHAOS_IL2CPP_UINT16 value);
bool HalfIsNaN(CHAOS_IL2CPP_UINT16 value);
bool HalfIsNaNOrZero(CHAOS_IL2CPP_UINT16 value);
bool HalfIsNegative(CHAOS_IL2CPP_UINT16 value);
bool HalfIsNegativeInfinity(CHAOS_IL2CPP_UINT16 value);
bool HalfIsNormal(CHAOS_IL2CPP_UINT16 value);
bool HalfIsOddInteger(CHAOS_IL2CPP_UINT16 value);
bool HalfIsPositive(CHAOS_IL2CPP_UINT16 value);
bool HalfIsPositiveInfinity(CHAOS_IL2CPP_UINT16 value);
bool HalfIsPow2(CHAOS_IL2CPP_UINT16 value);
bool HalfIsRealNumber(CHAOS_IL2CPP_UINT16 value);
bool HalfIsSubnormal(CHAOS_IL2CPP_UINT16 value);
bool HalfIsZero(CHAOS_IL2CPP_UINT16 value);
bool HalfIsInteger(CHAOS_IL2CPP_UINT16 value);
bool HalfIsInfinity(CHAOS_IL2CPP_UINT16 value);
CHAOS_IL2CPP_INT32 HalfCompare(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value);
bool HalfEquals(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value);
bool HalfOperatorEquals(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value);
bool HalfOperatorLessThan(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value);
bool HalfOperatorLessThanOrEqual(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value);
bool HalfOperatorGreaterThan(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value);
bool HalfOperatorGreaterThanOrEqual(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value);

// ── IEEE 754-2019 arithmetic (IFloatingPointIeee754<Half>) ──────────────
CHAOS_IL2CPP_UINT16 HalfCopySign(CHAOS_IL2CPP_UINT16 magnitude, CHAOS_IL2CPP_UINT16 sign);
CHAOS_IL2CPP_UINT16 HalfMaxNumber(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value);
CHAOS_IL2CPP_UINT16 HalfMinNumber(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value);
CHAOS_IL2CPP_UINT16 HalfMaxMagnitude(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value);
CHAOS_IL2CPP_UINT16 HalfMinMagnitude(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value);
CHAOS_IL2CPP_UINT16 HalfMaxMagnitudeNumber(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value);
CHAOS_IL2CPP_UINT16 HalfMinMagnitudeNumber(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value);
CHAOS_IL2CPP_UINT16 HalfCeiling(CHAOS_IL2CPP_UINT16 value);
CHAOS_IL2CPP_UINT16 HalfFloor(CHAOS_IL2CPP_UINT16 value);
CHAOS_IL2CPP_UINT16 HalfTruncate(CHAOS_IL2CPP_UINT16 value);
CHAOS_IL2CPP_UINT16 HalfRoundToEven(CHAOS_IL2CPP_UINT16 value);
CHAOS_IL2CPP_UINT16 HalfSqrt(CHAOS_IL2CPP_UINT16 value);
CHAOS_IL2CPP_UINT16 HalfAbs(CHAOS_IL2CPP_UINT16 value);
CHAOS_IL2CPP_INT32  HalfSign(CHAOS_IL2CPP_UINT16 value);

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_HALF_CLASSIFICATION_H_
