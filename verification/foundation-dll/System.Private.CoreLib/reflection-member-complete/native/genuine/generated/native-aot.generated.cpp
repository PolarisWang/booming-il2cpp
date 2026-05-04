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

extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method14(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method15(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method16(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method17(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method18(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method19(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method20(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method21(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INT32>(0xDEAD);
}

extern "C" CHAOS_IL2CPP_INT64 chaos_external_runtime_System_Private_CoreLib_System_Reflection_FieldInfo__get_FieldHandle_System_RuntimeFieldHandle__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INT64>(0);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Reflection_FieldInfo__get_IsInitOnly_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INT32>(0);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Reflection_FieldInfo__get_IsLiteral_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INT32>(0);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Reflection_FieldInfo__get_IsStatic_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INT32>(0);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_MetadataToken_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return chaos_reflection_get_metadata_token(chaos_arg_0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_ReflectedType_System_Type__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return chaos_reflection_get_reflected_type(chaos_arg_0);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodBase__get_CallingConvention_System_Reflection_CallingConventions__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return chaos_reflection_get_calling_convention(chaos_arg_0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodBase__get_IsPublic_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return chaos_reflection_get_is_public(chaos_arg_0);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodBase__get_IsStatic_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INT32>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodBase__get_IsVirtual_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return chaos_reflection_get_is_virtual(chaos_arg_0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodBase__get_MethodHandle_System_RuntimeMethodHandle__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return chaos_reflection_get_method_handle(chaos_arg_0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodInfo__GetBaseDefinition_System_Reflection_MethodInfo__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return chaos_reflection_get_base_definition(chaos_arg_0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Reflection_PropertyInfo__GetIndexParameters_System_Reflection_ParameterInfo____(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Reflection_PropertyInfo__get_CanRead_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INT32>(0);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Reflection_PropertyInfo__get_CanWrite_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INT32>(0);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_RuntimeFieldHandle__GetHashCode_System_Int32__(CHAOS_IL2CPP_INT64 chaos_arg_0)
{
    return chaos_runtimefieldhandle_get_hash_code(chaos_arg_0);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_RuntimeMethodHandle__GetHashCode_System_Int32__(CHAOS_IL2CPP_INT64 chaos_arg_0)
{
    return chaos_runtimemethodhandle_get_hash_code(chaos_arg_0);
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

extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method14(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method15(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method16(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method17(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method18(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method19(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method20(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method21(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);

// Managed method: ReflectionMemberCompleteNativeEntry/ReflectionMemberCompleteNativeEntry::Method0()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method0(void)
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

// Managed method: ReflectionMemberCompleteNativeEntry/ReflectionMemberCompleteNativeEntry::Method1()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method1(void)
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

// Managed method: ReflectionMemberCompleteNativeEntry/ReflectionMemberCompleteNativeEntry::Method2()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method2(void)
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

// Managed method: ReflectionMemberCompleteNativeEntry/ReflectionMemberCompleteNativeEntry::Method3()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method3(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_ReflectedType_System_Type__(chaos_arg_0);
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

    goto chaos_ip_15;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionMemberCompleteNativeEntry/ReflectionMemberCompleteNativeEntry::Method4()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method4(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 6> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_MetadataToken_System_Int32__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_10;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionMemberCompleteNativeEntry/ReflectionMemberCompleteNativeEntry::Method5()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method5(void)
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

// Managed method: ReflectionMemberCompleteNativeEntry/ReflectionMemberCompleteNativeEntry::Method6()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method6(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodInfo__GetBaseDefinition_System_Reflection_MethodInfo__(chaos_arg_0);
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

    goto chaos_ip_15;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionMemberCompleteNativeEntry/ReflectionMemberCompleteNativeEntry::Method7()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method7(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 9> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodBase__get_MethodHandle_System_RuntimeMethodHandle__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]) | chaos_managed_pointer_local_slot_tag;

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_RuntimeMethodHandle__GetHashCode_System_Int32__(chaos_load_int64(chaos_arg_0));
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_18;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionMemberCompleteNativeEntry/ReflectionMemberCompleteNativeEntry::Method8()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method8(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 6> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodBase__get_CallingConvention_System_Reflection_CallingConventions__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_10;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionMemberCompleteNativeEntry/ReflectionMemberCompleteNativeEntry::Method9()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method9(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodBase__get_IsPublic_System_Boolean__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_13;
        }
        else
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_13;
        }
    }
    goto chaos_ip_13;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionMemberCompleteNativeEntry/ReflectionMemberCompleteNativeEntry::Method10()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method10(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodBase__get_IsStatic_System_Boolean__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_13;
        }
        else
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_13;
        }
    }
    goto chaos_ip_13;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionMemberCompleteNativeEntry/ReflectionMemberCompleteNativeEntry::Method11()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method11(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodBase__get_IsVirtual_System_Boolean__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_13;
        }
        else
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_13;
        }
    }
    goto chaos_ip_13;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionMemberCompleteNativeEntry/ReflectionMemberCompleteNativeEntry::Method12()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method12(void)
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

// Managed method: ReflectionMemberCompleteNativeEntry/ReflectionMemberCompleteNativeEntry::Method13()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method13(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_FieldInfo__get_IsStatic_System_Boolean__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_13;
        }
        else
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_13;
        }
    }
    goto chaos_ip_13;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionMemberCompleteNativeEntry/ReflectionMemberCompleteNativeEntry::Method14()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method14(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_FieldInfo__get_IsInitOnly_System_Boolean__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_13;
        }
        else
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_13;
        }
    }
    goto chaos_ip_13;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionMemberCompleteNativeEntry/ReflectionMemberCompleteNativeEntry::Method15()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method15(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_FieldInfo__get_IsLiteral_System_Boolean__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_13;
        }
        else
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_13;
        }
    }
    goto chaos_ip_13;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionMemberCompleteNativeEntry/ReflectionMemberCompleteNativeEntry::Method16()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method16(void)
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

// Managed method: ReflectionMemberCompleteNativeEntry/ReflectionMemberCompleteNativeEntry::Method17()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method17(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_PropertyInfo__get_CanRead_System_Boolean__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_13;
        }
        else
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_13;
        }
    }
    goto chaos_ip_13;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionMemberCompleteNativeEntry/ReflectionMemberCompleteNativeEntry::Method18()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method18(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_PropertyInfo__get_CanWrite_System_Boolean__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_13;
        }
        else
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_13;
        }
    }
    goto chaos_ip_13;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionMemberCompleteNativeEntry/ReflectionMemberCompleteNativeEntry::Method19()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method19(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_PropertyInfo__GetIndexParameters_System_Reflection_ParameterInfo____(chaos_arg_0);
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

    goto chaos_ip_12;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionMemberCompleteNativeEntry/ReflectionMemberCompleteNativeEntry::Method20()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method20(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 9> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_FieldInfo__get_FieldHandle_System_RuntimeFieldHandle__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = chaos_store_int64(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]) | chaos_managed_pointer_local_slot_tag;

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_RuntimeFieldHandle__GetHashCode_System_Int32__(chaos_load_int64(chaos_arg_0));
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_18;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionMemberCompleteNativeEntry/ReflectionMemberCompleteNativeEntry::Method21()
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method21(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 6> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodBase__get_CallingConvention_System_Reflection_CallingConventions__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_10;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ReflectionMemberCompleteNativeEntry/ReflectionMemberCompleteNativeEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 78> chaos_eval_stack{};
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
                    const auto chaos_result = ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method0();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_298;
            }
            case 1:
            {
                {
                    const auto chaos_result = ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method1();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_298;
            }
            case 2:
            {
                {
                    const auto chaos_result = ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method2();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_298;
            }
            case 3:
            {
                {
                    const auto chaos_result = ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method3();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_298;
            }
            case 4:
            {
                {
                    const auto chaos_result = ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method4();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_298;
            }
            case 5:
            {
                {
                    const auto chaos_result = ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method5();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_298;
            }
            case 6:
            {
                {
                    const auto chaos_result = ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method6();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_298;
            }
            case 7:
            {
                {
                    const auto chaos_result = ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method7();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_298;
            }
            case 8:
            {
                {
                    const auto chaos_result = ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method8();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_298;
            }
            case 9:
            {
                {
                    const auto chaos_result = ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method9();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_298;
            }
            case 10:
            {
                {
                    const auto chaos_result = ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method10();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_298;
            }
            case 11:
            {
                {
                    const auto chaos_result = ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method11();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_298;
            }
            case 12:
            {
                {
                    const auto chaos_result = ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method12();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_298;
            }
            case 13:
            {
                {
                    const auto chaos_result = ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method13();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_298;
            }
            case 14:
            {
                {
                    const auto chaos_result = ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method14();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_298;
            }
            case 15:
            {
                {
                    const auto chaos_result = ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method15();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_298;
            }
            case 16:
            {
                {
                    const auto chaos_result = ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method16();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_298;
            }
            case 17:
            {
                {
                    const auto chaos_result = ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method17();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_298;
            }
            case 18:
            {
                {
                    const auto chaos_result = ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method18();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_298;
            }
            case 19:
            {
                {
                    const auto chaos_result = ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method19();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_298;
            }
            case 20:
            {
                {
                    const auto chaos_result = ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method20();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_298;
            }
            case 21:
            {
                {
                    const auto chaos_result = ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Method21();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_298;
            }
            default:
            {
                goto chaos_ip_298;
            }
        }
    }
    goto chaos_ip_298;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[2];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Native AOT entry for ReflectionMemberCompleteNativeEntry/ReflectionMemberCompleteNativeEntry::Run:System.Int32(System.Int32)
// Managed symbol: ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Run
extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 chaos_entry_index)
{
    return ReflectionMemberCompleteNativeEntry_ReflectionMemberCompleteNativeEntry_Run(chaos_entry_index);
}