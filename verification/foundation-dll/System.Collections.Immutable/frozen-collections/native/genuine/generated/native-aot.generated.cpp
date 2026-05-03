#include <chaos/common.h>
#include "runtime_core.h"
#include "native-aot.metadata.h"

namespace
{
#include <chaos/common.h>

struct chaos_object_header
{
    CHAOS_IL2CPP_INTPTR type_id = 0;
};

constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_managed_array = 1;

struct chaos_managed_array
{
    chaos_object_header header{};
    CHAOS_IL2CPP_UINT8 element_type_shape = 0;
    CHAOS_IL2CPP_INTPTR element_type_id = 0;
    CHAOS_IL2CPP_INTPTR length = 0;
    CHAOS_IL2CPP_INTPTR* elements = nullptr;
};

constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_reference = 1;
constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_value = 2;
constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_interface = 3;

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

constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_FrozenCollectionsNativeEntry__PrivateImplementationDetails_ = 2;
constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_ReadOnlySpan_System_Byte_ = 3;
constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = 4;

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
        case chaos_type_id_FrozenCollectionsNativeEntry__PrivateImplementationDetails_:
            return false;
        case chaos_type_id_System_Private_CoreLib_System_ReadOnlySpan_System_Byte_:
            return false;
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

struct chaos_type_FrozenCollectionsNativeEntry__PrivateImplementationDetails_
{
    chaos_object_header header{};
    CHAOS_IL2CPP_INTPTR field_FrozenCollectionsNativeEntry__PrivateImplementationDetails___684888C0EBB17F374298B65EE2807526C066094C701BCC7EBBE1C1095F494FC1 = 0;
    CHAOS_IL2CPP_INTPTR field_FrozenCollectionsNativeEntry__PrivateImplementationDetails___983987033F0E117011E531DC33AD9BB15290BBA41A414D830FB5CBDBCDA2FF17 = 0;
    CHAOS_IL2CPP_INTPTR field_FrozenCollectionsNativeEntry__PrivateImplementationDetails___E8A4B2EE7EDE79A3AFB332B5B6CC3D952A65FD8CFFB897F5D18016577C33D7CC4 = 0;
};

struct chaos_type_System_Private_CoreLib_System_ReadOnlySpan_System_Byte_
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

extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);

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

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry__Method0_System_Int32__(void)
{
    return FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method0();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry__Method1_System_Int32__(void)
{
    return FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method1();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry__Method2_System_Int32__(void)
{
    return FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method2();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry__Method3_System_Int32__(void)
{
    return FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method3();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry__Method4_System_Int32__(void)
{
    return FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method4();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry__Method5_System_Int32__(void)
{
    return FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method5();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry__Method6_System_Int32__(void)
{
    return FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method6();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry__Method7_System_Int32__(void)
{
    return FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method7();
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary__Create_System_Byte_System_Byte__System_Collections_Frozen_FrozenDictionary_System_Byte_System_Byte__System_ReadOnlySpan_System_Collections_Generic_KeyValuePair_System_Byte_System_Byte___(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary__Create_System_String_System_Int32__System_Collections_Frozen_FrozenDictionary_System_String_System_Int32__System_ReadOnlySpan_System_Collections_Generic_KeyValuePair_System_String_System_Int32___(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary__Create_System_String_System_String__System_Collections_Frozen_FrozenDictionary_System_String_System_String__System_ReadOnlySpan_System_Collections_Generic_KeyValuePair_System_String_System_String___(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Collections_Immutable_System_Collections_Frozen_FrozenSet__Create_System_Byte__System_Collections_Frozen_FrozenSet_System_Byte__System_ReadOnlySpan_System_Byte__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Collections_Immutable_System_Collections_Frozen_FrozenSet__Create_System_Int32__System_Collections_Frozen_FrozenSet_System_Int32__System_ReadOnlySpan_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" void chaos_external_runtime_System_Private_CoreLib_System_ReadOnlySpan_System_Byte____ctor_System_Void_System_Void__System_Int32_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1)
{
    return;
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__CreateSpan_System_Int32__System_ReadOnlySpan_System_Int32__System_RuntimeFieldHandle_(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}
}

extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);

// Managed method: FrozenCollectionsNativeEntry/FrozenCollectionsNativeEntry::Method0()
extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method0(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10) chaos_eval_stack{};
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

    // Generic execution authority: definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create:System.Collections.Frozen.FrozenDictionary<!!0,!!1>(System.ReadOnlySpan<System.Collections.Generic.KeyValuePair<!!0,!!1>>); type=[]; method=[System.String, System.Int32]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create:System.Collections.Frozen.FrozenDictionary<!!0,!!1>(System.ReadOnlySpan<System.Collections.Generic.KeyValuePair<!!0,!!1>>);type=[];method=[System.String,System.Int32]; stub=stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create:System.Collections.Frozen.FrozenDictionary<!!0,!!1>(System.ReadOnlySpan<System.Collections.Generic.KeyValuePair<!!0,!!1>>);type=[];method=[System.String,System.Int32]
    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_normalize_native_int_argument(chaos_raw_arg_1);
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
        const auto chaos_result = chaos_external_runtime_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary__Create_System_String_System_Int32__System_Collections_Frozen_FrozenDictionary_System_String_System_Int32__System_ReadOnlySpan_System_Collections_Generic_KeyValuePair_System_String_System_Int32___(chaos_arg_0, chaos_arg_1);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_stack_top--;

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: FrozenCollectionsNativeEntry/FrozenCollectionsNativeEntry::Method1()
extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method1(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10) chaos_eval_stack{};
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

    // Generic execution authority: definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create:System.Collections.Frozen.FrozenDictionary<!!0,!!1>(System.ReadOnlySpan<System.Collections.Generic.KeyValuePair<!!0,!!1>>); type=[]; method=[System.Byte, System.Byte]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create:System.Collections.Frozen.FrozenDictionary<!!0,!!1>(System.ReadOnlySpan<System.Collections.Generic.KeyValuePair<!!0,!!1>>);type=[];method=[System.Byte,System.Byte]; stub=stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create:System.Collections.Frozen.FrozenDictionary<!!0,!!1>(System.ReadOnlySpan<System.Collections.Generic.KeyValuePair<!!0,!!1>>);type=[];method=[System.Byte,System.Byte]
    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_normalize_native_int_argument(chaos_raw_arg_1);
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
        const auto chaos_result = chaos_external_runtime_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary__Create_System_Byte_System_Byte__System_Collections_Frozen_FrozenDictionary_System_Byte_System_Byte__System_ReadOnlySpan_System_Collections_Generic_KeyValuePair_System_Byte_System_Byte___(chaos_arg_0, chaos_arg_1);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_stack_top--;

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: FrozenCollectionsNativeEntry/FrozenCollectionsNativeEntry::Method2()
extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method2(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10) chaos_eval_stack{};
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

    // Generic execution authority: definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create:System.Collections.Frozen.FrozenDictionary<!!0,!!1>(System.ReadOnlySpan<System.Collections.Generic.KeyValuePair<!!0,!!1>>); type=[]; method=[System.String, System.String]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create:System.Collections.Frozen.FrozenDictionary<!!0,!!1>(System.ReadOnlySpan<System.Collections.Generic.KeyValuePair<!!0,!!1>>);type=[];method=[System.String,System.String]; stub=stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create:System.Collections.Frozen.FrozenDictionary<!!0,!!1>(System.ReadOnlySpan<System.Collections.Generic.KeyValuePair<!!0,!!1>>);type=[];method=[System.String,System.String]
    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_normalize_native_int_argument(chaos_raw_arg_1);
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
        const auto chaos_result = chaos_external_runtime_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary__Create_System_String_System_String__System_Collections_Frozen_FrozenDictionary_System_String_System_String__System_ReadOnlySpan_System_Collections_Generic_KeyValuePair_System_String_System_String___(chaos_arg_0, chaos_arg_1);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_stack_top--;

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: FrozenCollectionsNativeEntry/FrozenCollectionsNativeEntry::Method3()
extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method3(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10) chaos_eval_stack{};
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

    // Generic execution authority: definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create:System.Collections.Frozen.FrozenSet<!!0>(System.ReadOnlySpan<!!0>); type=[]; method=[System.Byte]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create:System.Collections.Frozen.FrozenSet<!!0>(System.ReadOnlySpan<!!0>);type=[];method=[System.Byte]; stub=stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create:System.Collections.Frozen.FrozenSet<!!0>(System.ReadOnlySpan<!!0>);type=[];method=[System.Byte]
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
        const auto chaos_result = chaos_external_runtime_System_Collections_Immutable_System_Collections_Frozen_FrozenSet__Create_System_Byte__System_Collections_Frozen_FrozenSet_System_Byte__System_ReadOnlySpan_System_Byte__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_stack_top--;

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: FrozenCollectionsNativeEntry/FrozenCollectionsNativeEntry::Method4()
extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method4(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        auto* chaos_object = reinterpret_cast<chaos_type_FrozenCollectionsNativeEntry__PrivateImplementationDetails_*>(chaos_eval_stack[--chaos_stack_top]);
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_object->field_FrozenCollectionsNativeEntry__PrivateImplementationDetails___684888C0EBB17F374298B65EE2807526C066094C701BCC7EBBE1C1095F494FC1);
    }

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);

    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        auto* chaos_object = new chaos_type_System_Private_CoreLib_System_ReadOnlySpan_System_Byte_{};
        chaos_object->header.type_id = chaos_type_id_System_Private_CoreLib_System_ReadOnlySpan_System_Byte_;
        const auto chaos_arg_0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
        chaos_external_runtime_System_Private_CoreLib_System_ReadOnlySpan_System_Byte____ctor_System_Void_System_Void__System_Int32_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    }

    // Generic execution authority: definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create:System.Collections.Frozen.FrozenSet<!!0>(System.ReadOnlySpan<!!0>); type=[]; method=[System.Byte]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create:System.Collections.Frozen.FrozenSet<!!0>(System.ReadOnlySpan<!!0>);type=[];method=[System.Byte]; stub=stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create:System.Collections.Frozen.FrozenSet<!!0>(System.ReadOnlySpan<!!0>);type=[];method=[System.Byte]
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
        const auto chaos_result = chaos_external_runtime_System_Collections_Immutable_System_Collections_Frozen_FrozenSet__Create_System_Byte__System_Collections_Frozen_FrozenSet_System_Byte__System_ReadOnlySpan_System_Byte__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_stack_top--;

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: FrozenCollectionsNativeEntry/FrozenCollectionsNativeEntry::Method5()
extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method5(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        auto* chaos_object = reinterpret_cast<chaos_type_FrozenCollectionsNativeEntry__PrivateImplementationDetails_*>(chaos_eval_stack[--chaos_stack_top]);
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_object->field_FrozenCollectionsNativeEntry__PrivateImplementationDetails___983987033F0E117011E531DC33AD9BB15290BBA41A414D830FB5CBDBCDA2FF17);
    }

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(2);

    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        auto* chaos_object = new chaos_type_System_Private_CoreLib_System_ReadOnlySpan_System_Byte_{};
        chaos_object->header.type_id = chaos_type_id_System_Private_CoreLib_System_ReadOnlySpan_System_Byte_;
        const auto chaos_arg_0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
        chaos_external_runtime_System_Private_CoreLib_System_ReadOnlySpan_System_Byte____ctor_System_Void_System_Void__System_Int32_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    }

    // Generic execution authority: definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create:System.Collections.Frozen.FrozenSet<!!0>(System.ReadOnlySpan<!!0>); type=[]; method=[System.Byte]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create:System.Collections.Frozen.FrozenSet<!!0>(System.ReadOnlySpan<!!0>);type=[];method=[System.Byte]; stub=stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create:System.Collections.Frozen.FrozenSet<!!0>(System.ReadOnlySpan<!!0>);type=[];method=[System.Byte]
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
        const auto chaos_result = chaos_external_runtime_System_Collections_Immutable_System_Collections_Frozen_FrozenSet__Create_System_Byte__System_Collections_Frozen_FrozenSet_System_Byte__System_ReadOnlySpan_System_Byte__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_stack_top--;

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: FrozenCollectionsNativeEntry/FrozenCollectionsNativeEntry::Method6()
extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method6(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10) chaos_eval_stack{};
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

    // Generic execution authority: definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create:System.Collections.Frozen.FrozenSet<!!0>(System.ReadOnlySpan<!!0>); type=[]; method=[System.Int32]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create:System.Collections.Frozen.FrozenSet<!!0>(System.ReadOnlySpan<!!0>);type=[];method=[System.Int32]; stub=stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create:System.Collections.Frozen.FrozenSet<!!0>(System.ReadOnlySpan<!!0>);type=[];method=[System.Int32]
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
        const auto chaos_result = chaos_external_runtime_System_Collections_Immutable_System_Collections_Frozen_FrozenSet__Create_System_Int32__System_Collections_Frozen_FrozenSet_System_Int32__System_ReadOnlySpan_System_Int32__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_stack_top--;

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: FrozenCollectionsNativeEntry/FrozenCollectionsNativeEntry::Method7()
extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method7(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 9) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(74128298u);

    // Generic execution authority: definition=System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::CreateSpan:System.ReadOnlySpan<!!0>(System.RuntimeFieldHandle); type=[]; method=[System.Int32]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::CreateSpan:System.ReadOnlySpan<!!0>(System.RuntimeFieldHandle);type=[];method=[System.Int32]; stub=stub:definition=System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::CreateSpan:System.ReadOnlySpan<!!0>(System.RuntimeFieldHandle);type=[];method=[System.Int32]
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__CreateSpan_System_Int32__System_ReadOnlySpan_System_Int32__System_RuntimeFieldHandle_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    // Generic execution authority: definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create:System.Collections.Frozen.FrozenSet<!!0>(System.ReadOnlySpan<!!0>); type=[]; method=[System.Int32]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create:System.Collections.Frozen.FrozenSet<!!0>(System.ReadOnlySpan<!!0>);type=[];method=[System.Int32]; stub=stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create:System.Collections.Frozen.FrozenSet<!!0>(System.ReadOnlySpan<!!0>);type=[];method=[System.Int32]
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
        const auto chaos_result = chaos_external_runtime_System_Collections_Immutable_System_Collections_Frozen_FrozenSet__Create_System_Int32__System_Collections_Frozen_FrozenSet_System_Int32__System_ReadOnlySpan_System_Int32__(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_stack_top--;

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: FrozenCollectionsNativeEntry/FrozenCollectionsNativeEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 36) chaos_eval_stack{};
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
                    const auto chaos_result = FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method0();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_109;
            }
            case 1:
            {
                {
                    const auto chaos_result = FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method1();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_109;
            }
            case 2:
            {
                {
                    const auto chaos_result = FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method2();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_109;
            }
            case 3:
            {
                {
                    const auto chaos_result = FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method3();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_109;
            }
            case 4:
            {
                {
                    const auto chaos_result = FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method4();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_109;
            }
            case 5:
            {
                {
                    const auto chaos_result = FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method5();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_109;
            }
            case 6:
            {
                {
                    const auto chaos_result = FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method6();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_109;
            }
            case 7:
            {
                {
                    const auto chaos_result = FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Method7();
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
chaos_ip_109: ;
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1);

    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[2];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Native AOT entry for FrozenCollectionsNativeEntry/FrozenCollectionsNativeEntry::Run:System.Int32(System.Int32)
// Managed symbol: FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Run
extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 chaos_entry_index)
{
    return FrozenCollectionsNativeEntry_FrozenCollectionsNativeEntry_Run(chaos_entry_index);
}