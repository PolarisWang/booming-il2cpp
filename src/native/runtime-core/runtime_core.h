#ifndef CHAOS_IL2CPP_RUNTIME_CORE_H_
#define CHAOS_IL2CPP_RUNTIME_CORE_H_

#include "runtime_abi.h"
#include <chaos/native_types.h>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>

namespace chaos::il2cpp::runtime_core {

/// Thread-local RuntimeState for code paths that do not carry
/// an explicit RuntimeState* parameter (e.g. NativeAot external helpers).
void SetCurrentRuntimeState(RuntimeState* runtime_state);
RuntimeState* GetCurrentRuntimeState();

struct ManagedExceptionCarrier {
    ExceptionHandle exception;
};

using EngineLifecycleCallback = void (*)(const char* phase_utf8, void* user_data);
using FinalizerCallback = void (*)(void* object_instance);

enum class RuntimeMode {
    Aot = 0,
    Mixed = 1,
};

enum class MarshalStructureMarshallingClass : CHAOS_IL2CPP_UINT32 {
    FullyBlittableFastPath = 0,
    LayoutFixedMarshallerPath = 1,
    DescriptorDrivenMarshallerPath = 2,
};

enum class MarshalAllocationKind : CHAOS_IL2CPP_UINT32 {
    HGlobal = 0,
    CoTaskMem = 1,
    BStr = 2,
};

enum TaskRuntimeKernelCapabilityBits : CHAOS_IL2CPP_UINT64 {
    TaskRuntimeKernelCapabilityNone = 0ull,
    TaskRuntimeKernelCapabilityStateCore = 1ull << 0,
    TaskRuntimeKernelCapabilityContinuation = 1ull << 1,
    TaskRuntimeKernelCapabilityWait = 1ull << 2,
    TaskRuntimeKernelCapabilityFactory = 1ull << 3,
    TaskRuntimeKernelCapabilityIntrospection = 1ull << 4,
};

enum MarshalPlatformCapabilityBits : CHAOS_IL2CPP_UINT64 {
    MarshalPlatformCapabilityNone = 0ull,
    MarshalPlatformCapabilityMemoryBlock = 1ull << 0,
    MarshalPlatformCapabilityStringMarshaling = 1ull << 1,
    MarshalPlatformCapabilityStructureLayout = 1ull << 2,
    MarshalPlatformCapabilityDelegateBridge = 1ull << 3,
    MarshalPlatformCapabilityComCore = 1ull << 4,
    MarshalPlatformCapabilityComActivation = 1ull << 5,
    MarshalPlatformCapabilityVariantScalar = 1ull << 6,
    MarshalPlatformCapabilityVariantBulk = 1ull << 7,
    MarshalPlatformCapabilityVariantExtended = 1ull << 8,
};

struct MarshalMemoryBlockAbiV1 {
    CHAOS_IL2CPP_SIZE struct_size;
};

struct MarshalStringMarshalingAbiV1 {
    CHAOS_IL2CPP_SIZE struct_size;
};

struct MarshalStructureLayoutAbiV1 {
    CHAOS_IL2CPP_SIZE struct_size;
};

struct MarshalDelegateBridgeAbiV1 {
    CHAOS_IL2CPP_SIZE struct_size;
};

struct ComInteropAbiV1 {
    CHAOS_IL2CPP_SIZE struct_size;
    CHAOS_IL2CPP_UINT64 capability_bits;
};

struct VariantInteropAbiV1 {
    CHAOS_IL2CPP_SIZE struct_size;
    CHAOS_IL2CPP_UINT64 capability_bits;
};

struct MarshalPlatformAbiRootV1 {
    CHAOS_IL2CPP_UINT32 abi_version;
    CHAOS_IL2CPP_SIZE struct_size;
    CHAOS_IL2CPP_UINT64 capability_bits;
    const MarshalMemoryBlockAbiV1* memory_block;
    const MarshalStringMarshalingAbiV1* string_marshaling;
    const MarshalStructureLayoutAbiV1* structure_layout;
    const MarshalDelegateBridgeAbiV1* delegate_bridge;
    const ComInteropAbiV1* com_interop;
    const VariantInteropAbiV1* variant_interop;
};

struct TaskRuntimeKernelV1 {
    CHAOS_IL2CPP_UINT32 abi_version;
    CHAOS_IL2CPP_SIZE struct_size;
    CHAOS_IL2CPP_UINT64 capability_bits;
};

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

struct RuntimeNumericsVector2Carrier {
    float x;
    float y;
};

struct RuntimeNumericsVector3Carrier {
    float x;
    float y;
    float z;
};

struct RuntimeNumericsVector4Carrier {
    float x;
    float y;
    float z;
    float w;
};

struct RuntimeNumericsMatrix3x2Carrier {
    float m11;
    float m12;
    float m21;
    float m22;
    float m31;
    float m32;
};

struct RuntimeNumericsMatrix4x4Carrier {
    float m11;
    float m12;
    float m13;
    float m14;
    float m21;
    float m22;
    float m23;
    float m24;
    float m31;
    float m32;
    float m33;
    float m34;
    float m41;
    float m42;
    float m43;
    float m44;
};

struct RuntimeNumericsQuaternionCarrier {
    float x;
    float y;
    float z;
    float w;
};

struct RuntimeIntrinsicVector64Carrier {
    CHAOS_IL2CPP_UINT8 bytes[8];
};

struct RuntimeIntrinsicVector128Carrier {
    CHAOS_IL2CPP_UINT8 bytes[16];
};

struct RuntimeIntrinsicVector256Carrier {
    CHAOS_IL2CPP_UINT8 bytes[32];
};

struct RuntimeIntrinsicVector512Carrier {
    CHAOS_IL2CPP_UINT8 bytes[64];
};

template <typename TScalar, typename TCarrier, typename TBinaryOp>
inline TCarrier VectorFixedApplyBinary(TCarrier left_value, TCarrier right_value, TBinaryOp operation) {
    static_assert(sizeof(TCarrier) % sizeof(TScalar) == 0u, "carrier size must be divisible by scalar size");
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);

    TScalar left_lanes[lane_count];
    TScalar right_lanes[lane_count];
    TScalar result_lanes[lane_count];
    CHAOS_IL2CPP_MEMCPY(left_lanes, &left_value, sizeof(TCarrier));
    CHAOS_IL2CPP_MEMCPY(right_lanes, &right_value, sizeof(TCarrier));
    for (CHAOS_IL2CPP_SIZE lane_index = 0; lane_index < lane_count; ++lane_index) {
        result_lanes[lane_index] = operation(left_lanes[lane_index], right_lanes[lane_index]);
    }

    TCarrier result = {};
    CHAOS_IL2CPP_MEMCPY(&result, result_lanes, sizeof(TCarrier));
    return result;
}

template <typename TScalar, typename TCarrier, typename TUnaryOp>
inline TCarrier VectorFixedApplyUnary(TCarrier value, TUnaryOp operation) {
    static_assert(sizeof(TCarrier) % sizeof(TScalar) == 0u, "carrier size must be divisible by scalar size");
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);

    TScalar lanes[lane_count];
    TScalar result_lanes[lane_count];
    CHAOS_IL2CPP_MEMCPY(lanes, &value, sizeof(TCarrier));
    for (CHAOS_IL2CPP_SIZE lane_index = 0; lane_index < lane_count; ++lane_index) {
        result_lanes[lane_index] = operation(lanes[lane_index]);
    }

    TCarrier result = {};
    CHAOS_IL2CPP_MEMCPY(&result, result_lanes, sizeof(TCarrier));
    return result;
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedBroadcast(TScalar scalar_value) {
    static_assert(sizeof(TCarrier) % sizeof(TScalar) == 0u, "carrier size must be divisible by scalar size");
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);

    TScalar lanes[lane_count];
    for (CHAOS_IL2CPP_SIZE lane_index = 0; lane_index < lane_count; ++lane_index) {
        lanes[lane_index] = scalar_value;
    }

    TCarrier result = {};
    CHAOS_IL2CPP_MEMCPY(&result, lanes, sizeof(TCarrier));
    return result;
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedCreateScalar(TScalar scalar_value) {
    static_assert(sizeof(TCarrier) % sizeof(TScalar) == 0u, "carrier size must be divisible by scalar size");
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);

    TScalar lanes[lane_count] = {};
    lanes[0] = scalar_value;

    TCarrier result = {};
    CHAOS_IL2CPP_MEMCPY(&result, lanes, sizeof(TCarrier));
    return result;
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedCreateFromLanes(const TScalar* lane_values, CHAOS_IL2CPP_SIZE lane_count) {
    static_assert(sizeof(TCarrier) % sizeof(TScalar) == 0u, "carrier size must be divisible by scalar size");
    constexpr CHAOS_IL2CPP_SIZE expected_lane_count = sizeof(TCarrier) / sizeof(TScalar);

    TCarrier result = {};
    if (lane_values == nullptr || lane_count != expected_lane_count) {
        return result;
    }

    CHAOS_IL2CPP_MEMCPY(&result, lane_values, sizeof(TCarrier));
    return result;
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedCreateFromArray(const TScalar* values, CHAOS_IL2CPP_SIZE start_index) {
    static_assert(sizeof(TCarrier) % sizeof(TScalar) == 0u, "carrier size must be divisible by scalar size");
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);

    return values == nullptr
        ? TCarrier{}
        : VectorFixedCreateFromLanes<TScalar, TCarrier>(values + start_index, lane_count);
}

template <typename TScalar, typename TCarrier, typename THalfCarrier>
inline TCarrier VectorFixedCreateFromHalves(THalfCarrier lower_half, THalfCarrier upper_half) {
    static_assert(sizeof(TCarrier) == sizeof(THalfCarrier) * 2u, "carrier must be exactly two half carriers");

    TCarrier result = {};
    CHAOS_IL2CPP_MEMCPY(&result, &lower_half, sizeof(THalfCarrier));
    CHAOS_IL2CPP_MEMCPY(reinterpret_cast<unsigned char*>(&result) + sizeof(THalfCarrier), &upper_half, sizeof(THalfCarrier));
    return result;
}

