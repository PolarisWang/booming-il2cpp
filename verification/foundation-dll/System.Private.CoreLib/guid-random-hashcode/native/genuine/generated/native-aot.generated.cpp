#include <chaos/common.h>
#include "runtime_core.h"

namespace
{
static_assert(sizeof(CHAOS_IL2CPP_INTPTR) == sizeof(CHAOS_IL2CPP_UINT64), "native-aot ABI lowering requires 64-bit intptr_t");

constexpr CHAOS_IL2CPP_INTPTR chaos_runtime_managed_pointer_local_slot_tag =
    ChaosIl2cpp::Common::k_managed_pointer_local_slot_tag;
constexpr CHAOS_IL2CPP_INTPTR chaos_raw_int32_pointer_tag = 2;

struct chaos_managed_exception
{
    CHAOS_IL2CPP_INTPTR object_value = 0;
};

template <typename TAction>
struct chaos_finally_scope_guard
{
    explicit chaos_finally_scope_guard(TAction init_action)
        : action(init_action)
    {
    }

    chaos_finally_scope_guard(const chaos_finally_scope_guard&) = delete;
    chaos_finally_scope_guard& operator=(const chaos_finally_scope_guard&) = delete;

    ~chaos_finally_scope_guard()
    {
        if (active)
        {
            action();
        }
    }

    TAction action;
    bool active = true;
};

template <typename TAction>
chaos_finally_scope_guard<TAction> chaos_make_finally_scope_guard(TAction action)
{
    return chaos_finally_scope_guard<TAction>(action);
}

CHAOS_IL2CPP_INTPTR chaos_store_float32(float value) noexcept
{
    CHAOS_IL2CPP_UINT32 bits = 0;
    CHAOS_IL2CPP_MEMCPY(&bits, &value, sizeof(value));
    return static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT64>(bits));
}

float chaos_load_float32(CHAOS_IL2CPP_INTPTR value) noexcept
{
    const auto bits = static_cast<CHAOS_IL2CPP_UINT32>(static_cast<CHAOS_IL2CPP_UINT64>(value));
    float result = 0.0f;
    CHAOS_IL2CPP_MEMCPY(&result, &bits, sizeof(result));
    return result;
}

CHAOS_IL2CPP_INTPTR chaos_store_float64(double value) noexcept
{
    CHAOS_IL2CPP_UINT64 bits = 0;
    CHAOS_IL2CPP_MEMCPY(&bits, &value, sizeof(value));
    CHAOS_IL2CPP_INTPTR result = 0;
    CHAOS_IL2CPP_MEMCPY(&result, &bits, sizeof(result));
    return result;
}

double chaos_load_float64(CHAOS_IL2CPP_INTPTR value) noexcept
{
    CHAOS_IL2CPP_UINT64 bits = 0;
    CHAOS_IL2CPP_MEMCPY(&bits, &value, sizeof(bits));
    double result = 0.0;
    CHAOS_IL2CPP_MEMCPY(&result, &bits, sizeof(result));
    return result;
}

CHAOS_IL2CPP_INTPTR chaos_store_int64(CHAOS_IL2CPP_INT64 value) noexcept
{
    CHAOS_IL2CPP_INTPTR result = 0;
    CHAOS_IL2CPP_MEMCPY(&result, &value, sizeof(result));
    return result;
}

CHAOS_IL2CPP_INT64 chaos_load_int64(CHAOS_IL2CPP_INTPTR value) noexcept
{
    CHAOS_IL2CPP_INT64 result = 0;
    CHAOS_IL2CPP_MEMCPY(&result, &value, sizeof(result));
    return result;
}

CHAOS_IL2CPP_INTPTR chaos_store_uint64(CHAOS_IL2CPP_UINT64 value) noexcept
{
    CHAOS_IL2CPP_INTPTR result = 0;
    CHAOS_IL2CPP_MEMCPY(&result, &value, sizeof(result));
    return result;
}

CHAOS_IL2CPP_UINT64 chaos_load_uint64(CHAOS_IL2CPP_INTPTR value) noexcept
{
    CHAOS_IL2CPP_UINT64 result = 0;
    CHAOS_IL2CPP_MEMCPY(&result, &value, sizeof(result));
    return result;
}

CHAOS_IL2CPP_INT32 chaos_wrap_add(CHAOS_IL2CPP_INT32 left, CHAOS_IL2CPP_INT32 right) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(
        static_cast<CHAOS_IL2CPP_UINT32>(left) + static_cast<CHAOS_IL2CPP_UINT32>(right));
}

