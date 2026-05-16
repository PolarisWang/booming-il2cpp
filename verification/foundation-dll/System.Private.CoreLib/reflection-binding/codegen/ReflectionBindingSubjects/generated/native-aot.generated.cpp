#include <chaos/common.h>
#include <chaos/type_info.h>
#include "runtime_core.h"
#include "codegen_bridge.h"
#include "module_registry.h"
#include "abi_manifest.h"
#include "hotpatch_table.h"
#include "runtime_vtable.h"
#include "runtime_instantiation.h"
#include "reflection_query_model.h"
#include "load_store_chaos_bridge.h"

// Auto-generated stubs for missing chaos_external_runtime_* functions
// Family: unknown
// These functions are called by generated code but their definitions
// were not emitted by the codegen.
// CHAOS_IL2CPP_* types come from the included headers above.

static CHAOS_IL2CPP_INTPTR chaos_stub_definition_System_Private_CoreLib_System_Array__Empty___0_____type____method__System_Object(void)
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);

}


static CHAOS_IL2CPP_INTPTR chaos_stub_definition_System_Private_CoreLib_System_Array__Empty___0_____type____method__System_Type(void)
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);

}



// Forward declaration for dispatch table entries (defined in runtime_stubs.cpp)
extern "C" void InterpreterEntryDirect(
    CHAOS_IL2CPP_UINTPTR method_key,
    void*     args_buf,
    void*     ret_buf) noexcept;

#pragma warning(push)
#pragma warning(disable: 4065 4244)

namespace chaos::il2cpp::codegen::ReflectionBindingSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level



constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_managed_array = 1;
inline TypeInfo chaos_type_info_managed_array = { nullptr, nullptr, 1ULL, 0, 32, 2, 0 };

struct chaos_managed_array
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_UINT8 element_type_shape = 0;
	const TypeInfo* element_type_info = nullptr;
	CHAOS_IL2CPP_INTPTR length = 0;
	CHAOS_IL2CPP_INTPTR* elements = nullptr;
};

constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_reference = 1;
constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_value = 2;
constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_interface = 3;

#ifdef CHAOS_IL2CPP_VERIFY_MODE
constexpr CHAOS_IL2CPP_INTPTR chaos_managed_pointer_local_slot_tag = 0;
#else
constexpr CHAOS_IL2CPP_INTPTR chaos_managed_pointer_local_slot_tag = 1;
#endif

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
			*chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(CHAOS_IL2CPP_NEW_GC(TValue));
		}
		return reinterpret_cast<TValue*>(*chaos_slot);
	}

	return reinterpret_cast<TValue*>(chaos_managed_pointer);
}

inline TypeInfoV0 chaos_type_info_v0_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32 /* warm_delta */, 1 /* reference */, 1}, {nullptr, nullptr, 0, 0, 0, 0 }};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);inline TypeInfoV0 chaos_type_info_v0_ReflectionBindingSubjects_ReflectionBindingSubjects = {{&chaos_type_info_v0_System_Private_CoreLib_System_Object.hot, nullptr, 15879193500755149464ULL, 0u, 32 /* warm_delta */, 1 /* reference */, 1}, {nullptr, nullptr, 0, 0, 0, 0 }};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ReflectionBindingSubjects_ReflectionBindingSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(15879193500755149464ULL);inline TypeInfoV0 chaos_type_info_v0_System_Private_CoreLib_System_Reflection_Assembly = {{nullptr, nullptr, 5474029880995115448ULL, 0u, 32 /* warm_delta */, 1 /* reference */, 1}, {nullptr, nullptr, 0, 0, 0, 0 }};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly = static_cast<CHAOS_IL2CPP_INTPTR>(5474029880995115448ULL);inline TypeInfoV0 chaos_type_info_v0_System_Private_CoreLib_System_Reflection_AssemblyName = {{nullptr, nullptr, 17082367815459723707ULL, 0u, 32 /* warm_delta */, 1 /* reference */, 1}, {nullptr, nullptr, 0, 0, 0, 0 }};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = static_cast<CHAOS_IL2CPP_INTPTR>(17082367815459723707ULL);inline TypeInfoV0 chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ConstructorInfo = {{nullptr, nullptr, 4137207361503509124ULL, 0u, 32 /* warm_delta */, 1 /* reference */, 1}, {nullptr, nullptr, 0, 0, 0, 0 }};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo = static_cast<CHAOS_IL2CPP_INTPTR>(4137207361503509124ULL);inline TypeInfoV0 chaos_type_info_v0_System_Private_CoreLib_System_Reflection_FieldInfo = {{&chaos_type_info_v0_System_Private_CoreLib_System_Object.hot, nullptr, 17040031516751226236ULL, 0u, 32 /* warm_delta */, 1 /* reference */, 1}, {nullptr, nullptr, 0, 0, 0, 0 }};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo = static_cast<CHAOS_IL2CPP_INTPTR>(17040031516751226236ULL);inline TypeInfoV0 chaos_type_info_v0_System_Private_CoreLib_System_Reflection_MethodInfo = {{&chaos_type_info_v0_System_Private_CoreLib_System_Object.hot, nullptr, 10748947813473285525ULL, 0u, 32 /* warm_delta */, 1 /* reference */, 1}, {nullptr, nullptr, 0, 0, 0, 0 }};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = static_cast<CHAOS_IL2CPP_INTPTR>(10748947813473285525ULL);inline TypeInfoV0 chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ParameterInfo = {{&chaos_type_info_v0_System_Private_CoreLib_System_Object.hot, nullptr, 10691557903995528663ULL, 0u, 32 /* warm_delta */, 1 /* reference */, 1}, {nullptr, nullptr, 0, 0, 0, 0 }};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_ParameterInfo = static_cast<CHAOS_IL2CPP_INTPTR>(10691557903995528663ULL);inline TypeInfoV0 chaos_type_info_v0_System_Private_CoreLib_System_String = {{nullptr, nullptr, 1782325859292956794ULL, 0u, 32 /* warm_delta */, 1 /* reference */, 1}, {nullptr, nullptr, 0, 0, 0, 0 }};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_INTPTR>(1782325859292956794ULL);inline TypeInfoV0 chaos_type_info_v0_System_Private_CoreLib_System_Type = {{&chaos_type_info_v0_System_Private_CoreLib_System_Object.hot, nullptr, 3222174513575444759ULL, 0u, 32 /* warm_delta */, 1 /* reference */, 1}, {nullptr, nullptr, 0, 0, 0, 0 }};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Type = static_cast<CHAOS_IL2CPP_INTPTR>(3222174513575444759ULL);inline TypeInfoV0 chaos_type_info_v0_System_Private_CoreLib_System_Int32 = {{nullptr, nullptr, 11009693519287992193ULL, 0u, 32 /* warm_delta */, 2 /* value (boxed) */, 0}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_boxed_type_id_System_Private_CoreLib_System_Int32 = static_cast<CHAOS_IL2CPP_INTPTR>(11009693519287992193ULL);

// ── Virtual method table arrays ──
inline void* chaos_vtable_resolve(const void** vtable, CHAOS_IL2CPP_UINT32 slot) noexcept
{
	if (vtable == nullptr) CHAOS_IL2CPP_FAIL();
	if (vtable[slot] == nullptr) CHAOS_IL2CPP_FAIL();
	return const_cast<void*>(vtable[slot]);
}

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
		if (chaos_current == chaos_target_type_info || chaos_current->stable_id == chaos_target_type_info->stable_id)
		{
			return true;
		}

		chaos_current = chaos_current->parent;
	}

	return false;
}

bool chaos_type_implements_interface(const TypeInfo* chaos_actual_type_info, const TypeInfo* chaos_target_interface_type_info) noexcept
{
	if (chaos_actual_type_info == nullptr || chaos_target_interface_type_info == nullptr)
	{
		return false;
	}

	const auto* chaos_warm = GetWarmPtr(chaos_actual_type_info);
	if (chaos_warm == nullptr) return false;

	if (chaos_warm->iface_count == 0 &&
		chaos_warm->runtime_iface_count == 0)
	{
		return false;
	}

	for (CHAOS_IL2CPP_UINT32 chaos_i = 0; chaos_i < chaos_warm->iface_count; chaos_i++)
	{
		if (chaos_warm->iface_map[chaos_i].iface_stable_id == chaos_target_interface_type_info->stable_id)
		{
			return true;
		}
	}

	for (CHAOS_IL2CPP_UINT32 chaos_i = 0; chaos_i < chaos_warm->runtime_iface_count; chaos_i++)
	{
		if (chaos_warm->runtime_iface_map[chaos_i].iface_stable_id == chaos_target_interface_type_info->stable_id)
		{
			return true;
		}
	}

	return false;
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
		return chaos_array->element_type_shape == chaos_type_shape_reference            && chaos_is_type_compatible(&chaos_type_info_v0_System_Private_CoreLib_System_String.hot, chaos_array->element_type_info);
	}

	auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_value);
	if (chaos_array->element_type_shape == chaos_type_shape_interface)
	{
		return chaos_does_type_implement_interface(chaos_object_get_type_info(chaos_header), chaos_array->element_type_info);
	}

	if (chaos_array->element_type_shape == chaos_type_shape_reference)
	{
		return chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_array->element_type_info);
	}

	return false;
}

struct chaos_type_System_Private_CoreLib_System_Object
{
	ThinLockableHeader header{};
};

