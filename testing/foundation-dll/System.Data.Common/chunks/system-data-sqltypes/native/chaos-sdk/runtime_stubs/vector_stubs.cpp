#include <chaos/native_types.h>
#include <cstring>
#include "vector_fixed_templates.h"
#include <runtime_abi.h>

// ── Vector reduction stubs for Vector<int> (32-byte carrier, 8 lanes) ──
// Each stub: compare two Vector<int> values lane-by-lane, then check any/all lanes.

using namespace chaos::il2cpp::vector_fixed;
using TCarrier = chaos::il2cpp::numerics_carriers::RuntimeIntrinsicVector256Carrier;
using TScalar = CHAOS_IL2CPP_INT32;

extern "C" CHAOS_IL2CPP_INT32 chaos_vector_greater_than_any(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right)
{
    auto& lv = *reinterpret_cast<const TCarrier*>(left);
    auto& rv = *reinterpret_cast<const TCarrier*>(right);
    auto mask = VectorFixedCompareGreaterThan<TScalar, TScalar, TCarrier>(lv, rv);
    return VectorFixedAnyLaneNonZero<TCarrier>(mask) ? 1 : 0;
}

extern "C" CHAOS_IL2CPP_INT32 chaos_vector_greater_than_all(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right)
{
    auto& lv = *reinterpret_cast<const TCarrier*>(left);
    auto& rv = *reinterpret_cast<const TCarrier*>(right);
    auto mask = VectorFixedCompareGreaterThan<TScalar, TScalar, TCarrier>(lv, rv);
    return VectorFixedAllLanesNonZero<TScalar, TCarrier>(mask) ? 1 : 0;
}

extern "C" CHAOS_IL2CPP_INT32 chaos_vector_less_than_any(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right)
{
    auto& lv = *reinterpret_cast<const TCarrier*>(left);
    auto& rv = *reinterpret_cast<const TCarrier*>(right);
    auto mask = VectorFixedCompareLessThan<TScalar, TScalar, TCarrier>(lv, rv);
    return VectorFixedAnyLaneNonZero<TCarrier>(mask) ? 1 : 0;
}

extern "C" CHAOS_IL2CPP_INT32 chaos_vector_less_than_all(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right)
{
    auto& lv = *reinterpret_cast<const TCarrier*>(left);
    auto& rv = *reinterpret_cast<const TCarrier*>(right);
    auto mask = VectorFixedCompareLessThan<TScalar, TScalar, TCarrier>(lv, rv);
    return VectorFixedAllLanesNonZero<TScalar, TCarrier>(mask) ? 1 : 0;
}

extern "C" CHAOS_IL2CPP_INT32 chaos_vector_greater_than_or_equal_any(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right)
{
    auto& lv = *reinterpret_cast<const TCarrier*>(left);
    auto& rv = *reinterpret_cast<const TCarrier*>(right);
    auto mask = VectorFixedCompareGreaterThanOrEqual<TScalar, TScalar, TCarrier>(lv, rv);
    return VectorFixedAnyLaneNonZero<TCarrier>(mask) ? 1 : 0;
}

extern "C" CHAOS_IL2CPP_INT32 chaos_vector_greater_than_or_equal_all(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right)
{
    auto& lv = *reinterpret_cast<const TCarrier*>(left);
    auto& rv = *reinterpret_cast<const TCarrier*>(right);
    auto mask = VectorFixedCompareGreaterThanOrEqual<TScalar, TScalar, TCarrier>(lv, rv);
    return VectorFixedAllLanesNonZero<TScalar, TCarrier>(mask) ? 1 : 0;
}

extern "C" CHAOS_IL2CPP_INT32 chaos_vector_less_than_or_equal_any(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right)
{
    auto& lv = *reinterpret_cast<const TCarrier*>(left);
    auto& rv = *reinterpret_cast<const TCarrier*>(right);
    auto mask = VectorFixedCompareLessThanOrEqual<TScalar, TScalar, TCarrier>(lv, rv);
    return VectorFixedAnyLaneNonZero<TCarrier>(mask) ? 1 : 0;
}

extern "C" CHAOS_IL2CPP_INT32 chaos_vector_less_than_or_equal_all(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right)
{
    auto& lv = *reinterpret_cast<const TCarrier*>(left);
    auto& rv = *reinterpret_cast<const TCarrier*>(right);
    auto mask = VectorFixedCompareLessThanOrEqual<TScalar, TScalar, TCarrier>(lv, rv);
    return VectorFixedAllLanesNonZero<TScalar, TCarrier>(mask) ? 1 : 0;
}