CHAOS_IL2CPP_INT32 chaos_wrap_sub(CHAOS_IL2CPP_INT32 left, CHAOS_IL2CPP_INT32 right) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(
        static_cast<CHAOS_IL2CPP_UINT32>(left) - static_cast<CHAOS_IL2CPP_UINT32>(right));
}

CHAOS_IL2CPP_INT32 chaos_wrap_mul(CHAOS_IL2CPP_INT32 left, CHAOS_IL2CPP_INT32 right) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(
        static_cast<CHAOS_IL2CPP_UINT32>(left) * static_cast<CHAOS_IL2CPP_UINT32>(right));
}

CHAOS_IL2CPP_INT32 chaos_div(CHAOS_IL2CPP_INT32 left, CHAOS_IL2CPP_INT32 right)
{
    if (right == 0)
    {
        CHAOS_IL2CPP_ABORT();
    }

    if (left == CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(CHAOS_IL2CPP_INT32) && right == -1)
    {
        CHAOS_IL2CPP_ABORT();
    }

    return static_cast<CHAOS_IL2CPP_INT32>(left / right);
}

CHAOS_IL2CPP_INT32 chaos_rem(CHAOS_IL2CPP_INT32 left, CHAOS_IL2CPP_INT32 right)
{
    if (right == 0)
    {
        CHAOS_IL2CPP_ABORT();
    }

    if (left == CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(CHAOS_IL2CPP_INT32) && right == -1)
    {
        return 0;
    }

    return static_cast<CHAOS_IL2CPP_INT32>(left % right);
}

CHAOS_IL2CPP_INT32 chaos_shift_left_int32(CHAOS_IL2CPP_INT32 value, CHAOS_IL2CPP_INT32 amount) noexcept
{
    const auto shift = static_cast<CHAOS_IL2CPP_UINT32>(amount) & 31U;
    return static_cast<CHAOS_IL2CPP_INT32>(static_cast<CHAOS_IL2CPP_UINT32>(value) << shift);
}

CHAOS_IL2CPP_INT32 chaos_shift_right_int32(CHAOS_IL2CPP_INT32 value, CHAOS_IL2CPP_INT32 amount) noexcept
{
    const auto shift = static_cast<CHAOS_IL2CPP_UINT32>(amount) & 31U;
    if (shift == 0U)
    {
        return value;
    }

    const auto bits = static_cast<CHAOS_IL2CPP_UINT32>(value);
    if (value >= 0)
    {
        return static_cast<CHAOS_IL2CPP_INT32>(bits >> shift);
    }

    const auto fill = CHAOS_IL2CPP_NUMERIC_LIMITS_MAX(CHAOS_IL2CPP_UINT32) << (32U - shift);
    return static_cast<CHAOS_IL2CPP_INT32>((bits >> shift) | fill);
}

CHAOS_IL2CPP_INT32 chaos_shift_right_un_int32(CHAOS_IL2CPP_INT32 value, CHAOS_IL2CPP_INT32 amount) noexcept
{
    const auto shift = static_cast<CHAOS_IL2CPP_UINT32>(amount) & 31U;
    return static_cast<CHAOS_IL2CPP_INT32>(static_cast<CHAOS_IL2CPP_UINT32>(value) >> shift);
}

CHAOS_IL2CPP_INTPTR chaos_checked_conv_ovf_i1(CHAOS_IL2CPP_INTPTR value)
{
    if (value < static_cast<CHAOS_IL2CPP_INTPTR>(CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(CHAOS_IL2CPP_INT8)) ||
        value > static_cast<CHAOS_IL2CPP_INTPTR>(CHAOS_IL2CPP_NUMERIC_LIMITS_MAX(CHAOS_IL2CPP_INT8)))
    {
        CHAOS_IL2CPP_ABORT();
    }

    return static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT8>(value));
}

CHAOS_IL2CPP_INTPTR chaos_checked_conv_ovf_u1(CHAOS_IL2CPP_INTPTR value)
{
    if (value < static_cast<CHAOS_IL2CPP_INTPTR>(0) ||
        value > static_cast<CHAOS_IL2CPP_INTPTR>(CHAOS_IL2CPP_NUMERIC_LIMITS_MAX(CHAOS_IL2CPP_UINT8)))
    {
        CHAOS_IL2CPP_ABORT();
    }

    return static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT8>(value));
}

