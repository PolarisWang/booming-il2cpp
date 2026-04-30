#include <chaos/common.h>
#include "runtime_core.h"

namespace
{
static_assert(sizeof(CHAOS_IL2CPP_INTPTR) == sizeof(CHAOS_IL2CPP_UINT64), "native-aot ABI lowering requires 64-bit intptr_t");

constexpr CHAOS_IL2CPP_INTPTR chaos_runtime_managed_pointer_local_slot_tag =
    ChaosIl2cpp::Common::k_managed_pointer_local_slot_tag;
constexpr CHAOS_IL2CPP_INTPTR chaos_raw_int32_pointer_tag = 2;

struct chaos_managed_exception
{
    CHAOS_IL2CPP_INTPTR object_value = 0;
};

template <typename TAction>
struct chaos_finally_scope_guard
{
    explicit chaos_finally_scope_guard(TAction init_action)
        : action(init_action)
    {
    }

    chaos_finally_scope_guard(const chaos_finally_scope_guard&) = delete;
    chaos_finally_scope_guard& operator=(const chaos_finally_scope_guard&) = delete;

    ~chaos_finally_scope_guard()
    {
        if (active)
        {
            action();
        }
    }

    TAction action;
    bool active = true;
};

template <typename TAction>
chaos_finally_scope_guard<TAction> chaos_make_finally_scope_guard(TAction action)
{
    return chaos_finally_scope_guard<TAction>(action);
}

CHAOS_IL2CPP_INTPTR chaos_store_float32(float value) noexcept
{
    CHAOS_IL2CPP_UINT32 bits = 0;
    CHAOS_IL2CPP_MEMCPY(&bits, &value, sizeof(value));
    return static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT64>(bits));
}

float chaos_load_float32(CHAOS_IL2CPP_INTPTR value) noexcept
{
    const auto bits = static_cast<CHAOS_IL2CPP_UINT32>(static_cast<CHAOS_IL2CPP_UINT64>(value));
    float result = 0.0f;
    CHAOS_IL2CPP_MEMCPY(&result, &bits, sizeof(result));
    return result;
}

CHAOS_IL2CPP_INTPTR chaos_store_float64(double value) noexcept
{
    CHAOS_IL2CPP_UINT64 bits = 0;
    CHAOS_IL2CPP_MEMCPY(&bits, &value, sizeof(value));
    CHAOS_IL2CPP_INTPTR result = 0;
    CHAOS_IL2CPP_MEMCPY(&result, &bits, sizeof(result));
    return result;
}

double chaos_load_float64(CHAOS_IL2CPP_INTPTR value) noexcept
{
    CHAOS_IL2CPP_UINT64 bits = 0;
    CHAOS_IL2CPP_MEMCPY(&bits, &value, sizeof(bits));
    double result = 0.0;
    CHAOS_IL2CPP_MEMCPY(&result, &bits, sizeof(result));
    return result;
}

CHAOS_IL2CPP_INTPTR chaos_store_int64(CHAOS_IL2CPP_INT64 value) noexcept
{
    CHAOS_IL2CPP_INTPTR result = 0;
    CHAOS_IL2CPP_MEMCPY(&result, &value, sizeof(result));
    return result;
}

CHAOS_IL2CPP_INT64 chaos_load_int64(CHAOS_IL2CPP_INTPTR value) noexcept
{
    CHAOS_IL2CPP_INT64 result = 0;
    CHAOS_IL2CPP_MEMCPY(&result, &value, sizeof(result));
    return result;
}

CHAOS_IL2CPP_INTPTR chaos_store_uint64(CHAOS_IL2CPP_UINT64 value) noexcept
{
    CHAOS_IL2CPP_INTPTR result = 0;
    CHAOS_IL2CPP_MEMCPY(&result, &value, sizeof(result));
    return result;
}

