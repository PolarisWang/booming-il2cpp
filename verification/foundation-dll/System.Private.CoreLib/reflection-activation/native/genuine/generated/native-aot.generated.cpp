#include <chaos/common.h>
#include "runtime_core.h"
#include "codegen_bridge.h"

namespace
{
#include <chaos/common.h>

#include <chaos/type_info.h>

struct chaos_object_header
{
    const TypeInfo* type_info = nullptr;
};

constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_managed_array = 1;
inline constexpr TypeInfo chaos_type_info_managed_array = { nullptr, 1ULL, 2 };

struct chaos_managed_array
{
    chaos_object_header header{};
    CHAOS_IL2CPP_UINT8 element_type_shape = 0;
    const TypeInfo* element_type_info = nullptr;
    CHAOS_IL2CPP_INTPTR length = 0;
    CHAOS_IL2CPP_INTPTR* elements = nullptr;
};

constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_reference = 1;
constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_value = 2;
constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_interface = 3;

constexpr CHAOS_IL2CPP_INTPTR chaos_managed_pointer_local_slot_tag = 1;

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

inline constexpr TypeInfo chaos_type_info_System_Private_CoreLib_System_Object = { nullptr, 15228727185366376748ULL, 1 /* reference */ };
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
inline constexpr TypeInfo chaos_type_info_System_Private_CoreLib_System_String = { nullptr, 1782325859292956794ULL, 1 /* reference */ };
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_INTPTR>(1782325859292956794ULL);
inline constexpr TypeInfo chaos_type_info_System_Private_CoreLib_System_Type = { &chaos_type_info_System_Private_CoreLib_System_Object, 3222174513575444759ULL, 1 /* reference */ };
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Type = static_cast<CHAOS_IL2CPP_INTPTR>(3222174513575444759ULL);

inline const TypeInfo* chaos_get_parent_type_info(const TypeInfo* chaos_ti) noexcept
{
    if (chaos_ti == nullptr) return nullptr;
    return chaos_ti->parent;
}

bool chaos_is_type_compatible(const TypeInfo* chaos_actual_type_info, const TypeInfo* chaos_target_type_info) noexcept
{
    auto* chaos_current = chaos_actual_type_info;
    while (chaos_current != nullptr)
    {
        if (chaos_current == chaos_target_type_info)
        {
            return true;
        }

        chaos_current = chaos_current->parent;
    }

    return false;
}

bool chaos_type_implements_interface(const TypeInfo* chaos_actual_type_info, const TypeInfo* chaos_target_interface_type_info) noexcept
{
    switch (chaos_actual_type_info->stable_id)
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

bool chaos_does_type_implement_interface(const TypeInfo* chaos_actual_type_info, const TypeInfo* chaos_target_interface_type_info) noexcept
{
    auto* chaos_current = chaos_actual_type_info;
    while (chaos_current != nullptr)
    {
        if (chaos_type_implements_interface(chaos_current, chaos_target_interface_type_info))
        {
            return true;
        }

        chaos_current = chaos_current->parent;
    }

    return false;
}

bool chaos_is_array_type_compatible(
    CHAOS_IL2CPP_UINT8 chaos_actual_element_shape,
    const TypeInfo* chaos_actual_element_type_info,
    CHAOS_IL2CPP_UINT8 chaos_target_element_shape,
    const TypeInfo* chaos_target_element_type_info) noexcept
{
    if (chaos_actual_element_shape == chaos_type_shape_reference)
    {
        if (chaos_target_element_shape == chaos_type_shape_reference)
        {
            return chaos_is_type_compatible(chaos_actual_element_type_info, chaos_target_element_type_info);
        }

        if (chaos_target_element_shape == chaos_type_shape_interface)
        {
            return chaos_does_type_implement_interface(chaos_actual_element_type_info, chaos_target_element_type_info);
        }

        return false;
    }

    return chaos_actual_element_shape == chaos_target_element_shape
        && chaos_actual_element_type_info == chaos_target_element_type_info;
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
        return chaos_array->element_type_shape == chaos_type_shape_reference            && chaos_is_type_compatible(&chaos_type_info_System_Private_CoreLib_System_String, chaos_array->element_type_info);
    }

    auto* chaos_header = reinterpret_cast<chaos_object_header*>(chaos_value);
    if (chaos_array->element_type_shape == chaos_type_shape_interface)
    {
        return chaos_does_type_implement_interface(chaos_header->type_info, chaos_array->element_type_info);
    }

    if (chaos_array->element_type_shape == chaos_type_shape_reference)
    {
        return chaos_is_type_compatible(chaos_header->type_info, chaos_array->element_type_info);
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

CHAOS_IL2CPP_INTPTR chaos_reflection_create_reference_array(CHAOS_IL2CPP_INTPTR chaos_element_type_id, CHAOS_IL2CPP_SIZE chaos_length)
{
    auto* chaos_array = new chaos_managed_array{};
    chaos_array->header.type_info = &chaos_type_info_managed_array;
    chaos_array->element_type_shape = chaos_type_shape_reference;
    chaos_array->element_type_info = chaos_element_type_info;
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

CHAOS_IL2CPP_INTPTR chaos_reflection_create_type_value(CHAOS_IL2CPP_INTPTR chaos_type_handle)
{
    auto* chaos_type = new chaos_type_System_Private_CoreLib_System_Type{};
    chaos_type->header.type_info = &chaos_type_info_System_Private_CoreLib_System_Type;
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

CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_from_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle)
{
    return chaos_reflection_create_type_value(chaos_type_handle);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_handle(CHAOS_IL2CPP_INTPTR chaos_type_value) noexcept
{
    auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_type_value);
    return chaos_type->runtime_type_handle;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_object_type(CHAOS_IL2CPP_INTPTR chaos_object_value)
{
    if (chaos_object_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }

    auto* chaos_header = reinterpret_cast<chaos_object_header*>(chaos_object_value);
    switch (chaos_header->type_info->stable_id)
    {
        default:
            return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_instance(CHAOS_IL2CPP_INTPTR chaos_type_value)
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

CHAOS_IL2CPP_INTPTR chaos_reflection_resolve_method_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle, const char* chaos_method_name) noexcept
{
    if (chaos_method_name == nullptr)
    {
        return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }

    switch (chaos_type_handle)
    {
        case static_cast<CHAOS_IL2CPP_INTPTR>(46487956u):
            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method0") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(114130363u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method1") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(104109522u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method2") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(108536049u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method3") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(108583024u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method4") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(107969535u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method5") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(110858102u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method6") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(113064837u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Method7") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(106549924u);
            }

            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Run") == 0)
            {
                return static_cast<CHAOS_IL2CPP_INTPTR>(107511164u);
            }

            break;
        default:
            break;
    }

    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_method(CHAOS_IL2CPP_INTPTR chaos_type_value, CHAOS_IL2CPP_INTPTR chaos_name_value, CHAOS_IL2CPP_INT32 chaos_binding_flags = CHAOS_IL2CPP_INT32(0))
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
    chaos_method->header.type_info = &chaos_type_info_System_Private_CoreLib_System_Reflection_MethodInfo;
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
    chaos_closed_method->header.type_info = &chaos_type_info_System_Private_CoreLib_System_Reflection_MethodInfo;
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

extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Activator__CreateInstance_System_Object_System_Type_(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Activator__CreateInstance_System_Object_System_Type_System_Object___(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
    (void)chaos_arg_0; (void)chaos_arg_1;
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Activator__CreateInstance_System_Object_System_Type_System_Reflection_BindingFlags_System_Reflection_Binder_System_Object___System_Globalization_CultureInfo_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2, CHAOS_IL2CPP_INTPTR chaos_arg_3, CHAOS_IL2CPP_INTPTR chaos_arg_4)
{
    (void)chaos_arg_0; (void)chaos_arg_1; (void)chaos_arg_2; (void)chaos_arg_3; (void)chaos_arg_4;
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Array__CreateInstance_System_Array_System_Type_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
    (void)chaos_arg_0; (void)chaos_arg_1;
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Array__CreateInstance_System_Array_System_Type_System_Int32_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2)
{
    (void)chaos_arg_0; (void)chaos_arg_1; (void)chaos_arg_2;
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Array__CreateInstance_System_Array_System_Type_System_Int32___(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
    (void)chaos_arg_0; (void)chaos_arg_1;
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Array__Empty_System_Int32__System_Int32____()
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Array__Empty_System_Object__System_Object____()
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INT32>(0xDEAD);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return chaos_reflection_get_type_from_handle(chaos_arg_0);
}

static constexpr uint32_t kGenericTypeArgTokens[1] = { 0 };
static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };
static constexpr uint32_t kGenericMethodArgTokens[1] = { 0 };
static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };
}

// Populate generic registration arrays from this TU.
static void ChaosDoPopulateGenericRegistration(
    uint32_t* out_type_count,
    const GenericTypeRegistrationEntryV0** out_type_entries,
    const uint32_t** out_type_args,
    uint32_t* out_type_arg_count,
    uint32_t* out_method_count,
    const GenericMethodRegistrationEntryV0** out_method_entries,
    const uint32_t** out_method_args,
    uint32_t* out_method_arg_count)
{
    *out_type_count = 0;
    *out_type_entries = kGenericTypeEntries;
    *out_type_args = kGenericTypeArgTokens;
    *out_type_arg_count = 0;
    *out_method_count = 0;
    *out_method_entries = kGenericMethodEntries;
    *out_method_args = kGenericMethodArgTokens;
    *out_method_arg_count = 0;
}

// Static initializer registers the callback.
extern "C" void (*g_chaos_populate_generic_registration)(
    uint32_t*, const GenericTypeRegistrationEntryV0**, const uint32_t**, uint32_t*,
    uint32_t*, const GenericMethodRegistrationEntryV0**, const uint32_t**, uint32_t*);
namespace {
struct ChaosGenericRegistrationInit {
    ChaosGenericRegistrationInit() {
        g_chaos_populate_generic_registration =
            &ChaosDoPopulateGenericRegistration;
    }
} g_chaos_reg_init;
}

extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);

// Managed method: ReflectionActivationNativeEntry/ReflectionActivationNativeEntry::Method0()
extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method0(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Activator__CreateInstance_System_Object_System_Type_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_24;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionActivationNativeEntry/ReflectionActivationNativeEntry::Method1()
extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method1(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 9> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    // Generic execution authority: definition=System.Private.CoreLib/System.Array::Empty:!!0[](); type=[]; method=[System.Object]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Private.CoreLib/System.Array::Empty:!!0[]();type=[];method=[System.Object]; stub=stub:definition=System.Private.CoreLib/System.Array::Empty:!!0[]();type=[];method=[System.Object]
    {
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Array__Empty_System_Object__System_Object____();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Activator__CreateInstance_System_Object_System_Type_System_Object___(chaos_arg_0, chaos_arg_1);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_29;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionActivationNativeEntry/ReflectionActivationNativeEntry::Method2()
extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method2(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_5;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionActivationNativeEntry/ReflectionActivationNativeEntry::Method3()
extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method3(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 12> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    // Generic execution authority: definition=System.Private.CoreLib/System.Array::Empty:!!0[](); type=[]; method=[System.Object]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Private.CoreLib/System.Array::Empty:!!0[]();type=[];method=[System.Object]; stub=stub:definition=System.Private.CoreLib/System.Array::Empty:!!0[]();type=[];method=[System.Object]
    {
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Array__Empty_System_Object__System_Object____();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    {
        const auto chaos_raw_arg_4 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_4 = chaos_raw_arg_4;
        const auto chaos_raw_arg_3 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_3 = chaos_raw_arg_3;
        const auto chaos_raw_arg_2 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_2 = chaos_raw_arg_2;
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Activator__CreateInstance_System_Object_System_Type_System_Reflection_BindingFlags_System_Reflection_Binder_System_Object___System_Globalization_CultureInfo_(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_32;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionActivationNativeEntry/ReflectionActivationNativeEntry::Method4()
extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method4(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 9> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);

    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Array__CreateInstance_System_Array_System_Type_System_Int32_(chaos_arg_0, chaos_arg_1);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_26;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionActivationNativeEntry/ReflectionActivationNativeEntry::Method5()
extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method5(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);

    {
        const auto chaos_raw_arg_2 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_2 = chaos_raw_arg_2;
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Array__CreateInstance_System_Array_System_Type_System_Int32_System_Int32_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_28;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionActivationNativeEntry/ReflectionActivationNativeEntry::Method6()
extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method6(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 9> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    // Generic execution authority: definition=System.Private.CoreLib/System.Array::Empty:!!0[](); type=[]; method=[System.Int32]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Private.CoreLib/System.Array::Empty:!!0[]();type=[];method=[System.Int32]; stub=stub:definition=System.Private.CoreLib/System.Array::Empty:!!0[]();type=[];method=[System.Int32]
    {
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Array__Empty_System_Int32__System_Int32____();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Array__CreateInstance_System_Array_System_Type_System_Int32___(chaos_arg_0, chaos_arg_1);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_29;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionActivationNativeEntry/ReflectionActivationNativeEntry::Method7()
extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method7(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_5;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionActivationNativeEntry/ReflectionActivationNativeEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 36> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;
    chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_arg_0);

    chaos_eval_stack[chaos_stack_top++] = chaos_args[0];

    chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    {
        const auto chaos_switch_value = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
        switch (chaos_switch_value)
        {
            case 0:
            {
                {
                    const auto chaos_result = ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method0();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_109;
            }
            case 1:
            {
                {
                    const auto chaos_result = ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method1();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_109;
            }
            case 2:
            {
                {
                    const auto chaos_result = ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method2();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_109;
            }
            case 3:
            {
                {
                    const auto chaos_result = ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method3();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_109;
            }
            case 4:
            {
                {
                    const auto chaos_result = ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method4();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_109;
            }
            case 5:
            {
                {
                    const auto chaos_result = ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method5();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_109;
            }
            case 6:
            {
                {
                    const auto chaos_result = ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method6();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_109;
            }
            case 7:
            {
                {
                    const auto chaos_result = ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Method7();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_109;
            }
            default:
            {
                goto chaos_ip_109;
            }
        }
    }
    goto chaos_ip_109;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[2];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Native AOT entry for ReflectionActivationNativeEntry/ReflectionActivationNativeEntry::Run:System.Int32(System.Int32)
// Managed symbol: ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Run
extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 chaos_entry_index)
{
    return ReflectionActivationNativeEntry_ReflectionActivationNativeEntry_Run(chaos_entry_index);
}