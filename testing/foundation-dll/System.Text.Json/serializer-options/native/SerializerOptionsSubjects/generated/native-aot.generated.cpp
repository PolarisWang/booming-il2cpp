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

namespace chaos::il2cpp::codegen::SerializerOptionsSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_SerializerOptionsSubjects_SerializerOptionsSubjects = {{nullptr, nullptr, 9066421592269645544ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SerializerOptionsSubjects_SerializerOptionsSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(9066421592269645544ULL);
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
inline TypeInfoV0 chaos_mt_System_Text_Json_JsonSerializerOptions = {{nullptr, nullptr, 16716348012167816056ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Text_Json_JsonSerializerOptions = static_cast<CHAOS_IL2CPP_INTPTR>(16716348012167816056ULL);

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

struct chaos_type_SerializerOptionsSubjects_SerializerOptionsSubjects
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

struct chaos_type_System_Text_Json_JsonSerializerOptions
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
		case static_cast<CHAOS_IL2CPP_INTPTR>(45755368u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_11") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115584892u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_0") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113970465u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_10") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111892106u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_12") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101444536u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_13") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109128679u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_14") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104996846u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_15") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106934149u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_16") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107617772u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_17") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115880091u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_18") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111902162u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_19") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114356425u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_1") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116269162u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_20") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113965577u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_21") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116638482u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_22") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114229039u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_23") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101946528u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_24") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106714317u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_25") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110714486u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_26") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103939043u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_27") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109163636u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_28") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112001793u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_29") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116493514u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_2") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101673287u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_30") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109032460u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_31") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102214459u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_32") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103808910u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_33") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109510053u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_34") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107530584u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_35") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106527751u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_36") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112242218u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_37") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115271137u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_38") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109033684u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_39") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107726147u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_3") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(100935320u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_40") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111391619u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_41") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105057044u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_42") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102747885u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_43") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114894614u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_44") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101104335u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_45") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115686208u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_46") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110103337u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_47") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(117036210u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_48") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104372347u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_49") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112873804u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_4") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116842981u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_50") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102826630u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_51") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104391197u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_52") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108886660u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_53") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(117213555u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_54") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101392930u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_55") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112275097u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_56") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104761904u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_57") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116863231u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_58") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103509502u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_59") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101521109u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_5") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114994894u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_60") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114202757u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_61") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112265454u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_62") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106371483u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_63") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114886380u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_64") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104824257u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_65") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102383498u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_66") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116397287u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_67") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108713144u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_68") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108139405u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_69") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110794806u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_6") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113493115u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_70") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101452584u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_71") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110587287u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_72") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102680570u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_73") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102063729u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_74") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(117202524u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_7") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105217356u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_8") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102446889u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_9") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109379762u);
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

CHAOS_IL2CPP_INT32 chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = 0;


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

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosObjectGetHashCode(chaos_fn_arg_0);
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

extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_0(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_1(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_2(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_3(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_4(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_5(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_6(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_7(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_8(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_9(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_10(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_CustomEntrySubject_11(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_12(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_13(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_14(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_15(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_16(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_17(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_18(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_19(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_20(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_21(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_22(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_23(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_24(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_25(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_26(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_27(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_28(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_29(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_30(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_31(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_32(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_33(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_34(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_35(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_36(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_37(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_38(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_39(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_40(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_41(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_42(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_43(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_44(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_45(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_46(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_47(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_48(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_49(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_50(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_51(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_52(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_53(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_54(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_55(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_56(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_57(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_58(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_59(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_60(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_61(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_62(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_63(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_64(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_65(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_66(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_67(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_68(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_69(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_70(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_71(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_72(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_73(void);
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_74(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[76] = {
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
	::ChaosAbiMethodEntryV0 entries[75];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		75u,
		0u,
		227570397u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_0
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_1
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_2
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_3
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_4
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_5
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_6
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_7
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_8
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_9
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_10
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_CustomEntrySubject_11
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_12
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_13
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_14
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_15
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_16
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_17
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_18
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_19
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_20
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_21
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_22
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_23
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_24
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_25
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_26
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_27
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_28
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_29
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_30
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_31
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_32
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_33
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_34
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_35
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_36
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_37
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_38
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_39
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_40
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_41
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_42
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_43
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_44
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_45
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_46
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_47
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_48
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_49
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_50
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_51
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_52
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_53
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_54
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_55
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_56
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_57
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_58
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_59
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_60
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_61
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_62
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_63
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_64
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_65
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_66
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_67
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_68
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_69
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_70
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_71
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_72
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_73
		{ 0u, 0u },  // SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_74
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
		"SerializerOptionsSubjects",
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
		&chaos_mt_SerializerOptionsSubjects_SerializerOptionsSubjects.hot,
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
		/* .name_utf8         = */ "SerializerOptionsSubjects",
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
		::chaos::il2cpp::runtime_core::RegisterModule("SerializerOptionsSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[75] = {
	{ "Subject_0", 0x00000003u, 0u },  // SerializerOptionsSubjects
	{ "Subject_1", 0x00000004u, 0u },  // SerializerOptionsSubjects
	{ "Subject_2", 0x00000005u, 0u },  // SerializerOptionsSubjects
	{ "Subject_3", 0x00000006u, 0u },  // SerializerOptionsSubjects
	{ "Subject_4", 0x00000007u, 0u },  // SerializerOptionsSubjects
	{ "Subject_5", 0x00000008u, 0u },  // SerializerOptionsSubjects
	{ "Subject_6", 0x00000009u, 0u },  // SerializerOptionsSubjects
	{ "Subject_7", 0x0000000Au, 0u },  // SerializerOptionsSubjects
	{ "Subject_8", 0x0000000Bu, 0u },  // SerializerOptionsSubjects
	{ "Subject_9", 0x0000000Cu, 0u },  // SerializerOptionsSubjects
	{ "Subject_10", 0x0000000Du, 0u },  // SerializerOptionsSubjects
	{ "CustomEntrySubject_11", 0x0000000Eu, 0u },  // SerializerOptionsSubjects
	{ "Subject_12", 0x0000000Fu, 0u },  // SerializerOptionsSubjects
	{ "Subject_13", 0x00000010u, 0u },  // SerializerOptionsSubjects
	{ "Subject_14", 0x00000011u, 0u },  // SerializerOptionsSubjects
	{ "Subject_15", 0x00000012u, 0u },  // SerializerOptionsSubjects
	{ "Subject_16", 0x00000013u, 0u },  // SerializerOptionsSubjects
	{ "Subject_17", 0x00000014u, 0u },  // SerializerOptionsSubjects
	{ "Subject_18", 0x00000015u, 0u },  // SerializerOptionsSubjects
	{ "Subject_19", 0x00000016u, 0u },  // SerializerOptionsSubjects
	{ "Subject_20", 0x00000017u, 0u },  // SerializerOptionsSubjects
	{ "Subject_21", 0x00000018u, 0u },  // SerializerOptionsSubjects
	{ "Subject_22", 0x00000019u, 0u },  // SerializerOptionsSubjects
	{ "Subject_23", 0x0000001Au, 0u },  // SerializerOptionsSubjects
	{ "Subject_24", 0x0000001Bu, 0u },  // SerializerOptionsSubjects
	{ "Subject_25", 0x0000001Cu, 0u },  // SerializerOptionsSubjects
	{ "Subject_26", 0x0000001Du, 0u },  // SerializerOptionsSubjects
	{ "Subject_27", 0x0000001Eu, 0u },  // SerializerOptionsSubjects
	{ "Subject_28", 0x0000001Fu, 0u },  // SerializerOptionsSubjects
	{ "Subject_29", 0x00000020u, 0u },  // SerializerOptionsSubjects
	{ "Subject_30", 0x00000021u, 0u },  // SerializerOptionsSubjects
	{ "Subject_31", 0x00000022u, 0u },  // SerializerOptionsSubjects
	{ "Subject_32", 0x00000023u, 0u },  // SerializerOptionsSubjects
	{ "Subject_33", 0x00000024u, 0u },  // SerializerOptionsSubjects
	{ "Subject_34", 0x00000025u, 0u },  // SerializerOptionsSubjects
	{ "Subject_35", 0x00000026u, 0u },  // SerializerOptionsSubjects
	{ "Subject_36", 0x00000027u, 0u },  // SerializerOptionsSubjects
	{ "Subject_37", 0x00000028u, 0u },  // SerializerOptionsSubjects
	{ "Subject_38", 0x00000029u, 0u },  // SerializerOptionsSubjects
	{ "Subject_39", 0x0000002Au, 0u },  // SerializerOptionsSubjects
	{ "Subject_40", 0x0000002Bu, 0u },  // SerializerOptionsSubjects
	{ "Subject_41", 0x0000002Cu, 0u },  // SerializerOptionsSubjects
	{ "Subject_42", 0x0000002Du, 0u },  // SerializerOptionsSubjects
	{ "Subject_43", 0x0000002Eu, 0u },  // SerializerOptionsSubjects
	{ "Subject_44", 0x0000002Fu, 0u },  // SerializerOptionsSubjects
	{ "Subject_45", 0x00000030u, 0u },  // SerializerOptionsSubjects
	{ "Subject_46", 0x00000031u, 0u },  // SerializerOptionsSubjects
	{ "Subject_47", 0x00000032u, 0u },  // SerializerOptionsSubjects
	{ "Subject_48", 0x00000033u, 0u },  // SerializerOptionsSubjects
	{ "Subject_49", 0x00000034u, 0u },  // SerializerOptionsSubjects
	{ "Subject_50", 0x00000035u, 0u },  // SerializerOptionsSubjects
	{ "Subject_51", 0x00000036u, 0u },  // SerializerOptionsSubjects
	{ "Subject_52", 0x00000037u, 0u },  // SerializerOptionsSubjects
	{ "Subject_53", 0x00000038u, 0u },  // SerializerOptionsSubjects
	{ "Subject_54", 0x00000039u, 0u },  // SerializerOptionsSubjects
	{ "Subject_55", 0x0000003Au, 0u },  // SerializerOptionsSubjects
	{ "Subject_56", 0x0000003Bu, 0u },  // SerializerOptionsSubjects
	{ "Subject_57", 0x0000003Cu, 0u },  // SerializerOptionsSubjects
	{ "Subject_58", 0x0000003Du, 0u },  // SerializerOptionsSubjects
	{ "Subject_59", 0x0000003Eu, 0u },  // SerializerOptionsSubjects
	{ "Subject_60", 0x0000003Fu, 0u },  // SerializerOptionsSubjects
	{ "Subject_61", 0x00000040u, 0u },  // SerializerOptionsSubjects
	{ "Subject_62", 0x00000041u, 0u },  // SerializerOptionsSubjects
	{ "Subject_63", 0x00000042u, 0u },  // SerializerOptionsSubjects
	{ "Subject_64", 0x00000043u, 0u },  // SerializerOptionsSubjects
	{ "Subject_65", 0x00000044u, 0u },  // SerializerOptionsSubjects
	{ "Subject_66", 0x00000045u, 0u },  // SerializerOptionsSubjects
	{ "Subject_67", 0x00000046u, 0u },  // SerializerOptionsSubjects
	{ "Subject_68", 0x00000047u, 0u },  // SerializerOptionsSubjects
	{ "Subject_69", 0x00000048u, 0u },  // SerializerOptionsSubjects
	{ "Subject_70", 0x00000049u, 0u },  // SerializerOptionsSubjects
	{ "Subject_71", 0x0000004Au, 0u },  // SerializerOptionsSubjects
	{ "Subject_72", 0x0000004Bu, 0u },  // SerializerOptionsSubjects
	{ "Subject_73", 0x0000004Cu, 0u },  // SerializerOptionsSubjects
	{ "Subject_74", 0x0000004Du, 0u },  // SerializerOptionsSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "SerializerOptionsSubjects", "", 0u, 75u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[75] = {
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
	{ 0x0000002Eu, 43u },
	{ 0x0000002Fu, 44u },
	{ 0x00000030u, 45u },
	{ 0x00000031u, 46u },
	{ 0x00000032u, 47u },
	{ 0x00000033u, 48u },
	{ 0x00000034u, 49u },
	{ 0x00000035u, 50u },
	{ 0x00000036u, 51u },
	{ 0x00000037u, 52u },
	{ 0x00000038u, 53u },
	{ 0x00000039u, 54u },
	{ 0x0000003Au, 55u },
	{ 0x0000003Bu, 56u },
	{ 0x0000003Cu, 57u },
	{ 0x0000003Du, 58u },
	{ 0x0000003Eu, 59u },
	{ 0x0000003Fu, 60u },
	{ 0x00000040u, 61u },
	{ 0x00000041u, 62u },
	{ 0x00000042u, 63u },
	{ 0x00000043u, 64u },
	{ 0x00000044u, 65u },
	{ 0x00000045u, 66u },
	{ 0x00000046u, 67u },
	{ 0x00000047u, 68u },
	{ 0x00000048u, 69u },
	{ 0x00000049u, 70u },
	{ 0x0000004Au, 71u },
	{ 0x0000004Bu, 72u },
	{ 0x0000004Cu, 73u },
	{ 0x0000004Du, 74u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[75] = {
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_0
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_1
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_2
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_3
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_4
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_5
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_6
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_7
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_8
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_9
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_10
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_CustomEntrySubject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::CustomEntrySubject_11
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_12
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_13
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_14), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_14
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_15), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_15
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_16), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_16
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_17), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_17
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_18), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_18
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_19), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_19
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_20), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_20
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_21), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_21
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_22), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_22
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_23), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_23
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_24), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_24
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_25), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_25
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_26), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_26
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_27), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_27
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_28), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_28
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_29), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_29
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_30), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_30
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_31), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_31
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_32
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_33), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_33
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_34), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_34
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_35), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_35
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_36), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_36
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_37), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_37
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_38), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_38
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_39), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_39
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_40), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_40
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_41), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_41
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_42), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_42
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_43), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_43
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_44), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_44
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_45), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_45
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_46), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_46
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_47), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_47
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_48), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_48
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_49), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_49
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_50), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_50
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_51), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_51
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_52), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_52
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_53), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_53
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_54), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_54
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_55), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_55
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_56), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_56
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_57), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_57
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_58), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_58
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_59), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_59
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_60), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_60
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_61), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_61
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_62), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_62
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_63), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_63
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_64), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_64
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_65), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_65
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_66), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_66
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_67), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_67
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_68), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_68
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_69), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_69
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_70), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_70
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_71), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_71
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_72), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_72
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_73), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_73
	{ reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_74), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SerializerOptionsSubjects::Subject_74
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"SerializerOptionsSubjects",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	75u,
	s_hotpatch_slots,
	75u,
	s_hotpatch_entries,
	75u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[83] = {
	"System.Text.Json/JsonNamingPolicy::ConvertName:System.String(System.String)",
	"System.Private.CoreLib/System.String::get_Length:System.Int32()",
	"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode",
	"System.Text.Json/JsonNamingPolicy::get_CamelCase:JsonNamingPolicy()",
	"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()",
	"System.Text.Json/JsonNamingPolicy::get_KebabCaseLower:JsonNamingPolicy()",
	"System.Text.Json/JsonNamingPolicy::get_KebabCaseUpper:JsonNamingPolicy()",
	"System.Text.Json/JsonNamingPolicy::get_SnakeCaseLower:JsonNamingPolicy()",
	"System.Text.Json/JsonNamingPolicy::get_SnakeCaseUpper:JsonNamingPolicy()",
	"System.Text.Json/JsonSerializerOptions::get_AllowDuplicateProperties:System.Boolean()",
	"System.Text.Json/JsonSerializerOptions::get_AllowOutOfOrderMetadataProperties:System.Boolean()",
	"System.Text.Json/JsonSerializerOptions::get_AllowTrailingCommas:System.Boolean()",
	"System.Text.Json/JsonSerializerOptions::get_Converters:System.Collections.Generic.IList<System.Text.Json.Serialization.JsonConverter>()",
	"System.Text.Json/JsonSerializerOptions::get_DefaultBufferSize:System.Int32()",
	"System.Text.Json/JsonSerializerOptions::get_DefaultIgnoreCondition:System.Text.Json.Serialization.JsonIgnoreCondition()",
	"System.Text.Json/System.Text.Json.Serialization.JsonIgnoreCondition::GetHashCode:System.Int32()",
	"System.Text.Json/JsonSerializerOptions::get_DictionaryKeyPolicy:JsonNamingPolicy()",
	"System.Text.Json/JsonSerializerOptions::get_Encoder:JavaScriptEncoder()",
	"System.Text.Json/JsonSerializerOptions::get_IgnoreNullValues:System.Boolean()",
	"System.Text.Json/JsonSerializerOptions::get_IgnoreReadOnlyFields:System.Boolean()",
	"System.Text.Json/JsonSerializerOptions::get_IgnoreReadOnlyProperties:System.Boolean()",
	"System.Text.Json/JsonSerializerOptions::get_IncludeFields:System.Boolean()",
	"System.Text.Json/JsonSerializerOptions::get_IndentCharacter:System.Char()",
	"System.Text.Json/JsonSerializerOptions::get_IndentSize:System.Int32()",
	"System.Text.Json/JsonSerializerOptions::get_IsReadOnly:System.Boolean()",
	"System.Text.Json/JsonSerializerOptions::get_MaxDepth:System.Int32()",
	"System.Text.Json/JsonSerializerOptions::get_NewLine:System.String()",
	"System.Text.Json/JsonSerializerOptions::get_NumberHandling:System.Text.Json.Serialization.JsonNumberHandling()",
	"System.Text.Json/System.Text.Json.Serialization.JsonNumberHandling::GetHashCode:System.Int32()",
	"System.Text.Json/JsonSerializerOptions::get_PreferredObjectCreationHandling:System.Text.Json.Serialization.JsonObjectCreationHandling()",
	"System.Text.Json/System.Text.Json.Serialization.JsonObjectCreationHandling::GetHashCode:System.Int32()",
	"System.Text.Json/JsonSerializerOptions::get_PropertyNameCaseInsensitive:System.Boolean()",
	"System.Text.Json/JsonSerializerOptions::get_PropertyNamingPolicy:JsonNamingPolicy()",
	"System.Text.Json/JsonSerializerOptions::get_ReadCommentHandling:JsonCommentHandling()",
	"System.Text.Json/JsonCommentHandling::GetHashCode:System.Int32()",
	"System.Text.Json/JsonSerializerOptions::get_ReferenceHandler:System.Text.Json.Serialization.ReferenceHandler()",
	"System.Text.Json/JsonSerializerOptions::get_RespectNullableAnnotations:System.Boolean()",
	"System.Text.Json/JsonSerializerOptions::get_RespectRequiredConstructorParameters:System.Boolean()",
	"System.Text.Json/JsonSerializerOptions::get_Strict:JsonSerializerOptions()",
	"System.Text.Json/JsonSerializerOptions::get_TypeInfoResolver:System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver()",
	"System.Text.Json/JsonSerializerOptions::get_TypeInfoResolverChain:System.Collections.Generic.IList<System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver>()",
	"System.Text.Json/JsonSerializerOptions::get_UnknownTypeHandling:System.Text.Json.Serialization.JsonUnknownTypeHandling()",
	"System.Text.Json/System.Text.Json.Serialization.JsonUnknownTypeHandling::GetHashCode:System.Int32()",
	"System.Text.Json/JsonSerializerOptions::get_UnmappedMemberHandling:System.Text.Json.Serialization.JsonUnmappedMemberHandling()",
	"System.Text.Json/System.Text.Json.Serialization.JsonUnmappedMemberHandling::GetHashCode:System.Int32()",
	"System.Text.Json/JsonSerializerOptions::get_Web:JsonSerializerOptions()",
	"System.Text.Json/JsonSerializerOptions::get_WriteIndented:System.Boolean()",
	"System.Private.CoreLib/System.Byte",
	"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)",
	"System.Text.Json/JsonSerializerOptions::GetConverter:System.Text.Json.Serialization.JsonConverter(System.Type)",
	"System.Text.Json/JsonSerializerOptions::GetTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type)",
	"System.Text.Json/JsonSerializerOptions::MakeReadOnly:System.Void()",
	"System.Text.Json/JsonSerializerOptions::MakeReadOnly:System.Void(System.Boolean)",
	"System.Text.Json/JsonSerializerOptions::set_AllowDuplicateProperties:System.Void(System.Boolean)",
	"System.Text.Json/JsonSerializerOptions::set_AllowOutOfOrderMetadataProperties:System.Void(System.Boolean)",
	"System.Text.Json/JsonSerializerOptions::set_AllowTrailingCommas:System.Void(System.Boolean)",
	"System.Text.Json/JsonSerializerOptions::set_DefaultBufferSize:System.Void(System.Int32)",
	"System.Text.Json/JsonSerializerOptions::set_DefaultIgnoreCondition:System.Void(System.Text.Json.Serialization.JsonIgnoreCondition)",
	"System.Text.Json/JsonSerializerOptions::set_DictionaryKeyPolicy:System.Void(JsonNamingPolicy)",
	"System.Text.Json/JsonSerializerOptions::set_Encoder:System.Void(JavaScriptEncoder)",
	"System.Text.Json/JsonSerializerOptions::set_IgnoreNullValues:System.Void(System.Boolean)",
	"System.Text.Json/JsonSerializerOptions::set_IgnoreReadOnlyFields:System.Void(System.Boolean)",
	"System.Text.Json/JsonSerializerOptions::set_IgnoreReadOnlyProperties:System.Void(System.Boolean)",
	"System.Text.Json/JsonSerializerOptions::set_IncludeFields:System.Void(System.Boolean)",
	"System.Text.Json/JsonSerializerOptions::set_IndentCharacter:System.Void(System.Char)",
	"System.Text.Json/JsonSerializerOptions::set_IndentSize:System.Void(System.Int32)",
	"System.Text.Json/JsonSerializerOptions::set_MaxDepth:System.Void(System.Int32)",
	"System.Text.Json/JsonSerializerOptions::set_NewLine:System.Void(System.String)",
	"System.Text.Json/JsonSerializerOptions::set_NumberHandling:System.Void(System.Text.Json.Serialization.JsonNumberHandling)",
	"System.Text.Json/JsonSerializerOptions::set_PreferredObjectCreationHandling:System.Void(System.Text.Json.Serialization.JsonObjectCreationHandling)",
	"System.Text.Json/JsonSerializerOptions::set_PropertyNameCaseInsensitive:System.Void(System.Boolean)",
	"System.Text.Json/JsonSerializerOptions::set_PropertyNamingPolicy:System.Void(JsonNamingPolicy)",
	"System.Text.Json/JsonSerializerOptions::set_ReadCommentHandling:System.Void(JsonCommentHandling)",
	"System.Text.Json/JsonSerializerOptions::set_ReferenceHandler:System.Void(System.Text.Json.Serialization.ReferenceHandler)",
	"System.Text.Json/JsonSerializerOptions::set_RespectNullableAnnotations:System.Void(System.Boolean)",
	"System.Text.Json/JsonSerializerOptions::set_RespectRequiredConstructorParameters:System.Void(System.Boolean)",
	"System.Text.Json/JsonSerializerOptions::set_TypeInfoResolver:System.Void(System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver)",
	"System.Text.Json/JsonSerializerOptions::set_UnknownTypeHandling:System.Void(System.Text.Json.Serialization.JsonUnknownTypeHandling)",
	"System.Text.Json/JsonSerializerOptions::set_UnmappedMemberHandling:System.Void(System.Text.Json.Serialization.JsonUnmappedMemberHandling)",
	"System.Text.Json/JsonSerializerOptions::set_WriteIndented:System.Void(System.Boolean)",
	"System.Text.Json/JsonSerializerOptions::TryGetTypeInfo:System.Boolean(System.Type,System.Text.Json.Serialization.Metadata.JsonTypeInfo&)",
	"System.Text.Json/JsonSerializerOptions::.ctor:System.Void()",
	"System.Text.Json/JsonSerializerOptions::.ctor:System.Void(JsonSerializerDefaults)",
};

extern "C" void* kChaosExternalRuntimeFnTable[83] = {
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__),
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
};

extern "C" int32_t kChaosExternalRuntimeCount = 83;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[75])() = {
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_6),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_7),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_8),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_9),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_10),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_CustomEntrySubject_11),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_12),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_13),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_14),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_15),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_16),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_17),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_18),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_19),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_20),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_21),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_22),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_23),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_24),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_25),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_26),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_27),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_28),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_29),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_30),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_31),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_32),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_33),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_34),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_35),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_36),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_37),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_38),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_39),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_40),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_41),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_42),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_43),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_44),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_45),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_46),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_47),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_48),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_49),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_50),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_51),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_52),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_53),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_54),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_55),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_56),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_57),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_58),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_59),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_60),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_61),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_62),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_63),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_64),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_65),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_66),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_67),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_68),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_69),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_70),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_71),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_72),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_73),
	reinterpret_cast<void(*)()>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_74),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[75])() = {
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
	[]() {kAotMethods[43]();},
	[]() {kAotMethods[44]();},
	[]() {kAotMethods[45]();},
	[]() {kAotMethods[46]();},
	[]() {kAotMethods[47]();},
	[]() {kAotMethods[48]();},
	[]() {kAotMethods[49]();},
	[]() {kAotMethods[50]();},
	[]() {kAotMethods[51]();},
	[]() {kAotMethods[52]();},
	[]() {kAotMethods[53]();},
	[]() {kAotMethods[54]();},
	[]() {kAotMethods[55]();},
	[]() {kAotMethods[56]();},
	[]() {kAotMethods[57]();},
	[]() {kAotMethods[58]();},
	[]() {kAotMethods[59]();},
	[]() {kAotMethods[60]();},
	[]() {kAotMethods[61]();},
	[]() {kAotMethods[62]();},
	[]() {kAotMethods[63]();},
	[]() {kAotMethods[64]();},
	[]() {kAotMethods[65]();},
	[]() {kAotMethods[66]();},
	[]() {kAotMethods[67]();},
	[]() {kAotMethods[68]();},
	[]() {kAotMethods[69]();},
	[]() {kAotMethods[70]();},
	[]() {kAotMethods[71]();},
	[]() {kAotMethods[72]();},
	[]() {kAotMethods[73]();},
	[]() {kAotMethods[74]();},
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 75;
extern "C" const int kSubjectEntryIndices[75] = {
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
	13,
	14,
	15,
	16,
	17,
	18,
	19,
	20,
	21,
	22,
	23,
	24,
	25,
	26,
	27,
	28,
	29,
	30,
	31,
	32,
	33,
	34,
	35,
	36,
	37,
	38,
	39,
	40,
	41,
	42,
	43,
	44,
	45,
	46,
	47,
	48,
	49,
	50,
	51,
	52,
	53,
	54,
	55,
	56,
	57,
	58,
	59,
	60,
	61,
	62,
	63,
	64,
	65,
	66,
	67,
	68,
	69,
	70,
	71,
	72,
	73,
	74,
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

	/* ── Entry 0: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry0;
	/* ── Entry 1: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry1;
	/* ── Entry 2: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry2;
	/* ── Entry 3: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry3;
	/* ── Entry 4: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry4;
	/* ── Entry 5: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry5;
	/* ── Entry 6: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_7 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry6;
	/* ── Entry 7: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_8 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry7;
	/* ── Entry 8: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_9 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry8;
	/* ── Entry 9: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_10 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry9;
	/* ── Entry 10: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_12 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry10;
	/* ── Entry 11: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_13 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry11;
	/* ── Entry 12: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_14 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry12;
	/* ── Entry 13: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_15 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry13;
	/* ── Entry 14: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_16 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry14;
	/* ── Entry 15: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_17 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry15;
	/* ── Entry 16: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_18 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry16;
	/* ── Entry 17: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_19 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry17;
	/* ── Entry 18: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_20 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry18;
	/* ── Entry 19: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_21 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry19;
	/* ── Entry 20: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_22 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry20;
	/* ── Entry 21: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_23 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry21;
	/* ── Entry 22: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_24 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry22;
	/* ── Entry 23: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_25 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry23;
	/* ── Entry 24: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_26 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry24;
	/* ── Entry 25: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_27 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry25;
	/* ── Entry 26: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_28 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry26;
	/* ── Entry 27: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_29 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry27;
	/* ── Entry 28: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_30 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry28;
	/* ── Entry 29: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_31 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry29;
	/* ── Entry 30: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry30;
	/* ── Entry 31: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_33 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry31;
	/* ── Entry 32: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_34 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry32;
	/* ── Entry 33: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_35 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry33;
	/* ── Entry 34: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_36 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry34;
	/* ── Entry 35: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_37 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry35;
	/* ── Entry 36: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_38 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry36;
	/* ── Entry 37: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_39 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry37;
	/* ── Entry 38: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_40 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry38;
	/* ── Entry 39: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_41 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry39;
	/* ── Entry 40: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_42 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry40;
	/* ── Entry 41: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_43 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry41;
	/* ── Entry 42: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_44 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry42;
	/* ── Entry 43: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_45 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry43;
	/* ── Entry 44: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_46 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry44;
	/* ── Entry 45: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_47 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry45;
	/* ── Entry 46: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_48 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry46;
	/* ── Entry 47: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_49 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry47;
	/* ── Entry 48: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_50 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry48;
	/* ── Entry 49: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_51 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry49;
	/* ── Entry 50: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_52 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry50;
	/* ── Entry 51: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_53 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry51;
	/* ── Entry 52: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_54 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry52;
	/* ── Entry 53: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_55 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry53;
	/* ── Entry 54: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_56 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry54;
	/* ── Entry 55: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_57 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry55;
	/* ── Entry 56: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_58 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry56;
	/* ── Entry 57: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_59 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry57;
	/* ── Entry 58: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_60 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry58;
	/* ── Entry 59: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_61 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry59;
	/* ── Entry 60: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_62 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry60;
	/* ── Entry 61: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_63 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry61;
	/* ── Entry 62: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_64 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry62;
	/* ── Entry 63: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_65 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry63;
	/* ── Entry 64: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_66 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry64;
	/* ── Entry 65: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_67 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry65;
	/* ── Entry 66: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_68 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry66;
	/* ── Entry 67: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_69 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry67;
	/* ── Entry 68: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_70 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry68;
	/* ── Entry 69: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_71 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry69;
	/* ── Entry 70: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_72 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry70;
	/* ── Entry 71: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_73 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry71;
	/* ── Entry 72: SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_74 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry72;
} kChaosGcSlotMapsSection = {
	/* entry0 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_0 */
	.entry0 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_0),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry1 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_1 */
	.entry1 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry2 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_2 */
	.entry2 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_2),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry3 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_3 */
	.entry3 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_3),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry4 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_4 */
	.entry4 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_4),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry5 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_5 */
	.entry5 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_5),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry6 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_7 */
	.entry6 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_7),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry7 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_8 */
	.entry7 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_8),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry8 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_9 */
	.entry8 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_9),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry9 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_10 */
	.entry9 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_10),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry10 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_12 */
	.entry10 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_12),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry11 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_13 */
	.entry11 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_13),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry12 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_14 */
	.entry12 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_14),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry13 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_15 */
	.entry13 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_15),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry14 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_16 */
	.entry14 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_16),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry15 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_17 */
	.entry15 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_17),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry16 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_18 */
	.entry16 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_18),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry17 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_19 */
	.entry17 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_19),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry18 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_20 */
	.entry18 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_20),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry19 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_21 */
	.entry19 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_21),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry20 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_22 */
	.entry20 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_22),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry21 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_23 */
	.entry21 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_23),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry22 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_24 */
	.entry22 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_24),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry23 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_25 */
	.entry23 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_25),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry24 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_26 */
	.entry24 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_26),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry25 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_27 */
	.entry25 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_27),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry26 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_28 */
	.entry26 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_28),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry27 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_29 */
	.entry27 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_29),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry28 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_30 */
	.entry28 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_30),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry29 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_31 */
	.entry29 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_31),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry30 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_32 */
	.entry30 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry31 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_33 */
	.entry31 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_33),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry32 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_34 */
	.entry32 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_34),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry33 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_35 */
	.entry33 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_35),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry34 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_36 */
	.entry34 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_36),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry35 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_37 */
	.entry35 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_37),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry36 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_38 */
	.entry36 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_38),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry37 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_39 */
	.entry37 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_39),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry38 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_40 */
	.entry38 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_40),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry39 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_41 */
	.entry39 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_41),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry40 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_42 */
	.entry40 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_42),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry41 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_43 */
	.entry41 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_43),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry42 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_44 */
	.entry42 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_44),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry43 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_45 */
	.entry43 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_45),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry44 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_46 */
	.entry44 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_46),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry45 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_47 */
	.entry45 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_47),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry46 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_48 */
	.entry46 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_48),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry47 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_49 */
	.entry47 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_49),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry48 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_50 */
	.entry48 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_50),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry49 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_51 */
	.entry49 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_51),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry50 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_52 */
	.entry50 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_52),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry51 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_53 */
	.entry51 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_53),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry52 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_54 */
	.entry52 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_54),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry53 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_55 */
	.entry53 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_55),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry54 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_56 */
	.entry54 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_56),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry55 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_57 */
	.entry55 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_57),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry56 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_58 */
	.entry56 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_58),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry57 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_59 */
	.entry57 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_59),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry58 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_60 */
	.entry58 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_60),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry59 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_61 */
	.entry59 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_61),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry60 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_62 */
	.entry60 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_62),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry61 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_63 */
	.entry61 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_63),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry62 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_64 */
	.entry62 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_64),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry63 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_65 */
	.entry63 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_65),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry64 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_66 */
	.entry64 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_66),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry65 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_67 */
	.entry65 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_67),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry66 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_68 */
	.entry66 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_68),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry67 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_69 */
	.entry67 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_69),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry68 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_70 */
	.entry68 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_70),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry69 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_71 */
	.entry69 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_71),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry70 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_72 */
	.entry70 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_72),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry71 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_73 */
	.entry71 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_73),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry72 = SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_74 */
	.entry72 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_74),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 2492u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[75] = {
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_0),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_1),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_2),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_3),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_4),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_5),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_6),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_7),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_8),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_9),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_10),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_CustomEntrySubject_11),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_12),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_13),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_14),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_15),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_16),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_17),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_18),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_19),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_20),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_21),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_22),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_23),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_24),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_25),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_26),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_27),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_28),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_29),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_30),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_31),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_32),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_33),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_34),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_35),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_36),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_37),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_38),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_39),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_40),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_41),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_42),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_43),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_44),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_45),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_46),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_47),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_48),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_49),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_50),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_51),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_52),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_53),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_54),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_55),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_56),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_57),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_58),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_59),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_60),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_61),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_62),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_63),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_64),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_65),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_66),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_67),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_68),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_69),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_70),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_71),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_72),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_73),
	reinterpret_cast<void*>(&SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_74),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 75u,
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
	.image_name_utf8    = "SerializerOptionsSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_SerializerOptionsSubjects_SerializerOptionsSubjects[1] = {
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_SerializerOptionsSubjects_SerializerOptionsSubjects[75] = {
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_6:System.Void()", "Subject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_7:System.Void()", "Subject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_8:System.Void()", "Subject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_9:System.Void()", "Subject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_10:System.Void()", "Subject_10", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::CustomEntrySubject_11:System.Void()", "CustomEntrySubject_11", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_12:System.Void()", "Subject_12", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_13:System.Void()", "Subject_13", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_14:System.Void()", "Subject_14", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_15:System.Void()", "Subject_15", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_16:System.Void()", "Subject_16", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_17:System.Void()", "Subject_17", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_18:System.Void()", "Subject_18", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_19:System.Void()", "Subject_19", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_20:System.Void()", "Subject_20", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_21:System.Void()", "Subject_21", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_22:System.Void()", "Subject_22", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_23:System.Void()", "Subject_23", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_24:System.Void()", "Subject_24", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_25:System.Void()", "Subject_25", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_26:System.Void()", "Subject_26", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_27:System.Void()", "Subject_27", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_28:System.Void()", "Subject_28", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_29:System.Void()", "Subject_29", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_30:System.Void()", "Subject_30", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_31:System.Void()", "Subject_31", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_32:System.Void()", "Subject_32", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_33:System.Void()", "Subject_33", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_34:System.Void()", "Subject_34", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_35:System.Void()", "Subject_35", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_36:System.Void()", "Subject_36", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_37:System.Void()", "Subject_37", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_38:System.Void()", "Subject_38", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_39:System.Void()", "Subject_39", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_40:System.Void()", "Subject_40", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_41:System.Void()", "Subject_41", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_42:System.Void()", "Subject_42", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_43:System.Void()", "Subject_43", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_44:System.Void()", "Subject_44", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_45:System.Void()", "Subject_45", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_46:System.Void()", "Subject_46", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_47:System.Void()", "Subject_47", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_48:System.Void()", "Subject_48", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_49:System.Void()", "Subject_49", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_50:System.Void()", "Subject_50", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_51:System.Void()", "Subject_51", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_52:System.Void()", "Subject_52", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_53:System.Void()", "Subject_53", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_54:System.Void()", "Subject_54", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_55:System.Void()", "Subject_55", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_56:System.Void()", "Subject_56", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_57:System.Void()", "Subject_57", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_58:System.Void()", "Subject_58", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_59:System.Void()", "Subject_59", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_60:System.Void()", "Subject_60", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_61:System.Void()", "Subject_61", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_62:System.Void()", "Subject_62", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_63:System.Void()", "Subject_63", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_64:System.Void()", "Subject_64", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_65:System.Void()", "Subject_65", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_66:System.Void()", "Subject_66", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_67:System.Void()", "Subject_67", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_68:System.Void()", "Subject_68", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_69:System.Void()", "Subject_69", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_70:System.Void()", "Subject_70", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_71:System.Void()", "Subject_71", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_72:System.Void()", "Subject_72", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_73:System.Void()", "Subject_73", "System.Void", 0, nullptr, 0u },
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_74:System.Void()", "Subject_74", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0u, "SerializerOptionsSubjects/SerializerOptionsSubjects", "SerializerOptionsSubjects/SerializerOptionsSubjects", "", "SerializerOptionsSubjects", "SerializerOptionsSubjects", nullptr, kReflFields_SerializerOptionsSubjects_SerializerOptionsSubjects, 1u, nullptr, 0u,
	kReflMethods_SerializerOptionsSubjects_SerializerOptionsSubjects, 75u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "SerializerOptionsSubjects", kReflTypePtrs, 1u };

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
// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_0()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_0(void)
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
			_s0 = 0;
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[0])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{{
				_s3 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[0])(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s3;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_1()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_1(void)
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


	CHAOS_EH_TRY
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[3])();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[3])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_2()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_2(void)
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


	CHAOS_EH_TRY
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[5])();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[5])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_3()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_3(void)
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


	CHAOS_EH_TRY
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[6])();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[6])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_4()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_4(void)
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


	CHAOS_EH_TRY
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[7])();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[7])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_5()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_5(void)
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


	CHAOS_EH_TRY
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[8])();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[8])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_6()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_6(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_7()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_7(void)
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
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[9])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[9])();
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			chaos_locals[0] = _s6;
			_s6 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s7;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_8()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_8(void)
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
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[10])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[10])();
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			chaos_locals[0] = _s6;
			_s6 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s7;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_9()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_9(void)
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
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[11])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[11])();
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			chaos_locals[0] = _s6;
			_s6 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s7;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_10()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_10(void)
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


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[12])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[12])();
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::CustomEntrySubject_11()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_CustomEntrySubject_11(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_12()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_12(void)
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


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[13])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[13])();
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_13()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_13(void)
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
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[14])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s1;
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[15])();
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s3 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[14])();
				_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s4;
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[15])();
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			chaos_locals[0] = _s4;
			_s4 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s5;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_14()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_14(void)
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


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[16])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[16])();
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_15()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_15(void)
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


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[17])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[17])();
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_16()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_16(void)
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
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[18])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[18])();
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			chaos_locals[0] = _s6;
			_s6 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s7;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_17()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_17(void)
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
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[19])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[19])();
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			chaos_locals[0] = _s6;
			_s6 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s7;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_18()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_18(void)
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
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[20])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[20])();
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			chaos_locals[0] = _s6;
			_s6 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s7;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_19()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_19(void)
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
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[21])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[21])();
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			chaos_locals[0] = _s6;
			_s6 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s7;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_20()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_20(void)
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


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[22])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[22])();
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_21()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_21(void)
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


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[23])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[23])();
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_22()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_22(void)
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
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[24])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[24])();
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			chaos_locals[0] = _s6;
			_s6 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s7;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_23()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_23(void)
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


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[25])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[25])();
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_24()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_24(void)
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


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[26])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[26])();
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s3;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_25()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_25(void)
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
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[27])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s1;
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[28])();
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s3 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[27])();
				_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s4;
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[28])();
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			chaos_locals[0] = _s4;
			_s4 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s5;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_26()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_26(void)
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
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[29])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s1;
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[30])();
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s3 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[29])();
				_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s4;
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[30])();
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			chaos_locals[0] = _s4;
			_s4 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s5;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_27()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_27(void)
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
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[31])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[31])();
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			chaos_locals[0] = _s6;
			_s6 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s7;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_28()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_28(void)
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


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[32])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[32])();
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_29()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_29(void)
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
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[33])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s1;
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[34])();
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s3 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[33])();
				_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s4;
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[34])();
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			chaos_locals[0] = _s4;
			_s4 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s5;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_30()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_30(void)
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


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[35])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[35])();
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_31()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_31(void)
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
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[36])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[36])();
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			chaos_locals[0] = _s6;
			_s6 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s7;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_32()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_32(void)
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
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[37])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[37])();
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			chaos_locals[0] = _s6;
			_s6 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s7;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_33()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_33(void)
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


	CHAOS_EH_TRY
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[38])();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[38])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_34()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_34(void)
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


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[39])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[39])();
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_35()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_35(void)
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


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[40])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[40])();
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_36()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_36(void)
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
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[41])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s1;
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[42])();
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s3 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[41])();
				_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s4;
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[42])();
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			chaos_locals[0] = _s4;
			_s4 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s5;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_37()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_37(void)
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
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[43])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s1;
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[44])();
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s3 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[43])();
				_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s4;
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[44])();
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			chaos_locals[0] = _s4;
			_s4 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s5;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_38()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_38(void)
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


	CHAOS_EH_TRY
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[45])();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[45])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_39()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_39(void)
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
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[46])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[46])();
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			chaos_locals[0] = _s6;
			_s6 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s7;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_40()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_40(void)
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
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[49])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[49])(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_41()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_41(void)
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
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[50])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[50])(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_42()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_42(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[51])();
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_43()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_43(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[52])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_44()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_44(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[53])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_45()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_45(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[54])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_46()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_46(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[55])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_47()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_47(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[56])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_48()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_48(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[57])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_49()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_49(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = 0;
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[58])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_50()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_50(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = 0;
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[59])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_51()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_51(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[60])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_52()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_52(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[61])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_53()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_53(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[62])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_54()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_54(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[63])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_55()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_55(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(65);
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[64])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_56()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_56(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[65])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_57()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_57(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[66])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_58()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_58(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[67])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_59()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_59(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[68])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_60()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_60(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[69])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_61()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_61(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[70])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_62()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_62(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = 0;
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[71])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_63()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_63(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[72])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_64()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_64(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = 0;
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[73])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_65()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_65(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[74])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_66()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_66(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[75])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_67()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_67(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = 0;
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[76])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_68()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_68(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[77])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_69()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_69(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[78])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_70()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_70(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[79])(chaos_arg_0);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_71()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_71(void)
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
	CHAOS_IL2CPP_INTPTR _s17{};
	CHAOS_IL2CPP_INTPTR _s18{};
	CHAOS_IL2CPP_INTPTR _s19{};
	CHAOS_IL2CPP_INTPTR _s20{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[80])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = 0;
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s5;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s6 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_arg_1 = _s6;
				const auto chaos_arg_0 = _s5;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[80])(chaos_arg_0, chaos_arg_1);
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			chaos_locals[0] = _s6;
			_s6 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s7;
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_72()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_72(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Text_Json_JsonSerializerOptions, {});
				chaos_object->header.type_info = &chaos_mt_System_Text_Json_JsonSerializerOptions.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_73()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_73(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Text_Json_JsonSerializerOptions, {});
				chaos_object->header.type_info = &chaos_mt_System_Text_Json_JsonSerializerOptions.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_74()