CHAOS_IL2CPP_UINT64 chaos_load_uint64(CHAOS_IL2CPP_INTPTR value) noexcept
{
    CHAOS_IL2CPP_UINT64 result = 0;
    CHAOS_IL2CPP_MEMCPY(&result, &value, sizeof(result));
    return result;
}

CHAOS_IL2CPP_INT32 chaos_wrap_add(CHAOS_IL2CPP_INT32 left, CHAOS_IL2CPP_INT32 right) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(
        static_cast<CHAOS_IL2CPP_UINT32>(left) + static_cast<CHAOS_IL2CPP_UINT32>(right));
}

CHAOS_IL2CPP_INT32 chaos_wrap_sub(CHAOS_IL2CPP_INT32 left, CHAOS_IL2CPP_INT32 right) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(
        static_cast<CHAOS_IL2CPP_UINT32>(left) - static_cast<CHAOS_IL2CPP_UINT32>(right));
}

CHAOS_IL2CPP_INT32 chaos_wrap_mul(CHAOS_IL2CPP_INT32 left, CHAOS_IL2CPP_INT32 right) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(
        static_cast<CHAOS_IL2CPP_UINT32>(left) * static_cast<CHAOS_IL2CPP_UINT32>(right));
}

CHAOS_IL2CPP_INT32 chaos_div(CHAOS_IL2CPP_INT32 left, CHAOS_IL2CPP_INT32 right)
{
    if (right == 0)
    {
        CHAOS_IL2CPP_ABORT();
    }

    if (left == CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(CHAOS_IL2CPP_INT32) && right == -1)
    {
        CHAOS_IL2CPP_ABORT();
    }

    return static_cast<CHAOS_IL2CPP_INT32>(left / right);
}

CHAOS_IL2CPP_INT32 chaos_rem(CHAOS_IL2CPP_INT32 left, CHAOS_IL2CPP_INT32 right)
{
    if (right == 0)
    {
        CHAOS_IL2CPP_ABORT();
    }

    if (left == CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(CHAOS_IL2CPP_INT32) && right == -1)
    {
        return 0;
    }

    return static_cast<CHAOS_IL2CPP_INT32>(left % right);
}

CHAOS_IL2CPP_INT32 chaos_shift_left_int32(CHAOS_IL2CPP_INT32 value, CHAOS_IL2CPP_INT32 amount) noexcept
{
    const auto shift = static_cast<CHAOS_IL2CPP_UINT32>(amount) & 31U;
    return static_cast<CHAOS_IL2CPP_INT32>(static_cast<CHAOS_IL2CPP_UINT32>(value) << shift);
}

CHAOS_IL2CPP_INT32 chaos_shift_right_int32(CHAOS_IL2CPP_INT32 value, CHAOS_IL2CPP_INT32 amount) noexcept
{
    const auto shift = static_cast<CHAOS_IL2CPP_UINT32>(amount) & 31U;
    if (shift == 0U)
    {
        return value;
    }

    const auto bits = static_cast<CHAOS_IL2CPP_UINT32>(value);
    if (value >= 0)
    {
        return static_cast<CHAOS_IL2CPP_INT32>(bits >> shift);
    }

    const auto fill = CHAOS_IL2CPP_NUMERIC_LIMITS_MAX(CHAOS_IL2CPP_UINT32) << (32U - shift);
    return static_cast<CHAOS_IL2CPP_INT32>((bits >> shift) | fill);
}

CHAOS_IL2CPP_INT32 chaos_shift_right_un_int32(CHAOS_IL2CPP_INT32 value, CHAOS_IL2CPP_INT32 amount) noexcept
{
    const auto shift = static_cast<CHAOS_IL2CPP_UINT32>(amount) & 31U;
    return static_cast<CHAOS_IL2CPP_INT32>(static_cast<CHAOS_IL2CPP_UINT32>(value) >> shift);
}

