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

extern "C" CHAOS_IL2CPP_INT32 ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);


// StringId constants for compile-time-known string literals.
constexpr CHAOS_IL2CPP_UINT64 chaos_string_id_2430D84680AABD0B = 2607821981565500683U;

// AOT-baked string table: sorted by StringId for binary search at runtime.
constexpr chaos::il2cpp::string_table::StringEntry chaos_aot_string_entries[] = {
    { chaos_string_id_2430D84680AABD0B, "hello", 5u },
};

constexpr CHAOS_IL2CPP_UINT32 chaos_aot_string_entry_count = sizeof(chaos_aot_string_entries) / sizeof(chaos_aot_string_entries[0]);

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_ImmutableArrayNativeEntry_ImmutableArrayNativeEntry__Method0_System_Int32__(void)
{
    return ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method0();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_ImmutableArrayNativeEntry_ImmutableArrayNativeEntry__Method1_System_Int32__(void)
{
    return ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method1();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_ImmutableArrayNativeEntry_ImmutableArrayNativeEntry__Method2_System_Int32__(void)
{
    return ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method2();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_ImmutableArrayNativeEntry_ImmutableArrayNativeEntry__Method3_System_Int32__(void)
{
    return ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method3();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_ImmutableArrayNativeEntry_ImmutableArrayNativeEntry__Method4_System_Int32__(void)
{
    return ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method4();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_ImmutableArrayNativeEntry_ImmutableArrayNativeEntry__Method5_System_Int32__(void)
{
    return ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method5();
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Collections_Immutable_ImmutableArray__Create_System_Byte__ImmutableArray_System_Byte__System_Byte_(CHAOS_IL2CPP_UINT8 chaos_arg_0)
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Collections_Immutable_ImmutableArray__Create_System_Byte__ImmutableArray_System_Byte__System_Byte_System_Byte_(CHAOS_IL2CPP_UINT8 chaos_arg_0, CHAOS_IL2CPP_UINT8 chaos_arg_1)
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Collections_Immutable_ImmutableArray__Create_System_Byte__ImmutableArray_System_Byte___(void)
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Collections_Immutable_ImmutableArray__Create_System_Int32__ImmutableArray_System_Int32__System_Int32_(CHAOS_IL2CPP_INT32 chaos_arg_0)
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Collections_Immutable_ImmutableArray__Create_System_Int32__ImmutableArray_System_Int32___(void)
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Collections_Immutable_ImmutableArray__Create_System_String__ImmutableArray_System_String__System_String_(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}
}

extern "C" CHAOS_IL2CPP_INT32 ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);

// Managed method: ImmutableArrayNativeEntry/ImmutableArrayNativeEntry::Method0()
extern "C" CHAOS_IL2CPP_INT32 ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method0(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    // Generic execution authority: definition=System.Collections.Immutable/ImmutableArray::Create:ImmutableArray<!!0>(); type=[]; method=[System.Byte]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Collections.Immutable/ImmutableArray::Create:ImmutableArray<!!0>();type=[];method=[System.Byte]; stub=stub:definition=System.Collections.Immutable/ImmutableArray::Create:ImmutableArray<!!0>();type=[];method=[System.Byte]
    {
        const auto chaos_result = chaos_external_runtime_System_Collections_Immutable_ImmutableArray__Create_System_Byte__ImmutableArray_System_Byte___();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_stack_top--;

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ImmutableArrayNativeEntry/ImmutableArrayNativeEntry::Method1()
extern "C" CHAOS_IL2CPP_INT32 ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method1(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);

    // Generic execution authority: definition=System.Collections.Immutable/ImmutableArray::Create:ImmutableArray<!!0>(!!0); type=[]; method=[System.Byte]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Collections.Immutable/ImmutableArray::Create:ImmutableArray<!!0>(!!0);type=[];method=[System.Byte]; stub=stub:definition=System.Collections.Immutable/ImmutableArray::Create:ImmutableArray<!!0>(!!0);type=[];method=[System.Byte]
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Collections_Immutable_ImmutableArray__Create_System_Byte__ImmutableArray_System_Byte__System_Byte_(static_cast<CHAOS_IL2CPP_UINT8>(chaos_arg_0));
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_stack_top--;

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ImmutableArrayNativeEntry/ImmutableArrayNativeEntry::Method2()
extern "C" CHAOS_IL2CPP_INT32 ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method2(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 9) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);

    // Generic execution authority: definition=System.Collections.Immutable/ImmutableArray::Create:ImmutableArray<!!0>(!!0,!!0); type=[]; method=[System.Byte]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Collections.Immutable/ImmutableArray::Create:ImmutableArray<!!0>(!!0,!!0);type=[];method=[System.Byte]; stub=stub:definition=System.Collections.Immutable/ImmutableArray::Create:ImmutableArray<!!0>(!!0,!!0);type=[];method=[System.Byte]
    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Collections_Immutable_ImmutableArray__Create_System_Byte__ImmutableArray_System_Byte__System_Byte_System_Byte_(static_cast<CHAOS_IL2CPP_UINT8>(chaos_arg_0), static_cast<CHAOS_IL2CPP_UINT8>(chaos_arg_1));
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_stack_top--;

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ImmutableArrayNativeEntry/ImmutableArrayNativeEntry::Method3()
extern "C" CHAOS_IL2CPP_INT32 ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method3(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    // Generic execution authority: definition=System.Collections.Immutable/ImmutableArray::Create:ImmutableArray<!!0>(); type=[]; method=[System.Int32]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Collections.Immutable/ImmutableArray::Create:ImmutableArray<!!0>();type=[];method=[System.Int32]; stub=stub:definition=System.Collections.Immutable/ImmutableArray::Create:ImmutableArray<!!0>();type=[];method=[System.Int32]
    {
        const auto chaos_result = chaos_external_runtime_System_Collections_Immutable_ImmutableArray__Create_System_Int32__ImmutableArray_System_Int32___();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_stack_top--;

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ImmutableArrayNativeEntry/ImmutableArrayNativeEntry::Method4()
extern "C" CHAOS_IL2CPP_INT32 ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method4(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);

    // Generic execution authority: definition=System.Collections.Immutable/ImmutableArray::Create:ImmutableArray<!!0>(!!0); type=[]; method=[System.Int32]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Collections.Immutable/ImmutableArray::Create:ImmutableArray<!!0>(!!0);type=[];method=[System.Int32]; stub=stub:definition=System.Collections.Immutable/ImmutableArray::Create:ImmutableArray<!!0>(!!0);type=[];method=[System.Int32]
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Collections_Immutable_ImmutableArray__Create_System_Int32__ImmutableArray_System_Int32__System_Int32_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_stack_top--;

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ImmutableArrayNativeEntry/ImmutableArrayNativeEntry::Method5()
extern "C" CHAOS_IL2CPP_INT32 ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method5(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_id = chaos_type_id_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
        chaos_string->utf8_data = "hello";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }

    // Generic execution authority: definition=System.Collections.Immutable/ImmutableArray::Create:ImmutableArray<!!0>(!!0); type=[]; method=[System.String]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Collections.Immutable/ImmutableArray::Create:ImmutableArray<!!0>(!!0);type=[];method=[System.String]; stub=stub:definition=System.Collections.Immutable/ImmutableArray::Create:ImmutableArray<!!0>(!!0);type=[];method=[System.String]
    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
        const auto chaos_result = chaos_external_runtime_System_Collections_Immutable_ImmutableArray__Create_System_String__ImmutableArray_System_String__System_String_(chaos_arg_0);
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_stack_top--;

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ImmutableArrayNativeEntry/ImmutableArrayNativeEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 30) chaos_eval_stack{};
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
                    const auto chaos_result = ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method0();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_85;
            }
            case 1:
            {
                {
                    const auto chaos_result = ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method1();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_85;
            }
            case 2:
            {
                {
                    const auto chaos_result = ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method2();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_85;
            }
            case 3:
            {
                {
                    const auto chaos_result = ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method3();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_85;
            }
            case 4:
            {
                {
                    const auto chaos_result = ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method4();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_85;
            }
            case 5:
            {
                {
                    const auto chaos_result = ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Method5();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_85;
            }
            default:
            {
                goto chaos_ip_85;
            }
        }
    }
chaos_ip_85: ;
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1);

    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[2];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Native AOT entry for ImmutableArrayNativeEntry/ImmutableArrayNativeEntry::Run:System.Int32(System.Int32)
// Managed symbol: ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Run
extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 chaos_entry_index)
{
    return ImmutableArrayNativeEntry_ImmutableArrayNativeEntry_Run(chaos_entry_index);
}