struct chaos_type_ReflectionBindingSubjects_ReflectionBindingSubjects : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_type_System_Private_CoreLib_System_Reflection_Assembly
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR runtime_assembly_name_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR declaring_type_handle = 0;
	CHAOS_IL2CPP_INTPTR runtime_method_handle = 0;
	CHAOS_IL2CPP_INTPTR generic_definition_method_handle = 0;
	CHAOS_IL2CPP_INTPTR generic_argument_type_handle = 0;
	CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
	CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR declaring_type_handle = 0;
	CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
	CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR declaring_type_handle = 0;
	CHAOS_IL2CPP_INTPTR runtime_method_handle = 0;
	CHAOS_IL2CPP_INTPTR generic_definition_method_handle = 0;
	CHAOS_IL2CPP_INTPTR generic_argument_type_handle = 0;
	CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
	CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_Reflection_ParameterInfo : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_String
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR length = 0;
	const char* utf8_data = nullptr;
	CHAOS_IL2CPP_UINT64 string_id = 0u;  // stable StringId, 0 = not yet materialized
};

struct chaos_type_System_Private_CoreLib_System_Type : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR runtime_type_handle = 0;
	CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
	CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;
};

struct chaos_boxed_type_System_Private_CoreLib_System_Int32
{
	PureTypeHeader header{};
	CHAOS_IL2CPP_INTPTR value = 0;
};

const char* chaos_reflection_get_string_utf8(CHAOS_IL2CPP_INTPTR chaos_string_value) noexcept
{
	if (chaos_string_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))
	{
		return nullptr;
	}

	if (chaos_is_string_id(chaos_string_value))
	{
		const auto chaos_view = chaos::il2cpp::string_table::Resolve(
			chaos_extract_string_id(chaos_string_value));
		return chaos_view.utf8_data;
	}

	auto* chaos_string = reinterpret_cast<chaos_type_System_Private_CoreLib_System_String*>(chaos_string_value);
	return chaos_string->utf8_data;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_string_utf8_copy(const char* chaos_utf8_data, CHAOS_IL2CPP_SIZE chaos_length)
{
	if (chaos_utf8_data == nullptr)
	{
		return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}

	const auto chaos_id = chaos::il2cpp::string_table::Intern(
		chaos_utf8_data, static_cast<CHAOS_IL2CPP_UINT32>(chaos_length));
	return chaos_make_string_id_value(chaos_id);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionConcatStringPairValues(CHAOS_IL2CPP_INTPTR chaos_left_string_value, CHAOS_IL2CPP_INTPTR chaos_right_string_value)
{
	const auto chaos_left_length =
		chaos_left_string_value == static_cast<CHAOS_IL2CPP_INTPTR>(0)
			? static_cast<CHAOS_IL2CPP_SIZE>(0)
			: chaos_is_string_id(chaos_left_string_value)
				? static_cast<CHAOS_IL2CPP_SIZE>(chaos::il2cpp::string_table::Resolve(
					chaos_extract_string_id(chaos_left_string_value)).byte_count)
				: static_cast<CHAOS_IL2CPP_SIZE>(reinterpret_cast<chaos_type_System_Private_CoreLib_System_String*>(chaos_left_string_value)->length);
	const auto chaos_right_length =
		chaos_right_string_value == static_cast<CHAOS_IL2CPP_INTPTR>(0)
			? static_cast<CHAOS_IL2CPP_SIZE>(0)
			: chaos_is_string_id(chaos_right_string_value)
				? static_cast<CHAOS_IL2CPP_SIZE>(chaos::il2cpp::string_table::Resolve(
					chaos_extract_string_id(chaos_right_string_value)).byte_count)
				: static_cast<CHAOS_IL2CPP_SIZE>(reinterpret_cast<chaos_type_System_Private_CoreLib_System_String*>(chaos_right_string_value)->length);
	const char* chaos_left_utf8 = chaos_reflection_get_string_utf8(chaos_left_string_value);
	const char* chaos_right_utf8 = chaos_reflection_get_string_utf8(chaos_right_string_value);
	if ((chaos_left_string_value != static_cast<CHAOS_IL2CPP_INTPTR>(0) && chaos_left_utf8 == nullptr && chaos_left_length != 0) ||
		(chaos_right_string_value != static_cast<CHAOS_IL2CPP_INTPTR>(0) && chaos_right_utf8 == nullptr && chaos_right_length != 0))
	{
		CHAOS_IL2CPP_FAIL();
	}

	const auto chaos_combined_length = chaos_left_length + chaos_right_length;
	auto* chaos_combined_utf8 = new char[chaos_combined_length + 1];
	if (chaos_left_length != 0)
	{
		CHAOS_IL2CPP_MEMCPY(chaos_combined_utf8, chaos_left_utf8, chaos_left_length);
	}
	if (chaos_right_length != 0)
	{
		CHAOS_IL2CPP_MEMCPY(chaos_combined_utf8 + chaos_left_length, chaos_right_utf8, chaos_right_length);
	}
	chaos_combined_utf8[chaos_combined_length] = '\0';
	const auto chaos_result = chaos_reflection_create_string_utf8_copy(chaos_combined_utf8, chaos_combined_length);
	delete[] chaos_combined_utf8;
	return chaos_result;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_string_literal(const char* chaos_utf8_data)
{
	if (chaos_utf8_data == nullptr)
	{
		return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}

	return chaos_reflection_create_string_utf8_copy(chaos_utf8_data, CHAOS_IL2CPP_STRLEN(chaos_utf8_data));
}


CHAOS_IL2CPP_INTPTR chaos_string_materialize(CHAOS_IL2CPP_INTPTR chaos_value) noexcept
{
	if (chaos_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))
	{
		return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}

	if (!chaos_is_string_id(chaos_value))
	{
		return chaos_value;
	}

	const auto chaos_id = chaos_extract_string_id(chaos_value);
	const auto chaos_view = chaos::il2cpp::string_table::Resolve(chaos_id);

	auto* chaos_string = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_String, {});
	chaos_string->header.type_info = &chaos_type_info_v0_System_Private_CoreLib_System_String.hot;
	chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_view.byte_count);
	auto* owned_utf8 = static_cast<char*>(chaos::il2cpp::runtime_core::GcAllocateAtomic(chaos_view.byte_count + 1));
	CHAOS_IL2CPP_MEMCPY(owned_utf8, chaos_view.utf8_data, chaos_view.byte_count);
	owned_utf8[chaos_view.byte_count] = '\0';
	chaos_string->utf8_data = owned_utf8;
	chaos_string->string_id = chaos_id;
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_reference_array(const TypeInfo* chaos_element_type_info, CHAOS_IL2CPP_SIZE chaos_length)
{
	auto* chaos_array = CHAOS_IL2CPP_NEW_GC(chaos_managed_array);
	chaos_array->header.type_info = &chaos_type_info_managed_array;
	chaos_array->element_type_shape = chaos_type_shape_reference;
	chaos_array->element_type_info = chaos_element_type_info;
	chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
	chaos_array->elements = chaos_length == 0 ? nullptr : static_cast<CHAOS_IL2CPP_INTPTR*>(CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, chaos_length));
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_name_value_from_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept
{
	switch (chaos_type_handle)
	{
		case static_cast<CHAOS_IL2CPP_INTPTR>(46055704u):
			return chaos_reflection_create_string_literal("ReflectionBindingSubjects");
		default:
			return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}
}

CHAOS_IL2CPP_INT32 chaos_reflection_get_type_metadata_token_from_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept
{
	switch (chaos_type_handle)
	{
		case static_cast<CHAOS_IL2CPP_INTPTR>(46055704u):
			return 33554434;
		default:
			return 0;
	}
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_type_value(CHAOS_IL2CPP_INTPTR chaos_type_handle)
{
	auto* chaos_type = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Type);
	chaos_type->header.type_info = &chaos_type_info_v0_System_Private_CoreLib_System_Type.hot;
	chaos_type->runtime_type_handle = chaos_type_handle;
	chaos_type->runtime_name_value = chaos_reflection_get_type_name_value_from_handle(chaos_type_handle);
	chaos_type->runtime_metadata_token_value = chaos_reflection_get_type_metadata_token_from_handle(chaos_type_handle);
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_type);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethod_name_value_from_handle(CHAOS_IL2CPP_INTPTR chaos_method_handle) noexcept
{
	switch (chaos_method_handle)
	{
		case static_cast<CHAOS_IL2CPP_INTPTR>(108502934u):
			return chaos_reflection_create_string_literal("Run");
		case static_cast<CHAOS_IL2CPP_INTPTR>(116605137u):
			return chaos_reflection_create_string_literal("Subject_0");
		case static_cast<CHAOS_IL2CPP_INTPTR>(112732858u):
			return chaos_reflection_create_string_literal("Subject_10");
		case static_cast<CHAOS_IL2CPP_INTPTR>(102167601u):
			return chaos_reflection_create_string_literal("Subject_11");
		case static_cast<CHAOS_IL2CPP_INTPTR>(107793242u):
			return chaos_reflection_create_string_literal("Subject_1");
		case static_cast<CHAOS_IL2CPP_INTPTR>(111032951u):
			return chaos_reflection_create_string_literal("Subject_2");
		case static_cast<CHAOS_IL2CPP_INTPTR>(107950600u):
			return chaos_reflection_create_string_literal("Subject_3");
		case static_cast<CHAOS_IL2CPP_INTPTR>(103640021u):
			return chaos_reflection_create_string_literal("Subject_4");
		case static_cast<CHAOS_IL2CPP_INTPTR>(105628414u):
			return chaos_reflection_create_string_literal("Subject_5");
		case static_cast<CHAOS_IL2CPP_INTPTR>(116939051u):
			return chaos_reflection_create_string_literal("Subject_6");
		case static_cast<CHAOS_IL2CPP_INTPTR>(104854460u):
			return chaos_reflection_create_string_literal("Subject_7");
		case static_cast<CHAOS_IL2CPP_INTPTR>(114394009u):
			return chaos_reflection_create_string_literal("Subject_8");
		case static_cast<CHAOS_IL2CPP_INTPTR>(103511842u):
			return chaos_reflection_create_string_literal("Subject_9");
		default:
			return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}
}

