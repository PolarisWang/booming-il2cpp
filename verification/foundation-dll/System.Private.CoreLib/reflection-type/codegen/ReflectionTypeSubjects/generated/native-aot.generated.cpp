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

namespace chaos::il2cpp::codegen::ReflectionTypeSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_ReflectionTypeSubjects_ReflectionTypeSubjects = {{nullptr, nullptr, 14582329566846711146ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ReflectionTypeSubjects_ReflectionTypeSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(14582329566846711146ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_NullReferenceException = {{nullptr, nullptr, 5314911908653589872ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_NullReferenceException = static_cast<CHAOS_IL2CPP_INTPTR>(5314911908653589872ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {{nullptr, nullptr, 5474029880995115448ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly = static_cast<CHAOS_IL2CPP_INTPTR>(5474029880995115448ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName = {{nullptr, nullptr, 17082367815459723707ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = static_cast<CHAOS_IL2CPP_INTPTR>(17082367815459723707ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo = {{nullptr, nullptr, 4137207361503509124ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo = static_cast<CHAOS_IL2CPP_INTPTR>(4137207361503509124ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo = {{nullptr, nullptr, 17040031516751226236ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo = static_cast<CHAOS_IL2CPP_INTPTR>(17040031516751226236ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, nullptr, 10748947813473285525ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = static_cast<CHAOS_IL2CPP_INTPTR>(10748947813473285525ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_String = {{nullptr, nullptr, 1782325859292956794ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_INTPTR>(1782325859292956794ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Type = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, nullptr, 3222174513575444759ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Type = static_cast<CHAOS_IL2CPP_INTPTR>(3222174513575444759ULL);
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

struct chaos_type_ReflectionTypeSubjects_ReflectionTypeSubjects
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_NullReferenceException
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR _message = 0;
	CHAOS_IL2CPP_INTPTR _innerException = 0;
	CHAOS_IL2CPP_INTPTR _stackTrace = 0;
	CHAOS_IL2CPP_INT32 _HResult = 0;
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


CHAOS_IL2CPP_INTPTR chaos_string_materialize(CHAOS_IL2CPP_INTPTR chaos_value) noexcept
{
	if (chaos_value == 0)
	{
		return 0;
	}

	if (!chaos_is_string_id(chaos_value))
	{
		return chaos_value;
	}

	const auto chaos_id = chaos_extract_string_id(chaos_value);
	const auto chaos_view = chaos::il2cpp::string_table::Resolve(chaos_id);

	auto* chaos_string = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_String, {});
	chaos_string->header.type_info = &chaos_mt_System_Private_CoreLib_System_String.hot;
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

CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssembly(CHAOS_IL2CPP_INTPTR chaos_type_value)
{
	auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_type_value);
	if (chaos_type == nullptr)
	{
		return 0;
	}

	switch (chaos_type->runtime_type_handle)
	{
		default:
		{
			auto* chaos_assembly = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Reflection_Assembly);
			chaos_assembly->header.type_info = &chaos_mt_System_Private_CoreLib_System_Reflection_Assembly.hot;
			chaos_assembly->runtime_assembly_name_value = chaos_reflection_create_string_literal("System.Private.CoreLib");
			return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_assembly);
		}
	}
}

CHAOS_IL2CPP_INTPTR chaos_reflection_resolve_type_handle(CHAOS_IL2CPP_INTPTR chaos_assembly_name_value, const char* chaos_type_name) noexcept
{
	const char* chaos_assembly_name = chaos_reflection_get_string_utf8(chaos_assembly_name_value);
	if (chaos_assembly_name == nullptr || chaos_type_name == nullptr)
	{
		return 0;
	}

	return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_from_assembly(CHAOS_IL2CPP_INTPTR chaos_assembly_value, CHAOS_IL2CPP_INTPTR chaos_name_value)
{
	auto* chaos_assembly = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_Assembly*>(chaos_assembly_value);
	if (chaos_assembly == nullptr)
	{
		return 0;
	}

	const auto chaos_type_handle = chaos_reflection_resolve_type_handle(
		chaos_assembly->runtime_assembly_name_value,
		chaos_reflection_get_string_utf8(chaos_name_value));
	if (chaos_type_handle == 0)
	{
		return 0;
	}

	return chaos_reflection_create_type_value(chaos_type_handle);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeByName(CHAOS_IL2CPP_INTPTR chaos_name_value)
{
	const char* chaos_name = chaos_reflection_get_string_utf8(chaos_name_value);
	if (chaos_name == nullptr)
	{
		return 0;
	}

	return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssembly_name(CHAOS_IL2CPP_INTPTR chaos_assembly_value)
{
	auto* chaos_assembly = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_Assembly*>(chaos_assembly_value);
	if (chaos_assembly == nullptr)
	{
		return 0;
	}

	auto* chaos_assembly_name = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName);
	chaos_assembly_name->header.type_info = &chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName.hot;
	chaos_assembly_name->runtime_name_value = chaos_assembly->runtime_assembly_name_value;
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_assembly_name);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssembly_name_value(CHAOS_IL2CPP_INTPTR chaos_assembly_name_value) noexcept
{
	auto* chaos_assembly_name = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName*>(chaos_assembly_name_value);
	return chaos_assembly_name == nullptr
		? 0
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
		case static_cast<CHAOS_IL2CPP_INTPTR>(37835082u):
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

CHAOS_IL2CPP_INT32 chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = 0;


	// AOT-baked string table: sorted by StringId for binary search at runtime.
	constexpr chaos::il2cpp::string_table::StringEntry chaos_aot_string_entries[] = {
		{ 2607821981565500683U, "hello", 5u },
		{ 7644665705925415803U, "System.Int32", 12u },
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

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosObjectGetHashCode(chaos_fn_arg_0);
}

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
	static CHAOS_IL2CPP_UINT8 s_sentinel = 0;
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_sentinel);
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
	static CHAOS_IL2CPP_UINT8 s_sentinel = 0;
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_sentinel);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetNestedTypes_System_Type____(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosTypeGetNestedTypes(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperties_System_Reflection_PropertyInfo___System_Reflection_BindingFlags_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
	(void)chaos_arg_0; (void)chaos_arg_1;
	static CHAOS_IL2CPP_UINT8 s_sentinel = 0;
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_sentinel);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperties_System_Reflection_PropertyInfo____(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	(void)chaos_arg_0;
	static CHAOS_IL2CPP_UINT8 s_sentinel = 0;
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_sentinel);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetTypeFromHandle(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetType_System_Type_System_String_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetTypeByName(chaos_fn_arg_0);
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


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[44] = {
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
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[43];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		43u,
		0u,
		2759118301u,  // FNV-1a over entries+params
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
		&chaos_mt_ReflectionTypeSubjects_ReflectionTypeSubjects.hot,
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
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[43] = {
	{ "Subject_0", 0x00000003u, 0u },  // ReflectionTypeSubjects
	{ "Subject_1", 0x00000004u, 0u },  // ReflectionTypeSubjects
	{ "Subject_2", 0x00000005u, 0u },  // ReflectionTypeSubjects
	{ "Subject_3", 0x00000006u, 0u },  // ReflectionTypeSubjects
	{ "Subject_4", 0x00000007u, 0u },  // ReflectionTypeSubjects
	{ "Subject_5", 0x00000008u, 0u },  // ReflectionTypeSubjects
	{ "Subject_6", 0x00000009u, 0u },  // ReflectionTypeSubjects
	{ "Subject_7", 0x0000000Au, 0u },  // ReflectionTypeSubjects
	{ "Subject_8", 0x0000000Bu, 0u },  // ReflectionTypeSubjects
	{ "Subject_9", 0x0000000Cu, 0u },  // ReflectionTypeSubjects
	{ "Subject_10", 0x0000000Du, 0u },  // ReflectionTypeSubjects
	{ "Subject_11", 0x0000000Eu, 0u },  // ReflectionTypeSubjects
	{ "Subject_12", 0x0000000Fu, 0u },  // ReflectionTypeSubjects
	{ "Subject_13", 0x00000010u, 0u },  // ReflectionTypeSubjects
	{ "Subject_14", 0x00000011u, 0u },  // ReflectionTypeSubjects
	{ "Subject_15", 0x00000012u, 0u },  // ReflectionTypeSubjects
	{ "Subject_16", 0x00000013u, 0u },  // ReflectionTypeSubjects
	{ "Subject_17", 0x00000014u, 0u },  // ReflectionTypeSubjects
	{ "Subject_18", 0x00000015u, 0u },  // ReflectionTypeSubjects
	{ "Subject_19", 0x00000016u, 0u },  // ReflectionTypeSubjects
	{ "Subject_20", 0x00000017u, 0u },  // ReflectionTypeSubjects
	{ "Subject_21", 0x00000018u, 0u },  // ReflectionTypeSubjects
	{ "Subject_22", 0x00000019u, 0u },  // ReflectionTypeSubjects
	{ "Subject_23", 0x0000001Au, 0u },  // ReflectionTypeSubjects
	{ "Subject_24", 0x0000001Bu, 0u },  // ReflectionTypeSubjects
	{ "Subject_25", 0x0000001Cu, 0u },  // ReflectionTypeSubjects
	{ "Subject_26", 0x0000001Du, 0u },  // ReflectionTypeSubjects
	{ "Subject_27", 0x0000001Eu, 0u },  // ReflectionTypeSubjects
	{ "Subject_28", 0x0000001Fu, 0u },  // ReflectionTypeSubjects
	{ "Subject_29", 0x00000020u, 0u },  // ReflectionTypeSubjects
	{ "Subject_30", 0x00000021u, 0u },  // ReflectionTypeSubjects
	{ "Subject_31", 0x00000022u, 0u },  // ReflectionTypeSubjects
	{ "Subject_32", 0x00000023u, 0u },  // ReflectionTypeSubjects
	{ "Subject_33", 0x00000024u, 0u },  // ReflectionTypeSubjects
	{ "Subject_34", 0x00000025u, 0u },  // ReflectionTypeSubjects
	{ "Subject_35", 0x00000026u, 0u },  // ReflectionTypeSubjects
	{ "Subject_36", 0x00000027u, 0u },  // ReflectionTypeSubjects
	{ "Subject_37", 0x00000028u, 0u },  // ReflectionTypeSubjects
	{ "Subject_38", 0x00000029u, 0u },  // ReflectionTypeSubjects
	{ "Subject_39", 0x0000002Au, 0u },  // ReflectionTypeSubjects
	{ "Subject_40", 0x0000002Bu, 0u },  // ReflectionTypeSubjects
	{ "Subject_41", 0x0000002Cu, 0u },  // ReflectionTypeSubjects
	{ "Subject_42", 0x0000002Du, 0u },  // ReflectionTypeSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "ReflectionTypeSubjects", "", 0u, 43u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[43] = {
	{ 0x00000003u, 0u },
	{ 0x00000004u, 1u },
	{ 0x00000005u, 2u },
	{ 0x00000006u, 3u },
	{ 0x00000007u, 4u },
	{ 0x00000008u, 5u },
	{ 0x00000009u, 6u },
	{ 0x0000000Au, 7u },
	{ 0x0000000Bu, 8u },
	{ 0x0000000Cu, 9u },
	{ 0x0000000Du, 10u },
	{ 0x0000000Eu, 11u },
	{ 0x0000000Fu, 12u },
	{ 0x00000010u, 13u },
	{ 0x00000011u, 14u },
	{ 0x00000012u, 15u },
	{ 0x00000013u, 16u },
	{ 0x00000014u, 17u },
	{ 0x00000015u, 18u },
	{ 0x00000016u, 19u },
	{ 0x00000017u, 20u },
	{ 0x00000018u, 21u },
	{ 0x00000019u, 22u },
	{ 0x0000001Au, 23u },
	{ 0x0000001Bu, 24u },
	{ 0x0000001Cu, 25u },
	{ 0x0000001Du, 26u },
	{ 0x0000001Eu, 27u },
	{ 0x0000001Fu, 28u },
	{ 0x00000020u, 29u },
	{ 0x00000021u, 30u },
	{ 0x00000022u, 31u },
	{ 0x00000023u, 32u },
	{ 0x00000024u, 33u },
	{ 0x00000025u, 34u },
	{ 0x00000026u, 35u },
	{ 0x00000027u, 36u },
	{ 0x00000028u, 37u },
	{ 0x00000029u, 38u },
	{ 0x0000002Au, 39u },
	{ 0x0000002Bu, 40u },
	{ 0x0000002Cu, 41u },
	{ 0x0000002Du, 42u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[43] = {
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_0
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_1
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_2
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_3
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_4
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_5
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_6
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_7
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_8
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_9
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_10
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_11
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_12
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_13
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_14), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_14
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_15), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_15
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_16), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_16
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_17), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_17
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_18), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_18
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_19), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_19
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_20), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_20
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_21), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_21
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_22), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_22
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_23), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_23
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_24), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_24
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_25), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_25
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_26), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_26
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_27), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_27
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_28), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_28
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_29), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_29
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_30), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_30
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_31), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_31
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_32
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_33), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_33
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_34), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_34
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_35), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_35
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_36), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_36
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_37), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_37
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_38), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_38
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_39), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_39
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_40), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_40
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_41), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_41
	{ reinterpret_cast<void*>(&ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_42), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionTypeSubjects::Subject_42
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"ReflectionTypeSubjects",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	43u,
	s_hotpatch_slots,
	43u,
	s_hotpatch_entries,
	43u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[49] = {
	"System.Private.CoreLib/System.Type::GetType:System.Type(System.String)",
	"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()",
	"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode",
	"System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)",
	"System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)",
	"System.Private.CoreLib/System.RuntimeTypeHandle",
	"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)",
	"System.Private.CoreLib/System.Byte",
	"System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()",
	"System.Private.CoreLib/System.String::get_Length:System.Int32()",
	"System.Private.CoreLib/System.Type::get_FullName:System.String()",
	"System.Private.CoreLib/System.Type::get_Namespace:System.String()",
	"System.Private.CoreLib/System.Type::get_AssemblyQualifiedName:System.String()",
	"System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()",
	"System.Private.CoreLib/System.RuntimeTypeHandle::GetHashCode:System.Int32()",
	"System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsAbstract:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsSealed:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsInterface:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsByRef:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsPointer:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsNested:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsPublic:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsNotPublic:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsNestedPublic:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_IsNestedPrivate:System.Boolean()",
	"System.Private.CoreLib/System.Type::get_BaseType:System.Type()",
	"System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()",
	"System.Private.CoreLib/System.Reflection.MemberInfo::get_ReflectedType:System.Type()",
	"System.Private.CoreLib/System.Type::get_UnderlyingSystemType:System.Type()",
	"System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()",
	"System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[](System.Reflection.BindingFlags)",
	"System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()",
	"System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[](System.Reflection.BindingFlags)",
	"System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[]()",
	"System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[](System.Reflection.BindingFlags)",
	"System.Private.CoreLib/System.Type::GetEvents:System.Reflection.EventInfo[]()",
	"System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[]()",
	"System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[](System.Reflection.BindingFlags)",
	"System.Private.CoreLib/System.Type::GetMembers:System.Reflection.MemberInfo[]()",
	"System.Private.CoreLib/System.Type::GetInterfaces:System.Type[]()",
	"System.Private.CoreLib/System.Type::GetElementType:System.Type()",
	"System.Private.CoreLib/System.Type::GetNestedTypes:System.Type[]()",
	"System.Private.CoreLib/System.Type::GetNestedType:System.Type(System.String)",
	"System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()",
};

extern "C" void* kChaosExternalRuntimeFnTable[49] = {
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetType_System_Type_System_String_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_Name_System_String__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_FullName_System_String__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_Namespace_System_String__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_AssemblyQualifiedName_System_String__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_TypeHandle_System_RuntimeTypeHandle__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_RuntimeTypeHandle__GetHashCode_System_Int32__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsValueType_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsEnum_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsAbstract_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsSealed_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsInterface_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsArray_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsByRef_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsPointer_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsGenericType_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsConstructedGenericType_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNested_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsPublic_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNotPublic_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNestedPublic_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNestedPrivate_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_BaseType_System_Type__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_DeclaringType_System_Type__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_ReflectedType_System_Type__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_UnderlyingSystemType_System_Type__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethods_System_Reflection_MethodInfo____),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethods_System_Reflection_MethodInfo___System_Reflection_BindingFlags_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetFields_System_Reflection_FieldInfo____),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetFields_System_Reflection_FieldInfo___System_Reflection_BindingFlags_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperties_System_Reflection_PropertyInfo____),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperties_System_Reflection_PropertyInfo___System_Reflection_BindingFlags_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetEvents_System_Reflection_EventInfo____),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetConstructors_System_Reflection_ConstructorInfo____),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetConstructors_System_Reflection_ConstructorInfo___System_Reflection_BindingFlags_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetMembers_System_Reflection_MemberInfo____),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetInterfaces_System_Type____),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetElementType_System_Type__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetNestedTypes_System_Type____),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetNestedType_System_Type_System_String_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_Assembly_System_Reflection_Assembly__),
};

