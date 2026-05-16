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

// Forward declaration for dispatch table entries (defined in runtime_stubs.cpp)
extern "C" void InterpreterEntryDirect(
    CHAOS_IL2CPP_UINTPTR method_key,
    void*     args_buf,
    void*     ret_buf) noexcept;

#pragma warning(push)
#pragma warning(disable: 4065 4244)

namespace chaos::il2cpp::codegen::ReflectionTypeSubjects {

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

inline TypeInfoV0 chaos_type_info_v0_ReflectionTypeSubjects_ReflectionTypeSubjects = {{nullptr, nullptr, 14582329566846711146ULL, 0u, 32 /* warm_delta */, 1 /* reference */, 1}, {nullptr, nullptr, 0, 0, 0, 0 }};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ReflectionTypeSubjects_ReflectionTypeSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(14582329566846711146ULL);inline TypeInfoV0 chaos_type_info_v0_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32 /* warm_delta */, 1 /* reference */, 1}, {nullptr, nullptr, 0, 0, 0, 0 }};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);inline TypeInfoV0 chaos_type_info_v0_System_Private_CoreLib_System_Reflection_Assembly = {{nullptr, nullptr, 5474029880995115448ULL, 0u, 32 /* warm_delta */, 1 /* reference */, 1}, {nullptr, nullptr, 0, 0, 0, 0 }};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly = static_cast<CHAOS_IL2CPP_INTPTR>(5474029880995115448ULL);inline TypeInfoV0 chaos_type_info_v0_System_Private_CoreLib_System_Reflection_AssemblyName = {{nullptr, nullptr, 17082367815459723707ULL, 0u, 32 /* warm_delta */, 1 /* reference */, 1}, {nullptr, nullptr, 0, 0, 0, 0 }};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = static_cast<CHAOS_IL2CPP_INTPTR>(17082367815459723707ULL);inline TypeInfoV0 chaos_type_info_v0_System_Private_CoreLib_System_Reflection_ConstructorInfo = {{nullptr, nullptr, 4137207361503509124ULL, 0u, 32 /* warm_delta */, 1 /* reference */, 1}, {nullptr, nullptr, 0, 0, 0, 0 }};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo = static_cast<CHAOS_IL2CPP_INTPTR>(4137207361503509124ULL);inline TypeInfoV0 chaos_type_info_v0_System_Private_CoreLib_System_Reflection_FieldInfo = {{nullptr, nullptr, 17040031516751226236ULL, 0u, 32 /* warm_delta */, 1 /* reference */, 1}, {nullptr, nullptr, 0, 0, 0, 0 }};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo = static_cast<CHAOS_IL2CPP_INTPTR>(17040031516751226236ULL);inline TypeInfoV0 chaos_type_info_v0_System_Private_CoreLib_System_Reflection_MethodInfo = {{&chaos_type_info_v0_System_Private_CoreLib_System_Object.hot, nullptr, 10748947813473285525ULL, 0u, 32 /* warm_delta */, 1 /* reference */, 1}, {nullptr, nullptr, 0, 0, 0, 0 }};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = static_cast<CHAOS_IL2CPP_INTPTR>(10748947813473285525ULL);inline TypeInfoV0 chaos_type_info_v0_System_Private_CoreLib_System_String = {{nullptr, nullptr, 1782325859292956794ULL, 0u, 32 /* warm_delta */, 1 /* reference */, 1}, {nullptr, nullptr, 0, 0, 0, 0 }};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_INTPTR>(1782325859292956794ULL);inline TypeInfoV0 chaos_type_info_v0_System_Private_CoreLib_System_Type = {{&chaos_type_info_v0_System_Private_CoreLib_System_Object.hot, nullptr, 3222174513575444759ULL, 0u, 32 /* warm_delta */, 1 /* reference */, 1}, {nullptr, nullptr, 0, 0, 0, 0 }};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Type = static_cast<CHAOS_IL2CPP_INTPTR>(3222174513575444759ULL);
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

	auto* chaos_header = reinterpret_cast<FatHeader*>(chaos_value);
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

struct chaos_type_ReflectionTypeSubjects_ReflectionTypeSubjects
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Object
{
	ThinLockableHeader header{};
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

struct chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo
{
	ThinLockableHeader header{};
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
		default:
			return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}
}

CHAOS_IL2CPP_INT32 chaos_reflection_get_type_metadata_token_from_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept
{
	switch (chaos_type_handle)
	{
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
		default:
			return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}
}

