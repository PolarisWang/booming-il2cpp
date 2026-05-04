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

inline constexpr TypeInfo chaos_type_info_System_Private_CoreLib_System_ArgumentException = { nullptr, 4429271818216920541ULL, 1 /* reference */ };
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_ArgumentException = static_cast<CHAOS_IL2CPP_INTPTR>(4429271818216920541ULL);
inline constexpr TypeInfo chaos_type_info_System_Private_CoreLib_System_ArgumentNullException = { nullptr, 12400915656331929738ULL, 1 /* reference */ };
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_ArgumentNullException = static_cast<CHAOS_IL2CPP_INTPTR>(12400915656331929738ULL);
inline constexpr TypeInfo chaos_type_info_System_Private_CoreLib_System_ArgumentOutOfRangeException = { nullptr, 15763389239383405623ULL, 1 /* reference */ };
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_ArgumentOutOfRangeException = static_cast<CHAOS_IL2CPP_INTPTR>(15763389239383405623ULL);
inline constexpr TypeInfo chaos_type_info_System_Private_CoreLib_System_Exception = { nullptr, 10972282733316558392ULL, 1 /* reference */ };
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Exception = static_cast<CHAOS_IL2CPP_INTPTR>(10972282733316558392ULL);
inline constexpr TypeInfo chaos_type_info_System_Private_CoreLib_System_InvalidOperationException = { nullptr, 5529823529457257812ULL, 1 /* reference */ };
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_InvalidOperationException = static_cast<CHAOS_IL2CPP_INTPTR>(5529823529457257812ULL);
inline constexpr TypeInfo chaos_type_info_System_Private_CoreLib_System_NotImplementedException = { nullptr, 16853252435633719857ULL, 1 /* reference */ };
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_NotImplementedException = static_cast<CHAOS_IL2CPP_INTPTR>(16853252435633719857ULL);
inline constexpr TypeInfo chaos_type_info_System_Private_CoreLib_System_NotSupportedException = { nullptr, 9608462597425565551ULL, 1 /* reference */ };
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_NotSupportedException = static_cast<CHAOS_IL2CPP_INTPTR>(9608462597425565551ULL);
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
        case chaos_type_id_System_Private_CoreLib_System_ArgumentException:
            return false;
        case chaos_type_id_System_Private_CoreLib_System_ArgumentNullException:
            return false;
        case chaos_type_id_System_Private_CoreLib_System_ArgumentOutOfRangeException:
            return false;
        case chaos_type_id_System_Private_CoreLib_System_Exception:
            return false;
        case chaos_type_id_System_Private_CoreLib_System_InvalidOperationException:
            return false;
        case chaos_type_id_System_Private_CoreLib_System_NotImplementedException:
            return false;
        case chaos_type_id_System_Private_CoreLib_System_NotSupportedException:
            return false;
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

struct chaos_type_System_Private_CoreLib_System_ArgumentException
{
    chaos_object_header header{};
};

struct chaos_type_System_Private_CoreLib_System_ArgumentNullException
{
    chaos_object_header header{};
};

struct chaos_type_System_Private_CoreLib_System_ArgumentOutOfRangeException
{
    chaos_object_header header{};
};

struct chaos_type_System_Private_CoreLib_System_Exception
{
    chaos_object_header header{};
};

struct chaos_type_System_Private_CoreLib_System_InvalidOperationException
{
    chaos_object_header header{};
};

struct chaos_type_System_Private_CoreLib_System_NotImplementedException
{
    chaos_object_header header{};
};

struct chaos_type_System_Private_CoreLib_System_NotSupportedException
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

extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);


// StringId constants for compile-time-known string literals.
constexpr CHAOS_IL2CPP_UINT64 chaos_string_id_2430D84680AABD0B = 2607821981565500683U;

// AOT-baked string table: sorted by StringId for binary search at runtime.
constexpr chaos::il2cpp::string_table::StringEntry chaos_aot_string_entries[] = {
    { chaos_string_id_2430D84680AABD0B, "hello", 5u },
};

constexpr CHAOS_IL2CPP_UINT32 chaos_aot_string_entry_count = sizeof(chaos_aot_string_entries) / sizeof(chaos_aot_string_entries[0]);

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Exception__GetBaseException_System_Exception__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return chaos_exception_get_base_exception(chaos_arg_0);
}

