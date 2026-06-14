#ifndef CHAOS_IL2CPP_VECTOR_FIXED_TEMPLATES_H_
#define CHAOS_IL2CPP_VECTOR_FIXED_TEMPLATES_H_

#include <chaos/native_types.h>
#include <cstring>
#include <cstdint>
#include <runtime_abi.h>
#include "runtime_capability.h"
#include "numerics_carriers.h"
#include <runtime_core.h>  // GetCurrentRuntimeState, GetRuntimeAbiV0

// windows.h max/min macros conflict with std::numeric_limits::max/min
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

namespace chaos::il2cpp::vector_fixed {

// Bring carrier types into scope for MSVC two-phase lookup inside templates.
using ::chaos::il2cpp::numerics_carriers::RuntimeNumericsVector2Carrier;
using ::chaos::il2cpp::numerics_carriers::RuntimeNumericsVector3Carrier;
using ::chaos::il2cpp::numerics_carriers::RuntimeNumericsVector4Carrier;
using ::chaos::il2cpp::numerics_carriers::RuntimeNumericsMatrix3x2Carrier;
using ::chaos::il2cpp::numerics_carriers::RuntimeNumericsMatrix4x4Carrier;
using ::chaos::il2cpp::numerics_carriers::RuntimeNumericsQuaternionCarrier;
using ::chaos::il2cpp::numerics_carriers::RuntimeIntrinsicVector64Carrier;
using ::chaos::il2cpp::numerics_carriers::RuntimeIntrinsicVector128Carrier;
using ::chaos::il2cpp::numerics_carriers::RuntimeIntrinsicVector256Carrier;
using ::chaos::il2cpp::numerics_carriers::RuntimeIntrinsicVector512Carrier;

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
            using TUnsigned = std::make_unsigned_t<TScalar>;
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
            using TUnsigned = std::make_unsigned_t<TScalar>;
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
    static_assert(std::is_signed_v<TScalar>, "arithmetic right shift requires signed scalar");
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
            using TUnsigned = std::make_unsigned_t<TScalar>;
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
            using TUnsigned = std::make_unsigned_t<TScalar>;
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
    static_assert(std::is_signed_v<TScalar>, "arithmetic right shift requires signed scalar");
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
    using TUnsigned = std::make_unsigned_t<TScalar>;
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
    using TUnsigned = std::make_unsigned_t<TScalar>;
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
    using TUnsigned = std::make_unsigned_t<TScalar>;
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
    using TUnsigned = std::make_unsigned_t<TScalar>;
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
    static_assert(sizeof(TCarrier) == sizeof(chaos::il2cpp::numerics_carriers::RuntimeIntrinsicVector256Carrier), "Shuffle2x128 requires 256-bit carrier");

    chaos::il2cpp::numerics_carriers::RuntimeIntrinsicVector128Carrier left_halves[2];
    chaos::il2cpp::numerics_carriers::RuntimeIntrinsicVector128Carrier right_halves[2];
    CHAOS_IL2CPP_MEMCPY(left_halves, &left_value, sizeof(TCarrier));
    CHAOS_IL2CPP_MEMCPY(right_halves, &right_value, sizeof(TCarrier));