CHAOS_IL2CPP_INTPTR* chaos_resolve_native_int_slot(CHAOS_IL2CPP_INTPTR chaos_address)
{
    if ((chaos_address & chaos_runtime_managed_pointer_local_slot_tag) != 0)
    {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(
            static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_runtime_managed_pointer_local_slot_tag));
    }

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address);
}

struct chaos_object_header
{
    CHAOS_IL2CPP_INTPTR type_id = 0;
};

constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_managed_array = 1;

struct chaos_managed_array
{
    chaos_object_header header{};
    // compatibility marker: std::CHAOS_IL2CPP_UINT8 element_type_shape = 0;
    CHAOS_IL2CPP_UINT8 element_type_shape = 0;
    CHAOS_IL2CPP_INTPTR element_type_id = 0;
    CHAOS_IL2CPP_INTPTR length = 0;
    CHAOS_IL2CPP_INTPTR* elements = nullptr;
};

constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_reference = 1;
constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_value = 2;
constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_interface = 3;

// compatibility marker: constexpr CHAOS_IL2CPP_INTPTR chaos_managed_pointer_local_slot_tag = 1;
constexpr CHAOS_IL2CPP_INTPTR chaos_managed_pointer_local_slot_tag = ChaosIl2cpp::Common::k_managed_pointer_local_slot_tag;
constexpr CHAOS_IL2CPP_INTPTR chaos_string_id_tag = static_cast<CHAOS_IL2CPP_INTPTR>(1) << (sizeof(CHAOS_IL2CPP_INTPTR) * 8 - 1);
static_assert(sizeof(CHAOS_IL2CPP_INTPTR) == sizeof(CHAOS_IL2CPP_UINT64), "string id tagging requires 64-bit native int");

inline bool chaos_is_string_id(CHAOS_IL2CPP_INTPTR chaos_value) noexcept
{
    return (chaos_value & chaos_string_id_tag) != 0;
}

inline CHAOS_IL2CPP_UINT64 chaos_extract_string_id(CHAOS_IL2CPP_INTPTR chaos_value) noexcept
{
    return static_cast<CHAOS_IL2CPP_UINT64>(chaos_value & ~chaos_string_id_tag);
}

inline CHAOS_IL2CPP_INTPTR chaos_make_string_id_value(CHAOS_IL2CPP_UINT64 chaos_id) noexcept
{
    return chaos_string_id_tag | static_cast<CHAOS_IL2CPP_INTPTR>(chaos_id);
}

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

constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_GuidRandomHashcodeNativeEntry__PrivateImplementationDetails_ = 2;
constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Guid = 3;
constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Random = 4;
constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = 5;
constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Byte = 6;

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
        case chaos_type_id_GuidRandomHashcodeNativeEntry__PrivateImplementationDetails_:
            return false;
        case chaos_type_id_System_Private_CoreLib_System_Guid:
            return false;
        case chaos_type_id_System_Private_CoreLib_System_Random:
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

struct chaos_type_GuidRandomHashcodeNativeEntry__PrivateImplementationDetails_
{
    chaos_object_header header{};
    CHAOS_IL2CPP_INTPTR field_GuidRandomHashcodeNativeEntry__PrivateImplementationDetails___039058C6F2C0CB492C533B0A4D14EF77CC0F78ABCCCED5287D84A1A2011CFB81 = 0;
};

struct chaos_type_System_Private_CoreLib_System_Guid
{
    chaos_object_header header{};
};

struct chaos_type_System_Private_CoreLib_System_Random
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

struct chaos_valuetype_System_Private_CoreLib_System_Byte
{
};

extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);


// StringId constants for compile-time-known string literals.
constexpr CHAOS_IL2CPP_UINT64 chaos_string_id_4155F09940D24CA1 = 4707933526493908129U;

// AOT-baked string table: sorted by StringId for binary search at runtime.
constexpr chaos::il2cpp::string_table::StringEntry chaos_aot_string_entries[] = {
    { chaos_string_id_4155F09940D24CA1, "11111111-1111-1111-1111-111111111111", 36u },
};

constexpr CHAOS_IL2CPP_UINT32 chaos_aot_string_entry_count = sizeof(chaos_aot_string_entries) / sizeof(chaos_aot_string_entries[0]);

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_GuidRandomHashcodeNativeEntry__PrivateImplementationDetails___039058C6F2C0CB492C533B0A4D14EF77CC0F78ABCCCED5287D84A1A2011CFB81[] =
{
    0x01, 0x02, 0x03,
};