CHAOS_IL2CPP_INT32 ChaosReflectionGetMethod_metadata_token_from_handle(CHAOS_IL2CPP_INTPTR chaos_method_handle) noexcept
{
	switch (chaos_method_handle)
	{
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

CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssembly(CHAOS_IL2CPP_INTPTR chaos_type_value)
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

CHAOS_IL2CPP_INTPTR chaos_reflection_resolve_type_handle(CHAOS_IL2CPP_INTPTR chaos_assembly_name_value, const char* chaos_type_name) noexcept
{
	const char* chaos_assembly_name = chaos_reflection_get_string_utf8(chaos_assembly_name_value);
	if (chaos_assembly_name == nullptr || chaos_type_name == nullptr)
	{
		return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}

	return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_from_assembly(CHAOS_IL2CPP_INTPTR chaos_assembly_value, CHAOS_IL2CPP_INTPTR chaos_name_value)
{
	auto* chaos_assembly = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_Assembly*>(chaos_assembly_value);
	if (chaos_assembly == nullptr)
	{
		return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}

	const auto chaos_type_handle = chaos_reflection_resolve_type_handle(
		chaos_assembly->runtime_assembly_name_value,
		chaos_reflection_get_string_utf8(chaos_name_value));
	if (chaos_type_handle == static_cast<CHAOS_IL2CPP_INTPTR>(0))
	{
		return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}

	return chaos_reflection_create_type_value(chaos_type_handle);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeByName(CHAOS_IL2CPP_INTPTR chaos_name_value)
{
	const char* chaos_name = chaos_reflection_get_string_utf8(chaos_name_value);
	if (chaos_name == nullptr)
	{
		return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}

	return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssembly_name(CHAOS_IL2CPP_INTPTR chaos_assembly_value)
{
	auto* chaos_assembly = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_Assembly*>(chaos_assembly_value);
	if (chaos_assembly == nullptr)
	{
		return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}

	auto* chaos_assembly_name = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName);
	chaos_assembly_name->header.type_info = &chaos_type_info_v0_System_Private_CoreLib_System_Reflection_AssemblyName.hot;
	chaos_assembly_name->runtime_name_value = chaos_assembly->runtime_assembly_name_value;
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_assembly_name);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssembly_name_value(CHAOS_IL2CPP_INTPTR chaos_assembly_name_value) noexcept
{
	auto* chaos_assembly_name = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName*>(chaos_assembly_name_value);
	return chaos_assembly_name == nullptr
		? static_cast<CHAOS_IL2CPP_INTPTR>(0)
		: chaos_assembly_name->runtime_name_value;
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
		case static_cast<CHAOS_IL2CPP_INTPTR>(37835082u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Run") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114257612u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_0") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107135939u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_10") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104201728u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_11") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105193871u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_12") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102603890u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_13") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104527593u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_14") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109112020u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_15") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107804483u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_16") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108293590u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_17") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109448877u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_18") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107608920u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_19") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106606087u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_1") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108443476u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_20") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110848795u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_21") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102586476u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_22") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101902853u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_23") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116742766u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_24") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104097383u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_25") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113190456u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_26") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109301569u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_27") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106860810u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_28") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107561251u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_29") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116084148u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_2") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108780333u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_30") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103983774u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_31") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107068021u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_32") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104902620u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_33") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112399819u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_34") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107157882u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_35") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106541041u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_36") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105929896u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_37") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115064599u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_38") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116682534u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_39") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102766525u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_3") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107625046u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_40") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109327009u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_41") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111625706u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_42") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113807047u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_4") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104525327u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_5") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103533184u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_6") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103859049u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_7") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101935346u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_8") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101624251u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_9") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108442252u);
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