CHAOS_IL2CPP_INTPTR chaos_checked_conv_ovf_i1(CHAOS_IL2CPP_INTPTR value)
{
    if (value < static_cast<CHAOS_IL2CPP_INTPTR>(CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(CHAOS_IL2CPP_INT8)) ||
        value > static_cast<CHAOS_IL2CPP_INTPTR>(CHAOS_IL2CPP_NUMERIC_LIMITS_MAX(CHAOS_IL2CPP_INT8)))
    {
        CHAOS_IL2CPP_ABORT();
    }

    return static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT8>(value));
}

CHAOS_IL2CPP_INTPTR chaos_checked_conv_ovf_u1(CHAOS_IL2CPP_INTPTR value)
{
    if (value < static_cast<CHAOS_IL2CPP_INTPTR>(0) ||
        value > static_cast<CHAOS_IL2CPP_INTPTR>(CHAOS_IL2CPP_NUMERIC_LIMITS_MAX(CHAOS_IL2CPP_UINT8)))
    {
        CHAOS_IL2CPP_ABORT();
    }

    return static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT8>(value));
}

CHAOS_IL2CPP_INTPTR* chaos_resolve_native_int_slot(CHAOS_IL2CPP_INTPTR chaos_address)
{
    if ((chaos_address & chaos_runtime_managed_pointer_local_slot_tag) != 0)
    {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(
            static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_runtime_managed_pointer_local_slot_tag));
    }

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address);
}

struct chaos_object_header
{
    CHAOS_IL2CPP_INTPTR type_id = 0;
};

constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_managed_array = 1;

struct chaos_managed_array
{
    chaos_object_header header{};
    // compatibility marker: std::CHAOS_IL2CPP_UINT8 element_type_shape = 0;
    CHAOS_IL2CPP_UINT8 element_type_shape = 0;
    CHAOS_IL2CPP_INTPTR element_type_id = 0;
    CHAOS_IL2CPP_INTPTR length = 0;
    CHAOS_IL2CPP_INTPTR* elements = nullptr;
};

constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_reference = 1;
constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_value = 2;
constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_interface = 3;

// compatibility marker: constexpr CHAOS_IL2CPP_INTPTR chaos_managed_pointer_local_slot_tag = 1;
constexpr CHAOS_IL2CPP_INTPTR chaos_managed_pointer_local_slot_tag = ChaosIl2cpp::Common::k_managed_pointer_local_slot_tag;
constexpr CHAOS_IL2CPP_INTPTR chaos_string_id_tag = static_cast<CHAOS_IL2CPP_INTPTR>(1) << (sizeof(CHAOS_IL2CPP_INTPTR) * 8 - 1);
static_assert(sizeof(CHAOS_IL2CPP_INTPTR) == sizeof(CHAOS_IL2CPP_UINT64), "string id tagging requires 64-bit native int");

inline bool chaos_is_string_id(CHAOS_IL2CPP_INTPTR chaos_value) noexcept
{
    return (chaos_value & chaos_string_id_tag) != 0;
}

inline CHAOS_IL2CPP_UINT64 chaos_extract_string_id(CHAOS_IL2CPP_INTPTR chaos_value) noexcept
{
    return static_cast<CHAOS_IL2CPP_UINT64>(chaos_value & ~chaos_string_id_tag);
}

inline CHAOS_IL2CPP_INTPTR chaos_make_string_id_value(CHAOS_IL2CPP_UINT64 chaos_id) noexcept
{
    return chaos_string_id_tag | static_cast<CHAOS_IL2CPP_INTPTR>(chaos_id);
}

CHAOS_IL2CPP_INTPTR chaos_normalize_native_int_argument(CHAOS_IL2CPP_INTPTR chaos_value) noexcept
{
    if ((chaos_value & chaos_managed_pointer_local_slot_tag) == 0)
    {
        return chaos_value;
    }

    auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_value & ~chaos_managed_pointer_local_slot_tag));
    return *chaos_slot;
}

