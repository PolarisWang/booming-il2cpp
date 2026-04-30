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
constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Type = 4;

CHAOS_IL2CPP_INTPTR chaos_get_base_type_id(CHAOS_IL2CPP_INTPTR chaos_type_id) noexcept
{
    switch (chaos_type_id)
    {
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
        case static_cast<CHAOS_IL2CPP_INTPTR>(37708852u):
            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method0") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(109890971u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method1") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(114256562u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method2") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(102707921u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method3") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(104799184u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method4") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(114800095u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method5") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(107618262u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Run") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(111700188u);
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

extern "C" CHAOS_IL2CPP_INT32 ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);


// StringId constants for compile-time-known string literals.
constexpr CHAOS_IL2CPP_UINT64 chaos_string_id_08140B07B4D19DF7 = 582102379563949559U;

// AOT-baked string table: sorted by StringId for binary search at runtime.
constexpr chaos::il2cpp::string_table::StringEntry chaos_aot_string_entries[] = {
    { chaos_string_id_08140B07B4D19DF7, "99", 2u },
};

constexpr CHAOS_IL2CPP_UINT32 chaos_aot_string_entry_count = sizeof(chaos_aot_string_entries) / sizeof(chaos_aot_string_entries[0]);

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Reflection_Assembly__GetModules_System_Reflection_Module____(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return chaos_reflection_create_reference_array(
        chaos_type_id_System_Private_CoreLib_System_Reflection_Module,
        static_cast<CHAOS_IL2CPP_SIZE>(0));
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return chaos_reflection_get_type_from_handle(chaos_arg_0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_Assembly_System_Reflection_Assembly__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return chaos_reflection_get_assembly(chaos_arg_0);
}
}

extern "C" CHAOS_IL2CPP_INT32 ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);

// Managed method: ReflectionModuleSemanticPatchEntry/ReflectionModuleSemanticPatchEntry::Method0()
extern "C" CHAOS_IL2CPP_INT32 ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Method0(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 17) chaos_eval_stack{};
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
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_Assembly__GetModules_System_Reflection_Module____(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_21;

chaos_ip_21:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_22;

chaos_ip_22:
    {
        const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
        auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack[--chaos_stack_top]);
        if (chaos_array == nullptr)
        {
            CHAOS_IL2CPP_ABORT();
        }
        if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
        {
            CHAOS_IL2CPP_ABORT();
        }
        chaos_eval_stack[chaos_stack_top++] = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
    }
    goto chaos_ip_23;

chaos_ip_23:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_28;

chaos_ip_28:
    chaos_eval_stack[chaos_stack_top] = chaos_eval_stack[chaos_stack_top - 1];
    chaos_stack_top++;
    goto chaos_ip_29;

chaos_ip_29:
    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            goto chaos_ip_35;
        }
    }
    goto chaos_ip_31;

chaos_ip_31:
    chaos_stack_top--;
    goto chaos_ip_32;

chaos_ip_32:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_33;

chaos_ip_33:
    goto chaos_ip_40;

chaos_ip_35:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_40;

chaos_ip_40:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_41;

chaos_ip_41:
    goto chaos_ip_43;

chaos_ip_43:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_44;

chaos_ip_44:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionModuleSemanticPatchEntry/ReflectionModuleSemanticPatchEntry::Method1()
extern "C" CHAOS_IL2CPP_INT32 ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Method1(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 17) chaos_eval_stack{};
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
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_Assembly__GetModules_System_Reflection_Module____(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_21;

chaos_ip_21:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_22;

chaos_ip_22:
    {
        const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
        auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack[--chaos_stack_top]);
        if (chaos_array == nullptr)
        {
            CHAOS_IL2CPP_ABORT();
        }
        if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
        {
            CHAOS_IL2CPP_ABORT();
        }
        chaos_eval_stack[chaos_stack_top++] = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
    }
    goto chaos_ip_23;

chaos_ip_23:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_28;

chaos_ip_28:
    chaos_eval_stack[chaos_stack_top] = chaos_eval_stack[chaos_stack_top - 1];
    chaos_stack_top++;
    goto chaos_ip_29;

chaos_ip_29:
    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            goto chaos_ip_35;
        }
    }
    goto chaos_ip_31;

chaos_ip_31:
    chaos_stack_top--;
    goto chaos_ip_32;

chaos_ip_32:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_33;

chaos_ip_33:
    goto chaos_ip_40;

chaos_ip_35:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_40;

chaos_ip_40:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_41;

chaos_ip_41:
    goto chaos_ip_43;

chaos_ip_43:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_44;

chaos_ip_44:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionModuleSemanticPatchEntry/ReflectionModuleSemanticPatchEntry::Method2()
extern "C" CHAOS_IL2CPP_INT32 ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Method2(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 12) chaos_eval_stack{};
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
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_Assembly__GetModules_System_Reflection_Module____(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_21;

chaos_ip_21:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_22;

chaos_ip_22:
    {
        const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
        auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack[--chaos_stack_top]);
        if (chaos_array == nullptr)
        {
            CHAOS_IL2CPP_ABORT();
        }
        if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
        {
            CHAOS_IL2CPP_ABORT();
        }
        chaos_eval_stack[chaos_stack_top++] = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
    }
    goto chaos_ip_23;

chaos_ip_23:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_28;

chaos_ip_28:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_33;

chaos_ip_33:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_34;