bool chaos_try_get_static_field_data(
    CHAOS_IL2CPP_INTPTR chaos_field_handle,
    const CHAOS_IL2CPP_UINT8*& chaos_data,
    CHAOS_IL2CPP_SIZE& chaos_size)
{
    if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(72578864u))
    {
        chaos_data = chaos_static_field_data_GuidRandomHashcodeNativeEntry__PrivateImplementationDetails___039058C6F2C0CB492C533B0A4D14EF77CC0F78ABCCCED5287D84A1A2011CFB81;
        chaos_size = sizeof(chaos_static_field_data_GuidRandomHashcodeNativeEntry__PrivateImplementationDetails___039058C6F2C0CB492C533B0A4D14EF77CC0F78ABCCCED5287D84A1A2011CFB81);
        return true;
    }
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

extern "C" void chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
    chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
}
}

extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0);

// Managed method: GuidRandomHashcodeNativeEntry/GuidRandomHashcodeNativeEntry::Method0()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method0(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_id = chaos_type_id_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(36);
        chaos_string->utf8_data = "11111111-1111-1111-1111-111111111111";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }
    goto chaos_ip_6;

chaos_ip_6:
    {
        auto* chaos_object = new chaos_type_System_Private_CoreLib_System_Guid{};
        chaos_object->header.type_id = chaos_type_id_System_Private_CoreLib_System_Guid;
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    }
    goto chaos_ip_11;

chaos_ip_11:
    chaos_stack_top--;
    goto chaos_ip_12;

chaos_ip_12:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_13;

chaos_ip_13:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_14;

chaos_ip_14:
    goto chaos_ip_16;

chaos_ip_16:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_17;

chaos_ip_17:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: GuidRandomHashcodeNativeEntry/GuidRandomHashcodeNativeEntry::Method1()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method1(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_id = chaos_type_id_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(36);
        chaos_string->utf8_data = "11111111-1111-1111-1111-111111111111";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }
    goto chaos_ip_6;

chaos_ip_6:
    {
        auto* chaos_object = new chaos_type_System_Private_CoreLib_System_Guid{};
        chaos_object->header.type_id = chaos_type_id_System_Private_CoreLib_System_Guid;
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    }
    goto chaos_ip_11;

chaos_ip_11:
    chaos_stack_top--;
    goto chaos_ip_12;

chaos_ip_12:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_13;

chaos_ip_13:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_14;

chaos_ip_14:
    goto chaos_ip_16;

chaos_ip_16:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_17;

chaos_ip_17:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: GuidRandomHashcodeNativeEntry/GuidRandomHashcodeNativeEntry::Method2()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method2(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]) | chaos_managed_pointer_local_slot_tag;
    goto chaos_ip_15;

chaos_ip_15:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_20;

chaos_ip_20:
    chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_21;

chaos_ip_21:
    goto chaos_ip_23;

chaos_ip_23:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];
    goto chaos_ip_24;

chaos_ip_24:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: GuidRandomHashcodeNativeEntry/GuidRandomHashcodeNativeEntry::Method3()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method3(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 9) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_id = chaos_type_id_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(36);
        chaos_string->utf8_data = "11111111-1111-1111-1111-111111111111";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }
    goto chaos_ip_6;

chaos_ip_6:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_11;

chaos_ip_11:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_12;

chaos_ip_12:
    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]) | chaos_managed_pointer_local_slot_tag;
    goto chaos_ip_20;

chaos_ip_20:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_25;

chaos_ip_25:
    chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_26;

chaos_ip_26:
    goto chaos_ip_28;

chaos_ip_28:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];
    goto chaos_ip_29;

chaos_ip_29:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: GuidRandomHashcodeNativeEntry/GuidRandomHashcodeNativeEntry::Method4()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method4(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 11) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    {
        auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
        chaos_string->header.type_id = chaos_type_id_System_Private_CoreLib_System_String;
        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(36);
        chaos_string->utf8_data = "11111111-1111-1111-1111-111111111111";
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
    }
    goto chaos_ip_6;

chaos_ip_6:
    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]) | chaos_managed_pointer_local_slot_tag;
    goto chaos_ip_8;

chaos_ip_8:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_13;