    auto select_half = [&](CHAOS_IL2CPP_UINT8 selector) -> chaos::il2cpp::numerics_carriers::RuntimeIntrinsicVector128Carrier {
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

    chaos::il2cpp::numerics_carriers::RuntimeIntrinsicVector128Carrier result_halves[2] = {};
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
    if constexpr (std::is_floating_point_v<TInputScalar>) {
        if (!CHAOS_IL2CPP_ISFINITE(value)) {
            return static_cast<TOutputScalar>(0);
        }

        return static_cast<TOutputScalar>(CHAOS_IL2CPP_TRUNC(value));
    }

    return static_cast<TOutputScalar>(value);
}

template <typename TOutputScalar, typename TInputScalar>
inline TOutputScalar VectorFixedConvertScalarSaturating(TInputScalar value) {
    if constexpr (std::is_floating_point_v<TInputScalar>) {
        if (!CHAOS_IL2CPP_ISFINITE(value)) {
            return static_cast<TOutputScalar>(0);
        }
    }

    using TOutputLimits = std::numeric_limits<TOutputScalar>;
    using TInputLimits = std::numeric_limits<TInputScalar>;

    if constexpr (std::is_same_v<TOutputScalar, TInputScalar>) {
        return value;
    } else if constexpr (std::is_integral_v<TInputScalar> && std::is_integral_v<TOutputScalar>) {
        if constexpr (std::is_signed_v<TInputScalar> == std::is_signed_v<TOutputScalar>) {
            if (value < static_cast<TInputScalar>(TOutputLimits::lowest())) {
                return TOutputLimits::lowest();
            }

            if (value > static_cast<TInputScalar>(TOutputLimits::max())) {
                return TOutputLimits::max();
            }

            return static_cast<TOutputScalar>(value);
        } else if constexpr (std::is_signed_v<TInputScalar>) {
            if (value <= static_cast<TInputScalar>(0)) {
                return static_cast<TOutputScalar>(0);
            }

            using TUnsignedInput = std::make_unsigned_t<TInputScalar>;
            const auto unsigned_value = static_cast<TUnsignedInput>(value);
            if (unsigned_value > static_cast<TUnsignedInput>(TOutputLimits::max())) {
                return TOutputLimits::max();
            }

            return static_cast<TOutputScalar>(value);
        } else {
            if (value > static_cast<std::make_unsigned_t<TOutputScalar>>(TOutputLimits::max())) {
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
    static_assert(sizeof(TOutputCarrier) == sizeof(chaos::il2cpp::numerics_carriers::RuntimeIntrinsicVector128Carrier), "output carrier must be 128-bit");
    return VectorFixedConvertToVector<TOutputScalar, TInputScalar, TOutputCarrier, TInputCarrier>(value);
}

template <typename TOutputScalar, typename TInputScalar, typename TOutputCarrier, typename TInputCarrier>
inline TOutputCarrier VectorFixedConvertToVector128Saturating(TInputCarrier value) {
    static_assert(sizeof(TOutputCarrier) == sizeof(chaos::il2cpp::numerics_carriers::RuntimeIntrinsicVector128Carrier), "output carrier must be 128-bit");
    return VectorFixedConvertToVectorSaturating<TOutputScalar, TInputScalar, TOutputCarrier, TInputCarrier>(value);
}

template <typename TOutputScalar, typename TInputScalar, typename TOutputCarrier, typename TInputCarrier>
inline TOutputCarrier VectorFixedConvertToVector128Truncating(TInputCarrier value) {
    static_assert(sizeof(TOutputCarrier) == sizeof(chaos::il2cpp::numerics_carriers::RuntimeIntrinsicVector128Carrier), "output carrier must be 128-bit");
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
    (void)mask_value;
    static_assert(sizeof(TScalar) == sizeof(TMaskScalar), "fixup mask width must match scalar width");
    return VectorFixedApplyBinary<TScalar, TCarrier>(
        left_value,
        right_value,
        [](TScalar left_lane, TScalar right_lane) {
            (void)right_lane;
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

template <typename TScalar, typename TCarrier>
inline TScalar VectorFixedGetElement(TCarrier value, CHAOS_IL2CPP_INT32 index) {
    static_assert(sizeof(TCarrier) % sizeof(TScalar) == 0u, "carrier size must be divisible by scalar size");
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);
    TScalar lanes[lane_count];
    CHAOS_IL2CPP_MEMCPY(lanes, &value, sizeof(TCarrier));
    const auto lane_index = static_cast<CHAOS_IL2CPP_SIZE>(index);
    return lane_index < lane_count ? lanes[lane_index] : TScalar{};
}

template <typename TScalar, typename TCarrier>
inline CHAOS_IL2CPP_INT32 VectorFixedGetHashCode(TCarrier value) {
    static_assert(sizeof(TCarrier) % sizeof(TScalar) == 0u, "carrier size must be divisible by scalar size");
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);
    TScalar lanes[lane_count];
    CHAOS_IL2CPP_MEMCPY(lanes, &value, sizeof(TCarrier));
    CHAOS_IL2CPP_INT32 hash = 0;
    for (CHAOS_IL2CPP_SIZE i = 0; i < lane_count; ++i) {
        CHAOS_IL2CPP_INT32 lane_hash;
        if constexpr (std::is_floating_point_v<TScalar>) {
            if constexpr (sizeof(TScalar) == sizeof(CHAOS_IL2CPP_UINT32)) {
                CHAOS_IL2CPP_UINT32 bits;
                CHAOS_IL2CPP_MEMCPY(&bits, &lanes[i], sizeof(CHAOS_IL2CPP_UINT32));
                if (bits == 0x80000000u) { bits = 0u; }
                lane_hash = static_cast<CHAOS_IL2CPP_INT32>(bits);
            } else {
                CHAOS_IL2CPP_UINT64 bits;
                CHAOS_IL2CPP_MEMCPY(&bits, &lanes[i], sizeof(CHAOS_IL2CPP_UINT64));
                if (bits == 0x8000000000000000ull) { bits = 0ull; }
                lane_hash = static_cast<CHAOS_IL2CPP_INT32>(bits) ^ static_cast<CHAOS_IL2CPP_INT32>(bits >> 32);
            }
        } else if constexpr (sizeof(TScalar) <= sizeof(CHAOS_IL2CPP_INT32)) {
            using TUnsigned = std::make_unsigned_t<TScalar>;
            lane_hash = static_cast<CHAOS_IL2CPP_INT32>(static_cast<TUnsigned>(lanes[i]));
        } else {
            CHAOS_IL2CPP_UINT64 raw;
            CHAOS_IL2CPP_MEMCPY(&raw, &lanes[i], sizeof(CHAOS_IL2CPP_UINT64));
            lane_hash = static_cast<CHAOS_IL2CPP_INT32>(raw) ^ static_cast<CHAOS_IL2CPP_INT32>(raw >> 32);
        }
        hash = ((hash << 5) + hash) ^ lane_hash;
    }
    return hash;
}

template <typename TScalar, typename TCarrier>
inline void* VectorFixedFormatToString(TCarrier value) {
    static_assert(sizeof(TCarrier) % sizeof(TScalar) == 0u, "carrier size must be divisible by scalar size");
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);

    auto* runtime_state = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();

    TScalar lanes[lane_count];
    CHAOS_IL2CPP_MEMCPY(lanes, &value, sizeof(TCarrier));

    CHAOS_IL2CPP_STRING result = "<";
    for (CHAOS_IL2CPP_SIZE i = 0; i < lane_count; ++i) {
        result += CHAOS_IL2CPP_TO_STRING(lanes[i]);
        if (i + 1 < lane_count) {
            result += ", ";
        }
    }
    result += ">";

    const auto* abi = chaos::il2cpp::runtime_core::GetRuntimeAbiV0();
    return abi->string_new_utf8(runtime_state, nullptr, result.c_str(), static_cast<CHAOS_IL2CPP_UINTPTR>(result.size()));
}

template <typename TMaskScalar>
inline TMaskScalar VectorFixedAllBitsSetMaskLane() {
    if constexpr (std::is_floating_point_v<TMaskScalar>) {
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
        using TUnsigned = std::make_unsigned_t<TMaskScalar>;
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
        if constexpr (std::is_floating_point_v<TInputScalar>) {
            output_lanes[lane_index] = static_cast<TOutputScalar>(CHAOS_IL2CPP_FABS(lane));
        } else if constexpr (std::is_unsigned_v<TInputScalar>) {
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
    using namespace chaos::il2cpp::runtime_capability;
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

// ── Boolean predicates ──

/// Returns true if all bytes of the carrier are zero.
template <typename TCarrier>
inline bool VectorFixedIsAllZeros(const TCarrier& value) {
    for (CHAOS_IL2CPP_SIZE i = 0; i < sizeof(TCarrier); ++i) {
        if (reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(&value)[i] != 0)
            return false;
    }
    return true;
}

/// Returns true if every lane has all bits set.
template <typename TScalar, typename TCarrier>
inline bool VectorFixedAllLanesSet(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);
    const auto* lanes = reinterpret_cast<const TScalar*>(&value);
    TScalar all_ones = static_cast<TScalar>(~static_cast<TScalar>(0));
    for (CHAOS_IL2CPP_SIZE i = 0; i < lane_count; ++i) {
        if (lanes[i] != all_ones) return false;
    }
    return true;
}

/// Returns the number of lanes where all bits are set.
template <typename TScalar, typename TCarrier>
inline CHAOS_IL2CPP_INT32 VectorFixedCountWhereAllBitsSet(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);
    const auto* lanes = reinterpret_cast<const TScalar*>(&value);
    CHAOS_IL2CPP_INT32 count = 0;
    TScalar all_ones = static_cast<TScalar>(~static_cast<TScalar>(0));
    for (CHAOS_IL2CPP_SIZE i = 0; i < lane_count; ++i) {
        if (lanes[i] == all_ones) ++count;
    }
    return count;
}

/// Returns index of first lane matching scalar, or -1.
template <typename TScalar, typename TCarrier>
inline CHAOS_IL2CPP_INT32 VectorFixedIndexOf(const TCarrier& value, TScalar scalar) {
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);
    const auto* lanes = reinterpret_cast<const TScalar*>(&value);
    for (CHAOS_IL2CPP_SIZE i = 0; i < lane_count; ++i) {
        if (lanes[i] == scalar) return static_cast<CHAOS_IL2CPP_INT32>(i);
    }
    return -1;
}

/// Returns index of last lane matching scalar, or -1.
template <typename TScalar, typename TCarrier>
inline CHAOS_IL2CPP_INT32 VectorFixedLastIndexOf(const TCarrier& value, TScalar scalar) {
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);
    const auto* lanes = reinterpret_cast<const TScalar*>(&value);
    for (CHAOS_IL2CPP_SIZE i_ = lane_count; i_ > 0; --i_) {
        if (lanes[i_ - 1] == scalar) return static_cast<CHAOS_IL2CPP_INT32>(i_ - 1);
    }
    return -1;
}

/// For each lane: returns all-ones if the lane is zero, zero otherwise.
template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedIsZero(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    TScalar* dst = reinterpret_cast<TScalar*>(&result);
    const TScalar* src = reinterpret_cast<const TScalar*>(&value);
    TScalar all_ones = static_cast<TScalar>(~static_cast<TScalar>(0));
    // Use unsigned type for comparison to match JIT signed/unsigned behavior
    using UTScalar = typename std::conditional<sizeof(TScalar) == 1, CHAOS_IL2CPP_UINT8,
        typename std::conditional<sizeof(TScalar) == 2, CHAOS_IL2CPP_UINT16,
        typename std::conditional<sizeof(TScalar) == 4, CHAOS_IL2CPP_UINT32,
        CHAOS_IL2CPP_UINT64>::type>::type>::type;
    const UTScalar* usrc = reinterpret_cast<const UTScalar*>(&value);
    for (CHAOS_IL2CPP_SIZE i = 0; i < lane_count; ++i) {
        dst[i] = (usrc[i] == 0) ? all_ones : static_cast<TScalar>(0);
    }
    return result;
}

/// For each lane: returns all-ones if the lane is all-ones, zero otherwise.
template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedTestAllBitsSet(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    TScalar* dst = reinterpret_cast<TScalar*>(&result);
    const TScalar* src = reinterpret_cast<const TScalar*>(&value);
    TScalar all_ones = static_cast<TScalar>(~static_cast<TScalar>(0));
    for (CHAOS_IL2CPP_SIZE i = 0; i < lane_count; ++i) {
        dst[i] = (src[i] == all_ones) ? all_ones : static_cast<TScalar>(0);
    }
    return result;
}

// ── Saturating arithmetic ──

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedAddSaturate(const TCarrier& a, const TCarrier& b) {
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* al = reinterpret_cast<const TScalar*>(&a);
    const TScalar* bl = reinterpret_cast<const TScalar*>(&b);
    TScalar* rl = reinterpret_cast<TScalar*>(&result);
    for (CHAOS_IL2CPP_SIZE i = 0; i < lane_count; ++i) {
        // Use wider type for overflow detection
        if constexpr (sizeof(TScalar) == 1) {
            CHAOS_IL2CPP_INT16 wide = static_cast<CHAOS_IL2CPP_INT16>(al[i]) + static_cast<CHAOS_IL2CPP_INT16>(bl[i]);
            rl[i] = static_cast<TScalar>(wide > 127 ? 127 : (wide < -128 ? -128 : wide));
        } else if constexpr (sizeof(TScalar) == 2) {
            CHAOS_IL2CPP_INT32 wide = static_cast<CHAOS_IL2CPP_INT32>(al[i]) + static_cast<CHAOS_IL2CPP_INT32>(bl[i]);
            rl[i] = static_cast<TScalar>(wide > 32767 ? 32767 : (wide < -32768 ? -32768 : wide));
        } else if constexpr (sizeof(TScalar) == 4) {
            CHAOS_IL2CPP_INT64 wide = static_cast<CHAOS_IL2CPP_INT64>(al[i]) + static_cast<CHAOS_IL2CPP_INT64>(bl[i]);
            rl[i] = static_cast<TScalar>(wide > 2147483647LL ? 2147483647 : (wide < -2147483648LL ? -2147483648 : wide));
        } else {
            rl[i] = static_cast<TScalar>(0);
        }
    }
    return result;
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedSubtractSaturate(const TCarrier& a, const TCarrier& b) {
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* al = reinterpret_cast<const TScalar*>(&a);
    const TScalar* bl = reinterpret_cast<const TScalar*>(&b);
    TScalar* rl = reinterpret_cast<TScalar*>(&result);
    for (CHAOS_IL2CPP_SIZE i = 0; i < lane_count; ++i) {
        if constexpr (sizeof(TScalar) == 1) {
            CHAOS_IL2CPP_INT16 wide = static_cast<CHAOS_IL2CPP_INT16>(al[i]) - static_cast<CHAOS_IL2CPP_INT16>(bl[i]);
            rl[i] = static_cast<TScalar>(wide > 127 ? 127 : (wide < -128 ? -128 : wide));
        } else if constexpr (sizeof(TScalar) == 2) {
            CHAOS_IL2CPP_INT32 wide = static_cast<CHAOS_IL2CPP_INT32>(al[i]) - static_cast<CHAOS_IL2CPP_INT32>(bl[i]);
            rl[i] = static_cast<TScalar>(wide > 32767 ? 32767 : (wide < -32768 ? -32768 : wide));
        } else if constexpr (sizeof(TScalar) == 4) {
            CHAOS_IL2CPP_INT64 wide = static_cast<CHAOS_IL2CPP_INT64>(al[i]) - static_cast<CHAOS_IL2CPP_INT64>(bl[i]);
            rl[i] = static_cast<TScalar>(wide > 2147483647LL ? 2147483647 : (wide < -2147483648LL ? -2147483648 : wide));
        } else {
            rl[i] = static_cast<TScalar>(0);
        }
    }
    return result;
}

// ── Math (float/double) ──

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedSqrt(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    TScalar* rl = reinterpret_cast<TScalar*>(&result);
    for (CHAOS_IL2CPP_SIZE i = 0; i < lane_count; ++i) {
        if constexpr (std::is_same_v<TScalar, float>)
            rl[i] = std::sqrt(sl[i]);
        else if constexpr (std::is_same_v<TScalar, double>)
            rl[i] = std::sqrt(sl[i]);
        else rl[i] = static_cast<TScalar>(0);
    }
    return result;
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedCeiling(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    TScalar* rl = reinterpret_cast<TScalar*>(&result);
    for (CHAOS_IL2CPP_SIZE i = 0; i < lane_count; ++i) {
        if constexpr (std::is_same_v<TScalar, float>)
            rl[i] = std::ceil(sl[i]);
        else if constexpr (std::is_same_v<TScalar, double>)
            rl[i] = std::ceil(sl[i]);
        else rl[i] = sl[i];
    }
    return result;
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedFloor(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    TScalar* rl = reinterpret_cast<TScalar*>(&result);
    for (CHAOS_IL2CPP_SIZE i = 0; i < lane_count; ++i) {
        if constexpr (std::is_same_v<TScalar, float>)
            rl[i] = std::floor(sl[i]);
        else if constexpr (std::is_same_v<TScalar, double>)
            rl[i] = std::floor(sl[i]);
        else rl[i] = sl[i];
    }
    return result;
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedTruncate(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    TScalar* rl = reinterpret_cast<TScalar*>(&result);
    for (CHAOS_IL2CPP_SIZE i = 0; i < lane_count; ++i) {
        if constexpr (std::is_same_v<TScalar, float>)
            rl[i] = std::trunc(sl[i]);
        else if constexpr (std::is_same_v<TScalar, double>)
            rl[i] = std::trunc(sl[i]);
        else rl[i] = sl[i];
    }
    return result;
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedCopySign(const TCarrier& magnitude, const TCarrier& sign) {
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* ml = reinterpret_cast<const TScalar*>(&magnitude);
    const TScalar* sl = reinterpret_cast<const TScalar*>(&sign);
    TScalar* rl = reinterpret_cast<TScalar*>(&result);
    for (CHAOS_IL2CPP_SIZE i = 0; i < lane_count; ++i) {
        if constexpr (std::is_same_v<TScalar, float>)
            rl[i] = std::copysignf(ml[i], sl[i]);
        else if constexpr (std::is_same_v<TScalar, double>)
            rl[i] = std::copysign(ml[i], sl[i]);
        else rl[i] = static_cast<TScalar>(0);
    }
    return result;
}

// ── Min/Max variants ──

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedMaxNative(const TCarrier& a, const TCarrier& b) {
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* al = reinterpret_cast<const TScalar*>(&a);
    const TScalar* bl = reinterpret_cast<const TScalar*>(&b);
    TScalar* rl = reinterpret_cast<TScalar*>(&result);
    for (CHAOS_IL2CPP_SIZE i = 0; i < lane_count; ++i) {
        rl[i] = al[i] > bl[i] ? al[i] : bl[i];  // same as Max for integral types
    }
    return result;
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedMinNative(const TCarrier& a, const TCarrier& b) {
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* al = reinterpret_cast<const TScalar*>(&a);
    const TScalar* bl = reinterpret_cast<const TScalar*>(&b);
    TScalar* rl = reinterpret_cast<TScalar*>(&result);
    for (CHAOS_IL2CPP_SIZE i = 0; i < lane_count; ++i) {
        rl[i] = al[i] < bl[i] ? al[i] : bl[i];
    }
    return result;
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedClampNative(const TCarrier& value, const TCarrier& min, const TCarrier& max) {
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* vl = reinterpret_cast<const TScalar*>(&value);
    const TScalar* mn = reinterpret_cast<const TScalar*>(&min);
    const TScalar* mx = reinterpret_cast<const TScalar*>(&max);
    TScalar* rl = reinterpret_cast<TScalar*>(&result);
    for (CHAOS_IL2CPP_SIZE i = 0; i < lane_count; ++i) {
        rl[i] = vl[i] < mn[i] ? mn[i] : (vl[i] > mx[i] ? mx[i] : vl[i]);
    }
    return result;
}

// ── Conversions ──

template <typename TFromScalar, typename TToScalar, typename TFromCarrier, typename TToCarrier>
inline TToCarrier VectorFixedConvertToVector(const TFromCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TFromCarrier) / sizeof(TFromScalar);
    TToCarrier result{};
    const TFromScalar* sl = reinterpret_cast<const TFromScalar*>(&value);
    TToScalar* rl = reinterpret_cast<TToScalar*>(&result);
    for (CHAOS_IL2CPP_SIZE i = 0; i < lane_count; ++i)
        rl[i] = static_cast<TToScalar>(sl[i]);
    return result;
}

template <typename TFromScalar, typename TFromCarrier, typename TToCarrier>
inline TToCarrier VectorFixedWiden(const TFromCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TFromCarrier) / sizeof(TFromScalar);
    TToCarrier result{};
    using TToScalar = decltype(static_cast<TFromScalar>(0) + static_cast<TFromScalar>(0));  // fallback, won't compile
    const TFromScalar* sl = reinterpret_cast<const TFromScalar*>(&value);
    auto* rl = reinterpret_cast<TFromScalar*>(&result);
    for (CHAOS_IL2CPP_SIZE i = 0; i < lane_count; ++i)
        rl[i] = sl[i];
    return result;
}

// ── Reduction ──

template <typename TScalar, typename TCarrier>
inline TScalar VectorFixedSum(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    TScalar acc = static_cast<TScalar>(0);
    for (CHAOS_IL2CPP_SIZE i = 0; i < lane_count; ++i)
        acc += sl[i];
    return acc;
}

template <typename TScalar, typename TCarrier>
inline TScalar VectorFixedDot(const TCarrier& a, const TCarrier& b) {
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);
    const TScalar* al = reinterpret_cast<const TScalar*>(&a);
    const TScalar* bl = reinterpret_cast<const TScalar*>(&b);
    TScalar acc = static_cast<TScalar>(0);
    for (CHAOS_IL2CPP_SIZE i = 0; i < lane_count; ++i)
        acc += al[i] * bl[i];
    return acc;
}

template <typename TScalar, typename TCarrier>
inline CHAOS_IL2CPP_UINT64 VectorFixedExtractMostSignificantBits(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    CHAOS_IL2CPP_UINT64 result = 0;
    for (CHAOS_IL2CPP_SIZE i = 0; i < lane_count && i < 64; ++i)
        if ((sl[i] & (static_cast<TScalar>(1) << (sizeof(TScalar) * 8 - 1))) != 0)
            result |= (static_cast<CHAOS_IL2CPP_UINT64>(1) << i);
    return result;
}

// ── CopyTo ──

template <typename TScalar, typename TCarrier>
inline void VectorFixedCopyTo(const TCarrier& value, TScalar* dest, CHAOS_IL2CPP_INT32 startIndex) {
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    for (CHAOS_IL2CPP_SIZE i = 0; i < lane_count; ++i)
        dest[startIndex + i] = sl[i];
}

// ── Float/double predicates (return carrier mask: all-ones = true, 0 = false) ──

// Helper: float reinterpret as uint32
inline CHAOS_IL2CPP_UINT32 FloatAsUInt32(float f) {
    CHAOS_IL2CPP_UINT32 u;
    std::memcpy(&u, &f, sizeof(u));
    return u;
}
inline CHAOS_IL2CPP_UINT64 DoubleAsUInt64(double d) {
    CHAOS_IL2CPP_UINT64 u;
    std::memcpy(&u, &d, sizeof(u));
    return u;
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedIsNaN(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    auto* rl = reinterpret_cast<TScalar*>(&result);
    TScalar all_ones; std::memset(&all_ones, 0xFF, sizeof(TScalar));
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i) {
        bool isnan = false;
        if constexpr (std::is_same_v<TScalar, float>) isnan = (FloatAsUInt32(sl[i]) & 0x7F800000u) == 0x7F800000u && (FloatAsUInt32(sl[i]) & 0x007FFFFFu) != 0;
        else if constexpr (std::is_same_v<TScalar, double>) isnan = (DoubleAsUInt64(sl[i]) & 0x7FF0000000000000ULL) == 0x7FF0000000000000ULL && (DoubleAsUInt64(sl[i]) & 0x000FFFFFFFFFFFFFULL) != 0;
        rl[i] = isnan ? all_ones : static_cast<TScalar>(0);
    }
    return result;
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedIsInfinity(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    auto* rl = reinterpret_cast<TScalar*>(&result);
    TScalar all_ones; std::memset(&all_ones, 0xFF, sizeof(TScalar));
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i) {
        bool isinf = false;
        if constexpr (std::is_same_v<TScalar, float>) {
            CHAOS_IL2CPP_UINT32 u = FloatAsUInt32(sl[i]);
            isinf = (u & 0x7FFFFFFFu) == 0x7F800000u;
        } else if constexpr (std::is_same_v<TScalar, double>) {
            CHAOS_IL2CPP_UINT64 u = DoubleAsUInt64(sl[i]);
            isinf = (u & 0x7FFFFFFFFFFFFFFFULL) == 0x7FF0000000000000ULL;
        }
        rl[i] = isinf ? all_ones : static_cast<TScalar>(0);
    }
    return result;
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedIsFinite(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    auto* rl = reinterpret_cast<TScalar*>(&result);
    TScalar all_ones; std::memset(&all_ones, 0xFF, sizeof(TScalar));
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i) {
        bool finite = true;
        if constexpr (std::is_same_v<TScalar, float>) {
            CHAOS_IL2CPP_UINT32 u = FloatAsUInt32(sl[i]);
            finite = (u & 0x7F800000u) != 0x7F800000u;
        } else if constexpr (std::is_same_v<TScalar, double>) {
            CHAOS_IL2CPP_UINT64 u = DoubleAsUInt64(sl[i]);
            finite = (u & 0x7FF0000000000000ULL) != 0x7FF0000000000000ULL;
        }
        rl[i] = finite ? all_ones : static_cast<TScalar>(0);
    }
    return result;
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedIsNegative(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    auto* rl = reinterpret_cast<TScalar*>(&result);
    TScalar all_ones; std::memset(&all_ones, 0xFF, sizeof(TScalar));
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i) {
        bool neg = false;
        if constexpr (std::is_same_v<TScalar, float>) neg = (FloatAsUInt32(sl[i]) & 0x80000000u) != 0;
        else if constexpr (std::is_same_v<TScalar, double>) neg = (DoubleAsUInt64(sl[i]) & 0x8000000000000000ULL) != 0;
        else neg = sl[i] < 0;
        rl[i] = neg ? all_ones : static_cast<TScalar>(0);
    }
    return result;
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedIsPositive(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    auto* rl = reinterpret_cast<TScalar*>(&result);
    TScalar all_ones; std::memset(&all_ones, 0xFF, sizeof(TScalar));
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i) {
        bool pos = false;
        if constexpr (std::is_same_v<TScalar, float>) {
            CHAOS_IL2CPP_UINT32 u = FloatAsUInt32(sl[i]);
            pos = (u & 0x7FFFFFFFu) != 0 && (u & 0x7F800000u) != 0x7F800000u && (u & 0x80000000u) == 0;
        } else if constexpr (std::is_same_v<TScalar, double>) {
            CHAOS_IL2CPP_UINT64 u = DoubleAsUInt64(sl[i]);
            pos = (u & 0x7FFFFFFFFFFFFFFFULL) != 0 && (u & 0x7FF0000000000000ULL) != 0x7FF0000000000000ULL && (u & 0x8000000000000000ULL) == 0;
        } else pos = sl[i] > 0;
        rl[i] = pos ? all_ones : static_cast<TScalar>(0);
    }
    return result;
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedIsNegativeInfinity(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    auto* rl = reinterpret_cast<TScalar*>(&result);
    TScalar all_ones; std::memset(&all_ones, 0xFF, sizeof(TScalar));
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i) {
        bool match = false;
        if constexpr (std::is_same_v<TScalar, float>) match = FloatAsUInt32(sl[i]) == 0xFF800000u;
        else if constexpr (std::is_same_v<TScalar, double>) match = DoubleAsUInt64(sl[i]) == 0xFFF0000000000000ULL;
        rl[i] = match ? all_ones : static_cast<TScalar>(0);
    }
    return result;
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedIsPositiveInfinity(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    auto* rl = reinterpret_cast<TScalar*>(&result);
    TScalar all_ones; std::memset(&all_ones, 0xFF, sizeof(TScalar));
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i) {
        bool match = false;
        if constexpr (std::is_same_v<TScalar, float>) match = FloatAsUInt32(sl[i]) == 0x7F800000u;
        else if constexpr (std::is_same_v<TScalar, double>) match = DoubleAsUInt64(sl[i]) == 0x7FF0000000000000ULL;
        rl[i] = match ? all_ones : static_cast<TScalar>(0);
    }
    return result;
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedIsNormal(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    auto* rl = reinterpret_cast<TScalar*>(&result);
    TScalar all_ones; std::memset(&all_ones, 0xFF, sizeof(TScalar));
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i) {
        bool normal = false;
        if constexpr (std::is_same_v<TScalar, float>) {
            CHAOS_IL2CPP_UINT32 u = FloatAsUInt32(sl[i]);
            CHAOS_IL2CPP_UINT32 exp = (u >> 23) & 0xFFu;
            normal = exp != 0 && exp != 0xFF;
        } else if constexpr (std::is_same_v<TScalar, double>) {
            CHAOS_IL2CPP_UINT64 u = DoubleAsUInt64(sl[i]);
            CHAOS_IL2CPP_UINT64 exp = (u >> 52) & 0x7FFULL;
            normal = exp != 0 && exp != 0x7FF;
        }
        rl[i] = normal ? all_ones : static_cast<TScalar>(0);
    }
    return result;
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedIsSubnormal(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    auto* rl = reinterpret_cast<TScalar*>(&result);
    TScalar all_ones; std::memset(&all_ones, 0xFF, sizeof(TScalar));
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i) {
        bool sub = false;
        if constexpr (std::is_same_v<TScalar, float>) {
            CHAOS_IL2CPP_UINT32 u = FloatAsUInt32(sl[i]);
            sub = (u & 0x7F800000u) == 0 && (u & 0x007FFFFFu) != 0;
        } else if constexpr (std::is_same_v<TScalar, double>) {
            CHAOS_IL2CPP_UINT64 u = DoubleAsUInt64(sl[i]);
            sub = (u & 0x7FF0000000000000ULL) == 0 && (u & 0x000FFFFFFFFFFFFFULL) != 0;
        }
        rl[i] = sub ? all_ones : static_cast<TScalar>(0);
    }
    return result;
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedIsInteger(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    auto* rl = reinterpret_cast<TScalar*>(&result);
    TScalar all_ones; std::memset(&all_ones, 0xFF, sizeof(TScalar));
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i) {
        bool isint = false;
        if constexpr (std::is_floating_point_v<TScalar>) {
            isint = std::trunc(sl[i]) == sl[i] && !std::isinf(sl[i]) && !std::isnan(static_cast<double>(sl[i]));
        } else isint = true;
        rl[i] = isint ? all_ones : static_cast<TScalar>(0);
    }
    return result;
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedIsEvenInteger(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    auto* rl = reinterpret_cast<TScalar*>(&result);
    TScalar all_ones; std::memset(&all_ones, 0xFF, sizeof(TScalar));
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i) {
        bool even = false;
        if constexpr (std::is_floating_point_v<TScalar>) {
            even = std::trunc(sl[i]) == sl[i] && !std::isinf(sl[i]) && !std::isnan(static_cast<double>(sl[i])) && std::fmod(sl[i], static_cast<TScalar>(2)) == static_cast<TScalar>(0);
        } else even = (sl[i] & static_cast<TScalar>(1)) == 0;
        rl[i] = even ? all_ones : static_cast<TScalar>(0);
    }
    return result;
}

template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedIsOddInteger(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    auto* rl = reinterpret_cast<TScalar*>(&result);
    TScalar all_ones; std::memset(&all_ones, 0xFF, sizeof(TScalar));
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i) {
        bool odd = false;
        if constexpr (std::is_floating_point_v<TScalar>) {
            odd = std::trunc(sl[i]) == sl[i] && !std::isinf(sl[i]) && !std::isnan(static_cast<double>(sl[i])) && std::fmod(sl[i], static_cast<TScalar>(2)) != static_cast<TScalar>(0);
        } else odd = (sl[i] & static_cast<TScalar>(1)) != 0;
        rl[i] = odd ? all_ones : static_cast<TScalar>(0);
    }
    return result;
}

// ── FusedMultiplyAdd ──
template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedFusedMultiplyAdd(const TCarrier& a, const TCarrier& b, const TCarrier& c) {
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* al = reinterpret_cast<const TScalar*>(&a);
    const TScalar* bl = reinterpret_cast<const TScalar*>(&b);
    const TScalar* cl = reinterpret_cast<const TScalar*>(&c);
    auto* rl = reinterpret_cast<TScalar*>(&result);
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i) {
        if constexpr (std::is_same_v<TScalar, float>)
            rl[i] = std::fmaf(al[i], bl[i], cl[i]);
        else if constexpr (std::is_same_v<TScalar, double>)
            rl[i] = std::fma(al[i], bl[i], cl[i]);
        else
            rl[i] = al[i] * bl[i] + cl[i];
    }
    return result;
}

// ── Narrow (TScalar → smaller type, TFromCarrier → TToCarrier) ──
template <typename TFromScalar, typename TToScalar, typename TFromCarrier, typename TToCarrier>
inline TToCarrier VectorFixedNarrow(const TFromCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TFromCarrier) / sizeof(TFromScalar);
    TToCarrier result{};
    const TFromScalar* sl = reinterpret_cast<const TFromScalar*>(&value);
    auto* rl = reinterpret_cast<TToScalar*>(&result);
    TToScalar min_val{}, max_val{};
    if constexpr (std::is_signed_v<TToScalar>) {
        min_val = static_cast<TToScalar>(static_cast<TToScalar>(1) << (sizeof(TToScalar) * 8 - 1));
        max_val = static_cast<TToScalar>(~(static_cast<TToScalar>(1) << (sizeof(TToScalar) * 8 - 1)));
    } else {
        max_val = static_cast<TToScalar>(~static_cast<TToScalar>(0));
    }
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i) {
        // Saturating narrow
        if (sl[i] > static_cast<TFromScalar>(max_val))
            rl[i] = max_val;
        else if (sl[i] < static_cast<TFromScalar>(min_val))
            rl[i] = min_val;
        else
            rl[i] = static_cast<TToScalar>(sl[i]);
    }
    return result;
}

// ── Widen (TScalar → larger type, lower/upper halves) ──
template <typename TFromScalar, typename TToScalar, typename TFromCarrier, typename TToCarrier>
inline TToCarrier VectorFixedWidenLower(const TFromCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TToCarrier) / sizeof(TToScalar);
    TToCarrier result{};
    const TFromScalar* sl = reinterpret_cast<const TFromScalar*>(&value);
    auto* rl = reinterpret_cast<TToScalar*>(&result);
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i)
        rl[i] = static_cast<TToScalar>(sl[i]);
    return result;
}

template <typename TFromScalar, typename TToScalar, typename TFromCarrier, typename TToCarrier>
inline TToCarrier VectorFixedWidenUpper(const TFromCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE half = sizeof(TFromCarrier) / sizeof(TFromScalar) / 2;
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TToCarrier) / sizeof(TToScalar);
    TToCarrier result{};
    const TFromScalar* sl = reinterpret_cast<const TFromScalar*>(&value);
    auto* rl = reinterpret_cast<TToScalar*>(&result);
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i)
        rl[i] = static_cast<TToScalar>(sl[half + i]);
    return result;
}