extern "C" void chaos_external_runtime_System_Private_CoreLib_System_Exception___ctor_System_Void_System_String_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
    chaos_reflection_set_exception_metadata(chaos_arg_0, chaos_arg_1);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Exception__get_HResult_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return chaos_exception_get_hresult(chaos_arg_0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Exception__get_InnerException_System_Exception__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return chaos_exception_get_inner_exception(chaos_arg_0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Exception__get_Message_System_String__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return chaos_reflection_get_exception_message(chaos_arg_0);
}

extern "C" void chaos_external_runtime_System_Private_CoreLib_System_InvalidOperationException___ctor_System_Void_System_String_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
    chaos_reflection_set_exception_metadata(chaos_arg_0, chaos_arg_1);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INT32>(0xDEAD);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Object__ToString_System_String__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
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

extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);

// Managed method: ExceptionThrowDiagnosticsNativeEntry/ExceptionThrowDiagnosticsNativeEntry::Method0()
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method0(void)
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
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        auto* chaos_object = new chaos_type_System_Private_CoreLib_System_Exception{};
        chaos_object->header.type_info = &chaos_type_info_System_Private_CoreLib_System_Exception;
        const auto chaos_arg_0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
        chaos_external_runtime_System_Private_CoreLib_System_Exception___ctor_System_Void_System_String_(chaos_arg_0, chaos_arg_1);
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    }

    chaos_stack_top--;

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_16;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ExceptionThrowDiagnosticsNativeEntry/ExceptionThrowDiagnosticsNativeEntry::Method1()
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method1(void)
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

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    {
        auto* chaos_object = new chaos_type_System_Private_CoreLib_System_Exception{};
        chaos_object->header.type_info = &chaos_type_info_System_Private_CoreLib_System_Exception;
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    }

    chaos_stack_top--;

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_17;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ExceptionThrowDiagnosticsNativeEntry/ExceptionThrowDiagnosticsNativeEntry::Method2()
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method2(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        auto* chaos_object = new chaos_type_System_Private_CoreLib_System_Exception{};
        chaos_object->header.type_info = &chaos_type_info_System_Private_CoreLib_System_Exception;
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    }

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Exception__get_Message_System_String__(chaos_arg_0);
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

// Managed method: ExceptionThrowDiagnosticsNativeEntry/ExceptionThrowDiagnosticsNativeEntry::Method3()
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method3(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        auto* chaos_object = new chaos_type_System_Private_CoreLib_System_Exception{};
        chaos_object->header.type_info = &chaos_type_info_System_Private_CoreLib_System_Exception;
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    }

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Exception__get_InnerException_System_Exception__(chaos_arg_0);
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

    goto chaos_ip_19;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ExceptionThrowDiagnosticsNativeEntry/ExceptionThrowDiagnosticsNativeEntry::Method4()
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method4(void)
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

// Managed method: ExceptionThrowDiagnosticsNativeEntry/ExceptionThrowDiagnosticsNativeEntry::Method5()
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method5(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 6> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        auto* chaos_object = new chaos_type_System_Private_CoreLib_System_Exception{};
        chaos_object->header.type_info = &chaos_type_info_System_Private_CoreLib_System_Exception;
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    }

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Exception__get_HResult_System_Int32__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_14;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ExceptionThrowDiagnosticsNativeEntry/ExceptionThrowDiagnosticsNativeEntry::Method6()
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method6(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        auto* chaos_object = new chaos_type_System_Private_CoreLib_System_Exception{};
        chaos_object->header.type_info = &chaos_type_info_System_Private_CoreLib_System_Exception;
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    }

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Object__ToString_System_String__(chaos_arg_0);
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