extern "C" void SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_74(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Text_Json_JsonSerializerOptions, {});
				chaos_object->header.type_info = &chaos_mt_System_Text_Json_JsonSerializerOptions.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
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
				chaos_static_SerializerOptionsSubjects_SerializerOptionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}



}  // namespace chaos::il2cpp::codegen::SerializerOptionsSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 75;

// ── JIT Method Entry Table ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kMethodJson_0[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-0\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_0:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_0()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_0:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_0:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-0\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_0()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":51,\"handlerOffset\":52,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":3,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.String\",\"callee\":\"System.Text.Json/JsonNamingPolicy::ConvertName:System.String(System.String)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonNamingPolicy::ConvertName:System.String(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonNamingPolicy::ConvertName:System.String(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":18,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":19,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":24,\"resultType\":\"System.String\",\"callee\":\"System.Text.Json/JsonNamingPolicy::ConvertName:System.String(System.String)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonNamingPolicy::ConvertName:System.String(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonNamingPolicy::ConvertName:System.String(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":34,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":36,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":37,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":40,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":49,\"ilOffset\":41,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":43,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":44,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":63,\"ilOffset\":50,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":52,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":54,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":55,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":63,\"ilOffset\":61,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":63,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_1[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-1\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_1:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_1()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_1:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_1:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-1\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_1()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":39,\"handlerOffset\":40,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"JsonNamingPolicy\",\"callee\":\"System.Text.Json/JsonNamingPolicy::get_CamelCase:JsonNamingPolicy()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonNamingPolicy::get_CamelCase:JsonNamingPolicy()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonNamingPolicy::get_CamelCase:JsonNamingPolicy()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":12,\"resultType\":\"JsonNamingPolicy\",\"callee\":\"System.Text.Json/JsonNamingPolicy::get_CamelCase:JsonNamingPolicy()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonNamingPolicy::get_CamelCase:JsonNamingPolicy()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonNamingPolicy::get_CamelCase:JsonNamingPolicy()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":37,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":32,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":51,\"ilOffset\":38,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":40,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":42,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":43,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":51,\"ilOffset\":49,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":51,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_2[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-2\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_2:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_2()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_2:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_2:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-2\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_2()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_2\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":39,\"handlerOffset\":40,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"JsonNamingPolicy\",\"callee\":\"System.Text.Json/JsonNamingPolicy::get_KebabCaseLower:JsonNamingPolicy()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonNamingPolicy::get_KebabCaseLower:JsonNamingPolicy()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonNamingPolicy::get_KebabCaseLower:JsonNamingPolicy()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":12,\"resultType\":\"JsonNamingPolicy\",\"callee\":\"System.Text.Json/JsonNamingPolicy::get_KebabCaseLower:JsonNamingPolicy()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonNamingPolicy::get_KebabCaseLower:JsonNamingPolicy()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonNamingPolicy::get_KebabCaseLower:JsonNamingPolicy()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":37,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":32,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":51,\"ilOffset\":38,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":40,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":42,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":43,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":51,\"ilOffset\":49,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":51,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_3[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-3\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_3:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_3()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_3:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_3:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-3\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_3()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_3\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":39,\"handlerOffset\":40,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"JsonNamingPolicy\",\"callee\":\"System.Text.Json/JsonNamingPolicy::get_KebabCaseUpper:JsonNamingPolicy()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonNamingPolicy::get_KebabCaseUpper:JsonNamingPolicy()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonNamingPolicy::get_KebabCaseUpper:JsonNamingPolicy()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":12,\"resultType\":\"JsonNamingPolicy\",\"callee\":\"System.Text.Json/JsonNamingPolicy::get_KebabCaseUpper:JsonNamingPolicy()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonNamingPolicy::get_KebabCaseUpper:JsonNamingPolicy()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonNamingPolicy::get_KebabCaseUpper:JsonNamingPolicy()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":37,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":32,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":51,\"ilOffset\":38,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":40,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":42,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":43,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":51,\"ilOffset\":49,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":51,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_4[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-4\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_4:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_4()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_4:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_4:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-4\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_4()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_4\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":39,\"handlerOffset\":40,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"JsonNamingPolicy\",\"callee\":\"System.Text.Json/JsonNamingPolicy::get_SnakeCaseLower:JsonNamingPolicy()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonNamingPolicy::get_SnakeCaseLower:JsonNamingPolicy()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonNamingPolicy::get_SnakeCaseLower:JsonNamingPolicy()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":12,\"resultType\":\"JsonNamingPolicy\",\"callee\":\"System.Text.Json/JsonNamingPolicy::get_SnakeCaseLower:JsonNamingPolicy()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonNamingPolicy::get_SnakeCaseLower:JsonNamingPolicy()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonNamingPolicy::get_SnakeCaseLower:JsonNamingPolicy()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":37,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":32,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":51,\"ilOffset\":38,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":40,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":42,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":43,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":51,\"ilOffset\":49,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":51,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_5[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-5\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_5:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_5()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_5:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_5:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-5\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_5()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_5\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":39,\"handlerOffset\":40,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"JsonNamingPolicy\",\"callee\":\"System.Text.Json/JsonNamingPolicy::get_SnakeCaseUpper:JsonNamingPolicy()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonNamingPolicy::get_SnakeCaseUpper:JsonNamingPolicy()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonNamingPolicy::get_SnakeCaseUpper:JsonNamingPolicy()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":12,\"resultType\":\"JsonNamingPolicy\",\"callee\":\"System.Text.Json/JsonNamingPolicy::get_SnakeCaseUpper:JsonNamingPolicy()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonNamingPolicy::get_SnakeCaseUpper:JsonNamingPolicy()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonNamingPolicy::get_SnakeCaseUpper:JsonNamingPolicy()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":37,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":32,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":51,\"ilOffset\":38,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":40,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":42,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":43,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":51,\"ilOffset\":49,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":51,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_6[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-6\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_6:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_6()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_6:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_6:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-6\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_6()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_6\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_7[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-7\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_7:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_7()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_7:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_7:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-7\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_7()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_7\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":43,\"handlerOffset\":44,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_AllowDuplicateProperties:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_AllowDuplicateProperties:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_AllowDuplicateProperties:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":13,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":14,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":14,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":15,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_AllowDuplicateProperties:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_AllowDuplicateProperties:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_AllowDuplicateProperties:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":25,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":26,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":41,\"ilOffset\":33,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":36,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":42,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":44,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":46,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":47,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":53,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":55,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_8[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-8\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_8:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_8()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_8:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_8:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-8\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_8()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_8\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":43,\"handlerOffset\":44,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_AllowOutOfOrderMetadataProperties:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_AllowOutOfOrderMetadataProperties:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_AllowOutOfOrderMetadataProperties:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":13,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":14,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":14,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":15,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_AllowOutOfOrderMetadataProperties:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_AllowOutOfOrderMetadataProperties:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_AllowOutOfOrderMetadataProperties:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":25,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":26,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":41,\"ilOffset\":33,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":36,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":42,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":44,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":46,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":47,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":53,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":55,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_9[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-9\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_9:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_9()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_9:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_9:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-9\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_9()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_9\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":43,\"handlerOffset\":44,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_AllowTrailingCommas:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_AllowTrailingCommas:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_AllowTrailingCommas:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":13,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":14,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":14,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":15,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_AllowTrailingCommas:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_AllowTrailingCommas:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_AllowTrailingCommas:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":25,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":26,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":41,\"ilOffset\":33,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":36,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":42,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":44,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":46,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":47,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":53,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":55,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_10[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-10\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_10:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_10()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_10:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_10:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-10\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_10()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_10\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":41,\"handlerOffset\":42,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Collections.Generic.IList\\u003CSystem.Text.Json.Serialization.JsonConverter\\u003E\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_Converters:System.Collections.Generic.IList\\u003CSystem.Text.Json.Serialization.JsonConverter\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_Converters:System.Collections.Generic.IList\\u003CSystem.Text.Json.Serialization.JsonConverter\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_Converters:System.Collections.Generic.IList\\u003CSystem.Text.Json.Serialization.JsonConverter\\u003E()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":13,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":14,\"resultType\":\"System.Collections.Generic.IList\\u003CSystem.Text.Json.Serialization.JsonConverter\\u003E\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_Converters:System.Collections.Generic.IList\\u003CSystem.Text.Json.Serialization.JsonConverter\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_Converters:System.Collections.Generic.IList\\u003CSystem.Text.Json.Serialization.JsonConverter\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_Converters:System.Collections.Generic.IList\\u003CSystem.Text.Json.Serialization.JsonConverter\\u003E()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":39,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":34,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":42,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":45,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":53,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_11[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.custom-entry-subject-11\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::CustomEntrySubject_11:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::CustomEntrySubject_11()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::CustomEntrySubject_11:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::CustomEntrySubject_11:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.custom-entry-subject-11\",\"signature\":\"System.Void SerializerOptionsSubjects::CustomEntrySubject_11()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_CustomEntrySubject_11\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_12[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-12\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_12:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_12()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_12:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_12:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-12\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_12()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_12\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":31,\"handlerOffset\":32,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_DefaultBufferSize:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_DefaultBufferSize:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_DefaultBufferSize:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":8,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_DefaultBufferSize:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_DefaultBufferSize:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_DefaultBufferSize:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":20,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":29,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":24,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":43,\"ilOffset\":30,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":34,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":35,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":43,\"ilOffset\":41,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":43,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_13[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-13\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_13:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_13()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_13:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_13:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-13\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_13()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_13\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":59,\"handlerOffset\":60,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Text.Json.Serialization.JsonIgnoreCondition\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_DefaultIgnoreCondition:System.Text.Json.Serialization.JsonIgnoreCondition()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_DefaultIgnoreCondition:System.Text.Json.Serialization.JsonIgnoreCondition()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_DefaultIgnoreCondition:System.Text.Json.Serialization.JsonIgnoreCondition()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.JsonIgnoreCondition::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonIgnoreCondition\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":22,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":23,\"resultType\":\"System.Text.Json.Serialization.JsonIgnoreCondition\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_DefaultIgnoreCondition:System.Text.Json.Serialization.JsonIgnoreCondition()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_DefaultIgnoreCondition:System.Text.Json.Serialization.JsonIgnoreCondition()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_DefaultIgnoreCondition:System.Text.Json.Serialization.JsonIgnoreCondition()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":37,\"resultType\":\"System.Int32\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.JsonIgnoreCondition::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonIgnoreCondition\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":42,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":45,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":47,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":48,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":57,\"ilOffset\":49,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":51,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":52,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":71,\"ilOffset\":58,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":60,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":62,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":63,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":71,\"ilOffset\":69,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":71,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_14[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-14\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_14:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_14()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_14:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_14:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-14\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_14()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_14\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":41,\"handlerOffset\":42,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"JsonNamingPolicy\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_DictionaryKeyPolicy:JsonNamingPolicy()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_DictionaryKeyPolicy:JsonNamingPolicy()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_DictionaryKeyPolicy:JsonNamingPolicy()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":13,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":14,\"resultType\":\"JsonNamingPolicy\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_DictionaryKeyPolicy:JsonNamingPolicy()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_DictionaryKeyPolicy:JsonNamingPolicy()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_DictionaryKeyPolicy:JsonNamingPolicy()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":39,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":34,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":42,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":45,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":53,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_15[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-15\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_15:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_15()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_15:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_15:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-15\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_15()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_15\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":41,\"handlerOffset\":42,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"JavaScriptEncoder\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_Encoder:JavaScriptEncoder()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_Encoder:JavaScriptEncoder()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_Encoder:JavaScriptEncoder()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":13,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":14,\"resultType\":\"JavaScriptEncoder\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_Encoder:JavaScriptEncoder()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_Encoder:JavaScriptEncoder()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_Encoder:JavaScriptEncoder()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":39,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":34,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":42,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":45,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":53,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_16[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-16\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_16:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_16()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_16:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_16:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-16\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_16()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_16\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":43,\"handlerOffset\":44,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_IgnoreNullValues:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IgnoreNullValues:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IgnoreNullValues:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":13,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":14,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":14,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":15,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_IgnoreNullValues:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IgnoreNullValues:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IgnoreNullValues:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":25,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":26,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":41,\"ilOffset\":33,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":36,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":42,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":44,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":46,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":47,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":53,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":55,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_17[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-17\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_17:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_17()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_17:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_17:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-17\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_17()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_17\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":43,\"handlerOffset\":44,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_IgnoreReadOnlyFields:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IgnoreReadOnlyFields:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IgnoreReadOnlyFields:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":13,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":14,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":14,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":15,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_IgnoreReadOnlyFields:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IgnoreReadOnlyFields:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IgnoreReadOnlyFields:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":25,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":26,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":41,\"ilOffset\":33,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":36,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":42,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":44,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":46,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":47,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":53,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":55,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_18[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-18\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_18:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_18()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_18:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_18:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-18\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_18()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_18\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":43,\"handlerOffset\":44,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_IgnoreReadOnlyProperties:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IgnoreReadOnlyProperties:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IgnoreReadOnlyProperties:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":13,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":14,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":14,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":15,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_IgnoreReadOnlyProperties:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IgnoreReadOnlyProperties:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IgnoreReadOnlyProperties:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":25,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":26,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":41,\"ilOffset\":33,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":36,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":42,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":44,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":46,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":47,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":53,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":55,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_19[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-19\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_19:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_19()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_19:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_19:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-19\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_19()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_19\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":43,\"handlerOffset\":44,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_IncludeFields:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IncludeFields:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IncludeFields:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":13,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":14,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":14,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":15,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_IncludeFields:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IncludeFields:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IncludeFields:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":25,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":26,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":41,\"ilOffset\":33,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":36,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":42,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":44,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":46,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":47,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":53,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":55,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_20[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-20\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_20:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_20()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_20:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_20:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-20\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_20()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_20\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":31,\"handlerOffset\":32,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Char\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_IndentCharacter:System.Char()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IndentCharacter:System.Char()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IndentCharacter:System.Char()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":8,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":9,\"resultType\":\"System.Char\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_IndentCharacter:System.Char()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IndentCharacter:System.Char()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IndentCharacter:System.Char()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":20,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":29,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":24,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":43,\"ilOffset\":30,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":34,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":35,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":43,\"ilOffset\":41,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":43,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_21[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-21\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_21:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_21()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_21:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_21:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-21\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_21()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_21\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":31,\"handlerOffset\":32,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_IndentSize:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IndentSize:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IndentSize:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":8,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_IndentSize:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IndentSize:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IndentSize:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":20,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":29,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":24,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":43,\"ilOffset\":30,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":34,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":35,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":43,\"ilOffset\":41,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":43,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_22[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-22\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_22:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_22()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_22:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_22:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-22\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_22()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_22\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":43,\"handlerOffset\":44,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_IsReadOnly:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IsReadOnly:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IsReadOnly:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":13,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":14,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":14,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":15,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_IsReadOnly:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IsReadOnly:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_IsReadOnly:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":25,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":26,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":41,\"ilOffset\":33,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":36,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":42,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":44,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":46,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":47,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":53,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":55,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_23[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-23\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_23:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_23()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_23:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_23:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-23\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_23()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_23\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":31,\"handlerOffset\":32,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_MaxDepth:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_MaxDepth:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_MaxDepth:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":8,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_MaxDepth:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_MaxDepth:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_MaxDepth:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":20,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":29,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":24,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":43,\"ilOffset\":30,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":34,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":35,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":43,\"ilOffset\":41,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":43,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_24[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-24\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_24:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_24()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_24:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_24:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-24\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_24()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_24\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":41,\"handlerOffset\":42,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.String\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_NewLine:System.String()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_NewLine:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_NewLine:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":13,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":14,\"resultType\":\"System.String\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_NewLine:System.String()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_NewLine:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_NewLine:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":39,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":34,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":42,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":45,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":53,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_25[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-25\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_25:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_25()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_25:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_25:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-25\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_25()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_25\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":59,\"handlerOffset\":60,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Text.Json.Serialization.JsonNumberHandling\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_NumberHandling:System.Text.Json.Serialization.JsonNumberHandling()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_NumberHandling:System.Text.Json.Serialization.JsonNumberHandling()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_NumberHandling:System.Text.Json.Serialization.JsonNumberHandling()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.JsonNumberHandling::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonNumberHandling\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":22,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":23,\"resultType\":\"System.Text.Json.Serialization.JsonNumberHandling\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_NumberHandling:System.Text.Json.Serialization.JsonNumberHandling()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_NumberHandling:System.Text.Json.Serialization.JsonNumberHandling()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_NumberHandling:System.Text.Json.Serialization.JsonNumberHandling()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":37,\"resultType\":\"System.Int32\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.JsonNumberHandling::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonNumberHandling\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":42,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":45,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":47,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":48,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":57,\"ilOffset\":49,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":51,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":52,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":71,\"ilOffset\":58,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":60,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":62,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":63,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":71,\"ilOffset\":69,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":71,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_26[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-26\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_26:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_26()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_26:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_26:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-26\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_26()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_26\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":59,\"handlerOffset\":60,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Text.Json.Serialization.JsonObjectCreationHandling\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_PreferredObjectCreationHandling:System.Text.Json.Serialization.JsonObjectCreationHandling()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_PreferredObjectCreationHandling:System.Text.Json.Serialization.JsonObjectCreationHandling()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_PreferredObjectCreationHandling:System.Text.Json.Serialization.JsonObjectCreationHandling()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.JsonObjectCreationHandling::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonObjectCreationHandling\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":22,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":23,\"resultType\":\"System.Text.Json.Serialization.JsonObjectCreationHandling\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_PreferredObjectCreationHandling:System.Text.Json.Serialization.JsonObjectCreationHandling()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_PreferredObjectCreationHandling:System.Text.Json.Serialization.JsonObjectCreationHandling()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_PreferredObjectCreationHandling:System.Text.Json.Serialization.JsonObjectCreationHandling()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":37,\"resultType\":\"System.Int32\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.JsonObjectCreationHandling::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonObjectCreationHandling\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":42,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":45,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":47,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":48,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":57,\"ilOffset\":49,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":51,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":52,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":71,\"ilOffset\":58,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":60,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":62,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":63,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":71,\"ilOffset\":69,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":71,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_27[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-27\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_27:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_27()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_27:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_27:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-27\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_27()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_27\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":43,\"handlerOffset\":44,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_PropertyNameCaseInsensitive:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_PropertyNameCaseInsensitive:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_PropertyNameCaseInsensitive:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":13,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":14,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":14,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":15,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_PropertyNameCaseInsensitive:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_PropertyNameCaseInsensitive:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_PropertyNameCaseInsensitive:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":25,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":26,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":41,\"ilOffset\":33,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":36,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":42,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":44,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":46,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":47,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":53,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":55,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_28[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-28\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_28:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_28()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_28:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_28:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-28\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_28()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_28\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":41,\"handlerOffset\":42,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"JsonNamingPolicy\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_PropertyNamingPolicy:JsonNamingPolicy()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_PropertyNamingPolicy:JsonNamingPolicy()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_PropertyNamingPolicy:JsonNamingPolicy()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":13,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":14,\"resultType\":\"JsonNamingPolicy\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_PropertyNamingPolicy:JsonNamingPolicy()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_PropertyNamingPolicy:JsonNamingPolicy()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_PropertyNamingPolicy:JsonNamingPolicy()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":39,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":34,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":42,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":45,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":53,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_29[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-29\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_29:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_29()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_29:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_29:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-29\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_29()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_29\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":59,\"handlerOffset\":60,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"JsonCommentHandling\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_ReadCommentHandling:JsonCommentHandling()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_ReadCommentHandling:JsonCommentHandling()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_ReadCommentHandling:JsonCommentHandling()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"callee\":\"System.Text.Json/JsonCommentHandling::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Text.Json/JsonCommentHandling\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":22,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":23,\"resultType\":\"JsonCommentHandling\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_ReadCommentHandling:JsonCommentHandling()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_ReadCommentHandling:JsonCommentHandling()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_ReadCommentHandling:JsonCommentHandling()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":37,\"resultType\":\"System.Int32\",\"callee\":\"System.Text.Json/JsonCommentHandling::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Text.Json/JsonCommentHandling\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":42,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":45,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":47,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":48,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":57,\"ilOffset\":49,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":51,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":52,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":71,\"ilOffset\":58,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":60,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":62,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":63,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":71,\"ilOffset\":69,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":71,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_30[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-30\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_30:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_30()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_30:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_30:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-30\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_30()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_30\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":41,\"handlerOffset\":42,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Text.Json.Serialization.ReferenceHandler\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_ReferenceHandler:System.Text.Json.Serialization.ReferenceHandler()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_ReferenceHandler:System.Text.Json.Serialization.ReferenceHandler()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_ReferenceHandler:System.Text.Json.Serialization.ReferenceHandler()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":13,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":14,\"resultType\":\"System.Text.Json.Serialization.ReferenceHandler\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_ReferenceHandler:System.Text.Json.Serialization.ReferenceHandler()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_ReferenceHandler:System.Text.Json.Serialization.ReferenceHandler()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_ReferenceHandler:System.Text.Json.Serialization.ReferenceHandler()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":39,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":34,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":42,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":45,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":53,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_31[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-31\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_31:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_31()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_31:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_31:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-31\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_31()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_31\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":43,\"handlerOffset\":44,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_RespectNullableAnnotations:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_RespectNullableAnnotations:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_RespectNullableAnnotations:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":13,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":14,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":14,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":15,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_RespectNullableAnnotations:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_RespectNullableAnnotations:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_RespectNullableAnnotations:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":25,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":26,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":41,\"ilOffset\":33,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":36,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":42,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":44,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":46,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":47,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":53,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":55,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_32[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-32\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_32:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_32()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_32:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_32:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-32\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_32()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_32\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":43,\"handlerOffset\":44,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_RespectRequiredConstructorParameters:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_RespectRequiredConstructorParameters:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_RespectRequiredConstructorParameters:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":13,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":14,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":14,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":15,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_RespectRequiredConstructorParameters:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_RespectRequiredConstructorParameters:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_RespectRequiredConstructorParameters:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":25,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":26,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":41,\"ilOffset\":33,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":36,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":42,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":44,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":46,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":47,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":53,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":55,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_33[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-33\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_33:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_33()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_33:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_33:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-33\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_33()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_33\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":39,\"handlerOffset\":40,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"JsonSerializerOptions\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_Strict:JsonSerializerOptions()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_Strict:JsonSerializerOptions()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_Strict:JsonSerializerOptions()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":12,\"resultType\":\"JsonSerializerOptions\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_Strict:JsonSerializerOptions()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_Strict:JsonSerializerOptions()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_Strict:JsonSerializerOptions()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":37,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":32,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":51,\"ilOffset\":38,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":40,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":42,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":43,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":51,\"ilOffset\":49,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":51,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_34[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-34\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_34:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_34()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_34:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_34:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-34\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_34()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_34\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":41,\"handlerOffset\":42,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_TypeInfoResolver:System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_TypeInfoResolver:System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_TypeInfoResolver:System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":13,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":14,\"resultType\":\"System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_TypeInfoResolver:System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_TypeInfoResolver:System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_TypeInfoResolver:System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":39,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":34,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":42,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":45,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":53,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_35[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-35\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_35:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_35()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_35:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_35:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-35\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_35()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_35\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":41,\"handlerOffset\":42,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Collections.Generic.IList\\u003CSystem.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver\\u003E\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_TypeInfoResolverChain:System.Collections.Generic.IList\\u003CSystem.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_TypeInfoResolverChain:System.Collections.Generic.IList\\u003CSystem.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_TypeInfoResolverChain:System.Collections.Generic.IList\\u003CSystem.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver\\u003E()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":13,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":14,\"resultType\":\"System.Collections.Generic.IList\\u003CSystem.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver\\u003E\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_TypeInfoResolverChain:System.Collections.Generic.IList\\u003CSystem.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_TypeInfoResolverChain:System.Collections.Generic.IList\\u003CSystem.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_TypeInfoResolverChain:System.Collections.Generic.IList\\u003CSystem.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver\\u003E()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":39,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":34,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":42,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":45,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":53,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_36[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-36\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_36:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_36()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_36:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_36:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-36\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_36()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_36\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":59,\"handlerOffset\":60,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Text.Json.Serialization.JsonUnknownTypeHandling\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_UnknownTypeHandling:System.Text.Json.Serialization.JsonUnknownTypeHandling()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_UnknownTypeHandling:System.Text.Json.Serialization.JsonUnknownTypeHandling()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_UnknownTypeHandling:System.Text.Json.Serialization.JsonUnknownTypeHandling()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.JsonUnknownTypeHandling::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonUnknownTypeHandling\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":22,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":23,\"resultType\":\"System.Text.Json.Serialization.JsonUnknownTypeHandling\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_UnknownTypeHandling:System.Text.Json.Serialization.JsonUnknownTypeHandling()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_UnknownTypeHandling:System.Text.Json.Serialization.JsonUnknownTypeHandling()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_UnknownTypeHandling:System.Text.Json.Serialization.JsonUnknownTypeHandling()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":37,\"resultType\":\"System.Int32\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.JsonUnknownTypeHandling::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonUnknownTypeHandling\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":42,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":45,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":47,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":48,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":57,\"ilOffset\":49,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":51,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":52,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":71,\"ilOffset\":58,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":60,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":62,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":63,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":71,\"ilOffset\":69,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":71,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_37[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-37\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_37:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_37()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_37:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_37:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-37\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_37()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_37\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":59,\"handlerOffset\":60,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Text.Json.Serialization.JsonUnmappedMemberHandling\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_UnmappedMemberHandling:System.Text.Json.Serialization.JsonUnmappedMemberHandling()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_UnmappedMemberHandling:System.Text.Json.Serialization.JsonUnmappedMemberHandling()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_UnmappedMemberHandling:System.Text.Json.Serialization.JsonUnmappedMemberHandling()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.JsonUnmappedMemberHandling::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonUnmappedMemberHandling\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":22,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":23,\"resultType\":\"System.Text.Json.Serialization.JsonUnmappedMemberHandling\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_UnmappedMemberHandling:System.Text.Json.Serialization.JsonUnmappedMemberHandling()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_UnmappedMemberHandling:System.Text.Json.Serialization.JsonUnmappedMemberHandling()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_UnmappedMemberHandling:System.Text.Json.Serialization.JsonUnmappedMemberHandling()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":37,\"resultType\":\"System.Int32\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.JsonUnmappedMemberHandling::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonUnmappedMemberHandling\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":42,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":45,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":47,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":48,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":57,\"ilOffset\":49,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":51,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":52,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":71,\"ilOffset\":58,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":60,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":62,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":63,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":71,\"ilOffset\":69,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":71,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_38[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-38\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_38:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_38()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_38:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_38:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-38\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_38()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_38\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":39,\"handlerOffset\":40,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"JsonSerializerOptions\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_Web:JsonSerializerOptions()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_Web:JsonSerializerOptions()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_Web:JsonSerializerOptions()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":12,\"resultType\":\"JsonSerializerOptions\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_Web:JsonSerializerOptions()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_Web:JsonSerializerOptions()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_Web:JsonSerializerOptions()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":37,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":32,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":51,\"ilOffset\":38,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":40,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":42,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":43,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":51,\"ilOffset\":49,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":51,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_39[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-39\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_39:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_39()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_39:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_39:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-39\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_39()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_39\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":43,\"handlerOffset\":44,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_WriteIndented:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_WriteIndented:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_WriteIndented:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":13,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":14,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":14,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":15,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/JsonSerializerOptions::get_WriteIndented:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_WriteIndented:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::get_WriteIndented:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":25,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":26,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":41,\"ilOffset\":33,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":36,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":42,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":44,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":46,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":47,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":53,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":55,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_40[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-40\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_40:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_40()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_40:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_40:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-40\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_40()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_40\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":61,\"handlerOffset\":62,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":3,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":8,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":13,\"resultType\":\"System.Text.Json.Serialization.JsonConverter\",\"callee\":\"System.Text.Json/JsonSerializerOptions::GetConverter:System.Text.Json.Serialization.JsonConverter(System.Type)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::GetConverter:System.Text.Json.Serialization.JsonConverter(System.Type)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::GetConverter:System.Text.Json.Serialization.JsonConverter(System.Type)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":23,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":24,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":29,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":34,\"resultType\":\"System.Text.Json.Serialization.JsonConverter\",\"callee\":\"System.Text.Json/JsonSerializerOptions::GetConverter:System.Text.Json.Serialization.JsonConverter(System.Type)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::GetConverter:System.Text.Json.Serialization.JsonConverter(System.Type)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::GetConverter:System.Text.Json.Serialization.JsonConverter(System.Type)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":39,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":46,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":47,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":49,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":50,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":59,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":53,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":54,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":73,\"ilOffset\":60,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":62,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":64,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":65,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":73,\"ilOffset\":71,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":73,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_41[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-41\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_41:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_41()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_41:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_41:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-41\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_41()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_41\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":61,\"handlerOffset\":62,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":3,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":8,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":13,\"resultType\":\"System.Text.Json.Serialization.Metadata.JsonTypeInfo\",\"callee\":\"System.Text.Json/JsonSerializerOptions::GetTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::GetTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::GetTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":23,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":24,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":29,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":34,\"resultType\":\"System.Text.Json.Serialization.Metadata.JsonTypeInfo\",\"callee\":\"System.Text.Json/JsonSerializerOptions::GetTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::GetTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::GetTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":39,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":46,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":47,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":49,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":50,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":59,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":53,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":54,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":73,\"ilOffset\":60,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":62,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":64,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":65,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":73,\"ilOffset\":71,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":73,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_42[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-42\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_42:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_42()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_42:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_42:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-42\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_42()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_42\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":11,\"handlerOffset\":12,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::MakeReadOnly:System.Void()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::MakeReadOnly:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::MakeReadOnly:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":12,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":15,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_43[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-43\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_43:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_43()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_43:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_43:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-43\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_43()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_43\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::MakeReadOnly:System.Void(System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::MakeReadOnly:System.Void(System.Boolean)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::MakeReadOnly:System.Void(System.Boolean)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_44[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-44\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_44:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_44()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_44:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_44:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-44\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_44()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_44\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_AllowDuplicateProperties:System.Void(System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_AllowDuplicateProperties:System.Void(System.Boolean)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_AllowDuplicateProperties:System.Void(System.Boolean)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_45[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-45\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_45:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_45()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_45:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_45:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-45\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_45()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_45\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_AllowOutOfOrderMetadataProperties:System.Void(System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_AllowOutOfOrderMetadataProperties:System.Void(System.Boolean)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_AllowOutOfOrderMetadataProperties:System.Void(System.Boolean)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_46[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-46\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_46:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_46()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_46:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_46:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-46\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_46()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_46\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_AllowTrailingCommas:System.Void(System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_AllowTrailingCommas:System.Void(System.Boolean)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_AllowTrailingCommas:System.Void(System.Boolean)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_47[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-47\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_47:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_47()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_47:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_47:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-47\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_47()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_47\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":13,\"handlerOffset\":14,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":5,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_DefaultBufferSize:System.Void(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_DefaultBufferSize:System.Void(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_DefaultBufferSize:System.Void(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":25,\"ilOffset\":12,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":17,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":25,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":25,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_48[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-48\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_48:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_48()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_48:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_48:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-48\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_48()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_48\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_DefaultIgnoreCondition:System.Void(System.Text.Json.Serialization.JsonIgnoreCondition)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_DefaultIgnoreCondition:System.Void(System.Text.Json.Serialization.JsonIgnoreCondition)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_DefaultIgnoreCondition:System.Void(System.Text.Json.Serialization.JsonIgnoreCondition)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_49[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-49\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_49:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_49()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_49:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_49:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-49\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_49()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_49\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_DictionaryKeyPolicy:System.Void(JsonNamingPolicy)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_DictionaryKeyPolicy:System.Void(JsonNamingPolicy)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_DictionaryKeyPolicy:System.Void(JsonNamingPolicy)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_50[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-50\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_50:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_50()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_50:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_50:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-50\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_50()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_50\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_Encoder:System.Void(JavaScriptEncoder)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_Encoder:System.Void(JavaScriptEncoder)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_Encoder:System.Void(JavaScriptEncoder)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_51[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-51\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_51:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_51()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_51:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_51:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-51\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_51()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_51\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_IgnoreNullValues:System.Void(System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_IgnoreNullValues:System.Void(System.Boolean)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_IgnoreNullValues:System.Void(System.Boolean)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_52[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-52\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_52:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_52()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_52:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_52:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-52\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_52()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_52\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_IgnoreReadOnlyFields:System.Void(System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_IgnoreReadOnlyFields:System.Void(System.Boolean)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_IgnoreReadOnlyFields:System.Void(System.Boolean)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_53[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-53\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_53:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_53()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_53:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_53:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-53\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_53()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_53\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_IgnoreReadOnlyProperties:System.Void(System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_IgnoreReadOnlyProperties:System.Void(System.Boolean)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_IgnoreReadOnlyProperties:System.Void(System.Boolean)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_54[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-54\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_54:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_54()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_54:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_54:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-54\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_54()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_54\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_IncludeFields:System.Void(System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_IncludeFields:System.Void(System.Boolean)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_IncludeFields:System.Void(System.Boolean)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_55[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-55\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_55:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_55()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_55:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_55:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-55\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_55()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_55\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":13,\"handlerOffset\":14,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":65,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":5,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_IndentCharacter:System.Void(System.Char)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_IndentCharacter:System.Void(System.Char)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_IndentCharacter:System.Void(System.Char)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":25,\"ilOffset\":12,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":17,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":25,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":25,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_56[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-56\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_56:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_56()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_56:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_56:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-56\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_56()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_56\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":13,\"handlerOffset\":14,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":5,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_IndentSize:System.Void(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_IndentSize:System.Void(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_IndentSize:System.Void(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":25,\"ilOffset\":12,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":17,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":25,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":25,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_57[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-57\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_57:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_57()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_57:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_57:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-57\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_57()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_57\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":13,\"handlerOffset\":14,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":5,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_MaxDepth:System.Void(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_MaxDepth:System.Void(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_MaxDepth:System.Void(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":25,\"ilOffset\":12,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":17,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":25,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":25,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_58[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-58\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_58:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_58()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_58:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_58:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-58\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_58()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_58\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":16,\"handlerOffset\":17,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":3,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_NewLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_NewLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_NewLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":28,\"ilOffset\":15,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":17,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":20,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":28,\"ilOffset\":26,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":28,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_59[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-59\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_59:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_59()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_59:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_59:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-59\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_59()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_59\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_NumberHandling:System.Void(System.Text.Json.Serialization.JsonNumberHandling)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_NumberHandling:System.Void(System.Text.Json.Serialization.JsonNumberHandling)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_NumberHandling:System.Void(System.Text.Json.Serialization.JsonNumberHandling)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_60[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-60\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_60:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_60()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_60:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_60:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-60\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_60()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_60\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_PreferredObjectCreationHandling:System.Void(System.Text.Json.Serialization.JsonObjectCreationHandling)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_PreferredObjectCreationHandling:System.Void(System.Text.Json.Serialization.JsonObjectCreationHandling)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_PreferredObjectCreationHandling:System.Void(System.Text.Json.Serialization.JsonObjectCreationHandling)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_61[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-61\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_61:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_61()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_61:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_61:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-61\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_61()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_61\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_PropertyNameCaseInsensitive:System.Void(System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_PropertyNameCaseInsensitive:System.Void(System.Boolean)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_PropertyNameCaseInsensitive:System.Void(System.Boolean)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_62[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-62\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_62:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_62()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_62:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_62:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-62\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_62()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_62\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_PropertyNamingPolicy:System.Void(JsonNamingPolicy)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_PropertyNamingPolicy:System.Void(JsonNamingPolicy)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_PropertyNamingPolicy:System.Void(JsonNamingPolicy)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_63[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-63\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_63:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_63()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_63:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_63:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-63\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_63()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_63\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_ReadCommentHandling:System.Void(JsonCommentHandling)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_ReadCommentHandling:System.Void(JsonCommentHandling)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_ReadCommentHandling:System.Void(JsonCommentHandling)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_64[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-64\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_64:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_64()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_64:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_64:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-64\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_64()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_64\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_ReferenceHandler:System.Void(System.Text.Json.Serialization.ReferenceHandler)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_ReferenceHandler:System.Void(System.Text.Json.Serialization.ReferenceHandler)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_ReferenceHandler:System.Void(System.Text.Json.Serialization.ReferenceHandler)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_65[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-65\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_65:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_65()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_65:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_65:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-65\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_65()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_65\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_RespectNullableAnnotations:System.Void(System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_RespectNullableAnnotations:System.Void(System.Boolean)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_RespectNullableAnnotations:System.Void(System.Boolean)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_66[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-66\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_66:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_66()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_66:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_66:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-66\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_66()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_66\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_RespectRequiredConstructorParameters:System.Void(System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_RespectRequiredConstructorParameters:System.Void(System.Boolean)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_RespectRequiredConstructorParameters:System.Void(System.Boolean)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_67[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-67\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_67:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_67()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_67:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_67:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-67\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_67()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_67\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_TypeInfoResolver:System.Void(System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_TypeInfoResolver:System.Void(System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_TypeInfoResolver:System.Void(System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_68[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-68\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_68:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_68()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_68:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_68:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-68\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_68()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_68\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_UnknownTypeHandling:System.Void(System.Text.Json.Serialization.JsonUnknownTypeHandling)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_UnknownTypeHandling:System.Void(System.Text.Json.Serialization.JsonUnknownTypeHandling)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_UnknownTypeHandling:System.Void(System.Text.Json.Serialization.JsonUnknownTypeHandling)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_69[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-69\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_69:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_69()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_69:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_69:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-69\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_69()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_69\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_UnmappedMemberHandling:System.Void(System.Text.Json.Serialization.JsonUnmappedMemberHandling)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_UnmappedMemberHandling:System.Void(System.Text.Json.Serialization.JsonUnmappedMemberHandling)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_UnmappedMemberHandling:System.Void(System.Text.Json.Serialization.JsonUnmappedMemberHandling)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_70[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-70\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_70:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_70()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_70:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_70:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-70\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_70()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_70\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/JsonSerializerOptions::set_WriteIndented:System.Void(System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_WriteIndented:System.Void(System.Boolean)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::set_WriteIndented:System.Void(System.Boolean)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_71[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-71\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_71:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_71()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_71:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_71:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-71\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_71()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_71\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":67,\"handlerOffset\":68,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":3,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":8,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":15,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/JsonSerializerOptions::TryGetTypeInfo:System.Boolean(System.Type,System.Text.Json.Serialization.Metadata.JsonTypeInfo\\u0026)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::TryGetTypeInfo:System.Boolean(System.Type,System.Text.Json.Serialization.Metadata.JsonTypeInfo\\u0026)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::TryGetTypeInfo:System.Boolean(System.Type,System.Text.Json.Serialization.Metadata.JsonTypeInfo\\u0026)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":25,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":26,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":26,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":27,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":32,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":37,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":39,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/JsonSerializerOptions::TryGetTypeInfo:System.Boolean(System.Type,System.Text.Json.Serialization.Metadata.JsonTypeInfo\\u0026)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::TryGetTypeInfo:System.Boolean(System.Type,System.Text.Json.Serialization.Metadata.JsonTypeInfo\\u0026)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::TryGetTypeInfo:System.Boolean(System.Type,System.Text.Json.Serialization.Metadata.JsonTypeInfo\\u0026)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":49,\"ilOffset\":44,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":46,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":50,\"ilOffset\":47,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":49,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":50,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":52,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":53,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":55,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":56,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":65,\"ilOffset\":57,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":59,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":60,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":79,\"ilOffset\":66,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":68,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":70,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":71,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":79,\"ilOffset\":77,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":79,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_72[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-72\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_72:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_72()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_72:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_72:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-72\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_72()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_72\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":10,\"handlerOffset\":11,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":2,\"resultType\":\"System.Text.Json/JsonSerializerOptions\",\"callee\":\"System.Text.Json/JsonSerializerOptions::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":22,\"ilOffset\":9,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":11,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":14,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":22,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":22,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_73[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-73\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_73:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_73()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_73:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_73:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-73\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_73()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_73\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":11,\"handlerOffset\":12,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":3,\"resultType\":\"System.Text.Json/JsonSerializerOptions\",\"callee\":\"System.Text.Json/JsonSerializerOptions::.ctor:System.Void(JsonSerializerDefaults)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::.ctor:System.Void(JsonSerializerDefaults)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":12,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":15,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";
static const char kMethodJson_74[] = "{\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-74\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_74:System.Void()\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_74()\",\"identity\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"definitionSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_74:System.Void()\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::Subject_74:System.Void()\",\"methodId\":\"serializer-options-subjects.serializer-options-subjects.subject-74\",\"signature\":\"System.Void SerializerOptionsSubjects::Subject_74()\"},\"nativeSymbol\":\"SerializerOptionsSubjects_SerializerOptionsSubjects_Subject_74\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":11,\"handlerOffset\":12,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":3,\"resultType\":\"System.Text.Json/JsonSerializerOptions\",\"callee\":\"System.Text.Json/JsonSerializerOptions::.ctor:System.Void(JsonSerializerDefaults)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions::.ctor:System.Void(JsonSerializerDefaults)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonSerializerOptions\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":12,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"ilOffset\":15,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SerializerOptionsSubjects\",\"subjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SerializerOptionsSubjects/SerializerOptionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SerializerOptionsSubjects\"}";