chaos_ip_13:
    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            goto chaos_ip_18;
        }
    }
    goto chaos_ip_15;

chaos_ip_15:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_16;

chaos_ip_16:
    goto chaos_ip_19;

chaos_ip_18:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(1);
    goto chaos_ip_19;

chaos_ip_19:
    chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_20;

chaos_ip_20:
    goto chaos_ip_22;

chaos_ip_22:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];
    goto chaos_ip_23;

chaos_ip_23:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: GuidRandomHashcodeNativeEntry/GuidRandomHashcodeNativeEntry::Method5()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method5(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 14) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_6;

chaos_ip_6:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_7;

chaos_ip_7:
    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]) | chaos_managed_pointer_local_slot_tag;
    goto chaos_ip_15;

chaos_ip_15:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_20;

chaos_ip_20:
    chaos_eval_stack[chaos_stack_top] = chaos_eval_stack[chaos_stack_top - 1];
    chaos_stack_top++;
    goto chaos_ip_21;

chaos_ip_21:
    {
        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];
        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            goto chaos_ip_27;
        }
    }
    goto chaos_ip_23;

chaos_ip_23:
    chaos_stack_top--;
    goto chaos_ip_24;

chaos_ip_24:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_25;

chaos_ip_25:
    goto chaos_ip_32;

chaos_ip_27:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_32;

chaos_ip_32:
    chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_33;

chaos_ip_33:
    goto chaos_ip_35;

chaos_ip_35:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];
    goto chaos_ip_36;

chaos_ip_36:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: GuidRandomHashcodeNativeEntry/GuidRandomHashcodeNativeEntry::Method6()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method6(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    {
        auto* chaos_object = new chaos_type_System_Private_CoreLib_System_Random{};
        chaos_object->header.type_id = chaos_type_id_System_Private_CoreLib_System_Random;
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    }
    goto chaos_ip_6;

chaos_ip_6:
    chaos_stack_top--;
    goto chaos_ip_7;

chaos_ip_7:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_8;

chaos_ip_8:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_9;

chaos_ip_9:
    goto chaos_ip_11;

chaos_ip_11:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_12;

chaos_ip_12:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: GuidRandomHashcodeNativeEntry/GuidRandomHashcodeNativeEntry::Method7()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method7(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 6) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    {
        auto* chaos_object = new chaos_type_System_Private_CoreLib_System_Random{};
        chaos_object->header.type_id = chaos_type_id_System_Private_CoreLib_System_Random;
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    }
    goto chaos_ip_6;

chaos_ip_6:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_11;

chaos_ip_11:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_12;

chaos_ip_12:
    goto chaos_ip_14;

chaos_ip_14:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_15;

chaos_ip_15:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: GuidRandomHashcodeNativeEntry/GuidRandomHashcodeNativeEntry::Method8()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method8(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    {
        auto* chaos_object = new chaos_type_System_Private_CoreLib_System_Random{};
        chaos_object->header.type_id = chaos_type_id_System_Private_CoreLib_System_Random;
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    }
    goto chaos_ip_6;

chaos_ip_6:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);
    goto chaos_ip_8;

chaos_ip_8:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_13;

chaos_ip_13:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_14;

chaos_ip_14:
    goto chaos_ip_16;

chaos_ip_16:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_17;

chaos_ip_17:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: GuidRandomHashcodeNativeEntry/GuidRandomHashcodeNativeEntry::Method9()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method9(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    {
        auto* chaos_object = new chaos_type_System_Private_CoreLib_System_Random{};
        chaos_object->header.type_id = chaos_type_id_System_Private_CoreLib_System_Random;
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    }
    goto chaos_ip_6;

chaos_ip_6:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_11;

chaos_ip_11:
    chaos_eval_stack[chaos_stack_top - 1] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[chaos_stack_top - 1]));
    goto chaos_ip_12;

chaos_ip_12:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_13;

chaos_ip_13:
    goto chaos_ip_15;

chaos_ip_15:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_16;

chaos_ip_16:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: GuidRandomHashcodeNativeEntry/GuidRandomHashcodeNativeEntry::Method10()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method10(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 12) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    {
        auto* chaos_object = new chaos_type_System_Private_CoreLib_System_Random{};
        chaos_object->header.type_id = chaos_type_id_System_Private_CoreLib_System_Random;
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    }
    goto chaos_ip_6;

chaos_ip_6:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(3);
    goto chaos_ip_7;