// ── WithElement ──
template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedWithElement(const TCarrier& value, CHAOS_IL2CPP_INT32 index, TScalar newValue) {
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    auto* rl = reinterpret_cast<TScalar*>(&result);
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i)
        rl[i] = sl[i];
    if (index >= 0 && static_cast<CHAOS_IL2CPP_SIZE>(index) < N)
        rl[index] = newValue;
    return result;
}

// ── WithLower / WithUpper ──
template <typename TCarrier>
inline TCarrier VectorFixedWithLower(const TCarrier& upper, const RuntimeIntrinsicVector128Carrier& lower) {
    TCarrier result = upper;
    std::memcpy(reinterpret_cast<CHAOS_IL2CPP_UINT8*>(&result), lower.bytes, 16);
    return result;
}

template <typename TCarrier>
inline TCarrier VectorFixedWithUpper(const TCarrier& lower, const RuntimeIntrinsicVector128Carrier& upper) {
    TCarrier result = lower;
    std::memcpy(reinterpret_cast<CHAOS_IL2CPP_UINT8*>(&result) + 16, upper.bytes, 16);
    return result;
}

// ── LoadUnsafe (from pointer) ──
template <typename TCarrier>
inline TCarrier VectorFixedLoadUnsafe(const CHAOS_IL2CPP_INTPTR source) {
    TCarrier result{};
    std::memcpy(&result, reinterpret_cast<const void*>(source), sizeof(TCarrier));
    return result;
}