extern "C" const JitMethodEntry kChaosJitMethodEntries[75] =
{
    { kMethodJson_0, 6884u, 0x00000003u, 0u },
    { kMethodJson_1, 6439u, 0x00000004u, 0u },
    { kMethodJson_2, 6469u, 0x00000005u, 0u },
    { kMethodJson_3, 6469u, 0x00000006u, 0u },
    { kMethodJson_4, 6469u, 0x00000007u, 0u },
    { kMethodJson_5, 6469u, 0x00000008u, 0u },
    { kMethodJson_6, 1315u, 0x00000009u, 0u },
    { kMethodJson_7, 6314u, 0x0000000Au, 0u },
    { kMethodJson_8, 6368u, 0x0000000Bu, 0u },
    { kMethodJson_9, 6284u, 0x0000000Cu, 0u },
    { kMethodJson_10, 7250u, 0x0000000Du, 0u },
    { kMethodJson_11, 1415u, 0x0000000Eu, 0u },
    { kMethodJson_12, 5443u, 0x0000000Fu, 0u },
    { kMethodJson_13, 7685u, 0x00000010u, 0u },
    { kMethodJson_14, 6728u, 0x00000011u, 0u },
    { kMethodJson_15, 6664u, 0x00000012u, 0u },
    { kMethodJson_16, 6274u, 0x00000013u, 0u },
    { kMethodJson_17, 6298u, 0x00000014u, 0u },
    { kMethodJson_18, 6322u, 0x00000015u, 0u },
    { kMethodJson_19, 6256u, 0x00000016u, 0u },
    { kMethodJson_20, 5423u, 0x00000017u, 0u },
    { kMethodJson_21, 5401u, 0x00000018u, 0u },
    { kMethodJson_22, 6238u, 0x00000019u, 0u },
    { kMethodJson_23, 5389u, 0x0000001Au, 0u },
    { kMethodJson_24, 6626u, 0x0000001Bu, 0u },
    { kMethodJson_25, 7625u, 0x0000001Cu, 0u },
    { kMethodJson_26, 7823u, 0x0000001Du, 0u },
    { kMethodJson_27, 6340u, 0x0000001Eu, 0u },
    { kMethodJson_28, 6734u, 0x0000001Fu, 0u },
    { kMethodJson_29, 7295u, 0x00000020u, 0u },
    { kMethodJson_30, 6958u, 0x00000021u, 0u },
    { kMethodJson_31, 6334u, 0x00000022u, 0u },
    { kMethodJson_32, 6394u, 0x00000023u, 0u },
    { kMethodJson_33, 6499u, 0x00000024u, 0u },
    { kMethodJson_34, 7070u, 0x00000025u, 0u },
    { kMethodJson_35, 7452u, 0x00000026u, 0u },
    { kMethodJson_36, 7715u, 0x00000027u, 0u },
    { kMethodJson_37, 7769u, 0x00000028u, 0u },
    { kMethodJson_38, 6481u, 0x00000029u, 0u },
    { kMethodJson_39, 6256u, 0x0000002Au, 0u },
    { kMethodJson_40, 9382u, 0x0000002Bu, 0u },
    { kMethodJson_41, 9440u, 0x0000002Cu, 0u },
    { kMethodJson_42, 3280u, 0x0000002Du, 0u },
    { kMethodJson_43, 3424u, 0x0000002Eu, 0u },
    { kMethodJson_44, 3472u, 0x0000002Fu, 0u },
    { kMethodJson_45, 3499u, 0x00000030u, 0u },
    { kMethodJson_46, 3457u, 0x00000031u, 0u },
    { kMethodJson_47, 3446u, 0x00000032u, 0u },
    { kMethodJson_48, 3574u, 0x00000033u, 0u },
    { kMethodJson_49, 3452u, 0x00000034u, 0u },
    { kMethodJson_50, 3419u, 0x00000035u, 0u },
    { kMethodJson_51, 3448u, 0x00000036u, 0u },
    { kMethodJson_52, 3460u, 0x00000037u, 0u },
    { kMethodJson_53, 3472u, 0x00000038u, 0u },
    { kMethodJson_54, 3439u, 0x00000039u, 0u },
    { kMethodJson_55, 3437u, 0x0000003Au, 0u },
    { kMethodJson_56, 3425u, 0x0000003Bu, 0u },
    { kMethodJson_57, 3419u, 0x0000003Cu, 0u },
    { kMethodJson_58, 3424u, 0x0000003Du, 0u },
    { kMethodJson_59, 3547u, 0x0000003Eu, 0u },
    { kMethodJson_60, 3622u, 0x0000003Fu, 0u },
    { kMethodJson_61, 3481u, 0x00000040u, 0u },
    { kMethodJson_62, 3455u, 0x00000041u, 0u },
    { kMethodJson_63, 3472u, 0x00000042u, 0u },
    { kMethodJson_64, 3536u, 0x00000043u, 0u },
    { kMethodJson_65, 3478u, 0x00000044u, 0u },
    { kMethodJson_66, 3508u, 0x00000045u, 0u },
    { kMethodJson_67, 3578u, 0x00000046u, 0u },
    { kMethodJson_68, 3577u, 0x00000047u, 0u },
    { kMethodJson_69, 3595u, 0x00000048u, 0u },
    { kMethodJson_70, 3439u, 0x00000049u, 0u },
    { kMethodJson_71, 9298u, 0x0000004Au, 0u },
    { kMethodJson_72, 3233u, 0x0000004Bu, 0u },
    { kMethodJson_73, 3380u, 0x0000004Cu, 0u },
    { kMethodJson_74, 3380u, 0x0000004Du, 0u }
};

extern "C" const uint32_t kChaosJitMethodEntryCount = 75u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitMethods(kChaosJitMethodEntries, kChaosJitMethodEntryCount);
}