CHAOS_IL2CPP_INT32 ChaosReflectionGetMethod_metadata_token_from_handle(CHAOS_IL2CPP_INTPTR chaos_method_handle) noexcept
{
	switch (chaos_method_handle)
	{
		case static_cast<CHAOS_IL2CPP_INTPTR>(108502934u):
			return 100663309;
		case static_cast<CHAOS_IL2CPP_INTPTR>(116605137u):
			return 100663297;
		case static_cast<CHAOS_IL2CPP_INTPTR>(112732858u):
			return 100663307;
		case static_cast<CHAOS_IL2CPP_INTPTR>(102167601u):
			return 100663308;
		case static_cast<CHAOS_IL2CPP_INTPTR>(107793242u):
			return 100663298;
		case static_cast<CHAOS_IL2CPP_INTPTR>(111032951u):
			return 100663299;
		case static_cast<CHAOS_IL2CPP_INTPTR>(107950600u):
			return 100663300;
		case static_cast<CHAOS_IL2CPP_INTPTR>(103640021u):
			return 100663301;
		case static_cast<CHAOS_IL2CPP_INTPTR>(105628414u):
			return 100663302;
		case static_cast<CHAOS_IL2CPP_INTPTR>(116939051u):
			return 100663303;
		case static_cast<CHAOS_IL2CPP_INTPTR>(104854460u):
			return 100663304;
		case static_cast<CHAOS_IL2CPP_INTPTR>(114394009u):
			return 100663305;
		case static_cast<CHAOS_IL2CPP_INTPTR>(103511842u):
			return 100663306;
		default:
			return 0;
	}
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetDeclaringType_handle_from_type_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept
{
	switch (chaos_type_handle)
	{
		default:
			return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetMemberName(CHAOS_IL2CPP_INTPTR chaos_member_value) noexcept
{
	if (chaos_member_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))
	{
		return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}

	auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_member_value);
	if (chaos_object_get_type_info(chaos_header) == &chaos_type_info_v0_System_Private_CoreLib_System_Type.hot
		|| chaos_object_get_type_info(chaos_header)->stable_id == (&chaos_type_info_v0_System_Private_CoreLib_System_Type.hot)->stable_id)
	{
		auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_member_value);
		return chaos_type->runtime_name_value == static_cast<CHAOS_IL2CPP_INTPTR>(0)
			? chaos_reflection_get_type_name_value_from_handle(chaos_type->runtime_type_handle)
			: chaos_type->runtime_name_value;
	}

	if (chaos_object_get_type_info(chaos_header) == &chaos_type_info_v0_System_Private_CoreLib_System_Reflection_MethodInfo.hot
		|| chaos_object_get_type_info(chaos_header)->stable_id == (&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_MethodInfo.hot)->stable_id)
	{
		auto* chaos_method = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo*>(chaos_member_value);
		return chaos_method->runtime_name_value == static_cast<CHAOS_IL2CPP_INTPTR>(0)
			? ChaosReflectionGetMethod_name_value_from_handle(chaos_method->runtime_method_handle)
			: chaos_method->runtime_name_value;
	}

	if (chaos_object_get_type_info(chaos_header) == &chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ConstructorInfo.hot
		|| chaos_object_get_type_info(chaos_header)->stable_id == (&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ConstructorInfo.hot)->stable_id)
	{
		auto* chaos_method = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo*>(chaos_member_value);
		return chaos_method->runtime_name_value == static_cast<CHAOS_IL2CPP_INTPTR>(0)
			? ChaosReflectionGetMethod_name_value_from_handle(chaos_method->runtime_method_handle)
			: chaos_method->runtime_name_value;
	}

	if (chaos_object_get_type_info(chaos_header) == &chaos_type_info_v0_System_Private_CoreLib_System_Reflection_FieldInfo.hot
		|| chaos_object_get_type_info(chaos_header)->stable_id == (&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_FieldInfo.hot)->stable_id)
	{
		auto* chaos_field = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo*>(chaos_member_value);
		return chaos_field->runtime_name_value;
	}

	return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetDeclaringType(CHAOS_IL2CPP_INTPTR chaos_member_value)
{
	if (chaos_member_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))
	{
		return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}

	auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_member_value);
	CHAOS_IL2CPP_INTPTR chaos_declaring_type_handle = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	if (chaos_object_get_type_info(chaos_header) == &chaos_type_info_v0_System_Private_CoreLib_System_Type.hot
		|| chaos_object_get_type_info(chaos_header)->stable_id == (&chaos_type_info_v0_System_Private_CoreLib_System_Type.hot)->stable_id)
	{
		auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_member_value);
		chaos_declaring_type_handle = ChaosReflectionGetDeclaringType_handle_from_type_handle(chaos_type->runtime_type_handle);
	}
	else if (chaos_object_get_type_info(chaos_header) == &chaos_type_info_v0_System_Private_CoreLib_System_Reflection_MethodInfo.hot
		|| chaos_object_get_type_info(chaos_header)->stable_id == (&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_MethodInfo.hot)->stable_id)
	{
		auto* chaos_method = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo*>(chaos_member_value);
		chaos_declaring_type_handle = chaos_method->declaring_type_handle;
	}
	else if (chaos_object_get_type_info(chaos_header) == &chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ConstructorInfo.hot
		|| chaos_object_get_type_info(chaos_header)->stable_id == (&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ConstructorInfo.hot)->stable_id)
	{
		auto* chaos_method = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo*>(chaos_member_value);
		chaos_declaring_type_handle = chaos_method->declaring_type_handle;
	}
	else if (chaos_object_get_type_info(chaos_header) == &chaos_type_info_v0_System_Private_CoreLib_System_Reflection_FieldInfo.hot
		|| chaos_object_get_type_info(chaos_header)->stable_id == (&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_FieldInfo.hot)->stable_id)
	{
		auto* chaos_field = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo*>(chaos_member_value);
		chaos_declaring_type_handle = chaos_field->declaring_type_handle;
	}

	return chaos_declaring_type_handle == static_cast<CHAOS_IL2CPP_INTPTR>(0)
		? static_cast<CHAOS_IL2CPP_INTPTR>(0)
		: chaos_reflection_create_type_value(chaos_declaring_type_handle);
}

