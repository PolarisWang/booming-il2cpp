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

namespace chaos::il2cpp::codegen::MetadataTypeinfoSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects = {{nullptr, nullptr, 3685459336376209594ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(3685459336376209594ULL);
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

struct chaos_type_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects
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
		case static_cast<CHAOS_IL2CPP_INTPTR>(38186906u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_1") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112414887u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_23") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115432263u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_25") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113824741u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_26") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102199116u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_27") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110474875u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_28") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106361522u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_36") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108693237u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_38") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101365279u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_0") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101094355u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_10") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111699152u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_11") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110265759u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_12") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105305410u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_13") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106334649u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_14") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104652708u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_15") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111563411u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_16") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110431014u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_17") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113292221u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_18") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116455592u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_19") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108813079u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_20") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107936299u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_21") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112628924u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_22") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111414485u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_24") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102030199u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_29") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102002820u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_2") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103804669u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_30") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103811886u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_31") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107503045u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_32") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103562540u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_33") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112311515u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_34") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109609674u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_35") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105117953u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_37") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102827559u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_3") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108643174u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_4") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104710879u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_5") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101329936u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_6") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114867193u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_7") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104747138u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_8") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114379787u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_9") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116875292u);
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

CHAOS_IL2CPP_INT32 chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = 0;


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

extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_0(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_1(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_2(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_3(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_4(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_5(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_6(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_7(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_8(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_9(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_10(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_11(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_12(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_13(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_14(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_15(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_16(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_17(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_18(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_19(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_20(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_21(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_22(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_23(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_24(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_25(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_26(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_27(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_28(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_29(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_30(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_31(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_32(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_33(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_34(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_35(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_36(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_37(void);
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_38(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[40] = {
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
	::ChaosAbiMethodEntryV0 entries[39];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		39u,
		0u,
		3060515645u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_0
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_1
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_2
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_3
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_4
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_5
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_6
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_7
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_8
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_9
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_10
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_11
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_12
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_13
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_14
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_15
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_16
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_17
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_18
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_19
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_20
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_21
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_22
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_23
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_24
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_25
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_26
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_27
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_28
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_29
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_30
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_31
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_32
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_33
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_34
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_35
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_36
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_37
		{ 0u, 0u },  // MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_38
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
		"MetadataTypeinfoSubjects",
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
		&chaos_mt_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects.hot,
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
		/* .name_utf8         = */ "MetadataTypeinfoSubjects",
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
		::chaos::il2cpp::runtime_core::RegisterModule("MetadataTypeinfoSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[39] = {
	{ "Subject_0", 0x00000003u, 0u },  // MetadataTypeinfoSubjects
	{ "CustomEntrySubject_1", 0x00000004u, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_2", 0x00000005u, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_3", 0x00000006u, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_4", 0x00000007u, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_5", 0x00000008u, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_6", 0x00000009u, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_7", 0x0000000Au, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_8", 0x0000000Bu, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_9", 0x0000000Cu, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_10", 0x0000000Du, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_11", 0x0000000Eu, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_12", 0x0000000Fu, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_13", 0x00000010u, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_14", 0x00000011u, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_15", 0x00000012u, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_16", 0x00000013u, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_17", 0x00000014u, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_18", 0x00000015u, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_19", 0x00000016u, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_20", 0x00000017u, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_21", 0x00000018u, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_22", 0x00000019u, 0u },  // MetadataTypeinfoSubjects
	{ "CustomEntrySubject_23", 0x0000001Au, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_24", 0x0000001Bu, 0u },  // MetadataTypeinfoSubjects
	{ "CustomEntrySubject_25", 0x0000001Cu, 0u },  // MetadataTypeinfoSubjects
	{ "CustomEntrySubject_26", 0x0000001Du, 0u },  // MetadataTypeinfoSubjects
	{ "CustomEntrySubject_27", 0x0000001Eu, 0u },  // MetadataTypeinfoSubjects
	{ "CustomEntrySubject_28", 0x0000001Fu, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_29", 0x00000020u, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_30", 0x00000021u, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_31", 0x00000022u, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_32", 0x00000023u, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_33", 0x00000024u, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_34", 0x00000025u, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_35", 0x00000026u, 0u },  // MetadataTypeinfoSubjects
	{ "CustomEntrySubject_36", 0x00000027u, 0u },  // MetadataTypeinfoSubjects
	{ "Subject_37", 0x00000028u, 0u },  // MetadataTypeinfoSubjects
	{ "CustomEntrySubject_38", 0x00000029u, 0u },  // MetadataTypeinfoSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "MetadataTypeinfoSubjects", "", 0u, 39u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[39] = {
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
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[39] = {
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_0
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::CustomEntrySubject_1
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_2
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_3
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_4
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_5
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_6
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_7
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_8
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_9
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_10
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_11
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_12
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_13
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_14), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_14
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_15), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_15
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_16), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_16
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_17), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_17
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_18), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_18
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_19), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_19
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_20), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_20
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_21), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_21
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_22), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_22
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_23), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::CustomEntrySubject_23
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_24), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_24
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_25), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::CustomEntrySubject_25
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_26), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::CustomEntrySubject_26
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_27), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::CustomEntrySubject_27
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_28), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::CustomEntrySubject_28
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_29), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_29
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_30), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_30
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_31), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_31
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_32
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_33), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_33
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_34), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_34
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_35), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_35
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_36), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::CustomEntrySubject_36
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_37), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::Subject_37
	{ reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_38), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MetadataTypeinfoSubjects::CustomEntrySubject_38
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"MetadataTypeinfoSubjects",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	39u,
	s_hotpatch_slots,
	39u,
	s_hotpatch_entries,
	39u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[39] = {
	"System.Private.CoreLib/System.Byte",
	"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::CreateJsonPropertyInfo:System.Text.Json.Serialization.Metadata.JsonPropertyInfo(System.Type,System.String)",
	"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()",
	"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::CreateJsonTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type,JsonSerializerOptions)",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_ConstructorAttributeProvider:System.Reflection.ICustomAttributeProvider()",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Converter:System.Text.Json.Serialization.JsonConverter()",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_CreateObject:System.Func<System.Object>()",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_ElementType:System.Type()",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_IsReadOnly:System.Boolean()",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_KeyType:System.Type()",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Kind:System.Text.Json.Serialization.Metadata.JsonTypeInfoKind()",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfoKind::GetHashCode:System.Int32()",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_NumberHandling:System.Nullable<System.Text.Json.Serialization.JsonNumberHandling>()",
	"System.Private.CoreLib/System.Nullable<System.Text.Json.Serialization.JsonNumberHandling>::GetHashCode:System.Int32()",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnDeserialized:System.Action<System.Object>()",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnDeserializing:System.Action<System.Object>()",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnSerialized:System.Action<System.Object>()",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnSerializing:System.Action<System.Object>()",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Options:JsonSerializerOptions()",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OriginatingResolver:System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver()",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_PolymorphismOptions:System.Text.Json.Serialization.Metadata.JsonPolymorphismOptions()",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_PreferredPropertyObjectCreationHandling:System.Nullable<System.Text.Json.Serialization.JsonObjectCreationHandling>()",
	"System.Private.CoreLib/System.Nullable<System.Text.Json.Serialization.JsonObjectCreationHandling>::GetHashCode:System.Int32()",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Properties:System.Collections.Generic.IList<System.Text.Json.Serialization.Metadata.JsonPropertyInfo>()",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Type:System.Type()",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_UnmappedMemberHandling:System.Nullable<System.Text.Json.Serialization.JsonUnmappedMemberHandling>()",
	"System.Private.CoreLib/System.Nullable<System.Text.Json.Serialization.JsonUnmappedMemberHandling>::GetHashCode:System.Int32()",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::MakeReadOnly:System.Void()",
	"System.Private.CoreLib/System.Nullable<System.Text.Json.Serialization.JsonNumberHandling>",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_NumberHandling:System.Void(System.Nullable<System.Text.Json.Serialization.JsonNumberHandling>)",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_OriginatingResolver:System.Void(System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver)",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_PolymorphismOptions:System.Void(System.Text.Json.Serialization.Metadata.JsonPolymorphismOptions)",
	"System.Private.CoreLib/System.Nullable<System.Text.Json.Serialization.JsonObjectCreationHandling>",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_PreferredPropertyObjectCreationHandling:System.Void(System.Nullable<System.Text.Json.Serialization.JsonObjectCreationHandling>)",
	"System.Private.CoreLib/System.Nullable<System.Text.Json.Serialization.JsonUnmappedMemberHandling>",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_UnmappedMemberHandling:System.Void(System.Nullable<System.Text.Json.Serialization.JsonUnmappedMemberHandling>)",
	"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_CreateObject:System.Void(System.Func<System.Object>)",
};

extern "C" void* kChaosExternalRuntimeFnTable[39] = {
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_),
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
};

extern "C" int32_t kChaosExternalRuntimeCount = 39;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[39])() = {
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_1),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_6),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_7),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_8),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_9),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_10),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_11),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_12),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_13),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_14),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_15),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_16),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_17),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_18),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_19),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_20),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_21),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_22),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_23),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_24),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_25),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_26),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_27),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_28),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_29),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_30),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_31),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_32),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_33),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_34),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_35),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_36),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_37),
	reinterpret_cast<void(*)()>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_38),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[39])() = {
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
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 39;
extern "C" const int kSubjectEntryIndices[39] = {
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

	/* ── Entry 0: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry0;
	/* ── Entry 1: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry1;
	/* ── Entry 2: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry2;
	/* ── Entry 3: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry3;
	/* ── Entry 4: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry4;
	/* ── Entry 5: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_6 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry5;
	/* ── Entry 6: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_7 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry6;
	/* ── Entry 7: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_8 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry7;
	/* ── Entry 8: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_9 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry8;
	/* ── Entry 9: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_10 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry9;
	/* ── Entry 10: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_11 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry10;
	/* ── Entry 11: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_12 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry11;
	/* ── Entry 12: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_13 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry12;
	/* ── Entry 13: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_14 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry13;
	/* ── Entry 14: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_15 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry14;
	/* ── Entry 15: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_16 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry15;
	/* ── Entry 16: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_17 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry16;
	/* ── Entry 17: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_18 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry17;
	/* ── Entry 18: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_19 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry18;
	/* ── Entry 19: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_20 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry19;
	/* ── Entry 20: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_21 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry20;
	/* ── Entry 21: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_22 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry21;
	/* ── Entry 22: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_24 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry22;
	/* ── Entry 23: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_29 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry23;
	/* ── Entry 24: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_30 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry24;
	/* ── Entry 25: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_31 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry25;
	/* ── Entry 26: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry26;
	/* ── Entry 27: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_35 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry27;
	/* ── Entry 28: MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_37 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry28;
} kChaosGcSlotMapsSection = {
	/* entry0 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_0 */
	.entry0 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_0),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry1 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_2 */
	.entry1 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_2),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry2 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_3 */
	.entry2 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_3),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry3 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_4 */
	.entry3 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_4),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry4 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_5 */
	.entry4 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_5),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry5 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_6 */
	.entry5 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_6),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry6 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_7 */
	.entry6 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_7),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry7 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_8 */
	.entry7 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_8),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry8 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_9 */
	.entry8 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_9),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry9 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_10 */
	.entry9 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_10),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry10 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_11 */
	.entry10 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_11),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry11 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_12 */
	.entry11 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_12),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry12 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_13 */
	.entry12 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_13),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry13 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_14 */
	.entry13 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_14),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry14 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_15 */
	.entry14 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_15),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry15 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_16 */
	.entry15 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_16),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry16 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_17 */
	.entry16 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_17),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry17 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_18 */
	.entry17 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_18),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry18 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_19 */
	.entry18 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_19),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry19 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_20 */
	.entry19 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_20),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry20 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_21 */
	.entry20 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_21),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry21 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_22 */
	.entry21 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_22),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry22 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_24 */
	.entry22 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_24),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry23 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_29 */
	.entry23 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_29),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry24 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_30 */
	.entry24 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_30),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry25 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_31 */
	.entry25 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_31),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry26 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_32 */
	.entry26 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_32),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry27 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_35 */
	.entry27 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_35),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry28 = MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_37 */
	.entry28 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_37),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 1128u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[39] = {
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_0),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_1),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_2),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_3),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_4),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_5),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_6),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_7),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_8),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_9),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_10),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_11),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_12),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_13),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_14),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_15),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_16),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_17),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_18),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_19),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_20),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_21),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_22),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_23),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_24),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_25),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_26),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_27),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_28),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_29),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_30),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_31),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_32),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_33),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_34),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_35),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_36),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_37),
	reinterpret_cast<void*>(&MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_38),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 39u,
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
	.image_name_utf8    = "MetadataTypeinfoSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects[1] = {
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects[39] = {
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_1:System.Void()", "CustomEntrySubject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_6:System.Void()", "Subject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_7:System.Void()", "Subject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_8:System.Void()", "Subject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_9:System.Void()", "Subject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_10:System.Void()", "Subject_10", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_11:System.Void()", "Subject_11", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_12:System.Void()", "Subject_12", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_13:System.Void()", "Subject_13", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_14:System.Void()", "Subject_14", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_15:System.Void()", "Subject_15", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_16:System.Void()", "Subject_16", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_17:System.Void()", "Subject_17", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_18:System.Void()", "Subject_18", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_19:System.Void()", "Subject_19", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_20:System.Void()", "Subject_20", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_21:System.Void()", "Subject_21", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_22:System.Void()", "Subject_22", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_23:System.Void()", "CustomEntrySubject_23", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_24:System.Void()", "Subject_24", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_25:System.Void()", "CustomEntrySubject_25", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_26:System.Void()", "CustomEntrySubject_26", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_27:System.Void()", "CustomEntrySubject_27", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_28:System.Void()", "CustomEntrySubject_28", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_29:System.Void()", "Subject_29", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_30:System.Void()", "Subject_30", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_31:System.Void()", "Subject_31", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_32:System.Void()", "Subject_32", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_33:System.Void()", "Subject_33", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_34:System.Void()", "Subject_34", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_35:System.Void()", "Subject_35", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_36:System.Void()", "CustomEntrySubject_36", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_37:System.Void()", "Subject_37", "System.Void", 0, nullptr, 0u },
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_38:System.Void()", "CustomEntrySubject_38", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0u, "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects", "MetadataTypeinfoSubjects/MetadataTypeinfoSubjects", "", "MetadataTypeinfoSubjects", "MetadataTypeinfoSubjects", nullptr, kReflFields_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects, 1u, nullptr, 0u,
	kReflMethods_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects, 39u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "MetadataTypeinfoSubjects", kReflTypePtrs, 1u };

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
// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_0()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_0(void)
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


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{{
				_s2 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[2])(chaos_arg_0, chaos_arg_1);
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
			{{
				_s4 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_1 = _s4;
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[2])(chaos_arg_0, chaos_arg_1);
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_1()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_2()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_2(void)
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
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = 0;
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[5])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[5])(chaos_arg_0, chaos_arg_1);
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_3()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_3(void)
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[6])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[6])();
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_4()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_4(void)
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[7])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[7])();
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_5()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_5(void)
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[8])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[8])();
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_6()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_6(void)
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[9])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[9])();
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_7()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_7(void)
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_8()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_8(void)
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[11])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[11])();
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_9()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_9(void)
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[12])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s1;
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[13])();
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s3 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[12])();
				_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s4;
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[13])();
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_10()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_10(void)
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_11()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_11(void)
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_12()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_12(void)
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_13()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_13(void)
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[18])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[18])();
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_14()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_14(void)
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[19])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[19])();
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_15()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_15(void)
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[20])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[20])();
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_16()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_16(void)
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[21])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[21])();
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_17()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_17(void)
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[22])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[22])();
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_18()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_18(void)
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[23])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s1;
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[24])();
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s3 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[23])();
				_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s4;
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[24])();
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_19()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_19(void)
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[25])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[25])();
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_20()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_20(void)
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
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[26])();
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_21()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_21(void)
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_22()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_22(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[29])();
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_23()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_23(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_24()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_24(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s1;
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
			_s1 = chaos_locals[0];
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[31])(chaos_arg_0);
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_25()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_25(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_26()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_26(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_27()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_27(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_28()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_28(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_29()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_29(void)
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
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[32])(chaos_arg_0);
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_30()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_30(void)
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
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[33])(chaos_arg_0);
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_31()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_31(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s1;
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
			_s1 = chaos_locals[0];
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[35])(chaos_arg_0);
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_32()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_32(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s1;
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
			_s1 = chaos_locals[0];
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[37])(chaos_arg_0);
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_33()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_33(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_34()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_34(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_35()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_35(void)
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[8])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[8])();
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_36()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_36(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_37()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_37(void)
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
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[38])(chaos_arg_0);
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
				chaos_static_MetadataTypeinfoSubjects_MetadataTypeinfoSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_38()
extern "C" void MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_38(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}



}  // namespace chaos::il2cpp::codegen::MetadataTypeinfoSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 39;

// ── JIT Method Entry Table ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kMethodJson_0[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-0\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_0:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_0()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_0:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_0:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-0\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_0()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":71,\"handlerOffset\":72,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":3,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":8,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":13,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":18,\"resultType\":\"System.Text.Json.Serialization.Metadata.JsonPropertyInfo\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::CreateJsonPropertyInfo:System.Text.Json.Serialization.Metadata.JsonPropertyInfo(System.Type,System.String)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::CreateJsonPropertyInfo:System.Text.Json.Serialization.Metadata.JsonPropertyInfo(System.Type,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::CreateJsonPropertyInfo:System.Text.Json.Serialization.Metadata.JsonPropertyInfo(System.Type,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":28,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":29,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":34,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":39,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":44,\"resultType\":\"System.Text.Json.Serialization.Metadata.JsonPropertyInfo\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::CreateJsonPropertyInfo:System.Text.Json.Serialization.Metadata.JsonPropertyInfo(System.Type,System.String)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::CreateJsonPropertyInfo:System.Text.Json.Serialization.Metadata.JsonPropertyInfo(System.Type,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::CreateJsonPropertyInfo:System.Text.Json.Serialization.Metadata.JsonPropertyInfo(System.Type,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":49,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":54,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":56,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":57,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":59,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":60,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":69,\"ilOffset\":61,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":63,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":64,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":83,\"ilOffset\":70,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":72,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":74,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":75,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":83,\"ilOffset\":81,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":83,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_1[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.custom-entry-subject-1\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_1:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::CustomEntrySubject_1()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_1:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_1:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.custom-entry-subject-1\",\"signature\":\"System.Void MetadataTypeinfoSubjects::CustomEntrySubject_1()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_2[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-2\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_2:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_2()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_2:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_2:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-2\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_2()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_2\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":61,\"handlerOffset\":62,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":2,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":12,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"System.Text.Json.Serialization.Metadata.JsonTypeInfo\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::CreateJsonTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type,JsonSerializerOptions)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::CreateJsonTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type,JsonSerializerOptions)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::CreateJsonTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type,JsonSerializerOptions)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":23,\"resultType\":\"System.RuntimeTypeHandle\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":28,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":33,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":34,\"resultType\":\"System.Text.Json.Serialization.Metadata.JsonTypeInfo\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::CreateJsonTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type,JsonSerializerOptions)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::CreateJsonTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type,JsonSerializerOptions)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::CreateJsonTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type,JsonSerializerOptions)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":39,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":46,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":47,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":49,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":50,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":59,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":53,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":54,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":73,\"ilOffset\":60,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":62,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":64,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":65,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":73,\"ilOffset\":71,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":73,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_3[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-3\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_3:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_3()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_3:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_3:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-3\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_3()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_3\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":41,\"handlerOffset\":42,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Reflection.ICustomAttributeProvider\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_ConstructorAttributeProvider:System.Reflection.ICustomAttributeProvider()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_ConstructorAttributeProvider:System.Reflection.ICustomAttributeProvider()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_ConstructorAttributeProvider:System.Reflection.ICustomAttributeProvider()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":13,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":14,\"resultType\":\"System.Reflection.ICustomAttributeProvider\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_ConstructorAttributeProvider:System.Reflection.ICustomAttributeProvider()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_ConstructorAttributeProvider:System.Reflection.ICustomAttributeProvider()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_ConstructorAttributeProvider:System.Reflection.ICustomAttributeProvider()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":39,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":34,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":42,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":45,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":53,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_4[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-4\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_4:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_4()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_4:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_4:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-4\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_4()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_4\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":41,\"handlerOffset\":42,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Text.Json.Serialization.JsonConverter\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Converter:System.Text.Json.Serialization.JsonConverter()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Converter:System.Text.Json.Serialization.JsonConverter()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Converter:System.Text.Json.Serialization.JsonConverter()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":13,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":14,\"resultType\":\"System.Text.Json.Serialization.JsonConverter\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Converter:System.Text.Json.Serialization.JsonConverter()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Converter:System.Text.Json.Serialization.JsonConverter()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Converter:System.Text.Json.Serialization.JsonConverter()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":39,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":34,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":42,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":45,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":53,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_5[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-5\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_5:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_5()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_5:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_5:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-5\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_5()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_5\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":41,\"handlerOffset\":42,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Func\\u003CSystem.Object\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_CreateObject:System.Func\\u003CSystem.Object\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_CreateObject:System.Func\\u003CSystem.Object\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_CreateObject:System.Func\\u003CSystem.Object\\u003E()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":13,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":14,\"resultType\":\"System.Func\\u003CSystem.Object\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_CreateObject:System.Func\\u003CSystem.Object\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_CreateObject:System.Func\\u003CSystem.Object\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_CreateObject:System.Func\\u003CSystem.Object\\u003E()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":39,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":34,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":42,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":45,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":53,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_6[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-6\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_6:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_6()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_6:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_6:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-6\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_6()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_6\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":41,\"handlerOffset\":42,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Type\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_ElementType:System.Type()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_ElementType:System.Type()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_ElementType:System.Type()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":13,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":14,\"resultType\":\"System.Type\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_ElementType:System.Type()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_ElementType:System.Type()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_ElementType:System.Type()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":39,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":34,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":42,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":45,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":53,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_7[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-7\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_7:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_7()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_7:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_7:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-7\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_7()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_7\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":43,\"handlerOffset\":44,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_IsReadOnly:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_IsReadOnly:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_IsReadOnly:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":13,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":14,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":14,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":15,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_IsReadOnly:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_IsReadOnly:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_IsReadOnly:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":25,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":26,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":41,\"ilOffset\":33,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":36,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":42,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":44,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":46,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":47,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":53,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":55,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_8[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-8\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_8:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_8()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_8:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_8:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-8\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_8()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_8\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":41,\"handlerOffset\":42,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Type\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_KeyType:System.Type()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_KeyType:System.Type()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_KeyType:System.Type()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":13,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":14,\"resultType\":\"System.Type\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_KeyType:System.Type()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_KeyType:System.Type()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_KeyType:System.Type()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":39,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":34,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":42,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":45,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":53,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_9[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-9\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_9:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_9()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_9:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_9:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-9\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_9()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_9\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":59,\"handlerOffset\":60,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Text.Json.Serialization.Metadata.JsonTypeInfoKind\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Kind:System.Text.Json.Serialization.Metadata.JsonTypeInfoKind()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Kind:System.Text.Json.Serialization.Metadata.JsonTypeInfoKind()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Kind:System.Text.Json.Serialization.Metadata.JsonTypeInfoKind()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfoKind::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfoKind\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":22,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":23,\"resultType\":\"System.Text.Json.Serialization.Metadata.JsonTypeInfoKind\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Kind:System.Text.Json.Serialization.Metadata.JsonTypeInfoKind()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Kind:System.Text.Json.Serialization.Metadata.JsonTypeInfoKind()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Kind:System.Text.Json.Serialization.Metadata.JsonTypeInfoKind()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":37,\"resultType\":\"System.Int32\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfoKind::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfoKind\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":42,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":45,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":47,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":48,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":57,\"ilOffset\":49,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":51,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":52,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":71,\"ilOffset\":58,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":60,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":62,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":63,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":71,\"ilOffset\":69,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":71,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_10[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-10\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_10:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_10()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_10:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_10:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-10\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_10()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_10\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":59,\"handlerOffset\":60,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Nullable\\u003CSystem.Text.Json.Serialization.JsonNumberHandling\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_NumberHandling:System.Nullable\\u003CSystem.Text.Json.Serialization.JsonNumberHandling\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_NumberHandling:System.Nullable\\u003CSystem.Text.Json.Serialization.JsonNumberHandling\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_NumberHandling:System.Nullable\\u003CSystem.Text.Json.Serialization.JsonNumberHandling\\u003E()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Serialization.JsonNumberHandling\\u003E::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Serialization.JsonNumberHandling\\u003E\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":22,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":23,\"resultType\":\"System.Nullable\\u003CSystem.Text.Json.Serialization.JsonNumberHandling\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_NumberHandling:System.Nullable\\u003CSystem.Text.Json.Serialization.JsonNumberHandling\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_NumberHandling:System.Nullable\\u003CSystem.Text.Json.Serialization.JsonNumberHandling\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_NumberHandling:System.Nullable\\u003CSystem.Text.Json.Serialization.JsonNumberHandling\\u003E()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":37,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Serialization.JsonNumberHandling\\u003E::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Serialization.JsonNumberHandling\\u003E\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":42,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":45,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":47,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":48,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":57,\"ilOffset\":49,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":51,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":52,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":71,\"ilOffset\":58,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":60,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":62,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":63,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":71,\"ilOffset\":69,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":71,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_11[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-11\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_11:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_11()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_11:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_11:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-11\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_11()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_11\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":41,\"handlerOffset\":42,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Action\\u003CSystem.Object\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnDeserialized:System.Action\\u003CSystem.Object\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnDeserialized:System.Action\\u003CSystem.Object\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnDeserialized:System.Action\\u003CSystem.Object\\u003E()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":13,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":14,\"resultType\":\"System.Action\\u003CSystem.Object\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnDeserialized:System.Action\\u003CSystem.Object\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnDeserialized:System.Action\\u003CSystem.Object\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnDeserialized:System.Action\\u003CSystem.Object\\u003E()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":39,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":34,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":42,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":45,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":53,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_12[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-12\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_12:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_12()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_12:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_12:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-12\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_12()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_12\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":41,\"handlerOffset\":42,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Action\\u003CSystem.Object\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnDeserializing:System.Action\\u003CSystem.Object\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnDeserializing:System.Action\\u003CSystem.Object\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnDeserializing:System.Action\\u003CSystem.Object\\u003E()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":13,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":14,\"resultType\":\"System.Action\\u003CSystem.Object\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnDeserializing:System.Action\\u003CSystem.Object\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnDeserializing:System.Action\\u003CSystem.Object\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnDeserializing:System.Action\\u003CSystem.Object\\u003E()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":39,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":34,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":42,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":45,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":53,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_13[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-13\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_13:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_13()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_13:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_13:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-13\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_13()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_13\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":41,\"handlerOffset\":42,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Action\\u003CSystem.Object\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnSerialized:System.Action\\u003CSystem.Object\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnSerialized:System.Action\\u003CSystem.Object\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnSerialized:System.Action\\u003CSystem.Object\\u003E()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":13,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":14,\"resultType\":\"System.Action\\u003CSystem.Object\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnSerialized:System.Action\\u003CSystem.Object\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnSerialized:System.Action\\u003CSystem.Object\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnSerialized:System.Action\\u003CSystem.Object\\u003E()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":39,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":34,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":42,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":45,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":53,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_14[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-14\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_14:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_14()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_14:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_14:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-14\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_14()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_14\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":41,\"handlerOffset\":42,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Action\\u003CSystem.Object\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnSerializing:System.Action\\u003CSystem.Object\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnSerializing:System.Action\\u003CSystem.Object\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnSerializing:System.Action\\u003CSystem.Object\\u003E()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":13,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":14,\"resultType\":\"System.Action\\u003CSystem.Object\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnSerializing:System.Action\\u003CSystem.Object\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnSerializing:System.Action\\u003CSystem.Object\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OnSerializing:System.Action\\u003CSystem.Object\\u003E()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":39,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":34,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":42,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":45,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":53,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_15[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-15\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_15:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_15()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_15:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_15:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-15\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_15()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_15\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":41,\"handlerOffset\":42,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"JsonSerializerOptions\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Options:JsonSerializerOptions()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Options:JsonSerializerOptions()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Options:JsonSerializerOptions()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":13,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":14,\"resultType\":\"JsonSerializerOptions\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Options:JsonSerializerOptions()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Options:JsonSerializerOptions()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Options:JsonSerializerOptions()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":39,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":34,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":42,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":45,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":53,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_16[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-16\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_16:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_16()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_16:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_16:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-16\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_16()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_16\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":41,\"handlerOffset\":42,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OriginatingResolver:System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OriginatingResolver:System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OriginatingResolver:System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":13,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":14,\"resultType\":\"System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OriginatingResolver:System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OriginatingResolver:System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_OriginatingResolver:System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":39,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":34,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":42,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":45,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":53,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_17[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-17\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_17:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_17()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_17:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_17:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-17\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_17()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_17\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":41,\"handlerOffset\":42,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Text.Json.Serialization.Metadata.JsonPolymorphismOptions\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_PolymorphismOptions:System.Text.Json.Serialization.Metadata.JsonPolymorphismOptions()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_PolymorphismOptions:System.Text.Json.Serialization.Metadata.JsonPolymorphismOptions()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_PolymorphismOptions:System.Text.Json.Serialization.Metadata.JsonPolymorphismOptions()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":13,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":14,\"resultType\":\"System.Text.Json.Serialization.Metadata.JsonPolymorphismOptions\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_PolymorphismOptions:System.Text.Json.Serialization.Metadata.JsonPolymorphismOptions()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_PolymorphismOptions:System.Text.Json.Serialization.Metadata.JsonPolymorphismOptions()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_PolymorphismOptions:System.Text.Json.Serialization.Metadata.JsonPolymorphismOptions()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":39,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":34,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":42,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":45,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":53,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_18[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-18\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_18:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_18()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_18:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_18:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-18\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_18()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_18\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":59,\"handlerOffset\":60,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Nullable\\u003CSystem.Text.Json.Serialization.JsonObjectCreationHandling\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_PreferredPropertyObjectCreationHandling:System.Nullable\\u003CSystem.Text.Json.Serialization.JsonObjectCreationHandling\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_PreferredPropertyObjectCreationHandling:System.Nullable\\u003CSystem.Text.Json.Serialization.JsonObjectCreationHandling\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_PreferredPropertyObjectCreationHandling:System.Nullable\\u003CSystem.Text.Json.Serialization.JsonObjectCreationHandling\\u003E()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Serialization.JsonObjectCreationHandling\\u003E::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Serialization.JsonObjectCreationHandling\\u003E\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":22,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":23,\"resultType\":\"System.Nullable\\u003CSystem.Text.Json.Serialization.JsonObjectCreationHandling\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_PreferredPropertyObjectCreationHandling:System.Nullable\\u003CSystem.Text.Json.Serialization.JsonObjectCreationHandling\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_PreferredPropertyObjectCreationHandling:System.Nullable\\u003CSystem.Text.Json.Serialization.JsonObjectCreationHandling\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_PreferredPropertyObjectCreationHandling:System.Nullable\\u003CSystem.Text.Json.Serialization.JsonObjectCreationHandling\\u003E()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":37,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Serialization.JsonObjectCreationHandling\\u003E::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Serialization.JsonObjectCreationHandling\\u003E\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":42,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":45,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":47,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":48,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":57,\"ilOffset\":49,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":51,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":52,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":71,\"ilOffset\":58,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":60,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":62,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":63,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":71,\"ilOffset\":69,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":71,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_19[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-19\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_19:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_19()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_19:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_19:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-19\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_19()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_19\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":41,\"handlerOffset\":42,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Collections.Generic.IList\\u003CSystem.Text.Json.Serialization.Metadata.JsonPropertyInfo\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Properties:System.Collections.Generic.IList\\u003CSystem.Text.Json.Serialization.Metadata.JsonPropertyInfo\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Properties:System.Collections.Generic.IList\\u003CSystem.Text.Json.Serialization.Metadata.JsonPropertyInfo\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Properties:System.Collections.Generic.IList\\u003CSystem.Text.Json.Serialization.Metadata.JsonPropertyInfo\\u003E()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":13,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":14,\"resultType\":\"System.Collections.Generic.IList\\u003CSystem.Text.Json.Serialization.Metadata.JsonPropertyInfo\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Properties:System.Collections.Generic.IList\\u003CSystem.Text.Json.Serialization.Metadata.JsonPropertyInfo\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Properties:System.Collections.Generic.IList\\u003CSystem.Text.Json.Serialization.Metadata.JsonPropertyInfo\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Properties:System.Collections.Generic.IList\\u003CSystem.Text.Json.Serialization.Metadata.JsonPropertyInfo\\u003E()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":39,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":34,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":42,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":45,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":53,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_20[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-20\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_20:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_20()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_20:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_20:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-20\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_20()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_20\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":41,\"handlerOffset\":42,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Type\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Type:System.Type()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Type:System.Type()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Type:System.Type()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":13,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":14,\"resultType\":\"System.Type\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Type:System.Type()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Type:System.Type()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Type:System.Type()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":39,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":34,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":42,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":45,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":53,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_21[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-21\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_21:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_21()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_21:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_21:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-21\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_21()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_21\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":59,\"handlerOffset\":60,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Nullable\\u003CSystem.Text.Json.Serialization.JsonUnmappedMemberHandling\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_UnmappedMemberHandling:System.Nullable\\u003CSystem.Text.Json.Serialization.JsonUnmappedMemberHandling\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_UnmappedMemberHandling:System.Nullable\\u003CSystem.Text.Json.Serialization.JsonUnmappedMemberHandling\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_UnmappedMemberHandling:System.Nullable\\u003CSystem.Text.Json.Serialization.JsonUnmappedMemberHandling\\u003E()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Serialization.JsonUnmappedMemberHandling\\u003E::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Serialization.JsonUnmappedMemberHandling\\u003E\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":22,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":23,\"resultType\":\"System.Nullable\\u003CSystem.Text.Json.Serialization.JsonUnmappedMemberHandling\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_UnmappedMemberHandling:System.Nullable\\u003CSystem.Text.Json.Serialization.JsonUnmappedMemberHandling\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_UnmappedMemberHandling:System.Nullable\\u003CSystem.Text.Json.Serialization.JsonUnmappedMemberHandling\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_UnmappedMemberHandling:System.Nullable\\u003CSystem.Text.Json.Serialization.JsonUnmappedMemberHandling\\u003E()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":37,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Serialization.JsonUnmappedMemberHandling\\u003E::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Serialization.JsonUnmappedMemberHandling\\u003E\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":42,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":45,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":47,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":48,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":57,\"ilOffset\":49,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":51,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":52,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":71,\"ilOffset\":58,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":60,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":62,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":63,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":71,\"ilOffset\":69,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":71,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_22[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-22\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_22:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_22()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_22:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_22:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-22\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_22()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_22\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":11,\"handlerOffset\":12,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::MakeReadOnly:System.Void()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::MakeReadOnly:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::MakeReadOnly:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":12,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":15,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_23[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.custom-entry-subject-23\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_23:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::CustomEntrySubject_23()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_23:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_23:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.custom-entry-subject-23\",\"signature\":\"System.Void MetadataTypeinfoSubjects::CustomEntrySubject_23()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_23\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_24[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-24\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_24:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_24()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_24:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_24:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-24\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_24()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_24\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":20,\"handlerOffset\":21,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":3,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Serialization.JsonNumberHandling\\u003E\",\"ilOffset\":5,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Serialization.JsonNumberHandling\\u003E\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Serialization.JsonNumberHandling\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":11,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":12,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_NumberHandling:System.Void(System.Nullable\\u003CSystem.Text.Json.Serialization.JsonNumberHandling\\u003E)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_NumberHandling:System.Void(System.Nullable\\u003CSystem.Text.Json.Serialization.JsonNumberHandling\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_NumberHandling:System.Void(System.Nullable\\u003CSystem.Text.Json.Serialization.JsonNumberHandling\\u003E)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":32,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":21,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":24,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":32,\"ilOffset\":30,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":32,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_25[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.custom-entry-subject-25\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_25:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::CustomEntrySubject_25()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_25:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_25:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.custom-entry-subject-25\",\"signature\":\"System.Void MetadataTypeinfoSubjects::CustomEntrySubject_25()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_25\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_26[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.custom-entry-subject-26\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_26:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::CustomEntrySubject_26()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_26:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_26:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.custom-entry-subject-26\",\"signature\":\"System.Void MetadataTypeinfoSubjects::CustomEntrySubject_26()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_26\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_27[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.custom-entry-subject-27\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_27:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::CustomEntrySubject_27()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_27:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_27:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.custom-entry-subject-27\",\"signature\":\"System.Void MetadataTypeinfoSubjects::CustomEntrySubject_27()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_27\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_28[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.custom-entry-subject-28\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_28:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::CustomEntrySubject_28()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_28:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_28:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.custom-entry-subject-28\",\"signature\":\"System.Void MetadataTypeinfoSubjects::CustomEntrySubject_28()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_28\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_29[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-29\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_29:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_29()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_29:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_29:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-29\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_29()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_29\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_OriginatingResolver:System.Void(System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_OriginatingResolver:System.Void(System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_OriginatingResolver:System.Void(System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_30[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-30\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_30:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_30()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_30:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_30:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-30\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_30()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_30\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_PolymorphismOptions:System.Void(System.Text.Json.Serialization.Metadata.JsonPolymorphismOptions)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_PolymorphismOptions:System.Void(System.Text.Json.Serialization.Metadata.JsonPolymorphismOptions)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_PolymorphismOptions:System.Void(System.Text.Json.Serialization.Metadata.JsonPolymorphismOptions)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_31[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-31\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_31:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_31()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_31:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_31:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-31\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_31()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_31\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":20,\"handlerOffset\":21,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":3,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Serialization.JsonObjectCreationHandling\\u003E\",\"ilOffset\":5,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Serialization.JsonObjectCreationHandling\\u003E\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Serialization.JsonObjectCreationHandling\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":11,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":12,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_PreferredPropertyObjectCreationHandling:System.Void(System.Nullable\\u003CSystem.Text.Json.Serialization.JsonObjectCreationHandling\\u003E)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_PreferredPropertyObjectCreationHandling:System.Void(System.Nullable\\u003CSystem.Text.Json.Serialization.JsonObjectCreationHandling\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_PreferredPropertyObjectCreationHandling:System.Void(System.Nullable\\u003CSystem.Text.Json.Serialization.JsonObjectCreationHandling\\u003E)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":32,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":21,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":24,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":32,\"ilOffset\":30,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":32,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_32[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-32\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_32:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_32()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_32:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_32:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-32\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_32()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_32\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":20,\"handlerOffset\":21,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":3,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Serialization.JsonUnmappedMemberHandling\\u003E\",\"ilOffset\":5,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Serialization.JsonUnmappedMemberHandling\\u003E\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Serialization.JsonUnmappedMemberHandling\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":11,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":12,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_UnmappedMemberHandling:System.Void(System.Nullable\\u003CSystem.Text.Json.Serialization.JsonUnmappedMemberHandling\\u003E)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_UnmappedMemberHandling:System.Void(System.Nullable\\u003CSystem.Text.Json.Serialization.JsonUnmappedMemberHandling\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_UnmappedMemberHandling:System.Void(System.Nullable\\u003CSystem.Text.Json.Serialization.JsonUnmappedMemberHandling\\u003E)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":32,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":21,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":24,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":32,\"ilOffset\":30,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":32,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_33[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-33\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_33:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_33()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_33:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_33:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-33\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_33()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_33\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_34[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-34\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_34:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_34()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_34:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_34:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-34\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_34()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_34\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_35[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-35\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_35:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_35()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_35:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_35:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-35\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_35()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_35\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":41,\"handlerOffset\":42,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Func\\u003CSystem.Object\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_CreateObject:System.Func\\u003CSystem.Object\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_CreateObject:System.Func\\u003CSystem.Object\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_CreateObject:System.Func\\u003CSystem.Object\\u003E()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":13,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":14,\"resultType\":\"System.Func\\u003CSystem.Object\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_CreateObject:System.Func\\u003CSystem.Object\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_CreateObject:System.Func\\u003CSystem.Object\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_CreateObject:System.Func\\u003CSystem.Object\\u003E()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":39,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":34,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":42,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":45,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":53,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":53,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_36[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.custom-entry-subject-36\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_36:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::CustomEntrySubject_36()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_36:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_36:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.custom-entry-subject-36\",\"signature\":\"System.Void MetadataTypeinfoSubjects::CustomEntrySubject_36()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_36\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_37[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-37\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_37:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_37()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_37:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::Subject_37:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.subject-37\",\"signature\":\"System.Void MetadataTypeinfoSubjects::Subject_37()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_Subject_37\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_CreateObject:System.Void(System.Func\\u003CSystem.Object\\u003E)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_CreateObject:System.Void(System.Func\\u003CSystem.Object\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_CreateObject:System.Void(System.Func\\u003CSystem.Object\\u003E)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MetadataTypeinfoSubjects\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";
static const char kMethodJson_38[] = "{\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.custom-entry-subject-38\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_38:System.Void()\",\"signature\":\"System.Void MetadataTypeinfoSubjects::CustomEntrySubject_38()\",\"identity\":{\"assemblyName\":\"MetadataTypeinfoSubjects\",\"declaringTypeSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects\",\"definitionSubjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_38:System.Void()\",\"subjectId\":\"MetadataTypeinfoSubjects/MetadataTypeinfoSubjects::CustomEntrySubject_38:System.Void()\",\"methodId\":\"metadata-typeinfo-subjects.metadata-typeinfo-subjects.custom-entry-subject-38\",\"signature\":\"System.Void MetadataTypeinfoSubjects::CustomEntrySubject_38()\"},\"nativeSymbol\":\"MetadataTypeinfoSubjects_MetadataTypeinfoSubjects_CustomEntrySubject_38\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MetadataTypeinfoSubjects\"}";

extern "C" const JitMethodEntry kChaosJitMethodEntries[39] =
{
    { kMethodJson_0, 9980u, 0x00000003u, 0u },
    { kMethodJson_1, 1389u, 0x00000004u, 0u },
    { kMethodJson_2, 9747u, 0x00000005u, 0u },
    { kMethodJson_3, 7130u, 0x00000006u, 0u },
    { kMethodJson_4, 7032u, 0x00000007u, 0u },
    { kMethodJson_5, 6986u, 0x00000008u, 0u },
    { kMethodJson_6, 6780u, 0x00000009u, 0u },
    { kMethodJson_7, 6378u, 0x0000000Au, 0u },
    { kMethodJson_8, 6756u, 0x0000000Bu, 0u },
    { kMethodJson_9, 7789u, 0x0000000Cu, 0u },
    { kMethodJson_10, 8121u, 0x0000000Du, 0u },
    { kMethodJson_11, 7022u, 0x0000000Eu, 0u },
    { kMethodJson_12, 7028u, 0x0000000Fu, 0u },
    { kMethodJson_13, 7010u, 0x00000010u, 0u },
    { kMethodJson_14, 7016u, 0x00000011u, 0u },
    { kMethodJson_15, 6844u, 0x00000012u, 0u },
    { kMethodJson_16, 7236u, 0x00000013u, 0u },
    { kMethodJson_17, 7252u, 0x00000014u, 0u },
    { kMethodJson_18, 8367u, 0x00000015u, 0u },
    { kMethodJson_19, 7494u, 0x00000016u, 0u },
    { kMethodJson_20, 6746u, 0x00000017u, 0u },
    { kMethodJson_21, 8265u, 0x00000018u, 0u },
    { kMethodJson_22, 3345u, 0x00000019u, 0u },
    { kMethodJson_23, 1397u, 0x0000001Au, 0u },
    { kMethodJson_24, 4494u, 0x0000001Bu, 0u },
    { kMethodJson_25, 1397u, 0x0000001Cu, 0u },
    { kMethodJson_26, 1397u, 0x0000001Du, 0u },
    { kMethodJson_27, 1397u, 0x0000001Eu, 0u },
    { kMethodJson_28, 1397u, 0x0000001Fu, 0u },
    { kMethodJson_29, 3652u, 0x00000020u, 0u },
    { kMethodJson_30, 3658u, 0x00000021u, 0u },
    { kMethodJson_31, 4617u, 0x00000022u, 0u },
    { kMethodJson_32, 4566u, 0x00000023u, 0u },
    { kMethodJson_33, 1305u, 0x00000024u, 0u },
    { kMethodJson_34, 1305u, 0x00000025u, 0u },
    { kMethodJson_35, 6994u, 0x00000026u, 0u },
    { kMethodJson_36, 1397u, 0x00000027u, 0u },
    { kMethodJson_37, 3556u, 0x00000028u, 0u },
    { kMethodJson_38, 1397u, 0x00000029u, 0u }
};

extern "C" const uint32_t kChaosJitMethodEntryCount = 39u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitMethods(kChaosJitMethodEntries, kChaosJitMethodEntryCount);
}