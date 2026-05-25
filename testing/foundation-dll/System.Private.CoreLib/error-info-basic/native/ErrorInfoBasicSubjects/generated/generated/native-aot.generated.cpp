#include <chaos/common.h>
#include <chaos/type_info.h>
#include "runtime_core.h"
#include <chaos/eh.h>
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
#include "enum_stubs.h"
#include "enum_metadata.generated.h"

// Forward declaration for dispatch table entries (defined in runtime_stubs.cpp)
extern "C" void InterpreterEntryDirect(
    CHAOS_IL2CPP_UINTPTR method_key,
    void*     args_buf,
    void*     ret_buf) noexcept;

#pragma warning(push)
#pragma warning(disable: 4065 4244)

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" CHAOS_IL2CPP_INT32 ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetExceptionForHR_Zero(void);
extern "C" CHAOS_IL2CPP_INT32 ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetExceptionForHR_Zero(void);
extern "C" CHAOS_IL2CPP_INT32 ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetHRForException(void);
extern "C" CHAOS_IL2CPP_INT32 ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestThrowExceptionForHR_S_OK(void);
extern "C" void ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_0(void);
extern "C" void ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_1(void);
extern "C" void ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_2(void);
extern "C" void ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_3(void);
extern "C" void ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_4(void);

namespace chaos::il2cpp::codegen::ErrorInfoBasicSubjects {

// Bring runtime_core and jit declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;
using namespace chaos::il2cpp::jit;

// Runtime prelude dependencies included at translation unit level


inline MethodTable chaos_mt_ErrorInfoBasicSubjects_ErrorInfoBasicSubjects = {nullptr, nullptr, 10241870055853413912ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ErrorInfoBasicSubjects_ErrorInfoBasicSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(10241870055853413912ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Exception = {nullptr, nullptr, 10972282733316558392ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Exception = static_cast<CHAOS_IL2CPP_INTPTR>(10972282733316558392ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Object = {nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {nullptr, nullptr, 5474029880995115448ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly = static_cast<CHAOS_IL2CPP_INTPTR>(5474029880995115448ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName = {nullptr, nullptr, 17082367815459723707ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = static_cast<CHAOS_IL2CPP_INTPTR>(17082367815459723707ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo = {nullptr, nullptr, 4137207361503509124ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo = static_cast<CHAOS_IL2CPP_INTPTR>(4137207361503509124ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo = {nullptr, nullptr, 17040031516751226236ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo = static_cast<CHAOS_IL2CPP_INTPTR>(17040031516751226236ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo = {nullptr, nullptr, 10748947813473285525ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = static_cast<CHAOS_IL2CPP_INTPTR>(10748947813473285525ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_String = {nullptr, nullptr, 1782325859292956794ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_INTPTR>(1782325859292956794ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Type = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), nullptr, 3222174513575444759ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Type = static_cast<CHAOS_IL2CPP_INTPTR>(3222174513575444759ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Int32 = {nullptr, nullptr, 11009693519287992193ULL, 0u, 32, 2, 0, nullptr, nullptr, 0, 0, 0, 0};
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
		return chaos_array->element_type_shape == chaos_type_shape_reference            && chaos_is_type_compatible(chaos_mt_System_Private_CoreLib_System_String.AsTypeInfoHot(), chaos_array->element_type_info);
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

struct chaos_type_ErrorInfoBasicSubjects_ErrorInfoBasicSubjects
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Exception
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
	chaos_string->header.type_info = chaos_mt_System_Private_CoreLib_System_String.AsTypeInfoHot();
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
	chaos_type->header.type_info = chaos_mt_System_Private_CoreLib_System_Type.AsTypeInfoHot();
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
		case static_cast<CHAOS_IL2CPP_INTPTR>(39266078u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Run") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111882186u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "TestGetExceptionForHR") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109454958u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "TestGetExceptionForHR_Zero") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109543103u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "TestGetHRForException") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111645306u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "TestThrowExceptionForHR_S_OK") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108615801u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(41478040u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_0") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109030481u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_1") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116995802u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_2") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103458295u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_3") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(117153160u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_4") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112842581u);
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
	chaos_method->header.type_info = chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo.AsTypeInfoHot();
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
	chaos_closed_method->header.type_info = chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo.AsTypeInfoHot();
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


// ── GcTypeLayout pointer offset arrays (computed via offsetof) ──
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Exception[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Exception, _message)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Exception, _innerException)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Exception, _stackTrace))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly, runtime_assembly_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Type[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Type, runtime_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Type, runtime_name_value))};

extern "C" void ChaosRegisterGcLayouts() {
	auto& registry = chaos::il2cpp::runtime_core::GcLayoutRegistry::Instance();

	registry.Register(10241870055853413912ULL, sizeof(chaos_type_ErrorInfoBasicSubjects_ErrorInfoBasicSubjects), nullptr, 0);
	registry.Register(10972282733316558392ULL, sizeof(chaos_type_System_Private_CoreLib_System_Exception), kGcOffsets_chaos_type_System_Private_CoreLib_System_Exception, 3);
	registry.Register(15228727185366376748ULL, sizeof(chaos_type_System_Private_CoreLib_System_Object), nullptr, 0);
	registry.Register(5474029880995115448ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly, 1);
	registry.Register(17082367815459723707ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, 1);
	registry.Register(4137207361503509124ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, 5);
	registry.Register(17040031516751226236ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, 2);
	registry.Register(10748947813473285525ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, 5);
	registry.Register(1782325859292956794ULL, sizeof(chaos_type_System_Private_CoreLib_System_String), nullptr, 0);
	registry.Register(3222174513575444759ULL, sizeof(chaos_type_System_Private_CoreLib_System_Type), kGcOffsets_chaos_type_System_Private_CoreLib_System_Type, 2);

	// Register MethodTable address ranges for IsValidTypeInfoPointer.
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_ErrorInfoBasicSubjects_ErrorInfoBasicSubjects), reinterpret_cast<uintptr_t>(&chaos_mt_ErrorInfoBasicSubjects_ErrorInfoBasicSubjects) + sizeof(chaos_mt_ErrorInfoBasicSubjects_ErrorInfoBasicSubjects));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Exception), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Exception) + sizeof(chaos_mt_System_Private_CoreLib_System_Exception));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object) + sizeof(chaos_mt_System_Private_CoreLib_System_Object));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_Assembly));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String) + sizeof(chaos_mt_System_Private_CoreLib_System_String));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Type), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Type) + sizeof(chaos_mt_System_Private_CoreLib_System_Type));
}

