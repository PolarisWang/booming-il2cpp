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

constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = 2;
constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = 3;
constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Int32 = 4;
constexpr CHAOS_IL2CPP_INTPTR chaos_boxed_type_id_System_Private_CoreLib_System_Int32 = 5;

CHAOS_IL2CPP_INTPTR chaos_get_base_type_id(CHAOS_IL2CPP_INTPTR chaos_type_id) noexcept
{
    switch (chaos_type_id)
    {
        case chaos_boxed_type_id_System_Private_CoreLib_System_Int32:
            return chaos_type_id_System_Private_CoreLib_System_Object;
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
        case chaos_type_id_System_Private_CoreLib_System_Object:
            return false;
        case chaos_type_id_System_Private_CoreLib_System_String:
            return false;
        case chaos_boxed_type_id_System_Private_CoreLib_System_Int32:
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

struct chaos_type_System_Private_CoreLib_System_Object
{
    chaos_object_header header{};
};

struct chaos_type_System_Private_CoreLib_System_String
{
    chaos_object_header header{};
    CHAOS_IL2CPP_INTPTR length = 0;
    const char* utf8_data = nullptr;
    CHAOS_IL2CPP_UINT64 string_id = 0u;  // stable StringId, 0 = not yet materialized
};

struct chaos_valuetype_System_Private_CoreLib_System_Int32
{
};

struct chaos_boxed_type_System_Private_CoreLib_System_Int32
{
    chaos_object_header header{};
    CHAOS_IL2CPP_INTPTR value = 0;
};

bool chaos_object_equals(CHAOS_IL2CPP_INTPTR chaos_left_value, CHAOS_IL2CPP_INTPTR chaos_right_value) noexcept
{
    // StringId fast path: tagged integers compare directly (O(1)).
    if (chaos_is_string_id(chaos_left_value) && chaos_is_string_id(chaos_right_value))
    {
        return chaos_left_value == chaos_right_value;
    }

    // Mixed StringId vs pointer: never equal (different representations).
    if (chaos_is_string_id(chaos_left_value) != chaos_is_string_id(chaos_right_value))
    {
        return false;
    }

    if (chaos_left_value == chaos_right_value)
    {
        return true;
    }

    if (chaos_left_value == static_cast<CHAOS_IL2CPP_INTPTR>(0) || chaos_right_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        return false;
    }

    auto* chaos_left_header = reinterpret_cast<chaos_object_header*>(chaos_left_value);
    auto* chaos_right_header = reinterpret_cast<chaos_object_header*>(chaos_right_value);
    if (chaos_left_header->type_id != chaos_right_header->type_id)
    {
        return false;
    }

    if (chaos_left_header->type_id == chaos_type_id_System_Private_CoreLib_System_String)
    {
        auto* chaos_left_string = reinterpret_cast<chaos_type_System_Private_CoreLib_System_String*>(chaos_left_value);
        auto* chaos_right_string = reinterpret_cast<chaos_type_System_Private_CoreLib_System_String*>(chaos_right_value);
        if (chaos_left_string->length != chaos_right_string->length)
        {
            return false;
        }

        if (chaos_left_string->utf8_data == nullptr || chaos_right_string->utf8_data == nullptr)
        {
            return chaos_left_string->utf8_data == chaos_right_string->utf8_data;
        }

        return CHAOS_IL2CPP_STRCMP(chaos_left_string->utf8_data, chaos_right_string->utf8_data) == 0;
    }

    switch (chaos_left_header->type_id)
    {
        case chaos_boxed_type_id_System_Private_CoreLib_System_Int32:
            return reinterpret_cast<chaos_boxed_type_System_Private_CoreLib_System_Int32*>(chaos_left_value)->value == reinterpret_cast<chaos_boxed_type_System_Private_CoreLib_System_Int32*>(chaos_right_value)->value;
        default:
            return false;
    }
}

extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Object__Equals_System_Boolean_System_Object_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
    return chaos_object_equals(chaos_arg_0, chaos_arg_1)
        ? static_cast<CHAOS_IL2CPP_INTPTR>(1)
        : static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" void chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
}
}

extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);

// Managed method: ObjectEqualityIdentityNativeEntry/ObjectEqualityIdentityNativeEntry::Method0()
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method0(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 12) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    {
        auto* chaos_object = new chaos_type_System_Private_CoreLib_System_Object{};
        chaos_object->header.type_id = chaos_type_id_System_Private_CoreLib_System_Object;
        const auto chaos_arg_0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
        chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    }
    goto chaos_ip_6;

chaos_ip_6:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);
    goto chaos_ip_8;