CHAOS_IL2CPP_INT32 ChaosReflectionGetMetadataToken(CHAOS_IL2CPP_INTPTR chaos_member_value) noexcept
{
	if (chaos_member_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))
	{
		return 0;
	}

	auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_member_value);
	if (chaos_object_get_type_info(chaos_header) == &chaos_type_info_v0_System_Private_CoreLib_System_Type.hot
		|| chaos_object_get_type_info(chaos_header)->stable_id == (&chaos_type_info_v0_System_Private_CoreLib_System_Type.hot)->stable_id)
	{
		auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_member_value);
		return chaos_type->runtime_metadata_token_value != 0
			? chaos_type->runtime_metadata_token_value
			: chaos_reflection_get_type_metadata_token_from_handle(chaos_type->runtime_type_handle);
	}

	if (chaos_object_get_type_info(chaos_header) == &chaos_type_info_v0_System_Private_CoreLib_System_Reflection_MethodInfo.hot
		|| chaos_object_get_type_info(chaos_header)->stable_id == (&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_MethodInfo.hot)->stable_id)
	{
		auto* chaos_method = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo*>(chaos_member_value);
		const auto chaos_lookup_handle = chaos_method->generic_definition_method_handle != static_cast<CHAOS_IL2CPP_INTPTR>(0)
			? chaos_method->generic_definition_method_handle
			: chaos_method->runtime_method_handle;
		return chaos_method->runtime_metadata_token_value != 0
			? chaos_method->runtime_metadata_token_value
			: ChaosReflectionGetMethod_metadata_token_from_handle(chaos_lookup_handle);
	}

	if (chaos_object_get_type_info(chaos_header) == &chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ConstructorInfo.hot
		|| chaos_object_get_type_info(chaos_header)->stable_id == (&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ConstructorInfo.hot)->stable_id)
	{
		auto* chaos_method = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo*>(chaos_member_value);
		const auto chaos_lookup_handle = chaos_method->generic_definition_method_handle != static_cast<CHAOS_IL2CPP_INTPTR>(0)
			? chaos_method->generic_definition_method_handle
			: chaos_method->runtime_method_handle;
		return chaos_method->runtime_metadata_token_value != 0
			? chaos_method->runtime_metadata_token_value
			: ChaosReflectionGetMethod_metadata_token_from_handle(chaos_lookup_handle);
	}

	if (chaos_object_get_type_info(chaos_header) == &chaos_type_info_v0_System_Private_CoreLib_System_Reflection_FieldInfo.hot
		|| chaos_object_get_type_info(chaos_header)->stable_id == (&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_FieldInfo.hot)->stable_id)
	{
		auto* chaos_field = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo*>(chaos_member_value);
		return chaos_field->runtime_metadata_token_value;
	}

	return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetParameterName(CHAOS_IL2CPP_INTPTR chaos_parameter_value) noexcept
{
	if (chaos_parameter_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))
	{
		return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}

	auto* chaos_parameter = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_ParameterInfo*>(chaos_parameter_value);
	return chaos_parameter->runtime_name_value;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetField(CHAOS_IL2CPP_INTPTR chaos_type_value, CHAOS_IL2CPP_INTPTR chaos_name_value)
{
	auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_type_value);
	const auto* chaos_field_name = chaos_reflection_get_string_utf8(chaos_name_value);
	if (chaos_type == nullptr || chaos_field_name == nullptr)
	{
		return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}

	switch (chaos_type->runtime_type_handle)
	{
		case static_cast<CHAOS_IL2CPP_INTPTR>(46055704u):
			if (CHAOS_IL2CPP_STRCMP(chaos_field_name, "_exitCode") == 0)
			{
				auto* chaos_field = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo);
				chaos_field->header.type_info = &chaos_type_info_v0_System_Private_CoreLib_System_Reflection_FieldInfo.hot;
				chaos_field->declaring_type_handle = chaos_type->runtime_type_handle;
				chaos_field->runtime_name_value = chaos_reflection_create_string_literal("_exitCode");
				chaos_field->runtime_metadata_token_value = 67108865;
				return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_field);
			}

			break;
		default:
			break;
	}

	return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetGenericTypeDefinition(CHAOS_IL2CPP_INTPTR chaos_type_value)
{
	auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_type_value);
	if (chaos_type == nullptr)
	{
		return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}

	switch (chaos_type->runtime_type_handle)
	{
		default:
			return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetGenericArguments(CHAOS_IL2CPP_INTPTR chaos_type_value)
{
	auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_type_value);
	if (chaos_type == nullptr)
	{
		return chaos_reflection_create_reference_array(
			&chaos_type_info_v0_System_Private_CoreLib_System_Type.hot,
			static_cast<CHAOS_IL2CPP_SIZE>(0));
	}

	switch (chaos_type->runtime_type_handle)
	{
		default:
			return chaos_reflection_create_reference_array(
				&chaos_type_info_v0_System_Private_CoreLib_System_Type.hot,
				static_cast<CHAOS_IL2CPP_SIZE>(0));
	}
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetConstructors(CHAOS_IL2CPP_INTPTR chaos_type_value, CHAOS_IL2CPP_INT32 chaos_binding_flags)
{
	(void)chaos_binding_flags;
	auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_type_value);
	if (chaos_type == nullptr)
	{
		return chaos_reflection_create_reference_array(
			&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ConstructorInfo.hot,
			static_cast<CHAOS_IL2CPP_SIZE>(0));
	}

	switch (chaos_type->runtime_type_handle)
	{
		default:
			return chaos_reflection_create_reference_array(
				&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ConstructorInfo.hot,
				static_cast<CHAOS_IL2CPP_SIZE>(0));
	}
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetParameters(CHAOS_IL2CPP_INTPTR chaos_method_value)
{
	if (chaos_method_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))
	{
		return chaos_reflection_create_reference_array(
			&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ParameterInfo.hot,
			static_cast<CHAOS_IL2CPP_SIZE>(0));
	}

	auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_method_value);
	CHAOS_IL2CPP_INTPTR chaos_method_handle = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	if (chaos_object_get_type_info(chaos_header) == &chaos_type_info_v0_System_Private_CoreLib_System_Reflection_MethodInfo.hot
		|| chaos_object_get_type_info(chaos_header)->stable_id == (&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_MethodInfo.hot)->stable_id)
	{
		auto* chaos_method = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo*>(chaos_method_value);
		chaos_method_handle = chaos_method->generic_definition_method_handle != static_cast<CHAOS_IL2CPP_INTPTR>(0)
			? chaos_method->generic_definition_method_handle
			: chaos_method->runtime_method_handle;
	}
	else if (chaos_object_get_type_info(chaos_header) == &chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ConstructorInfo.hot
		|| chaos_object_get_type_info(chaos_header)->stable_id == (&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ConstructorInfo.hot)->stable_id)
	{
		auto* chaos_method = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo*>(chaos_method_value);
		chaos_method_handle = chaos_method->runtime_method_handle;
	}

	switch (chaos_method_handle)
	{
		case static_cast<CHAOS_IL2CPP_INTPTR>(108502934u):
		{
			const auto chaos_array_value = chaos_reflection_create_reference_array(&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ParameterInfo.hot, static_cast<CHAOS_IL2CPP_SIZE>(1));
			auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_array_value);
			auto* chaos_parameter_0 = new chaos_type_System_Private_CoreLib_System_Reflection_ParameterInfo{};
			chaos_parameter_0->header.type_info = &chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ParameterInfo.hot;
			chaos_parameter_0->runtime_name_value = chaos_reflection_create_string_literal("entryIndex");
			chaos_array->elements[0] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_parameter_0);
			return chaos_array_value;
		}
		case static_cast<CHAOS_IL2CPP_INTPTR>(116605137u):
		{
			const auto chaos_array_value = chaos_reflection_create_reference_array(&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ParameterInfo.hot, static_cast<CHAOS_IL2CPP_SIZE>(0));
			auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_array_value);
			return chaos_array_value;
		}
		case static_cast<CHAOS_IL2CPP_INTPTR>(112732858u):
		{
			const auto chaos_array_value = chaos_reflection_create_reference_array(&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ParameterInfo.hot, static_cast<CHAOS_IL2CPP_SIZE>(0));
			auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_array_value);
			return chaos_array_value;
		}
		case static_cast<CHAOS_IL2CPP_INTPTR>(102167601u):
		{
			const auto chaos_array_value = chaos_reflection_create_reference_array(&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ParameterInfo.hot, static_cast<CHAOS_IL2CPP_SIZE>(0));
			auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_array_value);
			return chaos_array_value;
		}
		case static_cast<CHAOS_IL2CPP_INTPTR>(107793242u):
		{
			const auto chaos_array_value = chaos_reflection_create_reference_array(&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ParameterInfo.hot, static_cast<CHAOS_IL2CPP_SIZE>(0));
			auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_array_value);
			return chaos_array_value;
		}
		case static_cast<CHAOS_IL2CPP_INTPTR>(111032951u):
		{
			const auto chaos_array_value = chaos_reflection_create_reference_array(&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ParameterInfo.hot, static_cast<CHAOS_IL2CPP_SIZE>(0));
			auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_array_value);
			return chaos_array_value;
		}
		case static_cast<CHAOS_IL2CPP_INTPTR>(107950600u):
		{
			const auto chaos_array_value = chaos_reflection_create_reference_array(&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ParameterInfo.hot, static_cast<CHAOS_IL2CPP_SIZE>(0));
			auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_array_value);
			return chaos_array_value;
		}
		case static_cast<CHAOS_IL2CPP_INTPTR>(103640021u):
		{
			const auto chaos_array_value = chaos_reflection_create_reference_array(&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ParameterInfo.hot, static_cast<CHAOS_IL2CPP_SIZE>(0));
			auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_array_value);
			return chaos_array_value;
		}
		case static_cast<CHAOS_IL2CPP_INTPTR>(105628414u):
		{
			const auto chaos_array_value = chaos_reflection_create_reference_array(&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ParameterInfo.hot, static_cast<CHAOS_IL2CPP_SIZE>(0));
			auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_array_value);
			return chaos_array_value;
		}
		case static_cast<CHAOS_IL2CPP_INTPTR>(116939051u):
		{
			const auto chaos_array_value = chaos_reflection_create_reference_array(&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ParameterInfo.hot, static_cast<CHAOS_IL2CPP_SIZE>(0));
			auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_array_value);
			return chaos_array_value;
		}
		case static_cast<CHAOS_IL2CPP_INTPTR>(104854460u):
		{
			const auto chaos_array_value = chaos_reflection_create_reference_array(&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ParameterInfo.hot, static_cast<CHAOS_IL2CPP_SIZE>(0));
			auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_array_value);
			return chaos_array_value;
		}
		case static_cast<CHAOS_IL2CPP_INTPTR>(114394009u):
		{
			const auto chaos_array_value = chaos_reflection_create_reference_array(&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ParameterInfo.hot, static_cast<CHAOS_IL2CPP_SIZE>(0));
			auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_array_value);
			return chaos_array_value;
		}
		case static_cast<CHAOS_IL2CPP_INTPTR>(103511842u):
		{
			const auto chaos_array_value = chaos_reflection_create_reference_array(&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ParameterInfo.hot, static_cast<CHAOS_IL2CPP_SIZE>(0));
			auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_array_value);
			return chaos_array_value;
		}
		default:
			return chaos_reflection_create_reference_array(
				&chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ParameterInfo.hot,
				static_cast<CHAOS_IL2CPP_SIZE>(0));
	}
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeFromHandle(CHAOS_IL2CPP_INTPTR chaos_type_handle)
{
	return chaos_reflection_create_type_value(chaos_type_handle);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeHandle(CHAOS_IL2CPP_INTPTR chaos_type_value) noexcept
{
	auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_type_value);
	return chaos_type->runtime_type_handle;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_object_type(CHAOS_IL2CPP_INTPTR chaos_object_value)
{
	if (chaos_object_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))
	{
		return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}
	
	const auto* chaos_ti = chaos_object_get_type_info(reinterpret_cast<const void*>(chaos_object_value));
	switch (chaos_ti->stable_id)
	{
		case chaos_type_id_ReflectionBindingSubjects_ReflectionBindingSubjects:
			return chaos_reflection_create_type_value(static_cast<CHAOS_IL2CPP_INTPTR>(46055704u));
		default:
			return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_instance(CHAOS_IL2CPP_INTPTR chaos_type_value)
{
	auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_type_value);
	if (chaos_type == nullptr)
	{
		return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}
	
	switch (chaos_type->runtime_type_handle)
	{
		default:
			return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}
}

CHAOS_IL2CPP_INTPTR chaos_reflection_resolve_method_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle, const char* chaos_method_name) noexcept
{
	if (chaos_method_name == nullptr)
	{
		return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}

	switch (chaos_type_handle)
	{
		case static_cast<CHAOS_IL2CPP_INTPTR>(46055704u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Run") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108502934u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_0") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116605137u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_10") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112732858u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_11") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102167601u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_1") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107793242u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_2") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111032951u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_3") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107950600u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_4") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103640021u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_5") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105628414u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_6") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116939051u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_7") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104854460u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_8") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114394009u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_9") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103511842u);
			}

			break;
		default:
			break;
	}

	return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethod(CHAOS_IL2CPP_INTPTR chaos_type_value, CHAOS_IL2CPP_INTPTR chaos_name_value, CHAOS_IL2CPP_INT32 chaos_binding_flags = CHAOS_IL2CPP_INT32(0))
{
	(void)chaos_binding_flags;
	auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_type_value);
	const auto* chaos_method_name = chaos_reflection_get_string_utf8(chaos_name_value);
	const auto chaos_method_handle = chaos_reflection_resolve_method_handle(chaos_type->runtime_type_handle, chaos_method_name);
	if (chaos_method_handle == static_cast<CHAOS_IL2CPP_INTPTR>(0))
	{
		return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}

	auto* chaos_method = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo);
	chaos_method->header.type_info = &chaos_type_info_v0_System_Private_CoreLib_System_Reflection_MethodInfo.hot;
	chaos_method->declaring_type_handle = chaos_type->runtime_type_handle;
	chaos_method->runtime_method_handle = chaos_method_handle;
	chaos_method->generic_definition_method_handle = chaos_method_handle;
	chaos_method->runtime_name_value = ChaosReflectionGetMethod_name_value_from_handle(chaos_method_handle);
	chaos_method->runtime_metadata_token_value = ChaosReflectionGetMethod_metadata_token_from_handle(chaos_method_handle);
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_method);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_closed_generic_method_handle(CHAOS_IL2CPP_INTPTR chaos_definition_handle, CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept
{
	CHAOS_IL2CPP_UINT32 hash = 2166136261u;
	const auto chaos_definition_bits = static_cast<CHAOS_IL2CPP_UINT64>(chaos_definition_handle);
	const auto chaos_type_bits = static_cast<CHAOS_IL2CPP_UINT64>(chaos_type_handle);
	hash ^= static_cast<CHAOS_IL2CPP_UINT32>(chaos_definition_bits & 0xFFFFFFFFu);
	hash *= 16777619u;
	hash ^= static_cast<CHAOS_IL2CPP_UINT32>(chaos_definition_bits >> 32);
	hash *= 16777619u;
	hash ^= static_cast<CHAOS_IL2CPP_UINT32>(chaos_type_bits & 0xFFFFFFFFu);
	hash *= 16777619u;
	hash ^= static_cast<CHAOS_IL2CPP_UINT32>(chaos_type_bits >> 32);
	hash *= 16777619u;
	auto row_index = hash & 0x00FFFFFFu;
	if (row_index == 0u)
	{
		row_index = 1u;
	}

	return static_cast<CHAOS_IL2CPP_INTPTR>(0x06000000u | row_index);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionMakeGenericMethod(CHAOS_IL2CPP_INTPTR chaos_method_value, CHAOS_IL2CPP_INTPTR chaos_type_array_value)
{
	if (chaos_method_value == static_cast<CHAOS_IL2CPP_INTPTR>(0) || chaos_type_array_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))
	{
		return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}

	auto* chaos_method = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo*>(chaos_method_value);
	auto* chaos_type_array = reinterpret_cast<chaos_managed_array*>(chaos_type_array_value);
	if (chaos_type_array->length <= static_cast<CHAOS_IL2CPP_INTPTR>(0))
	{
		return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}

	const auto chaos_type_argument_value = chaos_type_array->elements[0];
	if (chaos_type_argument_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))
	{
		return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}

	auto* chaos_type_argument = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_type_argument_value);
	auto* chaos_closed_method = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo);
	chaos_closed_method->header.type_info = &chaos_type_info_v0_System_Private_CoreLib_System_Reflection_MethodInfo.hot;
	chaos_closed_method->declaring_type_handle = chaos_method->declaring_type_handle;
	chaos_closed_method->generic_definition_method_handle = chaos_method->generic_definition_method_handle != static_cast<CHAOS_IL2CPP_INTPTR>(0)
		? chaos_method->generic_definition_method_handle
		: chaos_method->runtime_method_handle;
	chaos_closed_method->generic_argument_type_handle = chaos_type_argument->runtime_type_handle;
	chaos_closed_method->runtime_method_handle = chaos_reflection_create_closed_generic_method_handle(
		chaos_closed_method->generic_definition_method_handle,
		chaos_closed_method->generic_argument_type_handle);
	chaos_closed_method->runtime_name_value = chaos_method->runtime_name_value == static_cast<CHAOS_IL2CPP_INTPTR>(0)
		? ChaosReflectionGetMethod_name_value_from_handle(chaos_closed_method->generic_definition_method_handle)
		: chaos_method->runtime_name_value;
	chaos_closed_method->runtime_metadata_token_value = chaos_method->runtime_metadata_token_value != 0
		? chaos_method->runtime_metadata_token_value
		: ChaosReflectionGetMethod_metadata_token_from_handle(chaos_closed_method->generic_definition_method_handle);
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_closed_method);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethod_handle(CHAOS_IL2CPP_INTPTR chaos_method_value) noexcept
{
	auto* chaos_method = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo*>(chaos_method_value);
	return chaos_method->runtime_method_handle;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionInvokeMethod(CHAOS_IL2CPP_INTPTR chaos_method_value, CHAOS_IL2CPP_INTPTR chaos_target_value, CHAOS_IL2CPP_INTPTR chaos_args_array_value)
{
	if (chaos_method_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))
	{
		return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}

	auto* chaos_method = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo*>(chaos_method_value);
	auto* chaos_args = reinterpret_cast<chaos_managed_array*>(chaos_args_array_value);

	return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

CHAOS_IL2CPP_INT32 chaos_static_ReflectionBindingSubjects_ReflectionBindingSubjects___exitCode = 0;


	// AOT-baked string table: sorted by StringId for binary search at runtime.
	constexpr chaos::il2cpp::string_table::StringEntry chaos_aot_string_entries[] = {
		{ 2607821981565500683U, "hello", 5u },
	};

	constexpr CHAOS_IL2CPP_UINT32 chaos_aot_string_entry_count = sizeof(chaos_aot_string_entries) / sizeof(chaos_aot_string_entries[0]);

	// Register the AOT-baked string table with the runtime before any code uses it.
	static const CHAOS_IL2CPP_UINT32 s_aot_string_table_registered = []()
	{
		::chaos::il2cpp::string_table::InitializeFromAot(
			chaos_aot_string_entries,
			chaos_aot_string_entry_count);
		return 0u;
	}();

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodBase__Invoke_System_Object_System_Object_System_Reflection_BindingFlags_System_Reflection_Binder_System_Object___System_Globalization_CultureInfo_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2, CHAOS_IL2CPP_INTPTR chaos_arg_3, CHAOS_IL2CPP_INTPTR chaos_arg_4, CHAOS_IL2CPP_INTPTR chaos_arg_5)
{
	(void)chaos_arg_0; (void)chaos_arg_1; (void)chaos_arg_2; (void)chaos_arg_3; (void)chaos_arg_4; (void)chaos_arg_5;
	return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetEvent_System_Reflection_EventInfo_System_String_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
	(void)chaos_arg_0; (void)chaos_arg_1;
	return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetEvent_System_Reflection_EventInfo_System_String_System_Reflection_BindingFlags_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2)
{
	(void)chaos_arg_0; (void)chaos_arg_1; (void)chaos_arg_2;
	return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetField_System_Reflection_FieldInfo_System_String_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
	(void)chaos_arg_0; (void)chaos_arg_1;
	return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetField_System_Reflection_FieldInfo_System_String_System_Reflection_BindingFlags_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2)
{
	(void)chaos_arg_0; (void)chaos_arg_1; (void)chaos_arg_2;
	return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethod_System_Reflection_MethodInfo_System_String_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
	(void)chaos_arg_0; (void)chaos_arg_1;
	return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethod_System_Reflection_MethodInfo_System_String_System_Reflection_BindingFlags_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2)
{
	(void)chaos_arg_0; (void)chaos_arg_1; (void)chaos_arg_2;
	return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethod_System_Reflection_MethodInfo_System_String_System_Type___(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2)
{
	(void)chaos_arg_0; (void)chaos_arg_1; (void)chaos_arg_2;
	return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperty_System_Reflection_PropertyInfo_System_String_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
	(void)chaos_arg_0; (void)chaos_arg_1;
	return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperty_System_Reflection_PropertyInfo_System_String_System_Reflection_BindingFlags_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2)
{
	(void)chaos_arg_0; (void)chaos_arg_1; (void)chaos_arg_2;
	return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperty_System_Reflection_PropertyInfo_System_String_System_Type___(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2)
{
	(void)chaos_arg_0; (void)chaos_arg_1; (void)chaos_arg_2;
	return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetTypeFromHandle(chaos_fn_arg_0);
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };
static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };
static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };
static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_0(void);
extern "C" void ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_1(void);
extern "C" void ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_2(void);
extern "C" void ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_3(void);
extern "C" void ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_4(void);
extern "C" void ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_5(void);
extern "C" void ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_6(void);
extern "C" void ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_7(void);
extern "C" void ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_8(void);
extern "C" void ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_9(void);
extern "C" void ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_10(void);
extern "C" void ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_11(void);
extern "C" void ReflectionBindingSubjects_ReflectionBindingSubjects_Run(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.
// NOTE: reinterpret_cast is needed because MSVC rejects &anon_struct.header
// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[14] = {
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	1u
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[13];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		13u,
		1u,
		3861836417u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_0
		{ 0u, 0u },  // ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_1
		{ 0u, 0u },  // ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_2
		{ 0u, 0u },  // ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_3
		{ 0u, 0u },  // ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_4
		{ 0u, 0u },  // ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_5
		{ 0u, 0u },  // ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_6
		{ 0u, 0u },  // ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_7
		{ 0u, 0u },  // ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_8
		{ 0u, 0u },  // ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_9
		{ 0u, 0u },  // ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_10
		{ 0u, 0u },  // ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_11
		{ 0u, 1u },  // ReflectionBindingSubjects_ReflectionBindingSubjects_Run
	},
	{
		1u,
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest = reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);

// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[2] = {
		0u,
		1548u,
	};

	static constexpr const char* s_type_names[2] = {
		"<Module>",
		"ReflectionBindingSubjects",
	};

	static constexpr const char* s_type_namespaces[2] = {
		"",
		"",
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_type_parent_tokens[2] = {
		0u,
		0u,
	};

	static const TypeInfoHot* const s_type_info_ptrs[2] = {
		nullptr,
		&chaos_type_info_v0_ReflectionBindingSubjects_ReflectionBindingSubjects.hot,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[3] = {
		0u,
		0u,
		0u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset[3] = {
		0u,
		0u,
		0u,
	};



	static const ::chaos::il2cpp::runtime_core::ModuleDescriptor s_native_aot_module = {
		/* .name_utf8         = */ "ReflectionBindingSubjects",
		/* .image             = */ &kReflImage,
		/* .type_flags        = */ s_type_flags,
		/* .type_names        = */ s_type_names,
		/* .type_namespaces   = */ s_type_namespaces,
		/* .type_parent_tokens= */ s_type_parent_tokens,
		/* .type_info_ptrs    = */ s_type_info_ptrs,
		/* .nested_type_children= */ nullptr,
		/* .nested_type_offset = */ s_nested_type_offset,
		/* .generic_param_constraint_data= */ nullptr,
		/* .generic_param_constraint_offset= */ s_generic_param_constraint_offset,
		/* .type_count        = */ 2u,
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 — deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,

		/* .abi_manifest      = */ s_abi_manifest,
	};
	static const CHAOS_IL2CPP_UINT32 s_native_aot_module_id =
		::chaos::il2cpp::runtime_core::RegisterModule("ReflectionBindingSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[13] = {
	{ "Subject_0", 0x00000004u, 0u },  // ReflectionBindingSubjects
	{ "Subject_1", 0x00000005u, 0u },  // ReflectionBindingSubjects
	{ "Subject_2", 0x00000006u, 0u },  // ReflectionBindingSubjects
	{ "Subject_3", 0x00000007u, 0u },  // ReflectionBindingSubjects
	{ "Subject_4", 0x00000008u, 0u },  // ReflectionBindingSubjects
	{ "Subject_5", 0x00000009u, 0u },  // ReflectionBindingSubjects
	{ "Subject_6", 0x0000000Au, 0u },  // ReflectionBindingSubjects
	{ "Subject_7", 0x0000000Bu, 0u },  // ReflectionBindingSubjects
	{ "Subject_8", 0x0000000Cu, 0u },  // ReflectionBindingSubjects
	{ "Subject_9", 0x0000000Du, 0u },  // ReflectionBindingSubjects
	{ "Subject_10", 0x0000000Eu, 0u },  // ReflectionBindingSubjects
	{ "Subject_11", 0x0000000Fu, 0u },  // ReflectionBindingSubjects
	{ "Run", 0x00000003u, 1u },  // ReflectionBindingSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "ReflectionBindingSubjects", "", 0u, 13u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[13] = {
	{ 0x00000003u, 12u },
	{ 0x00000004u, 0u },
	{ 0x00000005u, 1u },
	{ 0x00000006u, 2u },
	{ 0x00000007u, 3u },
	{ 0x00000008u, 4u },
	{ 0x00000009u, 5u },
	{ 0x0000000Au, 6u },
	{ 0x0000000Bu, 7u },
	{ 0x0000000Cu, 8u },
	{ 0x0000000Du, 9u },
	{ 0x0000000Eu, 10u },
	{ 0x0000000Fu, 11u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[13] = {
	{ reinterpret_cast<void*>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionBindingSubjects::Subject_0
	{ reinterpret_cast<void*>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionBindingSubjects::Subject_1
	{ reinterpret_cast<void*>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionBindingSubjects::Subject_2
	{ reinterpret_cast<void*>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionBindingSubjects::Subject_3
	{ reinterpret_cast<void*>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionBindingSubjects::Subject_4
	{ reinterpret_cast<void*>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionBindingSubjects::Subject_5
	{ reinterpret_cast<void*>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionBindingSubjects::Subject_6
	{ reinterpret_cast<void*>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionBindingSubjects::Subject_7
	{ reinterpret_cast<void*>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionBindingSubjects::Subject_8
	{ reinterpret_cast<void*>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionBindingSubjects::Subject_9
	{ reinterpret_cast<void*>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionBindingSubjects::Subject_10
	{ reinterpret_cast<void*>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionBindingSubjects::Subject_11
	{ reinterpret_cast<void*>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Run), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionBindingSubjects::Run
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"ReflectionBindingSubjects",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	13u,
	s_hotpatch_slots,
	13u,
	s_hotpatch_entries,
	13u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;

// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[16] =
{
	"ReflectionBindingSubjects/ReflectionBindingSubjects::_exitCode",
	"System.Private.CoreLib/System.Byte",
	"System.Private.CoreLib/System.Int32",
	"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()",
	"System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)",
	"System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String)",
	"System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String,System.Reflection.BindingFlags)",
	"System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String)",
	"System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String,System.Reflection.BindingFlags)",
	"System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String)",
	"System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Reflection.BindingFlags)",
	"System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Type[])",
	"System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String)",
	"System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Reflection.BindingFlags)",
	"System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Type[])",
	"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)",
};

// Auto-generated stubs for kChaosExternalRuntimeFnTable entries
// called directly via fn table dispatch (no chaos_external_runtime_*
// call by name in the generated code).
static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__(void)
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);

}


extern "C" void* kChaosExternalRuntimeFnTable[16] =
{
    nullptr,
    nullptr,
    nullptr,
    reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__),
    reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodBase__Invoke_System_Object_System_Object_System_Reflection_BindingFlags_System_Reflection_Binder_System_Object___System_Globalization_CultureInfo_),
    reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetEvent_System_Reflection_EventInfo_System_String_),
    reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetEvent_System_Reflection_EventInfo_System_String_System_Reflection_BindingFlags_),
    reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetField_System_Reflection_FieldInfo_System_String_),
    reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetField_System_Reflection_FieldInfo_System_String_System_Reflection_BindingFlags_),
    reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethod_System_Reflection_MethodInfo_System_String_),
    reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethod_System_Reflection_MethodInfo_System_String_System_Reflection_BindingFlags_),
    reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethod_System_Reflection_MethodInfo_System_String_System_Type___),
    reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperty_System_Reflection_PropertyInfo_System_String_),
    reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperty_System_Reflection_PropertyInfo_System_String_System_Reflection_BindingFlags_),
    reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperty_System_Reflection_PropertyInfo_System_String_System_Type___),
    reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_),
};

extern "C" int32_t kChaosExternalRuntimeCount = 16;


// (no method AOT entries for this module)

// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[13])() = {
	reinterpret_cast<void(*)()>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_6),
	reinterpret_cast<void(*)()>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_7),
	reinterpret_cast<void(*)()>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_8),
	reinterpret_cast<void(*)()>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_9),
	reinterpret_cast<void(*)()>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_10),
	reinterpret_cast<void(*)()>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_11),
	reinterpret_cast<void(*)()>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Run),
};

