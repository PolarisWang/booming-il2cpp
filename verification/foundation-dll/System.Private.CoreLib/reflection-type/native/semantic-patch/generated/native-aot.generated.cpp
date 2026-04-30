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
constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = 3;
constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = 4;
constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Type = 5;

CHAOS_IL2CPP_INTPTR chaos_get_base_type_id(CHAOS_IL2CPP_INTPTR chaos_type_id) noexcept
{
    switch (chaos_type_id)
    {
        case chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo:
            return chaos_type_id_System_Private_CoreLib_System_Object;
        case chaos_type_id_System_Private_CoreLib_System_Type:
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
        case chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo:
            return false;
        case chaos_type_id_System_Private_CoreLib_System_String:
            return false;
        case chaos_type_id_System_Private_CoreLib_System_Type:
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

struct chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo : public chaos_type_System_Private_CoreLib_System_Object
{
    CHAOS_IL2CPP_INTPTR declaring_type_handle = 0;
    CHAOS_IL2CPP_INTPTR runtime_method_handle = 0;
    CHAOS_IL2CPP_INTPTR generic_definition_method_handle = 0;
    CHAOS_IL2CPP_INTPTR generic_argument_type_handle = 0;
    CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
    CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_String
{
    chaos_object_header header{};
    CHAOS_IL2CPP_INTPTR length = 0;
    const char* utf8_data = nullptr;
    CHAOS_IL2CPP_UINT64 string_id = 0u;  // stable StringId, 0 = not yet materialized
};

struct chaos_type_System_Private_CoreLib_System_Type : public chaos_type_System_Private_CoreLib_System_Object
{
    CHAOS_IL2CPP_INTPTR runtime_type_handle = 0;
    CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
    CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;
};

const char* chaos_reflection_get_string_utf8(CHAOS_IL2CPP_INTPTR chaos_string_value) noexcept
{
    if (chaos_string_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        return nullptr;
    }

    if (chaos_is_string_id(chaos_string_value))
    {
        const auto chaos_view = chaos::il2cpp::string_table::Resolve(
            chaos_extract_string_id(chaos_string_value));
        return chaos_view.utf8_data;
    }

    auto* chaos_string = reinterpret_cast<chaos_type_System_Private_CoreLib_System_String*>(chaos_string_value);
    return chaos_string->utf8_data;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_string_utf8_copy(const char* chaos_utf8_data, CHAOS_IL2CPP_SIZE chaos_length)
{
    if (chaos_utf8_data == nullptr)
    {
        return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }

    const auto chaos_id = chaos::il2cpp::string_table::Intern(
        chaos_utf8_data, static_cast<CHAOS_IL2CPP_UINT32>(chaos_length));
    return chaos_make_string_id_value(chaos_id);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_concat_string_pair_values(CHAOS_IL2CPP_INTPTR chaos_left_string_value, CHAOS_IL2CPP_INTPTR chaos_right_string_value)
{
    const auto chaos_left_length =
        chaos_left_string_value == static_cast<CHAOS_IL2CPP_INTPTR>(0)
            ? static_cast<CHAOS_IL2CPP_SIZE>(0)
            : chaos_is_string_id(chaos_left_string_value)
                ? static_cast<CHAOS_IL2CPP_SIZE>(chaos::il2cpp::string_table::Resolve(
                    chaos_extract_string_id(chaos_left_string_value)).byte_count)
                : static_cast<CHAOS_IL2CPP_SIZE>(reinterpret_cast<chaos_type_System_Private_CoreLib_System_String*>(chaos_left_string_value)->length);
    const auto chaos_right_length =
        chaos_right_string_value == static_cast<CHAOS_IL2CPP_INTPTR>(0)
            ? static_cast<CHAOS_IL2CPP_SIZE>(0)
            : chaos_is_string_id(chaos_right_string_value)
                ? static_cast<CHAOS_IL2CPP_SIZE>(chaos::il2cpp::string_table::Resolve(
                    chaos_extract_string_id(chaos_right_string_value)).byte_count)
                : static_cast<CHAOS_IL2CPP_SIZE>(reinterpret_cast<chaos_type_System_Private_CoreLib_System_String*>(chaos_right_string_value)->length);
    const char* chaos_left_utf8 = chaos_reflection_get_string_utf8(chaos_left_string_value);
    const char* chaos_right_utf8 = chaos_reflection_get_string_utf8(chaos_right_string_value);
    if ((chaos_left_string_value != static_cast<CHAOS_IL2CPP_INTPTR>(0) && chaos_left_utf8 == nullptr && chaos_left_length != 0) ||
        (chaos_right_string_value != static_cast<CHAOS_IL2CPP_INTPTR>(0) && chaos_right_utf8 == nullptr && chaos_right_length != 0))
    {
        CHAOS_IL2CPP_ABORT();
    }

    const auto chaos_combined_length = chaos_left_length + chaos_right_length;
    auto* chaos_combined_utf8 = new char[chaos_combined_length + 1];
    if (chaos_left_length != 0)
    {
        CHAOS_IL2CPP_MEMCPY(chaos_combined_utf8, chaos_left_utf8, chaos_left_length);
    }
    if (chaos_right_length != 0)
    {
        CHAOS_IL2CPP_MEMCPY(chaos_combined_utf8 + chaos_left_length, chaos_right_utf8, chaos_right_length);
    }
    chaos_combined_utf8[chaos_combined_length] = '\0';
    const auto chaos_result = chaos_reflection_create_string_utf8_copy(chaos_combined_utf8, chaos_combined_length);
    delete[] chaos_combined_utf8;
    return chaos_result;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_string_literal(const char* chaos_utf8_data)
{
    if (chaos_utf8_data == nullptr)
    {
        return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }

    return chaos_reflection_create_string_utf8_copy(chaos_utf8_data, CHAOS_IL2CPP_STRLEN(chaos_utf8_data));
}


CHAOS_IL2CPP_INTPTR chaos_string_materialize(CHAOS_IL2CPP_INTPTR chaos_value) noexcept
{
    if (chaos_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }

    if (!chaos_is_string_id(chaos_value))
    {
        return chaos_value;
    }

    const auto chaos_id = chaos_extract_string_id(chaos_value);
    const auto chaos_view = chaos::il2cpp::string_table::Resolve(chaos_id);

    auto* chaos_string = static_cast<chaos_type_System_Private_CoreLib_System_String*>(chaos::il2cpp::runtime_core::GcAllocate(sizeof(
chaos_type_System_Private_CoreLib_System_String)));
    chaos_string->header.type_id = chaos_type_id_System_Private_CoreLib_System_String;
    chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_view.byte_count);
    auto* owned_utf8 = static_cast<char*>(chaos::il2cpp::runtime_core::GcAllocateAtomic(chaos_view.byte_count + 1));
    CHAOS_IL2CPP_MEMCPY(owned_utf8, chaos_view.utf8_data, chaos_view.byte_count);
    owned_utf8[chaos_view.byte_count] = '\0';
    chaos_string->utf8_data = owned_utf8;
    chaos_string->string_id = chaos_id;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_reference_array(CHAOS_IL2CPP_INTPTR chaos_element_type_id, CHAOS_IL2CPP_SIZE chaos_length)
{
    auto* chaos_array = new chaos_managed_array{};
    chaos_array->header.type_id = chaos_type_id_managed_array;
    chaos_array->element_type_shape = chaos_type_shape_reference;
    chaos_array->element_type_id = chaos_element_type_id;
    chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
    chaos_array->elements = chaos_length == 0 ? nullptr : new CHAOS_IL2CPP_INTPTR[chaos_length]{};
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_name_value_from_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept
{
    switch (chaos_type_handle)
    {
        default:
            return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }
}

CHAOS_IL2CPP_INT32 chaos_reflection_get_type_metadata_token_from_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept
{
    switch (chaos_type_handle)
    {
        default:
            return 0;
    }
}

CHAOS_IL2CPP_INT32 chaos_reflection_get_type_attributes_from_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept
{
    switch (chaos_type_handle)
    {
        default:
            return 0;
    }
}

CHAOS_IL2CPP_INT32 chaos_reflection_get_type_kind_flags_from_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept
{
    switch (chaos_type_handle)
    {
        default:
            return 0;
    }
}

CHAOS_IL2CPP_INT32 chaos_reflection_get_type_generic_flags_from_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept
{
    switch (chaos_type_handle)
    {
        default:
            return 0;
    }
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_full_name_from_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept
{
    switch (chaos_type_handle)
    {
        default:
            return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_namespace_from_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept
{
    switch (chaos_type_handle)
    {
        default:
            return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_qualified_name_from_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept
{
    switch (chaos_type_handle)
    {
        default:
            return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_type_value(CHAOS_IL2CPP_INTPTR chaos_type_handle)
{
    auto* chaos_type = new chaos_type_System_Private_CoreLib_System_Type{};
    chaos_type->header.type_id = chaos_type_id_System_Private_CoreLib_System_Type;
    chaos_type->runtime_type_handle = chaos_type_handle;
    chaos_type->runtime_name_value = chaos_reflection_get_type_name_value_from_handle(chaos_type_handle);
    chaos_type->runtime_metadata_token_value = chaos_reflection_get_type_metadata_token_from_handle(chaos_type_handle);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_type);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_method_name_value_from_handle(CHAOS_IL2CPP_INTPTR chaos_method_handle) noexcept
{
    switch (chaos_method_handle)
    {
        default:
            return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }
}

CHAOS_IL2CPP_INT32 chaos_reflection_get_method_metadata_token_from_handle(CHAOS_IL2CPP_INTPTR chaos_method_handle) noexcept
{
    switch (chaos_method_handle)
    {
        default:
            return 0;
    }
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_declaring_type_handle_from_type_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept
{
    switch (chaos_type_handle)
    {
        default:
            return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_assembly(CHAOS_IL2CPP_INTPTR chaos_type_value)
{
    auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_type_value);
    if (chaos_type == nullptr)
    {
        return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }

    switch (chaos_type->runtime_type_handle)
    {
        default:
            return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_executing_assembly()
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_assembly_types(CHAOS_IL2CPP_INTPTR chaos_assembly_value)
{
    auto* chaos_assembly = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_Assembly*>(chaos_assembly_value);
    if (chaos_assembly == nullptr)
    {
        return chaos_reflection_create_reference_array(
            chaos_type_id_System_Private_CoreLib_System_Type,
            static_cast<CHAOS_IL2CPP_SIZE>(0));
    }

    const auto chaos_name_value = chaos_assembly->runtime_assembly_name_value;
    const char* chaos_assembly_name = chaos_reflection_get_string_utf8(chaos_name_value);
    if (chaos_assembly_name == nullptr)
    {
        return chaos_reflection_create_reference_array(
            chaos_type_id_System_Private_CoreLib_System_Type,
            static_cast<CHAOS_IL2CPP_SIZE>(0));
    }

    return chaos_reflection_create_reference_array(
        chaos_type_id_System_Private_CoreLib_System_Type,
        static_cast<CHAOS_IL2CPP_SIZE>(0));
}

CHAOS_IL2CPP_INTPTR chaos_reflection_resolve_type_handle(CHAOS_IL2CPP_INTPTR chaos_assembly_name_value, const char* chaos_type_name) noexcept
{
    const char* chaos_assembly_name = chaos_reflection_get_string_utf8(chaos_assembly_name_value);
    if (chaos_assembly_name == nullptr || chaos_type_name == nullptr)
    {
        return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }

    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_from_assembly(CHAOS_IL2CPP_INTPTR chaos_assembly_value, CHAOS_IL2CPP_INTPTR chaos_name_value)
{
    auto* chaos_assembly = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_Assembly*>(chaos_assembly_value);
    if (chaos_assembly == nullptr)
    {
        return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }

    const auto chaos_type_handle = chaos_reflection_resolve_type_handle(
        chaos_assembly->runtime_assembly_name_value,
        chaos_reflection_get_string_utf8(chaos_name_value));
    if (chaos_type_handle == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }

    return chaos_reflection_create_type_value(chaos_type_handle);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_by_name(CHAOS_IL2CPP_INTPTR chaos_name_value)
{
    const char* chaos_name = chaos_reflection_get_string_utf8(chaos_name_value);
    if (chaos_name == nullptr)
    {
        return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }

    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_assembly_name(CHAOS_IL2CPP_INTPTR chaos_assembly_value)
{
    auto* chaos_assembly = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_Assembly*>(chaos_assembly_value);
    if (chaos_assembly == nullptr)
    {
        return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }

    auto* chaos_assembly_name = new chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName{};
    chaos_assembly_name->header.type_id = chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName;
    chaos_assembly_name->runtime_name_value = chaos_assembly->runtime_assembly_name_value;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_assembly_name);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_assembly_name_value(CHAOS_IL2CPP_INTPTR chaos_assembly_name_value) noexcept
{
    auto* chaos_assembly_name = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName*>(chaos_assembly_name_value);
    return chaos_assembly_name == nullptr
        ? static_cast<CHAOS_IL2CPP_INTPTR>(0)
        : chaos_assembly_name->runtime_name_value;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_from_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle)
{
    return chaos_reflection_create_type_value(chaos_type_handle);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_handle(CHAOS_IL2CPP_INTPTR chaos_type_value) noexcept
{
    auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_type_value);
    return chaos_type->runtime_type_handle;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_resolve_method_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle, const char* chaos_method_name) noexcept
{
    if (chaos_method_name == nullptr)
    {
        return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }

    switch (chaos_type_handle)
    {
        case static_cast<CHAOS_IL2CPP_INTPTR>(40834504u):
            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method0") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(110754263u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method10") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(114763062u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method11") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(113201599u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method12") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(107217252u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method13") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(116567877u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method14") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(107628946u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method15") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(111697531u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method16") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(105596976u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method17") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(108037553u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method18") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(115859870u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method19") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(116953575u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method1") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(103491790u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method20") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(102945783u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method21") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(103930862u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method22") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(110541981u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method23") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(112858652u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method24") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(110185363u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method25") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(114991082u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method26") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(103418825u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method27") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(102615400u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method28") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(101678799u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method29") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(101768774u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method2") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(105180925u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method30") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(110955448u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method31") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(116514329u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method32") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(113223738u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method33") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(101610339u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method34") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(114924908u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method35") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(100668589u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method36") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(117139006u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method37") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(106386055u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method38") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(111396048u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method39") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(109304785u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method3") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(111989372u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method40") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(107638841u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method41") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(112981720u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method42") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(103758851u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method4") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(117353715u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method5") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(109934538u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method6") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(112078505u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method7") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(108401992u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method8") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(114111791u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method9") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(117273638u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Run") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(107492592u);
            }

            break;
        default:
            break;
    }

    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_method(CHAOS_IL2CPP_INTPTR chaos_type_value, CHAOS_IL2CPP_INTPTR chaos_name_value, CHAOS_IL2CPP_INT32 chaos_binding_flags)
{
    (void)chaos_binding_flags;
    auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_type_value);
    const auto* chaos_method_name = chaos_reflection_get_string_utf8(chaos_name_value);
    const auto chaos_method_handle = chaos_reflection_resolve_method_handle(chaos_type->runtime_type_handle, chaos_method_name);
    if (chaos_method_handle == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }

    auto* chaos_method = new chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo{};
    chaos_method->header.type_id = chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo;
    chaos_method->declaring_type_handle = chaos_type->runtime_type_handle;
    chaos_method->runtime_method_handle = chaos_method_handle;
    chaos_method->generic_definition_method_handle = chaos_method_handle;
    chaos_method->runtime_name_value = chaos_reflection_get_method_name_value_from_handle(chaos_method_handle);
    chaos_method->runtime_metadata_token_value = chaos_reflection_get_method_metadata_token_from_handle(chaos_method_handle);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_method);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_closed_generic_method_handle(CHAOS_IL2CPP_INTPTR chaos_definition_handle, CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept
{
    CHAOS_IL2CPP_UINT32 hash = 2166136261u;
    const auto chaos_definition_bits = static_cast<CHAOS_IL2CPP_UINT64>(chaos_definition_handle);
    const auto chaos_type_bits = static_cast<CHAOS_IL2CPP_UINT64>(chaos_type_handle);
    hash ^= static_cast<CHAOS_IL2CPP_UINT32>(chaos_definition_bits & 0xFFFFFFFFu);
    hash *= 16777619u;
    hash ^= static_cast<CHAOS_IL2CPP_UINT32>(chaos_definition_bits >> 32);
    hash *= 16777619u;
    hash ^= static_cast<CHAOS_IL2CPP_UINT32>(chaos_type_bits & 0xFFFFFFFFu);
    hash *= 16777619u;
    hash ^= static_cast<CHAOS_IL2CPP_UINT32>(chaos_type_bits >> 32);
    hash *= 16777619u;
    auto row_index = hash & 0x00FFFFFFu;
    if (row_index == 0u)
    {
        row_index = 1u;
    }

    return static_cast<CHAOS_IL2CPP_INTPTR>(0x06000000u | row_index);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_make_generic_method(CHAOS_IL2CPP_INTPTR chaos_method_value, CHAOS_IL2CPP_INTPTR chaos_type_array_value)
{
    if (chaos_method_value == static_cast<CHAOS_IL2CPP_INTPTR>(0) || chaos_type_array_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }

    auto* chaos_method = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo*>(chaos_method_value);
    auto* chaos_type_array = reinterpret_cast<chaos_managed_array*>(chaos_type_array_value);
    if (chaos_type_array->length <= static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }

    const auto chaos_type_argument_value = chaos_type_array->elements[0];
    if (chaos_type_argument_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }

    auto* chaos_type_argument = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_type_argument_value);
    auto* chaos_closed_method = new chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo{};
    chaos_closed_method->header.type_id = chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo;
    chaos_closed_method->declaring_type_handle = chaos_method->declaring_type_handle;
    chaos_closed_method->generic_definition_method_handle = chaos_method->generic_definition_method_handle != static_cast<CHAOS_IL2CPP_INTPTR>(0)
        ? chaos_method->generic_definition_method_handle
        : chaos_method->runtime_method_handle;
    chaos_closed_method->generic_argument_type_handle = chaos_type_argument->runtime_type_handle;
    chaos_closed_method->runtime_method_handle = chaos_reflection_create_closed_generic_method_handle(
        chaos_closed_method->generic_definition_method_handle,
        chaos_closed_method->generic_argument_type_handle);
    chaos_closed_method->runtime_name_value = chaos_method->runtime_name_value == static_cast<CHAOS_IL2CPP_INTPTR>(0)
        ? chaos_reflection_get_method_name_value_from_handle(chaos_closed_method->generic_definition_method_handle)
        : chaos_method->runtime_name_value;
    chaos_closed_method->runtime_metadata_token_value = chaos_method->runtime_metadata_token_value != 0
        ? chaos_method->runtime_metadata_token_value
        : chaos_reflection_get_method_metadata_token_from_handle(chaos_closed_method->generic_definition_method_handle);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_closed_method);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_method_handle(CHAOS_IL2CPP_INTPTR chaos_method_value) noexcept
{
    auto* chaos_method = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo*>(chaos_method_value);
    return chaos_method->runtime_method_handle;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_invoke_method(CHAOS_IL2CPP_INTPTR chaos_method_value, CHAOS_IL2CPP_INTPTR chaos_target_value, CHAOS_IL2CPP_INTPTR chaos_args_array_value)
{
    if (chaos_method_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }

    auto* chaos_method = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo*>(chaos_method_value);
    auto* chaos_args = reinterpret_cast<chaos_managed_array*>(chaos_args_array_value);

    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method14(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method15(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method16(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method17(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method18(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method19(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method20(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method21(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method22(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method23(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method24(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method25(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method26(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method27(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method28(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method29(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method30(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method31(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method32(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method33(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method34(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method35(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method36(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method37(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method38(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method39(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method40(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method41(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method42(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);


// StringId constants for compile-time-known string literals.
constexpr CHAOS_IL2CPP_UINT64 chaos_string_id_08140B07B4D19DF7 = 582102379563949559U;

// AOT-baked string table: sorted by StringId for binary search at runtime.
constexpr chaos::il2cpp::string_table::StringEntry chaos_aot_string_entries[] = {
    { chaos_string_id_08140B07B4D19DF7, "99", 2u },
};

constexpr CHAOS_IL2CPP_UINT32 chaos_aot_string_entry_count = sizeof(chaos_aot_string_entries) / sizeof(chaos_aot_string_entries[0]);

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_DeclaringType_System_Type__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return chaos_reflection_get_declaring_type(chaos_arg_0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_Name_System_String__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return chaos_reflection_get_member_name(chaos_arg_0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetConstructors_System_Reflection_ConstructorInfo___System_Reflection_BindingFlags_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1)
{
    return chaos_reflection_get_constructors(chaos_arg_0, chaos_arg_1);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetElementType_System_Type__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetFields_System_Reflection_FieldInfo___System_Reflection_BindingFlags_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1)
{
    auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_arg_0);
    return chaos_reflection_get_fields(chaos_type->runtime_type_handle);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetFields_System_Reflection_FieldInfo____(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_arg_0);
    return chaos_reflection_get_fields(chaos_type->runtime_type_handle);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetInterfaces_System_Type____(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_arg_0);
    return chaos_reflection_get_interfaces(chaos_type->runtime_type_handle);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetMembers_System_Reflection_MemberInfo____(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_arg_0);
    return chaos_reflection_get_members(chaos_type->runtime_type_handle);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethods_System_Reflection_MethodInfo___System_Reflection_BindingFlags_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1)
{
    auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_arg_0);
    return chaos_reflection_get_methods(chaos_type->runtime_type_handle);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethods_System_Reflection_MethodInfo____(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_arg_0);
    return chaos_reflection_get_methods(chaos_type->runtime_type_handle);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetNestedType_System_Type_System_String_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetNestedTypes_System_Type____(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_arg_0);
    return chaos_reflection_get_nested_types(chaos_type->runtime_type_handle);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return chaos_reflection_get_type_from_handle(chaos_arg_0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_AssemblyQualifiedName_System_String__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_arg_0);
    return chaos_reflection_get_type_qualified_name_from_handle(chaos_type->runtime_type_handle);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_Assembly_System_Reflection_Assembly__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return chaos_reflection_get_assembly(chaos_arg_0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_BaseType_System_Type__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_FullName_System_String__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_arg_0);
    return chaos_reflection_get_type_full_name_from_handle(chaos_type->runtime_type_handle);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsAbstract_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_arg_0);
    const auto chaos_attrs = chaos_reflection_get_type_attributes_from_handle(chaos_type->runtime_type_handle);
    return (chaos_attrs & 0x0080) ? static_cast<CHAOS_IL2CPP_INTPTR>(1) : static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsArray_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsByRef_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsConstructedGenericType_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_arg_0);
    if (chaos_type->runtime_type_handle == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }
    return (chaos_reflection_get_type_generic_flags_from_handle(chaos_type->runtime_type_handle) & 0x02) != 0
        ? static_cast<CHAOS_IL2CPP_INTPTR>(1)
        : static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsEnum_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_arg_0);
    const auto chaos_flags = chaos_reflection_get_type_kind_flags_from_handle(chaos_type->runtime_type_handle);
    return chaos_flags == 2 ? static_cast<CHAOS_IL2CPP_INTPTR>(1) : static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsGenericType_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_arg_0);
    if (chaos_type->runtime_type_handle == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }
    return (chaos_reflection_get_type_generic_flags_from_handle(chaos_type->runtime_type_handle) & 0x01) != 0
        ? static_cast<CHAOS_IL2CPP_INTPTR>(1)
        : static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsInterface_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_arg_0);
    const auto chaos_attrs = chaos_reflection_get_type_attributes_from_handle(chaos_type->runtime_type_handle);
    return (chaos_attrs & 0x0020) ? static_cast<CHAOS_IL2CPP_INTPTR>(1) : static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNestedPrivate_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_arg_0);
    const auto chaos_attrs = chaos_reflection_get_type_attributes_from_handle(chaos_type->runtime_type_handle);
    return (chaos_attrs & 0x00000007) == 0x00000003 ? static_cast<CHAOS_IL2CPP_INTPTR>(1) : static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNestedPublic_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_arg_0);
    const auto chaos_attrs = chaos_reflection_get_type_attributes_from_handle(chaos_type->runtime_type_handle);
    return (chaos_attrs & 0x00000007) == 0x00000002 ? static_cast<CHAOS_IL2CPP_INTPTR>(1) : static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNested_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_arg_0);
    const auto chaos_attrs = chaos_reflection_get_type_attributes_from_handle(chaos_type->runtime_type_handle);
    return (chaos_attrs & 0x00000060) == 0x00000060 ? static_cast<CHAOS_IL2CPP_INTPTR>(1) : static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNotPublic_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_arg_0);
    const auto chaos_attrs = chaos_reflection_get_type_attributes_from_handle(chaos_type->runtime_type_handle);
    return (chaos_attrs & 0x00000007) == 0x00000000 ? static_cast<CHAOS_IL2CPP_INTPTR>(1) : static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsPointer_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsPublic_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_arg_0);
    const auto chaos_attrs = chaos_reflection_get_type_attributes_from_handle(chaos_type->runtime_type_handle);
    return (chaos_attrs & 0x00000007) == 0x00000001 ? static_cast<CHAOS_IL2CPP_INTPTR>(1) : static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsSealed_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_arg_0);
    const auto chaos_attrs = chaos_reflection_get_type_attributes_from_handle(chaos_type->runtime_type_handle);
    return (chaos_attrs & 0x0100) ? static_cast<CHAOS_IL2CPP_INTPTR>(1) : static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsValueType_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_arg_0);
    const auto chaos_flags = chaos_reflection_get_type_kind_flags_from_handle(chaos_type->runtime_type_handle);
    return chaos_flags == 1 ? static_cast<CHAOS_IL2CPP_INTPTR>(1) : static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_Namespace_System_String__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_arg_0);
    return chaos_reflection_get_type_namespace_from_handle(chaos_type->runtime_type_handle);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_TypeHandle_System_RuntimeTypeHandle__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return chaos_reflection_get_type_handle(chaos_arg_0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_UnderlyingSystemType_System_Type__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return chaos_arg_0;
}
}

extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method14(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method15(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method16(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method17(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method18(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method19(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method20(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method21(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method22(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method23(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method24(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method25(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method26(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method27(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method28(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method29(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method30(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method31(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method32(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method33(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method34(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method35(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method36(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method37(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method38(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method39(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method40(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method41(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method42(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method0()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method0(void)
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

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method1()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method1(void)
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

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method2()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method2(void)
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

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method3()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method3(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 9) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_TypeHandle_System_RuntimeTypeHandle__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_21;

chaos_ip_21:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
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

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method4()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method4(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 13) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_Name_System_String__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
    chaos_eval_stack[chaos_stack_top] = chaos_eval_stack[chaos_stack_top - 1];
    chaos_stack_top++;
    goto chaos_ip_17;

chaos_ip_17:
    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            goto chaos_ip_23;
        }
    }
    goto chaos_ip_19;

chaos_ip_19:
    chaos_stack_top--;
    goto chaos_ip_20;

chaos_ip_20:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_21;

chaos_ip_21:
    goto chaos_ip_28;

chaos_ip_23:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_28;

chaos_ip_28:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_29;

chaos_ip_29:
    goto chaos_ip_31;

chaos_ip_31:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_32;

chaos_ip_32:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method5()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method5(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 13) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_FullName_System_String__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
    chaos_eval_stack[chaos_stack_top] = chaos_eval_stack[chaos_stack_top - 1];
    chaos_stack_top++;
    goto chaos_ip_17;

chaos_ip_17:
    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            goto chaos_ip_23;
        }
    }
    goto chaos_ip_19;

chaos_ip_19:
    chaos_stack_top--;
    goto chaos_ip_20;

chaos_ip_20:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_21;

chaos_ip_21:
    goto chaos_ip_28;

chaos_ip_23:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_28;

chaos_ip_28:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_29;

chaos_ip_29:
    goto chaos_ip_31;

chaos_ip_31:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_32;

chaos_ip_32:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method6()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method6(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 13) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_Namespace_System_String__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
    chaos_eval_stack[chaos_stack_top] = chaos_eval_stack[chaos_stack_top - 1];
    chaos_stack_top++;
    goto chaos_ip_17;

chaos_ip_17:
    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            goto chaos_ip_23;
        }
    }
    goto chaos_ip_19;

chaos_ip_19:
    chaos_stack_top--;
    goto chaos_ip_20;

chaos_ip_20:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_21;

chaos_ip_21:
    goto chaos_ip_28;

chaos_ip_23:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_28;

chaos_ip_28:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_29;

chaos_ip_29:
    goto chaos_ip_31;

chaos_ip_31:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_32;

chaos_ip_32:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method7()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method7(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 13) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_AssemblyQualifiedName_System_String__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
    chaos_eval_stack[chaos_stack_top] = chaos_eval_stack[chaos_stack_top - 1];
    chaos_stack_top++;
    goto chaos_ip_17;

chaos_ip_17:
    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            goto chaos_ip_23;
        }
    }
    goto chaos_ip_19;

chaos_ip_19:
    chaos_stack_top--;
    goto chaos_ip_20;

chaos_ip_20:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_21;

chaos_ip_21:
    goto chaos_ip_28;

chaos_ip_23:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_28;

chaos_ip_28:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_29;

chaos_ip_29:
    goto chaos_ip_31;

chaos_ip_31:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_32;

chaos_ip_32:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method8()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method8(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_TypeHandle_System_RuntimeTypeHandle__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_17;

chaos_ip_17:
    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]) | chaos_managed_pointer_local_slot_tag;
    goto chaos_ip_19;

chaos_ip_19:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_24;

chaos_ip_24:
    chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_25;

chaos_ip_25:
    goto chaos_ip_27;

chaos_ip_27:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];
    goto chaos_ip_28;

chaos_ip_28:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method9()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method9(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 11) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsValueType_System_Boolean__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            goto chaos_ip_21;
        }
    }
    goto chaos_ip_18;

chaos_ip_18:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_19;

chaos_ip_19:
    goto chaos_ip_22;

chaos_ip_21:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
    goto chaos_ip_22;

chaos_ip_22:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_23;

chaos_ip_23:
    goto chaos_ip_25;

chaos_ip_25:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_26;

chaos_ip_26:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method10()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method10(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 11) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsEnum_System_Boolean__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            goto chaos_ip_21;
        }
    }
    goto chaos_ip_18;

chaos_ip_18:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_19;

chaos_ip_19:
    goto chaos_ip_22;

chaos_ip_21:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
    goto chaos_ip_22;

chaos_ip_22:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_23;

chaos_ip_23:
    goto chaos_ip_25;

chaos_ip_25:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_26;

chaos_ip_26:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method11()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method11(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 11) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsAbstract_System_Boolean__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            goto chaos_ip_21;
        }
    }
    goto chaos_ip_18;

chaos_ip_18:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_19;

chaos_ip_19:
    goto chaos_ip_22;

chaos_ip_21:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
    goto chaos_ip_22;

chaos_ip_22:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_23;

chaos_ip_23:
    goto chaos_ip_25;

chaos_ip_25:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_26;

chaos_ip_26:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method12()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method12(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 11) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsSealed_System_Boolean__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            goto chaos_ip_21;
        }
    }
    goto chaos_ip_18;

chaos_ip_18:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_19;

chaos_ip_19:
    goto chaos_ip_22;

chaos_ip_21:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
    goto chaos_ip_22;

chaos_ip_22:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_23;

chaos_ip_23:
    goto chaos_ip_25;

chaos_ip_25:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_26;

chaos_ip_26:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method13()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method13(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 11) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsInterface_System_Boolean__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            goto chaos_ip_21;
        }
    }
    goto chaos_ip_18;

chaos_ip_18:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_19;

chaos_ip_19:
    goto chaos_ip_22;

chaos_ip_21:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
    goto chaos_ip_22;

chaos_ip_22:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_23;

chaos_ip_23:
    goto chaos_ip_25;

chaos_ip_25:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_26;

chaos_ip_26:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method14()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method14(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 11) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsArray_System_Boolean__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            goto chaos_ip_21;
        }
    }
    goto chaos_ip_18;

chaos_ip_18:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_19;

chaos_ip_19:
    goto chaos_ip_22;

chaos_ip_21:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
    goto chaos_ip_22;

chaos_ip_22:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_23;

chaos_ip_23:
    goto chaos_ip_25;

chaos_ip_25:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_26;

chaos_ip_26:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method15()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method15(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 11) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsByRef_System_Boolean__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            goto chaos_ip_21;
        }
    }
    goto chaos_ip_18;

chaos_ip_18:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_19;

chaos_ip_19:
    goto chaos_ip_22;

chaos_ip_21:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
    goto chaos_ip_22;

chaos_ip_22:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_23;

chaos_ip_23:
    goto chaos_ip_25;

chaos_ip_25:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_26;

chaos_ip_26:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method16()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method16(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 11) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsPointer_System_Boolean__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            goto chaos_ip_21;
        }
    }
    goto chaos_ip_18;

chaos_ip_18:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_19;

chaos_ip_19:
    goto chaos_ip_22;

chaos_ip_21:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
    goto chaos_ip_22;

chaos_ip_22:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_23;

chaos_ip_23:
    goto chaos_ip_25;

chaos_ip_25:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_26;

chaos_ip_26:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method17()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method17(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 11) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsGenericType_System_Boolean__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            goto chaos_ip_21;
        }
    }
    goto chaos_ip_18;

chaos_ip_18:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_19;

chaos_ip_19:
    goto chaos_ip_22;

chaos_ip_21:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
    goto chaos_ip_22;

chaos_ip_22:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_23;

chaos_ip_23:
    goto chaos_ip_25;

chaos_ip_25:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_26;

chaos_ip_26:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method18()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method18(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 11) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsConstructedGenericType_System_Boolean__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            goto chaos_ip_21;
        }
    }
    goto chaos_ip_18;

chaos_ip_18:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_19;

chaos_ip_19:
    goto chaos_ip_22;

chaos_ip_21:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
    goto chaos_ip_22;

chaos_ip_22:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_23;

chaos_ip_23:
    goto chaos_ip_25;

chaos_ip_25:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_26;

chaos_ip_26:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method19()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method19(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 11) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNested_System_Boolean__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            goto chaos_ip_21;
        }
    }
    goto chaos_ip_18;

chaos_ip_18:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_19;

chaos_ip_19:
    goto chaos_ip_22;

chaos_ip_21:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
    goto chaos_ip_22;

chaos_ip_22:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_23;

chaos_ip_23:
    goto chaos_ip_25;

chaos_ip_25:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_26;

chaos_ip_26:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method20()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method20(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 11) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsPublic_System_Boolean__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            goto chaos_ip_21;
        }
    }
    goto chaos_ip_18;

chaos_ip_18:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_19;

chaos_ip_19:
    goto chaos_ip_22;

chaos_ip_21:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
    goto chaos_ip_22;

chaos_ip_22:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_23;

chaos_ip_23:
    goto chaos_ip_25;

chaos_ip_25:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_26;

chaos_ip_26:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method21()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method21(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 11) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNotPublic_System_Boolean__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            goto chaos_ip_21;
        }
    }
    goto chaos_ip_18;

chaos_ip_18:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_19;

chaos_ip_19:
    goto chaos_ip_22;

chaos_ip_21:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
    goto chaos_ip_22;

chaos_ip_22:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_23;

chaos_ip_23:
    goto chaos_ip_25;

chaos_ip_25:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_26;

chaos_ip_26:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method22()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method22(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 11) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNestedPublic_System_Boolean__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            goto chaos_ip_21;
        }
    }
    goto chaos_ip_18;

chaos_ip_18:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_19;

chaos_ip_19:
    goto chaos_ip_22;

chaos_ip_21:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
    goto chaos_ip_22;

chaos_ip_22:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_23;

chaos_ip_23:
    goto chaos_ip_25;

chaos_ip_25:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_26;

chaos_ip_26:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method23()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method23(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 11) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNestedPrivate_System_Boolean__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            goto chaos_ip_21;
        }
    }
    goto chaos_ip_18;

chaos_ip_18:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_19;

chaos_ip_19:
    goto chaos_ip_22;

chaos_ip_21:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
    goto chaos_ip_22;

chaos_ip_22:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_23;

chaos_ip_23:
    goto chaos_ip_25;

chaos_ip_25:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_26;

chaos_ip_26:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method24()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method24(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_BaseType_System_Type__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
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

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method25()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method25(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_DeclaringType_System_Type__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
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

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method26()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method26(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_16;

chaos_ip_16:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
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

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method27()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method27(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_UnderlyingSystemType_System_Type__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
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

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method28()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method28(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethods_System_Reflection_MethodInfo____(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
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

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method29()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method29(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 9) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(4);
    goto chaos_ip_12;

chaos_ip_12:
    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethods_System_Reflection_MethodInfo___System_Reflection_BindingFlags_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_17;

chaos_ip_17:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_22;

chaos_ip_22:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_23;

chaos_ip_23:
    goto chaos_ip_25;

chaos_ip_25:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_26;

chaos_ip_26:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method30()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method30(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetFields_System_Reflection_FieldInfo____(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
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

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method31()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method31(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 9) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(4);
    goto chaos_ip_12;

chaos_ip_12:
    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetFields_System_Reflection_FieldInfo___System_Reflection_BindingFlags_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_17;

chaos_ip_17:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_22;

chaos_ip_22:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_23;

chaos_ip_23:
    goto chaos_ip_25;

chaos_ip_25:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_26;

chaos_ip_26:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method32()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method32(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_16;

chaos_ip_16:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
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

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method33()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method33(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 9) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(4);
    goto chaos_ip_12;

chaos_ip_12:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_17;

chaos_ip_17:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_22;

chaos_ip_22:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_23;

chaos_ip_23:
    goto chaos_ip_25;

chaos_ip_25:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_26;

chaos_ip_26:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method34()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method34(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_16;

chaos_ip_16:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
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

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method35()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method35(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_16;

chaos_ip_16:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
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

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method36()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method36(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 9) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(4);
    goto chaos_ip_12;

chaos_ip_12:
    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetConstructors_System_Reflection_ConstructorInfo___System_Reflection_BindingFlags_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_17;

chaos_ip_17:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_22;

chaos_ip_22:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_23;

chaos_ip_23:
    goto chaos_ip_25;

chaos_ip_25:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_26;

chaos_ip_26:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method37()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method37(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetMembers_System_Reflection_MemberInfo____(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
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

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method38()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method38(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetInterfaces_System_Type____(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
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

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method39()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method39(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetElementType_System_Type__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
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

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method40()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method40(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetNestedTypes_System_Type____(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
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

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method41()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method41(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 9) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_id = chaos_type_id_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(2);
        chaos_string->utf8_data = "99";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }
    goto chaos_ip_16;

chaos_ip_16:
    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetNestedType_System_Type_System_String_(chaos_arg_0, chaos_arg_1);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_21;

chaos_ip_21:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
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

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Method42()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method42(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_6;

chaos_ip_6:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_11;

chaos_ip_11:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_Assembly_System_Reflection_Assembly__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
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

// Managed method: ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0)
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method0();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method1();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method2();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method3();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method4();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method5();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method6();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method7();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method8();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method9();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method10();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method11();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method12();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method13();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method14();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method15();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method16();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method17();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method18();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method19();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method20();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method21();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method22();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method23();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method24();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method25();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method26();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method27();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method28();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method29();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method30();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method31();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method32();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method33();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method34();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method35();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method36();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method37();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method38();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method39();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method40();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method41();
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
        const auto chaos_result = ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method42();
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

// Native AOT entry for ReflectionTypeSemanticPatchEntry/ReflectionTypeSemanticPatchEntry::Run:System.Int32(System.Int32)
// Managed symbol: ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Run
extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 chaos_entry_index)
{
    return ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Run(chaos_entry_index);
}