chaos_ip_7:
    {
        const auto chaos_length = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);
        if (chaos_length < 0)
        {
            CHAOS_IL2CPP_ABORT();
        }
        auto* chaos_array = new chaos_managed_array{};
        chaos_array->header.type_id = chaos_type_id_managed_array;
        chaos_array->element_type_shape = 2;
        chaos_array->element_type_id = chaos_type_id_System_Private_CoreLib_System_Byte;
        chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
        chaos_array->elements = chaos_length == 0 ? nullptr : new CHAOS_IL2CPP_INTPTR[static_cast<CHAOS_IL2CPP_SIZE>(chaos_length)]{};
        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
    }
    goto chaos_ip_12;

chaos_ip_12:
    chaos_eval_stack[chaos_stack_top] = chaos_eval_stack[chaos_stack_top - 1];
    chaos_stack_top++;
    goto chaos_ip_13;

chaos_ip_13:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(72578864u);
    goto chaos_ip_18;

chaos_ip_18:
    {
        const auto chaos_raw_arg_1 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_1 = chaos_raw_arg_1;
        const auto chaos_raw_arg_0 = chaos_eval_stack[--chaos_stack_top];
        const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
        chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(chaos_arg_0, chaos_arg_1);
    }
    goto chaos_ip_23;

chaos_ip_23:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_29;

chaos_ip_29:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_30;

chaos_ip_30:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_31;

chaos_ip_31:
    goto chaos_ip_33;

chaos_ip_33:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_34;

chaos_ip_34:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: GuidRandomHashcodeNativeEntry/GuidRandomHashcodeNativeEntry::Method11()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method11(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]) | chaos_managed_pointer_local_slot_tag;
    goto chaos_ip_3;

chaos_ip_3:
    chaos_eval_stack[chaos_stack_top] = chaos_eval_stack[chaos_stack_top - 1];
    chaos_stack_top++;
    goto chaos_ip_4;

chaos_ip_4:
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
    goto chaos_ip_10;

chaos_ip_10:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);
    goto chaos_ip_12;

chaos_ip_12:
    // Generic execution authority: definition=System.Private.CoreLib/System.HashCode::Add:System.Void(!!0); type=[]; method=[System.Int32]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Private.CoreLib/System.HashCode::Add:System.Void(!!0);type=[];method=[System.Int32]; stub=stub:definition=System.Private.CoreLib/System.HashCode::Add:System.Void(!!0);type=[];method=[System.Int32]
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_18;

chaos_ip_18:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    goto chaos_ip_19;

chaos_ip_19:
    chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_20;

chaos_ip_20:
    goto chaos_ip_22;

chaos_ip_22:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];
    goto chaos_ip_23;

chaos_ip_23:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: GuidRandomHashcodeNativeEntry/GuidRandomHashcodeNativeEntry::Method12()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method12(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 8) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]) | chaos_managed_pointer_local_slot_tag;
    goto chaos_ip_3;

chaos_ip_3:
    chaos_eval_stack[chaos_stack_top] = chaos_eval_stack[chaos_stack_top - 1];
    chaos_stack_top++;
    goto chaos_ip_4;

chaos_ip_4:
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
    goto chaos_ip_10;

chaos_ip_10:
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_15;

chaos_ip_15:
    chaos_locals[1] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_16;

chaos_ip_16:
    goto chaos_ip_18;

chaos_ip_18:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[1];
    goto chaos_ip_19;

chaos_ip_19:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: GuidRandomHashcodeNativeEntry/GuidRandomHashcodeNativeEntry::Method13()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method13(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7) chaos_eval_stack{};
    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


    goto chaos_ip_1;

chaos_ip_1:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);
    goto chaos_ip_3;

chaos_ip_3:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(42);
    goto chaos_ip_5;

chaos_ip_5:
    // Generic execution authority: definition=System.Private.CoreLib/System.HashCode::Combine:System.Int32(!!0,!!1); type=[]; method=[System.Int32, System.Int32]; support=Specialized; specialization=SpecializedBody; body=body:definition=System.Private.CoreLib/System.HashCode::Combine:System.Int32(!!0,!!1);type=[];method=[System.Int32,System.Int32]; stub=stub:definition=System.Private.CoreLib/System.HashCode::Combine:System.Int32(!!0,!!1);type=[];method=[System.Int32,System.Int32]
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(0));
    goto chaos_ip_10;

chaos_ip_10:
    chaos_locals[0] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_11;

