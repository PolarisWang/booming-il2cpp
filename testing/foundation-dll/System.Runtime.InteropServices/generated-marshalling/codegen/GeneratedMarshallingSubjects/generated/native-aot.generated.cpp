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

namespace chaos::il2cpp::codegen::GeneratedMarshallingSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects = {{nullptr, nullptr, 4583132895785458666ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(4583132895785458666ULL);
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
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Runtime_InteropServices_Marshalling_CustomMarshallerAttribute = {{nullptr, nullptr, 5467339372344573430ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Runtime_InteropServices_Marshalling_CustomMarshallerAttribute = static_cast<CHAOS_IL2CPP_INTPTR>(5467339372344573430ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_String = {{nullptr, nullptr, 1782325859292956794ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_INTPTR>(1782325859292956794ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Type = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, nullptr, 3222174513575444759ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Type = static_cast<CHAOS_IL2CPP_INTPTR>(3222174513575444759ULL);
inline TypeInfoV0 chaos_mt_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComClassAttribute = {{nullptr, nullptr, 7897236160915927842ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComClassAttribute = static_cast<CHAOS_IL2CPP_INTPTR>(7897236160915927842ULL);
inline TypeInfoV0 chaos_mt_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComInterfaceAttribute = {{nullptr, nullptr, 13596162198449115047ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComInterfaceAttribute = static_cast<CHAOS_IL2CPP_INTPTR>(13596162198449115047ULL);
inline TypeInfoV0 chaos_mt_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers = {{nullptr, nullptr, 593620431420093221ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers = static_cast<CHAOS_IL2CPP_INTPTR>(593620431420093221ULL);

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

struct chaos_type_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects
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

struct chaos_type_System_Private_CoreLib_System_Runtime_InteropServices_Marshalling_CustomMarshallerAttribute
{
	ThinLockableHeader header{};
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

struct chaos_type_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComClassAttribute
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComInterfaceAttribute
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers
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
		case static_cast<CHAOS_IL2CPP_INTPTR>(47085066u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_0") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114199043u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_10") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114276800u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_11") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116472143u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_12") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115626802u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_13") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108693929u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_14") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103647636u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_15") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109982211u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_16") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113485206u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_17") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101338477u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_18") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107182360u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_19") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107920327u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_1") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107864468u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_20") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112676315u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_21") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103927340u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_22") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107867845u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_23") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104176686u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_24") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103192359u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_25") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115044600u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_26") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105482753u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_27") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109974474u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_28") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114197219u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_29") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102644596u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_2") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105555309u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_30") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109684318u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_31") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105254453u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_32") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114839196u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_33") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112343691u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_34") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116045370u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_35") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105480113u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_36") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109591656u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_37") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109713367u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_38") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106607078u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_39") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101768573u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_3") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(100924822u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_40") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113861729u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_41") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110832810u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_42") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105118343u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_43") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106121176u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_44") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108100645u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_45") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102399502u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_46") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(100805051u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_47") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107623052u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_48") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103039849u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_49") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101116146u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_4") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103911759u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_50") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108712388u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_51") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102252979u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_52") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114047686u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_53") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101416029u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_54") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105271664u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_55") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115184191u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_56") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102639714u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_57") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106119129u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_58") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102129660u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_59") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103035499u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_5") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101716416u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_60") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(117356911u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_61") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110658016u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_62") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112947017u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_63") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110492754u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_64") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111183139u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_65") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102928820u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_66") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116238605u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_67") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102116790u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_68") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107719271u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_6") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112910761u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_7") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103066418u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_8") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107179771u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_9") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115681228u);
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

CHAOS_IL2CPP_INT32 chaos_static_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects___exitCode = 0;

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

extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_0(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_1(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_2(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_3(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_4(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_5(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_6(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_7(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_8(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_9(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_10(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_11(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_12(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_13(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_14(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_15(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_16(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_17(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_18(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_19(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_20(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_21(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_22(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_23(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_24(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_25(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_26(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_27(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_28(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_29(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_30(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_31(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_32(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_33(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_34(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_35(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_36(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_37(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_38(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_39(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_40(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_41(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_42(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_43(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_44(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_45(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_46(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_47(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_48(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_49(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_50(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_51(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_52(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_53(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_54(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_55(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_56(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_57(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_58(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_59(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_60(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_61(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_62(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_63(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_64(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_65(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_66(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_67(void);
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_68(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[70] = {
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
	::ChaosAbiMethodEntryV0 entries[69];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		69u,
		0u,
		1614941805u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_0
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_1
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_2
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_3
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_4
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_5
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_6
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_7
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_8
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_9
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_10
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_11
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_12
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_13
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_14
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_15
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_16
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_17
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_18
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_19
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_20
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_21
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_22
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_23
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_24
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_25
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_26
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_27
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_28
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_29
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_30
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_31
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_32
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_33
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_34
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_35
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_36
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_37
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_38
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_39
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_40
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_41
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_42
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_43
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_44
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_45
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_46
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_47
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_48
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_49
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_50
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_51
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_52
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_53
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_54
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_55
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_56
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_57
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_58
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_59
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_60
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_61
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_62
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_63
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_64
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_65
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_66
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_67
		{ 0u, 0u },  // GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_68
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
		"GeneratedMarshallingSubjects",
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
		&chaos_mt_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects.hot,
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
		/* .name_utf8         = */ "GeneratedMarshallingSubjects",
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
		::chaos::il2cpp::runtime_core::RegisterModule("GeneratedMarshallingSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[69] = {
	{ "Subject_0", 0x00000003u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_1", 0x00000004u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_2", 0x00000005u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_3", 0x00000006u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_4", 0x00000007u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_5", 0x00000008u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_6", 0x00000009u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_7", 0x0000000Au, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_8", 0x0000000Bu, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_9", 0x0000000Cu, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_10", 0x0000000Du, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_11", 0x0000000Eu, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_12", 0x0000000Fu, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_13", 0x00000010u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_14", 0x00000011u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_15", 0x00000012u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_16", 0x00000013u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_17", 0x00000014u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_18", 0x00000015u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_19", 0x00000016u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_20", 0x00000017u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_21", 0x00000018u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_22", 0x00000019u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_23", 0x0000001Au, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_24", 0x0000001Bu, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_25", 0x0000001Cu, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_26", 0x0000001Du, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_27", 0x0000001Eu, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_28", 0x0000001Fu, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_29", 0x00000020u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_30", 0x00000021u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_31", 0x00000022u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_32", 0x00000023u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_33", 0x00000024u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_34", 0x00000025u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_35", 0x00000026u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_36", 0x00000027u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_37", 0x00000028u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_38", 0x00000029u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_39", 0x0000002Au, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_40", 0x0000002Bu, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_41", 0x0000002Cu, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_42", 0x0000002Du, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_43", 0x0000002Eu, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_44", 0x0000002Fu, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_45", 0x00000030u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_46", 0x00000031u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_47", 0x00000032u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_48", 0x00000033u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_49", 0x00000034u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_50", 0x00000035u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_51", 0x00000036u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_52", 0x00000037u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_53", 0x00000038u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_54", 0x00000039u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_55", 0x0000003Au, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_56", 0x0000003Bu, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_57", 0x0000003Cu, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_58", 0x0000003Du, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_59", 0x0000003Eu, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_60", 0x0000003Fu, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_61", 0x00000040u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_62", 0x00000041u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_63", 0x00000042u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_64", 0x00000043u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_65", 0x00000044u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_66", 0x00000045u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_67", 0x00000046u, 0u },  // GeneratedMarshallingSubjects
	{ "Subject_68", 0x00000047u, 0u },  // GeneratedMarshallingSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "GeneratedMarshallingSubjects", "", 0u, 69u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[69] = {
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
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[69] = {
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_0
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_1
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_2
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_3
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_4
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_5
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_6
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_7
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_8
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_9
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_10
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_11
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_12
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_13
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_14), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_14
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_15), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_15
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_16), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_16
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_17), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GeneratedMarshallingSubjects::Subject_17
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_18), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GeneratedMarshallingSubjects::Subject_18
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_19), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GeneratedMarshallingSubjects::Subject_19
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_20), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GeneratedMarshallingSubjects::Subject_20
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_21), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GeneratedMarshallingSubjects::Subject_21
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_22), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GeneratedMarshallingSubjects::Subject_22
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_23), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GeneratedMarshallingSubjects::Subject_23
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_24), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GeneratedMarshallingSubjects::Subject_24
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_25), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GeneratedMarshallingSubjects::Subject_25
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_26), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GeneratedMarshallingSubjects::Subject_26
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_27), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GeneratedMarshallingSubjects::Subject_27
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_28), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_28
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_29), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_29
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_30), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GeneratedMarshallingSubjects::Subject_30
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_31), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_31
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_32
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_33), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_33
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_34), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_34
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_35), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_35
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_36), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_36
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_37), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_37
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_38), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_38
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_39), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_39
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_40), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_40
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_41), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_41
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_42), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_42
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_43), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_43
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_44), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_44
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_45), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_45
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_46), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_46
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_47), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_47
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_48), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_48
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_49), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_49
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_50), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_50
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_51), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_51
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_52), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_52
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_53), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_53
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_54), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_54
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_55), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_55
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_56), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_56
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_57), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GeneratedMarshallingSubjects::Subject_57
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_58), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_58
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_59), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_59
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_60), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_60
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_61), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_61
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_62), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_62
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_63), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_63
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_64), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GeneratedMarshallingSubjects::Subject_64
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_65), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GeneratedMarshallingSubjects::Subject_65
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_66), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GeneratedMarshallingSubjects::Subject_66
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_67), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GeneratedMarshallingSubjects::Subject_67
	{ reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_68), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GeneratedMarshallingSubjects::Subject_68
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"GeneratedMarshallingSubjects",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	69u,
	s_hotpatch_slots,
	69u,
	s_hotpatch_entries,
	69u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[24] = {
	"System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsVoidMarshaller::ConvertToUnmanaged:System.Void(System.Exception)",
	"GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::_exitCode",
	"System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComClassAttribute::.ctor:System.Void()",
	"System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_ExceptionToUnmanagedMarshaller:System.Type()",
	"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()",
	"System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_Options:System.Runtime.InteropServices.Marshalling.ComInterfaceOptions()",
	"System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceOptions::GetHashCode:System.Int32()",
	"System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_StringMarshalling:StringMarshalling()",
	"System.Runtime.InteropServices/StringMarshalling::GetHashCode:System.Int32()",
	"System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_StringMarshallingCustomType:System.Type()",
	"System.Private.CoreLib/System.Byte",
	"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)",
	"System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_ExceptionToUnmanagedMarshaller:System.Void(System.Type)",
	"System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_Options:System.Void(System.Runtime.InteropServices.Marshalling.ComInterfaceOptions)",
	"System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_StringMarshalling:System.Void(StringMarshalling)",
	"System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_StringMarshallingCustomType:System.Void(System.Type)",
	"System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::.ctor:System.Void()",
	"System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::Clear:System.Void(System.Runtime.InteropServices.Marshalling.IIUnknownStrategy)",
	"System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::.ctor:System.Void()",
	"System.Private.CoreLib/System.Runtime.InteropServices.Marshalling.CustomMarshallerAttribute::get_ManagedType:System.Type()",
	"System.Private.CoreLib/System.Runtime.InteropServices.Marshalling.CustomMarshallerAttribute::get_MarshallerType:System.Type()",
	"System.Private.CoreLib/System.Runtime.InteropServices.Marshalling.CustomMarshallerAttribute::get_MarshalMode:System.Runtime.InteropServices.Marshalling.MarshalMode()",
	"System.Private.CoreLib/System.Runtime.InteropServices.Marshalling.MarshalMode::GetHashCode:System.Int32()",
	"System.Private.CoreLib/System.Runtime.InteropServices.Marshalling.CustomMarshallerAttribute::.ctor:System.Void(System.Type,System.Runtime.InteropServices.Marshalling.MarshalMode,System.Type)",
};

extern "C" void* kChaosExternalRuntimeFnTable[24] = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__),
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
};

extern "C" int32_t kChaosExternalRuntimeCount = 24;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[69])() = {
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_6),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_7),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_8),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_9),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_10),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_11),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_12),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_13),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_14),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_15),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_16),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_17),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_18),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_19),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_20),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_21),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_22),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_23),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_24),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_25),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_26),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_27),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_28),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_29),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_30),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_31),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_32),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_33),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_34),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_35),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_36),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_37),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_38),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_39),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_40),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_41),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_42),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_43),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_44),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_45),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_46),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_47),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_48),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_49),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_50),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_51),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_52),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_53),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_54),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_55),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_56),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_57),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_58),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_59),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_60),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_61),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_62),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_63),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_64),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_65),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_66),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_67),
	reinterpret_cast<void(*)()>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_68),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[69])() = {
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
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 69;
extern "C" const int kSubjectEntryIndices[69] = {
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

	/* ── Entry 0: GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_17 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry0;
	/* ── Entry 1: GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_18 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry1;
	/* ── Entry 2: GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_19 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry2;
	/* ── Entry 3: GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_20 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry3;
	/* ── Entry 4: GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_21 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry4;
	/* ── Entry 5: GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_22 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry5;
	/* ── Entry 6: GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_23 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry6;
	/* ── Entry 7: GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_24 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry7;
	/* ── Entry 8: GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_25 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry8;
	/* ── Entry 9: GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_26 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry9;
	/* ── Entry 10: GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_27 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry10;
	/* ── Entry 11: GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_30 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry11;
	/* ── Entry 12: GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_57 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry12;
	/* ── Entry 13: GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_65 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry13;
	/* ── Entry 14: GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_66 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry14;
	/* ── Entry 15: GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_67 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry15;
	/* ── Entry 16: GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_68 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry16;
} kChaosGcSlotMapsSection = {
	/* entry0 = GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_17 */
	.entry0 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_17),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry1 = GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_18 */
	.entry1 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_18),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry2 = GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_19 */
	.entry2 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_19),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry3 = GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_20 */
	.entry3 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_20),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry4 = GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_21 */
	.entry4 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_21),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry5 = GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_22 */
	.entry5 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_22),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry6 = GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_23 */
	.entry6 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_23),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry7 = GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_24 */
	.entry7 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_24),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry8 = GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_25 */
	.entry8 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_25),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry9 = GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_26 */
	.entry9 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_26),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry10 = GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_27 */
	.entry10 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_27),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry11 = GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_30 */
	.entry11 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_30),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry12 = GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_57 */
	.entry12 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_57),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry13 = GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_65 */
	.entry13 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_65),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry14 = GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_66 */
	.entry14 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_66),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry15 = GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_67 */
	.entry15 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_67),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry16 = GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_68 */
	.entry16 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_68),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 584u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[69] = {
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_0),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_1),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_2),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_3),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_4),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_5),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_6),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_7),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_8),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_9),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_10),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_11),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_12),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_13),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_14),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_15),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_16),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_17),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_18),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_19),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_20),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_21),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_22),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_23),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_24),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_25),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_26),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_27),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_28),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_29),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_30),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_31),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_32),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_33),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_34),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_35),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_36),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_37),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_38),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_39),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_40),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_41),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_42),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_43),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_44),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_45),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_46),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_47),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_48),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_49),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_50),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_51),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_52),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_53),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_54),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_55),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_56),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_57),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_58),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_59),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_60),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_61),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_62),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_63),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_64),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_65),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_66),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_67),
	reinterpret_cast<void*>(&GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_68),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 69u,
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
	.image_name_utf8    = "GeneratedMarshallingSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects[1] = {
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects[69] = {
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_6:System.Void()", "Subject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_7:System.Void()", "Subject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_8:System.Void()", "Subject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_9:System.Void()", "Subject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_10:System.Void()", "Subject_10", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_11:System.Void()", "Subject_11", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_12:System.Void()", "Subject_12", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_13:System.Void()", "Subject_13", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_14:System.Void()", "Subject_14", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_15:System.Void()", "Subject_15", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_16:System.Void()", "Subject_16", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_17:System.Void()", "Subject_17", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_18:System.Void()", "Subject_18", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_19:System.Void()", "Subject_19", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_20:System.Void()", "Subject_20", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_21:System.Void()", "Subject_21", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_22:System.Void()", "Subject_22", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_23:System.Void()", "Subject_23", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_24:System.Void()", "Subject_24", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_25:System.Void()", "Subject_25", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_26:System.Void()", "Subject_26", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_27:System.Void()", "Subject_27", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_28:System.Void()", "Subject_28", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_29:System.Void()", "Subject_29", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_30:System.Void()", "Subject_30", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_31:System.Void()", "Subject_31", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_32:System.Void()", "Subject_32", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_33:System.Void()", "Subject_33", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_34:System.Void()", "Subject_34", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_35:System.Void()", "Subject_35", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_36:System.Void()", "Subject_36", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_37:System.Void()", "Subject_37", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_38:System.Void()", "Subject_38", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_39:System.Void()", "Subject_39", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_40:System.Void()", "Subject_40", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_41:System.Void()", "Subject_41", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_42:System.Void()", "Subject_42", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_43:System.Void()", "Subject_43", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_44:System.Void()", "Subject_44", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_45:System.Void()", "Subject_45", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_46:System.Void()", "Subject_46", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_47:System.Void()", "Subject_47", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_48:System.Void()", "Subject_48", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_49:System.Void()", "Subject_49", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_50:System.Void()", "Subject_50", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_51:System.Void()", "Subject_51", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_52:System.Void()", "Subject_52", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_53:System.Void()", "Subject_53", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_54:System.Void()", "Subject_54", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_55:System.Void()", "Subject_55", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_56:System.Void()", "Subject_56", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_57:System.Void()", "Subject_57", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_58:System.Void()", "Subject_58", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_59:System.Void()", "Subject_59", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_60:System.Void()", "Subject_60", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_61:System.Void()", "Subject_61", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_62:System.Void()", "Subject_62", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_63:System.Void()", "Subject_63", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_64:System.Void()", "Subject_64", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_65:System.Void()", "Subject_65", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_66:System.Void()", "Subject_66", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_67:System.Void()", "Subject_67", "System.Void", 0, nullptr, 0u },
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_68:System.Void()", "Subject_68", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0u, "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects", "GeneratedMarshallingSubjects/GeneratedMarshallingSubjects", "", "GeneratedMarshallingSubjects", "GeneratedMarshallingSubjects", nullptr, kReflFields_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects, 1u, nullptr, 0u,
	kReflMethods_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects, 69u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "GeneratedMarshallingSubjects", kReflTypePtrs, 1u };

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
// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_0()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_1()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_2()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_3()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_4()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_5()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_6()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_6(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_7()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_7(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_8()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_9()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_9(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_10()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_10(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_11()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_11(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_12()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_12(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_13()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_13(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_14()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_14(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_15()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_15(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_16()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_16(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_17()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_17(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[0])(chaos_arg_0);
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
				chaos_static_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_18()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_18(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComClassAttribute, {});
				chaos_object->header.type_info = &chaos_mt_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComClassAttribute.hot;
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
				chaos_static_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_19()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_19(void)
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[3])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[3])();
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
				chaos_static_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects___exitCode = chaos_value;
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
				chaos_static_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_20()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_20(void)
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[5])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s1;
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[6])();
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s3 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[5])();
				_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s4;
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[6])();
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
				chaos_static_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects___exitCode = chaos_value;
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
				chaos_static_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_21()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_21(void)
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[7])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s1;
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[8])();
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s3 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[7])();
				_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s4;
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[8])();
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
				chaos_static_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects___exitCode = chaos_value;
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
				chaos_static_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_22()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_22(void)
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
				chaos_static_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects___exitCode = chaos_value;
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
				chaos_static_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_23()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_23(void)
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
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[12])(chaos_arg_0);
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
				chaos_static_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_24()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_24(void)
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
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[13])(chaos_arg_0);
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
				chaos_static_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_25()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_25(void)
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
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[14])(chaos_arg_0);
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
				chaos_static_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_26()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_26(void)
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
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[15])(chaos_arg_0);
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
				chaos_static_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_27()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_27(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComInterfaceAttribute, {});
				chaos_object->header.type_info = &chaos_mt_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComInterfaceAttribute.hot;
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
				chaos_static_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_28()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_28(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_29()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_29(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_30()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_30(void)
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
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[17])(chaos_arg_0);
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
				chaos_static_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_31()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_31(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_32()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_32(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_33()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_33(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_34()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_34(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_35()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_35(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_36()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_36(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_37()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_37(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_38()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_38(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_39()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_39(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_40()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_40(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_41()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_41(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_42()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_42(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_43()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_43(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_44()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_44(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_45()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_45(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_46()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_46(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_47()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_47(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_48()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_48(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_49()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_49(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_50()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_50(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_51()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_51(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_52()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_52(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_53()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_53(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_54()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_54(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_55()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_55(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_56()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_56(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_57()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_57(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers, {});
				chaos_object->header.type_info = &chaos_mt_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers.hot;
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
				chaos_static_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_58()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_58(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_59()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_59(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_60()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_60(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_61()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_61(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_62()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_62(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_63()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_63(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_64()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_64(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_65()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_65(void)
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
				chaos_static_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects___exitCode = chaos_value;
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
				chaos_static_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_66()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_66(void)
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
				chaos_static_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects___exitCode = chaos_value;
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
				chaos_static_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_67()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_67(void)
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[21])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s1;
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[22])();
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s3 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[21])();
				_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s4;
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[22])();
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
				chaos_static_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects___exitCode = chaos_value;
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
				chaos_static_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GeneratedMarshallingSubjects/GeneratedMarshallingSubjects::Subject_68()
extern "C" void GeneratedMarshallingSubjects_GeneratedMarshallingSubjects_Subject_68(void)
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


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s2;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Runtime_InteropServices_Marshalling_CustomMarshallerAttribute, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Runtime_InteropServices_Marshalling_CustomMarshallerAttribute.hot;
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
				chaos_static_GeneratedMarshallingSubjects_GeneratedMarshallingSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}



}  // namespace chaos::il2cpp::codegen::GeneratedMarshallingSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 69;

extern "C" void ChaosJitRegisterAll() {}