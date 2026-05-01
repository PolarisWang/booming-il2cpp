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

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_HALF_CLASSIFICATION_H_
