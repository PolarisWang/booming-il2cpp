#include <chaos/native_types.h>
#include <cstring>
#include "vector_fixed_templates.h"
#include <runtime_abi.h>

// ── Vector reduction stubs for all Vector<T> element types ─────
// System.Numerics.Vector<T> is always 32 bytes (Vector256) on x64.
// Number of lanes = 32 / sizeof(T).
// Templates (VectorFixedCompareGreaterThan etc.) handle all types generically.
// Macro generates one overload per scalar type for each of the 8 reduction operators.

using namespace chaos::il2cpp::vector_fixed;
using TCarrier = chaos::il2cpp::numerics_carriers::RuntimeIntrinsicVector256Carrier;

// ── Macro: generate all 8 reduction stubs for a given scalar type and name suffix ──
#define VECTOR_REDUCTION_STUBS(scalar, suffix) \
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_greater_than_any_##suffix(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right) { \
    auto& lv = *reinterpret_cast<const TCarrier*>(left); \
    auto& rv = *reinterpret_cast<const TCarrier*>(right); \
    auto mask = VectorFixedCompareGreaterThan<scalar, scalar, TCarrier>(lv, rv); \
    return VectorFixedAnyLaneNonZero<TCarrier>(mask) ? 1 : 0; \
} \
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_greater_than_all_##suffix(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right) { \
    auto& lv = *reinterpret_cast<const TCarrier*>(left); \
    auto& rv = *reinterpret_cast<const TCarrier*>(right); \
    auto mask = VectorFixedCompareGreaterThan<scalar, scalar, TCarrier>(lv, rv); \
    return VectorFixedAllLanesNonZero<scalar, TCarrier>(mask) ? 1 : 0; \
} \
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_less_than_any_##suffix(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right) { \
    auto& lv = *reinterpret_cast<const TCarrier*>(left); \
    auto& rv = *reinterpret_cast<const TCarrier*>(right); \
    auto mask = VectorFixedCompareLessThan<scalar, scalar, TCarrier>(lv, rv); \
    return VectorFixedAnyLaneNonZero<TCarrier>(mask) ? 1 : 0; \
} \
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_less_than_all_##suffix(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right) { \
    auto& lv = *reinterpret_cast<const TCarrier*>(left); \
    auto& rv = *reinterpret_cast<const TCarrier*>(right); \
    auto mask = VectorFixedCompareLessThan<scalar, scalar, TCarrier>(lv, rv); \
    return VectorFixedAllLanesNonZero<scalar, TCarrier>(mask) ? 1 : 0; \
} \
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_greater_than_or_equal_any_##suffix(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right) { \
    auto& lv = *reinterpret_cast<const TCarrier*>(left); \
    auto& rv = *reinterpret_cast<const TCarrier*>(right); \
    auto mask = VectorFixedCompareGreaterThanOrEqual<scalar, scalar, TCarrier>(lv, rv); \
    return VectorFixedAnyLaneNonZero<TCarrier>(mask) ? 1 : 0; \
} \
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_greater_than_or_equal_all_##suffix(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right) { \
    auto& lv = *reinterpret_cast<const TCarrier*>(left); \
    auto& rv = *reinterpret_cast<const TCarrier*>(right); \
    auto mask = VectorFixedCompareGreaterThanOrEqual<scalar, scalar, TCarrier>(lv, rv); \
    return VectorFixedAllLanesNonZero<scalar, TCarrier>(mask) ? 1 : 0; \
} \
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_less_than_or_equal_any_##suffix(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right) { \
    auto& lv = *reinterpret_cast<const TCarrier*>(left); \
    auto& rv = *reinterpret_cast<const TCarrier*>(right); \
    auto mask = VectorFixedCompareLessThanOrEqual<scalar, scalar, TCarrier>(lv, rv); \
    return VectorFixedAnyLaneNonZero<TCarrier>(mask) ? 1 : 0; \
} \
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_less_than_or_equal_all_##suffix(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right) { \
    auto& lv = *reinterpret_cast<const TCarrier*>(left); \
    auto& rv = *reinterpret_cast<const TCarrier*>(right); \
    auto mask = VectorFixedCompareLessThanOrEqual<scalar, scalar, TCarrier>(lv, rv); \
    return VectorFixedAllLanesNonZero<scalar, TCarrier>(mask) ? 1 : 0; \
}

// Generate stubs for all Vector<T> element types
VECTOR_REDUCTION_STUBS(CHAOS_IL2CPP_INT32, i32)
VECTOR_REDUCTION_STUBS(CHAOS_IL2CPP_UINT32, u32)
VECTOR_REDUCTION_STUBS(CHAOS_IL2CPP_INT64, i64)
VECTOR_REDUCTION_STUBS(CHAOS_IL2CPP_UINT64, u64)
VECTOR_REDUCTION_STUBS(float, f)
VECTOR_REDUCTION_STUBS(double, d)
VECTOR_REDUCTION_STUBS(CHAOS_IL2CPP_INT16, i16)
VECTOR_REDUCTION_STUBS(CHAOS_IL2CPP_UINT16, u16)
VECTOR_REDUCTION_STUBS(CHAOS_IL2CPP_UINT8, u8)
VECTOR_REDUCTION_STUBS(CHAOS_IL2CPP_INT8, i8)