template <typename TValue>
TValue* chaos_resolve_managed_value_pointer(CHAOS_IL2CPP_INTPTR chaos_managed_pointer)
{
    if ((chaos_managed_pointer & chaos_managed_pointer_local_slot_tag) != 0)
    {
        auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_managed_pointer & ~chaos_managed_pointer_local_slot_tag));
        if (*chaos_slot == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            *chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(new TValue{});
        }
        return reinterpret_cast<TValue*>(*chaos_slot);
    }

    return reinterpret_cast<TValue*>(chaos_managed_pointer);
}

constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = 2;

CHAOS_IL2CPP_INTPTR chaos_get_base_type_id(CHAOS_IL2CPP_INTPTR chaos_type_id) noexcept
{
    switch (chaos_type_id)
    {
        default:
            return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }
}

bool chaos_is_type_compatible(CHAOS_IL2CPP_INTPTR chaos_actual_type_id, CHAOS_IL2CPP_INTPTR chaos_target_type_id) noexcept
{
    auto chaos_current_type_id = chaos_actual_type_id;
    while (chaos_current_type_id != static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        if (chaos_current_type_id == chaos_target_type_id)
        {
            return true;
        }

        chaos_current_type_id = chaos_get_base_type_id(chaos_current_type_id);
    }

    return false;
}

bool chaos_type_implements_interface(CHAOS_IL2CPP_INTPTR chaos_actual_type_id, CHAOS_IL2CPP_INTPTR chaos_target_interface_type_id) noexcept
{
    switch (chaos_actual_type_id)
    {
        case chaos_type_id_System_Private_CoreLib_System_String:
            return false;
        default:
            return false;
    }
}

bool chaos_does_type_implement_interface(CHAOS_IL2CPP_INTPTR chaos_actual_type_id, CHAOS_IL2CPP_INTPTR chaos_target_interface_type_id) noexcept
{
    auto chaos_current_type_id = chaos_actual_type_id;
    while (chaos_current_type_id != static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        if (chaos_type_implements_interface(chaos_current_type_id, chaos_target_interface_type_id))
        {
            return true;
        }

        chaos_current_type_id = chaos_get_base_type_id(chaos_current_type_id);
    }

    return false;
}

bool chaos_is_array_type_compatible(
    CHAOS_IL2CPP_UINT8 chaos_actual_element_shape,
    CHAOS_IL2CPP_INTPTR chaos_actual_element_type_id,
    CHAOS_IL2CPP_UINT8 chaos_target_element_shape,
    CHAOS_IL2CPP_INTPTR chaos_target_element_type_id) noexcept
{
    if (chaos_actual_element_shape == chaos_type_shape_reference)
    {
        if (chaos_target_element_shape == chaos_type_shape_reference)
        {
            return chaos_is_type_compatible(chaos_actual_element_type_id, chaos_target_element_type_id);
        }

        if (chaos_target_element_shape == chaos_type_shape_interface)
        {
            return chaos_does_type_implement_interface(chaos_actual_element_type_id, chaos_target_element_type_id);
        }

        return false;
    }

    return chaos_actual_element_shape == chaos_target_element_shape
        && chaos_actual_element_type_id == chaos_target_element_type_id;
}

bool chaos_is_array_store_compatible(const chaos_managed_array* chaos_array, CHAOS_IL2CPP_INTPTR chaos_value) noexcept
{
    if (chaos_array == nullptr)
    {
        return false;
    }

    if (chaos_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        return true;
    }

    // StringId fast path: materialized value carries a heap pointer;
    // this line is reached only if caller skipped materialization.
    if (chaos_is_string_id(chaos_value))
    {
        return chaos_array->element_type_shape == chaos_type_shape_reference            && chaos_is_type_compatible(chaos_type_id_System_Private_CoreLib_System_String, chaos_array->element_type_id);
    }

    auto* chaos_header = reinterpret_cast<chaos_object_header*>(chaos_value);
    if (chaos_array->element_type_shape == chaos_type_shape_interface)
    {
        return chaos_does_type_implement_interface(chaos_header->type_id, chaos_array->element_type_id);
    }

    if (chaos_array->element_type_shape == chaos_type_shape_reference)
    {
        return chaos_is_type_compatible(chaos_header->type_id, chaos_array->element_type_id);
    }

    return false;
}