CHAOS_IL2CPP_INT32 chaos_static_ErrorInfoBasicSubjects_ErrorInfoBasicSubjects___exitCode = 0;


	// AOT-baked string table: sorted by StringId for binary search at runtime.
	constexpr chaos::il2cpp::string_table::StringEntry chaos_aot_string_entries[] = {
		{ 1243298078550130315U, "_HResult", 8u },
		{ 1507291510045848963U, "80004005", 8u },
		{ 1892267295418834067U, "0X80004005", 10u },
		{ 2005660522750557875U, "0x80004005", 10u },
		{ 3005067945668247343U, "test error", 10u },
		{ 8098064423716734927U, "HResult", 7u },
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

static void chaos_external_runtime_System_Private_CoreLib_System_Exception___ctor_System_Void_System_String_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	ChaosReflectionSetExceptionMetadata(chaos_fn_arg_0, chaos_fn_arg_1);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Exception__get_Message_System_String__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetExceptionMessage(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_String__Contains_System_Boolean_System_String_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	return ChaosStringContains(chaos_fn_arg_0, chaos_fn_arg_1);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetField_System_Reflection_FieldInfo_System_String_System_Reflection_BindingFlags_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2)
{
	(void)chaos_arg_0; (void)chaos_arg_1; (void)chaos_arg_2;
	static CHAOS_IL2CPP_UINT8 s_sentinel = 0;
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_sentinel);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperty_System_Reflection_PropertyInfo_System_String_System_Reflection_BindingFlags_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2)
{
	(void)chaos_arg_0; (void)chaos_arg_1; (void)chaos_arg_2;
	static CHAOS_IL2CPP_UINT8 s_sentinel = 0;
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_sentinel);
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


// ── Bridge/import thunks ──
extern "C" void* kChaosExternalRuntimeFnTable[];

// Bridge/import thunk for: ErrorInfoBasicSubjects/ErrorInfoBasicSubjects::_exitCode
extern "C" void chaos_bridge_thunk_0(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[0])();
}

// Bridge/import thunk for: System.Runtime.InteropServices/Marshal::GetExceptionForHR:System.Exception(System.Int32)
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[1])(chaos_fn_arg_0);
	return result;
}

// Bridge/import thunk for: System.Private.CoreLib/System.String::IsNullOrEmpty:System.Boolean(System.String)
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_2(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[3])(chaos_fn_arg_0);
	return result;
}

// Bridge/import thunk for: System.Private.CoreLib/System.Exception
extern "C" void chaos_bridge_thunk_3(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[6])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Int32
extern "C" void chaos_bridge_thunk_4(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[9])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Reflection.FieldInfo::SetValue:System.Void(System.Object,System.Object)
extern "C" void chaos_bridge_thunk_5(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[10])(chaos_fn_arg_0, chaos_fn_arg_1);
}

