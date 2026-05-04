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

inline constexpr TypeInfo chaos_type_info_System_Private_CoreLib_System_String = { nullptr, 1782325859292956794ULL, 1 /* reference */ };
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_INTPTR>(1782325859292956794ULL);

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
        case chaos_type_id_System_Private_CoreLib_System_String:
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

struct chaos_type_System_Private_CoreLib_System_String
{
    chaos_object_header header{};
    CHAOS_IL2CPP_INTPTR length = 0;
    const char* utf8_data = nullptr;
    CHAOS_IL2CPP_UINT64 string_id = 0u;  // stable StringId, 0 = not yet materialized
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
    chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
    chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_view.byte_count);
    auto* owned_utf8 = static_cast<char*>(chaos::il2cpp::runtime_core::GcAllocateAtomic(chaos_view.byte_count + 1));
    CHAOS_IL2CPP_MEMCPY(owned_utf8, chaos_view.utf8_data, chaos_view.byte_count);
    owned_utf8[chaos_view.byte_count] = '\0';
    chaos_string->utf8_data = owned_utf8;
    chaos_string->string_id = chaos_id;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
}

extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method14(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method15(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method16(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method17(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method18(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method19(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);


// StringId constants for compile-time-known string literals.
constexpr CHAOS_IL2CPP_UINT64 chaos_string_id_2430D84680AABD0B = 2607821981565500683U;
constexpr CHAOS_IL2CPP_UINT64 chaos_string_id_4F59FF5E730C8AF3 = 5717881983045765875U;

// AOT-baked string table: sorted by StringId for binary search at runtime.
constexpr chaos::il2cpp::string_table::StringEntry chaos_aot_string_entries[] = {
    { chaos_string_id_2430D84680AABD0B, "hello", 5u },
    { chaos_string_id_4F59FF5E730C8AF3, "world", 5u },
};

constexpr CHAOS_IL2CPP_UINT32 chaos_aot_string_entry_count = sizeof(chaos_aot_string_entries) / sizeof(chaos_aot_string_entries[0]);

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Array__Empty_System_Object__System_Object____()
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Array__Empty_System_String__System_String____()
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Char__IsDigit_System_Boolean_System_Char_(CHAOS_IL2CPP_INT32 chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INT32>(0);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Char__IsLetter_System_Boolean_System_Char_(CHAOS_IL2CPP_INT32 chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INT32>(0);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Char__IsWhiteSpace_System_Boolean_System_Char_(CHAOS_IL2CPP_INT32 chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INT32>(0);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_String__Compare_System_Int32_System_String_System_Int32_System_String_System_Int32_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2, CHAOS_IL2CPP_INTPTR chaos_arg_3, CHAOS_IL2CPP_INTPTR chaos_arg_4)
{
    (void)chaos_arg_0; (void)chaos_arg_1; (void)chaos_arg_2; (void)chaos_arg_3; (void)chaos_arg_4;
    return static_cast<CHAOS_IL2CPP_INT32>(0);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_String__Compare_System_Int32_System_String_System_String_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
    (void)chaos_arg_0; (void)chaos_arg_1;
    return static_cast<CHAOS_IL2CPP_INT32>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_String__Concat_System_String_System_String_System_String_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
    return chaos_reflection_concat_string_pair_values(chaos_arg_0, chaos_arg_1);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_String__Contains_System_Boolean_System_String_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
    return chaos_string_contains(chaos_arg_0, chaos_arg_1);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_String__Format_System_String_System_String_System_Object___(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
    (void)chaos_arg_0; (void)chaos_arg_1;
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_String__IndexOf_System_Int32_System_Char_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
    (void)chaos_arg_0; (void)chaos_arg_1;
    return static_cast<CHAOS_IL2CPP_INT32>(0);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_String__IndexOf_System_Int32_System_String_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
    (void)chaos_arg_0; (void)chaos_arg_1;
    return static_cast<CHAOS_IL2CPP_INT32>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_String__Join_System_String_System_String_System_String___(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
    return chaos_string_join_ss(chaos_arg_0, chaos_arg_1);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_String__Replace_System_String_System_String_System_String_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2)
{
    (void)chaos_arg_0; (void)chaos_arg_1; (void)chaos_arg_2;
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_String__Split_System_String___System_Char_System_StringSplitOptions_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2)
{
    (void)chaos_arg_0; (void)chaos_arg_1; (void)chaos_arg_2;
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_String__StartsWith_System_Boolean_System_String_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
    return chaos_string_starts_with(chaos_arg_0, chaos_arg_1);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_String__Substring_System_String_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
    (void)chaos_arg_0; (void)chaos_arg_1;
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_String__Substring_System_String_System_Int32_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2)
{
    (void)chaos_arg_0; (void)chaos_arg_1; (void)chaos_arg_2;
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_String__ToLower_System_String__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_String__ToUpper_System_String__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_String__Trim_System_String__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        CHAOS_IL2CPP_ABORT();
    }

    if (chaos_is_string_id(chaos_arg_0))
    {
        return static_cast<CHAOS_IL2CPP_INT32>(
            chaos::il2cpp::string_table::Resolve(
                chaos_extract_string_id(chaos_arg_0)).byte_count);
    }

    auto* chaos_string = reinterpret_cast<CHAOS_IL2CPP_STRING_TYPE*>(chaos_arg_0);
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_string->length);
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

extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method14(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method15(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method16(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method17(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method18(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method19(void);
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);

// Managed method: StringCharTextCoreNativeEntry/StringCharTextCoreNativeEntry::Method0()
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method0(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__IndexOf_System_Int32_System_String_(chaos_arg_0, chaos_arg_1);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_19;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: StringCharTextCoreNativeEntry/StringCharTextCoreNativeEntry::Method1()
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method1(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(65);

    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__IndexOf_System_Int32_System_Char_(chaos_arg_0, chaos_arg_1);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_16;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: StringCharTextCoreNativeEntry/StringCharTextCoreNativeEntry::Method2()
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method2(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);

    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__Substring_System_String_System_Int32_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_20;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: StringCharTextCoreNativeEntry/StringCharTextCoreNativeEntry::Method3()
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method3(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 9> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(2);

    {
        const auto chaos_raw_arg_2 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_2 = chaos_raw_arg_2;
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__Substring_System_String_System_Int32_System_Int32_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2));
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_21;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: StringCharTextCoreNativeEntry/StringCharTextCoreNativeEntry::Method4()
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method4(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__Compare_System_Int32_System_String_System_String_(chaos_arg_0, chaos_arg_1);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_19;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: StringCharTextCoreNativeEntry/StringCharTextCoreNativeEntry::Method5()
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method5(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "world";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(3);

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
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__Compare_System_Int32_System_String_System_Int32_System_String_System_Int32_System_Int32_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1), chaos_arg_2, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_4));
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_22;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: StringCharTextCoreNativeEntry/StringCharTextCoreNativeEntry::Method6()
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method6(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__Concat_System_String_System_String_System_String_(chaos_arg_0, chaos_arg_1);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_24;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: StringCharTextCoreNativeEntry/StringCharTextCoreNativeEntry::Method7()
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method7(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__Concat_System_String_System_String_System_String_(chaos_arg_0, chaos_arg_1);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__Concat_System_String_System_String_System_String_(chaos_arg_0, chaos_arg_1);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_29;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: StringCharTextCoreNativeEntry/StringCharTextCoreNativeEntry::Method8()
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method8(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
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
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__Format_System_String_System_String_System_Object___(chaos_arg_0, chaos_arg_1);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_24;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: StringCharTextCoreNativeEntry/StringCharTextCoreNativeEntry::Method9()
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method9(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 11> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__StartsWith_System_Boolean_System_String_(chaos_arg_0, chaos_arg_1);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_22;
        }
        else
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_22;
        }
    }
    goto chaos_ip_22;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: StringCharTextCoreNativeEntry/StringCharTextCoreNativeEntry::Method10()
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method10(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 11> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__Contains_System_Boolean_System_String_(chaos_arg_0, chaos_arg_1);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_22;
        }
        else
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_22;
        }
    }
    goto chaos_ip_22;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: StringCharTextCoreNativeEntry/StringCharTextCoreNativeEntry::Method11()
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method11(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 9> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    {
        const auto chaos_raw_arg_2 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_2 = chaos_raw_arg_2;
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__Replace_System_String_System_String_System_String_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_29;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: StringCharTextCoreNativeEntry/StringCharTextCoreNativeEntry::Method12()
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method12(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(65);

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    {
        const auto chaos_raw_arg_2 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_2 = chaos_raw_arg_2;
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__Split_System_String___System_Char_System_StringSplitOptions_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2));
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack[--chaos_stack_top]);
        if (chaos_array == nullptr)
        {
            CHAOS_IL2CPP_ABORT();
        }
        chaos_eval_stack[chaos_stack_top++] = chaos_array->length;
    }

    chaos_eval_stack[chaos_stack_top - 1] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[chaos_stack_top - 1]));

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_19;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: StringCharTextCoreNativeEntry/StringCharTextCoreNativeEntry::Method13()
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method13(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__ToUpper_System_String__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_19;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: StringCharTextCoreNativeEntry/StringCharTextCoreNativeEntry::Method14()
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method14(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__ToLower_System_String__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_19;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: StringCharTextCoreNativeEntry/StringCharTextCoreNativeEntry::Method15()
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method15(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__Trim_System_String__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_19;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: StringCharTextCoreNativeEntry/StringCharTextCoreNativeEntry::Method16()
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method16(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    // Generic execution authority: definition=System.Private.CoreLib/System.Array::Empty:!!0[](); type=[]; method=[System.String]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Private.CoreLib/System.Array::Empty:!!0[]();type=[];method=[System.String]; stub=stub:definition=System.Private.CoreLib/System.Array::Empty:!!0[]();type=[];method=[System.String]
    {
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Array__Empty_System_String__System_String____();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__Join_System_String_System_String_System_String___(chaos_arg_0, chaos_arg_1);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_24;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: StringCharTextCoreNativeEntry/StringCharTextCoreNativeEntry::Method17()
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method17(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(65);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Char__IsDigit_System_Boolean_System_Char_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_14;
        }
        else
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_14;
        }
    }
    goto chaos_ip_14;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: StringCharTextCoreNativeEntry/StringCharTextCoreNativeEntry::Method18()
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method18(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(65);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Char__IsLetter_System_Boolean_System_Char_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_14;
        }
        else
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_14;
        }
    }
    goto chaos_ip_14;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: StringCharTextCoreNativeEntry/StringCharTextCoreNativeEntry::Method19()
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method19(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(65);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Char__IsWhiteSpace_System_Boolean_System_Char_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_14;
        }
        else
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_14;
        }
    }
    goto chaos_ip_14;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: StringCharTextCoreNativeEntry/StringCharTextCoreNativeEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 72> chaos_eval_stack{};
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
                    const auto chaos_result = StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method0();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_268;
            }
            case 1:
            {
                {
                    const auto chaos_result = StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method1();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_268;
            }
            case 2:
            {
                {
                    const auto chaos_result = StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method2();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_268;
            }
            case 3:
            {
                {
                    const auto chaos_result = StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method3();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_268;
            }
            case 4:
            {
                {
                    const auto chaos_result = StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method4();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_268;
            }
            case 5:
            {
                {
                    const auto chaos_result = StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method5();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_268;
            }
            case 6:
            {
                {
                    const auto chaos_result = StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method6();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_268;
            }
            case 7:
            {
                {
                    const auto chaos_result = StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method7();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_268;
            }
            case 8:
            {
                {
                    const auto chaos_result = StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method8();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_268;
            }
            case 9:
            {
                {
                    const auto chaos_result = StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method9();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_268;
            }
            case 10:
            {
                {
                    const auto chaos_result = StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method10();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_268;
            }
            case 11:
            {
                {
                    const auto chaos_result = StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method11();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_268;
            }
            case 12:
            {
                {
                    const auto chaos_result = StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method12();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_268;
            }
            case 13:
            {
                {
                    const auto chaos_result = StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method13();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_268;
            }
            case 14:
            {
                {
                    const auto chaos_result = StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method14();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_268;
            }
            case 15:
            {
                {
                    const auto chaos_result = StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method15();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_268;
            }
            case 16:
            {
                {
                    const auto chaos_result = StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method16();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_268;
            }
            case 17:
            {
                {
                    const auto chaos_result = StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method17();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_268;
            }
            case 18:
            {
                {
                    const auto chaos_result = StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method18();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_268;
            }
            case 19:
            {
                {
                    const auto chaos_result = StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Method19();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_268;
            }
            default:
            {
                goto chaos_ip_268;
            }
        }
    }
    goto chaos_ip_268;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[2];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Native AOT entry for StringCharTextCoreNativeEntry/StringCharTextCoreNativeEntry::Run:System.Int32(System.Int32)
// Managed symbol: StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Run
extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 chaos_entry_index)
{
    return StringCharTextCoreNativeEntry_StringCharTextCoreNativeEntry_Run(chaos_entry_index);
}