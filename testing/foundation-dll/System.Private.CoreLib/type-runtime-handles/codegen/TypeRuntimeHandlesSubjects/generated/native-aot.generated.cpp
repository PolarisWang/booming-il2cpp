#include <chaos/common.h>
#include <chaos/type_info.h>
#include "runtime_core.h"
#include <chaos/eh.h>
#include "com_ccw.h"
#include "codegen_bridge.h"
#include "module_registry.h"
#include "abi_manifest.h"
#include "hotpatch_table.h"
#include "runtime_vtable.h"
#include "runtime_instantiation.h"
#include "reflection_query_model.h"
#include "load_store_chaos_bridge.h"
#include "interpreter_entry.h"
#include <gc/gc_bgc_inline.h>
#include <gc/gc_card_table.h>
#include <ChaosGeneratedRuntimePrelude.h>
#include "enum_metadata.generated.h"

// Forward declaration for dispatch table entries (defined in runtime_stubs.cpp)
extern "C" void InterpreterEntryDirect(
    CHAOS_IL2CPP_UINTPTR method_key,
    void*     args_buf,
    void*     ret_buf) noexcept;

#pragma warning(push)
#pragma warning(disable: 4065 4244)

namespace chaos::il2cpp::codegen::TypeRuntimeHandlesSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {{nullptr, nullptr, 5474029880995115448ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly = static_cast<CHAOS_IL2CPP_INTPTR>(5474029880995115448ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName = {{nullptr, nullptr, 17082367815459723707ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = static_cast<CHAOS_IL2CPP_INTPTR>(17082367815459723707ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo = {{nullptr, nullptr, 4137207361503509124ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo = static_cast<CHAOS_IL2CPP_INTPTR>(4137207361503509124ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo = {{nullptr, nullptr, 17040031516751226236ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo = static_cast<CHAOS_IL2CPP_INTPTR>(17040031516751226236ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo = {{nullptr, nullptr, 10748947813473285525ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = static_cast<CHAOS_IL2CPP_INTPTR>(10748947813473285525ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_String = {{nullptr, nullptr, 1782325859292956794ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_INTPTR>(1782325859292956794ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Type = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, nullptr, 3222174513575444759ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Type = static_cast<CHAOS_IL2CPP_INTPTR>(3222174513575444759ULL);
inline TypeInfoV0 chaos_mt_TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects = {{nullptr, nullptr, 17228237116062327654ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(17228237116062327654ULL);

// ── Virtual method table arrays ──
bool chaos_is_array_store_compatible(const chaos_managed_array* chaos_array, CHAOS_IL2CPP_INTPTR chaos_value) noexcept
{
	if (chaos_array == nullptr)
	{
		return false;
	}

	if (chaos_value == 0)
	{
		return true;
	}

	// StringId fast path: materialized value carries a heap pointer;
	// this line is reached only if caller skipped materialization.
	if (chaos_is_string_id(chaos_value))
	{
		return chaos_array->element_type_shape == chaos_type_shape_reference            && chaos_is_type_compatible(&chaos_mt_System_Private_CoreLib_System_String.hot, chaos_array->element_type_info);
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

struct chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo
{
	ThinLockableHeader header{};
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

struct chaos_type_TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects
{
	ThinLockableHeader header{};
};

const char* chaos_reflection_get_string_utf8(CHAOS_IL2CPP_INTPTR chaos_string_value) noexcept
{
	if (chaos_string_value == 0)
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
		return 0;
	}

	const auto chaos_id = chaos::il2cpp::string_table::Intern(
		chaos_utf8_data, static_cast<CHAOS_IL2CPP_UINT32>(chaos_length));
	return chaos_make_string_id_value(chaos_id);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionConcatStringPairValues(CHAOS_IL2CPP_INTPTR chaos_left_string_value, CHAOS_IL2CPP_INTPTR chaos_right_string_value)
{
	const auto chaos_left_length =
		chaos_left_string_value == 0
			? static_cast<CHAOS_IL2CPP_SIZE>(0)
			: chaos_is_string_id(chaos_left_string_value)
				? static_cast<CHAOS_IL2CPP_SIZE>(chaos::il2cpp::string_table::Resolve(
					chaos_extract_string_id(chaos_left_string_value)).byte_count)
				: static_cast<CHAOS_IL2CPP_SIZE>(reinterpret_cast<chaos_type_System_Private_CoreLib_System_String*>(chaos_left_string_value)->length);
	const auto chaos_right_length =
		chaos_right_string_value == 0
			? static_cast<CHAOS_IL2CPP_SIZE>(0)
			: chaos_is_string_id(chaos_right_string_value)
				? static_cast<CHAOS_IL2CPP_SIZE>(chaos::il2cpp::string_table::Resolve(
					chaos_extract_string_id(chaos_right_string_value)).byte_count)
				: static_cast<CHAOS_IL2CPP_SIZE>(reinterpret_cast<chaos_type_System_Private_CoreLib_System_String*>(chaos_right_string_value)->length);
	const char* chaos_left_utf8 = chaos_reflection_get_string_utf8(chaos_left_string_value);
	const char* chaos_right_utf8 = chaos_reflection_get_string_utf8(chaos_right_string_value);
	if ((chaos_left_string_value != 0 && chaos_left_utf8 == nullptr && chaos_left_length != 0) ||
		(chaos_right_string_value != 0 && chaos_right_utf8 == nullptr && chaos_right_length != 0))
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
		return 0;
	}

	return chaos_reflection_create_string_utf8_copy(chaos_utf8_data, CHAOS_IL2CPP_STRLEN(chaos_utf8_data));
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_reference_array(const TypeInfo* chaos_element_type_info, CHAOS_IL2CPP_SIZE chaos_length)
{
	auto* chaos_array = CHAOS_IL2CPP_NEW_GC(chaos_managed_array);
	chaos_array->header.type_info = &chaos_type_info_managed_array.hot;
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
			return 0;
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
	chaos_type->header.type_info = &chaos_mt_System_Private_CoreLib_System_Type.hot;
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
			return 0;
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
			return 0;
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
	if (chaos_object_value == 0)
	{
		return 0;
	}
	
	const auto* chaos_ti = chaos_object_get_type_info(reinterpret_cast<const void*>(chaos_object_value));
	switch (chaos_ti->stable_id)
	{
		default:
			return 0;
	}
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_instance(CHAOS_IL2CPP_INTPTR chaos_type_value)
{
	auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_type_value);
	if (chaos_type == nullptr)
	{
		return 0;
	}
	
	switch (chaos_type->runtime_type_handle)
	{
		default:
			return 0;
	}
}

CHAOS_IL2CPP_INTPTR chaos_reflection_resolve_method_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle, const char* chaos_method_name) noexcept
{
	if (chaos_method_name == nullptr)
	{
		return 0;
	}

	switch (chaos_type_handle)
	{
		case static_cast<CHAOS_IL2CPP_INTPTR>(42873448u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, ".ctor") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110363298u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Main") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115638315u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(49732038u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_0") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109770079u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_10") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116073940u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_11") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114766403u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_12") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115255510u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_1") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106389136u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_2") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103149177u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_3") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109806338u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_4") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106153555u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_5") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107187300u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_6") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108863869u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_7") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113702374u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_8") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116808663u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_9") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116686952u);
			}

			break;
		default:
			break;
	}

	return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethod(CHAOS_IL2CPP_INTPTR chaos_type_value, CHAOS_IL2CPP_INTPTR chaos_name_value, CHAOS_IL2CPP_INT32 chaos_binding_flags = CHAOS_IL2CPP_INT32(0))
{
	(void)chaos_binding_flags;
	auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_type_value);
	const auto* chaos_method_name = chaos_reflection_get_string_utf8(chaos_name_value);
	const auto chaos_method_handle = chaos_reflection_resolve_method_handle(chaos_type->runtime_type_handle, chaos_method_name);
	if (chaos_method_handle == 0)
	{
		return 0;
	}

	auto* chaos_method = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo);
	chaos_method->header.type_info = &chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo.hot;
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
	if (chaos_method_value == 0 || chaos_type_array_value == 0)
	{
		return 0;
	}

	auto* chaos_method = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo*>(chaos_method_value);
	auto* chaos_type_array = reinterpret_cast<chaos_managed_array*>(chaos_type_array_value);
	if (chaos_type_array->length <= 0)
	{
		return 0;
	}

	const auto chaos_type_argument_value = chaos_type_array->elements[0];
	if (chaos_type_argument_value == 0)
	{
		return 0;
	}

	auto* chaos_type_argument = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_type_argument_value);
	auto* chaos_closed_method = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo);
	chaos_closed_method->header.type_info = &chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo.hot;
	chaos_closed_method->declaring_type_handle = chaos_method->declaring_type_handle;
	chaos_closed_method->generic_definition_method_handle = chaos_method->generic_definition_method_handle != 0
		? chaos_method->generic_definition_method_handle
		: chaos_method->runtime_method_handle;
	chaos_closed_method->generic_argument_type_handle = chaos_type_argument->runtime_type_handle;
	chaos_closed_method->runtime_method_handle = chaos_reflection_create_closed_generic_method_handle(
		chaos_closed_method->generic_definition_method_handle,
		chaos_closed_method->generic_argument_type_handle);
	chaos_closed_method->runtime_name_value = chaos_method->runtime_name_value == 0
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
	if (chaos_method_value == 0)
	{
		return 0;
	}

	auto* chaos_method = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo*>(chaos_method_value);
	auto* chaos_args = reinterpret_cast<chaos_managed_array*>(chaos_args_array_value);

	return 0;
}

CHAOS_IL2CPP_INT32 chaos_static_TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects___exitCode = 0;

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosObjectGetHashCode(chaos_fn_arg_0);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	ChaosObjectCtor(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_Name_System_String__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetMemberName(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_RuntimeTypeHandle__GetHashCode_System_Int32__(CHAOS_IL2CPP_INT64 chaos_fn_arg_0)
{
	return ChaosRuntimetypehandleGetHashCode(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	if (chaos_arg_0 == 0)
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

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetFields_System_Reflection_FieldInfo____(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetFields(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethods_System_Reflection_MethodInfo____(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetMethods(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetTypeFromHandle(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_BaseType_System_Type__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetBaseType(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_FullName_System_String__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetTypeFullName(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsArray_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetIsArray(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsEnum_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetIsEnum(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsValueType_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetIsValueType(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_TypeHandle_System_RuntimeTypeHandle__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetTypeHandle(chaos_fn_arg_0);
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_0(void);
extern "C" void TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_1(void);
extern "C" void TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_2(void);
extern "C" void TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_3(void);
extern "C" void TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_4(void);
extern "C" void TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_5(void);
extern "C" void TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_6(void);
extern "C" void TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_7(void);
extern "C" void TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_8(void);
extern "C" void TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_9(void);
extern "C" void TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_10(void);
extern "C" void TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_11(void);
extern "C" void TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_12(void);
extern "C" void TypeRuntimeHandlesSubjects_AutoGeneratedProgram__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void TypeRuntimeHandlesSubjects_AutoGeneratedProgram_Main_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[16] = {
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
	1u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[15];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		15u,
		1u,
		1654918200u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_0
		{ 0u, 0u },  // TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_1
		{ 0u, 0u },  // TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_2
		{ 0u, 0u },  // TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_3
		{ 0u, 0u },  // TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_4
		{ 0u, 0u },  // TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_5
		{ 0u, 0u },  // TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_6
		{ 0u, 0u },  // TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_7
		{ 0u, 0u },  // TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_8
		{ 0u, 0u },  // TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_9
		{ 0u, 0u },  // TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_10
		{ 0u, 0u },  // TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_11
		{ 0u, 0u },  // TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_12
		{ 0u, 0u },  // TypeRuntimeHandlesSubjects_AutoGeneratedProgram__ctor
		{ 0u, 1u },  // TypeRuntimeHandlesSubjects_AutoGeneratedProgram_Main_System_String
	},
	{
		2u,
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[3] = {
		0u,
		1548u,
		0u,
	};

	static constexpr const char* s_type_names[3] = {
		"<Module>",
		"TypeRuntimeHandlesSubjects",
		"AutoGeneratedProgram",
	};

	static constexpr const char* s_type_namespaces[3] = {
		"",
		"",
		"",
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_type_parent_tokens[3] = {
		0u,
		0u,
		0u,
	};

	static const TypeInfoHot* const s_type_info_ptrs[3] = {
		nullptr,
		&chaos_mt_TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects.hot,
		nullptr,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[4] = {
		0u,
		0u,
		0u,
		0u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset[4] = {
		0u,
		0u,
		0u,
		0u,
	};



	static const ::chaos::il2cpp::runtime_core::ModuleDescriptor s_native_aot_module = {
		/* .name_utf8         = */ "TypeRuntimeHandlesSubjects",
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
		/* .type_count        = */ 3u,
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 — deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,

		/* .abi_manifest      = */ s_abi_manifest,
	};
	static const CHAOS_IL2CPP_UINT32 s_native_aot_module_id =
		::chaos::il2cpp::runtime_core::RegisterModule("TypeRuntimeHandlesSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[15] = {
	{ ".ctor", 0x00000012u, 0u },  // AutoGeneratedProgram
	{ "Main", 0x00000011u, 1u },  // AutoGeneratedProgram
	{ "Subject_0", 0x00000004u, 0u },  // TypeRuntimeHandlesSubjects
	{ "Subject_1", 0x00000005u, 0u },  // TypeRuntimeHandlesSubjects
	{ "Subject_2", 0x00000006u, 0u },  // TypeRuntimeHandlesSubjects
	{ "Subject_3", 0x00000007u, 0u },  // TypeRuntimeHandlesSubjects
	{ "Subject_4", 0x00000008u, 0u },  // TypeRuntimeHandlesSubjects
	{ "Subject_5", 0x00000009u, 0u },  // TypeRuntimeHandlesSubjects
	{ "Subject_6", 0x0000000Au, 0u },  // TypeRuntimeHandlesSubjects
	{ "Subject_7", 0x0000000Bu, 0u },  // TypeRuntimeHandlesSubjects
	{ "Subject_8", 0x0000000Cu, 0u },  // TypeRuntimeHandlesSubjects
	{ "Subject_9", 0x0000000Du, 0u },  // TypeRuntimeHandlesSubjects
	{ "Subject_10", 0x0000000Eu, 0u },  // TypeRuntimeHandlesSubjects
	{ "Subject_11", 0x0000000Fu, 0u },  // TypeRuntimeHandlesSubjects
	{ "Subject_12", 0x00000010u, 0u },  // TypeRuntimeHandlesSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[2] = {
	{ "AutoGeneratedProgram", "", 0u, 2u },
	{ "TypeRuntimeHandlesSubjects", "", 2u, 13u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[15] = {
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
	{ 0x00000011u, 14u },
	{ 0x00000012u, 13u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[15] = {
	{ reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // TypeRuntimeHandlesSubjects::Subject_0
	{ reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // TypeRuntimeHandlesSubjects::Subject_1
	{ reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // TypeRuntimeHandlesSubjects::Subject_2
	{ reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // TypeRuntimeHandlesSubjects::Subject_3
	{ reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // TypeRuntimeHandlesSubjects::Subject_4
	{ reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // TypeRuntimeHandlesSubjects::Subject_5
	{ reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // TypeRuntimeHandlesSubjects::Subject_6
	{ reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // TypeRuntimeHandlesSubjects::Subject_7
	{ reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // TypeRuntimeHandlesSubjects::Subject_8
	{ reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // TypeRuntimeHandlesSubjects::Subject_9
	{ reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // TypeRuntimeHandlesSubjects::Subject_10
	{ reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // TypeRuntimeHandlesSubjects::Subject_11
	{ reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // TypeRuntimeHandlesSubjects::Subject_12
	{ reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_AutoGeneratedProgram__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGeneratedProgram::.ctor
	{ reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_AutoGeneratedProgram_Main_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGeneratedProgram::Main
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"TypeRuntimeHandlesSubjects",
	s_hotpatch_types,
	2u,
	s_hotpatch_methods,
	15u,
	s_hotpatch_slots,
	15u,
	s_hotpatch_entries,
	15u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[17] = {
	"System.Private.CoreLib/System.RuntimeTypeHandle",
	"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)",
	"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()",
	"TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::_exitCode",
	"System.Private.CoreLib/System.Byte",
	"System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()",
	"System.Private.CoreLib/System.RuntimeTypeHandle::GetHashCode:System.Int32()",
	"System.Private.CoreLib/System.Type::get_FullName:System.String()",
	"System.Private.CoreLib/System.String::get_Length:System.Int32()",
	"System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()",
	"System.Private.CoreLib/System.Type::get_BaseType:System.Type()",
	"System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()",
	"System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()",
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
};

extern "C" void* kChaosExternalRuntimeFnTable[17] = {
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__),
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsValueType_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsEnum_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsArray_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_TypeHandle_System_RuntimeTypeHandle__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_RuntimeTypeHandle__GetHashCode_System_Int32__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_FullName_System_String__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_Name_System_String__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_BaseType_System_Type__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethods_System_Reflection_MethodInfo____),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetFields_System_Reflection_FieldInfo____),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__),
};

extern "C" int32_t kChaosExternalRuntimeCount = 17;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[15])() = {
	reinterpret_cast<void(*)()>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_6),
	reinterpret_cast<void(*)()>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_7),
	reinterpret_cast<void(*)()>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_8),
	reinterpret_cast<void(*)()>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_9),
	reinterpret_cast<void(*)()>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_10),
	reinterpret_cast<void(*)()>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_11),
	reinterpret_cast<void(*)()>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_12),
	reinterpret_cast<void(*)()>(&TypeRuntimeHandlesSubjects_AutoGeneratedProgram__ctor),
	reinterpret_cast<void(*)()>(&TypeRuntimeHandlesSubjects_AutoGeneratedProgram_Main_System_String),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[15])() = {
	[]() {kAotMethods[0]();},
	[]() {kAotMethods[1]();},
	[]() {kAotMethods[2]();},
	[]() {kAotMethods[3]();},
	[]() {kAotMethods[4]();},
	[]() {kAotMethods[5]();},
	[]() {kAotMethods[6]();},
	[]() {kAotMethods[7]();},
	[]() {kAotMethods[8]();},
	[]() {kAotMethods[9]();},
	[]() {kAotMethods[10]();},
	[]() {kAotMethods[11]();},
	[]() {kAotMethods[12]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[13])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[14])(0);},
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 13;
extern "C" const int kSubjectEntryIndices[13] = {
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	9,
	10,
	11,
	12,
};

// Single-method dispatch via hotpatch dispatch table.
// NOTE: Uses kBenchmarkWrappers (not raw kAotMethods) to ensure instance
// methods receive a valid this-pointer sentinel and default argument values.
extern "C" CHAOS_IL2CPP_INT32 RunNativeAot(
	CHAOS_IL2CPP_INT32 chaos_entry_index)
{
	if (chaos_entry_index < 0 || chaos_entry_index >= kAotMethodCount)
		return -1;
	auto& entry = s_hotpatch_entries[chaos_entry_index];
	if (chaos::il2cpp::runtime_core::HotpatchIsActive(entry)
		&& !chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(entry))
	{
		uint64_t __chaos_args[4] = {}; uint64_t __chaos_ret[2] = {};
		chaos::il2cpp::runtime_core::InterpreterEntryDirect(
			entry.method_key, __chaos_args, __chaos_ret);
	} else {
		kBenchmarkWrappers[chaos_entry_index]();
	}
	return 0;
}

// All-methods loop: run every method and return a bitmask of failures.
extern "C" CHAOS_IL2CPP_INT32 RunNativeAotAll()
{
	CHAOS_IL2CPP_INT32 result = 0;
	for (int i = 0; i < kAotMethodCount; i++) {
		auto& entry = s_hotpatch_entries[i];
		if (chaos::il2cpp::runtime_core::HotpatchIsActive(entry)
			&& !chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(entry))
		{
			uint64_t __chaos_args[4] = {}; uint64_t __chaos_ret[2] = {};
			chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				entry.method_key, __chaos_args, __chaos_ret);
		} else {
			// Use kBenchmarkWrappers which supply correct default argument values
			kBenchmarkWrappers[i]();
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
	if (chaos::il2cpp::runtime_core::HotpatchIsActive(entry)
		&& !chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(entry))
	{
		chaos::il2cpp::runtime_core::InterpreterEntryDirectFast(
			entry.method_key);
	} else {
		kBenchmarkWrappers[chaos_entry_index]();
	}
	return 0;
}

// Pure AOT benchmark: calls kBenchmarkWrappers[i] directly, no hotpatch overhead.
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
// ── GC Slot Map Section ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for precise stack root scanning.
// One nested struct entry per method with GC-referencing stack slots.
// Iterated by GcRegisterSlotMapsFromSection() which advances by
// entry_total_size bytes per entry.
#if defined(_MSC_VER)
#pragma pack(push, 1)
#endif
#if defined(__GNUC__) || defined(__clang__)
static const struct __attribute__((packed)) {
#else
static const struct {
#endif

	/* ── Entry 0: TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry0;
	/* ── Entry 1: TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry1;
	/* ── Entry 2: TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry2;
	/* ── Entry 3: TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_6 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry3;
	/* ── Entry 4: TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_7 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry4;
	/* ── Entry 5: TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_8 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry5;
	/* ── Entry 6: TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_9 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry6;
	/* ── Entry 7: TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_10 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry7;
	/* ── Entry 8: TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_11 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry8;
	/* ── Entry 9: TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_12 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry9;
	/* ── Entry 10: TypeRuntimeHandlesSubjects_AutoGeneratedProgram__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry10;
	/* ── Entry 11: TypeRuntimeHandlesSubjects_AutoGeneratedProgram_Main_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry11;
} kChaosGcSlotMapsSection = {
	/* entry0 = TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_3 */
	.entry0 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_3),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry1 = TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_4 */
	.entry1 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_4),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry2 = TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_5 */
	.entry2 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_5),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry3 = TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_6 */
	.entry3 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_6),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry4 = TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_7 */
	.entry4 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_7),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry5 = TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_8 */
	.entry5 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_8),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry6 = TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_9 */
	.entry6 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_9),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry7 = TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_10 */
	.entry7 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_10),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry8 = TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_11 */
	.entry8 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_11),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry9 = TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_12 */
	.entry9 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_12),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry10 = TypeRuntimeHandlesSubjects_AutoGeneratedProgram__ctor */
	.entry10 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&TypeRuntimeHandlesSubjects_AutoGeneratedProgram__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry11 = TypeRuntimeHandlesSubjects_AutoGeneratedProgram_Main_System_String */
	.entry11 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&TypeRuntimeHandlesSubjects_AutoGeneratedProgram_Main_System_String),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 412u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[15] = {
	reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_0),
	reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_1),
	reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_2),
	reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_3),
	reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_4),
	reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_5),
	reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_6),
	reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_7),
	reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_8),
	reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_9),
	reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_10),
	reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_11),
	reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_12),
	reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_AutoGeneratedProgram__ctor),
	reinterpret_cast<void*>(&TypeRuntimeHandlesSubjects_AutoGeneratedProgram_Main_System_String),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 15u,
	.reverse_pinvoke_wrappers  = nullptr,
	.reverse_pinvoke_wrapper_count = 0u,
	.invoker_pointers          = nullptr,
	.invoker_pointer_count     = 0u,
	.unresolved_virtual_calls = nullptr,
	.unresolved_virtual_call_count = 0u,
	.type_capabilities       = nullptr,
	.type_capability_count   = 0u,
	.vtable_descriptors = nullptr,
	.vtable_descriptor_count = 0u,
	.slot_map_section_begin = reinterpret_cast<const void*>(&kChaosGcSlotMapsSection),
	.slot_map_section_end   = reinterpret_cast<const void*>(
		reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&kChaosGcSlotMapsSection) + kChaosGcSlotMapsSize),
};

// MetadataRegistrationV0
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

// CodegenRegistrationOptionsV0
extern "C" const CodegenRegistrationOptionsV0 chaos_codegen_options
	= {
	.struct_size       = sizeof(CodegenRegistrationOptionsV0),
	.registration_flags = 0u,
	.image_name_utf8    = "TypeRuntimeHandlesSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects[1] = {
	{ 0u, "TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects[13] = {
	{ 0u, "TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::Subject_6:System.Void()", "Subject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::Subject_7:System.Void()", "Subject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::Subject_8:System.Void()", "Subject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::Subject_9:System.Void()", "Subject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, "TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::Subject_10:System.Void()", "Subject_10", "System.Void", 0, nullptr, 0u },
	{ 0u, "TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::Subject_11:System.Void()", "Subject_11", "System.Void", 0, nullptr, 0u },
	{ 0u, "TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::Subject_12:System.Void()", "Subject_12", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_TypeRuntimeHandlesSubjects_AutoGeneratedProgram[2] = {
	{ 0u, "TypeRuntimeHandlesSubjects/AutoGeneratedProgram::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "TypeRuntimeHandlesSubjects/AutoGeneratedProgram::Main:System.Void(System.String[])", "Main", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[2] = {
	{ 0u, "TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects", "TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects", "", "TypeRuntimeHandlesSubjects", "TypeRuntimeHandlesSubjects", nullptr, kReflFields_TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects, 1u, nullptr, 0u,
	kReflMethods_TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects, 13u },
	{ 0u, "TypeRuntimeHandlesSubjects/AutoGeneratedProgram", "TypeRuntimeHandlesSubjects/AutoGeneratedProgram", "", "AutoGeneratedProgram", "AutoGeneratedProgram", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_TypeRuntimeHandlesSubjects_AutoGeneratedProgram, 2u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[2] = {
	&kReflTypes[0],
	&kReflTypes[1],
};

static const ReflectionQueryImageDescriptor kReflImage = { "TypeRuntimeHandlesSubjects", kReflTypePtrs, 2u };

// Fake ImageHandle that ResolveSubjectId will decode back to kReflImage.
// BootstrapRuntime's aot_image_handle fallback discovers this via
// LookupModule(mid)->image at lines 311-321 of bootstrap.cpp.
// ── Generic registration proof-host helper ─────────────────
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
// Managed method: TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::Subject_0()
extern "C" void TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::Subject_1()
extern "C" void TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::Subject_2()
extern "C" void TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::Subject_3()
extern "C" void TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_3(void)
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


	CHAOS_EH_TRY
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::Subject_4()
extern "C" void TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_4(void)
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


	CHAOS_EH_TRY
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::Subject_5()
extern "C" void TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_5(void)
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


	CHAOS_EH_TRY
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::Subject_6()
extern "C" void TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_6(void)
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


	CHAOS_EH_TRY
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::Subject_7()
extern "C" void TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_7(void)
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
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	CHAOS_EH_TRY
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::Subject_8()
extern "C" void TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_8(void)
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


	CHAOS_EH_TRY
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::Subject_9()
extern "C" void TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_9(void)
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


	CHAOS_EH_TRY
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::Subject_10()
extern "C" void TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_10(void)
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


	CHAOS_EH_TRY
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::Subject_11()
extern "C" void TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_11(void)
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


	CHAOS_EH_TRY
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TypeRuntimeHandlesSubjects/TypeRuntimeHandlesSubjects::Subject_12()
extern "C" void TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects_Subject_12(void)
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


	CHAOS_EH_TRY
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_TypeRuntimeHandlesSubjects_TypeRuntimeHandlesSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TypeRuntimeHandlesSubjects/AutoGeneratedProgram::.ctor()
extern "C" void TypeRuntimeHandlesSubjects_AutoGeneratedProgram__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: TypeRuntimeHandlesSubjects/AutoGeneratedProgram::Main(System.String[])
extern "C" void TypeRuntimeHandlesSubjects_AutoGeneratedProgram_Main_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	return;
}



}  // namespace chaos::il2cpp::codegen::TypeRuntimeHandlesSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 15;

extern "C" void ChaosJitRegisterAll() {}