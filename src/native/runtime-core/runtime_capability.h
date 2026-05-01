#ifndef CHAOS_IL2CPP_RUNTIME_CAPABILITY_H_
#define CHAOS_IL2CPP_RUNTIME_CAPABILITY_H_

#include <chaos/native_types.h>

namespace chaos::il2cpp::runtime_capability {

enum class ValueTypeKernelBackendKind {
    Generic = 0,
    Platform = 1,
    Intrinsic = 2,
};

enum class VectorKernelBackendKind {
    Generic = 0,
    Intrinsic = 1,
};

enum TypeCapabilityVectorLaneKindV1 : CHAOS_IL2CPP_UINT32 {
    TypeCapabilityVectorLaneKindNone = CHAOS_TYPE_CAPABILITY_VECTOR_LANE_NONE,
    TypeCapabilityVectorLaneKindInteger = CHAOS_TYPE_CAPABILITY_VECTOR_LANE_INTEGER,
    TypeCapabilityVectorLaneKindFloating = CHAOS_TYPE_CAPABILITY_VECTOR_LANE_FLOATING,
};

enum TypeCapabilityScalarKindV1 : CHAOS_IL2CPP_UINT32 {
    TypeCapabilityScalarKindNone = CHAOS_TYPE_CAPABILITY_SCALAR_KIND_NONE,
    TypeCapabilityScalarKindSignedInteger = CHAOS_TYPE_CAPABILITY_SCALAR_KIND_SIGNED_INTEGER,
    TypeCapabilityScalarKindUnsignedInteger = CHAOS_TYPE_CAPABILITY_SCALAR_KIND_UNSIGNED_INTEGER,
    TypeCapabilityScalarKindNativeSignedInteger = CHAOS_TYPE_CAPABILITY_SCALAR_KIND_NATIVE_SIGNED_INTEGER,
    TypeCapabilityScalarKindNativeUnsignedInteger = CHAOS_TYPE_CAPABILITY_SCALAR_KIND_NATIVE_UNSIGNED_INTEGER,
    TypeCapabilityScalarKindFloating = CHAOS_TYPE_CAPABILITY_SCALAR_KIND_FLOATING,
};

enum class VectorShiftBackendKind {
    Generic = 0,
    Intrinsic = 1,
};

}  // namespace chaos::il2cpp::runtime_capability

#endif  // CHAOS_IL2CPP_RUNTIME_CAPABILITY_H_
