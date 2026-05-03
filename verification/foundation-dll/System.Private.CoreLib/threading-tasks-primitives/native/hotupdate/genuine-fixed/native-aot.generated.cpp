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

extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method14(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Threading_Tasks_Task__Delay_System_Threading_Tasks_Task_System_Int32_(CHAOS_IL2CPP_INT32 chaos_arg_0);

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Threading_Tasks_Task__Wait_System_Boolean_System_Int32_(CHAOS_IL2CPP_INT32 chaos_arg_0);

extern "C" void chaos_external_runtime_System_Private_CoreLib_System_Threading_Tasks_Task__Wait_System_Void__(void);

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Threading_Tasks_Task__get_CompletedTask_System_Threading_Tasks_Task__(void);

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Threading_Tasks_Task__get_IsCompleted_System_Boolean__(void);

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Threading_Tasks_Task__get_Status_System_Threading_Tasks_TaskStatus__(void);

extern "C" void chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Sleep_System_Void_System_Int32_(CHAOS_IL2CPP_INT32 chaos_arg_0);

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__(void);

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_ManagedThreadId_System_Int32__(void);

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry__Method0_System_Int32__(void);

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry__Method10_System_Int32__(void);

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry__Method11_System_Int32__(void);

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry__Method12_System_Int32__(void);

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry__Method13_System_Int32__(void);

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry__Method14_System_Int32__(void);

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry__Method1_System_Int32__(void);

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry__Method2_System_Int32__(void);

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry__Method3_System_Int32__(void);

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry__Method4_System_Int32__(void);

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry__Method5_System_Int32__(void);

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry__Method6_System_Int32__(void);

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry__Method7_System_Int32__(void);

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry__Method8_System_Int32__(void);

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry__Method9_System_Int32__(void);
}

extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method14(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);

// Managed method: ThreadingTasksPrimitivesNativeEntry/ThreadingTasksPrimitivesNativeEntry::Method0()
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method0(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ThreadingTasksPrimitivesNativeEntry/ThreadingTasksPrimitivesNativeEntry::Method1()
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method1(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ThreadingTasksPrimitivesNativeEntry/ThreadingTasksPrimitivesNativeEntry::Method2()
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method2(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Threading_Tasks_Task__Delay_System_Threading_Tasks_Task_System_Int32_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_stack_top--;

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ThreadingTasksPrimitivesNativeEntry/ThreadingTasksPrimitivesNativeEntry::Method3()
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method3(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Threading_Tasks_Task__get_CompletedTask_System_Threading_Tasks_Task__();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        chaos_external_runtime_System_Private_CoreLib_System_Threading_Tasks_Task__Wait_System_Void__();
    }

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ThreadingTasksPrimitivesNativeEntry/ThreadingTasksPrimitivesNativeEntry::Method4()
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method4(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 11) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Threading_Tasks_Task__get_CompletedTask_System_Threading_Tasks_Task__();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            CHAOS_IL2CPP_ABORT();
        }
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Threading_Tasks_Task__Wait_System_Boolean_System_Int32_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_19;
        }
        else
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_19;
        }
    }
chaos_ip_19: ;
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ThreadingTasksPrimitivesNativeEntry/ThreadingTasksPrimitivesNativeEntry::Method5()
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method5(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Threading_Tasks_Task__get_CompletedTask_System_Threading_Tasks_Task__();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Threading_Tasks_Task__get_IsCompleted_System_Boolean__();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_17;
        }
        else
        {
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
            chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
            goto chaos_ip_17;
        }
    }
chaos_ip_17: ;
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ThreadingTasksPrimitivesNativeEntry/ThreadingTasksPrimitivesNativeEntry::Method6()
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method6(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Threading_Tasks_Task__get_CompletedTask_System_Threading_Tasks_Task__();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Threading_Tasks_Task__get_Status_System_Threading_Tasks_TaskStatus__();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_stack_top--;

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ThreadingTasksPrimitivesNativeEntry/ThreadingTasksPrimitivesNativeEntry::Method7()
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method7(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ThreadingTasksPrimitivesNativeEntry/ThreadingTasksPrimitivesNativeEntry::Method8()
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method8(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ThreadingTasksPrimitivesNativeEntry/ThreadingTasksPrimitivesNativeEntry::Method9()
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method9(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ThreadingTasksPrimitivesNativeEntry/ThreadingTasksPrimitivesNativeEntry::Method10()
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method10(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ThreadingTasksPrimitivesNativeEntry/ThreadingTasksPrimitivesNativeEntry::Method11()
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method11(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ThreadingTasksPrimitivesNativeEntry/ThreadingTasksPrimitivesNativeEntry::Method12()
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method12(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Sleep_System_Void_System_Int32_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
    }

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ThreadingTasksPrimitivesNativeEntry/ThreadingTasksPrimitivesNativeEntry::Method13()
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method13(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_stack_top--;

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ThreadingTasksPrimitivesNativeEntry/ThreadingTasksPrimitivesNativeEntry::Method14()
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method14(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 6) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    {
        const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    {
        const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_ManagedThreadId_System_Int32__();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: ThreadingTasksPrimitivesNativeEntry/ThreadingTasksPrimitivesNativeEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 57) chaos_eval_stack{};
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
                    const auto chaos_result = ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method0();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_193;
            }
            case 1:
            {
                {
                    const auto chaos_result = ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method1();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_193;
            }
            case 2:
            {
                {
                    const auto chaos_result = ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method2();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_193;
            }
            case 3:
            {
                {
                    const auto chaos_result = ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method3();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_193;
            }
            case 4:
            {
                {
                    const auto chaos_result = ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method4();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_193;
            }
            case 5:
            {
                {
                    const auto chaos_result = ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method5();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_193;
            }
            case 6:
            {
                {
                    const auto chaos_result = ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method6();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_193;
            }
            case 7:
            {
                {
                    const auto chaos_result = ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method7();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_193;
            }
            case 8:
            {
                {
                    const auto chaos_result = ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method8();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_193;
            }
            case 9:
            {
                {
                    const auto chaos_result = ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method9();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_193;
            }
            case 10:
            {
                {
                    const auto chaos_result = ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method10();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_193;
            }
            case 11:
            {
                {
                    const auto chaos_result = ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method11();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_193;
            }
            case 12:
            {
                {
                    const auto chaos_result = ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method12();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_193;
            }
            case 13:
            {
                {
                    const auto chaos_result = ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method13();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_193;
            }
            case 14:
            {
                {
                    const auto chaos_result = ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method14();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_193;
            }
            default:
            {
                goto chaos_ip_193;
            }
        }
    }
chaos_ip_193: ;
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1);

    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[2];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Native AOT entry for ThreadingTasksPrimitivesNativeEntry/ThreadingTasksPrimitivesNativeEntry::Run:System.Int32(System.Int32)
// Managed symbol: ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Run
extern "C" int RunNativeAot_threading_tasks_primitives_genuine_fixed(CHAOS_IL2CPP_INT32 chaos_entry_index)
{
    return ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Run(chaos_entry_index);
}