// Bridge/import thunk for: System.Private.CoreLib/System.Reflection.PropertyInfo::SetValue:System.Void(System.Object,System.Object)
extern "C" void chaos_bridge_thunk_6(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[12])(chaos_fn_arg_0, chaos_fn_arg_1);
}

// Bridge/import thunk for: System.Runtime.InteropServices/Marshal::GetHRForException:System.Int32(System.Exception)
extern "C" CHAOS_IL2CPP_INT32 chaos_bridge_thunk_7(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[13])(chaos_fn_arg_0);
	return result;
}

// Bridge/import thunk for: System.Runtime.InteropServices/Marshal::ThrowExceptionForHR:System.Void(System.Int32)
extern "C" void chaos_bridge_thunk_8(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[14])(chaos_fn_arg_0);
}

extern "C" void ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_0(void);
extern "C" void ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_1(void);
extern "C" void ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_2(void);
extern "C" void ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_3(void);
extern "C" void ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_4(void);
extern "C" CHAOS_IL2CPP_INT32 ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetExceptionForHR_Zero(void);
extern "C" CHAOS_IL2CPP_INT32 ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetExceptionForHR(void);
extern "C" CHAOS_IL2CPP_INT32 ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetHRForException(void);
extern "C" CHAOS_IL2CPP_INT32 ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestThrowExceptionForHR_S_OK(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[11] = {
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	1u,
	1u,
	1u,
	1u,
	1u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[10];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		10u,
		1u,
		2640594090u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_0
		{ 0u, 0u },  // ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_1
		{ 0u, 0u },  // ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_2
		{ 0u, 0u },  // ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_3
		{ 0u, 0u },  // ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_4
		{ 1u, 1u },  // ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_Run_System_Int32
		{ 1u, 0u },  // ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetExceptionForHR_Zero
		{ 1u, 0u },  // ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetExceptionForHR
		{ 1u, 0u },  // ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetHRForException
		{ 1u, 0u },  // ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestThrowExceptionForHR_S_OK
	},
	{
		1u,
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[3] = {
		0u,
		1548u,
		1548u,
	};

	static constexpr const char* s_type_names[3] = {
		"<Module>",
		"ErrorInfoBasicSubjects",
		"ErrorInfoBasicNativeEntry",
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
		chaos_mt_ErrorInfoBasicSubjects_ErrorInfoBasicSubjects.AsTypeInfoHot(),
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
		/* .name_utf8         = */ "ErrorInfoBasicSubjects",
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
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 -- deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,
	/* .custom_attribute_method_offset   = */ nullptr,
	/* .custom_attribute_field_offset    = */ nullptr,
	/* .custom_attribute_property_offset = */ nullptr,
	/* .custom_attribute_param_offset    = */ nullptr,
	/* .custom_attribute_method_count    = */ 0u,
	/* .custom_attribute_field_count     = */ 0u,
	/* .custom_attribute_property_count  = */ 0u,
	/* .custom_attribute_param_count   = */ 0u,
		/* .abi_manifest      = */ s_abi_manifest,
	};
	static const CHAOS_IL2CPP_UINT32 s_native_aot_module_id =
		::chaos::il2cpp::runtime_core::RegisterModule("ErrorInfoBasicSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[10] = {
	{ "Run", 0x00000009u, 1u },  // ErrorInfoBasicNativeEntry
	{ "TestGetExceptionForHR_Zero", 0x0000000Du, 0u },  // ErrorInfoBasicNativeEntry
	{ "TestGetExceptionForHR", 0x0000000Au, 0u },  // ErrorInfoBasicNativeEntry
	{ "TestGetHRForException", 0x0000000Bu, 0u },  // ErrorInfoBasicNativeEntry
	{ "TestThrowExceptionForHR_S_OK", 0x0000000Cu, 0u },  // ErrorInfoBasicNativeEntry
	{ "Subject_0", 0x00000004u, 0u },  // ErrorInfoBasicSubjects
	{ "Subject_1", 0x00000005u, 0u },  // ErrorInfoBasicSubjects
	{ "Subject_2", 0x00000006u, 0u },  // ErrorInfoBasicSubjects
	{ "Subject_3", 0x00000007u, 0u },  // ErrorInfoBasicSubjects
	{ "Subject_4", 0x00000008u, 0u },  // ErrorInfoBasicSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[2] = {
	{ "ErrorInfoBasicNativeEntry", "", 0u, 5u },
	{ "ErrorInfoBasicSubjects", "", 5u, 5u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[10] = {
	{ 0x00000004u, 0u },
	{ 0x00000005u, 1u },
	{ 0x00000006u, 2u },
	{ 0x00000007u, 3u },
	{ 0x00000008u, 4u },
	{ 0x00000009u, 5u },
	{ 0x0000000Au, 7u },
	{ 0x0000000Bu, 8u },
	{ 0x0000000Cu, 9u },
	{ 0x0000000Du, 6u },
};

// Dispatch table (function pointers)
// s_hotpatch_entries — external linkage for verification_dispatch
// NOTE: `static` is deliberately omitted so that the verification dispatch
// TU (verification_dispatch.generated.cpp) can access this array via
// GetHotpatchEntries(). Without external linkage, the linker cannot resolve
// the symbol across translation units.
extern "C" HotpatchEntryV0 s_hotpatch_entries[10] = {
	{ reinterpret_cast<void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ErrorInfoBasicSubjects::Subject_0
	{ reinterpret_cast<void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ErrorInfoBasicSubjects::Subject_1
	{ reinterpret_cast<void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ErrorInfoBasicSubjects::Subject_2
	{ reinterpret_cast<void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ErrorInfoBasicSubjects::Subject_3
	{ reinterpret_cast<void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ErrorInfoBasicSubjects::Subject_4
	{ reinterpret_cast<void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_Run_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ErrorInfoBasicNativeEntry::Run
	{ reinterpret_cast<void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetExceptionForHR_Zero), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ErrorInfoBasicNativeEntry::TestGetExceptionForHR_Zero
	{ reinterpret_cast<void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetExceptionForHR), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ErrorInfoBasicNativeEntry::TestGetExceptionForHR
	{ reinterpret_cast<void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetHRForException), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ErrorInfoBasicNativeEntry::TestGetHRForException
	{ reinterpret_cast<void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestThrowExceptionForHR_S_OK), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ErrorInfoBasicNativeEntry::TestThrowExceptionForHR_S_OK
};

// Accessor functions for verification_dispatch.generated.cpp
extern "C" const HotpatchEntryV0* GetHotpatchEntries() noexcept { return s_hotpatch_entries; }
extern "C" int32_t GetHotpatchEntryCount() noexcept { return 10; }

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"ErrorInfoBasicSubjects",
	s_hotpatch_types,
	2u,
	s_hotpatch_methods,
	10u,
	s_hotpatch_slots,
	10u,
	s_hotpatch_entries,
	10u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[15] = {
	"ErrorInfoBasicSubjects/ErrorInfoBasicSubjects::_exitCode",
	"System.Runtime.InteropServices/Marshal::GetExceptionForHR:System.Exception(System.Int32)",
	"System.Private.CoreLib/System.Exception::get_Message:System.String()",
	"System.Private.CoreLib/System.String::IsNullOrEmpty:System.Boolean(System.String)",
	"System.Private.CoreLib/System.String::Contains:System.Boolean(System.String)",
	"System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String)",
	"System.Private.CoreLib/System.Exception",
	"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)",
	"System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String,System.Reflection.BindingFlags)",
	"System.Private.CoreLib/System.Int32",
	"System.Private.CoreLib/System.Reflection.FieldInfo::SetValue:System.Void(System.Object,System.Object)",
	"System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Reflection.BindingFlags)",
	"System.Private.CoreLib/System.Reflection.PropertyInfo::SetValue:System.Void(System.Object,System.Object)",
	"System.Runtime.InteropServices/Marshal::GetHRForException:System.Int32(System.Exception)",
	"System.Runtime.InteropServices/Marshal::ThrowExceptionForHR:System.Void(System.Int32)",
};

extern "C" void* kChaosExternalRuntimeFnTable[15] = {
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Exception__get_Message_System_String__),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_String__Contains_System_Boolean_System_String_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Exception___ctor_System_Void_System_String_),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetField_System_Reflection_FieldInfo_System_String_System_Reflection_BindingFlags_),
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperty_System_Reflection_PropertyInfo_System_String_System_Reflection_BindingFlags_),
	nullptr,
	nullptr,
	nullptr,
};

extern "C" int32_t kChaosExternalRuntimeCount = 15;
// (no method AOT entries for this module)
// ── Method table (kMethodTable[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
// NOTE: Pure data only — dispatch routing is in <chaos/hotpatch_dispatch.h>.
// JIT mode uses direct_ptr (precode trampoline → JIT compilation).

static void (*kMethodTable[10])() = {
	reinterpret_cast<void(*)()>(&ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_Run_System_Int32),
	reinterpret_cast<void(*)()>(&ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetExceptionForHR_Zero),
	reinterpret_cast<void(*)()>(&ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetExceptionForHR),
	reinterpret_cast<void(*)()>(&ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetHRForException),
	reinterpret_cast<void(*)()>(&ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestThrowExceptionForHR_S_OK),
};

// ── Default-arg thunks (kDefaultArgThunks[]) ────────────────────────────
// Each thunk supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_null_instance = 0;
extern "C" void (*kDefaultArgThunks[10])() = {
	[]() {kMethodTable[0]();},
	[]() {kMethodTable[1]();},
	[]() {kMethodTable[2]();},
	[]() {kMethodTable[3]();},
	[]() {kMethodTable[4]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[5])(0);},
	[]() {kMethodTable[6]();},
	[]() {kMethodTable[7]();},
	[]() {kMethodTable[8]();},
	[]() {kMethodTable[9]();},
};

// ── Subject slot map (kSubjectSlotMap[]) ────────────────────────────
// Maps subject index (0-based sequential) to kMethodTable index.
// External consumers use this to route --benchmark N to the correct
// AOT method slot, since kMethodTable[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 5;
extern "C" const int kSubjectSlotMap[5] = {
	0,
	1,
	2,
	3,
	4,
};
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

	/* ── Entry 0: ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry0;
	/* ── Entry 1: ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry1;
	/* ── Entry 2: ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry2;
	/* ── Entry 3: ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry3;
	/* ── Entry 4: ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry4;
	/* ── Entry 5: ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_Run_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry5;
	/* ── Entry 6: ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetExceptionForHR_Zero ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry6;
	/* ── Entry 7: ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetExceptionForHR ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[9];
	} entry7;
	/* ── Entry 8: ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetHRForException ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry8;
	/* ── Entry 9: ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestThrowExceptionForHR_S_OK ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry9;
} kChaosGcSlotMapsSection = {
	/* entry0 = ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_0 */
	.entry0 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry1 = ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_1 */
	.entry1 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry2 = ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_2 */
	.entry2 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_2),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry3 = ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_3 */
	.entry3 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_3),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry4 = ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_4 */
	.entry4 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_4),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry5 = ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_Run_System_Int32 */
	.entry5 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_Run_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 8u, 16u, 24u, 32u }
	},
	/* entry6 = ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetExceptionForHR_Zero */
	.entry6 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetExceptionForHR_Zero),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry7 = ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetExceptionForHR */
	.entry7 = {
		/* entry_total_size = 56 */ 56u,
		/* code_address */ reinterpret_cast<const void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetExceptionForHR),
		/* frame_size = 72 */ 72u,
		/* num_gc_slots = 9 */ 9u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u }
	},
	/* entry8 = ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetHRForException */
	.entry8 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetHRForException),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry9 = ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestThrowExceptionForHR_S_OK */
	.entry9 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestThrowExceptionForHR_S_OK),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 372u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[10] = {
	reinterpret_cast<void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_0),
	reinterpret_cast<void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_1),
	reinterpret_cast<void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_2),
	reinterpret_cast<void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_3),
	reinterpret_cast<void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_4),
	reinterpret_cast<void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_Run_System_Int32),
	reinterpret_cast<void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetExceptionForHR_Zero),
	reinterpret_cast<void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetExceptionForHR),
	reinterpret_cast<void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetHRForException),
	reinterpret_cast<void*>(&ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestThrowExceptionForHR_S_OK),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 10u,
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
	.image_name_utf8    = "ErrorInfoBasicSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_ErrorInfoBasicSubjects_ErrorInfoBasicSubjects[1] = {
	{ 0x04000001u, "ErrorInfoBasicSubjects/ErrorInfoBasicSubjects::_exitCode", "_exitCode", "System.Int32", 0LL, 3u },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_ErrorInfoBasicSubjects_ErrorInfoBasicSubjects[5] = {
	{ 0x00000004u, "ErrorInfoBasicSubjects/ErrorInfoBasicSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000005u, "ErrorInfoBasicSubjects/ErrorInfoBasicSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000006u, "ErrorInfoBasicSubjects/ErrorInfoBasicSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000007u, "ErrorInfoBasicSubjects/ErrorInfoBasicSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000008u, "ErrorInfoBasicSubjects/ErrorInfoBasicSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry[5] = {
	{ 0x00000009u, "ErrorInfoBasicSubjects/ErrorInfoBasicNativeEntry::Run:System.Int32(System.Int32)", "Run", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Du, "ErrorInfoBasicSubjects/ErrorInfoBasicNativeEntry::TestGetExceptionForHR_Zero:System.Int32()", "TestGetExceptionForHR_Zero", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Au, "ErrorInfoBasicSubjects/ErrorInfoBasicNativeEntry::TestGetExceptionForHR:System.Int32()", "TestGetExceptionForHR", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Bu, "ErrorInfoBasicSubjects/ErrorInfoBasicNativeEntry::TestGetHRForException:System.Int32()", "TestGetHRForException", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Cu, "ErrorInfoBasicSubjects/ErrorInfoBasicNativeEntry::TestThrowExceptionForHR_S_OK:System.Int32()", "TestThrowExceptionForHR_S_OK", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[2] = {
	{ 0x00000001u, "ErrorInfoBasicSubjects/ErrorInfoBasicSubjects", "ErrorInfoBasicSubjects/ErrorInfoBasicSubjects", "", "ErrorInfoBasicSubjects", "ErrorInfoBasicSubjects", nullptr, kReflFields_ErrorInfoBasicSubjects_ErrorInfoBasicSubjects, 1u, nullptr, 0u,
nullptr, 0u,    kReflMethods_ErrorInfoBasicSubjects_ErrorInfoBasicSubjects, 5u, nullptr, 0u, 0u },
	{ 0x00000002u, "ErrorInfoBasicSubjects/ErrorInfoBasicNativeEntry", "ErrorInfoBasicSubjects/ErrorInfoBasicNativeEntry", "", "ErrorInfoBasicNativeEntry", "ErrorInfoBasicNativeEntry", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry, 5u, nullptr, 0u, 0u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[2] = {
	&kReflTypes[0],
	&kReflTypes[1],
};

static const ReflectionQueryImageDescriptor kReflImage = { "ErrorInfoBasicSubjects", kReflTypePtrs, 2u, 1, 0, 0, 0 };

// Fake ImageHandle that ResolveSubjectId will decode back to kReflImage.
// BootstrapRuntime's aot_image_handle fallback discovers this via
// LookupModule(mid)->image at lines 311-321 of bootstrap.cpp.
	// Register generated enum metadata (tables + dispatch + type descriptors).
	// ChaosRegisterEnumGeneratedMetadata is defined in enum_metadata.generated.h.
	static const CHAOS_IL2CPP_UINT32 s_enum_registered = []() noexcept {
		ChaosRegisterEnumGeneratedMetadata();
		return 1u;
	}();

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
// Managed method: ErrorInfoBasicSubjects/ErrorInfoBasicSubjects::Subject_0()
extern "C" void ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s0;
				auto& _d5 = s_hotpatch_entries[5];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d5)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d5))
				{
					alignas(16) uint8_t _d_ab[4];
					ArgBuffer _d_bw(_d_ab);
					_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d5.method_key, _d_ab, &_d_hpresult);
				}
				else
				{
					_d_hpresult = ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_Run_System_Int32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s1;
				auto& _d5 = s_hotpatch_entries[5];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d5)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d5))
				{
					alignas(16) uint8_t _d_ab[4];
					ArgBuffer _d_bw(_d_ab);
					_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d5.method_key, _d_ab, &_d_hpresult);
				}
				else
				{
					_d_hpresult = ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_Run_System_Int32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
				}
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
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
				chaos_static_ErrorInfoBasicSubjects_ErrorInfoBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ErrorInfoBasicSubjects_ErrorInfoBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ErrorInfoBasicSubjects/ErrorInfoBasicSubjects::Subject_1()
