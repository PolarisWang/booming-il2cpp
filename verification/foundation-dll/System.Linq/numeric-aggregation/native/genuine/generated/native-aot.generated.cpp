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

extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method14(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method15(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method16(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method17(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method18(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method19(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method20(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method21(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method22(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method23(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method24(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method25(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method26(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method27(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method28(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method29(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method30(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method31(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method32(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method33(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method34(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method35(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method36(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method37(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method38(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method39(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method40(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method41(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method42(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method43(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method44(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method45(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method46(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method47(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method48(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method49(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method50(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method51(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method52(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method53(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method54(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method55(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method56(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method57(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method58(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method59(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method60(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method61(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method62(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method63(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method64(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method0_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method0();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method10_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method10();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method11_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method11();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method12_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method12();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method13_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method13();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method14_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method14();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method15_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method15();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method16_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method16();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method17_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method17();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method18_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method18();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method19_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method19();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method1_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method1();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method20_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method20();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method21_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method21();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method22_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method22();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method23_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method23();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method24_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method24();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method25_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method25();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method26_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method26();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method27_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method27();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method28_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method28();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method29_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method29();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method2_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method2();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method30_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method30();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method31_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method31();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method32_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method32();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method33_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method33();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method34_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method34();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method35_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method35();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method36_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method36();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method37_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method37();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method38_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method38();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method39_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method39();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method3_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method3();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method40_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method40();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method41_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method41();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method42_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method42();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method43_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method43();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method44_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method44();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method45_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method45();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method46_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method46();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method47_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method47();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method48_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method48();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method49_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method49();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method4_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method4();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method50_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method50();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method51_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method51();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method52_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method52();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method53_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method53();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method54_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method54();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method55_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method55();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method56_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method56();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method57_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method57();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method58_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method58();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method59_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method59();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method5_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method5();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method60_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method60();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method61_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method61();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method62_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method62();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method63_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method63();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method64_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method64();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method6_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method6();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method7_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method7();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method8_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method8();
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_NumericAggregationNativeEntry_NumericAggregationNativeEntry__Method9_System_Int32__(void)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method9();
}
}

extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method14(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method15(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method16(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method17(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method18(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method19(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method20(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method21(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method22(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method23(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method24(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method25(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method26(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method27(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method28(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method29(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method30(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method31(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method32(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method33(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method34(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method35(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method36(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method37(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method38(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method39(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method40(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method41(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method42(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method43(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method44(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method45(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method46(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method47(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method48(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method49(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method50(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method51(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method52(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method53(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method54(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method55(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method56(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method57(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method58(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method59(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method60(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method61(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method62(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method63(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method64(void);
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method0()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method0(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method1()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method1(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method2()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method2(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method3()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method3(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method4()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method4(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method5()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method5(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method6()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method6(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method7()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method7(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method8()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method8(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method9()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method9(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method10()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method10(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method11()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method11(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method12()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method12(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method13()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method13(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method14()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method14(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method15()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method15(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method16()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method16(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method17()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method17(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method18()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method18(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method19()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method19(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method20()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method20(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method21()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method21(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method22()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method22(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method23()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method23(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method24()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method24(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method25()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method25(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method26()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method26(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method27()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method27(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method28()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method28(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method29()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method29(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method30()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method30(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method31()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method31(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method32()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method32(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method33()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method33(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method34()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method34(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method35()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method35(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method36()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method36(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method37()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method37(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method38()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method38(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method39()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method39(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method40()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method40(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method41()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method41(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method42()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method42(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method43()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method43(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method44()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method44(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method45()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method45(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method46()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method46(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method47()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method47(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method48()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method48(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method49()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method49(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method50()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method50(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method51()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method51(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method52()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method52(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method53()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method53(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method54()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method54(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method55()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method55(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method56()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method56(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method57()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method57(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method58()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method58(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method59()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method59(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method60()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method60(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method61()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method61(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method62()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method62(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method63()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method63(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Method64()
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method64(void)
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

// Managed method: NumericAggregationNativeEntry/NumericAggregationNativeEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 NumericAggregationNativeEntry_NumericAggregationNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 207) chaos_eval_stack{};
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
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method0();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 1:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method1();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 2:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method2();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 3:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method3();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 4:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method4();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 5:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method5();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 6:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method6();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 7:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method7();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 8:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method8();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 9:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method9();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 10:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method10();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 11:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method11();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 12:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method12();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 13:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method13();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 14:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method14();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 15:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method15();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 16:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method16();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 17:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method17();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 18:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method18();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 19:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method19();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 20:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method20();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 21:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method21();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 22:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method22();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 23:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method23();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 24:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method24();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 25:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method25();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 26:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method26();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 27:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method27();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 28:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method28();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 29:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method29();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 30:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method30();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 31:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method31();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 32:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method32();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 33:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method33();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 34:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method34();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 35:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method35();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 36:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method36();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 37:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method37();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 38:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method38();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 39:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method39();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 40:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method40();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 41:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method41();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 42:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method42();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 43:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method43();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 44:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method44();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 45:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method45();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 46:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method46();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 47:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method47();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 48:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method48();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 49:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method49();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 50:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method50();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 51:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method51();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 52:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method52();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 53:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method53();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 54:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method54();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 55:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method55();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 56:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method56();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 57:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method57();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 58:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method58();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 59:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method59();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 60:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method60();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 61:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method61();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 62:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method62();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 63:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method63();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            case 64:
            {
                {
                    const auto chaos_result = NumericAggregationNativeEntry_NumericAggregationNativeEntry_Method64();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_943;
            }
            default:
            {
                goto chaos_ip_943;
            }
        }
    }
chaos_ip_943: ;
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1);

    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];

    chaos_eval_stack[chaos_stack_top++] = chaos_locals[2];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Native AOT entry for NumericAggregationNativeEntry/NumericAggregationNativeEntry::Run:System.Int32(System.Int32)
// Managed symbol: NumericAggregationNativeEntry_NumericAggregationNativeEntry_Run
extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 chaos_entry_index)
{
    return NumericAggregationNativeEntry_NumericAggregationNativeEntry_Run(chaos_entry_index);
}