CHAOS_IL2CPP_INT32 chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = 0;


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

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_DeclaringType_System_Type__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetDeclaringType(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_Name_System_String__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetMemberName(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_ReflectedType_System_Type__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetReflectedType(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_RuntimeTypeHandle__GetHashCode_System_Int32__(CHAOS_IL2CPP_INT64 chaos_fn_arg_0)
{
	return ChaosRuntimetypehandleGetHashCode(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
	{
		CHAOS_IL2CPP_FAIL();
	}

	if (chaos_is_string_id(chaos_arg_0))
	{
		return static_cast<CHAOS_IL2CPP_INT32>(
			chaos::il2cpp::string_table::Resolve(
				chaos_extract_string_id(chaos_arg_0)).byte_count);
	}

	auto* chaos_string = reinterpret_cast<CHAOS_IL2CPP_STRING_TYPE*>(chaos_arg_0);
	return static_cast<CHAOS_IL2CPP_INT32>(chaos_string->length);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetConstructors_System_Reflection_ConstructorInfo___System_Reflection_BindingFlags_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	return ChaosReflectionGetConstructors(chaos_fn_arg_0, chaos_fn_arg_1);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetConstructors_System_Reflection_ConstructorInfo____(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetConstructorsDefault(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetElementType_System_Type__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosTypeGetElementType(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetEvents_System_Reflection_EventInfo____(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosTypeGetEvents(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetFields_System_Reflection_FieldInfo___System_Reflection_BindingFlags_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	return ChaosReflectionGetFieldsBindingflags(chaos_fn_arg_0, chaos_fn_arg_1);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetFields_System_Reflection_FieldInfo____(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetFields(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetInterfaces_System_Type____(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosTypeGetInterfaces(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetMembers_System_Reflection_MemberInfo____(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	(void)chaos_arg_0;
	return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethods_System_Reflection_MethodInfo___System_Reflection_BindingFlags_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	return ChaosReflectionGetMethodsBindingflags(chaos_fn_arg_0, chaos_fn_arg_1);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethods_System_Reflection_MethodInfo____(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetMethods(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetNestedType_System_Type_System_String_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
	(void)chaos_arg_0; (void)chaos_arg_1;
	return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetNestedTypes_System_Type____(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosTypeGetNestedTypes(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperties_System_Reflection_PropertyInfo___System_Reflection_BindingFlags_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
	(void)chaos_arg_0; (void)chaos_arg_1;
	return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperties_System_Reflection_PropertyInfo____(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	(void)chaos_arg_0;
	return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetTypeFromHandle(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_AssemblyQualifiedName_System_String__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetAssemblyQualifiedName(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_Assembly_System_Reflection_Assembly__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetAssembly(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_BaseType_System_Type__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetBaseType(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_FullName_System_String__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetTypeFullName(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsAbstract_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetIsAbstract(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsArray_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetIsArray(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsByRef_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosTypeGetIsByRef(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsConstructedGenericType_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetIsConstructedGeneric(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsEnum_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetIsEnum(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsGenericType_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetIsGenericType(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsInterface_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetIsInterface(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNestedPrivate_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosTypeGetIsNestedPrivate(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNestedPublic_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosTypeGetIsNestedPublic(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNested_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosTypeGetIsNested(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNotPublic_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosTypeGetIsNotPublic(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsPointer_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosTypeGetIsPointer(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsPublic_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosTypeGetIsPublic(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsSealed_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetIsSealed(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsValueType_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetIsValueType(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_Namespace_System_String__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetNamespace(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_TypeHandle_System_RuntimeTypeHandle__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetTypeHandle(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_UnderlyingSystemType_System_Type__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosTypeGetUnderlyingSystemType(chaos_fn_arg_0);
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };
static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };
static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };
static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_0(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_1(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_2(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_3(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_4(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_5(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_6(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_7(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_8(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_9(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_10(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_11(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_12(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_13(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_14(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_15(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_16(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_17(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_18(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_19(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_20(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_21(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_22(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_23(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_24(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_25(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_26(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_27(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_28(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_29(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_30(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_31(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_32(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_33(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_34(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_35(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_36(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_37(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_38(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_39(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_40(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_41(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_42(void);
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Run(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);


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
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[45] = {
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
	0u,
	0u,
	0u,
	0u,
	0u,
	1u
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[44];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		44u,
		1u,
		800415209u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_0
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_1
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_2
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_3
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_4
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_5
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_6
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_7
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_8
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_9
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_10
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_11
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_12
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_13
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_14
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_15
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_16
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_17
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_18
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_19
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_20
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_21
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_22
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_23
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_24
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_25
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_26
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_27
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_28
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_29
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_30
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_31
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_32
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_33
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_34
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_35
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_36
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_37
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_38
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_39
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_40
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_41
		{ 0u, 0u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_42
		{ 0u, 1u },  // ReflectionTypeSubjects_ReflectionTypeSubjects_Run
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
		"ReflectionTypeSubjects",
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
		&chaos_type_info_v0_ReflectionTypeSubjects_ReflectionTypeSubjects.hot,
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
		/* .name_utf8         = */ "ReflectionTypeSubjects",
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
		::chaos::il2cpp::runtime_core::RegisterModule("ReflectionTypeSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[44] = {
	{ "Subject_0", 0x00000004u, 0u },  // ReflectionTypeSubjects
	{ "Subject_1", 0x00000005u, 0u },  // ReflectionTypeSubjects
	{ "Subject_2", 0x00000006u, 0u },  // ReflectionTypeSubjects
	{ "Subject_3", 0x00000007u, 0u },  // ReflectionTypeSubjects
	{ "Subject_4", 0x00000008u, 0u },  // ReflectionTypeSubjects
	{ "Subject_5", 0x00000009u, 0u },  // ReflectionTypeSubjects
	{ "Subject_6", 0x0000000Au, 0u },  // ReflectionTypeSubjects
	{ "Subject_7", 0x0000000Bu, 0u },  // ReflectionTypeSubjects
	{ "Subject_8", 0x0000000Cu, 0u },  // ReflectionTypeSubjects
	{ "Subject_9", 0x0000000Du, 0u },  // ReflectionTypeSubjects
	{ "Subject_10", 0x0000000Eu, 0u },  // ReflectionTypeSubjects
	{ "Subject_11", 0x0000000Fu, 0u },  // ReflectionTypeSubjects
	{ "Subject_12", 0x00000010u, 0u },  // ReflectionTypeSubjects
	{ "Subject_13", 0x00000011u, 0u },  // ReflectionTypeSubjects
	{ "Subject_14", 0x00000012u, 0u },  // ReflectionTypeSubjects
	{ "Subject_15", 0x00000013u, 0u },  // ReflectionTypeSubjects
	{ "Subject_16", 0x00000014u, 0u },  // ReflectionTypeSubjects
	{ "Subject_17", 0x00000015u, 0u },  // ReflectionTypeSubjects
	{ "Subject_18", 0x00000016u, 0u },  // ReflectionTypeSubjects
	{ "Subject_19", 0x00000017u, 0u },  // ReflectionTypeSubjects
	{ "Subject_20", 0x00000018u, 0u },  // ReflectionTypeSubjects
	{ "Subject_21", 0x00000019u, 0u },  // ReflectionTypeSubjects
	{ "Subject_22", 0x0000001Au, 0u },  // ReflectionTypeSubjects
	{ "Subject_23", 0x0000001Bu, 0u },  // ReflectionTypeSubjects
	{ "Subject_24", 0x0000001Cu, 0u },  // ReflectionTypeSubjects
	{ "Subject_25", 0x0000001Du, 0u },  // ReflectionTypeSubjects
	{ "Subject_26", 0x0000001Eu, 0u },  // ReflectionTypeSubjects
	{ "Subject_27", 0x0000001Fu, 0u },  // ReflectionTypeSubjects
	{ "Subject_28", 0x00000020u, 0u },  // ReflectionTypeSubjects
	{ "Subject_29", 0x00000021u, 0u },  // ReflectionTypeSubjects
	{ "Subject_30", 0x00000022u, 0u },  // ReflectionTypeSubjects
	{ "Subject_31", 0x00000023u, 0u },  // ReflectionTypeSubjects
	{ "Subject_32", 0x00000024u, 0u },  // ReflectionTypeSubjects
	{ "Subject_33", 0x00000025u, 0u },  // ReflectionTypeSubjects
	{ "Subject_34", 0x00000026u, 0u },  // ReflectionTypeSubjects
	{ "Subject_35", 0x00000027u, 0u },  // ReflectionTypeSubjects
	{ "Subject_36", 0x00000028u, 0u },  // ReflectionTypeSubjects
	{ "Subject_37", 0x00000029u, 0u },  // ReflectionTypeSubjects
	{ "Subject_38", 0x0000002Au, 0u },  // ReflectionTypeSubjects
	{ "Subject_39", 0x0000002Bu, 0u },  // ReflectionTypeSubjects
	{ "Subject_40", 0x0000002Cu, 0u },  // ReflectionTypeSubjects
	{ "Subject_41", 0x0000002Du, 0u },  // ReflectionTypeSubjects
	{ "Subject_42", 0x0000002Eu, 0u },  // ReflectionTypeSubjects
	{ "Run", 0x00000003u, 1u },  // ReflectionTypeSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "ReflectionTypeSubjects", "", 0u, 44u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[44] = {
	{ 0x00000003u, 43u },
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
	{ 0x00000010u, 12u },
	{ 0x00000011u, 13u },
	{ 0x00000012u, 14u },
	{ 0x00000013u, 15u },
	{ 0x00000014u, 16u },
	{ 0x00000015u, 17u },
	{ 0x00000016u, 18u },
	{ 0x00000017u, 19u },
	{ 0x00000018u, 20u },
	{ 0x00000019u, 21u },
	{ 0x0000001Au, 22u },
	{ 0x0000001Bu, 23u },
	{ 0x0000001Cu, 24u },
	{ 0x0000001Du, 25u },
	{ 0x0000001Eu, 26u },
	{ 0x0000001Fu, 27u },
	{ 0x00000020u, 28u },
	{ 0x00000021u, 29u },
	{ 0x00000022u, 30u },
	{ 0x00000023u, 31u },
	{ 0x00000024u, 32u },
	{ 0x00000025u, 33u },
	{ 0x00000026u, 34u },
	{ 0x00000027u, 35u },
	{ 0x00000028u, 36u },
	{ 0x00000029u, 37u },
	{ 0x0000002Au, 38u },
	{ 0x0000002Bu, 39u },
	{ 0x0000002Cu, 40u },
	{ 0x0000002Du, 41u },
	{ 0x0000002Eu, 42u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[44] = {
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypeSubjects::Subject_0
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypeSubjects::Subject_1
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypeSubjects::Subject_2
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_3
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_4
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_5
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_6
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_7
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_8
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_9
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_10
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_11
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_12
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_13
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_14), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_14
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_15), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_15
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_16), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_16
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_17), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_17
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_18), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_18
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_19), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_19
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_20), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_20
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_21), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_21
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_22), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_22
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_23), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_23
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_24), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_24
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_25), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_25
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_26), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_26
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_27), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_27
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_28), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_28
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_29), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_29
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_30), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_30
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_31), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_31
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_32
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_33), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_33
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_34), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_34
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_35), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_35
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_36), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_36
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_37), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_37
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_38), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_38
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_39), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_39
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_40), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_40
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_41), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_41
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_42), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ReflectionTypeSubjects::Subject_42
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Run), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypeSubjects::Run
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"ReflectionTypeSubjects",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	44u,
	s_hotpatch_slots,
	44u,
	s_hotpatch_entries,
	44u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;

// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[46] =
{
	"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode",
	"System.Private.CoreLib/System.Byte",
	"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()",
	"System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()",
	"System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()",
	"System.Private.CoreLib/System.Reflection.MemberInfo::get_ReflectedType:System.Type()",
	"System.Private.CoreLib/System.RuntimeTypeHandle",
	"System.Private.CoreLib/System.RuntimeTypeHandle::GetHashCode:System.Int32()",
	"System.Private.CoreLib/System.String::get_Length:System.Int32()",
	"System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[]()",
	"System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[](System.Reflection.BindingFlags)",
	"System.Private.CoreLib/System.Type::GetElementType:System.Type()",
	"System.Private.CoreLib/System.Type::GetEvents:System.Reflection.EventInfo[]()",
	"System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()",
	"System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[](System.Reflection.BindingFlags)",
	"System.Private.CoreLib/System.Type::GetInterfaces:System.Type[]()",
	"System.Private.CoreLib/System.Type::GetMembers:System.Reflection.MemberInfo[]()",
	"System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()",
	"System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[](System.Reflection.BindingFlags)",
	"System.Private.CoreLib/System.Type::GetNestedType:System.Type(System.String)",
	"System.Private.CoreLib/System.Type::GetNestedTypes:System.Type[]()",
	"System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[]()",
	"System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[](System.Reflection.BindingFlags)",
	"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)",
	"System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()",
	"System.Private.CoreLib/System.Type::get_AssemblyQualifiedName:System.String()",
	"System.Private.CoreLib/System.Type::get_BaseType:System.Type()",
	"System.Private.CoreLib/System.Type::get_FullName:System.String()",
	"System.Private.CoreLib/System.Type::get_IsAbstract:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsByRef:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsInterface:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsNested:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsNestedPrivate:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsNestedPublic:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsNotPublic:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsPointer:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsPublic:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsSealed:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_Namespace:System.String()",
	"System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()",
	"System.Private.CoreLib/System.Type::get_UnderlyingSystemType:System.Type()",
};

extern "C" void* kChaosExternalRuntimeFnTable[46] =
{
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_DeclaringType_System_Type__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_Name_System_String__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_ReflectedType_System_Type__),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_RuntimeTypeHandle__GetHashCode_System_Int32__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetConstructors_System_Reflection_ConstructorInfo____),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetConstructors_System_Reflection_ConstructorInfo___System_Reflection_BindingFlags_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetElementType_System_Type__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetEvents_System_Reflection_EventInfo____),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetFields_System_Reflection_FieldInfo____),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetFields_System_Reflection_FieldInfo___System_Reflection_BindingFlags_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetInterfaces_System_Type____),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetMembers_System_Reflection_MemberInfo____),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethods_System_Reflection_MethodInfo____),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethods_System_Reflection_MethodInfo___System_Reflection_BindingFlags_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetNestedType_System_Type_System_String_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetNestedTypes_System_Type____),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperties_System_Reflection_PropertyInfo____),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperties_System_Reflection_PropertyInfo___System_Reflection_BindingFlags_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_Assembly_System_Reflection_Assembly__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_AssemblyQualifiedName_System_String__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_BaseType_System_Type__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_FullName_System_String__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsAbstract_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsArray_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsByRef_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsConstructedGenericType_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsEnum_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsGenericType_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsInterface_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNested_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNestedPrivate_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNestedPublic_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNotPublic_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsPointer_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsPublic_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsSealed_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsValueType_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_Namespace_System_String__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_TypeHandle_System_RuntimeTypeHandle__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_UnderlyingSystemType_System_Type__),
};

extern "C" int32_t kChaosExternalRuntimeCount = 46;


// (no method AOT entries for this module)

// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[44])() = {
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_6),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_7),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_8),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_9),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_10),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_11),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_12),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_13),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_14),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_15),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_16),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_17),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_18),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_19),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_20),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_21),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_22),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_23),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_24),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_25),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_26),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_27),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_28),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_29),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_30),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_31),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_32),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_33),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_34),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_35),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_36),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_37),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_38),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_39),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_40),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_41),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_42),
	reinterpret_cast<void(*)()>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Run),
};

static void (*kBenchmarkWrappers[44])() = {
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
	[]() { kAotMethods[12](); },
	[]() { kAotMethods[13](); },
	[]() { kAotMethods[14](); },
	[]() { kAotMethods[15](); },
	[]() { kAotMethods[16](); },
	[]() { kAotMethods[17](); },
	[]() { kAotMethods[18](); },
	[]() { kAotMethods[19](); },
	[]() { kAotMethods[20](); },
	[]() { kAotMethods[21](); },
	[]() { kAotMethods[22](); },
	[]() { kAotMethods[23](); },
	[]() { kAotMethods[24](); },
	[]() { kAotMethods[25](); },
	[]() { kAotMethods[26](); },
	[]() { kAotMethods[27](); },
	[]() { kAotMethods[28](); },
	[]() { kAotMethods[29](); },
	[]() { kAotMethods[30](); },
	[]() { kAotMethods[31](); },
	[]() { kAotMethods[32](); },
	[]() { kAotMethods[33](); },
	[]() { kAotMethods[34](); },
	[]() { kAotMethods[35](); },
	[]() { kAotMethods[36](); },
	[]() { kAotMethods[37](); },
	[]() { kAotMethods[38](); },
	[]() { kAotMethods[39](); },
	[]() { kAotMethods[40](); },
	[]() { kAotMethods[41](); },
	[]() { kAotMethods[42](); },
	[]() { reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[43])(static_cast<CHAOS_IL2CPP_INTPTR>(0)); },
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
static void* const kMethodPointers[44] = {
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_0),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_1),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_2),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_3),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_4),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_5),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_6),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_7),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_8),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_9),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_10),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_11),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_12),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_13),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_14),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_15),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_16),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_17),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_18),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_19),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_20),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_21),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_22),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_23),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_24),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_25),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_26),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_27),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_28),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_29),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_30),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_31),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_32),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_33),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_34),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_35),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_36),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_37),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_38),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_39),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_40),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_41),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_42),
	reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Run),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 44u,
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
	.image_name_utf8    = "ReflectionTypeSubjects",
};

// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryMethodDescriptor kReflMethods_ReflectionTypeSubjects_ReflectionTypeSubjects[44] = {
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_6:System.Void()", "Subject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_7:System.Void()", "Subject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_8:System.Void()", "Subject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_9:System.Void()", "Subject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_10:System.Void()", "Subject_10", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_11:System.Void()", "Subject_11", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_12:System.Void()", "Subject_12", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_13:System.Void()", "Subject_13", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_14:System.Void()", "Subject_14", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_15:System.Void()", "Subject_15", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_16:System.Void()", "Subject_16", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_17:System.Void()", "Subject_17", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_18:System.Void()", "Subject_18", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_19:System.Void()", "Subject_19", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_20:System.Void()", "Subject_20", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_21:System.Void()", "Subject_21", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_22:System.Void()", "Subject_22", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_23:System.Void()", "Subject_23", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_24:System.Void()", "Subject_24", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_25:System.Void()", "Subject_25", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_26:System.Void()", "Subject_26", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_27:System.Void()", "Subject_27", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_28:System.Void()", "Subject_28", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_29:System.Void()", "Subject_29", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_30:System.Void()", "Subject_30", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_31:System.Void()", "Subject_31", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_32:System.Void()", "Subject_32", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_33:System.Void()", "Subject_33", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_34:System.Void()", "Subject_34", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_35:System.Void()", "Subject_35", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_36:System.Void()", "Subject_36", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_37:System.Void()", "Subject_37", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_38:System.Void()", "Subject_38", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_39:System.Void()", "Subject_39", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_40:System.Void()", "Subject_40", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_41:System.Void()", "Subject_41", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_42:System.Void()", "Subject_42", "System.Void", 0, nullptr, 0u },
	{ 0u, /*metadata_token — unused by ResolveSubjectId*/ "ReflectionTypeSubjects/ReflectionTypeSubjects::Run:System.Void(System.Int32)", "Run", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects", "ReflectionTypeSubjects/ReflectionTypeSubjects", "", "ReflectionTypeSubjects", "ReflectionTypeSubjects", nullptr, nullptr, 0u, nullptr, 0u,
 kReflMethods_ReflectionTypeSubjects_ReflectionTypeSubjects, 44u },
};