chaos_ip_8:
    {
        const auto chaos_value = chaos_eval_stack[--chaos_stack_top];
        auto* chaos_boxed = new chaos_boxed_type_System_Private_CoreLib_System_Int32{};
        chaos_boxed->header.type_id = chaos_boxed_type_id_System_Private_CoreLib_System_Int32;
        chaos_boxed->value = chaos_value;
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
    }
    goto chaos_ip_13;

chaos_ip_13:
    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Object__Equals_System_Boolean_System_Object_(chaos_arg_0, chaos_arg_1);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_18;

chaos_ip_18:
    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            goto chaos_ip_23;
        }
    }
    goto chaos_ip_20;

chaos_ip_20:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_21;

chaos_ip_21:
    goto chaos_ip_24;

chaos_ip_23:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
    goto chaos_ip_24;

chaos_ip_24:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_25;

chaos_ip_25:
    goto chaos_ip_27;

chaos_ip_27:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_28;

chaos_ip_28:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ObjectEqualityIdentityNativeEntry/ObjectEqualityIdentityNativeEntry::Method1()
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method1(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 13) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);
    goto chaos_ip_3;

chaos_ip_3:
    {
        const auto chaos_value = chaos_eval_stack[--chaos_stack_top];
        auto* chaos_boxed = new chaos_boxed_type_System_Private_CoreLib_System_Int32{};
        chaos_boxed->header.type_id = chaos_boxed_type_id_System_Private_CoreLib_System_Int32;
        chaos_boxed->value = chaos_value;
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
    }
    goto chaos_ip_8;

chaos_ip_8:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);
    goto chaos_ip_10;

chaos_ip_10:
    {
        const auto chaos_value = chaos_eval_stack[--chaos_stack_top];
        auto* chaos_boxed = new chaos_boxed_type_System_Private_CoreLib_System_Int32{};
        chaos_boxed->header.type_id = chaos_boxed_type_id_System_Private_CoreLib_System_Int32;
        chaos_boxed->value = chaos_value;
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
    }
    goto chaos_ip_15;

chaos_ip_15:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_20;

chaos_ip_20:
    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            goto chaos_ip_25;
        }
    }
    goto chaos_ip_22;

chaos_ip_22:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_23;

chaos_ip_23:
    goto chaos_ip_26;

chaos_ip_25:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
    goto chaos_ip_26;

chaos_ip_26:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_27;

chaos_ip_27:
    goto chaos_ip_29;

chaos_ip_29:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_30;

chaos_ip_30:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ObjectEqualityIdentityNativeEntry/ObjectEqualityIdentityNativeEntry::Method2()
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method2(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 12) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);
    goto chaos_ip_3;

chaos_ip_3:
    {
        const auto chaos_value = chaos_eval_stack[--chaos_stack_top];
        auto* chaos_boxed = new chaos_boxed_type_System_Private_CoreLib_System_Int32{};
        chaos_boxed->header.type_id = chaos_boxed_type_id_System_Private_CoreLib_System_Int32;
        chaos_boxed->value = chaos_value;
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
    }
    goto chaos_ip_8;

chaos_ip_8:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);
    goto chaos_ip_10;

chaos_ip_10:
    {
        const auto chaos_value = chaos_eval_stack[--chaos_stack_top];
        auto* chaos_boxed = new chaos_boxed_type_System_Private_CoreLib_System_Int32{};
        chaos_boxed->header.type_id = chaos_boxed_type_id_System_Private_CoreLib_System_Int32;
        chaos_boxed->value = chaos_value;
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
    }
    goto chaos_ip_15;

chaos_ip_15:
    {
        const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_eval_stack[--chaos_stack_top]);
        const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_eval_stack[--chaos_stack_top]);
        if (chaos_left == chaos_right)
        {
            goto chaos_ip_20;
        }
    }
    goto chaos_ip_17;

chaos_ip_17:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_18;

chaos_ip_18:
    goto chaos_ip_21;

chaos_ip_20:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
    goto chaos_ip_21;

chaos_ip_21:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_22;

chaos_ip_22:
    goto chaos_ip_24;

chaos_ip_24:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_25;

chaos_ip_25:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ObjectEqualityIdentityNativeEntry/ObjectEqualityIdentityNativeEntry::Method3()
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method3(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 6) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    {
        auto* chaos_object = new chaos_type_System_Private_CoreLib_System_Object{};
        chaos_object->header.type_id = chaos_type_id_System_Private_CoreLib_System_Object;
        const auto chaos_arg_0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
        chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    }
    goto chaos_ip_6;

chaos_ip_6:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_11;