// Managed method: ExceptionThrowDiagnosticsNativeEntry/ExceptionThrowDiagnosticsNativeEntry::Method7()
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method7(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        auto* chaos_object = new chaos_type_System_Private_CoreLib_System_Exception{};
        chaos_object->header.type_info = &chaos_type_info_System_Private_CoreLib_System_Exception;
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    }

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Exception__GetBaseException_System_Exception__(chaos_arg_0);
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

    goto chaos_ip_19;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ExceptionThrowDiagnosticsNativeEntry/ExceptionThrowDiagnosticsNativeEntry::Method8()
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method8(void)
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
        auto* chaos_object = new chaos_type_System_Private_CoreLib_System_ArgumentException{};
        chaos_object->header.type_info = &chaos_type_info_System_Private_CoreLib_System_ArgumentException;
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    }

    chaos_stack_top--;

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_21;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ExceptionThrowDiagnosticsNativeEntry/ExceptionThrowDiagnosticsNativeEntry::Method9()
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method9(void)
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
        auto* chaos_object = new chaos_type_System_Private_CoreLib_System_ArgumentNullException{};
        chaos_object->header.type_info = &chaos_type_info_System_Private_CoreLib_System_ArgumentNullException;
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    }

    chaos_stack_top--;

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_16;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ExceptionThrowDiagnosticsNativeEntry/ExceptionThrowDiagnosticsNativeEntry::Method10()
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method10(void)
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
        auto* chaos_object = new chaos_type_System_Private_CoreLib_System_ArgumentOutOfRangeException{};
        chaos_object->header.type_info = &chaos_type_info_System_Private_CoreLib_System_ArgumentOutOfRangeException;
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    }

    chaos_stack_top--;

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_16;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ExceptionThrowDiagnosticsNativeEntry/ExceptionThrowDiagnosticsNativeEntry::Method11()
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method11(void)
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
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        auto* chaos_object = new chaos_type_System_Private_CoreLib_System_InvalidOperationException{};
        chaos_object->header.type_info = &chaos_type_info_System_Private_CoreLib_System_InvalidOperationException;
        const auto chaos_arg_0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
        chaos_external_runtime_System_Private_CoreLib_System_InvalidOperationException___ctor_System_Void_System_String_(chaos_arg_0, chaos_arg_1);
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    }

    chaos_stack_top--;

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_16;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ExceptionThrowDiagnosticsNativeEntry/ExceptionThrowDiagnosticsNativeEntry::Method12()
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method12(void)
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
        auto* chaos_object = new chaos_type_System_Private_CoreLib_System_NotSupportedException{};
        chaos_object->header.type_info = &chaos_type_info_System_Private_CoreLib_System_NotSupportedException;
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    }

    chaos_stack_top--;

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_16;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ExceptionThrowDiagnosticsNativeEntry/ExceptionThrowDiagnosticsNativeEntry::Method13()
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method13(void)
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
        auto* chaos_object = new chaos_type_System_Private_CoreLib_System_NotImplementedException{};
        chaos_object->header.type_info = &chaos_type_info_System_Private_CoreLib_System_NotImplementedException;
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    }

    chaos_stack_top--;

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_16;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ExceptionThrowDiagnosticsNativeEntry/ExceptionThrowDiagnosticsNativeEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 54> chaos_eval_stack{};
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
                    const auto chaos_result = ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method0();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            case 1:
            {
                {
                    const auto chaos_result = ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method1();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            case 2:
            {
                {
                    const auto chaos_result = ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method2();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            case 3:
            {
                {
                    const auto chaos_result = ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method3();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            case 4:
            {
                {
                    const auto chaos_result = ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method4();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            case 5:
            {
                {
                    const auto chaos_result = ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method5();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            case 6:
            {
                {
                    const auto chaos_result = ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method6();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            case 7:
            {
                {
                    const auto chaos_result = ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method7();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            case 8:
            {
                {
                    const auto chaos_result = ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method8();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            case 9:
            {
                {
                    const auto chaos_result = ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method9();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            case 10:
            {
                {
                    const auto chaos_result = ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method10();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            case 11:
            {
                {
                    const auto chaos_result = ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method11();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            case 12:
            {
                {
                    const auto chaos_result = ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method12();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            case 13:
            {
                {
                    const auto chaos_result = ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Method13();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            default:
            {
                goto chaos_ip_181;
            }
        }
    }
    goto chaos_ip_181;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[2];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Native AOT entry for ExceptionThrowDiagnosticsNativeEntry/ExceptionThrowDiagnosticsNativeEntry::Run:System.Int32(System.Int32)
// Managed symbol: ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Run
extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 chaos_entry_index)
{
    return ExceptionThrowDiagnosticsNativeEntry_ExceptionThrowDiagnosticsNativeEntry_Run(chaos_entry_index);
}