static void (*kBenchmarkWrappers[13])() = {
	[]() { kAotMethods[0](); },
	[]() { kAotMethods[1](); },
	[]() { kAotMethods[2](); },
	[]() { kAotMethods[3](); },
	[]() { kAotMethods[4](); },
	[]() { kAotMethods[5](); },
	[]() { kAotMethods[6](); },
	[]() { kAotMethods[7](); },
	[]() { kAotMethods[8](); },
	[]() { kAotMethods[9](); },
	[]() { kAotMethods[10](); },
	[]() { kAotMethods[11](); },
	[]() { reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[12])(static_cast<CHAOS_IL2CPP_INTPTR>(0)); },
};

// Single-method dispatch via hotpatch dispatch table.
extern "C" CHAOS_IL2CPP_INT32 RunNativeAot(
	CHAOS_IL2CPP_INT32 chaos_entry_index)
{
	if (chaos_entry_index < 0 || chaos_entry_index >= kAotMethodCount)
		return -1;
	auto& entry = s_hotpatch_entries[chaos_entry_index];
	if (chaos::il2cpp::runtime_core::HotpatchIsActive(entry) && !chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(entry)) {
		uint64_t __chaos_args[4] = {}; uint64_t __chaos_ret[2] = {};
		chaos::il2cpp::runtime_core::InterpreterEntryDirect(
			entry.method_key, __chaos_args, __chaos_ret);
	} else {
		reinterpret_cast<void(*)()>(entry.direct_ptr)();
	}
	return 0;
}

// All-methods loop: run every method and return a bitmask of failures.
extern "C" CHAOS_IL2CPP_INT32 RunNativeAotAll()
{
	CHAOS_IL2CPP_INT32 result = 0;
	for (int i = 0; i < kAotMethodCount; i++) {
		auto& entry = s_hotpatch_entries[i];
	if (chaos::il2cpp::runtime_core::HotpatchIsActive(entry) && !chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(entry)) {
			uint64_t __chaos_args[4] = {}; uint64_t __chaos_ret[2] = {};
			chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				entry.method_key, __chaos_args, __chaos_ret);
		} else {
			reinterpret_cast<void(*)()>(entry.direct_ptr)();
		}
	}
	return result;
}

// Fast benchmark dispatch: no setjmp, inline slot access.
extern "C" CHAOS_IL2CPP_INT32 RunNativeAotBench(
	CHAOS_IL2CPP_INT32 chaos_entry_index)
{
	if (chaos_entry_index < 0 || chaos_entry_index >= kAotMethodCount)
		return -1;
	auto& entry = s_hotpatch_entries[chaos_entry_index];
	if (chaos::il2cpp::runtime_core::HotpatchIsActive(entry) && !chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(entry)) {
		chaos::il2cpp::runtime_core::InterpreterEntryDirectFast(
			entry.method_key);
	} else {
		reinterpret_cast<void(*)()>(entry.direct_ptr)();
	}
	return 0;
}