static constexpr const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static constexpr ReflectionQueryImageDescriptor kReflImage = { "ReflectionTypeSubjects", kReflTypePtrs, 1u };

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

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_0()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_1()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_2()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_3()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
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


	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		const auto chaos_address = _s0;
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
	_s0 = chaos_locals[1];
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[2])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		const auto chaos_address = _s2;
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
	_s2 = chaos_locals[1];
	{
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[2])();
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
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_4()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_4(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_Name_System_String__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_Name_System_String__(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_5()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_5(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_FullName_System_String__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_FullName_System_String__(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_6()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_6(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_Namespace_System_String__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_Namespace_System_String__(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_7()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_7(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_AssemblyQualifiedName_System_String__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_AssemblyQualifiedName_System_String__(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_8()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
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
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_TypeHandle_System_RuntimeTypeHandle__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s0;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_RuntimeTypeHandle__GetHashCode_System_Int32__(ChaosLoadInt64(chaos_arg_0));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_TypeHandle_System_RuntimeTypeHandle__(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_RuntimeTypeHandle__GetHashCode_System_Int32__(ChaosLoadInt64(chaos_arg_0));
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_9()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_9(void)
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
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsValueType_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsValueType_System_Boolean__(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			if (_s1 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			}
			else
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			}
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
				{
				}
				else
				{
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
					{
						auto chaos_value = _s2;
						chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
					}
				}
				return;
			}
		}
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_10()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_10(void)
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
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsEnum_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
			{
				CHAOS_IL2CPP_FAIL();
			}
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsEnum_System_Boolean__(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			if (_s1 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			}
			else
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			}
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
				{
				}
				else
				{
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
					{
						auto chaos_value = _s2;
						chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
					}
				}
				return;
			}
		}
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_11()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_11(void)
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
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsAbstract_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsAbstract_System_Boolean__(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			if (_s1 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			}
			else
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			}
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
				{
				}
				else
				{
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
					{
						auto chaos_value = _s2;
						chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
					}
				}
				return;
			}
		}
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_12()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_12(void)
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
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsSealed_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsSealed_System_Boolean__(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			if (_s1 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			}
			else
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			}
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
				{
				}
				else
				{
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
					{
						auto chaos_value = _s2;
						chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
					}
				}
				return;
			}
		}
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_13()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_13(void)
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
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsInterface_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsInterface_System_Boolean__(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			if (_s1 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			}
			else
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			}
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
				{
				}
				else
				{
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
					{
						auto chaos_value = _s2;
						chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
					}
				}
				return;
			}
		}
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_14()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_14(void)
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
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsArray_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsArray_System_Boolean__(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			if (_s1 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			}
			else
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			}
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
				{
				}
				else
				{
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
					{
						auto chaos_value = _s2;
						chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
					}
				}
				return;
			}
		}
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_15()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_15(void)
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
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsByRef_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsByRef_System_Boolean__(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			if (_s1 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			}
			else
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			}
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
				{
				}
				else
				{
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
					{
						auto chaos_value = _s2;
						chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
					}
				}
				return;
			}
		}
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_16()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_16(void)
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
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsPointer_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsPointer_System_Boolean__(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			if (_s1 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			}
			else
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			}
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
				{
				}
				else
				{
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
					{
						auto chaos_value = _s2;
						chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
					}
				}
				return;
			}
		}
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_17()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_17(void)
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
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsGenericType_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
			{
				CHAOS_IL2CPP_FAIL();
			}
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsGenericType_System_Boolean__(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			if (_s1 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			}
			else
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			}
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
				{
				}
				else
				{
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
					{
						auto chaos_value = _s2;
						chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
					}
				}
				return;
			}
		}
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_18()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_18(void)
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
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsConstructedGenericType_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
			{
				CHAOS_IL2CPP_FAIL();
			}
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsConstructedGenericType_System_Boolean__(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			if (_s1 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			}
			else
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			}
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
				{
				}
				else
				{
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
					{
						auto chaos_value = _s2;
						chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
					}
				}
				return;
			}
		}
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_19()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_19(void)
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
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNested_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNested_System_Boolean__(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			if (_s1 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			}
			else
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			}
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
				{
				}
				else
				{
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
					{
						auto chaos_value = _s2;
						chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
					}
				}
				return;
			}
		}
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_20()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_20(void)
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
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsPublic_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsPublic_System_Boolean__(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			if (_s1 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			}
			else
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			}
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
				{
				}
				else
				{
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
					{
						auto chaos_value = _s2;
						chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
					}
				}
				return;
			}
		}
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_21()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_21(void)
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
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNotPublic_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNotPublic_System_Boolean__(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			if (_s1 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			}
			else
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			}
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
				{
				}
				else
				{
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
					{
						auto chaos_value = _s2;
						chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
					}
				}
				return;
			}
		}
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_22()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_22(void)
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
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNestedPublic_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNestedPublic_System_Boolean__(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			if (_s1 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			}
			else
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			}
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
				{
				}
				else
				{
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
					{
						auto chaos_value = _s2;
						chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
					}
				}
				return;
			}
		}
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_23()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_23(void)
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
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNestedPrivate_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNestedPrivate_System_Boolean__(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			if (_s1 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			}
			else
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			}
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
				{
				}
				else
				{
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
					{
						auto chaos_value = _s2;
						chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
					}
				}
				return;
			}
		}
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_24()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_24(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_BaseType_System_Type__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[2])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_BaseType_System_Type__(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[2])();
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
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_25()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_25(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_DeclaringType_System_Type__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[2])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_DeclaringType_System_Type__(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[2])();
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
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_26()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_26(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_ReflectedType_System_Type__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[2])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_ReflectedType_System_Type__(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[2])();
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
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_27()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_27(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_UnderlyingSystemType_System_Type__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[2])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_UnderlyingSystemType_System_Type__(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[2])();
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
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_28()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_28(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethods_System_Reflection_MethodInfo____(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethods_System_Reflection_MethodInfo____(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_29()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_29(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethods_System_Reflection_MethodInfo___System_Reflection_BindingFlags_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_1 = _s2;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethods_System_Reflection_MethodInfo___System_Reflection_BindingFlags_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_30()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_30(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetFields_System_Reflection_FieldInfo____(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetFields_System_Reflection_FieldInfo____(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_31()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_31(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetFields_System_Reflection_FieldInfo___System_Reflection_BindingFlags_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_1 = _s2;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetFields_System_Reflection_FieldInfo___System_Reflection_BindingFlags_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_32()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_32(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperties_System_Reflection_PropertyInfo____(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperties_System_Reflection_PropertyInfo____(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_33()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_33(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperties_System_Reflection_PropertyInfo___System_Reflection_BindingFlags_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_1 = _s2;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperties_System_Reflection_PropertyInfo___System_Reflection_BindingFlags_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_34()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_34(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetEvents_System_Reflection_EventInfo____(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetEvents_System_Reflection_EventInfo____(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_35()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_35(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetConstructors_System_Reflection_ConstructorInfo____(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetConstructors_System_Reflection_ConstructorInfo____(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_36()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_36(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetConstructors_System_Reflection_ConstructorInfo___System_Reflection_BindingFlags_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_1 = _s2;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetConstructors_System_Reflection_ConstructorInfo___System_Reflection_BindingFlags_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_37()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_37(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetMembers_System_Reflection_MemberInfo____(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetMembers_System_Reflection_MemberInfo____(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_38()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_38(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetInterfaces_System_Type____(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetInterfaces_System_Type____(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_39()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_39(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetElementType_System_Type__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[2])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetElementType_System_Type__(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[2])();
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
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_40()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_40(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetNestedTypes_System_Type____(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetNestedTypes_System_Type____(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_41()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_41(void)
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
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetNestedType_System_Type_System_String_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[2])();
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
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetNestedType_System_Type_System_String_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[2])();
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
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_42()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_42(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_Assembly_System_Reflection_Assembly__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[2])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s2;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_Assembly_System_Reflection_Assembly__(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[2])();
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
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Run(System.Int32)
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Run(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
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
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_0();
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
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_1();
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
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_2();
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
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_3();
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
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_4();
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
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_5();
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
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_6();
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
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_7();
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
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_8();
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
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_9();
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
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_10();
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
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_11();
					}
				}
				break;
			}
			case 12:
			{
				{
					auto& _d12 = s_hotpatch_entries[12];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d12)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d12))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d12.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_12();
					}
				}
				break;
			}
			case 13:
			{
				{
					auto& _d13 = s_hotpatch_entries[13];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d13)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d13))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d13.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_13();
					}
				}
				break;
			}
			case 14:
			{
				{
					auto& _d14 = s_hotpatch_entries[14];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d14)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d14))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d14.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_14();
					}
				}
				break;
			}
			case 15:
			{
				{
					auto& _d15 = s_hotpatch_entries[15];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d15)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d15))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d15.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_15();
					}
				}
				break;
			}
			case 16:
			{
				{
					auto& _d16 = s_hotpatch_entries[16];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d16)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d16))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d16.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_16();
					}
				}
				break;
			}
			case 17:
			{
				{
					auto& _d17 = s_hotpatch_entries[17];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d17)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d17))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d17.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_17();
					}
				}
				break;
			}
			case 18:
			{
				{
					auto& _d18 = s_hotpatch_entries[18];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d18)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d18))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d18.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_18();
					}
				}
				break;
			}
			case 19:
			{
				{
					auto& _d19 = s_hotpatch_entries[19];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d19)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d19))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d19.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_19();
					}
				}
				break;
			}
			case 20:
			{
				{
					auto& _d20 = s_hotpatch_entries[20];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d20)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d20))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d20.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_20();
					}
				}
				break;
			}
			case 21:
			{
				{
					auto& _d21 = s_hotpatch_entries[21];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d21)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d21))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d21.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_21();
					}
				}
				break;
			}
			case 22:
			{
				{
					auto& _d22 = s_hotpatch_entries[22];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d22)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d22))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d22.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_22();
					}
				}
				break;
			}
			case 23:
			{
				{
					auto& _d23 = s_hotpatch_entries[23];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d23)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d23))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d23.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_23();
					}
				}
				break;
			}
			case 24:
			{
				{
					auto& _d24 = s_hotpatch_entries[24];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d24)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d24))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d24.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_24();
					}
				}
				break;
			}
			case 25:
			{
				{
					auto& _d25 = s_hotpatch_entries[25];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d25)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d25))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d25.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_25();
					}
				}
				break;
			}
			case 26:
			{
				{
					auto& _d26 = s_hotpatch_entries[26];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d26)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d26))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d26.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_26();
					}
				}
				break;
			}
			case 27:
			{
				{
					auto& _d27 = s_hotpatch_entries[27];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d27)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d27))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d27.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_27();
					}
				}
				break;
			}
			case 28:
			{
				{
					auto& _d28 = s_hotpatch_entries[28];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d28)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d28))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d28.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_28();
					}
				}
				break;
			}
			case 29:
			{
				{
					auto& _d29 = s_hotpatch_entries[29];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d29)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d29))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d29.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_29();
					}
				}
				break;
			}
			case 30:
			{
				{
					auto& _d30 = s_hotpatch_entries[30];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d30)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d30))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d30.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_30();
					}
				}
				break;
			}
			case 31:
			{
				{
					auto& _d31 = s_hotpatch_entries[31];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d31)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d31))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d31.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_31();
					}
				}
				break;
			}
			case 32:
			{
				{
					auto& _d32 = s_hotpatch_entries[32];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d32)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d32))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d32.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_32();
					}
				}
				break;
			}
			case 33:
			{
				{
					auto& _d33 = s_hotpatch_entries[33];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d33)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d33))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d33.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_33();
					}
				}
				break;
			}
			case 34:
			{
				{
					auto& _d34 = s_hotpatch_entries[34];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d34)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d34))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d34.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_34();
					}
				}
				break;
			}
			case 35:
			{
				{
					auto& _d35 = s_hotpatch_entries[35];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d35)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d35))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d35.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_35();
					}
				}
				break;
			}
			case 36:
			{
				{
					auto& _d36 = s_hotpatch_entries[36];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d36)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d36))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d36.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_36();
					}
				}
				break;
			}
			case 37:
			{
				{
					auto& _d37 = s_hotpatch_entries[37];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d37)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d37))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d37.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_37();
					}
				}
				break;
			}
			case 38:
			{
				{
					auto& _d38 = s_hotpatch_entries[38];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d38)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d38))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d38.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_38();
					}
				}
				break;
			}
			case 39:
			{
				{
					auto& _d39 = s_hotpatch_entries[39];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d39)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d39))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d39.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_39();
					}
				}
				break;
			}
			case 40:
			{
				{
					auto& _d40 = s_hotpatch_entries[40];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d40)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d40))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d40.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_40();
					}
				}
				break;
			}
			case 41:
			{
				{
					auto& _d41 = s_hotpatch_entries[41];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d41)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d41))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d41.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_41();
					}
				}
				break;
			}
			case 42:
			{
				{
					auto& _d42 = s_hotpatch_entries[42];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d42)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d42))
					{
						alignas(16) uint8_t _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d42.method_key, _d_ab, nullptr);
					}
					else
					{
						ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_42();
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



}  // namespace chaos::il2cpp::codegen::ReflectionTypeSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 44;