extern "C" int32_t kChaosExternalRuntimeCount = 49;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[43])() = {
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
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
static void (*kBenchmarkWrappers[43])() = {
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
	[]() {kAotMethods[13]();},
	[]() {kAotMethods[14]();},
	[]() {kAotMethods[15]();},
	[]() {kAotMethods[16]();},
	[]() {kAotMethods[17]();},
	[]() {kAotMethods[18]();},
	[]() {kAotMethods[19]();},
	[]() {kAotMethods[20]();},
	[]() {kAotMethods[21]();},
	[]() {kAotMethods[22]();},
	[]() {kAotMethods[23]();},
	[]() {kAotMethods[24]();},
	[]() {kAotMethods[25]();},
	[]() {kAotMethods[26]();},
	[]() {kAotMethods[27]();},
	[]() {kAotMethods[28]();},
	[]() {kAotMethods[29]();},
	[]() {kAotMethods[30]();},
	[]() {kAotMethods[31]();},
	[]() {kAotMethods[32]();},
	[]() {kAotMethods[33]();},
	[]() {kAotMethods[34]();},
	[]() {kAotMethods[35]();},
	[]() {kAotMethods[36]();},
	[]() {kAotMethods[37]();},
	[]() {kAotMethods[38]();},
	[]() {kAotMethods[39]();},
	[]() {kAotMethods[40]();},
	[]() {kAotMethods[41]();},
	[]() {kAotMethods[42]();},
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
static void* const kMethodPointers[43] = {
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
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 43u,
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
	.image_name_utf8    = "ReflectionTypeSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_ReflectionTypeSubjects_ReflectionTypeSubjects[1] = {
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_ReflectionTypeSubjects_ReflectionTypeSubjects[43] = {
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_6:System.Void()", "Subject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_7:System.Void()", "Subject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_8:System.Void()", "Subject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_9:System.Void()", "Subject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_10:System.Void()", "Subject_10", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_11:System.Void()", "Subject_11", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_12:System.Void()", "Subject_12", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_13:System.Void()", "Subject_13", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_14:System.Void()", "Subject_14", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_15:System.Void()", "Subject_15", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_16:System.Void()", "Subject_16", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_17:System.Void()", "Subject_17", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_18:System.Void()", "Subject_18", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_19:System.Void()", "Subject_19", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_20:System.Void()", "Subject_20", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_21:System.Void()", "Subject_21", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_22:System.Void()", "Subject_22", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_23:System.Void()", "Subject_23", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_24:System.Void()", "Subject_24", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_25:System.Void()", "Subject_25", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_26:System.Void()", "Subject_26", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_27:System.Void()", "Subject_27", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_28:System.Void()", "Subject_28", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_29:System.Void()", "Subject_29", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_30:System.Void()", "Subject_30", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_31:System.Void()", "Subject_31", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_32:System.Void()", "Subject_32", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_33:System.Void()", "Subject_33", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_34:System.Void()", "Subject_34", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_35:System.Void()", "Subject_35", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_36:System.Void()", "Subject_36", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_37:System.Void()", "Subject_37", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_38:System.Void()", "Subject_38", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_39:System.Void()", "Subject_39", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_40:System.Void()", "Subject_40", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_41:System.Void()", "Subject_41", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_42:System.Void()", "Subject_42", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0u, "ReflectionTypeSubjects/ReflectionTypeSubjects", "ReflectionTypeSubjects/ReflectionTypeSubjects", "", "ReflectionTypeSubjects", "ReflectionTypeSubjects", nullptr, kReflFields_ReflectionTypeSubjects_ReflectionTypeSubjects, 1u, nullptr, 0u,
	kReflMethods_ReflectionTypeSubjects_ReflectionTypeSubjects, 43u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "ReflectionTypeSubjects", kReflTypePtrs, 1u };

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
// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_0()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_0(void)
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


	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("System.Int32");
	}}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetType_System_Type_System_String_(chaos_arg_0);
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
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(7880838);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
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

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_1()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_1(void)
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


	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("System.Int32");
	}}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[3])(chaos_arg_0, chaos_arg_1);
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
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(7880838);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
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

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_2()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_2(void)
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


	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("System.Int32");
	}}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[4])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
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
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(7880838);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
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