// Pure AOT benchmark: calls kAotMethods[i] directly, no hotpatch overhead.
extern "C" double BenchmarkMethod(
	int chaos_entry_index, int iterations) {
	if (chaos_entry_index < 0 || chaos_entry_index >= kAotMethodCount)
		return -1.0;
	auto start = std::chrono::steady_clock::now();
	for (int i = 0; i < iterations; i++) {
		kBenchmarkWrappers[chaos_entry_index]();
	}
	auto end = std::chrono::steady_clock::now();
	return std::chrono::duration<double, std::milli>(
		end - start).count();
}

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[13] = {
	reinterpret_cast<void*>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_0),
	reinterpret_cast<void*>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_1),
	reinterpret_cast<void*>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_2),
	reinterpret_cast<void*>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_3),
	reinterpret_cast<void*>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_4),
	reinterpret_cast<void*>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_5),
	reinterpret_cast<void*>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_6),
	reinterpret_cast<void*>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_7),
	reinterpret_cast<void*>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_8),
	reinterpret_cast<void*>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_9),
	reinterpret_cast<void*>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_10),
	reinterpret_cast<void*>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_11),
	reinterpret_cast<void*>(&ReflectionBindingSubjects_ReflectionBindingSubjects_Run),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 13u,
	.reverse_pinvoke_wrappers  = nullptr,
	.reverse_pinvoke_wrapper_count = 0u,
	.invoker_pointers          = nullptr,
	.invoker_pointer_count     = 0u,
	.unresolved_virtual_calls = nullptr,
	.unresolved_virtual_call_count = 0u,
	.type_capabilities       = nullptr,
	.type_capability_count   = 0u,
};

extern "C" const MetadataRegistrationV0 chaos_codegen_metadata_registration
	= {
	.struct_size              = sizeof(MetadataRegistrationV0),
	.generic_types            = kGenericTypeEntries,
	.generic_type_count       = sizeof(kGenericTypeEntries) / sizeof(kGenericTypeEntries[0]),
	.generic_type_args        = kGenericTypeArgTokens,
	.generic_type_arg_count   = sizeof(kGenericTypeArgTokens) / sizeof(kGenericTypeArgTokens[0]),
	.generic_methods          = kGenericMethodEntries,
	.generic_method_count     = sizeof(kGenericMethodEntries) / sizeof(kGenericMethodEntries[0]),
	.generic_method_args      = kGenericMethodArgTokens,
	.generic_method_arg_count = sizeof(kGenericMethodArgTokens) / sizeof(kGenericMethodArgTokens[0]),
	.method_aot_entries       = s_method_aot_entries,
	.method_aot_entry_count  = sizeof(s_method_aot_entries) / sizeof(s_method_aot_entries[0]),
	.method_aot_entry_args    = s_method_aot_entry_args,
	.method_aot_entry_arg_count = sizeof(s_method_aot_entry_args) / sizeof(s_method_aot_entry_args[0]),
	.field_offsets           = nullptr,
	.field_offset_count      = 0u,
	.metadata_usages         = nullptr,
	.metadata_usage_count    = 0u,
};

extern "C" const CodegenRegistrationOptionsV0 chaos_codegen_options
	= {
	.struct_size       = sizeof(CodegenRegistrationOptionsV0),
	.registration_flags = 0u,
	.image_name_utf8    = "ReflectionBindingSubjects",
};

// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryMethodDescriptor kReflMethods_ReflectionBindingSubjects_ReflectionBindingSubjects[13] = {
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionBindingSubjects/ReflectionBindingSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionBindingSubjects/ReflectionBindingSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionBindingSubjects/ReflectionBindingSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionBindingSubjects/ReflectionBindingSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionBindingSubjects/ReflectionBindingSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionBindingSubjects/ReflectionBindingSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionBindingSubjects/ReflectionBindingSubjects::Subject_6:System.Void()", "Subject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionBindingSubjects/ReflectionBindingSubjects::Subject_7:System.Void()", "Subject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionBindingSubjects/ReflectionBindingSubjects::Subject_8:System.Void()", "Subject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionBindingSubjects/ReflectionBindingSubjects::Subject_9:System.Void()", "Subject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionBindingSubjects/ReflectionBindingSubjects::Subject_10:System.Void()", "Subject_10", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionBindingSubjects/ReflectionBindingSubjects::Subject_11:System.Void()", "Subject_11", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionBindingSubjects/ReflectionBindingSubjects::Run:System.Void(System.Int32)", "Run", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0u, "ReflectionBindingSubjects/ReflectionBindingSubjects", "ReflectionBindingSubjects/ReflectionBindingSubjects", "", "ReflectionBindingSubjects", "ReflectionBindingSubjects", nullptr, nullptr, 0u, nullptr, 0u,
 kReflMethods_ReflectionBindingSubjects_ReflectionBindingSubjects, 13u },
};

static constexpr const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static constexpr ReflectionQueryImageDescriptor kReflImage = { "ReflectionBindingSubjects", kReflTypePtrs, 1u };

// Fake ImageHandle that ResolveSubjectId will decode back to kReflImage.
// BootstrapRuntime's aot_image_handle fallback discovers this via
// LookupModule(mid)->image at lines 311-321 of bootstrap.cpp.

// Populate generic registration arrays from this TU.
static void ChaosDoPopulateGenericRegistration(
	CHAOS_IL2CPP_UINT32* out_type_count,
	const GenericTypeRegistrationEntryV0** out_type_entries,
	const CHAOS_IL2CPP_UINT32** out_type_args,
	CHAOS_IL2CPP_UINT32* out_type_arg_count,
	CHAOS_IL2CPP_UINT32* out_method_count,
	const GenericMethodRegistrationEntryV0** out_method_entries,
	const CHAOS_IL2CPP_UINT32** out_method_args,
	CHAOS_IL2CPP_UINT32* out_method_arg_count)
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
	CHAOS_IL2CPP_UINT32*, const GenericTypeRegistrationEntryV0**, const CHAOS_IL2CPP_UINT32**, CHAOS_IL2CPP_UINT32*,
	CHAOS_IL2CPP_UINT32*, const GenericMethodRegistrationEntryV0**, const CHAOS_IL2CPP_UINT32**, CHAOS_IL2CPP_UINT32*);
namespace {
struct ChaosGenericRegistrationInit {
	ChaosGenericRegistrationInit() {
		g_chaos_populate_generic_registration =
			&ChaosDoPopulateGenericRegistration;
	}
} g_chaos_reg_init;
}

