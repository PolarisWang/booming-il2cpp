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

extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);

bool chaos_try_get_static_field_data(
    CHAOS_IL2CPP_INTPTR chaos_field_handle,
    const CHAOS_IL2CPP_UINT8*& chaos_data,
    CHAOS_IL2CPP_SIZE& chaos_size)
{
    chaos_data = nullptr;
    chaos_size = 0;
    return false;
}

void chaos_initialize_array_from_field_data_int32(CHAOS_IL2CPP_INTPTR chaos_array_value, CHAOS_IL2CPP_INTPTR chaos_field_handle)
{
    auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_array_value);
    if (chaos_array == nullptr || chaos_array->element_type_shape != chaos_type_shape_value)
    {
        CHAOS_IL2CPP_ABORT();
    }
    const CHAOS_IL2CPP_UINT8* chaos_bytes = nullptr;
    CHAOS_IL2CPP_SIZE chaos_size = 0;
    if (!chaos_try_get_static_field_data(chaos_field_handle, chaos_bytes, chaos_size) ||
        (chaos_size % sizeof(CHAOS_IL2CPP_INT32)) != 0)
    {
        CHAOS_IL2CPP_ABORT();
    }
    const auto chaos_element_count = static_cast<CHAOS_IL2CPP_INT32>(chaos_size / sizeof(CHAOS_IL2CPP_INT32));
    if (chaos_element_count > static_cast<CHAOS_IL2CPP_INT32>(chaos_array->length))
    {
        CHAOS_IL2CPP_ABORT();
    }
    for (CHAOS_IL2CPP_INT32 chaos_index = 0; chaos_index < chaos_element_count; chaos_index++)
    {
        CHAOS_IL2CPP_INT32 chaos_value = 0;
        CHAOS_IL2CPP_MEMCPY(&chaos_value, chaos_bytes + (static_cast<CHAOS_IL2CPP_SIZE>(chaos_index) * sizeof(CHAOS_IL2CPP_INT32)), sizeof(chaos_value));
        chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value);
    }
}

CHAOS_IL2CPP_INTPTR chaos_create_field_data_span_int32(CHAOS_IL2CPP_INTPTR chaos_field_handle)
{
    const CHAOS_IL2CPP_UINT8* chaos_bytes = nullptr;
    CHAOS_IL2CPP_SIZE chaos_size = 0;
    if (!chaos_try_get_static_field_data(chaos_field_handle, chaos_bytes, chaos_size) ||
        (chaos_size % sizeof(CHAOS_IL2CPP_INT32)) != 0)
    {
        CHAOS_IL2CPP_ABORT();
    }
    return ChaosIl2cpp::Common::create_raw_span_int32(const_cast<CHAOS_IL2CPP_UINT8*>(chaos_bytes), static_cast<CHAOS_IL2CPP_INT32>(chaos_size / sizeof(CHAOS_IL2CPP_INT32)), true);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Memory_System_Byte___ToArray_System_Byte____(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Memory_System_Byte___get_Span_System_Span_System_Byte___(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INT32>(0xDEAD);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_ReadOnlySpan_System_Byte___ToArray_System_Byte____()
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Runtime_InteropServices_MemoryMarshal__GetReference_System_Byte__System_Byte__System_ReadOnlySpan_System_Byte__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Runtime_InteropServices_MemoryMarshal__GetReference_System_Byte__System_Byte__System_Span_System_Byte__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" void chaos_external_runtime_System_Private_CoreLib_System_Span_System_Byte___CopyTo_System_Void_System_Span_System_Byte__(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
    (void)chaos_arg_0; (void)chaos_arg_1;
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Span_System_Byte___get_Empty_System_Span_System_Byte___()
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
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

extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);

// Managed method: SpanMemoryBuffersNativeEntry/SpanMemoryBuffersNativeEntry::Method0()
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method0(void)
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

// Managed method: SpanMemoryBuffersNativeEntry/SpanMemoryBuffersNativeEntry::Method1()
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method1(void)
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

// Managed method: SpanMemoryBuffersNativeEntry/SpanMemoryBuffersNativeEntry::Method2()
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method2(void)
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

// Managed method: SpanMemoryBuffersNativeEntry/SpanMemoryBuffersNativeEntry::Method3()
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method3(void)
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

// Managed method: SpanMemoryBuffersNativeEntry/SpanMemoryBuffersNativeEntry::Method4()
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method4(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 12> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]) | chaos_managed_pointer_local_slot_tag;

    chaos_eval_stack[chaos_stack_top] = chaos_eval_stack[chaos_stack_top - 1];
    chaos_stack_top++;

    {
        const auto chaos_address = chaos_eval_stack[--chaos_stack_top];
        if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
        {
            auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
            *chaos_slot = static_cast<CHAOS_IL2CPP_INTPTR>(0);
        }
        else
        {
            *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = static_cast<CHAOS_IL2CPP_INTPTR>(0);
        }
    }

    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]) | chaos_managed_pointer_local_slot_tag;

    {
        const auto chaos_address = chaos_eval_stack[--chaos_stack_top];
        if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
        {
            auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
            *chaos_slot = static_cast<CHAOS_IL2CPP_INTPTR>(0);
        }
        else
        {
            *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = static_cast<CHAOS_IL2CPP_INTPTR>(0);
        }
    }

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];

    // Generic execution authority: definition=System.Private.CoreLib/System.Span`1::CopyTo:System.Void(System.Span<!0>); type=[System.Byte]; method=[]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Private.CoreLib/System.Span`1::CopyTo:System.Void(System.Span<!0>);type=[System.Byte];method=[]; stub=stub:definition=System.Private.CoreLib/System.Span`1::CopyTo:System.Void(System.Span<!0>);type=[System.Byte];method=[]
    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        chaos_external_runtime_System_Private_CoreLib_System_Span_System_Byte___CopyTo_System_Void_System_Span_System_Byte__(chaos_arg_0, chaos_arg_1);
    }

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_29;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[2];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: SpanMemoryBuffersNativeEntry/SpanMemoryBuffersNativeEntry::Method5()
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method5(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    // Generic execution authority: definition=System.Private.CoreLib/System.Span`1::get_Empty:System.Span<!0>(); type=[System.Byte]; method=[]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Private.CoreLib/System.Span`1::get_Empty:System.Span<!0>();type=[System.Byte];method=[]; stub=stub:definition=System.Private.CoreLib/System.Span`1::get_Empty:System.Span<!0>();type=[System.Byte];method=[]
    {
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Span_System_Byte___get_Empty_System_Span_System_Byte___();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]) | chaos_managed_pointer_local_slot_tag;

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

    chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_23;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: SpanMemoryBuffersNativeEntry/SpanMemoryBuffersNativeEntry::Method6()
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method6(void)
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