// Managed method: ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_3()
extern "C" void ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s0;
				if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
				{
					auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
					*chaos_slot = 0;
				}
				else
				{
					*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
				}
			}
			_s0 = chaos_locals[0];
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s0;
				if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
				{
					auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
					*chaos_slot = 0;
				}
				else
				{
					*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
				}
			}
			_s0 = chaos_locals[0];
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
	}
		__except(CHAOS_SEH_FILTER_ALL())
	{
		if (reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj) < 0)
		{
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
		}
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(
			chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s0;
				if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
				{
					auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
					*chaos_slot = 0;
				}
				else
				{
					*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
				}
			}
			_s0 = chaos_locals[0];
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		else
		{
		if (reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj) < 0)
		{
			chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
		}
		if (reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj) < 0)
		{
			chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
		}
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(
			chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_Name_System_String__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_FullName_System_String__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(11);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_Namespace_System_String__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(6);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_AssemblyQualifiedName_System_String__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(103);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_TypeHandle_System_RuntimeTypeHandle__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s0;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_RuntimeTypeHandle__GetHashCode_System_Int32__(ChaosLoadInt64(chaos_arg_0));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(56793269);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsValueType_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
		{
			if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsEnum_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
		{
			if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsAbstract_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
		{
			if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsSealed_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
		{
			if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsInterface_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
		{
			if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsArray_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
		{
			if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsByRef_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
		{
			if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsPointer_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
		{
			if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsGenericType_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
		{
			if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsConstructedGenericType_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
		{
			if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNested_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
		{
			if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsPublic_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
		{
			if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNotPublic_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
		{
			if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNestedPublic_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
		{
			if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsNestedPrivate_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
		{
			if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_BaseType_System_Type__(chaos_arg_0);
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
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(115000);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
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


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_DeclaringType_System_Type__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_DeclaringType_System_Type__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
	}
		__except(CHAOS_SEH_FILTER_ALL())
	{
		if (reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj) < 0)
		{
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
		}
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(
			chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_DeclaringType_System_Type__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		else
		{
		if (reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj) < 0)
		{
			chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
		}
		if (reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj) < 0)
		{
			chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
		}
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(
			chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
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


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_ReflectedType_System_Type__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_ReflectedType_System_Type__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
	}
		__except(CHAOS_SEH_FILTER_ALL())
	{
		if (reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj) < 0)
		{
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
		}
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(
			chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_ReflectedType_System_Type__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		else
		{
		if (reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj) < 0)
		{
			chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
		}
		if (reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj) < 0)
		{
			chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
		}
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(
			chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_UnderlyingSystemType_System_Type__(chaos_arg_0);
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
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(56793269);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethods_System_Reflection_MethodInfo____(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(47);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethods_System_Reflection_MethodInfo___System_Reflection_BindingFlags_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetFields_System_Reflection_FieldInfo____(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetFields_System_Reflection_FieldInfo___System_Reflection_BindingFlags_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperties_System_Reflection_PropertyInfo____(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperties_System_Reflection_PropertyInfo___System_Reflection_BindingFlags_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetEvents_System_Reflection_EventInfo____(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetConstructors_System_Reflection_ConstructorInfo____(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetConstructors_System_Reflection_ConstructorInfo___System_Reflection_BindingFlags_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetMembers_System_Reflection_MemberInfo____(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(49);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetInterfaces_System_Type____(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(33);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
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


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetElementType_System_Type__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetElementType_System_Type__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
	}
		__except(CHAOS_SEH_FILTER_ALL())
	{
		if (reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj) < 0)
		{
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
		}
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(
			chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetElementType_System_Type__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		else
		{
		if (reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj) < 0)
		{
			chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
		}
		if (reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj) < 0)
		{
			chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
		}
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(
			chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetNestedTypes_System_Type____(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
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


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetNestedType_System_Type_System_String_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetNestedType_System_Type_System_String_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
	}
		__except(CHAOS_SEH_FILTER_ALL())
	{
		if (reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj) < 0)
		{
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
		}
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(
			chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetNestedType_System_Type_System_String_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypeSubjects_ReflectionTypeSubjects___exitCode = chaos_value;
			}
		}
		else
		{
		if (reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj) < 0)
		{
			chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
		}
		if (reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj) < 0)
		{
			chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
		}
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(
			chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__get_Assembly_System_Reflection_Assembly__(chaos_arg_0);
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
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1495009);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
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



}  // namespace chaos::il2cpp::codegen::ReflectionTypeSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 43;

// ── JIT Method Entry Table ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kMethodJson_0[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-0\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_0:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_0()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_0:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_0:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-0\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_0()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"System.Int32\",\"ilOffset\":1,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetType:System.Type(System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetType:System.Type(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetType:System.Type(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":7880838,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":26,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":27,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":36,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":30,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":31,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":36,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_1[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-1\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_1:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_1()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_1:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_1:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-1\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_1()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"System.Int32\",\"ilOffset\":1,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":7880838,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":37,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":32,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":37,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_2[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-2\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_2:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_2()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_2:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_2:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-2\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_2()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_2\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"System.Int32\",\"ilOffset\":1,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":8,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":7880838,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":29,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":38,\"ilOffset\":30,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":38,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_3[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-3\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_3:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_3()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_3:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_3:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-3\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_3()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_3\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":25,\"handlerOffset\":26,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.RuntimeTypeHandle\",\"ilOffset\":4,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.RuntimeTypeHandle\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.RuntimeTypeHandle\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":10,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":16,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":18,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":31,\"ilOffset\":24,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":31,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":31,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_4[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-4\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_4:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_4()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_4:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_4:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-4\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_4()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_4\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":11,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":4,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":37,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":32,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":37,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_5[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-5\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_5:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_5()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_5:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_5:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-5\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_5()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_5\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":11,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.Type::get_FullName:System.String()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_FullName:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_FullName:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":11,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":29,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":38,\"ilOffset\":30,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":38,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_6[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-6\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_6:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_6()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_6:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_6:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-6\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_6()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_6\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":11,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.Type::get_Namespace:System.String()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_Namespace:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_Namespace:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":6,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":37,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":32,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":37,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_7[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-7\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_7:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_7()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_7:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_7:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-7\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_7()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_7\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":11,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.Type::get_AssemblyQualifiedName:System.String()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_AssemblyQualifiedName:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_AssemblyQualifiedName:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":103,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":29,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":38,\"ilOffset\":30,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":38,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_8[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-8\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_8:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_8()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_8:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_8:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-8\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_8()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_8\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":11,\"resultType\":\"System.RuntimeTypeHandle\",\"callee\":\"System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":17,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.RuntimeTypeHandle::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.RuntimeTypeHandle::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.RuntimeTypeHandle::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":56793269,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":34,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":35,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":44,\"ilOffset\":36,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":38,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":39,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":44,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_9[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-9\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_9:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_9()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_9:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_9:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-9\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_9()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_9\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":21,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":22,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":29,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":38,\"ilOffset\":30,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":38,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_10[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-10\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_10:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_10()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_10:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_10:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-10\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_10()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_10\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":11,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":21,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":22,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":35,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":30,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":35,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_11[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-11\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_11:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_11()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_11:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_11:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-11\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_11()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_11\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Type::get_IsAbstract:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsAbstract:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsAbstract:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":21,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":22,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":35,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":30,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":35,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_12[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-12\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_12:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_12()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_12:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_12:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-12\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_12()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_12\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Type::get_IsSealed:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsSealed:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsSealed:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":21,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":22,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":29,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":38,\"ilOffset\":30,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":38,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_13[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-13\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_13:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_13()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_13:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_13:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-13\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_13()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_13\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Type::get_IsInterface:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsInterface:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsInterface:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":21,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":22,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":35,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":30,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":35,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_14[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-14\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_14:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_14()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_14:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_14:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-14\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_14()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_14\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":21,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":22,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":35,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":30,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":35,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_15[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-15\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_15:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_15()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_15:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_15:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-15\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_15()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_15\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Type::get_IsByRef:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsByRef:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsByRef:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":21,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":22,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":35,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":30,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":35,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_16[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-16\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_16:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_16()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_16:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_16:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-16\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_16()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_16\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Type::get_IsPointer:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsPointer:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsPointer:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":21,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":22,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":35,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":30,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":35,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_17[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-17\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_17:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_17()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_17:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_17:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-17\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_17()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_17\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":11,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":21,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":22,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":35,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":30,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":35,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_18[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-18\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_18:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_18()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_18:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_18:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-18\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_18()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_18\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":11,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":21,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":22,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":35,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":30,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":35,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_19[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-19\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_19:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_19()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_19:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_19:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-19\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_19()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_19\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Type::get_IsNested:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsNested:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsNested:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":21,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":22,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":35,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":30,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":35,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_20[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-20\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_20:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_20()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_20:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_20:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-20\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_20()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_20\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Type::get_IsPublic:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsPublic:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsPublic:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":21,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":22,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":29,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":38,\"ilOffset\":30,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":38,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_21[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-21\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_21:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_21()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_21:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_21:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-21\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_21()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_21\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Type::get_IsNotPublic:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsNotPublic:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsNotPublic:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":21,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":22,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":35,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":30,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":35,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_22[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-22\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_22:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_22()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_22:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_22:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-22\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_22()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_22\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Type::get_IsNestedPublic:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsNestedPublic:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsNestedPublic:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":21,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":22,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":35,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":30,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":35,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_23[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-23\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_23:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_23()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_23:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_23:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-23\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_23()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_23\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Type::get_IsNestedPrivate:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsNestedPrivate:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_IsNestedPrivate:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":21,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":22,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":35,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":30,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":35,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_24[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-24\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_24:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_24()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_24:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_24:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-24\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_24()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_24\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":11,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::get_BaseType:System.Type()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_BaseType:System.Type()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_BaseType:System.Type()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":115000,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":41,\"ilOffset\":33,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":36,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_25[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-25\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_25:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_25()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_25:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_25:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-25\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_25()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_25\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":26,\"handlerOffset\":27,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":2,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":12,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":17,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":19,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":32,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":27,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":32,\"ilOffset\":30,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":32,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_26[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-26\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_26:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_26()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_26:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_26:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-26\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_26()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_26\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":26,\"handlerOffset\":27,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":2,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":12,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Reflection.MemberInfo::get_ReflectedType:System.Type()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MemberInfo::get_ReflectedType:System.Type()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MemberInfo::get_ReflectedType:System.Type()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":17,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":19,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":32,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":27,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":32,\"ilOffset\":30,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":32,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_27[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-27\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_27:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_27()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_27:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_27:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-27\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_27()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_27\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":11,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::get_UnderlyingSystemType:System.Type()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_UnderlyingSystemType:System.Type()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_UnderlyingSystemType:System.Type()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":56793269,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":41,\"ilOffset\":33,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":36,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_28[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-28\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_28:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_28()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_28:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_28:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-28\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_28()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_28\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Reflection.MethodInfo[]\",\"callee\":\"System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldlen\",\"opCode\":46,\"ilOffset\":16,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":47,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":35,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":30,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":35,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_29[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-29\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_29:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_29()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_29:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_29:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-29\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_29()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_29\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":12,\"resultType\":\"System.Reflection.MethodInfo[]\",\"callee\":\"System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[](System.Reflection.BindingFlags)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[](System.Reflection.BindingFlags)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[](System.Reflection.BindingFlags)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldlen\",\"opCode\":46,\"ilOffset\":17,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":22,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":31,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":26,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":31,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_30[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-30\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_30:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_30()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_30:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_30:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-30\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_30()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_30\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Reflection.FieldInfo[]\",\"callee\":\"System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldlen\",\"opCode\":46,\"ilOffset\":16,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":2,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":24,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":25,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":34,\"ilOffset\":26,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":29,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":34,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_31[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-31\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_31:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_31()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_31:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_31:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-31\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_31()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_31\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":12,\"resultType\":\"System.Reflection.FieldInfo[]\",\"callee\":\"System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[](System.Reflection.BindingFlags)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[](System.Reflection.BindingFlags)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[](System.Reflection.BindingFlags)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldlen\",\"opCode\":46,\"ilOffset\":17,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":22,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":31,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":26,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":31,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_32[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-32\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_32:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_32()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_32:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_32:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-32\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_32()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_32\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Reflection.PropertyInfo[]\",\"callee\":\"System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[]()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[]()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[]()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldlen\",\"opCode\":46,\"ilOffset\":16,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":21,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":30,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":25,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":30,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_33[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-33\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_33:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_33()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_33:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_33:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-33\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_33()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_33\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":12,\"resultType\":\"System.Reflection.PropertyInfo[]\",\"callee\":\"System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[](System.Reflection.BindingFlags)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[](System.Reflection.BindingFlags)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[](System.Reflection.BindingFlags)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldlen\",\"opCode\":46,\"ilOffset\":17,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":22,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":31,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":26,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":31,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_34[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-34\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_34:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_34()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_34:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_34:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-34\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_34()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_34\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":11,\"resultType\":\"System.Reflection.EventInfo[]\",\"callee\":\"System.Private.CoreLib/System.Type::GetEvents:System.Reflection.EventInfo[]()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetEvents:System.Reflection.EventInfo[]()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetEvents:System.Reflection.EventInfo[]()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldlen\",\"opCode\":46,\"ilOffset\":16,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":21,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":30,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":25,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":30,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_35[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-35\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_35:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_35()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_35:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_35:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-35\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_35()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_35\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Reflection.ConstructorInfo[]\",\"callee\":\"System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[]()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[]()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[]()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldlen\",\"opCode\":46,\"ilOffset\":16,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":21,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":30,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":25,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":30,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_36[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-36\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_36:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_36()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_36:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_36:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-36\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_36()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_36\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":12,\"resultType\":\"System.Reflection.ConstructorInfo[]\",\"callee\":\"System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[](System.Reflection.BindingFlags)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[](System.Reflection.BindingFlags)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[](System.Reflection.BindingFlags)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldlen\",\"opCode\":46,\"ilOffset\":17,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":22,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":31,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":26,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":31,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_37[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-37\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_37:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_37()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_37:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_37:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-37\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_37()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_37\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Reflection.MemberInfo[]\",\"callee\":\"System.Private.CoreLib/System.Type::GetMembers:System.Reflection.MemberInfo[]()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetMembers:System.Reflection.MemberInfo[]()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetMembers:System.Reflection.MemberInfo[]()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldlen\",\"opCode\":46,\"ilOffset\":16,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":49,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":35,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":30,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":35,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_38[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-38\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_38:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_38()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_38:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_38:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-38\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_38()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_38\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":11,\"resultType\":\"System.Type[]\",\"callee\":\"System.Private.CoreLib/System.Type::GetInterfaces:System.Type[]()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetInterfaces:System.Type[]()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetInterfaces:System.Type[]()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldlen\",\"opCode\":46,\"ilOffset\":16,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":33,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":35,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":30,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":35,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_39[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-39\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_39:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_39()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_39:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_39:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-39\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_39()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_39\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":26,\"handlerOffset\":27,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":2,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":12,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetElementType:System.Type()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetElementType:System.Type()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetElementType:System.Type()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":17,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":19,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":32,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":27,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":32,\"ilOffset\":30,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":32,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_40[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-40\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_40:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_40()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_40:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_40:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-40\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_40()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_40\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Type[]\",\"callee\":\"System.Private.CoreLib/System.Type::GetNestedTypes:System.Type[]()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetNestedTypes:System.Type[]()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetNestedTypes:System.Type[]()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldlen\",\"opCode\":46,\"ilOffset\":16,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":21,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":30,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":25,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":30,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_41[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-41\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_41:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_41()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_41:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_41:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-41\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_41()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_41\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":31,\"handlerOffset\":32,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":2,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":12,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":17,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetNestedType:System.Type(System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetNestedType:System.Type(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetNestedType:System.Type(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":22,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":24,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":37,\"ilOffset\":30,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":37,\"ilOffset\":35,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":37,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";
static const char kMethodJson_42[] = "{\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-42\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_42:System.Void()\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_42()\",\"identity\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"definitionSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_42:System.Void()\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::Subject_42:System.Void()\",\"methodId\":\"reflection-type-subjects.reflection-type-subjects.subject-42\",\"signature\":\"System.Void ReflectionTypeSubjects::Subject_42()\"},\"nativeSymbol\":\"ReflectionTypeSubjects_ReflectionTypeSubjects_Subject_42\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":1,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":11,\"resultType\":\"System.Reflection.Assembly\",\"callee\":\"System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1495009,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":41,\"ilOffset\":33,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"ilOffset\":36,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionTypeSubjects\",\"subjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionTypeSubjects/ReflectionTypeSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionTypeSubjects\"}";

extern "C" const JitMethodEntry kChaosJitMethodEntries[43] =
{
    { kMethodJson_0, 4106u, 0x00000003u, 0u },
    { kMethodJson_1, 4253u, 0x00000004u, 0u },
    { kMethodJson_2, 4400u, 0x00000005u, 0u },
    { kMethodJson_3, 4003u, 0x00000006u, 0u },
    { kMethodJson_4, 5223u, 0x00000007u, 0u },
    { kMethodJson_5, 5185u, 0x00000008u, 0u },
    { kMethodJson_6, 5187u, 0x00000009u, 0u },
    { kMethodJson_7, 5225u, 0x0000000Au, 0u },
    { kMethodJson_8, 5479u, 0x0000000Bu, 0u },
    { kMethodJson_9, 4986u, 0x0000000Cu, 0u },
    { kMethodJson_10, 4794u, 0x0000000Du, 0u },
    { kMethodJson_11, 4802u, 0x0000000Eu, 0u },
    { kMethodJson_12, 4985u, 0x0000000Fu, 0u },
    { kMethodJson_13, 4805u, 0x00000010u, 0u },
    { kMethodJson_14, 4793u, 0x00000011u, 0u },
    { kMethodJson_15, 4793u, 0x00000012u, 0u },
    { kMethodJson_16, 4799u, 0x00000013u, 0u },
    { kMethodJson_17, 4815u, 0x00000014u, 0u },
    { kMethodJson_18, 4848u, 0x00000015u, 0u },
    { kMethodJson_19, 4796u, 0x00000016u, 0u },
    { kMethodJson_20, 4985u, 0x00000017u, 0u },
    { kMethodJson_21, 4805u, 0x00000018u, 0u },
    { kMethodJson_22, 4814u, 0x00000019u, 0u },
    { kMethodJson_23, 4817u, 0x0000001Au, 0u },
    { kMethodJson_24, 5192u, 0x0000001Bu, 0u },
    { kMethodJson_25, 4461u, 0x0000001Cu, 0u },
    { kMethodJson_26, 4461u, 0x0000001Du, 0u },
    { kMethodJson_27, 5230u, 0x0000001Eu, 0u },
    { kMethodJson_28, 4819u, 0x0000001Fu, 0u },
    { kMethodJson_29, 4733u, 0x00000020u, 0u },
    { kMethodJson_30, 4811u, 0x00000021u, 0u },
    { kMethodJson_31, 4726u, 0x00000022u, 0u },
    { kMethodJson_32, 4553u, 0x00000023u, 0u },
    { kMethodJson_33, 4750u, 0x00000024u, 0u },
    { kMethodJson_34, 4533u, 0x00000025u, 0u },
    { kMethodJson_35, 4571u, 0x00000026u, 0u },
    { kMethodJson_36, 4768u, 0x00000027u, 0u },
    { kMethodJson_37, 4819u, 0x00000028u, 0u },
    { kMethodJson_38, 4764u, 0x00000029u, 0u },
    { kMethodJson_39, 4401u, 0x0000002Au, 0u },
    { kMethodJson_40, 4480u, 0x0000002Bu, 0u },
    { kMethodJson_41, 4542u, 0x0000002Cu, 0u },
    { kMethodJson_42, 5253u, 0x0000002Du, 0u }
};

extern "C" const uint32_t kChaosJitMethodEntryCount = 43u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitMethods(kChaosJitMethodEntries, kChaosJitMethodEntryCount);
}