// Managed method: ReflectionBindingSubjects/ReflectionBindingSubjects::Subject_0()
extern "C" void ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{{
		_s1 = CHAOS_IL2CPP_STRING_ID("hello");
	}}
	{
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethod_System_Reflection_MethodInfo_System_String_(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[3])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("hello");
	}}
	{
		const auto chaos_raw_arg_1 = _s3;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethod_System_Reflection_MethodInfo_System_String_(chaos_arg_0, chaos_arg_1);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[3])();
		_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s3);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s3);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s2;
	_s2 = chaos_locals[0];
	{
		if (_s2 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionBindingSubjects_ReflectionBindingSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionBindingSubjects/ReflectionBindingSubjects::Subject_1()
extern "C" void ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{{
		_s1 = CHAOS_IL2CPP_STRING_ID("hello");
	}}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_raw_arg_2;
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethod_System_Reflection_MethodInfo_System_String_System_Reflection_BindingFlags_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[3])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("hello");
	}}
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_2 = _s4;
		const auto chaos_arg_2 = chaos_raw_arg_2;
		const auto chaos_raw_arg_1 = _s3;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethod_System_Reflection_MethodInfo_System_String_System_Reflection_BindingFlags_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[3])();
		_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s3);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s3);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s2;
	_s2 = chaos_locals[0];
	{
		if (_s2 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionBindingSubjects_ReflectionBindingSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionBindingSubjects/ReflectionBindingSubjects::Subject_2()
extern "C" void ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{{
		_s1 = CHAOS_IL2CPP_STRING_ID("hello");
	}}
	{
		const auto chaos_result = chaos_stub_definition_System_Private_CoreLib_System_Array__Empty___0_____type____method__System_Type();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_raw_arg_2;
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethod_System_Reflection_MethodInfo_System_String_System_Type___(chaos_arg_0, chaos_arg_1, chaos_arg_2);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[3])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("hello");
	}}
	{
		const auto chaos_result = chaos_stub_definition_System_Private_CoreLib_System_Array__Empty___0_____type____method__System_Type();
		_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_2 = _s4;
		const auto chaos_arg_2 = chaos_raw_arg_2;
		const auto chaos_raw_arg_1 = _s3;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethod_System_Reflection_MethodInfo_System_String_System_Type___(chaos_arg_0, chaos_arg_1, chaos_arg_2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[3])();
		_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s3);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s3);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s2;
	_s2 = chaos_locals[0];
	{
		if (_s2 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionBindingSubjects_ReflectionBindingSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionBindingSubjects/ReflectionBindingSubjects::Subject_3()
extern "C" void ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{{
		_s1 = CHAOS_IL2CPP_STRING_ID("hello");
	}}
	{
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetField_System_Reflection_FieldInfo_System_String_(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[3])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("hello");
	}}
	{
		const auto chaos_raw_arg_1 = _s3;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetField_System_Reflection_FieldInfo_System_String_(chaos_arg_0, chaos_arg_1);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[3])();
		_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s3);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s3);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s2;
	_s2 = chaos_locals[0];
	{
		if (_s2 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionBindingSubjects_ReflectionBindingSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionBindingSubjects/ReflectionBindingSubjects::Subject_4()
extern "C" void ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{{
		_s1 = CHAOS_IL2CPP_STRING_ID("hello");
	}}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_raw_arg_2;
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetField_System_Reflection_FieldInfo_System_String_System_Reflection_BindingFlags_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[3])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("hello");
	}}
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_2 = _s4;
		const auto chaos_arg_2 = chaos_raw_arg_2;
		const auto chaos_raw_arg_1 = _s3;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetField_System_Reflection_FieldInfo_System_String_System_Reflection_BindingFlags_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[3])();
		_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s3);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s3);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s2;
	_s2 = chaos_locals[0];
	{
		if (_s2 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionBindingSubjects_ReflectionBindingSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionBindingSubjects/ReflectionBindingSubjects::Subject_5()
extern "C" void ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{{
		_s1 = CHAOS_IL2CPP_STRING_ID("hello");
	}}
	{
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperty_System_Reflection_PropertyInfo_System_String_(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[3])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("hello");
	}}
	{
		const auto chaos_raw_arg_1 = _s3;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperty_System_Reflection_PropertyInfo_System_String_(chaos_arg_0, chaos_arg_1);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[3])();
		_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s3);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s3);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s2;
	_s2 = chaos_locals[0];
	{
		if (_s2 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionBindingSubjects_ReflectionBindingSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionBindingSubjects/ReflectionBindingSubjects::Subject_6()
extern "C" void ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_6(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{{
		_s1 = CHAOS_IL2CPP_STRING_ID("hello");
	}}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_raw_arg_2;
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperty_System_Reflection_PropertyInfo_System_String_System_Reflection_BindingFlags_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[3])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("hello");
	}}
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_2 = _s4;
		const auto chaos_arg_2 = chaos_raw_arg_2;
		const auto chaos_raw_arg_1 = _s3;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperty_System_Reflection_PropertyInfo_System_String_System_Reflection_BindingFlags_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[3])();
		_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s3);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s3);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s2;
	_s2 = chaos_locals[0];
	{
		if (_s2 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionBindingSubjects_ReflectionBindingSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionBindingSubjects/ReflectionBindingSubjects::Subject_7()
extern "C" void ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_7(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{{
		_s1 = CHAOS_IL2CPP_STRING_ID("hello");
	}}
	{
		const auto chaos_result = chaos_stub_definition_System_Private_CoreLib_System_Array__Empty___0_____type____method__System_Type();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_raw_arg_2;
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperty_System_Reflection_PropertyInfo_System_String_System_Type___(chaos_arg_0, chaos_arg_1, chaos_arg_2);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[3])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("hello");
	}}
	{
		const auto chaos_result = chaos_stub_definition_System_Private_CoreLib_System_Array__Empty___0_____type____method__System_Type();
		_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_2 = _s4;
		const auto chaos_arg_2 = chaos_raw_arg_2;
		const auto chaos_raw_arg_1 = _s3;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperty_System_Reflection_PropertyInfo_System_String_System_Type___(chaos_arg_0, chaos_arg_1, chaos_arg_2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[3])();
		_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s3);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s3);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s2;
	_s2 = chaos_locals[0];
	{
		if (_s2 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionBindingSubjects_ReflectionBindingSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionBindingSubjects/ReflectionBindingSubjects::Subject_8()
extern "C" void ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{{
		_s1 = CHAOS_IL2CPP_STRING_ID("hello");
	}}
	{
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetEvent_System_Reflection_EventInfo_System_String_(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[3])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("hello");
	}}
	{
		const auto chaos_raw_arg_1 = _s3;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetEvent_System_Reflection_EventInfo_System_String_(chaos_arg_0, chaos_arg_1);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[3])();
		_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s3);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s3);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s2;
	_s2 = chaos_locals[0];
	{
		if (_s2 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionBindingSubjects_ReflectionBindingSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionBindingSubjects/ReflectionBindingSubjects::Subject_9()
extern "C" void ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_9(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{{
		_s1 = CHAOS_IL2CPP_STRING_ID("hello");
	}}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_raw_arg_2;
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetEvent_System_Reflection_EventInfo_System_String_System_Reflection_BindingFlags_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[3])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{{
		_s3 = CHAOS_IL2CPP_STRING_ID("hello");
	}}
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_2 = _s4;
		const auto chaos_arg_2 = chaos_raw_arg_2;
		const auto chaos_raw_arg_1 = _s3;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetEvent_System_Reflection_EventInfo_System_String_System_Reflection_BindingFlags_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[3])();
		_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s3);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s3);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s2;
	_s2 = chaos_locals[0];
	{
		if (_s2 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionBindingSubjects_ReflectionBindingSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionBindingSubjects/ReflectionBindingSubjects::Subject_10()
extern "C" void ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_10(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReflectionBindingSubjects/ReflectionBindingSubjects::Subject_11()
extern "C" void ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_11(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};
	CHAOS_IL2CPP_INTPTR _s17{};
	CHAOS_IL2CPP_INTPTR _s18{};
	CHAOS_IL2CPP_INTPTR _s19{};
	CHAOS_IL2CPP_INTPTR _s20{};
	CHAOS_IL2CPP_INTPTR _s21{};
	CHAOS_IL2CPP_INTPTR _s22{};
	CHAOS_IL2CPP_INTPTR _s23{};
	CHAOS_IL2CPP_INTPTR _s24{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_boxed_type_System_Private_CoreLib_System_Int32 chaos_box_storage_0{};
	{
		const auto chaos_value = _s1;
		chaos_box_storage_0.header.type_info = &chaos_type_info_v0_System_Private_CoreLib_System_Int32.hot;
		chaos_box_storage_0.value = chaos_value;
	}
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_box_storage_0);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_result = chaos_stub_definition_System_Private_CoreLib_System_Array__Empty___0_____type____method__System_Object();
		_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_5 = _s5;
		const auto chaos_arg_5 = chaos_raw_arg_5;
		const auto chaos_raw_arg_4 = _s4;
		const auto chaos_arg_4 = chaos_raw_arg_4;
		const auto chaos_raw_arg_3 = _s3;
		const auto chaos_arg_3 = chaos_raw_arg_3;
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_raw_arg_2;
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodBase__Invoke_System_Object_System_Object_System_Reflection_BindingFlags_System_Reflection_Binder_System_Object___System_Globalization_CultureInfo_(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4, chaos_arg_5);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[3])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_boxed_type_System_Private_CoreLib_System_Int32 chaos_box_storage_1{};
	{
		const auto chaos_value = _s3;
		chaos_box_storage_1.header.type_info = &chaos_type_info_v0_System_Private_CoreLib_System_Int32.hot;
		chaos_box_storage_1.value = chaos_value;
	}
	_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_box_storage_1);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_result = chaos_stub_definition_System_Private_CoreLib_System_Array__Empty___0_____type____method__System_Object();
		_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_5 = _s7;
		const auto chaos_arg_5 = chaos_raw_arg_5;
		const auto chaos_raw_arg_4 = _s6;
		const auto chaos_arg_4 = chaos_raw_arg_4;
		const auto chaos_raw_arg_3 = _s5;
		const auto chaos_arg_3 = chaos_raw_arg_3;
		const auto chaos_raw_arg_2 = _s4;
		const auto chaos_arg_2 = chaos_raw_arg_2;
		const auto chaos_raw_arg_1 = _s3;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodBase__Invoke_System_Object_System_Object_System_Reflection_BindingFlags_System_Reflection_Binder_System_Object___System_Globalization_CultureInfo_(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4, chaos_arg_5);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[3])();
		_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s3);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s3);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s2);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s2;
	_s2 = chaos_locals[0];
	{
		if (_s2 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionBindingSubjects_ReflectionBindingSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionBindingSubjects/ReflectionBindingSubjects::Run(System.Int32)
extern "C" void ReflectionBindingSubjects_ReflectionBindingSubjects_Run(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_switch_value = static_cast<CHAOS_IL2CPP_INT32>(_s0);
		switch (chaos_switch_value)
		{
			case 0:
			{
				{
					auto& _d0 = s_hotpatch_entries[0];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d0)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d0))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d0.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_0();
					}
				}
				break;
			}
			case 1:
			{
				{
					auto& _d1 = s_hotpatch_entries[1];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d1)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d1))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d1.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_1();
					}
				}
				break;
			}
			case 2:
			{
				{
					auto& _d2 = s_hotpatch_entries[2];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d2)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d2))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d2.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_2();
					}
				}
				break;
			}
			case 3:
			{
				{
					auto& _d3 = s_hotpatch_entries[3];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d3)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d3))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d3.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_3();
					}
				}
				break;
			}
			case 4:
			{
				{
					auto& _d4 = s_hotpatch_entries[4];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d4)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d4))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d4.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_4();
					}
				}
				break;
			}
			case 5:
			{
				{
					auto& _d5 = s_hotpatch_entries[5];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d5)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d5))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d5.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_5();
					}
				}
				break;
			}
			case 6:
			{
				{
					auto& _d6 = s_hotpatch_entries[6];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d6)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d6))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d6.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_6();
					}
				}
				break;
			}
			case 7:
			{
				{
					auto& _d7 = s_hotpatch_entries[7];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d7)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d7))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d7.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_7();
					}
				}
				break;
			}
			case 8:
			{
				{
					auto& _d8 = s_hotpatch_entries[8];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d8)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d8))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d8.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_8();
					}
				}
				break;
			}
			case 9:
			{
				{
					auto& _d9 = s_hotpatch_entries[9];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d9)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d9))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d9.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_9();
					}
				}
				break;
			}
			case 10:
			{
				{
					auto& _d10 = s_hotpatch_entries[10];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d10)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d10))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d10.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_10();
					}
				}
				break;
			}
			case 11:
			{
				{
					auto& _d11 = s_hotpatch_entries[11];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d11)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d11))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d11.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionBindingSubjects_ReflectionBindingSubjects_Subject_11();
					}
				}
				break;
			}
			default:
			{
				return;
				break;
			}
		}
	}
	return;
}



}  // namespace chaos::il2cpp::codegen::ReflectionBindingSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 13;