struct chaos_type_System_Private_CoreLib_System_String
{
    chaos_object_header header{};
    CHAOS_IL2CPP_INTPTR length = 0;
    const char* utf8_data = nullptr;
    CHAOS_IL2CPP_UINT64 string_id = 0u;  // stable StringId, 0 = not yet materialized
};

extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method14(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method15(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method16(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method17(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method18(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method19(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method20(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method21(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method22(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method23(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method24(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method25(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method26(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method27(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method28(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method29(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method30(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method31(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method32(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method33(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method34(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method35(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method36(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method37(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method38(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method39(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method40(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method41(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method42(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);
}

extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method14(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method15(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method16(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method17(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method18(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method19(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method20(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method21(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method22(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method23(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method24(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method25(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method26(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method27(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method28(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method29(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method30(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method31(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method32(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method33(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method34(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method35(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method36(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method37(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method38(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method39(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method40(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method41(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method42(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method0()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method0(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177280);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method1()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method1(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177279);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method2()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method2(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177278);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method3()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method3(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177277);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method4()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method4(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177276);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method5()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method5(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177275);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method6()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method6(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177274);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method7()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method7(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177273);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method8()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method8(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177272);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method9()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method9(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177271);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method10()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method10(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177270);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method11()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method11(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177269);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method12()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method12(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177268);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method13()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method13(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177267);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method14()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method14(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177266);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method15()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method15(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177265);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method16()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method16(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177264);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method17()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method17(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177263);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method18()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method18(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177262);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method19()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method19(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177261);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method20()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method20(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177260);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method21()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method21(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177259);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method22()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method22(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177258);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method23()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method23(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177257);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method24()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method24(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177256);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method25()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method25(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177255);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method26()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method26(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177254);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method27()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method27(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177253);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method28()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method28(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177252);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method29()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method29(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177251);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method30()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method30(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177250);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method31()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method31(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177249);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method32()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method32(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177248);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method33()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method33(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177247);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method34()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method34(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177246);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method35()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method35(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177245);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method36()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method36(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177244);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method37()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method37(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177243);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method38()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method38(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177242);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method39()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method39(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177241);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method40()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method40(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177240);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method41()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method41(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177239);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Method42()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method42(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177238);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 141) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;
    chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_arg_0);

    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = chaos_args[0];
    goto chaos_ip_2;

chaos_ip_2:
    chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_3;

chaos_ip_3:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];
    goto chaos_ip_4;

chaos_ip_4:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_5;

chaos_ip_5:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_switch_value = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
        switch (chaos_switch_value)
        {
            case 0:
                goto chaos_ip_188;
            case 1:
                goto chaos_ip_199;
            case 2:
                goto chaos_ip_210;
            case 3:
                goto chaos_ip_221;
            case 4:
                goto chaos_ip_232;
            case 5:
                goto chaos_ip_243;
            case 6:
                goto chaos_ip_254;
            case 7:
                goto chaos_ip_265;
            case 8:
                goto chaos_ip_276;
            case 9:
                goto chaos_ip_287;
            case 10:
                goto chaos_ip_298;
            case 11:
                goto chaos_ip_309;
            case 12:
                goto chaos_ip_320;
            case 13:
                goto chaos_ip_331;
            case 14:
                goto chaos_ip_342;
            case 15:
                goto chaos_ip_353;
            case 16:
                goto chaos_ip_364;
            case 17:
                goto chaos_ip_375;
            case 18:
                goto chaos_ip_386;
            case 19:
                goto chaos_ip_397;
            case 20:
                goto chaos_ip_408;
            case 21:
                goto chaos_ip_419;
            case 22:
                goto chaos_ip_430;
            case 23:
                goto chaos_ip_441;
            case 24:
                goto chaos_ip_452;
            case 25:
                goto chaos_ip_463;
            case 26:
                goto chaos_ip_474;
            case 27:
                goto chaos_ip_485;
            case 28:
                goto chaos_ip_493;
            case 29:
                goto chaos_ip_501;
            case 30:
                goto chaos_ip_509;
            case 31:
                goto chaos_ip_517;
            case 32:
                goto chaos_ip_525;
            case 33:
                goto chaos_ip_533;
            case 34:
                goto chaos_ip_541;
            case 35:
                goto chaos_ip_549;
            case 36:
                goto chaos_ip_557;
            case 37:
                goto chaos_ip_565;
            case 38:
                goto chaos_ip_573;
            case 39:
                goto chaos_ip_581;
            case 40:
                goto chaos_ip_589;
            case 41:
                goto chaos_ip_597;
            case 42:
                goto chaos_ip_605;
            default:
                goto chaos_ip_183;
        }
    }

chaos_ip_183:
    goto chaos_ip_613;

chaos_ip_188:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method0();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_193;

chaos_ip_193:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_194;

chaos_ip_194:
    goto chaos_ip_617;

chaos_ip_199:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method1();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_204;

chaos_ip_204:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_205;

chaos_ip_205:
    goto chaos_ip_617;

chaos_ip_210:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method2();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_215;

chaos_ip_215:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_216;

chaos_ip_216:
    goto chaos_ip_617;

chaos_ip_221:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method3();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_226;

chaos_ip_226:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_227;

chaos_ip_227:
    goto chaos_ip_617;

chaos_ip_232:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method4();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_237;

chaos_ip_237:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_238;

chaos_ip_238:
    goto chaos_ip_617;

chaos_ip_243:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method5();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_248;

chaos_ip_248:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_249;

chaos_ip_249:
    goto chaos_ip_617;

chaos_ip_254:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method6();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_259;

chaos_ip_259:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_260;

chaos_ip_260:
    goto chaos_ip_617;

chaos_ip_265:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method7();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_270;

chaos_ip_270:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_271;

chaos_ip_271:
    goto chaos_ip_617;

chaos_ip_276:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method8();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_281;

chaos_ip_281:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_282;

chaos_ip_282:
    goto chaos_ip_617;

chaos_ip_287:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method9();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_292;

chaos_ip_292:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_293;

chaos_ip_293:
    goto chaos_ip_617;

chaos_ip_298:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method10();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_303;

chaos_ip_303:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_304;

chaos_ip_304:
    goto chaos_ip_617;

chaos_ip_309:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method11();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_314;

chaos_ip_314:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_315;

chaos_ip_315:
    goto chaos_ip_617;

chaos_ip_320:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method12();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_325;

chaos_ip_325:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_326;

chaos_ip_326:
    goto chaos_ip_617;

chaos_ip_331:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method13();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_336;

chaos_ip_336:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_337;

chaos_ip_337:
    goto chaos_ip_617;

chaos_ip_342:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method14();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_347;

chaos_ip_347:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_348;

chaos_ip_348:
    goto chaos_ip_617;

chaos_ip_353:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method15();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_358;

chaos_ip_358:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_359;

chaos_ip_359:
    goto chaos_ip_617;

chaos_ip_364:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method16();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_369;

chaos_ip_369:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_370;

chaos_ip_370:
    goto chaos_ip_617;

chaos_ip_375:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method17();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_380;

chaos_ip_380:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_381;

chaos_ip_381:
    goto chaos_ip_617;

chaos_ip_386:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method18();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_391;

chaos_ip_391:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_392;

chaos_ip_392:
    goto chaos_ip_617;

chaos_ip_397:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method19();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_402;

chaos_ip_402:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_403;

chaos_ip_403:
    goto chaos_ip_617;

chaos_ip_408:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method20();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_413;

chaos_ip_413:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_414;

chaos_ip_414:
    goto chaos_ip_617;

chaos_ip_419:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method21();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_424;

chaos_ip_424:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_425;

chaos_ip_425:
    goto chaos_ip_617;

chaos_ip_430:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method22();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_435;

chaos_ip_435:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_436;

chaos_ip_436:
    goto chaos_ip_617;

chaos_ip_441:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method23();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_446;

chaos_ip_446:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_447;

chaos_ip_447:
    goto chaos_ip_617;

chaos_ip_452:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method24();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_457;

chaos_ip_457:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_458;

chaos_ip_458:
    goto chaos_ip_617;

chaos_ip_463:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method25();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_468;

chaos_ip_468:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_469;

chaos_ip_469:
    goto chaos_ip_617;

chaos_ip_474:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method26();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_479;

chaos_ip_479:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_480;

chaos_ip_480:
    goto chaos_ip_617;

chaos_ip_485:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method27();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_490;

chaos_ip_490:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_491;

chaos_ip_491:
    goto chaos_ip_617;

chaos_ip_493:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method28();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_498;

chaos_ip_498:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_499;

chaos_ip_499:
    goto chaos_ip_617;

chaos_ip_501:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method29();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_506;

chaos_ip_506:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_507;

chaos_ip_507:
    goto chaos_ip_617;

chaos_ip_509:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method30();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_514;

chaos_ip_514:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_515;

chaos_ip_515:
    goto chaos_ip_617;

chaos_ip_517:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method31();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_522;

chaos_ip_522:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_523;

chaos_ip_523:
    goto chaos_ip_617;

chaos_ip_525:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method32();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_530;

chaos_ip_530:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_531;

chaos_ip_531:
    goto chaos_ip_617;

chaos_ip_533:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method33();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_538;

chaos_ip_538:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_539;

chaos_ip_539:
    goto chaos_ip_617;

chaos_ip_541:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method34();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_546;

chaos_ip_546:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_547;

chaos_ip_547:
    goto chaos_ip_617;

chaos_ip_549:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method35();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_554;

chaos_ip_554:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_555;

chaos_ip_555:
    goto chaos_ip_617;

chaos_ip_557:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method36();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_562;

chaos_ip_562:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_563;

chaos_ip_563:
    goto chaos_ip_617;

chaos_ip_565:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method37();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_570;

chaos_ip_570:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_571;

chaos_ip_571:
    goto chaos_ip_617;

chaos_ip_573:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method38();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_578;

chaos_ip_578:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_579;

chaos_ip_579:
    goto chaos_ip_617;

chaos_ip_581:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method39();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_586;

chaos_ip_586:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_587;

chaos_ip_587:
    goto chaos_ip_617;

chaos_ip_589:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method40();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_594;

chaos_ip_594:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_595;

chaos_ip_595:
    goto chaos_ip_617;

chaos_ip_597:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method41();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_602;

chaos_ip_602:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_603;

chaos_ip_603:
    goto chaos_ip_617;

chaos_ip_605:
    {
        const auto chaos_result = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method42();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_610;

chaos_ip_610:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_611;

chaos_ip_611:
    goto chaos_ip_617;

chaos_ip_613:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
    goto chaos_ip_614;

chaos_ip_614:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_615;

chaos_ip_615:
    goto chaos_ip_617;

chaos_ip_617:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[2];
    goto chaos_ip_618;

chaos_ip_618:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Native AOT entry for ReflectionTypePatchEntry/ReflectionTypePatchEntry::Run:System.Int32(System.Int32)
// Managed symbol: ReflectionTypePatchEntry_ReflectionTypePatchEntry_Run
extern "C" int RunNativeAot_reflection_type_patch(CHAOS_IL2CPP_INT32 chaos_entry_index)
{
    return ReflectionTypePatchEntry_ReflectionTypePatchEntry_Run(chaos_entry_index);
}