// Managed method: SpanMemoryBuffersNativeEntry/SpanMemoryBuffersNativeEntry::Method7()
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method7(void)
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

// Managed method: SpanMemoryBuffersNativeEntry/SpanMemoryBuffersNativeEntry::Method8()
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method8(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]) | chaos_managed_pointer_local_slot_tag;

    chaos_eval_stack[chaos_stack_top] = chaos_eval_stack[chaos_stack_top - 1];
    chaos_stack_top++;

    {
        const auto chaos_address = chaos_eval_stack[--chaos_stack_top];
        if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
        {
            auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
            *chaos_slot = static_cast<CHAOS_IL2CPP_INTPTR>(0);
        }
        else
        {
            *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = static_cast<CHAOS_IL2CPP_INTPTR>(0);
        }
    }

    // Generic execution authority: definition=System.Private.CoreLib/System.ReadOnlySpan`1::ToArray:!0[](); type=[System.Byte]; method=[]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Private.CoreLib/System.ReadOnlySpan`1::ToArray:!0[]();type=[System.Byte];method=[]; stub=stub:definition=System.Private.CoreLib/System.ReadOnlySpan`1::ToArray:!0[]();type=[System.Byte];method=[]
    {
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_ReadOnlySpan_System_Byte___ToArray_System_Byte____();
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

    chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_20;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: SpanMemoryBuffersNativeEntry/SpanMemoryBuffersNativeEntry::Method9()
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method9(void)
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

// Managed method: SpanMemoryBuffersNativeEntry/SpanMemoryBuffersNativeEntry::Method10()
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method10(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 11> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]) | chaos_managed_pointer_local_slot_tag;

    chaos_eval_stack[chaos_stack_top] = chaos_eval_stack[chaos_stack_top - 1];
    chaos_stack_top++;

    {
        const auto chaos_address = chaos_eval_stack[--chaos_stack_top];
        if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
        {
            auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
            *chaos_slot = static_cast<CHAOS_IL2CPP_INTPTR>(0);
        }
        else
        {
            *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = static_cast<CHAOS_IL2CPP_INTPTR>(0);
        }
    }

    // Generic execution authority: definition=System.Private.CoreLib/System.Memory`1::get_Span:System.Span<!0>(); type=[System.Byte]; method=[]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Private.CoreLib/System.Memory`1::get_Span:System.Span<!0>();type=[System.Byte];method=[]; stub=stub:definition=System.Private.CoreLib/System.Memory`1::get_Span:System.Span<!0>();type=[System.Byte];method=[]
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Memory_System_Byte___get_Span_System_Span_System_Byte___(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]) | chaos_managed_pointer_local_slot_tag;

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

    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_32;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[2];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: SpanMemoryBuffersNativeEntry/SpanMemoryBuffersNativeEntry::Method11()
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method11(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]) | chaos_managed_pointer_local_slot_tag;

    chaos_eval_stack[chaos_stack_top] = chaos_eval_stack[chaos_stack_top - 1];
    chaos_stack_top++;

    {
        const auto chaos_address = chaos_eval_stack[--chaos_stack_top];
        if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
        {
            auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
            *chaos_slot = static_cast<CHAOS_IL2CPP_INTPTR>(0);
        }
        else
        {
            *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = static_cast<CHAOS_IL2CPP_INTPTR>(0);
        }
    }

    // Generic execution authority: definition=System.Private.CoreLib/System.Memory`1::ToArray:!0[](); type=[System.Byte]; method=[]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Private.CoreLib/System.Memory`1::ToArray:!0[]();type=[System.Byte];method=[]; stub=stub:definition=System.Private.CoreLib/System.Memory`1::ToArray:!0[]();type=[System.Byte];method=[]
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Memory_System_Byte___ToArray_System_Byte____(chaos_arg_0);
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

    chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_20;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: SpanMemoryBuffersNativeEntry/SpanMemoryBuffersNativeEntry::Method12()
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method12(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 9> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]) | chaos_managed_pointer_local_slot_tag;

    {
        const auto chaos_address = chaos_eval_stack[--chaos_stack_top];
        if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
        {
            auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
            *chaos_slot = static_cast<CHAOS_IL2CPP_INTPTR>(0);
        }
        else
        {
            *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = static_cast<CHAOS_IL2CPP_INTPTR>(0);
        }
    }

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    // Generic execution authority: definition=System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:!!0&(System.ReadOnlySpan<!!0>); type=[]; method=[System.Byte]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:!!0&(System.ReadOnlySpan<!!0>);type=[];method=[System.Byte]; stub=stub:definition=System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:!!0&(System.ReadOnlySpan<!!0>);type=[];method=[System.Byte]
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Runtime_InteropServices_MemoryMarshal__GetReference_System_Byte__System_Byte__System_ReadOnlySpan_System_Byte__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_address = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_value = chaos_load_indirect<CHAOS_IL2CPP_UINT8>(chaos_address);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value);
    }

    chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_19;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: SpanMemoryBuffersNativeEntry/SpanMemoryBuffersNativeEntry::Method13()
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method13(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 9> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]) | chaos_managed_pointer_local_slot_tag;

    {
        const auto chaos_address = chaos_eval_stack[--chaos_stack_top];
        if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
        {
            auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
            *chaos_slot = static_cast<CHAOS_IL2CPP_INTPTR>(0);
        }
        else
        {
            *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = static_cast<CHAOS_IL2CPP_INTPTR>(0);
        }
    }

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    // Generic execution authority: definition=System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:!!0&(System.Span<!!0>); type=[]; method=[System.Byte]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:!!0&(System.Span<!!0>);type=[];method=[System.Byte]; stub=stub:definition=System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:!!0&(System.Span<!!0>);type=[];method=[System.Byte]
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Runtime_InteropServices_MemoryMarshal__GetReference_System_Byte__System_Byte__System_Span_System_Byte__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_address = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_value = chaos_load_indirect<CHAOS_IL2CPP_UINT8>(chaos_address);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value);
    }

    chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_19;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: SpanMemoryBuffersNativeEntry/SpanMemoryBuffersNativeEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0)
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
                    const auto chaos_result = SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method0();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            case 1:
            {
                {
                    const auto chaos_result = SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method1();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            case 2:
            {
                {
                    const auto chaos_result = SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method2();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            case 3:
            {
                {
                    const auto chaos_result = SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method3();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            case 4:
            {
                {
                    const auto chaos_result = SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method4();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            case 5:
            {
                {
                    const auto chaos_result = SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method5();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            case 6:
            {
                {
                    const auto chaos_result = SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method6();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            case 7:
            {
                {
                    const auto chaos_result = SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method7();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            case 8:
            {
                {
                    const auto chaos_result = SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method8();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            case 9:
            {
                {
                    const auto chaos_result = SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method9();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            case 10:
            {
                {
                    const auto chaos_result = SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method10();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            case 11:
            {
                {
                    const auto chaos_result = SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method11();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            case 12:
            {
                {
                    const auto chaos_result = SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method12();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_181;
            }
            case 13:
            {
                {
                    const auto chaos_result = SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Method13();
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

// Native AOT entry for SpanMemoryBuffersNativeEntry/SpanMemoryBuffersNativeEntry::Run:System.Int32(System.Int32)
// Managed symbol: SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Run
extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 chaos_entry_index)
{
    return SpanMemoryBuffersNativeEntry_SpanMemoryBuffersNativeEntry_Run(chaos_entry_index);
}