// ── Create (from element array) ──
template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedCreateFromArray(const TScalar* values, CHAOS_IL2CPP_INT32 count) {
    TCarrier result{};
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TCarrier) / sizeof(TScalar);
    auto* rl = reinterpret_cast<TScalar*>(&result);
    CHAOS_IL2CPP_SIZE i = 0;
    for (; i < static_cast<CHAOS_IL2CPP_SIZE>(count) && i < N; ++i)
        rl[i] = values[i];
    for (; i < N; ++i)
        rl[i] = static_cast<TScalar>(0);
    return result;
}

// ── Transcendental ──
template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedCos(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    auto* rl = reinterpret_cast<TScalar*>(&result);
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i) {
        if constexpr (std::is_same_v<TScalar, float>) rl[i] = std::cos(sl[i]);
        else if constexpr (std::is_same_v<TScalar, double>) rl[i] = std::cos(sl[i]);
        else rl[i] = static_cast<TScalar>(0);
    }
    return result;
}
template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedSin(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    auto* rl = reinterpret_cast<TScalar*>(&result);
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i) {
        if constexpr (std::is_same_v<TScalar, float>) rl[i] = std::sin(sl[i]);
        else if constexpr (std::is_same_v<TScalar, double>) rl[i] = std::sin(sl[i]);
        else rl[i] = static_cast<TScalar>(0);
    }
    return result;
}
template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedExp(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    auto* rl = reinterpret_cast<TScalar*>(&result);
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i) {
        if constexpr (std::is_same_v<TScalar, float>) rl[i] = std::exp(sl[i]);
        else if constexpr (std::is_same_v<TScalar, double>) rl[i] = std::exp(sl[i]);
        else rl[i] = static_cast<TScalar>(0);
    }
    return result;
}
template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedRound(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    auto* rl = reinterpret_cast<TScalar*>(&result);
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i) {
        if constexpr (std::is_floating_point_v<TScalar>) rl[i] = std::round(sl[i]);
        else rl[i] = sl[i];
    }
    return result;
}