extern "C" void ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	CHAOS_EH_TRY
			{
				auto& _d7 = s_hotpatch_entries[7];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d7)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d7))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d7.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetExceptionForHR();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d7 = s_hotpatch_entries[7];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d7)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d7))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d7.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetExceptionForHR();
				}
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
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
				chaos_static_ErrorInfoBasicSubjects_ErrorInfoBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ErrorInfoBasicSubjects_ErrorInfoBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ErrorInfoBasicSubjects/ErrorInfoBasicSubjects::Subject_2()
extern "C" void ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	CHAOS_EH_TRY
			{
				auto& _d8 = s_hotpatch_entries[8];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d8)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d8))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d8.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetHRForException();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d8 = s_hotpatch_entries[8];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d8)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d8))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d8.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetHRForException();
				}
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
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
				chaos_static_ErrorInfoBasicSubjects_ErrorInfoBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ErrorInfoBasicSubjects_ErrorInfoBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ErrorInfoBasicSubjects/ErrorInfoBasicSubjects::Subject_3()
extern "C" void ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	CHAOS_EH_TRY
			{
				auto& _d9 = s_hotpatch_entries[9];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d9)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d9))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d9.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestThrowExceptionForHR_S_OK();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d9 = s_hotpatch_entries[9];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d9)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d9))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d9.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestThrowExceptionForHR_S_OK();
				}
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
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
				chaos_static_ErrorInfoBasicSubjects_ErrorInfoBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ErrorInfoBasicSubjects_ErrorInfoBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ErrorInfoBasicSubjects/ErrorInfoBasicSubjects::Subject_4()
