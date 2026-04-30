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

extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method14(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method15(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method16(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method17(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method18(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method19(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method20(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method21(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);
}

extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method14(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method15(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method16(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method17(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method18(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method19(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method20(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method21(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);

// Managed method: ReflectionMemberCompletePatchEntry/ReflectionMemberCompletePatchEntry::Method0()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method0(void)
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

// Managed method: ReflectionMemberCompletePatchEntry/ReflectionMemberCompletePatchEntry::Method1()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method1(void)
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

// Managed method: ReflectionMemberCompletePatchEntry/ReflectionMemberCompletePatchEntry::Method2()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method2(void)
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

// Managed method: ReflectionMemberCompletePatchEntry/ReflectionMemberCompletePatchEntry::Method3()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method3(void)
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

// Managed method: ReflectionMemberCompletePatchEntry/ReflectionMemberCompletePatchEntry::Method4()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method4(void)
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

// Managed method: ReflectionMemberCompletePatchEntry/ReflectionMemberCompletePatchEntry::Method5()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method5(void)
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

// Managed method: ReflectionMemberCompletePatchEntry/ReflectionMemberCompletePatchEntry::Method6()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method6(void)
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

// Managed method: ReflectionMemberCompletePatchEntry/ReflectionMemberCompletePatchEntry::Method7()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method7(void)
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

// Managed method: ReflectionMemberCompletePatchEntry/ReflectionMemberCompletePatchEntry::Method8()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method8(void)
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

// Managed method: ReflectionMemberCompletePatchEntry/ReflectionMemberCompletePatchEntry::Method9()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method9(void)
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

// Managed method: ReflectionMemberCompletePatchEntry/ReflectionMemberCompletePatchEntry::Method10()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method10(void)
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

// Managed method: ReflectionMemberCompletePatchEntry/ReflectionMemberCompletePatchEntry::Method11()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method11(void)
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

// Managed method: ReflectionMemberCompletePatchEntry/ReflectionMemberCompletePatchEntry::Method12()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method12(void)
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

// Managed method: ReflectionMemberCompletePatchEntry/ReflectionMemberCompletePatchEntry::Method13()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method13(void)
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

// Managed method: ReflectionMemberCompletePatchEntry/ReflectionMemberCompletePatchEntry::Method14()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method14(void)
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

// Managed method: ReflectionMemberCompletePatchEntry/ReflectionMemberCompletePatchEntry::Method15()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method15(void)
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

// Managed method: ReflectionMemberCompletePatchEntry/ReflectionMemberCompletePatchEntry::Method16()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method16(void)
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

// Managed method: ReflectionMemberCompletePatchEntry/ReflectionMemberCompletePatchEntry::Method17()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method17(void)
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

// Managed method: ReflectionMemberCompletePatchEntry/ReflectionMemberCompletePatchEntry::Method18()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method18(void)
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

// Managed method: ReflectionMemberCompletePatchEntry/ReflectionMemberCompletePatchEntry::Method19()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method19(void)
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

// Managed method: ReflectionMemberCompletePatchEntry/ReflectionMemberCompletePatchEntry::Method20()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method20(void)
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

// Managed method: ReflectionMemberCompletePatchEntry/ReflectionMemberCompletePatchEntry::Method21()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method21(void)
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

// Managed method: ReflectionMemberCompletePatchEntry/ReflectionMemberCompletePatchEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 78) chaos_eval_stack{};
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
                goto chaos_ip_104;
            case 1:
                goto chaos_ip_115;
            case 2:
                goto chaos_ip_126;
            case 3:
                goto chaos_ip_137;
            case 4:
                goto chaos_ip_148;
            case 5:
                goto chaos_ip_159;
            case 6:
                goto chaos_ip_170;
            case 7:
                goto chaos_ip_178;
            case 8:
                goto chaos_ip_186;
            case 9:
                goto chaos_ip_194;
            case 10:
                goto chaos_ip_202;
            case 11:
                goto chaos_ip_210;
            case 12:
                goto chaos_ip_218;
            case 13:
                goto chaos_ip_226;
            case 14:
                goto chaos_ip_234;
            case 15:
                goto chaos_ip_242;
            case 16:
                goto chaos_ip_250;
            case 17:
                goto chaos_ip_258;
            case 18:
                goto chaos_ip_266;
            case 19:
                goto chaos_ip_274;
            case 20:
                goto chaos_ip_282;
            case 21:
                goto chaos_ip_290;
            default:
                goto chaos_ip_99;
        }
    }

chaos_ip_99:
    goto chaos_ip_298;

chaos_ip_104:
    {
        const auto chaos_result = ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method0();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_109;

chaos_ip_109:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_110;

chaos_ip_110:
    goto chaos_ip_302;

chaos_ip_115:
    {
        const auto chaos_result = ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method1();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_120;

chaos_ip_120:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_121;

chaos_ip_121:
    goto chaos_ip_302;

chaos_ip_126:
    {
        const auto chaos_result = ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method2();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_131;

chaos_ip_131:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_132;

chaos_ip_132:
    goto chaos_ip_302;

chaos_ip_137:
    {
        const auto chaos_result = ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method3();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_142;

chaos_ip_142:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_143;

chaos_ip_143:
    goto chaos_ip_302;

chaos_ip_148:
    {
        const auto chaos_result = ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method4();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_153;

chaos_ip_153:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_154;

chaos_ip_154:
    goto chaos_ip_302;

chaos_ip_159:
    {
        const auto chaos_result = ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method5();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_164;

chaos_ip_164:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_165;

chaos_ip_165:
    goto chaos_ip_302;

chaos_ip_170:
    {
        const auto chaos_result = ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method6();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_175;

chaos_ip_175:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_176;

chaos_ip_176:
    goto chaos_ip_302;

chaos_ip_178:
    {
        const auto chaos_result = ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method7();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_183;

chaos_ip_183:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_184;

chaos_ip_184:
    goto chaos_ip_302;

chaos_ip_186:
    {
        const auto chaos_result = ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method8();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_191;

chaos_ip_191:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_192;

chaos_ip_192:
    goto chaos_ip_302;

chaos_ip_194:
    {
        const auto chaos_result = ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method9();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_199;

chaos_ip_199:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_200;

chaos_ip_200:
    goto chaos_ip_302;

chaos_ip_202:
    {
        const auto chaos_result = ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method10();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_207;

chaos_ip_207:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_208;

chaos_ip_208:
    goto chaos_ip_302;

chaos_ip_210:
    {
        const auto chaos_result = ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method11();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_215;

chaos_ip_215:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_216;

chaos_ip_216:
    goto chaos_ip_302;

chaos_ip_218:
    {
        const auto chaos_result = ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method12();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_223;

chaos_ip_223:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_224;

chaos_ip_224:
    goto chaos_ip_302;

chaos_ip_226:
    {
        const auto chaos_result = ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method13();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_231;

chaos_ip_231:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_232;

chaos_ip_232:
    goto chaos_ip_302;

chaos_ip_234:
    {
        const auto chaos_result = ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method14();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_239;

chaos_ip_239:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_240;

chaos_ip_240:
    goto chaos_ip_302;

chaos_ip_242:
    {
        const auto chaos_result = ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method15();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_247;

chaos_ip_247:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_248;

chaos_ip_248:
    goto chaos_ip_302;

chaos_ip_250:
    {
        const auto chaos_result = ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method16();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_255;

chaos_ip_255:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_256;

chaos_ip_256:
    goto chaos_ip_302;

chaos_ip_258:
    {
        const auto chaos_result = ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method17();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_263;

chaos_ip_263:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_264;

chaos_ip_264:
    goto chaos_ip_302;

chaos_ip_266:
    {
        const auto chaos_result = ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method18();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_271;

chaos_ip_271:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_272;

chaos_ip_272:
    goto chaos_ip_302;

chaos_ip_274:
    {
        const auto chaos_result = ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method19();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_279;

chaos_ip_279:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_280;

chaos_ip_280:
    goto chaos_ip_302;

chaos_ip_282:
    {
        const auto chaos_result = ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method20();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_287;

chaos_ip_287:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_288;

chaos_ip_288:
    goto chaos_ip_302;

chaos_ip_290:
    {
        const auto chaos_result = ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Method21();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_295;

chaos_ip_295:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_296;

chaos_ip_296:
    goto chaos_ip_302;

chaos_ip_298:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
    goto chaos_ip_299;

chaos_ip_299:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_300;

chaos_ip_300:
    goto chaos_ip_302;

chaos_ip_302:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[2];
    goto chaos_ip_303;

chaos_ip_303:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Native AOT entry for ReflectionMemberCompletePatchEntry/ReflectionMemberCompletePatchEntry::Run:System.Int32(System.Int32)
// Managed symbol: ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Run
extern "C" int RunNativeAot_reflection_member_complete_patch(CHAOS_IL2CPP_INT32 chaos_entry_index)
{
    return ReflectionMemberCompletePatchEntry_ReflectionMemberCompletePatchEntry_Run(chaos_entry_index);
}