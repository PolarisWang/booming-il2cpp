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

extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method14(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method15(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method16(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method17(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_BitConverter__GetBytes_System_Byte___System_Double_(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_BitConverter__GetBytes_System_Byte___System_Int32_(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Math__Abs_System_Double_System_Double_(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INT32>(0);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Math__Abs_System_Int32_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INT32>(0);
}

extern "C" CHAOS_IL2CPP_INT64 chaos_external_runtime_System_Private_CoreLib_System_Math__BigMul_System_Int64_System_Int32_System_Int32_(CHAOS_IL2CPP_INT64 chaos_arg_0, CHAOS_IL2CPP_INT64 chaos_arg_1)
{
    (void)chaos_arg_0; (void)chaos_arg_1;
    return static_cast<CHAOS_IL2CPP_INT64>(0);
}

extern "C" CHAOS_IL2CPP_INT64 chaos_external_runtime_System_Private_CoreLib_System_Math__Ceiling_System_Double_System_Double_(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INT64>(0);
}

extern "C" CHAOS_IL2CPP_INT64 chaos_external_runtime_System_Private_CoreLib_System_Math__Cos_System_Double_System_Double_(CHAOS_IL2CPP_INT64 chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INT64>(0);
}

extern "C" CHAOS_IL2CPP_INT64 chaos_external_runtime_System_Private_CoreLib_System_Math__Floor_System_Double_System_Double_(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INT64>(0);
}

extern "C" CHAOS_IL2CPP_INT64 chaos_external_runtime_System_Private_CoreLib_System_Math__Max_System_Double_System_Double_System_Double_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
    (void)chaos_arg_0; (void)chaos_arg_1;
    return static_cast<CHAOS_IL2CPP_INT64>(0);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Math__Max_System_Int32_System_Int32_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
    (void)chaos_arg_0; (void)chaos_arg_1;
    return static_cast<CHAOS_IL2CPP_INT32>(0);
}

extern "C" CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Math__Min_System_Int32_System_Int32_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
    (void)chaos_arg_0; (void)chaos_arg_1;
    return static_cast<CHAOS_IL2CPP_INT32>(0);
}

extern "C" CHAOS_IL2CPP_INT64 chaos_external_runtime_System_Private_CoreLib_System_Math__Pow_System_Double_System_Double_System_Double_(CHAOS_IL2CPP_INT64 chaos_arg_0, CHAOS_IL2CPP_INT64 chaos_arg_1)
{
    (void)chaos_arg_0; (void)chaos_arg_1;
    return static_cast<CHAOS_IL2CPP_INT64>(0);
}

extern "C" CHAOS_IL2CPP_INT64 chaos_external_runtime_System_Private_CoreLib_System_Math__Round_System_Double_System_Double_(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INT64>(0);
}

extern "C" CHAOS_IL2CPP_INT64 chaos_external_runtime_System_Private_CoreLib_System_Math__Sin_System_Double_System_Double_(CHAOS_IL2CPP_INT64 chaos_arg_0)
{
    (void)chaos_arg_0;
    return static_cast<CHAOS_IL2CPP_INT64>(0);
}

extern "C" CHAOS_IL2CPP_INT64 chaos_external_runtime_System_Private_CoreLib_System_Math__Sqrt_System_Double_System_Double_(CHAOS_IL2CPP_INT64 chaos_arg_0)
{
    return chaos_math_sqrt(chaos_arg_0);
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

extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method14(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method15(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method16(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method17(void);
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);

// Managed method: MathNumericsNativeEntry/MathNumericsNativeEntry::Method0()
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method0(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 6> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Math__Abs_System_Int32_System_Int32_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_11;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: MathNumericsNativeEntry/MathNumericsNativeEntry::Method1()
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method1(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = chaos_store_float64(42);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Math__Abs_System_Double_System_Double_(chaos_load_int64(chaos_arg_0));
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_eval_stack[chaos_stack_top - 1] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[chaos_stack_top - 1]));

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_19;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: MathNumericsNativeEntry/MathNumericsNativeEntry::Method2()
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method2(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = chaos_store_float64(42);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Math__Ceiling_System_Double_System_Double_(chaos_load_int64(chaos_arg_0));
        chaos_eval_stack[chaos_stack_top++] = chaos_store_int64(chaos_result);
    }

    chaos_eval_stack[chaos_stack_top - 1] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[chaos_stack_top - 1]));

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_19;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: MathNumericsNativeEntry/MathNumericsNativeEntry::Method3()
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method3(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = chaos_store_float64(42);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Math__Floor_System_Double_System_Double_(chaos_load_int64(chaos_arg_0));
        chaos_eval_stack[chaos_stack_top++] = chaos_store_int64(chaos_result);
    }

    chaos_eval_stack[chaos_stack_top - 1] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[chaos_stack_top - 1]));

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_19;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: MathNumericsNativeEntry/MathNumericsNativeEntry::Method4()
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method4(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = chaos_store_float64(42);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Math__Round_System_Double_System_Double_(chaos_load_int64(chaos_arg_0));
        chaos_eval_stack[chaos_stack_top++] = chaos_store_int64(chaos_result);
    }

    chaos_eval_stack[chaos_stack_top - 1] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[chaos_stack_top - 1]));

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_19;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: MathNumericsNativeEntry/MathNumericsNativeEntry::Method5()
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method5(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = chaos_store_float64(42);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Math__Round_System_Double_System_Double_(chaos_load_int64(chaos_arg_0));
        chaos_eval_stack[chaos_stack_top++] = chaos_store_int64(chaos_result);
    }

    chaos_eval_stack[chaos_stack_top - 1] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[chaos_stack_top - 1]));

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_19;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: MathNumericsNativeEntry/MathNumericsNativeEntry::Method6()
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method6(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);

    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Math__Max_System_Int32_System_Int32_System_Int32_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_13;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: MathNumericsNativeEntry/MathNumericsNativeEntry::Method7()
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method7(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = chaos_store_float64(42);

    chaos_eval_stack[chaos_stack_top++] = chaos_store_float64(42);

    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Math__Max_System_Double_System_Double_System_Double_(chaos_load_int64(chaos_arg_0), chaos_load_int64(chaos_arg_1));
        chaos_eval_stack[chaos_stack_top++] = chaos_store_int64(chaos_result);
    }

    chaos_eval_stack[chaos_stack_top - 1] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[chaos_stack_top - 1]));

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_28;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: MathNumericsNativeEntry/MathNumericsNativeEntry::Method8()
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method8(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);

    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Math__Min_System_Int32_System_Int32_System_Int32_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_13;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: MathNumericsNativeEntry/MathNumericsNativeEntry::Method9()
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method9(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = chaos_store_float64(42);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Math__Sqrt_System_Double_System_Double_(chaos_load_int64(chaos_arg_0));
        chaos_eval_stack[chaos_stack_top++] = chaos_store_int64(chaos_result);
    }

    chaos_eval_stack[chaos_stack_top - 1] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[chaos_stack_top - 1]));

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_19;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: MathNumericsNativeEntry/MathNumericsNativeEntry::Method10()
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method10(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = chaos_store_float64(42);

    chaos_eval_stack[chaos_stack_top++] = chaos_store_float64(42);

    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Math__Pow_System_Double_System_Double_System_Double_(chaos_load_int64(chaos_arg_0), chaos_load_int64(chaos_arg_1));
        chaos_eval_stack[chaos_stack_top++] = chaos_store_int64(chaos_result);
    }

    chaos_eval_stack[chaos_stack_top - 1] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[chaos_stack_top - 1]));

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_28;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: MathNumericsNativeEntry/MathNumericsNativeEntry::Method11()
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method11(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = chaos_store_float64(42);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Math__Sin_System_Double_System_Double_(chaos_load_int64(chaos_arg_0));
        chaos_eval_stack[chaos_stack_top++] = chaos_store_int64(chaos_result);
    }

    chaos_eval_stack[chaos_stack_top - 1] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[chaos_stack_top - 1]));

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_19;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: MathNumericsNativeEntry/MathNumericsNativeEntry::Method12()
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method12(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = chaos_store_float64(42);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Math__Cos_System_Double_System_Double_(chaos_load_int64(chaos_arg_0));
        chaos_eval_stack[chaos_stack_top++] = chaos_store_int64(chaos_result);
    }

    chaos_eval_stack[chaos_stack_top - 1] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[chaos_stack_top - 1]));

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_19;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: MathNumericsNativeEntry/MathNumericsNativeEntry::Method13()
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method13(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);

    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);

    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Math__BigMul_System_Int64_System_Int32_System_Int32_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
        chaos_eval_stack[chaos_stack_top++] = chaos_store_int64(chaos_result);
    }

    chaos_eval_stack[chaos_stack_top - 1] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[chaos_stack_top - 1]));

    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];

    goto chaos_ip_14;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: MathNumericsNativeEntry/MathNumericsNativeEntry::Method14()
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method14(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_BitConverter__GetBytes_System_Byte___System_Int32_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
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

    goto chaos_ip_13;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: MathNumericsNativeEntry/MathNumericsNativeEntry::Method15()
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method15(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8> chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    chaos_eval_stack[chaos_stack_top++] = chaos_store_float64(42);

    {
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_raw_arg_0;
        const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_BitConverter__GetBytes_System_Byte___System_Double_(chaos_load_int64(chaos_arg_0));
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

    goto chaos_ip_20;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: MathNumericsNativeEntry/MathNumericsNativeEntry::Method16()
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method16(void)
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

// Managed method: MathNumericsNativeEntry/MathNumericsNativeEntry::Method17()
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Method17(void)
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

// Managed method: MathNumericsNativeEntry/MathNumericsNativeEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 MathNumericsNativeEntry_MathNumericsNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1> chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3> chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 66> chaos_eval_stack{};
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
                    const auto chaos_result = MathNumericsNativeEntry_MathNumericsNativeEntry_Method0();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_238;
            }
            case 1:
            {
                {
                    const auto chaos_result = MathNumericsNativeEntry_MathNumericsNativeEntry_Method1();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_238;
            }
            case 2:
            {
                {
                    const auto chaos_result = MathNumericsNativeEntry_MathNumericsNativeEntry_Method2();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_238;
            }
            case 3:
            {
                {
                    const auto chaos_result = MathNumericsNativeEntry_MathNumericsNativeEntry_Method3();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_238;
            }
            case 4:
            {
                {
                    const auto chaos_result = MathNumericsNativeEntry_MathNumericsNativeEntry_Method4();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_238;
            }
            case 5:
            {
                {
                    const auto chaos_result = MathNumericsNativeEntry_MathNumericsNativeEntry_Method5();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_238;
            }
            case 6:
            {
                {
                    const auto chaos_result = MathNumericsNativeEntry_MathNumericsNativeEntry_Method6();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_238;
            }
            case 7:
            {
                {
                    const auto chaos_result = MathNumericsNativeEntry_MathNumericsNativeEntry_Method7();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_238;
            }
            case 8:
            {
                {
                    const auto chaos_result = MathNumericsNativeEntry_MathNumericsNativeEntry_Method8();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_238;
            }
            case 9:
            {
                {
                    const auto chaos_result = MathNumericsNativeEntry_MathNumericsNativeEntry_Method9();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_238;
            }
            case 10:
            {
                {
                    const auto chaos_result = MathNumericsNativeEntry_MathNumericsNativeEntry_Method10();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_238;
            }
            case 11:
            {
                {
                    const auto chaos_result = MathNumericsNativeEntry_MathNumericsNativeEntry_Method11();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_238;
            }
            case 12:
            {
                {
                    const auto chaos_result = MathNumericsNativeEntry_MathNumericsNativeEntry_Method12();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_238;
            }
            case 13:
            {
                {
                    const auto chaos_result = MathNumericsNativeEntry_MathNumericsNativeEntry_Method13();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_238;
            }
            case 14:
            {
                {
                    const auto chaos_result = MathNumericsNativeEntry_MathNumericsNativeEntry_Method14();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_238;
            }
            case 15:
            {
                {
                    const auto chaos_result = MathNumericsNativeEntry_MathNumericsNativeEntry_Method15();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_238;
            }
            case 16:
            {
                {
                    const auto chaos_result = MathNumericsNativeEntry_MathNumericsNativeEntry_Method16();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_238;
            }
            case 17:
            {
                {
                    const auto chaos_result = MathNumericsNativeEntry_MathNumericsNativeEntry_Method17();
                    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
                }
                chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
                goto chaos_ip_238;
            }
            default:
            {
                goto chaos_ip_238;
            }
        }
    }
    goto chaos_ip_238;
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[2];

    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Native AOT entry for MathNumericsNativeEntry/MathNumericsNativeEntry::Run:System.Int32(System.Int32)
// Managed symbol: MathNumericsNativeEntry_MathNumericsNativeEntry_Run
extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 chaos_entry_index)
{
    return MathNumericsNativeEntry_MathNumericsNativeEntry_Run(chaos_entry_index);
}