extern "C" void ErrorInfoBasicSubjects_ErrorInfoBasicSubjects_Subject_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	CHAOS_EH_TRY
			{
				auto& _d6 = s_hotpatch_entries[6];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d6)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d6))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d6.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetExceptionForHR_Zero();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d6 = s_hotpatch_entries[6];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d6)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d6))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d6.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetExceptionForHR_Zero();
				}
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
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
				chaos_static_ErrorInfoBasicSubjects_ErrorInfoBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ErrorInfoBasicSubjects_ErrorInfoBasicSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ErrorInfoBasicSubjects/ErrorInfoBasicNativeEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_switch_value = static_cast<CHAOS_IL2CPP_INT32>(_s0);
		switch (chaos_switch_value)
		{
			case 0:
				{
					auto& _d7 = s_hotpatch_entries[7];
					CHAOS_IL2CPP_INT32 _d_hpresult{};
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d7)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d7))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d7.method_key, nullptr, &_d_hpresult);
					}
					else
					{
						_d_hpresult = ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetExceptionForHR();
					}
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
				}
				chaos_locals[2] = _s0;
			case 1:
				{
					auto& _d8 = s_hotpatch_entries[8];
					CHAOS_IL2CPP_INT32 _d_hpresult{};
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d8)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d8))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d8.method_key, nullptr, &_d_hpresult);
					}
					else
					{
						_d_hpresult = ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetHRForException();
					}
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
				}
				chaos_locals[2] = _s0;
			case 2:
				{
					auto& _d9 = s_hotpatch_entries[9];
					CHAOS_IL2CPP_INT32 _d_hpresult{};
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d9)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d9))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d9.method_key, nullptr, &_d_hpresult);
					}
					else
					{
						_d_hpresult = ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestThrowExceptionForHR_S_OK();
					}
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
				}
				chaos_locals[2] = _s0;
			case 3:
			{
				{
					auto& _d6 = s_hotpatch_entries[6];
					CHAOS_IL2CPP_INT32 _d_hpresult{};
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d6)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d6))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d6.method_key, nullptr, &_d_hpresult);
					}
					else
					{
						_d_hpresult = ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetExceptionForHR_Zero();
					}
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
				}
				chaos_locals[2] = _s0;
				break;
			}
			default:
			{
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
				chaos_locals[2] = _s0;
				break;
			}
		}
	}
	_s0 = chaos_locals[2];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ErrorInfoBasicSubjects/ErrorInfoBasicNativeEntry::TestGetExceptionForHR_Zero()