// ── StoreUnsafe (write carrier to memory) ──
template <typename TCarrier>
inline void VectorFixedStoreUnsafe(CHAOS_IL2CPP_INTPTR dest, const TCarrier& value) {
    std::memcpy(reinterpret_cast<void*>(dest), &value, sizeof(TCarrier));
}

// ── Additional math ──
template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedHypot(const TCarrier& a, const TCarrier& b) {
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* al = reinterpret_cast<const TScalar*>(&a);
    const TScalar* bl = reinterpret_cast<const TScalar*>(&b);
    auto* rl = reinterpret_cast<TScalar*>(&result);
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i) {
        if constexpr (std::is_same_v<TScalar, float>) rl[i] = std::hypotf(al[i], bl[i]);
        else if constexpr (std::is_same_v<TScalar, double>) rl[i] = std::hypot(al[i], bl[i]);
        else rl[i] = static_cast<TScalar>(0);
    }
    return result;
}
template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedDegreesToRadians(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    auto* rl = reinterpret_cast<TScalar*>(&result);
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i) {
        if constexpr (std::is_floating_point_v<TScalar>) {
            constexpr TScalar pi = TScalar(3.14159265358979323846);
            rl[i] = sl[i] * pi / TScalar(180);
        } else rl[i] = static_cast<TScalar>(0);
    }
    return result;
}
template <typename TScalar, typename TCarrier>
inline TCarrier VectorFixedRadiansToDegrees(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TCarrier) / sizeof(TScalar);
    TCarrier result{};
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    auto* rl = reinterpret_cast<TScalar*>(&result);
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i) {
        if constexpr (std::is_floating_point_v<TScalar>) {
            constexpr TScalar pi = TScalar(3.14159265358979323846);
            rl[i] = sl[i] * TScalar(180) / pi;
        } else rl[i] = static_cast<TScalar>(0);
    }
    return result;
}