chaos_ip_11:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_12;

chaos_ip_12:
    goto chaos_ip_14;

chaos_ip_14:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_15;

chaos_ip_15:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ObjectEqualityIdentityNativeEntry/ObjectEqualityIdentityNativeEntry::Method4()
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method4(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 12) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    {
        auto* chaos_object = new chaos_type_System_Private_CoreLib_System_Object{};
        chaos_object->header.type_id = chaos_type_id_System_Private_CoreLib_System_Object;
        const auto chaos_arg_0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
        chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    }
    goto chaos_ip_6;

chaos_ip_6:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_11;

chaos_ip_11:
    chaos_eval_stack[chaos_stack_top] = chaos_eval_stack[chaos_stack_top - 1];
    chaos_stack_top++;
    goto chaos_ip_12;

chaos_ip_12:
    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            goto chaos_ip_18;
        }
    }
    goto chaos_ip_14;

chaos_ip_14:
    chaos_stack_top--;
    goto chaos_ip_15;

chaos_ip_15:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_16;

chaos_ip_16:
    goto chaos_ip_23;

chaos_ip_18:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_23;

chaos_ip_23:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_24;

chaos_ip_24:
    goto chaos_ip_26;

chaos_ip_26:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_27;

chaos_ip_27:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ObjectEqualityIdentityNativeEntry/ObjectEqualityIdentityNativeEntry::Method5()
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method5(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    {
        auto* chaos_object = new chaos_type_System_Private_CoreLib_System_Object{};
        chaos_object->header.type_id = chaos_type_id_System_Private_CoreLib_System_Object;
        const auto chaos_arg_0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
        chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    }
    goto chaos_ip_6;

chaos_ip_6:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_11;

chaos_ip_11:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_16;

chaos_ip_16:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_17;

chaos_ip_17:
    goto chaos_ip_19;

chaos_ip_19:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_20;

chaos_ip_20:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ObjectEqualityIdentityNativeEntry/ObjectEqualityIdentityNativeEntry::Method6()
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method6(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_2;

chaos_ip_2:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_3;

chaos_ip_3:
    goto chaos_ip_5;

chaos_ip_5:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_6;

chaos_ip_6:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ObjectEqualityIdentityNativeEntry/ObjectEqualityIdentityNativeEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 33) chaos_eval_stack{};
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
                goto chaos_ip_41;
            case 1:
                goto chaos_ip_49;
            case 2:
                goto chaos_ip_57;
            case 3:
                goto chaos_ip_65;
            case 4:
                goto chaos_ip_73;
            case 5:
                goto chaos_ip_81;
            case 6:
                goto chaos_ip_89;
            default:
                goto chaos_ip_39;
        }
    }

chaos_ip_39:
    goto chaos_ip_97;

chaos_ip_41:
    {
        const auto chaos_result = ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method0();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_46;

chaos_ip_46:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_47;

chaos_ip_47:
    goto chaos_ip_101;

chaos_ip_49:
    {
        const auto chaos_result = ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method1();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_54;

chaos_ip_54:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_55;

chaos_ip_55:
    goto chaos_ip_101;

chaos_ip_57:
    {
        const auto chaos_result = ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method2();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_62;

chaos_ip_62:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_63;

chaos_ip_63:
    goto chaos_ip_101;

chaos_ip_65:
    {
        const auto chaos_result = ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method3();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_70;

chaos_ip_70:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_71;

chaos_ip_71:
    goto chaos_ip_101;

chaos_ip_73:
    {
        const auto chaos_result = ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method4();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_78;

chaos_ip_78:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_79;

chaos_ip_79:
    goto chaos_ip_101;

chaos_ip_81:
    {
        const auto chaos_result = ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method5();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_86;

chaos_ip_86:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_87;

chaos_ip_87:
    goto chaos_ip_101;

chaos_ip_89:
    {
        const auto chaos_result = ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Method6();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_94;

chaos_ip_94:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_95;

chaos_ip_95:
    goto chaos_ip_101;

chaos_ip_97:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
    goto chaos_ip_98;

chaos_ip_98:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_99;

chaos_ip_99:
    goto chaos_ip_101;

chaos_ip_101:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[2];
    goto chaos_ip_102;

chaos_ip_102:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Native AOT entry for ObjectEqualityIdentityNativeEntry/ObjectEqualityIdentityNativeEntry::Run:System.Int32(System.Int32)
// Managed symbol: ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Run
extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 chaos_entry_index)
{
    return ObjectEqualityIdentityNativeEntry_ObjectEqualityIdentityNativeEntry_Run(chaos_entry_index);
}