chaos_ip_34:
    goto chaos_ip_36;

chaos_ip_36:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_37;

chaos_ip_37:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionModuleSemanticPatchEntry/ReflectionModuleSemanticPatchEntry::Method3()
extern "C" CHAOS_IL2CPP_INT32 ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Method3(void)
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
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_Assembly_System_Reflection_Assembly__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_16;

chaos_ip_16:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_Assembly__GetModules_System_Reflection_Module____(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_21;

chaos_ip_21:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_22;

chaos_ip_22:
    {
        const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
        auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack[--chaos_stack_top]);
        if (chaos_array == nullptr)
        {
            CHAOS_IL2CPP_ABORT();
        }
        if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
        {
            CHAOS_IL2CPP_ABORT();
        }
        chaos_eval_stack[chaos_stack_top++] = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
    }
    goto chaos_ip_23;

chaos_ip_23:
    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_id = chaos_type_id_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(2);
        chaos_string->utf8_data = "99";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }
    goto chaos_ip_28;

chaos_ip_28:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_33;

chaos_ip_33:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_38;

chaos_ip_38:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_39;

chaos_ip_39:
    goto chaos_ip_41;

chaos_ip_41:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_42;

chaos_ip_42:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionModuleSemanticPatchEntry/ReflectionModuleSemanticPatchEntry::Method4()
extern "C" CHAOS_IL2CPP_INT32 ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Method4(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 12) chaos_eval_stack{};
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
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_Assembly__GetModules_System_Reflection_Module____(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_21;

chaos_ip_21:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_22;

chaos_ip_22:
    {
        const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
        auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack[--chaos_stack_top]);
        if (chaos_array == nullptr)
        {
            CHAOS_IL2CPP_ABORT();
        }
        if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
        {
            CHAOS_IL2CPP_ABORT();
        }
        chaos_eval_stack[chaos_stack_top++] = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
    }
    goto chaos_ip_23;

chaos_ip_23:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_28;

chaos_ip_28:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_33;

chaos_ip_33:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_34;

chaos_ip_34:
    goto chaos_ip_36;

chaos_ip_36:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_37;

chaos_ip_37:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionModuleSemanticPatchEntry/ReflectionModuleSemanticPatchEntry::Method5()
extern "C" CHAOS_IL2CPP_INT32 ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Method5(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 14) chaos_eval_stack{};
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
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_Assembly__GetModules_System_Reflection_Module____(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_21;

chaos_ip_21:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_22;

chaos_ip_22:
    {
        const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
        auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack[--chaos_stack_top]);
        if (chaos_array == nullptr)
        {
            CHAOS_IL2CPP_ABORT();
        }
        if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
        {
            CHAOS_IL2CPP_ABORT();
        }
        chaos_eval_stack[chaos_stack_top++] = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
    }
    goto chaos_ip_23;

chaos_ip_23:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(40745025u);
    goto chaos_ip_28;

chaos_ip_28:
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_33;

chaos_ip_33:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_38;

chaos_ip_38:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_43;

chaos_ip_43:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_44;

chaos_ip_44:
    goto chaos_ip_46;

chaos_ip_46:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_47;

chaos_ip_47:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionModuleSemanticPatchEntry/ReflectionModuleSemanticPatchEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 30) chaos_eval_stack{};
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
                goto chaos_ip_37;
            case 1:
                goto chaos_ip_45;
            case 2:
                goto chaos_ip_53;
            case 3:
                goto chaos_ip_61;
            case 4:
                goto chaos_ip_69;
            case 5:
                goto chaos_ip_77;
            default:
                goto chaos_ip_35;
        }
    }

chaos_ip_35:
    goto chaos_ip_85;

chaos_ip_37:
    {
        const auto chaos_result = ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Method0();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_42;

chaos_ip_42:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_43;

chaos_ip_43:
    goto chaos_ip_89;

chaos_ip_45:
    {
        const auto chaos_result = ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Method1();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_50;

chaos_ip_50:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_51;

chaos_ip_51:
    goto chaos_ip_89;

chaos_ip_53:
    {
        const auto chaos_result = ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Method2();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_58;

chaos_ip_58:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_59;

chaos_ip_59:
    goto chaos_ip_89;

chaos_ip_61:
    {
        const auto chaos_result = ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Method3();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_66;

chaos_ip_66:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_67;

chaos_ip_67:
    goto chaos_ip_89;

chaos_ip_69:
    {
        const auto chaos_result = ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Method4();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_74;

chaos_ip_74:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_75;

chaos_ip_75:
    goto chaos_ip_89;

chaos_ip_77:
    {
        const auto chaos_result = ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Method5();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_82;

chaos_ip_82:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_83;

chaos_ip_83:
    goto chaos_ip_89;

chaos_ip_85:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
    goto chaos_ip_86;

chaos_ip_86:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_87;

chaos_ip_87:
    goto chaos_ip_89;

chaos_ip_89:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[2];
    goto chaos_ip_90;

chaos_ip_90:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Native AOT entry for ReflectionModuleSemanticPatchEntry/ReflectionModuleSemanticPatchEntry::Run:System.Int32(System.Int32)
// Managed symbol: ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Run
extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 chaos_entry_index)
{
    return ReflectionModuleSemanticPatchEntry_ReflectionModuleSemanticPatchEntry_Run(chaos_entry_index);
}