// ── NarrowWithSaturation ──
template <typename TFromScalar, typename TToScalar, typename TFromCarrier, typename TToCarrier>
inline TToCarrier VectorFixedNarrowWithSaturation(const TFromCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TFromCarrier) / sizeof(TFromScalar);
    TToCarrier result{};
    const TFromScalar* sl = reinterpret_cast<const TFromScalar*>(&value);
    auto* rl = reinterpret_cast<TToScalar*>(&result);
    TToScalar min_val{}, max_val{};
    if constexpr (std::is_signed_v<TToScalar>) {
        min_val = static_cast<TToScalar>(static_cast<TToScalar>(1) << (sizeof(TToScalar) * 8 - 1));
        max_val = static_cast<TToScalar>(~(static_cast<TToScalar>(1) << (sizeof(TToScalar) * 8 - 1)));
    } else {
        max_val = static_cast<TToScalar>(~static_cast<TToScalar>(0));
    }
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i) {
        if (sl[i] > static_cast<TFromScalar>(max_val)) rl[i] = max_val;
        else if (sl[i] < static_cast<TFromScalar>(min_val)) rl[i] = min_val;
        else rl[i] = static_cast<TToScalar>(sl[i]);
    }
    return result;
}

// ── All/Any scalar comparison ──
template <typename TScalar, typename TCarrier>
inline bool VectorFixedAllEqual(const TCarrier& value, TScalar scalar) {
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TCarrier) / sizeof(TScalar);
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i)
        if (sl[i] != scalar) return false;
    return true;
}
template <typename TScalar, typename TCarrier>
inline bool VectorFixedAnyEqual(const TCarrier& value, TScalar scalar) {
    constexpr CHAOS_IL2CPP_SIZE N = sizeof(TCarrier) / sizeof(TScalar);
    const TScalar* sl = reinterpret_cast<const TScalar*>(&value);
    for (CHAOS_IL2CPP_SIZE i = 0; i < N; ++i)
        if (sl[i] == scalar) return true;
    return false;
}

}  // namespace chaos::il2cpp::vector_fixed




// ���� VectorFixedAnyLaneNonZero ����
template <typename TCarrier>
inline bool VectorFixedAnyLaneNonZero(const TCarrier& value) {
    CHAOS_IL2CPP_UINT8 accum = 0;
    for (CHAOS_IL2CPP_SIZE i = 0; i < sizeof(TCarrier); ++i)
        accum |= reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(&value)[i];
    return accum != 0;
}

// ���� VectorFixedAllLanesNonZero ����
template <typename TScalar, typename TCarrier>
inline bool VectorFixedAllLanesNonZero(const TCarrier& value) {
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(TScalar);
    for (CHAOS_IL2CPP_SIZE i = 0; i < lane_count; ++i) {
        CHAOS_IL2CPP_UINT8 lane_accum = 0;
        for (CHAOS_IL2CPP_SIZE j = 0; j < sizeof(TScalar); ++j)
            lane_accum |= reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(&value)[i * sizeof(TScalar) + j];
        if (lane_accum == 0) return false;
    }
    return true;
}

#endif  // CHAOS_IL2CPP_VECTOR_FIXED_TEMPLATES_H_
