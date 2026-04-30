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

extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);
}

extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);

// Managed method: BoxingUnboxingCastsPatchEntry/BoxingUnboxingCastsPatchEntry::Method0()
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method0(void)
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

// Managed method: BoxingUnboxingCastsPatchEntry/BoxingUnboxingCastsPatchEntry::Method1()
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method1(void)
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

// Managed method: BoxingUnboxingCastsPatchEntry/BoxingUnboxingCastsPatchEntry::Method2()
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method2(void)
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

// Managed method: BoxingUnboxingCastsPatchEntry/BoxingUnboxingCastsPatchEntry::Method3()
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method3(void)
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

// Managed method: BoxingUnboxingCastsPatchEntry/BoxingUnboxingCastsPatchEntry::Method4()
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method4(void)
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

// Managed method: BoxingUnboxingCastsPatchEntry/BoxingUnboxingCastsPatchEntry::Method5()
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method5(void)
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

// Managed method: BoxingUnboxingCastsPatchEntry/BoxingUnboxingCastsPatchEntry::Method6()
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method6(void)
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

// Managed method: BoxingUnboxingCastsPatchEntry/BoxingUnboxingCastsPatchEntry::Method7()
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method7(void)
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

// Managed method: BoxingUnboxingCastsPatchEntry/BoxingUnboxingCastsPatchEntry::Method8()
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method8(void)
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

// Managed method: BoxingUnboxingCastsPatchEntry/BoxingUnboxingCastsPatchEntry::Method9()
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method9(void)
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

// Managed method: BoxingUnboxingCastsPatchEntry/BoxingUnboxingCastsPatchEntry::Method10()
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method10(void)
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

// Managed method: BoxingUnboxingCastsPatchEntry/BoxingUnboxingCastsPatchEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 45) chaos_eval_stack{};
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
                goto chaos_ip_57;
            case 1:
                goto chaos_ip_65;
            case 2:
                goto chaos_ip_73;
            case 3:
                goto chaos_ip_81;
            case 4:
                goto chaos_ip_89;
            case 5:
                goto chaos_ip_97;
            case 6:
                goto chaos_ip_105;
            case 7:
                goto chaos_ip_113;
            case 8:
                goto chaos_ip_121;
            case 9:
                goto chaos_ip_129;
            case 10:
                goto chaos_ip_137;
            default:
                goto chaos_ip_55;
        }
    }

chaos_ip_55:
    goto chaos_ip_145;

chaos_ip_57:
    {
        const auto chaos_result = BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method0();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_62;

chaos_ip_62:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_63;

chaos_ip_63:
    goto chaos_ip_149;

chaos_ip_65:
    {
        const auto chaos_result = BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method1();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_70;

chaos_ip_70:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_71;

chaos_ip_71:
    goto chaos_ip_149;

chaos_ip_73:
    {
        const auto chaos_result = BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method2();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_78;

chaos_ip_78:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_79;

chaos_ip_79:
    goto chaos_ip_149;

chaos_ip_81:
    {
        const auto chaos_result = BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method3();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_86;

chaos_ip_86:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_87;

chaos_ip_87:
    goto chaos_ip_149;

chaos_ip_89:
    {
        const auto chaos_result = BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method4();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_94;

chaos_ip_94:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_95;

chaos_ip_95:
    goto chaos_ip_149;

chaos_ip_97:
    {
        const auto chaos_result = BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method5();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_102;

chaos_ip_102:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_103;

chaos_ip_103:
    goto chaos_ip_149;

chaos_ip_105:
    {
        const auto chaos_result = BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method6();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_110;

chaos_ip_110:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_111;

chaos_ip_111:
    goto chaos_ip_149;

chaos_ip_113:
    {
        const auto chaos_result = BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method7();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_118;

chaos_ip_118:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_119;

chaos_ip_119:
    goto chaos_ip_149;

chaos_ip_121:
    {
        const auto chaos_result = BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method8();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_126;

chaos_ip_126:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_127;

chaos_ip_127:
    goto chaos_ip_149;

chaos_ip_129:
    {
        const auto chaos_result = BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method9();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_134;

chaos_ip_134:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_135;

chaos_ip_135:
    goto chaos_ip_149;

chaos_ip_137:
    {
        const auto chaos_result = BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Method10();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_142;

chaos_ip_142:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_143;

chaos_ip_143:
    goto chaos_ip_149;

chaos_ip_145:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
    goto chaos_ip_146;

chaos_ip_146:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_147;

chaos_ip_147:
    goto chaos_ip_149;

chaos_ip_149:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[2];
    goto chaos_ip_150;

chaos_ip_150:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Native AOT entry for BoxingUnboxingCastsPatchEntry/BoxingUnboxingCastsPatchEntry::Run:System.Int32(System.Int32)
// Managed symbol: BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Run
extern "C" int RunNativeAot_boxing_unboxing_casts_patch(CHAOS_IL2CPP_INT32 chaos_entry_index)
{
    return BoxingUnboxingCastsPatchEntry_BoxingUnboxingCastsPatchEntry_Run(chaos_entry_index);
}