chaos_ip_11:
    goto chaos_ip_13;

chaos_ip_13:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[0];
    goto chaos_ip_14;

chaos_ip_14:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Managed method: GuidRandomHashcodeNativeEntry/GuidRandomHashcodeNativeEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_arg_0)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 54) chaos_eval_stack{};
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
                goto chaos_ip_69;
            case 1:
                goto chaos_ip_77;
            case 2:
                goto chaos_ip_85;
            case 3:
                goto chaos_ip_93;
            case 4:
                goto chaos_ip_101;
            case 5:
                goto chaos_ip_109;
            case 6:
                goto chaos_ip_117;
            case 7:
                goto chaos_ip_125;
            case 8:
                goto chaos_ip_133;
            case 9:
                goto chaos_ip_141;
            case 10:
                goto chaos_ip_149;
            case 11:
                goto chaos_ip_157;
            case 12:
                goto chaos_ip_165;
            case 13:
                goto chaos_ip_173;
            default:
                goto chaos_ip_67;
        }
    }

chaos_ip_67:
    goto chaos_ip_181;

chaos_ip_69:
    {
        const auto chaos_result = GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method0();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_74;

chaos_ip_74:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_75;

chaos_ip_75:
    goto chaos_ip_185;

chaos_ip_77:
    {
        const auto chaos_result = GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method1();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_82;

chaos_ip_82:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_83;

chaos_ip_83:
    goto chaos_ip_185;

chaos_ip_85:
    {
        const auto chaos_result = GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method2();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_90;

chaos_ip_90:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_91;

chaos_ip_91:
    goto chaos_ip_185;

chaos_ip_93:
    {
        const auto chaos_result = GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method3();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_98;

chaos_ip_98:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_99;

chaos_ip_99:
    goto chaos_ip_185;

chaos_ip_101:
    {
        const auto chaos_result = GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method4();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_106;

chaos_ip_106:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_107;

chaos_ip_107:
    goto chaos_ip_185;

chaos_ip_109:
    {
        const auto chaos_result = GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method5();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_114;

chaos_ip_114:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_115;

chaos_ip_115:
    goto chaos_ip_185;

chaos_ip_117:
    {
        const auto chaos_result = GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method6();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_122;

chaos_ip_122:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_123;

chaos_ip_123:
    goto chaos_ip_185;

chaos_ip_125:
    {
        const auto chaos_result = GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method7();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_130;

chaos_ip_130:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_131;

chaos_ip_131:
    goto chaos_ip_185;

chaos_ip_133:
    {
        const auto chaos_result = GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method8();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_138;

chaos_ip_138:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_139;

chaos_ip_139:
    goto chaos_ip_185;

chaos_ip_141:
    {
        const auto chaos_result = GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method9();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_146;

chaos_ip_146:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_147;

chaos_ip_147:
    goto chaos_ip_185;

chaos_ip_149:
    {
        const auto chaos_result = GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method10();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_154;

chaos_ip_154:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_155;

chaos_ip_155:
    goto chaos_ip_185;

chaos_ip_157:
    {
        const auto chaos_result = GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method11();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_162;

chaos_ip_162:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_163;

chaos_ip_163:
    goto chaos_ip_185;

chaos_ip_165:
    {
        const auto chaos_result = GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method12();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_170;

chaos_ip_170:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_171;

chaos_ip_171:
    goto chaos_ip_185;

chaos_ip_173:
    {
        const auto chaos_result = GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Method13();
        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
    }
    goto chaos_ip_178;

chaos_ip_178:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_179;

chaos_ip_179:
    goto chaos_ip_185;

chaos_ip_181:
    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
    goto chaos_ip_182;

chaos_ip_182:
    chaos_locals[2] = chaos_eval_stack[--chaos_stack_top];
    goto chaos_ip_183;

chaos_ip_183:
    goto chaos_ip_185;

chaos_ip_185:
    chaos_eval_stack[chaos_stack_top++] = chaos_locals[2];
    goto chaos_ip_186;

chaos_ip_186:
    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);

}

// Native AOT entry for GuidRandomHashcodeNativeEntry/GuidRandomHashcodeNativeEntry::Run:System.Int32(System.Int32)
// Managed symbol: GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Run
extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 chaos_entry_index)
{
    return GuidRandomHashcodeNativeEntry_GuidRandomHashcodeNativeEntry_Run(chaos_entry_index);
}