#include <chaos/common.h>
#include "runtime_core.h"

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

extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);
}

extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);

// Managed method: ObjectEqualityIdentityPatchEntry/ObjectEqualityIdentityPatchEntry::Method0()
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method0(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177280);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ObjectEqualityIdentityPatchEntry/ObjectEqualityIdentityPatchEntry::Method1()
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method1(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177279);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ObjectEqualityIdentityPatchEntry/ObjectEqualityIdentityPatchEntry::Method2()
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method2(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177278);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ObjectEqualityIdentityPatchEntry/ObjectEqualityIdentityPatchEntry::Method3()
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method3(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177277);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ObjectEqualityIdentityPatchEntry/ObjectEqualityIdentityPatchEntry::Method4()
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method4(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177276);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ObjectEqualityIdentityPatchEntry/ObjectEqualityIdentityPatchEntry::Method5()
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method5(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177275);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ObjectEqualityIdentityPatchEntry/ObjectEqualityIdentityPatchEntry::Method6()
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method6(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177274);
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    goto chaos_ip_9;

chaos_ip_9:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_10;

chaos_ip_10:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ObjectEqualityIdentityPatchEntry/ObjectEqualityIdentityPatchEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0)
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
        const auto chaos_result = ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method0();
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
        const auto chaos_result = ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method1();
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
        const auto chaos_result = ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method2();
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
        const auto chaos_result = ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method3();
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
        const auto chaos_result = ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method4();
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
        const auto chaos_result = ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method5();
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
        const auto chaos_result = ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Method6();
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

// Native AOT entry for ObjectEqualityIdentityPatchEntry/ObjectEqualityIdentityPatchEntry::Run:System.Int32(System.Int32)
// Managed symbol: ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Run
extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 chaos_entry_index)
{
    return ObjectEqualityIdentityPatchEntry_ObjectEqualityIdentityPatchEntry_Run(chaos_entry_index);
}