extern "C" CHAOS_IL2CPP_INT32 ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetExceptionForHR_Zero(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};



	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[1])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = 0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			chaos_locals[2] = _s0;
		}
		else
		{
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			chaos_locals[2] = _s3;
		}
		_s3 = chaos_locals[2];
		return static_cast<CHAOS_IL2CPP_INT32>(_s3);
	}
}

// Managed method: ErrorInfoBasicSubjects/ErrorInfoBasicNativeEntry::TestGetExceptionForHR()
extern "C" CHAOS_IL2CPP_INT32 ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetExceptionForHR(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};



	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-2147467259);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[1])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = 0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
			_s3 = chaos_locals[1];
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = ChaosReflectionGetExceptionMessage(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[3])(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				if (_s3 == 0)
				{
					_s2 = chaos_locals[1];
					{
						const auto chaos_arg_0 = _s2;
						const auto chaos_result = ChaosReflectionGetExceptionMessage(chaos_arg_0);
						_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
					}
					chaos_locals[2] = _s2;
					_s2 = chaos_locals[2];
					{
						_s3 = CHAOS_IL2CPP_STRING_ID("0x80004005");
					}
					{
						const auto chaos_arg_1 = _s3;
						const auto chaos_arg_0 = _s2;
						const auto chaos_result = ChaosStringContains(chaos_arg_0, chaos_arg_1);
						_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
					}
					{
						if (_s2 == 0)
						{
							_s2 = chaos_locals[2];
							{
								_s3 = CHAOS_IL2CPP_STRING_ID("0X80004005");
							}
							{
								const auto chaos_arg_1 = _s3;
								const auto chaos_arg_0 = _s2;
								const auto chaos_result = ChaosStringContains(chaos_arg_0, chaos_arg_1);
								_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
							}
							{
								if (_s2 != 0)
								{
									_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
								}
								else
								{
									_s4 = chaos_locals[2];
									{
										_s5 = CHAOS_IL2CPP_STRING_ID("80004005");
									}
									{
										const auto chaos_arg_1 = _s5;
										const auto chaos_arg_0 = _s4;
										const auto chaos_result = ChaosStringContains(chaos_arg_0, chaos_arg_1);
										_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
									}
									_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
									_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
								}
							}
						}
						_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
						{
							if (_s4 == 0)
							{
								_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
								chaos_locals[4] = _s4;
							}
							else
							{
								_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
								chaos_locals[4] = _s4;
							}
						}
					}
				}
				else
				{
					_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
					chaos_locals[4] = _s5;
				}
			}
		}
		else
		{
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			chaos_locals[4] = _s3;
		}
		_s3 = chaos_locals[4];
		return static_cast<CHAOS_IL2CPP_INT32>(_s3);
	}
}