template <typename TScalar, typename TCarrier>
inline CHAOS_IL2CPP_INT32 VectorFixedExtractShiftCount(TCarrier value) {
    static_assert(sizeof(TCarrier) >= sizeof(TScalar), "carrier size must be at least one scalar lane");

    TScalar lane_value = {};
    CHAOS_IL2CPP_MEMCPY(&lane_value, &value, sizeof(TScalar));
    return static_cast<CHAOS_IL2CPP_INT32>(lane_value);
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedShiftLeft(TCarrier value, CHAOS_IL2CPP_INT32 shift_amount) {
    return VectorFixedApplyBinary<TScalar, TCarrier>(
        value,
        VectorFixedBroadcast<TScalar, TCarrier>(static_cast<TScalar>(shift_amount)),
        [shift_amount](TScalar lane, TScalar) {
            using TUnsigned = CHAOS_IL2CPP_MAKE_UNSIGNED<TScalar>;
            constexpr auto bit_width = static_cast<CHAOS_IL2CPP_INT32>(sizeof(TScalar) * 8u);
            if (shift_amount <= 0) {
                return lane;
            }

            if (shift_amount >= bit_width) {
                return static_cast<TScalar>(0);
            }

            return static_cast<TScalar>(static_cast<TUnsigned>(lane) << shift_amount);
        });
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedShiftRightLogical(TCarrier value, CHAOS_IL2CPP_INT32 shift_amount) {
    return VectorFixedApplyBinary<TScalar, TCarrier>(
        value,
        VectorFixedBroadcast<TScalar, TCarrier>(static_cast<TScalar>(shift_amount)),
        [shift_amount](TScalar lane, TScalar) {
            using TUnsigned = CHAOS_IL2CPP_MAKE_UNSIGNED<TScalar>;
            constexpr auto bit_width = static_cast<CHAOS_IL2CPP_INT32>(sizeof(TScalar) * 8u);
            if (shift_amount <= 0) {
                return lane;
            }

            if (shift_amount >= bit_width) {
                return static_cast<TScalar>(0);
            }

            return static_cast<TScalar>(static_cast<TUnsigned>(lane) >> shift_amount);
        });
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedShiftRightArithmetic(TCarrier value, CHAOS_IL2CPP_INT32 shift_amount) {
    static_assert(CHAOS_IL2CPP_IS_SIGNED<TScalar>, "arithmetic right shift requires signed scalar");
    return VectorFixedApplyBinary<TScalar, TCarrier>(
        value,
        VectorFixedBroadcast<TScalar, TCarrier>(static_cast<TScalar>(shift_amount)),
        [shift_amount](TScalar lane, TScalar) {
            constexpr auto bit_width = static_cast<CHAOS_IL2CPP_INT32>(sizeof(TScalar) * 8u);
            if (shift_amount <= 0) {
                return lane;
            }

            if (shift_amount >= bit_width) {
                return lane < 0 ? static_cast<TScalar>(-1) : static_cast<TScalar>(0);
            }

            return static_cast<TScalar>(lane >> shift_amount);
        });
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedShiftLeftVariable(TCarrier value, TCarrier shift_amounts) {
    return VectorFixedApplyBinary<TScalar, TCarrier>(
        value,
        shift_amounts,
        [](TScalar lane, TScalar shift_lane) {
            using TUnsigned = CHAOS_IL2CPP_MAKE_UNSIGNED<TScalar>;
            constexpr auto bit_width = static_cast<CHAOS_IL2CPP_INT32>(sizeof(TScalar) * 8u);
            const auto shift_amount = static_cast<CHAOS_IL2CPP_INT32>(shift_lane);
            if (shift_amount <= 0) {
                return lane;
            }

            if (shift_amount >= bit_width) {
                return static_cast<TScalar>(0);
            }

            return static_cast<TScalar>(static_cast<TUnsigned>(lane) << shift_amount);
        });
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedShiftRightLogicalVariable(TCarrier value, TCarrier shift_amounts) {
    return VectorFixedApplyBinary<TScalar, TCarrier>(
        value,
        shift_amounts,
        [](TScalar lane, TScalar shift_lane) {
            using TUnsigned = CHAOS_IL2CPP_MAKE_UNSIGNED<TScalar>;
            constexpr auto bit_width = static_cast<CHAOS_IL2CPP_INT32>(sizeof(TScalar) * 8u);
            const auto shift_amount = static_cast<CHAOS_IL2CPP_INT32>(shift_lane);
            if (shift_amount <= 0) {
                return lane;
            }

            if (shift_amount >= bit_width) {
                return static_cast<TScalar>(0);
            }

            return static_cast<TScalar>(static_cast<TUnsigned>(lane) >> shift_amount);
        });
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedShiftRightArithmeticVariable(TCarrier value, TCarrier shift_amounts) {
    static_assert(CHAOS_IL2CPP_IS_SIGNED<TScalar>, "arithmetic right shift requires signed scalar");
    return VectorFixedApplyBinary<TScalar, TCarrier>(
        value,
        shift_amounts,
        [](TScalar lane, TScalar shift_lane) {
            constexpr auto bit_width = static_cast<CHAOS_IL2CPP_INT32>(sizeof(TScalar) * 8u);
            const auto shift_amount = static_cast<CHAOS_IL2CPP_INT32>(shift_lane);
            if (shift_amount <= 0) {
                return lane;
            }

            if (shift_amount >= bit_width) {
                return lane < 0 ? static_cast<TScalar>(-1) : static_cast<TScalar>(0);
            }

            return static_cast<TScalar>(lane >> shift_amount);
        });
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedRotateLeft(TCarrier value, CHAOS_IL2CPP_UINT8 rotate_amount) {
    using TUnsigned = CHAOS_IL2CPP_MAKE_UNSIGNED<TScalar>;
    constexpr auto bit_width = static_cast<CHAOS_IL2CPP_UINT32>(sizeof(TScalar) * 8u);
    const auto normalized_rotate = static_cast<CHAOS_IL2CPP_UINT32>(rotate_amount) % bit_width;
    if (normalized_rotate == 0u) {
        return value;
    }

    return VectorFixedApplyUnary<TScalar, TCarrier>(
        value,
        [normalized_rotate](TScalar lane) {
            const auto unsigned_lane = static_cast<TUnsigned>(lane);
            return static_cast<TScalar>(
                static_cast<TUnsigned>(unsigned_lane << normalized_rotate) |
                static_cast<TUnsigned>(unsigned_lane >> (bit_width - normalized_rotate)));
        });
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedRotateRight(TCarrier value, CHAOS_IL2CPP_UINT8 rotate_amount) {
    using TUnsigned = CHAOS_IL2CPP_MAKE_UNSIGNED<TScalar>;
    constexpr auto bit_width = static_cast<CHAOS_IL2CPP_UINT32>(sizeof(TScalar) * 8u);
    const auto normalized_rotate = static_cast<CHAOS_IL2CPP_UINT32>(rotate_amount) % bit_width;
    if (normalized_rotate == 0u) {
        return value;
    }

    return VectorFixedApplyUnary<TScalar, TCarrier>(
        value,
        [normalized_rotate](TScalar lane) {
            const auto unsigned_lane = static_cast<TUnsigned>(lane);
            return static_cast<TScalar>(
                static_cast<TUnsigned>(unsigned_lane >> normalized_rotate) |
                static_cast<TUnsigned>(unsigned_lane << (bit_width - normalized_rotate)));
        });
}

template <typename TScalar, typename TRotateScalar, typename TCarrier>
inline TCarrier VectorFixedRotateLeftVariable(TCarrier value, TCarrier rotate_amounts) {
    using TUnsigned = CHAOS_IL2CPP_MAKE_UNSIGNED<TScalar>;
    constexpr auto bit_width = static_cast<CHAOS_IL2CPP_UINT32>(sizeof(TScalar) * 8u);
    return VectorFixedApplyBinary<TScalar, TCarrier>(
        value,
        rotate_amounts,
        [](TScalar lane, TScalar rotate_lane_bits) {
            const auto rotate_lane = static_cast<TRotateScalar>(rotate_lane_bits);
            const auto normalized_rotate = static_cast<CHAOS_IL2CPP_UINT32>(rotate_lane % bit_width);
            if (normalized_rotate == 0u) {
                return lane;
            }

            const auto unsigned_lane = static_cast<TUnsigned>(lane);
            return static_cast<TScalar>(
                static_cast<TUnsigned>(unsigned_lane << normalized_rotate) |
                static_cast<TUnsigned>(unsigned_lane >> (bit_width - normalized_rotate)));
        });
}

template <typename TScalar, typename TRotateScalar, typename TCarrier>
inline TCarrier VectorFixedRotateRightVariable(TCarrier value, TCarrier rotate_amounts) {
    using TUnsigned = CHAOS_IL2CPP_MAKE_UNSIGNED<TScalar>;
    constexpr auto bit_width = static_cast<CHAOS_IL2CPP_UINT32>(sizeof(TScalar) * 8u);
    return VectorFixedApplyBinary<TScalar, TCarrier>(
        value,
        rotate_amounts,
        [](TScalar lane, TScalar rotate_lane_bits) {
            const auto rotate_lane = static_cast<TRotateScalar>(rotate_lane_bits);
            const auto normalized_rotate = static_cast<CHAOS_IL2CPP_UINT32>(rotate_lane % bit_width);
            if (normalized_rotate == 0u) {
                return lane;
            }

            const auto unsigned_lane = static_cast<TUnsigned>(lane);
            return static_cast<TScalar>(
                static_cast<TUnsigned>(unsigned_lane >> normalized_rotate) |
                static_cast<TUnsigned>(unsigned_lane << (bit_width - normalized_rotate)));
        });
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedAlignRight32(TCarrier left_value, TCarrier right_value, CHAOS_IL2CPP_UINT8 control) {
    static_assert(sizeof(TScalar) == sizeof(CHAOS_IL2CPP_UINT32), "AlignRight32 requires 32-bit scalar lanes");
    static_assert(sizeof(TCarrier) % sizeof(TScalar) == 0u, "carrier size must be divisible by scalar size");
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);

    const auto shift = static_cast<CHAOS_IL2CPP_SIZE>(control & static_cast<CHAOS_IL2CPP_UINT8>(lane_count - 1u));
    TScalar left_lanes[lane_count];
    TScalar right_lanes[lane_count];
    TScalar combined_lanes[lane_count * 2u];
    TScalar result_lanes[lane_count];
    CHAOS_IL2CPP_MEMCPY(left_lanes, &left_value, sizeof(TCarrier));
    CHAOS_IL2CPP_MEMCPY(right_lanes, &right_value, sizeof(TCarrier));
    for (CHAOS_IL2CPP_SIZE lane_index = 0; lane_index < lane_count; ++lane_index) {
        combined_lanes[lane_index] = right_lanes[lane_index];
        combined_lanes[lane_count + lane_index] = left_lanes[lane_index];
        result_lanes[lane_index] = combined_lanes[lane_index + shift];
    }

    TCarrier result = {};
    CHAOS_IL2CPP_MEMCPY(&result, result_lanes, sizeof(TCarrier));
    return result;
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedAlignRight64(TCarrier left_value, TCarrier right_value, CHAOS_IL2CPP_UINT8 control) {
    static_assert(sizeof(TScalar) == sizeof(CHAOS_IL2CPP_UINT64), "AlignRight64 requires 64-bit scalar lanes");
    static_assert(sizeof(TCarrier) % sizeof(TScalar) == 0u, "carrier size must be divisible by scalar size");
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);

    const auto shift = static_cast<CHAOS_IL2CPP_SIZE>(control & static_cast<CHAOS_IL2CPP_UINT8>(lane_count - 1u));
    TScalar left_lanes[lane_count];
    TScalar right_lanes[lane_count];
    TScalar combined_lanes[lane_count * 2u];
    TScalar result_lanes[lane_count];
    CHAOS_IL2CPP_MEMCPY(left_lanes, &left_value, sizeof(TCarrier));
    CHAOS_IL2CPP_MEMCPY(right_lanes, &right_value, sizeof(TCarrier));
    for (CHAOS_IL2CPP_SIZE lane_index = 0; lane_index < lane_count; ++lane_index) {
        combined_lanes[lane_index] = right_lanes[lane_index];
        combined_lanes[lane_count + lane_index] = left_lanes[lane_index];
        result_lanes[lane_index] = combined_lanes[lane_index + shift];
    }

    TCarrier result = {};
    CHAOS_IL2CPP_MEMCPY(&result, result_lanes, sizeof(TCarrier));
    return result;
}

template <typename TCarrier>
inline TCarrier VectorFixedShuffle2x128(TCarrier left_value, TCarrier right_value, CHAOS_IL2CPP_UINT8 control) {
    static_assert(sizeof(TCarrier) == sizeof(RuntimeIntrinsicVector256Carrier), "Shuffle2x128 requires 256-bit carrier");

    RuntimeIntrinsicVector128Carrier left_halves[2];
    RuntimeIntrinsicVector128Carrier right_halves[2];
    CHAOS_IL2CPP_MEMCPY(left_halves, &left_value, sizeof(TCarrier));
    CHAOS_IL2CPP_MEMCPY(right_halves, &right_value, sizeof(TCarrier));

    auto select_half = [&](CHAOS_IL2CPP_UINT8 selector) -> RuntimeIntrinsicVector128Carrier {
        switch (selector & 0x3u) {
            case 0u:
                return left_halves[0];
            case 1u:
                return left_halves[1];
            case 2u:
                return right_halves[0];
            default:
                return right_halves[1];
        }
    };

    RuntimeIntrinsicVector128Carrier result_halves[2] = {};
    if ((control & 0x08u) == 0u) {
        result_halves[0] = select_half(control);
    }

    if ((control & 0x80u) == 0u) {
        result_halves[1] = select_half(static_cast<CHAOS_IL2CPP_UINT8>(control >> 4u));
    }

    TCarrier result = {};
    CHAOS_IL2CPP_MEMCPY(&result, result_halves, sizeof(TCarrier));
    return result;
}

template <typename TValueScalar, typename TIndexScalar, typename TCarrier>
inline TCarrier VectorFixedPermuteVar(TCarrier value, TCarrier indices) {
    static_assert(sizeof(TCarrier) % sizeof(TValueScalar) == 0u, "carrier size must be divisible by value scalar size");
    static_assert(sizeof(TCarrier) % sizeof(TIndexScalar) == 0u, "carrier size must be divisible by index scalar size");
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TValueScalar);

    TValueScalar value_lanes[lane_count];
    TIndexScalar index_lanes[lane_count];
    TValueScalar result_lanes[lane_count];
    CHAOS_IL2CPP_MEMCPY(value_lanes, &value, sizeof(TCarrier));
    CHAOS_IL2CPP_MEMCPY(index_lanes, &indices, sizeof(TCarrier));
    for (CHAOS_IL2CPP_SIZE lane_index = 0; lane_index < lane_count; ++lane_index) {
        const auto selected_index = static_cast<CHAOS_IL2CPP_SIZE>(
            static_cast<CHAOS_IL2CPP_UINT64>(index_lanes[lane_index]) % lane_count);
        result_lanes[lane_index] = value_lanes[selected_index];
    }

    TCarrier result = {};
    CHAOS_IL2CPP_MEMCPY(&result, result_lanes, sizeof(TCarrier));
    return result;
}

template <typename TValueScalar, typename TIndexScalar, typename TCarrier>
inline TCarrier VectorFixedPermuteVarX2(TCarrier left_value, TCarrier indices, TCarrier right_value) {
    static_assert(sizeof(TCarrier) % sizeof(TValueScalar) == 0u, "carrier size must be divisible by value scalar size");
    static_assert(sizeof(TCarrier) % sizeof(TIndexScalar) == 0u, "carrier size must be divisible by index scalar size");
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TValueScalar);

    TValueScalar left_lanes[lane_count];
    TValueScalar right_lanes[lane_count];
    TIndexScalar index_lanes[lane_count];
    TValueScalar result_lanes[lane_count];
    CHAOS_IL2CPP_MEMCPY(left_lanes, &left_value, sizeof(TCarrier));
    CHAOS_IL2CPP_MEMCPY(right_lanes, &right_value, sizeof(TCarrier));
    CHAOS_IL2CPP_MEMCPY(index_lanes, &indices, sizeof(TCarrier));
    for (CHAOS_IL2CPP_SIZE lane_index = 0; lane_index < lane_count; ++lane_index) {
        const auto selected_index = static_cast<CHAOS_IL2CPP_SIZE>(
            static_cast<CHAOS_IL2CPP_UINT64>(index_lanes[lane_index]) % (lane_count * 2u));
        result_lanes[lane_index] = selected_index < lane_count
            ? left_lanes[selected_index]
            : right_lanes[selected_index - lane_count];
    }

    TCarrier result = {};
    CHAOS_IL2CPP_MEMCPY(&result, result_lanes, sizeof(TCarrier));
    return result;
}

template <typename TOutputScalar, typename TInputScalar>
inline TOutputScalar VectorFixedConvertScalarUnchecked(TInputScalar value) {
    return static_cast<TOutputScalar>(value);
}

template <typename TOutputScalar, typename TInputScalar>
inline TOutputScalar VectorFixedConvertScalarTruncating(TInputScalar value) {
    if constexpr (CHAOS_IL2CPP_IS_FLOATING_POINT<TInputScalar>) {
        if (!CHAOS_IL2CPP_ISFINITE(value)) {
            return static_cast<TOutputScalar>(0);
        }

        return static_cast<TOutputScalar>(CHAOS_IL2CPP_TRUNC(value));
    }

    return static_cast<TOutputScalar>(value);
}

template <typename TOutputScalar, typename TInputScalar>
inline TOutputScalar VectorFixedConvertScalarSaturating(TInputScalar value) {
    if constexpr (CHAOS_IL2CPP_IS_FLOATING_POINT<TInputScalar>) {
        if (!CHAOS_IL2CPP_ISFINITE(value)) {
            return static_cast<TOutputScalar>(0);
        }
    }

    using TOutputLimits = CHAOS_IL2CPP_NUMERIC_LIMITS_TOutputScalar>;
    using TInputLimits = CHAOS_IL2CPP_NUMERIC_LIMITS_TInputScalar>;

    if constexpr (CHAOS_IL2CPP_IS_SAME<TOutputScalar, TInputScalar>) {
        return value;
    } else if constexpr (CHAOS_IL2CPP_IS_INTEGRAL<TInputScalar> && CHAOS_IL2CPP_IS_INTEGRAL<TOutputScalar>) {
        if constexpr (CHAOS_IL2CPP_IS_SIGNED<TInputScalar> == CHAOS_IL2CPP_IS_SIGNED<TOutputScalar>) {
            if (value < static_cast<TInputScalar>(TOutputLimits::lowest())) {
                return TOutputLimits::lowest();
            }

            if (value > static_cast<TInputScalar>(TOutputLimits::max())) {
                return TOutputLimits::max();
            }

            return static_cast<TOutputScalar>(value);
        } else if constexpr (CHAOS_IL2CPP_IS_SIGNED<TInputScalar>) {
            if (value <= static_cast<TInputScalar>(0)) {
                return static_cast<TOutputScalar>(0);
            }

            using TUnsignedInput = CHAOS_IL2CPP_MAKE_UNSIGNED<TInputScalar>;
            const auto unsigned_value = static_cast<TUnsignedInput>(value);
            if (unsigned_value > static_cast<TUnsignedInput>(TOutputLimits::max())) {
                return TOutputLimits::max();
            }

            return static_cast<TOutputScalar>(value);
        } else {
            if (value > static_cast<CHAOS_IL2CPP_MAKE_UNSIGNED<TOutputScalar>>(TOutputLimits::max())) {
                return TOutputLimits::max();
            }

            return static_cast<TOutputScalar>(value);
        }
    } else {
        if (value < static_cast<TInputScalar>(TOutputLimits::lowest())) {
            return TOutputLimits::lowest();
        }

        if (value > static_cast<TInputScalar>(TOutputLimits::max())) {
            return TOutputLimits::max();
        }

        return static_cast<TOutputScalar>(value);
    }
}

template <typename TOutputScalar, typename TInputScalar, typename TOutputCarrier, typename TInputCarrier>
inline TOutputCarrier VectorFixedConvertToVector(TInputCarrier value) {
    static_assert(sizeof(TInputCarrier) % sizeof(TInputScalar) == 0u, "input carrier size must be divisible by input scalar size");
    static_assert(sizeof(TOutputCarrier) % sizeof(TOutputScalar) == 0u, "output carrier size must be divisible by output scalar size");

    constexpr CHAOS_IL2CPP_SIZE input_lane_count = sizeof(TInputCarrier) / sizeof(TInputScalar);
    constexpr CHAOS_IL2CPP_SIZE output_lane_count = sizeof(TOutputCarrier) / sizeof(TOutputScalar);

    TInputScalar input_lanes[input_lane_count];
    TOutputScalar output_lanes[output_lane_count] = {};
    CHAOS_IL2CPP_MEMCPY(input_lanes, &value, sizeof(TInputCarrier));
    for (CHAOS_IL2CPP_SIZE lane_index = 0; lane_index < input_lane_count && lane_index < output_lane_count; ++lane_index) {
        output_lanes[lane_index] = VectorFixedConvertScalarUnchecked<TOutputScalar>(input_lanes[lane_index]);
    }

    TOutputCarrier result = {};
    CHAOS_IL2CPP_MEMCPY(&result, output_lanes, sizeof(TOutputCarrier));
    return result;
}

template <typename TOutputScalar, typename TInputScalar, typename TOutputCarrier, typename TInputCarrier>
inline TOutputCarrier VectorFixedConvertToVectorSaturating(TInputCarrier value) {
    static_assert(sizeof(TInputCarrier) % sizeof(TInputScalar) == 0u, "input carrier size must be divisible by input scalar size");
    static_assert(sizeof(TOutputCarrier) % sizeof(TOutputScalar) == 0u, "output carrier size must be divisible by output scalar size");

    constexpr CHAOS_IL2CPP_SIZE input_lane_count = sizeof(TInputCarrier) / sizeof(TInputScalar);
    constexpr CHAOS_IL2CPP_SIZE output_lane_count = sizeof(TOutputCarrier) / sizeof(TOutputScalar);

    TInputScalar input_lanes[input_lane_count];
    TOutputScalar output_lanes[output_lane_count] = {};
    CHAOS_IL2CPP_MEMCPY(input_lanes, &value, sizeof(TInputCarrier));
    for (CHAOS_IL2CPP_SIZE lane_index = 0; lane_index < input_lane_count && lane_index < output_lane_count; ++lane_index) {
        output_lanes[lane_index] = VectorFixedConvertScalarSaturating<TOutputScalar>(input_lanes[lane_index]);
    }

    TOutputCarrier result = {};
    CHAOS_IL2CPP_MEMCPY(&result, output_lanes, sizeof(TOutputCarrier));
    return result;
}

template <typename TOutputScalar, typename TInputScalar, typename TOutputCarrier, typename TInputCarrier>
inline TOutputCarrier VectorFixedConvertToVectorTruncating(TInputCarrier value) {
    static_assert(sizeof(TInputCarrier) % sizeof(TInputScalar) == 0u, "input carrier size must be divisible by input scalar size");
    static_assert(sizeof(TOutputCarrier) % sizeof(TOutputScalar) == 0u, "output carrier size must be divisible by output scalar size");

    constexpr CHAOS_IL2CPP_SIZE input_lane_count = sizeof(TInputCarrier) / sizeof(TInputScalar);
    constexpr CHAOS_IL2CPP_SIZE output_lane_count = sizeof(TOutputCarrier) / sizeof(TOutputScalar);

    TInputScalar input_lanes[input_lane_count];
    TOutputScalar output_lanes[output_lane_count] = {};
    CHAOS_IL2CPP_MEMCPY(input_lanes, &value, sizeof(TInputCarrier));
    for (CHAOS_IL2CPP_SIZE lane_index = 0; lane_index < input_lane_count && lane_index < output_lane_count; ++lane_index) {
        output_lanes[lane_index] = VectorFixedConvertScalarTruncating<TOutputScalar>(input_lanes[lane_index]);
    }

    TOutputCarrier result = {};
    CHAOS_IL2CPP_MEMCPY(&result, output_lanes, sizeof(TOutputCarrier));
    return result;
}

template <typename TOutputScalar, typename TInputScalar, typename TOutputCarrier, typename TInputCarrier>
inline TOutputCarrier VectorFixedConvertToVector128(TInputCarrier value) {
    static_assert(sizeof(TOutputCarrier) == sizeof(RuntimeIntrinsicVector128Carrier), "output carrier must be 128-bit");
    return VectorFixedConvertToVector<TOutputScalar, TInputScalar, TOutputCarrier, TInputCarrier>(value);
}

template <typename TOutputScalar, typename TInputScalar, typename TOutputCarrier, typename TInputCarrier>
inline TOutputCarrier VectorFixedConvertToVector128Saturating(TInputCarrier value) {
    static_assert(sizeof(TOutputCarrier) == sizeof(RuntimeIntrinsicVector128Carrier), "output carrier must be 128-bit");
    return VectorFixedConvertToVectorSaturating<TOutputScalar, TInputScalar, TOutputCarrier, TInputCarrier>(value);
}

template <typename TOutputScalar, typename TInputScalar, typename TOutputCarrier, typename TInputCarrier>
inline TOutputCarrier VectorFixedConvertToVector128Truncating(TInputCarrier value) {
    static_assert(sizeof(TOutputCarrier) == sizeof(RuntimeIntrinsicVector128Carrier), "output carrier must be 128-bit");
    return VectorFixedConvertToVectorTruncating<TOutputScalar, TInputScalar, TOutputCarrier, TInputCarrier>(value);
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedGetExponent(TCarrier value) {
    return VectorFixedApplyUnary<TScalar, TCarrier>(
        value,
        [](TScalar lane) {
            if (!CHAOS_IL2CPP_ISFINITE(lane) || lane == static_cast<TScalar>(0)) {
                return static_cast<TScalar>(0);
            }

            int exponent = 0;
            CHAOS_IL2CPP_FREXP(lane, &exponent);
            return static_cast<TScalar>(exponent);
        });
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedGetMantissa(TCarrier value, CHAOS_IL2CPP_UINT8 control) {
    (void)control;
    return VectorFixedApplyUnary<TScalar, TCarrier>(
        value,
        [](TScalar lane) {
            if (!CHAOS_IL2CPP_ISFINITE(lane) || lane == static_cast<TScalar>(0)) {
                return static_cast<TScalar>(0);
            }

            int exponent = 0;
            return static_cast<TScalar>(CHAOS_IL2CPP_FREXP(lane, &exponent));
        });
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedReciprocal14(TCarrier value) {
    return VectorFixedApplyUnary<TScalar, TCarrier>(
        value,
        [](TScalar lane) {
            return lane == static_cast<TScalar>(0)
                ? static_cast<TScalar>(0)
                : static_cast<TScalar>(static_cast<TScalar>(1) / lane);
        });
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedReciprocalSqrt14(TCarrier value) {
    return VectorFixedApplyUnary<TScalar, TCarrier>(
        value,
        [](TScalar lane) {
            if (!(lane > static_cast<TScalar>(0))) {
                return static_cast<TScalar>(0);
            }

            return static_cast<TScalar>(static_cast<TScalar>(1) / CHAOS_IL2CPP_SQRT(lane));
        });
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedRoundScale(TCarrier value, CHAOS_IL2CPP_UINT8 control) {
    (void)control;
    return VectorFixedApplyUnary<TScalar, TCarrier>(
        value,
        [](TScalar lane) {
            return static_cast<TScalar>(CHAOS_IL2CPP_TRUNC(lane));
        });
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedScale(TCarrier left_value, TCarrier right_value) {
    return VectorFixedApplyBinary<TScalar, TCarrier>(
        left_value,
        right_value,
        [](TScalar left_lane, TScalar right_lane) {
            if (!CHAOS_IL2CPP_ISFINITE(right_lane)) {
                return static_cast<TScalar>(0);
            }

            return static_cast<TScalar>(CHAOS_IL2CPP_LDEXP(left_lane, static_cast<int>(CHAOS_IL2CPP_TRUNC(right_lane))));
        });
}

template <typename TScalar, typename TMaskScalar, typename TCarrier>
inline TCarrier VectorFixedFixup(TCarrier left_value, TCarrier right_value, TCarrier mask_value, CHAOS_IL2CPP_UINT8 control) {
    (void)control;
    static_assert(sizeof(TScalar) == sizeof(TMaskScalar), "fixup mask width must match scalar width");
    return VectorFixedApplyBinary<TScalar, TCarrier>(
        left_value,
        right_value,
        [](TScalar left_lane, TScalar right_lane) {
            return CHAOS_IL2CPP_ISFINITE(left_lane) ? left_lane : right_lane;
        });
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedAdd(TCarrier left_value, TCarrier right_value) {
    return VectorFixedApplyBinary<TScalar, TCarrier>(
        left_value,
        right_value,
        [](TScalar left_lane, TScalar right_lane) {
            return static_cast<TScalar>(left_lane + right_lane);
        });
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedSubtract(TCarrier left_value, TCarrier right_value) {
    return VectorFixedApplyBinary<TScalar, TCarrier>(
        left_value,
        right_value,
        [](TScalar left_lane, TScalar right_lane) {
            return static_cast<TScalar>(left_lane - right_lane);
        });
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedMultiply(TCarrier left_value, TCarrier right_value) {
    return VectorFixedApplyBinary<TScalar, TCarrier>(
        left_value,
        right_value,
        [](TScalar left_lane, TScalar right_lane) {
            return static_cast<TScalar>(left_lane * right_lane);
        });
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedMultiplyScalar(TCarrier value, TScalar scalar_value) {
    return VectorFixedMultiply<TScalar, TCarrier>(
        value,
        VectorFixedBroadcast<TScalar, TCarrier>(scalar_value));
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedDivide(TCarrier left_value, TCarrier right_value) {
    return VectorFixedApplyBinary<TScalar, TCarrier>(
        left_value,
        right_value,
        [](TScalar left_lane, TScalar right_lane) {
            return static_cast<TScalar>(left_lane / right_lane);
        });
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedDivideScalar(TCarrier value, TScalar scalar_value) {
    return VectorFixedDivide<TScalar, TCarrier>(
        value,
        VectorFixedBroadcast<TScalar, TCarrier>(scalar_value));
}

template <typename TScalar, typename TCarrier>
inline bool VectorFixedEquals(TCarrier left_value, TCarrier right_value) {
    static_assert(sizeof(TCarrier) % sizeof(TScalar) == 0u, "carrier size must be divisible by scalar size");
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);

    TScalar left_lanes[lane_count];
    TScalar right_lanes[lane_count];
    CHAOS_IL2CPP_MEMCPY(left_lanes, &left_value, sizeof(TCarrier));
    CHAOS_IL2CPP_MEMCPY(right_lanes, &right_value, sizeof(TCarrier));
    for (CHAOS_IL2CPP_SIZE lane_index = 0; lane_index < lane_count; ++lane_index) {
        if (!(left_lanes[lane_index] == right_lanes[lane_index])) {
            return false;
        }
    }

    return true;
}

template <typename TMaskScalar>
inline TMaskScalar VectorFixedAllBitsSetMaskLane() {
    if constexpr (CHAOS_IL2CPP_IS_FLOATING_POINT<TMaskScalar>) {
        if constexpr (sizeof(TMaskScalar) == sizeof(CHAOS_IL2CPP_UINT32)) {
            constexpr CHAOS_IL2CPP_UINT32 all_bits = 0xFFFFFFFFu;
            TMaskScalar value = static_cast<TMaskScalar>(0);
            CHAOS_IL2CPP_MEMCPY(&value, &all_bits, sizeof(TMaskScalar));
            return value;
        } else {
            constexpr CHAOS_IL2CPP_UINT64 all_bits = 0xFFFFFFFFFFFFFFFFull;
            TMaskScalar value = static_cast<TMaskScalar>(0);
            CHAOS_IL2CPP_MEMCPY(&value, &all_bits, sizeof(TMaskScalar));
            return value;
        }
    } else {
        using TUnsigned = CHAOS_IL2CPP_MAKE_UNSIGNED<TMaskScalar>;
        return static_cast<TMaskScalar>(~static_cast<TUnsigned>(0));
    }
}

template <typename TInputScalar, typename TMaskScalar, typename TCarrier, typename TPredicate>
inline TCarrier VectorFixedCompareCore(TCarrier left_value, TCarrier right_value, TPredicate predicate) {
    static_assert(sizeof(TCarrier) % sizeof(TInputScalar) == 0u, "carrier size must be divisible by scalar size");
    static_assert(sizeof(TInputScalar) == sizeof(TMaskScalar), "compare mask lanes must match input lane width");
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TInputScalar);

    TInputScalar left_lanes[lane_count];
    TInputScalar right_lanes[lane_count];
    TMaskScalar result_lanes[lane_count];
    CHAOS_IL2CPP_MEMCPY(left_lanes, &left_value, sizeof(TCarrier));
    CHAOS_IL2CPP_MEMCPY(right_lanes, &right_value, sizeof(TCarrier));
    for (CHAOS_IL2CPP_SIZE lane_index = 0; lane_index < lane_count; ++lane_index) {
        result_lanes[lane_index] = predicate(left_lanes[lane_index], right_lanes[lane_index])
            ? VectorFixedAllBitsSetMaskLane<TMaskScalar>()
            : static_cast<TMaskScalar>(0);
    }

    TCarrier result = {};
    CHAOS_IL2CPP_MEMCPY(&result, result_lanes, sizeof(TCarrier));
    return result;
}

template <typename TInputScalar, typename TMaskScalar, typename TCarrier>
inline TCarrier VectorFixedCompareEqual(TCarrier left_value, TCarrier right_value) {
    return VectorFixedCompareCore<TInputScalar, TMaskScalar, TCarrier>(
        left_value,
        right_value,
        [](TInputScalar left_lane, TInputScalar right_lane) { return left_lane == right_lane; });
}

template <typename TInputScalar, typename TMaskScalar, typename TCarrier>
inline TCarrier VectorFixedCompareNotEqual(TCarrier left_value, TCarrier right_value) {
    return VectorFixedCompareCore<TInputScalar, TMaskScalar, TCarrier>(
        left_value,
        right_value,
        [](TInputScalar left_lane, TInputScalar right_lane) { return left_lane != right_lane; });
}

template <typename TInputScalar, typename TMaskScalar, typename TCarrier>
inline TCarrier VectorFixedCompareGreaterThan(TCarrier left_value, TCarrier right_value) {
    return VectorFixedCompareCore<TInputScalar, TMaskScalar, TCarrier>(
        left_value,
        right_value,
        [](TInputScalar left_lane, TInputScalar right_lane) { return left_lane > right_lane; });
}

template <typename TInputScalar, typename TMaskScalar, typename TCarrier>
inline TCarrier VectorFixedCompareGreaterThanOrEqual(TCarrier left_value, TCarrier right_value) {
    return VectorFixedCompareCore<TInputScalar, TMaskScalar, TCarrier>(
        left_value,
        right_value,
        [](TInputScalar left_lane, TInputScalar right_lane) { return left_lane >= right_lane; });
}

template <typename TInputScalar, typename TMaskScalar, typename TCarrier>
inline TCarrier VectorFixedCompareLessThan(TCarrier left_value, TCarrier right_value) {
    return VectorFixedCompareCore<TInputScalar, TMaskScalar, TCarrier>(
        left_value,
        right_value,
        [](TInputScalar left_lane, TInputScalar right_lane) { return left_lane < right_lane; });
}

template <typename TInputScalar, typename TMaskScalar, typename TCarrier>
inline TCarrier VectorFixedCompareLessThanOrEqual(TCarrier left_value, TCarrier right_value) {
    return VectorFixedCompareCore<TInputScalar, TMaskScalar, TCarrier>(
        left_value,
        right_value,
        [](TInputScalar left_lane, TInputScalar right_lane) { return left_lane <= right_lane; });
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedMin(TCarrier left_value, TCarrier right_value) {
    return VectorFixedApplyBinary<TScalar, TCarrier>(
        left_value,
        right_value,
        [](TScalar left_lane, TScalar right_lane) {
            return left_lane < right_lane ? left_lane : right_lane;
        });
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedMax(TCarrier left_value, TCarrier right_value) {
    return VectorFixedApplyBinary<TScalar, TCarrier>(
        left_value,
        right_value,
        [](TScalar left_lane, TScalar right_lane) {
            return left_lane > right_lane ? left_lane : right_lane;
        });
}

template <typename TInputScalar, typename TOutputScalar, typename TCarrier>
inline TCarrier VectorFixedAbs(TCarrier value) {
    static_assert(sizeof(TInputScalar) == sizeof(TOutputScalar), "abs input/output lanes must match width");
    static_assert(sizeof(TCarrier) % sizeof(TInputScalar) == 0u, "carrier size must be divisible by scalar size");
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TInputScalar);

    TInputScalar input_lanes[lane_count];
    TOutputScalar output_lanes[lane_count];
    CHAOS_IL2CPP_MEMCPY(input_lanes, &value, sizeof(TCarrier));
    for (CHAOS_IL2CPP_SIZE lane_index = 0; lane_index < lane_count; ++lane_index) {
        const TInputScalar lane = input_lanes[lane_index];
        if constexpr (CHAOS_IL2CPP_IS_FLOATING_POINT<TInputScalar>) {
            output_lanes[lane_index] = static_cast<TOutputScalar>(CHAOS_IL2CPP_FABS(lane));
        } else if constexpr (CHAOS_IL2CPP_IS_UNSIGNED<TInputScalar>) {
            output_lanes[lane_index] = static_cast<TOutputScalar>(lane);
        } else {
            output_lanes[lane_index] = lane < static_cast<TInputScalar>(0)
                ? static_cast<TOutputScalar>(-lane)
                : static_cast<TOutputScalar>(lane);
        }
    }

    TCarrier result = {};
    CHAOS_IL2CPP_MEMCPY(&result, output_lanes, sizeof(TCarrier));
    return result;
}

template <typename TInputScalar, typename TOutputScalar, typename TCarrier>
inline TCarrier VectorFixedNegate(TCarrier value) {
    static_assert(sizeof(TInputScalar) == sizeof(TOutputScalar), "negate input/output lanes must match width");
    static_assert(sizeof(TCarrier) % sizeof(TInputScalar) == 0u, "carrier size must be divisible by scalar size");
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TInputScalar);

    TInputScalar input_lanes[lane_count];
    TOutputScalar output_lanes[lane_count];
    CHAOS_IL2CPP_MEMCPY(input_lanes, &value, sizeof(TCarrier));
    for (CHAOS_IL2CPP_SIZE lane_index = 0; lane_index < lane_count; ++lane_index) {
        output_lanes[lane_index] = static_cast<TOutputScalar>(-input_lanes[lane_index]);
    }

    TCarrier result = {};
    CHAOS_IL2CPP_MEMCPY(&result, output_lanes, sizeof(TCarrier));
    return result;
}

template <typename TCarrier>
inline TCarrier VectorFixedBitwiseAnd(TCarrier left_value, TCarrier right_value) {
    TCarrier result = {};
    for (CHAOS_IL2CPP_SIZE byte_index = 0; byte_index < sizeof(TCarrier); ++byte_index) {
        result.bytes[byte_index] = static_cast<CHAOS_IL2CPP_UINT8>(left_value.bytes[byte_index] & right_value.bytes[byte_index]);
    }

    return result;
}

template <typename TCarrier>
inline TCarrier VectorFixedBitwiseOr(TCarrier left_value, TCarrier right_value) {
    TCarrier result = {};
    for (CHAOS_IL2CPP_SIZE byte_index = 0; byte_index < sizeof(TCarrier); ++byte_index) {
        result.bytes[byte_index] = static_cast<CHAOS_IL2CPP_UINT8>(left_value.bytes[byte_index] | right_value.bytes[byte_index]);
    }

    return result;
}

template <typename TCarrier>
inline TCarrier VectorFixedBitwiseXor(TCarrier left_value, TCarrier right_value) {
    TCarrier result = {};
    for (CHAOS_IL2CPP_SIZE byte_index = 0; byte_index < sizeof(TCarrier); ++byte_index) {
        result.bytes[byte_index] = static_cast<CHAOS_IL2CPP_UINT8>(left_value.bytes[byte_index] ^ right_value.bytes[byte_index]);
    }

    return result;
}

template <typename TCarrier>
inline TCarrier VectorFixedBitwiseAndNot(TCarrier left_value, TCarrier right_value) {
    TCarrier result = {};
    for (CHAOS_IL2CPP_SIZE byte_index = 0; byte_index < sizeof(TCarrier); ++byte_index) {
        result.bytes[byte_index] = static_cast<CHAOS_IL2CPP_UINT8>(~left_value.bytes[byte_index] & right_value.bytes[byte_index]);
    }

    return result;
}

template <typename TCarrier>
inline TCarrier VectorFixedBitwiseSelect(TCarrier mask_value, TCarrier left_value, TCarrier right_value) {
    TCarrier result = {};
    for (CHAOS_IL2CPP_SIZE byte_index = 0; byte_index < sizeof(TCarrier); ++byte_index) {
        result.bytes[byte_index] = static_cast<CHAOS_IL2CPP_UINT8>(
            (mask_value.bytes[byte_index] & left_value.bytes[byte_index]) |
            (static_cast<CHAOS_IL2CPP_UINT8>(~mask_value.bytes[byte_index]) & right_value.bytes[byte_index]));
    }

    return result;
}

template <typename TCarrier>
inline TCarrier VectorFixedTernaryLogic(
    TCarrier first_value,
    TCarrier second_value,
    TCarrier third_value,
    CHAOS_IL2CPP_UINT8 control) {
    TCarrier result = {};
    for (CHAOS_IL2CPP_SIZE byte_index = 0; byte_index < sizeof(TCarrier); ++byte_index) {
        CHAOS_IL2CPP_UINT8 byte_value = 0u;
        for (CHAOS_IL2CPP_UINT32 bit_index = 0; bit_index < 8u; ++bit_index) {
            const auto first_bit = static_cast<CHAOS_IL2CPP_UINT8>((first_value.bytes[byte_index] >> bit_index) & 0x1u);
            const auto second_bit = static_cast<CHAOS_IL2CPP_UINT8>((second_value.bytes[byte_index] >> bit_index) & 0x1u);
            const auto third_bit = static_cast<CHAOS_IL2CPP_UINT8>((third_value.bytes[byte_index] >> bit_index) & 0x1u);
            const auto control_index = static_cast<CHAOS_IL2CPP_UINT8>((first_bit << 2u) | (second_bit << 1u) | third_bit);
            const auto result_bit = static_cast<CHAOS_IL2CPP_UINT8>((control >> control_index) & 0x1u);
            byte_value = static_cast<CHAOS_IL2CPP_UINT8>(byte_value | static_cast<CHAOS_IL2CPP_UINT8>(result_bit << bit_index));
        }

        result.bytes[byte_index] = byte_value;
    }

    return result;
}

template <typename TCarrier>
inline TCarrier VectorFixedOnesComplement(TCarrier value) {
    TCarrier result = {};
    for (CHAOS_IL2CPP_SIZE byte_index = 0; byte_index < sizeof(TCarrier); ++byte_index) {
        result.bytes[byte_index] = static_cast<CHAOS_IL2CPP_UINT8>(~value.bytes[byte_index]);
    }

    return result;
}

template <typename TCarrier>
inline TCarrier VectorFixedOneFromCapability(const RuntimeTypeCapabilityInfoV0& capability_info) {
    if (capability_info.vector_lane_kind == TypeCapabilityVectorLaneKindFloating) {
        if (capability_info.value_size_bytes == sizeof(float)) {
            return VectorFixedBroadcast<float, TCarrier>(1.0f);
        }

        if (capability_info.value_size_bytes == sizeof(double)) {
            return VectorFixedBroadcast<double, TCarrier>(1.0);
        }

        return {};
    }

    switch (capability_info.value_size_bytes) {
        case 1u:
            return VectorFixedBroadcast<CHAOS_IL2CPP_UINT8, TCarrier>(static_cast<CHAOS_IL2CPP_UINT8>(1u));
        case 2u:
            return VectorFixedBroadcast<CHAOS_IL2CPP_UINT16, TCarrier>(static_cast<CHAOS_IL2CPP_UINT16>(1u));
        case 4u:
            return VectorFixedBroadcast<CHAOS_IL2CPP_UINT32, TCarrier>(static_cast<CHAOS_IL2CPP_UINT32>(1u));
        case 8u:
            return VectorFixedBroadcast<CHAOS_IL2CPP_UINT64, TCarrier>(static_cast<CHAOS_IL2CPP_UINT64>(1u));
        default:
            return {};
    }
}

const RuntimeAbiV0* GetRuntimeAbiV0();
const MarshalPlatformAbiRootV1* GetMarshalPlatformAbiRootV1();
const TaskRuntimeKernelV1* GetTaskRuntimeKernelV1();

/* GC allocation helpers for generated code. GcAllocate returns zeroed memory
   (scanned for pointers). GcAllocateAtomic returns zeroed memory that is NOT
   scanned for pointers — use for pointer-free data (e.g. string UTF-8 bytes). */
void* GcAllocate(CHAOS_IL2CPP_SIZE size);
void* GcAllocateAtomic(CHAOS_IL2CPP_SIZE size);
RuntimeStatus TypeQueryCapability(TypeInfoHandle type, RuntimeTypeCapabilityInfoV0* out_capability_info);
RuntimeMode GetRuntimeMode();
void SetRuntimeMode(RuntimeMode mode);
bool IsMixedMode();
void* BoxValueObject(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    TypeInfoHandle value_type,
    const void* value,
    CHAOS_IL2CPP_SIZE value_size);
RuntimeStatus UnboxValueObject(
    RuntimeState* runtime_state,
    void* boxed_object,
    void* out_value,
    CHAOS_IL2CPP_SIZE out_value_size);
bool ArrayStoreReference(
    void* array_instance,
    CHAOS_IL2CPP_UINTPTR index,
    void* value);
void* ArrayLoadReference(
    void* array_instance,
    CHAOS_IL2CPP_UINTPTR index);
bool ArrayCopyReferenceRange(
    void* source_array_instance,
    CHAOS_IL2CPP_UINTPTR source_index,
    void* target_array_instance,
    CHAOS_IL2CPP_UINTPTR target_index,
    CHAOS_IL2CPP_UINTPTR length);
bool ArrayClearReferenceRange(
    void* array_instance,
    CHAOS_IL2CPP_UINTPTR start_index,
    CHAOS_IL2CPP_UINTPTR length);
bool ArrayReverseReferenceRange(
    void* array_instance,
    CHAOS_IL2CPP_UINTPTR start_index,
    CHAOS_IL2CPP_UINTPTR length);
CHAOS_IL2CPP_INT32 EngineLogWrite(
    const char* category_utf8,
    const char* message_utf8);
CHAOS_IL2CPP_UINTPTR CreateEngineObjectHandle(void* object_instance);
void* ResolveEngineObjectHandle(CHAOS_IL2CPP_UINTPTR handle);
bool RegisterEngineLifecycleCallback(
    const char* phase_utf8,
    EngineLifecycleCallback callback,
    void* user_data);
bool DispatchEngineLifecycleCallbacks(const char* phase_utf8);
bool IsMainThreadLane();
bool ThreadStaticInt32Add(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const char* slot_key_utf8,
    CHAOS_IL2CPP_INT32 delta,
    CHAOS_IL2CPP_INT32* out_value);
bool MonitorEnter(void* monitor_target);
bool MonitorExit(void* monitor_target);
bool GcSafepoint(
    RuntimeState* runtime_state,
    ThreadState* thread_state);
CHAOS_IL2CPP_SIZE ReportThreadRoot(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const void* root_address,
    CHAOS_IL2CPP_SIZE root_size);
bool EnqueueFinalizer(
    RuntimeState* runtime_state,
    void* object_instance,
    FinalizerCallback finalizer);
CHAOS_IL2CPP_SIZE DrainFinalizerQueue(RuntimeState* runtime_state);
CHAOS_IL2CPP_INTPTR MarshalAllocHGlobal(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR size);
CHAOS_IL2CPP_INTPTR MarshalAllocCoTaskMem(RuntimeState* runtime_state, CHAOS_IL2CPP_INT32 size);
CHAOS_IL2CPP_INTPTR MarshalReAllocHGlobal(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory, CHAOS_IL2CPP_INTPTR size);
CHAOS_IL2CPP_INTPTR MarshalReAllocCoTaskMem(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory, CHAOS_IL2CPP_INT32 size);
bool MarshalFreeHGlobal(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory);
bool MarshalFreeCoTaskMem(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory);
bool MarshalZeroFreeCoTaskMemUtf8(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory);
void* MarshalPtrToStringUtf8(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    CHAOS_IL2CPP_INTPTR utf8_buffer,
    CHAOS_IL2CPP_INT32 length,
    bool has_explicit_length);
CHAOS_IL2CPP_INTPTR MarshalStringToCoTaskMemUtf8(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    void* managed_string);
CHAOS_IL2CPP_UINT8 MarshalReadByte(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset);
CHAOS_IL2CPP_INT16 MarshalReadInt16(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset);
CHAOS_IL2CPP_INT32 MarshalReadInt32(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset);
CHAOS_IL2CPP_INT64 MarshalReadInt64(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset);
CHAOS_IL2CPP_INTPTR MarshalReadIntPtr(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset);
void MarshalWriteByte(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_UINT8 value);
void MarshalWriteInt16(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INT16 value);
void MarshalWriteInt32(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INT32 value);
void MarshalWriteInt64(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INT64 value);
void MarshalWriteIntPtr(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INTPTR value);
CHAOS_IL2CPP_INT32 TaskKernelNewId();
CHAOS_IL2CPP_INT32 InteropKernel32GetLastError();
CHAOS_IL2CPP_INT32 InteropKernel32GetCurrentProcessId();
CHAOS_IL2CPP_INT32 InteropKernel32GetCurrentThreadId();
CHAOS_IL2CPP_INTPTR InteropKernel32GetCurrentProcess();
CHAOS_IL2CPP_INTPTR InteropKernel32GetCurrentThread();
bool InteropKernel32CloseHandle(CHAOS_IL2CPP_INTPTR handle);
bool InteropKernel32FreeLibrary(CHAOS_IL2CPP_INTPTR handle);
ValueTypeKernelBackendKind GetCharClassificationKernelBackend();
ValueTypeKernelBackendKind GetFloatingClassificationKernelBackend();
ValueTypeKernelBackendKind GetBitArithmeticKernelBackend();
ValueTypeKernelBackendKind GetTickArithmeticKernelBackend();
VectorKernelBackendKind GetVectorKernelBackend();
bool CharIsAscii(CHAOS_IL2CPP_UINT16 value);
bool CharIsAsciiDigit(CHAOS_IL2CPP_UINT16 value);
bool CharIsAsciiHexDigit(CHAOS_IL2CPP_UINT16 value);
bool CharIsAsciiHexDigitLower(CHAOS_IL2CPP_UINT16 value);
bool CharIsAsciiHexDigitUpper(CHAOS_IL2CPP_UINT16 value);
bool CharIsAsciiLetter(CHAOS_IL2CPP_UINT16 value);
bool CharIsAsciiLetterLower(CHAOS_IL2CPP_UINT16 value);
bool CharIsAsciiLetterOrDigit(CHAOS_IL2CPP_UINT16 value);
bool CharIsAsciiLetterUpper(CHAOS_IL2CPP_UINT16 value);
bool CharIsBetween(CHAOS_IL2CPP_UINT16 value, CHAOS_IL2CPP_UINT16 lower_bound, CHAOS_IL2CPP_UINT16 upper_bound);
bool CharIsHighSurrogate(CHAOS_IL2CPP_UINT16 value);
bool CharIsLatin1(CHAOS_IL2CPP_UINT16 value);
bool CharIsLowSurrogate(CHAOS_IL2CPP_UINT16 value);
bool CharIsSeparatorLatin1(CHAOS_IL2CPP_UINT16 value);
bool CharIsSurrogate(CHAOS_IL2CPP_UINT16 value);
bool CharIsSurrogatePair(CHAOS_IL2CPP_UINT16 high_surrogate, CHAOS_IL2CPP_UINT16 low_surrogate);
bool CharIsWhiteSpaceLatin1(CHAOS_IL2CPP_UINT16 value);
CHAOS_IL2CPP_INT32 CharCompare(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value);
bool CharEquals(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value);
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
CHAOS_IL2CPP_INT32 SingleCompare(float left_value, float right_value);
bool SingleEquals(float left_value, float right_value);
bool SingleIsEvenInteger(float value);
bool SingleIsFinite(float value);
bool SingleIsNaN(float value);
bool SingleIsNaNOrZero(float value);
bool SingleIsNegative(float value);
bool SingleIsNegativeInfinity(float value);
bool SingleIsNormal(float value);
bool SingleIsOddInteger(float value);
bool SingleIsPositive(float value);
bool SingleIsPositiveInfinity(float value);
bool SingleIsPow2(float value);
bool SingleIsRealNumber(float value);
bool SingleIsSubnormal(float value);
bool SingleIsZero(float value);
bool SingleIsInteger(float value);
bool SingleIsInfinity(float value);
CHAOS_IL2CPP_INT32 DoubleCompare(double left_value, double right_value);
bool DoubleEquals(double left_value, double right_value);
bool DoubleIsEvenInteger(double value);
bool DoubleIsFinite(double value);
bool DoubleIsNaN(double value);
bool DoubleIsNaNOrZero(double value);
bool DoubleIsNegative(double value);
bool DoubleIsNegativeInfinity(double value);
bool DoubleIsNormal(double value);
bool DoubleIsOddInteger(double value);
bool DoubleIsPositive(double value);
bool DoubleIsPositiveInfinity(double value);
bool DoubleIsPow2(double value);
bool DoubleIsRealNumber(double value);
bool DoubleIsSubnormal(double value);
bool DoubleIsZero(double value);
bool DoubleIsInteger(double value);
bool DoubleIsInfinity(double value);
bool NFloatIsEvenInteger(double value);
bool NFloatIsFinite(double value);
bool NFloatIsNaN(double value);
bool NFloatIsNegative(double value);
bool NFloatIsNegativeInfinity(double value);
bool NFloatIsNormal(double value);
bool NFloatIsOddInteger(double value);
bool NFloatIsPositive(double value);
bool NFloatIsPositiveInfinity(double value);
bool NFloatIsPow2(double value);
bool NFloatIsRealNumber(double value);
bool NFloatIsSubnormal(double value);
bool NFloatIsInteger(double value);
bool NFloatIsInfinity(double value);
CHAOS_IL2CPP_INT32 NFloatCompare(double left_value, double right_value);
bool NFloatEquals(double left_value, double right_value);
CHAOS_IL2CPP_INT32 Int128Compare(const void* left_value, const void* right_value);
bool Int128Equals(const void* left_value, const void* right_value);
CHAOS_IL2CPP_INT32 UInt128Compare(const void* left_value, const void* right_value);
bool UInt128Equals(const void* left_value, const void* right_value);
CHAOS_IL2CPP_INT32 IntPtrCompare(CHAOS_IL2CPP_INTPTR left_value, CHAOS_IL2CPP_INTPTR right_value);
bool IntPtrEquals(CHAOS_IL2CPP_INTPTR left_value, CHAOS_IL2CPP_INTPTR right_value);
CHAOS_IL2CPP_INT32 UIntPtrCompare(CHAOS_IL2CPP_UINTPTR left_value, CHAOS_IL2CPP_UINTPTR right_value);
bool UIntPtrEquals(CHAOS_IL2CPP_UINTPTR left_value, CHAOS_IL2CPP_UINTPTR right_value);
CHAOS_IL2CPP_INT32 DateTimeCompareTicks(const void* left_value, const void* right_value);
bool DateTimeEqualsTicks(const void* left_value, const void* right_value);
CHAOS_IL2CPP_INT32 TimeSpanCompareTicks(const void* left_value, const void* right_value);
bool TimeSpanEqualsTicks(const void* left_value, const void* right_value);
CHAOS_IL2CPP_INT32 DateOnlyCompareDayNumber(CHAOS_IL2CPP_INT32 left_value, CHAOS_IL2CPP_INT32 right_value);
bool DateOnlyEqualsDayNumber(CHAOS_IL2CPP_INT32 left_value, CHAOS_IL2CPP_INT32 right_value);
CHAOS_IL2CPP_INT32 TimeOnlyCompareTicksValue(CHAOS_IL2CPP_INT64 left_value, CHAOS_IL2CPP_INT64 right_value);
bool TimeOnlyEqualsTicksValue(CHAOS_IL2CPP_INT64 left_value, CHAOS_IL2CPP_INT64 right_value);
RuntimeNumericsVector2Carrier Vector2Add(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value);
RuntimeNumericsVector2Carrier Vector2Subtract(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value);
RuntimeNumericsVector2Carrier Vector2Zero();
RuntimeNumericsVector2Carrier Vector2One();
RuntimeNumericsVector2Carrier Vector2UnitX();
RuntimeNumericsVector2Carrier Vector2UnitY();
RuntimeNumericsVector2Carrier Vector2Multiply(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value);
RuntimeNumericsVector2Carrier Vector2MultiplyScalar(RuntimeNumericsVector2Carrier value, float scalar);
RuntimeNumericsVector2Carrier Vector2Divide(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value);
RuntimeNumericsVector2Carrier Vector2DivideScalar(RuntimeNumericsVector2Carrier value, float scalar);
RuntimeNumericsVector2Carrier Vector2Abs(RuntimeNumericsVector2Carrier value);
RuntimeNumericsVector2Carrier Vector2Min(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value);
RuntimeNumericsVector2Carrier Vector2Max(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value);
RuntimeNumericsVector2Carrier Vector2Clamp(RuntimeNumericsVector2Carrier value, RuntimeNumericsVector2Carrier min_value, RuntimeNumericsVector2Carrier max_value);
RuntimeNumericsVector2Carrier Vector2SquareRoot(RuntimeNumericsVector2Carrier value);
RuntimeNumericsVector2Carrier Vector2Normalize(RuntimeNumericsVector2Carrier value);
RuntimeNumericsVector2Carrier Vector2Lerp(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value, float amount);
RuntimeNumericsVector2Carrier Vector2Reflect(RuntimeNumericsVector2Carrier vector, RuntimeNumericsVector2Carrier normal);
bool Vector2Equals(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value);
CHAOS_IL2CPP_INT32 Vector2GetHashCode(RuntimeNumericsVector2Carrier value);
float Vector2Length(RuntimeNumericsVector2Carrier value);
float Vector2GetElement(RuntimeNumericsVector2Carrier value, CHAOS_IL2CPP_INT32 index);
RuntimeNumericsVector2Carrier Vector2Negate(RuntimeNumericsVector2Carrier value);
float Vector2Dot(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value);
float Vector2LengthSquared(RuntimeNumericsVector2Carrier value);
float Vector2DistanceSquared(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value);
RuntimeNumericsVector2Carrier Vector2Transform(RuntimeNumericsVector2Carrier value, RuntimeNumericsMatrix3x2Carrier matrix);
RuntimeNumericsVector2Carrier Vector2Transform(RuntimeNumericsVector2Carrier value, RuntimeNumericsMatrix4x4Carrier matrix);
RuntimeNumericsVector2Carrier Vector2Transform(RuntimeNumericsVector2Carrier value, RuntimeNumericsQuaternionCarrier rotation);
RuntimeNumericsVector2Carrier Vector2TransformNormal(RuntimeNumericsVector2Carrier value, RuntimeNumericsMatrix3x2Carrier matrix);
RuntimeNumericsVector2Carrier Vector2TransformNormal(RuntimeNumericsVector2Carrier value, RuntimeNumericsMatrix4x4Carrier matrix);
bool Vector2TryCopyTo(RuntimeNumericsVector2Carrier value, float* destination, CHAOS_IL2CPP_SIZE destination_length, CHAOS_IL2CPP_SIZE start_index);
RuntimeNumericsVector3Carrier Vector3Add(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value);
RuntimeNumericsVector3Carrier Vector3Subtract(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value);
RuntimeNumericsVector3Carrier Vector3Zero();
RuntimeNumericsVector3Carrier Vector3One();
RuntimeNumericsVector3Carrier Vector3UnitX();
RuntimeNumericsVector3Carrier Vector3UnitY();
RuntimeNumericsVector3Carrier Vector3UnitZ();
RuntimeNumericsVector3Carrier Vector3Multiply(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value);
RuntimeNumericsVector3Carrier Vector3MultiplyScalar(RuntimeNumericsVector3Carrier value, float scalar);
RuntimeNumericsVector3Carrier Vector3Divide(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value);
RuntimeNumericsVector3Carrier Vector3DivideScalar(RuntimeNumericsVector3Carrier value, float scalar);
RuntimeNumericsVector3Carrier Vector3Abs(RuntimeNumericsVector3Carrier value);
RuntimeNumericsVector3Carrier Vector3Min(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value);
RuntimeNumericsVector3Carrier Vector3Max(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value);
RuntimeNumericsVector3Carrier Vector3Clamp(RuntimeNumericsVector3Carrier value, RuntimeNumericsVector3Carrier min_value, RuntimeNumericsVector3Carrier max_value);
RuntimeNumericsVector3Carrier Vector3SquareRoot(RuntimeNumericsVector3Carrier value);
RuntimeNumericsVector3Carrier Vector3Normalize(RuntimeNumericsVector3Carrier value);
RuntimeNumericsVector3Carrier Vector3Lerp(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value, float amount);
RuntimeNumericsVector3Carrier Vector3Reflect(RuntimeNumericsVector3Carrier vector, RuntimeNumericsVector3Carrier normal);
RuntimeNumericsVector3Carrier Vector3Cross(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value);
bool Vector3Equals(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value);
CHAOS_IL2CPP_INT32 Vector3GetHashCode(RuntimeNumericsVector3Carrier value);
float Vector3Length(RuntimeNumericsVector3Carrier value);
float Vector3GetElement(RuntimeNumericsVector3Carrier value, CHAOS_IL2CPP_INT32 index);
RuntimeNumericsVector3Carrier Vector3Negate(RuntimeNumericsVector3Carrier value);
float Vector3Dot(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value);
float Vector3LengthSquared(RuntimeNumericsVector3Carrier value);
float Vector3DistanceSquared(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value);
RuntimeNumericsVector3Carrier Vector3Transform(RuntimeNumericsVector3Carrier value, RuntimeNumericsMatrix4x4Carrier matrix);
RuntimeNumericsVector3Carrier Vector3Transform(RuntimeNumericsVector3Carrier value, RuntimeNumericsQuaternionCarrier rotation);
RuntimeNumericsVector3Carrier Vector3TransformNormal(RuntimeNumericsVector3Carrier value, RuntimeNumericsMatrix4x4Carrier matrix);
bool Vector3TryCopyTo(RuntimeNumericsVector3Carrier value, float* destination, CHAOS_IL2CPP_SIZE destination_length, CHAOS_IL2CPP_SIZE start_index);
RuntimeNumericsVector4Carrier Vector4Add(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value);
RuntimeNumericsVector4Carrier Vector4Subtract(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value);
RuntimeNumericsVector4Carrier Vector4Zero();
RuntimeNumericsVector4Carrier Vector4One();
RuntimeNumericsVector4Carrier Vector4UnitX();
RuntimeNumericsVector4Carrier Vector4UnitY();
RuntimeNumericsVector4Carrier Vector4UnitZ();
RuntimeNumericsVector4Carrier Vector4UnitW();
RuntimeNumericsVector4Carrier Vector4Multiply(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value);
RuntimeNumericsVector4Carrier Vector4MultiplyScalar(RuntimeNumericsVector4Carrier value, float scalar);
RuntimeNumericsVector4Carrier Vector4Divide(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value);
RuntimeNumericsVector4Carrier Vector4DivideScalar(RuntimeNumericsVector4Carrier value, float scalar);
RuntimeNumericsVector4Carrier Vector4Abs(RuntimeNumericsVector4Carrier value);
RuntimeNumericsVector4Carrier Vector4Min(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value);
RuntimeNumericsVector4Carrier Vector4Max(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value);
RuntimeNumericsVector4Carrier Vector4Clamp(RuntimeNumericsVector4Carrier value, RuntimeNumericsVector4Carrier min_value, RuntimeNumericsVector4Carrier max_value);
RuntimeNumericsVector4Carrier Vector4SquareRoot(RuntimeNumericsVector4Carrier value);
RuntimeNumericsVector4Carrier Vector4Normalize(RuntimeNumericsVector4Carrier value);
RuntimeNumericsVector4Carrier Vector4Lerp(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value, float amount);
bool Vector4Equals(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value);
CHAOS_IL2CPP_INT32 Vector4GetHashCode(RuntimeNumericsVector4Carrier value);
float Vector4Length(RuntimeNumericsVector4Carrier value);
float Vector4GetElement(RuntimeNumericsVector4Carrier value, CHAOS_IL2CPP_INT32 index);
RuntimeNumericsVector4Carrier Vector4Negate(RuntimeNumericsVector4Carrier value);
float Vector4Dot(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value);
float Vector4LengthSquared(RuntimeNumericsVector4Carrier value);
float Vector4DistanceSquared(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value);
RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector2Carrier value, RuntimeNumericsMatrix4x4Carrier matrix);
RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector2Carrier value, RuntimeNumericsQuaternionCarrier rotation);
RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector3Carrier value, RuntimeNumericsMatrix4x4Carrier matrix);
RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector3Carrier value, RuntimeNumericsQuaternionCarrier rotation);
RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector4Carrier value, RuntimeNumericsMatrix4x4Carrier matrix);
RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector4Carrier value, RuntimeNumericsQuaternionCarrier rotation);
bool Vector4TryCopyTo(RuntimeNumericsVector4Carrier value, float* destination, CHAOS_IL2CPP_SIZE destination_length, CHAOS_IL2CPP_SIZE start_index);
RuntimeIntrinsicVector64Carrier Vector64Reinterpret(RuntimeIntrinsicVector64Carrier value);
RuntimeIntrinsicVector128Carrier Vector128Reinterpret(RuntimeIntrinsicVector128Carrier value);
RuntimeIntrinsicVector256Carrier Vector256Reinterpret(RuntimeIntrinsicVector256Carrier value);
RuntimeIntrinsicVector512Carrier Vector512Reinterpret(RuntimeIntrinsicVector512Carrier value);
CHAOS_IL2CPP_INT32 Vector64Count();
CHAOS_IL2CPP_INT32 Vector128Count();
CHAOS_IL2CPP_INT32 Vector256Count();
CHAOS_IL2CPP_INT32 Vector512Count();
CHAOS_IL2CPP_INT32 VectorPlatformByteWidth();
bool Vector64IsSupported();
bool Vector128IsSupported();
bool Vector256IsSupported();
bool Vector512IsSupported();

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_RUNTIME_CORE_H_
