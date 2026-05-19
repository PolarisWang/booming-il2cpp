#include <chaos/common.h>
#include <chaos/type_info.h>
#include "runtime_core.h"
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

// Forward declaration for dispatch table entries (defined in runtime_stubs.cpp)
extern "C" void InterpreterEntryDirect(
    CHAOS_IL2CPP_UINTPTR method_key,
    void*     args_buf,
    void*     ret_buf) noexcept;

#pragma warning(push)
#pragma warning(disable: 4065 4244)

namespace chaos::il2cpp::codegen::GarbageCollectionSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_GarbageCollectionSubjects_GarbageCollectionSubjects = {{nullptr, nullptr, 6074204908258229484ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_GarbageCollectionSubjects_GarbageCollectionSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(6074204908258229484ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {{nullptr, nullptr, 5474029880995115448ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly = static_cast<CHAOS_IL2CPP_INTPTR>(5474029880995115448ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName = {{nullptr, nullptr, 17082367815459723707ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = static_cast<CHAOS_IL2CPP_INTPTR>(17082367815459723707ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo = {{nullptr, nullptr, 4137207361503509124ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo = static_cast<CHAOS_IL2CPP_INTPTR>(4137207361503509124ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo = {{nullptr, nullptr, 17040031516751226236ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo = static_cast<CHAOS_IL2CPP_INTPTR>(17040031516751226236ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo = {{nullptr, nullptr, 10748947813473285525ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = static_cast<CHAOS_IL2CPP_INTPTR>(10748947813473285525ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_String = {{nullptr, nullptr, 1782325859292956794ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_INTPTR>(1782325859292956794ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Type = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, nullptr, 3222174513575444759ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Type = static_cast<CHAOS_IL2CPP_INTPTR>(3222174513575444759ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Int32 = {{nullptr, nullptr, 11009693519287992193ULL, 0u, 32, 2, 0}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_boxed_type_id_System_Private_CoreLib_System_Int32 = static_cast<CHAOS_IL2CPP_INTPTR>(11009693519287992193ULL);

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

struct chaos_type_GarbageCollectionSubjects_GarbageCollectionSubjects
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

struct chaos_boxed_type_System_Private_CoreLib_System_Int32
{
	PureTypeHeader header{};
	CHAOS_IL2CPP_INTPTR value = 0;
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
		case static_cast<CHAOS_IL2CPP_INTPTR>(36277100u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_0") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107589941u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_1") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112019806u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_2") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114679179u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_3") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(117174684u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_4") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107815601u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_5") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101603642u);
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

CHAOS_IL2CPP_INT32 chaos_static_GarbageCollectionSubjects_GarbageCollectionSubjects___exitCode = 0;

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Array__CreateInstance_System_Array_System_Type_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	return ChaosArrayCreateInstance(chaos_fn_arg_0, chaos_fn_arg_1);
}

static CHAOS_IL2CPP_INT64 chaos_external_runtime_System_Private_CoreLib_System_GC__GetTotalMemory_System_Int64_System_Boolean_(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	return chaos_gc_get_total_memory(chaos_fn_arg_0);
}

static void chaos_external_runtime_System_Private_CoreLib_System_GC__KeepAlive_System_Void_System_Object_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	chaos_gc_keepalive(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosObjectGetHashCode(chaos_fn_arg_0);
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

extern "C" void GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_0(void);
extern "C" void GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_1(void);
extern "C" void GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_2(void);
extern "C" void GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_3(void);
extern "C" void GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_4(void);
extern "C" void GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_5(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[7] = {
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[6];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		6u,
		0u,
		3795608245u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_0
		{ 0u, 0u },  // GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_1
		{ 0u, 0u },  // GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_2
		{ 0u, 0u },  // GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_3
		{ 0u, 0u },  // GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_4
		{ 0u, 0u },  // GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_5
	},
	{
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[2] = {
		0u,
		1548u,
	};

	static constexpr const char* s_type_names[2] = {
		"<Module>",
		"GarbageCollectionSubjects",
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
		&chaos_mt_GarbageCollectionSubjects_GarbageCollectionSubjects.hot,
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
		/* .name_utf8         = */ "GarbageCollectionSubjects",
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
		::chaos::il2cpp::runtime_core::RegisterModule("GarbageCollectionSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[6] = {
	{ "Subject_0", 0x00000003u, 0u },  // GarbageCollectionSubjects
	{ "Subject_1", 0x00000004u, 0u },  // GarbageCollectionSubjects
	{ "Subject_2", 0x00000005u, 0u },  // GarbageCollectionSubjects
	{ "Subject_3", 0x00000006u, 0u },  // GarbageCollectionSubjects
	{ "Subject_4", 0x00000007u, 0u },  // GarbageCollectionSubjects
	{ "Subject_5", 0x00000008u, 0u },  // GarbageCollectionSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "GarbageCollectionSubjects", "", 0u, 6u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[6] = {
	{ 0x00000003u, 0u },
	{ 0x00000004u, 1u },
	{ 0x00000005u, 2u },
	{ 0x00000006u, 3u },
	{ 0x00000007u, 4u },
	{ 0x00000008u, 5u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[6] = {
	{ reinterpret_cast<void*>(&GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GarbageCollectionSubjects::Subject_0
	{ reinterpret_cast<void*>(&GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GarbageCollectionSubjects::Subject_1
	{ reinterpret_cast<void*>(&GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GarbageCollectionSubjects::Subject_2
	{ reinterpret_cast<void*>(&GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GarbageCollectionSubjects::Subject_3
	{ reinterpret_cast<void*>(&GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GarbageCollectionSubjects::Subject_4
	{ reinterpret_cast<void*>(&GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GarbageCollectionSubjects::Subject_5
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"GarbageCollectionSubjects",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	6u,
	s_hotpatch_slots,
	6u,
	s_hotpatch_entries,
	6u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[11] = {
	"System.Private.CoreLib/System.GC::Collect:System.Void()",
	"System.Private.CoreLib/System.GC::WaitForPendingFinalizers:System.Void()",
	"System.Private.CoreLib/System.Int32",
	"System.Private.CoreLib/System.GC::KeepAlive:System.Void(System.Object)",
	"System.Private.CoreLib/System.GC::GetGeneration:System.Int32(System.Object)",
	"GarbageCollectionSubjects/GarbageCollectionSubjects::_exitCode",
	"System.Private.CoreLib/System.Byte",
	"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)",
	"System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)",
	"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()",
	"System.Private.CoreLib/System.GC::GetTotalMemory:System.Int64(System.Boolean)",
};

extern "C" void* kChaosExternalRuntimeFnTable[11] = {
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_GC__KeepAlive_System_Void_System_Object_),
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Array__CreateInstance_System_Array_System_Type_System_Int32_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_GC__GetTotalMemory_System_Int64_System_Boolean_),
};

extern "C" int32_t kChaosExternalRuntimeCount = 11;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[6])() = {
	reinterpret_cast<void(*)()>(&GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_5),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
static void (*kBenchmarkWrappers[6])() = {
	[]() {kAotMethods[0]();},
	[]() {kAotMethods[1]();},
	[]() {kAotMethods[2]();},
	[]() {kAotMethods[3]();},
	[]() {kAotMethods[4]();},
	[]() {kAotMethods[5]();},
};

// Single-method dispatch via hotpatch dispatch table.
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
static void* const kMethodPointers[6] = {
	reinterpret_cast<void*>(&GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_0),
	reinterpret_cast<void*>(&GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_1),
	reinterpret_cast<void*>(&GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_2),
	reinterpret_cast<void*>(&GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_3),
	reinterpret_cast<void*>(&GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_4),
	reinterpret_cast<void*>(&GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_5),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 6u,
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
	.image_name_utf8    = "GarbageCollectionSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_GarbageCollectionSubjects_GarbageCollectionSubjects[1] = {
	{ 0u, "GarbageCollectionSubjects/GarbageCollectionSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_GarbageCollectionSubjects_GarbageCollectionSubjects[6] = {
	{ 0u, "GarbageCollectionSubjects/GarbageCollectionSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "GarbageCollectionSubjects/GarbageCollectionSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "GarbageCollectionSubjects/GarbageCollectionSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "GarbageCollectionSubjects/GarbageCollectionSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "GarbageCollectionSubjects/GarbageCollectionSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "GarbageCollectionSubjects/GarbageCollectionSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0u, "GarbageCollectionSubjects/GarbageCollectionSubjects", "GarbageCollectionSubjects/GarbageCollectionSubjects", "", "GarbageCollectionSubjects", "GarbageCollectionSubjects", nullptr, kReflFields_GarbageCollectionSubjects_GarbageCollectionSubjects, 1u, nullptr, 0u,
	kReflMethods_GarbageCollectionSubjects_GarbageCollectionSubjects, 6u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "GarbageCollectionSubjects", kReflTypePtrs, 1u };

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
// Managed method: GarbageCollectionSubjects/GarbageCollectionSubjects::Subject_0()
extern "C" void GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	{
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[0])();
	}
	return;
}

// Managed method: GarbageCollectionSubjects/GarbageCollectionSubjects::Subject_1()
extern "C" void GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	{
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[1])();
	}
	return;
}

// Managed method: GarbageCollectionSubjects/GarbageCollectionSubjects::Subject_2()
extern "C" void GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_boxed_type_System_Private_CoreLib_System_Int32 chaos_box_storage_0{};
	{
		const auto chaos_value = _s0;
		chaos_box_storage_0.header.type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
		chaos_box_storage_0.value = chaos_value;
	}
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_box_storage_0);
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Private_CoreLib_System_GC__KeepAlive_System_Void_System_Object_(chaos_arg_0);
	}
	return;
}

// Managed method: GarbageCollectionSubjects/GarbageCollectionSubjects::Subject_3()
extern "C" void GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_3(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_boxed_type_System_Private_CoreLib_System_Int32 chaos_box_storage_0{};
	{
		const auto chaos_value = _s0;
		chaos_box_storage_0.header.type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
		chaos_box_storage_0.value = chaos_value;
	}
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_box_storage_0);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[4])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_GarbageCollectionSubjects_GarbageCollectionSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: GarbageCollectionSubjects/GarbageCollectionSubjects::Subject_4()
extern "C" void GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_4(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Array__CreateInstance_System_Array_System_Type_System_Int32_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(35342034);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_GarbageCollectionSubjects_GarbageCollectionSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: GarbageCollectionSubjects/GarbageCollectionSubjects::Subject_5()
extern "C" void GarbageCollectionSubjects_GarbageCollectionSubjects_Subject_5(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_GC__GetTotalMemory_System_Int64_System_Boolean_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
		_s0 = ChaosStoreInt64(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(52816);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_GarbageCollectionSubjects_GarbageCollectionSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}



}  // namespace chaos::il2cpp::codegen::GarbageCollectionSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 6;

extern "C" void ChaosJitRegisterAll() {}