// Managed method: ErrorInfoBasicSubjects/ErrorInfoBasicNativeEntry::TestGetHRForException()
extern "C" CHAOS_IL2CPP_INT32 ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestGetHRForException(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};



	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-2147467263);
	chaos_locals[0] = _s0;
	{
		_s0 = CHAOS_IL2CPP_STRING_ID("test error");
	}
	{
		const auto chaos_arg_1 = _s0;
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Exception, {});
		chaos_object->header.type_info = chaos_mt_System_Private_CoreLib_System_Exception.AsTypeInfoHot();
		chaos_external_runtime_System_Private_CoreLib_System_Exception___ctor_System_Void_System_String_(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), chaos_arg_1);
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[1] = _s0;
	CHAOS_EH_TRY
			CHAOS_EH_TRY
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(34138872u);
					{
						const auto chaos_arg_0 = _s0;
						const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
						_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
					}
					{
						_s1 = CHAOS_IL2CPP_STRING_ID("_HResult");
					}
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(36);
					{
						const auto chaos_arg_2 = _s2;
						const auto chaos_arg_1 = _s1;
						const auto chaos_arg_0 = _s0;
						if (chaos_arg_0 == 0)
						{
							::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
						}
						const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetField_System_Reflection_FieldInfo_System_String_System_Reflection_BindingFlags_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
						_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
					}
					_s1 = _s0;
					{
						if (_s1 != 0)
						{
							_s1 = chaos_locals[1];
							_s2 = chaos_locals[0];
							{
								const auto chaos_value = _s2;
								auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_System_Private_CoreLib_System_Int32, {});
								chaos_boxed->header.type_info = chaos_mt_System_Private_CoreLib_System_Int32.AsTypeInfoHot();
								chaos_boxed->value = chaos_value;
								_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
							}
							{
								const auto chaos_arg_1 = _s2;
								const auto chaos_arg_0 = _s1;
								reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[10])(chaos_arg_0, chaos_arg_1);
							}
						}
						else
						{
						}
					}
			CHAOS_EH_CATCH_BEGIN
				if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
				{ CHAOS_EH_RETHROW; }
				_s0 = CHAOS_EH_EXCEPTION_OBJ;
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(34138872u);
					{
						const auto chaos_arg_0 = _s0;
						const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
						_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
					}
					{
						_s1 = CHAOS_IL2CPP_STRING_ID("HResult");
					}
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(36);
					{
						const auto chaos_arg_2 = _s2;
						const auto chaos_arg_1 = _s1;
						const auto chaos_arg_0 = _s0;
						const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetProperty_System_Reflection_PropertyInfo_System_String_System_Reflection_BindingFlags_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
						_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
					}
					_s1 = _s0;
					{
						if (_s1 != 0)
						{
							_s1 = chaos_locals[1];
							_s2 = chaos_locals[0];
							{
								const auto chaos_value = _s2;
								auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_System_Private_CoreLib_System_Int32, {});
								chaos_boxed->header.type_info = chaos_mt_System_Private_CoreLib_System_Int32.AsTypeInfoHot();
								chaos_boxed->value = chaos_value;
								_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
							}
							{
								const auto chaos_arg_1 = _s2;
								const auto chaos_arg_0 = _s1;
								reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[12])(chaos_arg_0, chaos_arg_1);
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
							chaos_locals[3] = _s0;
						}
						else
						{
						}
					}
			CHAOS_EH_END
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
			chaos_locals[3] = _s0;
	CHAOS_EH_END
	_s0 = chaos_locals[1];
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[13])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = chaos_locals[0];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			chaos_locals[3] = _s0;
		}
		else
		{
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			chaos_locals[3] = _s5;
		}
		_s5 = chaos_locals[3];
		return static_cast<CHAOS_IL2CPP_INT32>(_s5);
	}
}

// Managed method: ErrorInfoBasicSubjects/ErrorInfoBasicNativeEntry::TestThrowExceptionForHR_S_OK()
extern "C" CHAOS_IL2CPP_INT32 ErrorInfoBasicSubjects_ErrorInfoBasicNativeEntry_TestThrowExceptionForHR_S_OK(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[14])(chaos_arg_0);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			chaos_locals[0] = _s0;
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			chaos_locals[0] = _s0;
	CHAOS_EH_END
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}



}  // namespace chaos::il2cpp::codegen::ErrorInfoBasicSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 10;

extern "C" void ChaosJitRegisterAll() {}