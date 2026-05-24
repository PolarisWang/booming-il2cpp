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
#include "eeclass.h"
#include "load_store_chaos_bridge.h"
#include "interpreter_entry.h"
#include <gc/gc_bgc_inline.h>
#include <gc/gc_card_table.h>
#include <ChaosGeneratedRuntimePrelude.h>
#include "enum_metadata.generated.h"
#include "enum_stubs.h"

// Forward declaration for dispatch table entries (defined in runtime_stubs.cpp)
extern "C" void InterpreterEntryDirect(
    CHAOS_IL2CPP_UINTPTR method_key,
    void*     args_buf,
    void*     ret_buf) noexcept;

#pragma warning(push)
#pragma warning(disable: 4065 4244)

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestGenericInstantiation(void);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestSupplementalMetadataRoundtrip(void);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupBySubjectId(void);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupByToken(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_0(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_1(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_2(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_3(void);

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestGenericInstantiation(void);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestSupplementalMetadataRoundtrip(void);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupBySubjectId(void);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupByToken(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_0(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_1(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_2(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_3(void);

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestGenericInstantiation(void);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestSupplementalMetadataRoundtrip(void);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupBySubjectId(void);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupByToken(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_0(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_1(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_2(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_3(void);

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestGenericInstantiation(void);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestSupplementalMetadataRoundtrip(void);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupBySubjectId(void);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupByToken(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_0(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_1(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_2(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_3(void);

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestGenericInstantiation(void);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestSupplementalMetadataRoundtrip(void);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupBySubjectId(void);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupByToken(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_0(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_1(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_2(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_3(void);

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestGenericInstantiation(void);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestSupplementalMetadataRoundtrip(void);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupBySubjectId(void);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupByToken(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_0(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_1(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_2(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_3(void);

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestGenericInstantiation(void);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestSupplementalMetadataRoundtrip(void);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupBySubjectId(void);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupByToken(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_0(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_1(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_2(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_3(void);

namespace chaos::il2cpp::codegen::GenericSupplementSubjects {

// Bring runtime_core and jit declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;
using namespace chaos::il2cpp::jit;

// Runtime prelude dependencies included at translation unit level


inline MethodTable chaos_mt_GenericSupplementSubjects_GenericSupplementSubjects = {nullptr, nullptr, 10344406691918470264ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_GenericSupplementSubjects_GenericSupplementSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(10344406691918470264ULL);
inline MethodTable chaos_mt_System_Collections_System_Collections_Generic_List_System_String_ = {nullptr, nullptr, 8001668736026369753ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Collections_System_Collections_Generic_List_System_String_ = static_cast<CHAOS_IL2CPP_INTPTR>(8001668736026369753ULL);
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
inline MethodTable chaos_mt_System_Private_CoreLib_System_Type = {&chaos_mt_System_Private_CoreLib_System_Object, nullptr, 3222174513575444759ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
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
		return chaos_array->element_type_shape == chaos_type_shape_reference            && chaos_is_type_compatible(&chaos_mt_System_Private_CoreLib_System_String, chaos_array->element_type_info);
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

struct chaos_type_GenericSupplementSubjects_GenericSupplementSubjects
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Collections_System_Collections_Generic_List_System_String_
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR items_array = 0;  // GC array reference
	CHAOS_IL2CPP_INT32 size = 0;           // element count
	CHAOS_IL2CPP_INT32 version = 0;        // modification counter
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
	chaos_string->header.type_info = &chaos_mt_System_Private_CoreLib_System_String;
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
	chaos_type->header.type_info = &chaos_mt_System_Private_CoreLib_System_Type;
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
			chaos_assembly->header.type_info = &chaos_mt_System_Private_CoreLib_System_Reflection_Assembly;
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
	chaos_assembly_name->header.type_info = &chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName;
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
		case static_cast<CHAOS_IL2CPP_INTPTR>(46908478u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Run") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(117116202u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "TestGenericInstantiation") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105659306u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "TestSupplementalMetadataRoundtrip") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115286734u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "TestTypeLookupBySubjectId") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107617344u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "TestTypeLookupByToken") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108319552u);
			}

			break;
		case static_cast<CHAOS_IL2CPP_INTPTR>(45286136u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_0") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116803633u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_1") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110591674u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_2") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104259671u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_3") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104137960u);
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
	chaos_method->header.type_info = &chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo;
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
	chaos_closed_method->header.type_info = &chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo;
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
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly, runtime_assembly_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Type[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Type, runtime_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Type, runtime_name_value))};

extern "C" void ChaosRegisterGcLayouts() {
	auto& registry = chaos::il2cpp::runtime_core::GcLayoutRegistry::Instance();

	registry.Register(10344406691918470264ULL, sizeof(chaos_type_GenericSupplementSubjects_GenericSupplementSubjects), nullptr, 0);
	registry.Register(8001668736026369753ULL, sizeof(chaos_type_System_Collections_System_Collections_Generic_List_System_String_), nullptr, 0);
	registry.Register(15228727185366376748ULL, sizeof(chaos_type_System_Private_CoreLib_System_Object), nullptr, 0);
	registry.Register(5474029880995115448ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly, 1);
	registry.Register(17082367815459723707ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, 1);
	registry.Register(4137207361503509124ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, 5);
	registry.Register(17040031516751226236ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, 2);
	registry.Register(10748947813473285525ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, 5);
	registry.Register(1782325859292956794ULL, sizeof(chaos_type_System_Private_CoreLib_System_String), nullptr, 0);
	registry.Register(3222174513575444759ULL, sizeof(chaos_type_System_Private_CoreLib_System_Type), kGcOffsets_chaos_type_System_Private_CoreLib_System_Type, 2);

	// Register TypeInfoV0 address ranges for IsValidTypeInfoPointer.
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_GenericSupplementSubjects_GenericSupplementSubjects), reinterpret_cast<uintptr_t>(&chaos_mt_GenericSupplementSubjects_GenericSupplementSubjects) + sizeof(chaos_mt_GenericSupplementSubjects_GenericSupplementSubjects));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Collections_System_Collections_Generic_List_System_String_), reinterpret_cast<uintptr_t>(&chaos_mt_System_Collections_System_Collections_Generic_List_System_String_) + sizeof(chaos_mt_System_Collections_System_Collections_Generic_List_System_String_));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object) + sizeof(chaos_mt_System_Private_CoreLib_System_Object));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_Assembly));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String) + sizeof(chaos_mt_System_Private_CoreLib_System_String));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Type), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Type) + sizeof(chaos_mt_System_Private_CoreLib_System_Type));
}

CHAOS_IL2CPP_INT32 chaos_static_GenericSupplementSubjects_GenericSupplementSubjects___exitCode = 0;


	// AOT-baked string table: sorted by StringId for binary search at runtime.
	constexpr chaos::il2cpp::string_table::StringEntry chaos_aot_string_entries[] = {
		{ 626102635892993007U, "ok", 2u },
		{ 722077002288857021U, "System.Collections.Generic.List`1", 33u },
		{ 2354652740555965525U, "update", 6u },
		{ 3414838253445058619U, "|", 1u },
		{ 3701717109319258325U, "hot", 3u },
		{ 4405817535019295009U, "List", 4u },
		{ 8124737553590710379U, "System.Collections/System.Collections.Generic.List<System.Int32>", 64u },
		{ 8768195223735421365U, "Add", 3u },
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

static void chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_List_System_String___Add_System_Void_System_String_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
	auto* _list = reinterpret_cast<chaos_list_fields*>(reinterpret_cast<char*>(chaos_arg_0) + 8);
	auto* hdr = reinterpret_cast<chaos_list_array_header*>(_list->items_array);
	if (hdr == nullptr || _list->size >= hdr->capacity) {
		auto old_cap = (hdr != nullptr) ? hdr->capacity : 0;
		auto new_cap = (old_cap == 0) ? 4 : old_cap * 2;
		auto* new_buf = static_cast<CHAOS_IL2CPP_INTPTR*>(CHAOS_IL2CPP_MALLOC(sizeof(CHAOS_IL2CPP_INT32) + static_cast<CHAOS_IL2CPP_SIZE>(new_cap) * sizeof(CHAOS_IL2CPP_INTPTR)));
		auto* new_hdr = reinterpret_cast<chaos_list_array_header*>(new_buf);
		new_hdr->capacity = new_cap;
		auto* new_elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(new_hdr + 1);
		if (hdr != nullptr && _list->size > 0) {
			auto* old_elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(hdr + 1);
			std::memcpy(new_elems, old_elems, static_cast<CHAOS_IL2CPP_SIZE>(_list->size) * sizeof(CHAOS_IL2CPP_INTPTR));
			CHAOS_IL2CPP_FREE(hdr);
		}
		_list->items_array = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(new_hdr);
		hdr = new_hdr;
	}
	auto* elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(hdr + 1);
	elems[_list->size] = chaos_arg_1;
	_list->size++;
	_list->version++;
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Int32__Parse_System_Int32_System_String_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosParseInt32(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	auto* chaos_value_slot = chaos_resolve_native_int_slot(chaos_arg_0);
	const CHAOS_IL2CPP_STRING chaos_formatted = ChaosIl2cpp::Common::format_int32(static_cast<CHAOS_IL2CPP_INT32>(*chaos_value_slot));
	const auto chaos_id = chaos::il2cpp::string_table::Intern(
		chaos_formatted.c_str(), static_cast<CHAOS_IL2CPP_UINT32>(chaos_formatted.size()));
	return chaos_make_string_id_value(chaos_id);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_MetadataToken_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetMetadataToken(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_String__Contains_System_Boolean_System_String_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	return ChaosStringContains(chaos_fn_arg_0, chaos_fn_arg_1);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_String__Split_System_String___System_Char_System_StringSplitOptions_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2)
{
	(void)chaos_arg_0; (void)chaos_arg_1; (void)chaos_arg_2;
	static CHAOS_IL2CPP_UINT8 s_sentinel = 0;
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_sentinel);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetGenericArguments_System_Type____(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetGenericArguments(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetGenericTypeDefinition_System_Type__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetGenericTypeDefinition(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethod_System_Reflection_MethodInfo_System_String_System_Reflection_BindingFlags_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2)
{
	(void)chaos_arg_0; (void)chaos_arg_1; (void)chaos_arg_2;
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

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_FullName_System_String__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetTypeFullName(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsGenericType_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetIsGenericType(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__op_Inequality_System_Boolean_System_Type_System_Type_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	return ChaosTypeInequality(chaos_fn_arg_0, chaos_fn_arg_1);
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };


// ── Bridge/import thunks ──
extern "C" void* kChaosExternalRuntimeFnTable[];

// Bridge/import thunk for: GenericSupplementSubjects/GenericSupplementSubjects::_exitCode
extern "C" void chaos_bridge_thunk_0(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[0])();
}

// Bridge/import thunk for: System.Collections/System.Collections.Generic.List<System.String>::.ctor:System.Void()
extern "C" void chaos_bridge_thunk_1(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[1])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Reflection.MethodInfo::op_Equality:System.Boolean(System.Reflection.MethodInfo,System.Reflection.MethodInfo)
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_10(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[23])(chaos_fn_arg_0, chaos_fn_arg_1);
	return result;
}

// Bridge/import thunk for: System.Collections/System.Collections.Generic.List<System.String>::get_Count:System.Int32()
extern "C" CHAOS_IL2CPP_INT32 chaos_bridge_thunk_2(void)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)()>(kChaosExternalRuntimeFnTable[3])();
	return result;
}

// Bridge/import thunk for: System.Private.CoreLib/System.Object::GetType:System.Type()
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_3(void)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)()>(kChaosExternalRuntimeFnTable[4])();
	return result;
}

// Bridge/import thunk for: System.Collections/System.Collections.Generic.List`1
extern "C" void chaos_bridge_thunk_4(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[7])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.String
extern "C" void chaos_bridge_thunk_5(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[11])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.String::Concat:System.String(System.String[])
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_6(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[13])(chaos_fn_arg_0);
	return result;
}

// Bridge/import thunk for: System.Private.CoreLib/System.String::op_Inequality:System.Boolean(System.String,System.String)
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_7(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[15])(chaos_fn_arg_0, chaos_fn_arg_1);
	return result;
}

// Bridge/import thunk for: System.Collections/System.Collections.Generic.List<System.Int32>
extern "C" void chaos_bridge_thunk_8(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[17])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Type::op_Equality:System.Boolean(System.Type,System.Type)
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_9(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[21])(chaos_fn_arg_0, chaos_fn_arg_1);
	return result;
}

extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_0(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_1(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_2(void);
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_3(void);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestGenericInstantiation(void);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestSupplementalMetadataRoundtrip(void);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupBySubjectId(void);
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupByToken(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[10] = {
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
	::ChaosAbiMethodEntryV0 entries[9];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		9u,
		1u,
		2330568274u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // GenericSupplementSubjects_GenericSupplementSubjects_Subject_0
		{ 0u, 0u },  // GenericSupplementSubjects_GenericSupplementSubjects_Subject_1
		{ 0u, 0u },  // GenericSupplementSubjects_GenericSupplementSubjects_Subject_2
		{ 0u, 0u },  // GenericSupplementSubjects_GenericSupplementSubjects_Subject_3
		{ 1u, 1u },  // GenericSupplementSubjects_GenericSupplementNativeEntry_Run_System_Int32
		{ 1u, 0u },  // GenericSupplementSubjects_GenericSupplementNativeEntry_TestGenericInstantiation
		{ 1u, 0u },  // GenericSupplementSubjects_GenericSupplementNativeEntry_TestSupplementalMetadataRoundtrip
		{ 1u, 0u },  // GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupBySubjectId
		{ 1u, 0u },  // GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupByToken
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
		12u,
	};

	static constexpr const char* s_type_names[3] = {
		"<Module>",
		"GenericSupplementSubjects",
		"GenericSupplementNativeEntry",
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
		&chaos_mt_GenericSupplementSubjects_GenericSupplementSubjects,
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
		/* .name_utf8         = */ "GenericSupplementSubjects",
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
		::chaos::il2cpp::runtime_core::RegisterModule("GenericSupplementSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[9] = {
	{ "Run", 0x0000000Au, 1u },  // GenericSupplementNativeEntry
	{ "TestGenericInstantiation", 0x0000000Eu, 0u },  // GenericSupplementNativeEntry
	{ "TestSupplementalMetadataRoundtrip", 0x0000000Bu, 0u },  // GenericSupplementNativeEntry
	{ "TestTypeLookupBySubjectId", 0x0000000Cu, 0u },  // GenericSupplementNativeEntry
	{ "TestTypeLookupByToken", 0x0000000Du, 0u },  // GenericSupplementNativeEntry
	{ "Subject_0", 0x00000006u, 0u },  // GenericSupplementSubjects
	{ "Subject_1", 0x00000007u, 0u },  // GenericSupplementSubjects
	{ "Subject_2", 0x00000008u, 0u },  // GenericSupplementSubjects
	{ "Subject_3", 0x00000009u, 0u },  // GenericSupplementSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[2] = {
	{ "GenericSupplementNativeEntry", "", 0u, 5u },
	{ "GenericSupplementSubjects", "", 5u, 4u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[9] = {
	{ 0x00000006u, 0u },
	{ 0x00000007u, 1u },
	{ 0x00000008u, 2u },
	{ 0x00000009u, 3u },
	{ 0x0000000Au, 4u },
	{ 0x0000000Bu, 6u },
	{ 0x0000000Cu, 7u },
	{ 0x0000000Du, 8u },
	{ 0x0000000Eu, 5u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[9] = {
	{ reinterpret_cast<void*>(&GenericSupplementSubjects_GenericSupplementSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GenericSupplementSubjects::Subject_0
	{ reinterpret_cast<void*>(&GenericSupplementSubjects_GenericSupplementSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GenericSupplementSubjects::Subject_1
	{ reinterpret_cast<void*>(&GenericSupplementSubjects_GenericSupplementSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GenericSupplementSubjects::Subject_2
	{ reinterpret_cast<void*>(&GenericSupplementSubjects_GenericSupplementSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GenericSupplementSubjects::Subject_3
	{ reinterpret_cast<void*>(&GenericSupplementSubjects_GenericSupplementNativeEntry_Run_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GenericSupplementNativeEntry::Run
	{ reinterpret_cast<void*>(&GenericSupplementSubjects_GenericSupplementNativeEntry_TestGenericInstantiation), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GenericSupplementNativeEntry::TestGenericInstantiation
	{ reinterpret_cast<void*>(&GenericSupplementSubjects_GenericSupplementNativeEntry_TestSupplementalMetadataRoundtrip), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GenericSupplementNativeEntry::TestSupplementalMetadataRoundtrip
	{ reinterpret_cast<void*>(&GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupBySubjectId), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GenericSupplementNativeEntry::TestTypeLookupBySubjectId
	{ reinterpret_cast<void*>(&GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupByToken), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GenericSupplementNativeEntry::TestTypeLookupByToken
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"GenericSupplementSubjects",
	s_hotpatch_types,
	2u,
	s_hotpatch_methods,
	9u,
	s_hotpatch_slots,
	9u,
	s_hotpatch_entries,
	9u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[25] = {
	"GenericSupplementSubjects/GenericSupplementSubjects::_exitCode",
	"System.Collections/System.Collections.Generic.List<System.String>::.ctor:System.Void()",
	"System.Collections/System.Collections.Generic.List<System.String>::Add:System.Void(System.String)",
	"System.Collections/System.Collections.Generic.List<System.String>::get_Count:System.Int32()",
	"System.Private.CoreLib/System.Object::GetType:System.Type()",
	"System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()",
	"System.Private.CoreLib/System.Type::GetGenericTypeDefinition:System.Type()",
	"System.Collections/System.Collections.Generic.List`1",
	"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)",
	"System.Private.CoreLib/System.Type::op_Inequality:System.Boolean(System.Type,System.Type)",
	"System.Private.CoreLib/System.Type::GetGenericArguments:System.Type[]()",
	"System.Private.CoreLib/System.String",
	"System.Private.CoreLib/System.Int32::ToString:System.String()",
	"System.Private.CoreLib/System.String::Concat:System.String(System.String[])",
	"System.Private.CoreLib/System.String::Split:System.String[](System.Char,System.StringSplitOptions)",
	"System.Private.CoreLib/System.String::op_Inequality:System.Boolean(System.String,System.String)",
	"System.Private.CoreLib/System.Int32::Parse:System.Int32(System.String)",
	"System.Collections/System.Collections.Generic.List<System.Int32>",
	"System.Private.CoreLib/System.Type::get_FullName:System.String()",
	"System.Private.CoreLib/System.String::Contains:System.Boolean(System.String)",
	"System.Private.CoreLib/System.Type::GetType:System.Type(System.String)",
	"System.Private.CoreLib/System.Type::op_Equality:System.Boolean(System.Type,System.Type)",
	"System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Reflection.BindingFlags)",
	"System.Private.CoreLib/System.Reflection.MethodInfo::op_Equality:System.Boolean(System.Reflection.MethodInfo,System.Reflection.MethodInfo)",
	"System.Private.CoreLib/System.Reflection.MemberInfo::get_MetadataToken:System.Int32()",
};

extern "C" void* kChaosExternalRuntimeFnTable[25] = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_IsGenericType_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetGenericTypeDefinition_System_Type__),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__op_Inequality_System_Boolean_System_Type_System_Type_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetGenericArguments_System_Type____),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_String__Split_System_String___System_Char_System_StringSplitOptions_),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Int32__Parse_System_Int32_System_String_),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__get_FullName_System_String__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_String__Contains_System_Boolean_System_String_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetType_System_Type_System_String_),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethod_System_Reflection_MethodInfo_System_String_System_Reflection_BindingFlags_),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_MetadataToken_System_Int32__),
};

extern "C" int32_t kChaosExternalRuntimeCount = 25;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[9])() = {
	reinterpret_cast<void(*)()>(&GenericSupplementSubjects_GenericSupplementSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&GenericSupplementSubjects_GenericSupplementSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&GenericSupplementSubjects_GenericSupplementSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&GenericSupplementSubjects_GenericSupplementSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&GenericSupplementSubjects_GenericSupplementNativeEntry_Run_System_Int32),
	reinterpret_cast<void(*)()>(&GenericSupplementSubjects_GenericSupplementNativeEntry_TestGenericInstantiation),
	reinterpret_cast<void(*)()>(&GenericSupplementSubjects_GenericSupplementNativeEntry_TestSupplementalMetadataRoundtrip),
	reinterpret_cast<void(*)()>(&GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupBySubjectId),
	reinterpret_cast<void(*)()>(&GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupByToken),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[9])() = {
	[]() {kAotMethods[0]();},
	[]() {kAotMethods[1]();},
	[]() {kAotMethods[2]();},
	[]() {kAotMethods[3]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[4])(0);},
	[]() {kAotMethods[5]();},
	[]() {kAotMethods[6]();},
	[]() {kAotMethods[7]();},
	[]() {kAotMethods[8]();},
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 4;
extern "C" const int kSubjectEntryIndices[4] = {
	0,
	1,
	2,
	3,
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

	/* ── Entry 0: GenericSupplementSubjects_GenericSupplementSubjects_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry0;
	/* ── Entry 1: GenericSupplementSubjects_GenericSupplementSubjects_Subject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry1;
	/* ── Entry 2: GenericSupplementSubjects_GenericSupplementSubjects_Subject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry2;
	/* ── Entry 3: GenericSupplementSubjects_GenericSupplementSubjects_Subject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry3;
	/* ── Entry 4: GenericSupplementSubjects_GenericSupplementNativeEntry_Run_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry4;
	/* ── Entry 5: GenericSupplementSubjects_GenericSupplementNativeEntry_TestGenericInstantiation ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[14];
	} entry5;
	/* ── Entry 6: GenericSupplementSubjects_GenericSupplementNativeEntry_TestSupplementalMetadataRoundtrip ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[15];
	} entry6;
	/* ── Entry 7: GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupBySubjectId ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[9];
	} entry7;
	/* ── Entry 8: GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupByToken ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[10];
	} entry8;
} kChaosGcSlotMapsSection = {
	/* entry0 = GenericSupplementSubjects_GenericSupplementSubjects_Subject_0 */
	.entry0 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&GenericSupplementSubjects_GenericSupplementSubjects_Subject_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry1 = GenericSupplementSubjects_GenericSupplementSubjects_Subject_1 */
	.entry1 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&GenericSupplementSubjects_GenericSupplementSubjects_Subject_1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry2 = GenericSupplementSubjects_GenericSupplementSubjects_Subject_2 */
	.entry2 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&GenericSupplementSubjects_GenericSupplementSubjects_Subject_2),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry3 = GenericSupplementSubjects_GenericSupplementSubjects_Subject_3 */
	.entry3 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&GenericSupplementSubjects_GenericSupplementSubjects_Subject_3),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry4 = GenericSupplementSubjects_GenericSupplementNativeEntry_Run_System_Int32 */
	.entry4 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&GenericSupplementSubjects_GenericSupplementNativeEntry_Run_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 8u, 16u, 24u }
	},
	/* entry5 = GenericSupplementSubjects_GenericSupplementNativeEntry_TestGenericInstantiation */
	.entry5 = {
		/* entry_total_size = 76 */ 76u,
		/* code_address */ reinterpret_cast<const void*>(&GenericSupplementSubjects_GenericSupplementNativeEntry_TestGenericInstantiation),
		/* frame_size = 112 */ 112u,
		/* num_gc_slots = 14 */ 14u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u, 80u, 88u, 96u, 104u }
	},
	/* entry6 = GenericSupplementSubjects_GenericSupplementNativeEntry_TestSupplementalMetadataRoundtrip */
	.entry6 = {
		/* entry_total_size = 80 */ 80u,
		/* code_address */ reinterpret_cast<const void*>(&GenericSupplementSubjects_GenericSupplementNativeEntry_TestSupplementalMetadataRoundtrip),
		/* frame_size = 120 */ 120u,
		/* num_gc_slots = 15 */ 15u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u, 80u, 88u, 96u, 104u, 112u }
	},
	/* entry7 = GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupBySubjectId */
	.entry7 = {
		/* entry_total_size = 56 */ 56u,
		/* code_address */ reinterpret_cast<const void*>(&GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupBySubjectId),
		/* frame_size = 72 */ 72u,
		/* num_gc_slots = 9 */ 9u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u }
	},
	/* entry8 = GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupByToken */
	.entry8 = {
		/* entry_total_size = 60 */ 60u,
		/* code_address */ reinterpret_cast<const void*>(&GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupByToken),
		/* frame_size = 80 */ 80u,
		/* num_gc_slots = 10 */ 10u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 432u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[9] = {
	reinterpret_cast<void*>(&GenericSupplementSubjects_GenericSupplementSubjects_Subject_0),
	reinterpret_cast<void*>(&GenericSupplementSubjects_GenericSupplementSubjects_Subject_1),
	reinterpret_cast<void*>(&GenericSupplementSubjects_GenericSupplementSubjects_Subject_2),
	reinterpret_cast<void*>(&GenericSupplementSubjects_GenericSupplementSubjects_Subject_3),
	reinterpret_cast<void*>(&GenericSupplementSubjects_GenericSupplementNativeEntry_Run_System_Int32),
	reinterpret_cast<void*>(&GenericSupplementSubjects_GenericSupplementNativeEntry_TestGenericInstantiation),
	reinterpret_cast<void*>(&GenericSupplementSubjects_GenericSupplementNativeEntry_TestSupplementalMetadataRoundtrip),
	reinterpret_cast<void*>(&GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupBySubjectId),
	reinterpret_cast<void*>(&GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupByToken),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 9u,
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
	.image_name_utf8    = "GenericSupplementSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_GenericSupplementSubjects_GenericSupplementSubjects[1] = {
	{ 0x04000001u, "GenericSupplementSubjects/GenericSupplementSubjects::_exitCode", "_exitCode", "System.Int32", 0LL, 3u },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_GenericSupplementSubjects_GenericSupplementSubjects[4] = {
	{ 0x00000006u, "GenericSupplementSubjects/GenericSupplementSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000007u, "GenericSupplementSubjects/GenericSupplementSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000008u, "GenericSupplementSubjects/GenericSupplementSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000009u, "GenericSupplementSubjects/GenericSupplementSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryFieldDescriptor kReflFields_GenericSupplementSubjects_GenericSupplementNativeEntry[2] = {
	{ 0x04000002u, "GenericSupplementSubjects/GenericSupplementNativeEntry::TestSubjectId", "TestSubjectId", "System.Int32", 0LL, 10u },
	{ 0x04000003u, "GenericSupplementSubjects/GenericSupplementNativeEntry::TestNamingConcept", "TestNamingConcept", "System.Int32", 0LL, 10u },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_GenericSupplementSubjects_GenericSupplementNativeEntry[5] = {
	{ 0x0000000Au, "GenericSupplementSubjects/GenericSupplementNativeEntry::Run:System.Int32(System.Int32)", "Run", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Eu, "GenericSupplementSubjects/GenericSupplementNativeEntry::TestGenericInstantiation:System.Int32()", "TestGenericInstantiation", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Bu, "GenericSupplementSubjects/GenericSupplementNativeEntry::TestSupplementalMetadataRoundtrip:System.Int32()", "TestSupplementalMetadataRoundtrip", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Cu, "GenericSupplementSubjects/GenericSupplementNativeEntry::TestTypeLookupBySubjectId:System.Int32()", "TestTypeLookupBySubjectId", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Du, "GenericSupplementSubjects/GenericSupplementNativeEntry::TestTypeLookupByToken:System.Int32()", "TestTypeLookupByToken", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[2] = {
	{ 0x00000001u, "GenericSupplementSubjects/GenericSupplementSubjects", "GenericSupplementSubjects/GenericSupplementSubjects", "", "GenericSupplementSubjects", "GenericSupplementSubjects", nullptr, kReflFields_GenericSupplementSubjects_GenericSupplementSubjects, 1u, nullptr, 0u,
nullptr, 0u,    kReflMethods_GenericSupplementSubjects_GenericSupplementSubjects, 4u, nullptr, 0u, 0u },
	{ 0x00000002u, "GenericSupplementSubjects/GenericSupplementNativeEntry", "GenericSupplementSubjects/GenericSupplementNativeEntry", "", "GenericSupplementNativeEntry", "GenericSupplementNativeEntry", nullptr, kReflFields_GenericSupplementSubjects_GenericSupplementNativeEntry, 2u, nullptr, 0u,
nullptr, 0u,    kReflMethods_GenericSupplementSubjects_GenericSupplementNativeEntry, 5u, nullptr, 0u, 0u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[2] = {
	&kReflTypes[0],
	&kReflTypes[1],
};

static const ReflectionQueryImageDescriptor kReflImage = { "GenericSupplementSubjects", kReflTypePtrs, 2u, 1, 0, 0, 0 };

// Fake ImageHandle that ResolveSubjectId will decode back to kReflImage.
// BootstrapRuntime's aot_image_handle fallback discovers this via
// LookupModule(mid)->image at lines 311-321 of bootstrap.cpp.

// ── AOT constexpr EEClass instances (Phase 2b) ──
// Each EEClass aliases the constexpr ReflectionQuery*Descriptor arrays
// emitted above, providing zero-copy reflection metadata without
// the O(n) reverse lookup + domain heap allocation in EnsureEEClass().

inline EEClass kEEClass_GenericSupplementSubjects_GenericSupplementSubjects = {
	/*name_utf8=*/       "GenericSupplementSubjects",
	/*namespace_utf8=*/  "",
	/*mt=*/              nullptr,
	/*methods=*/         {kReflMethods_GenericSupplementSubjects_GenericSupplementSubjects, 4u, true},
	/*fields=*/          {kReflFields_GenericSupplementSubjects_GenericSupplementSubjects, 1u, true},
	/*properties=*/      {nullptr, 0u, true},
	/*events=*/          {nullptr, 0u, true},
	/*generic_type_def=*/ nullptr,
	/*generic_param_count=*/ 0u,
};

inline EEClass kEEClass_GenericSupplementSubjects_GenericSupplementNativeEntry = {
	/*name_utf8=*/       "GenericSupplementNativeEntry",
	/*namespace_utf8=*/  "",
	/*mt=*/              nullptr,
	/*methods=*/         {kReflMethods_GenericSupplementSubjects_GenericSupplementNativeEntry, 5u, true},
	/*fields=*/          {kReflFields_GenericSupplementSubjects_GenericSupplementNativeEntry, 2u, true},
	/*properties=*/      {nullptr, 0u, true},
	/*events=*/          {nullptr, 0u, true},
	/*generic_type_def=*/ nullptr,
	/*generic_param_count=*/ 0u,
};

inline EEClass kEEClass_System_Private_CoreLib_System_Object = {
	/*name_utf8=*/       "System.Object",
	/*namespace_utf8=*/  "System",
	/*mt=*/              nullptr,
	/*methods=*/         {nullptr, 0u, true},
	/*fields=*/          {nullptr, 0u, true},
	/*properties=*/      {nullptr, 0u, true},
	/*events=*/          {nullptr, 0u, true},
	/*generic_type_def=*/ nullptr,
	/*generic_param_count=*/ 0u,
};

inline EEClass kEEClass_System_Collections_System_Collections_Generic_List_System_String_ = {
	/*name_utf8=*/       "System.Collections.Generic.List<System.String>",
	/*namespace_utf8=*/  "System.Collections.Generic.List<System",
	/*mt=*/              nullptr,
	/*methods=*/         {nullptr, 0u, true},
	/*fields=*/          {nullptr, 0u, true},
	/*properties=*/      {nullptr, 0u, true},
	/*events=*/          {nullptr, 0u, true},
	/*generic_type_def=*/ nullptr,
	/*generic_param_count=*/ 0u,
};

inline EEClass kEEClass_System_Private_CoreLib_System_String = {
	/*name_utf8=*/       "System.String",
	/*namespace_utf8=*/  "System",
	/*mt=*/              nullptr,
	/*methods=*/         {nullptr, 0u, true},
	/*fields=*/          {nullptr, 0u, true},
	/*properties=*/      {nullptr, 0u, true},
	/*events=*/          {nullptr, 0u, true},
	/*generic_type_def=*/ nullptr,
	/*generic_param_count=*/ 0u,
};

inline EEClass kEEClass_System_Private_CoreLib_System_Type = {
	/*name_utf8=*/       "System.Type",
	/*namespace_utf8=*/  "System",
	/*mt=*/              nullptr,
	/*methods=*/         {nullptr, 0u, true},
	/*fields=*/          {nullptr, 0u, true},
	/*properties=*/      {nullptr, 0u, true},
	/*events=*/          {nullptr, 0u, true},
	/*generic_type_def=*/ nullptr,
	/*generic_param_count=*/ 0u,
};

inline EEClass kEEClass_System_Private_CoreLib_System_Reflection_MethodInfo = {
	/*name_utf8=*/       "System.Reflection.MethodInfo",
	/*namespace_utf8=*/  "System.Reflection",
	/*mt=*/              nullptr,
	/*methods=*/         {nullptr, 0u, true},
	/*fields=*/          {nullptr, 0u, true},
	/*properties=*/      {nullptr, 0u, true},
	/*events=*/          {nullptr, 0u, true},
	/*generic_type_def=*/ nullptr,
	/*generic_param_count=*/ 0u,
};

inline EEClass kEEClass_System_Private_CoreLib_System_Reflection_ConstructorInfo = {
	/*name_utf8=*/       "System.Reflection.ConstructorInfo",
	/*namespace_utf8=*/  "System.Reflection",
	/*mt=*/              nullptr,
	/*methods=*/         {nullptr, 0u, true},
	/*fields=*/          {nullptr, 0u, true},
	/*properties=*/      {nullptr, 0u, true},
	/*events=*/          {nullptr, 0u, true},
	/*generic_type_def=*/ nullptr,
	/*generic_param_count=*/ 0u,
};

inline EEClass kEEClass_System_Private_CoreLib_System_Reflection_FieldInfo = {
	/*name_utf8=*/       "System.Reflection.FieldInfo",
	/*namespace_utf8=*/  "System.Reflection",
	/*mt=*/              nullptr,
	/*methods=*/         {nullptr, 0u, true},
	/*fields=*/          {nullptr, 0u, true},
	/*properties=*/      {nullptr, 0u, true},
	/*events=*/          {nullptr, 0u, true},
	/*generic_type_def=*/ nullptr,
	/*generic_param_count=*/ 0u,
};

inline EEClass kEEClass_System_Private_CoreLib_System_Reflection_Assembly = {
	/*name_utf8=*/       "System.Reflection.Assembly",
	/*namespace_utf8=*/  "System.Reflection",
	/*mt=*/              nullptr,
	/*methods=*/         {nullptr, 0u, true},
	/*fields=*/          {nullptr, 0u, true},
	/*properties=*/      {nullptr, 0u, true},
	/*events=*/          {nullptr, 0u, true},
	/*generic_type_def=*/ nullptr,
	/*generic_param_count=*/ 0u,
};

inline EEClass kEEClass_System_Private_CoreLib_System_Reflection_AssemblyName = {
	/*name_utf8=*/       "System.Reflection.AssemblyName",
	/*namespace_utf8=*/  "System.Reflection",
	/*mt=*/              nullptr,
	/*methods=*/         {nullptr, 0u, true},
	/*fields=*/          {nullptr, 0u, true},
	/*properties=*/      {nullptr, 0u, true},
	/*events=*/          {nullptr, 0u, true},
	/*generic_type_def=*/ nullptr,
	/*generic_param_count=*/ 0u,
};


inline MethodTable chaos_mt_GenericSupplementSubjects_GenericSupplementNativeEntry = {0};

inline TypeInfoV0 chaos_mt_GenericSupplementSubjects_GenericSupplementNativeEntry = {};
inline TypeInfoV0 chaos_mt_GenericSupplementSubjects_GenericSupplementSubjects = {};
inline TypeInfoV0 chaos_mt_System_Collections_System_Collections_Generic_List_System_String_ = {};
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {};
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {};
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName = {};
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo = {};
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo = {};
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo = {};
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_String = {};
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Type = {};
extern "C" void ChaosRegisterAotEEClasses() {
	// GenericSupplementSubjects/GenericSupplementSubjects
	chaos_mt_GenericSupplementSubjects_GenericSupplementSubjects.warm.cold_delta = static_cast<uint32_t>(
		reinterpret_cast<uint8_t*>(&kEEClass_GenericSupplementSubjects_GenericSupplementSubjects) - reinterpret_cast<uint8_t*>(&chaos_mt_GenericSupplementSubjects_GenericSupplementSubjects));
	kEEClass_GenericSupplementSubjects_GenericSupplementSubjects.mt = reinterpret_cast<MethodTable*>(&chaos_mt_GenericSupplementSubjects_GenericSupplementSubjects);

	// GenericSupplementSubjects/GenericSupplementNativeEntry
	chaos_mt_GenericSupplementSubjects_GenericSupplementNativeEntry.warm.cold_delta = static_cast<uint32_t>(
		reinterpret_cast<uint8_t*>(&kEEClass_GenericSupplementSubjects_GenericSupplementNativeEntry) - reinterpret_cast<uint8_t*>(&chaos_mt_GenericSupplementSubjects_GenericSupplementNativeEntry));
	kEEClass_GenericSupplementSubjects_GenericSupplementNativeEntry.mt = reinterpret_cast<MethodTable*>(&chaos_mt_GenericSupplementSubjects_GenericSupplementNativeEntry);

	// System.Private.CoreLib/System.Object
	chaos_mt_System_Private_CoreLib_System_Object.warm.cold_delta = static_cast<uint32_t>(
		reinterpret_cast<uint8_t*>(&kEEClass_System_Private_CoreLib_System_Object) - reinterpret_cast<uint8_t*>(&chaos_mt_System_Private_CoreLib_System_Object));
	kEEClass_System_Private_CoreLib_System_Object.mt = reinterpret_cast<MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object);

	// System.Collections/System.Collections.Generic.List<System.String>
	chaos_mt_System_Collections_System_Collections_Generic_List_System_String_.warm.cold_delta = static_cast<uint32_t>(
		reinterpret_cast<uint8_t*>(&kEEClass_System_Collections_System_Collections_Generic_List_System_String_) - reinterpret_cast<uint8_t*>(&chaos_mt_System_Collections_System_Collections_Generic_List_System_String_));
	kEEClass_System_Collections_System_Collections_Generic_List_System_String_.mt = reinterpret_cast<MethodTable*>(&chaos_mt_System_Collections_System_Collections_Generic_List_System_String_);

	// System.Private.CoreLib/System.String
	chaos_mt_System_Private_CoreLib_System_String.warm.cold_delta = static_cast<uint32_t>(
		reinterpret_cast<uint8_t*>(&kEEClass_System_Private_CoreLib_System_String) - reinterpret_cast<uint8_t*>(&chaos_mt_System_Private_CoreLib_System_String));
	kEEClass_System_Private_CoreLib_System_String.mt = reinterpret_cast<MethodTable*>(&chaos_mt_System_Private_CoreLib_System_String);

	// System.Private.CoreLib/System.Type
	chaos_mt_System_Private_CoreLib_System_Type.warm.cold_delta = static_cast<uint32_t>(
		reinterpret_cast<uint8_t*>(&kEEClass_System_Private_CoreLib_System_Type) - reinterpret_cast<uint8_t*>(&chaos_mt_System_Private_CoreLib_System_Type));
	kEEClass_System_Private_CoreLib_System_Type.mt = reinterpret_cast<MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Type);

	// System.Private.CoreLib/System.Reflection.MethodInfo
	chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo.warm.cold_delta = static_cast<uint32_t>(
		reinterpret_cast<uint8_t*>(&kEEClass_System_Private_CoreLib_System_Reflection_MethodInfo) - reinterpret_cast<uint8_t*>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo));
	kEEClass_System_Private_CoreLib_System_Reflection_MethodInfo.mt = reinterpret_cast<MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo);

	// System.Private.CoreLib/System.Reflection.ConstructorInfo
	chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo.warm.cold_delta = static_cast<uint32_t>(
		reinterpret_cast<uint8_t*>(&kEEClass_System_Private_CoreLib_System_Reflection_ConstructorInfo) - reinterpret_cast<uint8_t*>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo));
	kEEClass_System_Private_CoreLib_System_Reflection_ConstructorInfo.mt = reinterpret_cast<MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo);

	// System.Private.CoreLib/System.Reflection.FieldInfo
	chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo.warm.cold_delta = static_cast<uint32_t>(
		reinterpret_cast<uint8_t*>(&kEEClass_System_Private_CoreLib_System_Reflection_FieldInfo) - reinterpret_cast<uint8_t*>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo));
	kEEClass_System_Private_CoreLib_System_Reflection_FieldInfo.mt = reinterpret_cast<MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo);

	// System.Private.CoreLib/System.Reflection.Assembly
	chaos_mt_System_Private_CoreLib_System_Reflection_Assembly.warm.cold_delta = static_cast<uint32_t>(
		reinterpret_cast<uint8_t*>(&kEEClass_System_Private_CoreLib_System_Reflection_Assembly) - reinterpret_cast<uint8_t*>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly));
	kEEClass_System_Private_CoreLib_System_Reflection_Assembly.mt = reinterpret_cast<MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly);

	// System.Private.CoreLib/System.Reflection.AssemblyName
	chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName.warm.cold_delta = static_cast<uint32_t>(
		reinterpret_cast<uint8_t*>(&kEEClass_System_Private_CoreLib_System_Reflection_AssemblyName) - reinterpret_cast<uint8_t*>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName));
	kEEClass_System_Private_CoreLib_System_Reflection_AssemblyName.mt = reinterpret_cast<MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName);

}


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
// Managed method: GenericSupplementSubjects/GenericSupplementSubjects::Subject_0()
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_0(void)
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
					_d_hpresult = GenericSupplementSubjects_GenericSupplementNativeEntry_TestSupplementalMetadataRoundtrip();
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
					_d_hpresult = GenericSupplementSubjects_GenericSupplementNativeEntry_TestSupplementalMetadataRoundtrip();
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
				chaos_static_GenericSupplementSubjects_GenericSupplementSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_GenericSupplementSubjects_GenericSupplementSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GenericSupplementSubjects/GenericSupplementSubjects::Subject_1()
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_1(void)
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
					_d_hpresult = GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupBySubjectId();
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
					_d_hpresult = GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupBySubjectId();
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
				chaos_static_GenericSupplementSubjects_GenericSupplementSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_GenericSupplementSubjects_GenericSupplementSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GenericSupplementSubjects/GenericSupplementSubjects::Subject_2()
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_2(void)
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
					_d_hpresult = GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupByToken();
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
					_d_hpresult = GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupByToken();
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
				chaos_static_GenericSupplementSubjects_GenericSupplementSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_GenericSupplementSubjects_GenericSupplementSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GenericSupplementSubjects/GenericSupplementSubjects::Subject_3()
extern "C" void GenericSupplementSubjects_GenericSupplementSubjects_Subject_3(void)
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


	CHAOS_EH_TRY
			{
				auto& _d5 = s_hotpatch_entries[5];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d5)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d5))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d5.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = GenericSupplementSubjects_GenericSupplementNativeEntry_TestGenericInstantiation();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
			{
				auto& _d5 = s_hotpatch_entries[5];
				CHAOS_IL2CPP_INT32 _d_hpresult{};
				if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d5)
					&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d5))
				{
					::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
						_d5.method_key, nullptr, &_d_hpresult);
				}
				else
				{
					_d_hpresult = GenericSupplementSubjects_GenericSupplementNativeEntry_TestGenericInstantiation();
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
				chaos_static_GenericSupplementSubjects_GenericSupplementSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_GenericSupplementSubjects_GenericSupplementSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GenericSupplementSubjects/GenericSupplementNativeEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		if (_s0 != 0)
		{
		}
		_s0 = chaos_args[0];
		{
			const auto chaos_switch_value = static_cast<CHAOS_IL2CPP_INT32>(_s0);
			switch (chaos_switch_value)
			{
				case 0:
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
							_d_hpresult = GenericSupplementSubjects_GenericSupplementNativeEntry_TestSupplementalMetadataRoundtrip();
						}
						_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
					}
					chaos_locals[0] = _s0;
				case 1:
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
							_d_hpresult = GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupBySubjectId();
						}
						_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
					}
					chaos_locals[0] = _s0;
				case 2:
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
							_d_hpresult = GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupByToken();
						}
						_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
					}
					chaos_locals[0] = _s0;
				case 3:
				{
					{
						auto& _d5 = s_hotpatch_entries[5];
						CHAOS_IL2CPP_INT32 _d_hpresult{};
						if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d5)
							&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d5))
						{
							::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
								_d5.method_key, nullptr, &_d_hpresult);
						}
						else
						{
							_d_hpresult = GenericSupplementSubjects_GenericSupplementNativeEntry_TestGenericInstantiation();
						}
						_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
					}
					chaos_locals[0] = _s0;
					break;
				}
				default:
				{
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
					chaos_locals[0] = _s0;
					break;
				}
			}
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		{
			if (_s0 != 0)
			{
			}
			_s0 = chaos_locals[0];
			chaos_locals[1] = _s0;
			_s0 = chaos_locals[1];
			return static_cast<CHAOS_IL2CPP_INT32>(_s0);
		}
	}
}

// Managed method: GenericSupplementSubjects/GenericSupplementNativeEntry::TestGenericInstantiation()
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestGenericInstantiation(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10) chaos_locals{};
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
	CHAOS_IL2CPP_INTPTR _s25{};
	CHAOS_IL2CPP_INTPTR _s26{};
	CHAOS_IL2CPP_INTPTR _s27{};
	CHAOS_IL2CPP_INTPTR _s28{};
	CHAOS_IL2CPP_INTPTR _s29{};
	CHAOS_IL2CPP_INTPTR _s30{};
	CHAOS_IL2CPP_INTPTR _s31{};
	CHAOS_IL2CPP_INTPTR _s32{};
	CHAOS_IL2CPP_INTPTR _s33{};
	CHAOS_IL2CPP_INTPTR _s34{};
	CHAOS_IL2CPP_INTPTR _s35{};
	CHAOS_IL2CPP_INTPTR _s36{};
	CHAOS_IL2CPP_INTPTR _s37{};
	CHAOS_IL2CPP_INTPTR _s38{};
	CHAOS_IL2CPP_INTPTR _s39{};
	CHAOS_IL2CPP_INTPTR _s40{};
	CHAOS_IL2CPP_INTPTR _s41{};
	CHAOS_IL2CPP_INTPTR _s42{};
	CHAOS_IL2CPP_INTPTR _s43{};
	CHAOS_IL2CPP_INTPTR _s44{};
	CHAOS_IL2CPP_INTPTR _s45{};
	CHAOS_IL2CPP_INTPTR _s46{};
	CHAOS_IL2CPP_INTPTR _s47{};
	CHAOS_IL2CPP_INTPTR _s48{};
	CHAOS_IL2CPP_INTPTR _s49{};
	CHAOS_IL2CPP_INTPTR _s50{};
	CHAOS_IL2CPP_INTPTR _s51{};
	CHAOS_IL2CPP_INTPTR _s52{};
	CHAOS_IL2CPP_INTPTR _s53{};
	CHAOS_IL2CPP_INTPTR _s54{};
	CHAOS_IL2CPP_INTPTR _s55{};


	CHAOS_EH_TRY
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Collections_System_Collections_Generic_List_System_String_, {});
				chaos_object->header.type_info = &chaos_mt_System_Collections_System_Collections_Generic_List_System_String_;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s1 = _s0;
			{{
				_s2 = CHAOS_IL2CPP_STRING_ID("hot");
			}}
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_List_System_String___Add_System_Void_System_String_(chaos_arg_0, chaos_arg_1);
			}
			_s1 = _s0;
			{{
				_s2 = CHAOS_IL2CPP_STRING_ID("update");
			}}
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_List_System_String___Add_System_Void_System_String_(chaos_arg_0, chaos_arg_1);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("ok");
			}}
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_List_System_String___Add_System_Void_System_String_(chaos_arg_0, chaos_arg_1);
			}
			_s0 = chaos_locals[0];
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[3])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
			{
				if (_s1 == 0)
				{
					_s14 = chaos_locals[0];
					{
						const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[4])();
						_s15 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
					}
					{
						const auto chaos_arg_0 = _s15;
						const auto chaos_result = ChaosReflectionGetIsGenericType(chaos_arg_0);
						_s15 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
					}
					_s16 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
					_s15 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s15) == static_cast<CHAOS_IL2CPP_INTPTR>(_s16) ? 1 : 0);
					{
						if (_s15 == 0)
						{
							_s4 = chaos_locals[1];
							{
								const auto chaos_arg_0 = _s4;
								const auto chaos_result = ChaosReflectionGetGenericTypeDefinition(chaos_arg_0);
								_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
							}
							_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(45047970u);
							{
								const auto chaos_arg_0 = _s5;
								const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
								_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
							}
							{
								const auto chaos_arg_1 = _s5;
								const auto chaos_arg_0 = _s4;
								const auto chaos_result = ChaosTypeInequality(chaos_arg_0, chaos_arg_1);
								_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
							}
							{
								if (_s4 == 0)
								{
									_s4 = chaos_locals[1];
									{
										const auto chaos_arg_0 = _s4;
										const auto chaos_result = ChaosReflectionGetGenericArguments(chaos_arg_0);
										_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
									}
									_s4 = [&](){ auto* _c_arr = reinterpret_cast<chaos_managed_array*>(_s4); return _c_arr ? static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_c_arr->length)) : CHAOS_IL2CPP_INTPTR{}; }();
									_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s4));
									_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
									_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
									_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
									_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
									{
										if (_s4 == 0)
										{
											_s7 = chaos_locals[3];
											_s8 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
											{
												const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s8);
												auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s7);
												if (chaos_array == nullptr)
												{
													CHAOS_IL2CPP_FAIL();
												}
												if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
												{
													CHAOS_IL2CPP_FAIL();
												}
												const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
												_s7 = chaos_element;
											}
											_s8 = static_cast<CHAOS_IL2CPP_INTPTR>(49037434u);
											{
												const auto chaos_arg_0 = _s8;
												const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
												_s8 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
											}
											{
												const auto chaos_arg_1 = _s8;
												const auto chaos_arg_0 = _s7;
												const auto chaos_result = ChaosTypeInequality(chaos_arg_0, chaos_arg_1);
												_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
											}
											{
												if (_s7 == 0)
												{
													_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
													chaos_locals[5] = _s7;
												}
												else
												{
													_s12 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
													chaos_locals[5] = _s12;
												}
											}
										}
										else
										{
											_s11 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
											chaos_locals[5] = _s11;
										}
									}
								}
								else
								{
									_s8 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
									chaos_locals[5] = _s8;
								}
							}
						}
						else
						{
							_s18 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
							chaos_locals[5] = _s18;
						}
					}
				}
				else
				{
					_s14 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
					chaos_locals[5] = _s14;
				}
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			chaos_locals[5] = _s0;
	CHAOS_EH_END
	_s0 = chaos_locals[5];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: GenericSupplementSubjects/GenericSupplementNativeEntry::TestSupplementalMetadataRoundtrip()
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestSupplementalMetadataRoundtrip(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 10) chaos_locals{};
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
	CHAOS_IL2CPP_INTPTR _s25{};
	CHAOS_IL2CPP_INTPTR _s26{};
	CHAOS_IL2CPP_INTPTR _s27{};
	CHAOS_IL2CPP_INTPTR _s28{};
	CHAOS_IL2CPP_INTPTR _s29{};
	CHAOS_IL2CPP_INTPTR _s30{};
	CHAOS_IL2CPP_INTPTR _s31{};
	CHAOS_IL2CPP_INTPTR _s32{};
	CHAOS_IL2CPP_INTPTR _s33{};
	CHAOS_IL2CPP_INTPTR _s34{};
	CHAOS_IL2CPP_INTPTR _s35{};
	CHAOS_IL2CPP_INTPTR _s36{};
	CHAOS_IL2CPP_INTPTR _s37{};
	CHAOS_IL2CPP_INTPTR _s38{};
	CHAOS_IL2CPP_INTPTR _s39{};
	CHAOS_IL2CPP_INTPTR _s40{};
	CHAOS_IL2CPP_INTPTR _s41{};
	CHAOS_IL2CPP_INTPTR _s42{};
	CHAOS_IL2CPP_INTPTR _s43{};
	CHAOS_IL2CPP_INTPTR _s44{};
	CHAOS_IL2CPP_INTPTR _s45{};
	CHAOS_IL2CPP_INTPTR _s46{};
	CHAOS_IL2CPP_INTPTR _s47{};
	CHAOS_IL2CPP_INTPTR _s48{};
	CHAOS_IL2CPP_INTPTR _s49{};
	CHAOS_IL2CPP_INTPTR _s50{};
	CHAOS_IL2CPP_INTPTR _s51{};
	CHAOS_IL2CPP_INTPTR _s52{};
	CHAOS_IL2CPP_INTPTR _s53{};
	CHAOS_IL2CPP_INTPTR _s54{};
	CHAOS_IL2CPP_INTPTR _s55{};
	CHAOS_IL2CPP_INTPTR _s56{};
	CHAOS_IL2CPP_INTPTR _s57{};
	CHAOS_IL2CPP_INTPTR _s58{};
	CHAOS_IL2CPP_INTPTR _s59{};
	CHAOS_IL2CPP_INTPTR _s60{};
	CHAOS_IL2CPP_INTPTR _s61{};
	CHAOS_IL2CPP_INTPTR _s62{};
	CHAOS_IL2CPP_INTPTR _s63{};
	CHAOS_IL2CPP_INTPTR _s64{};


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("System.Collections/System.Collections.Generic.List<System.Int32>");
			}}
			chaos_locals[0] = _s0;
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("System.Collections.Generic.List`1");
			}}
			chaos_locals[1] = _s0;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(33554433);
			chaos_locals[2] = _s0;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
			{
				const auto chaos_length = static_cast<CHAOS_IL2CPP_INT32>(_s0);
				if (chaos_length < 0)
				{
					CHAOS_IL2CPP_FAIL();
				}
				auto* chaos_array = CHAOS_IL2CPP_NEW_GC(chaos_managed_array, {});
				chaos_array->header.type_info = &chaos_type_info_managed_array.hot;
				chaos_array->element_type_shape = 1;
				chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_String;
				chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
				chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
			}
			_s1 = _s0;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s3 = chaos_locals[0];
			{
				auto chaos_value_raw = _s3;
				if (chaos_is_string_id(chaos_value_raw))
				{
					chaos_value_raw = chaos_string_materialize(chaos_value_raw);
				}
				auto chaos_value = chaos_value_raw;
				const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
				auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
				if (chaos_array == nullptr)
				{
					CHAOS_IL2CPP_FAIL();
				}
				if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
				{
					CHAOS_IL2CPP_FAIL();
				}
				if (!chaos_is_array_store_compatible(chaos_array, chaos_value))
				{
					CHAOS_IL2CPP_FAIL();
				}
				BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)]));
				chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
				GC_END_STUBBORN_CHANGE(chaos_array);
				chaos_gc_dirty_card(chaos_array);
			}
			_s1 = _s0;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{{
				_s3 = CHAOS_IL2CPP_STRING_ID("|");
			}}
			{
				auto chaos_value_raw = _s3;
				if (chaos_is_string_id(chaos_value_raw))
				{
					chaos_value_raw = chaos_string_materialize(chaos_value_raw);
				}
				auto chaos_value = chaos_value_raw;
				const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
				auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
				BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)]));
				chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
				GC_END_STUBBORN_CHANGE(chaos_array);
				chaos_gc_dirty_card(chaos_array);
			}
			_s1 = _s0;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
			_s3 = chaos_locals[1];
			{
				auto chaos_value_raw = _s3;
				if (chaos_is_string_id(chaos_value_raw))
				{
					chaos_value_raw = chaos_string_materialize(chaos_value_raw);
				}
				auto chaos_value = chaos_value_raw;
				const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
				auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
				BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)]));
				chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
				GC_END_STUBBORN_CHANGE(chaos_array);
				chaos_gc_dirty_card(chaos_array);
			}
			_s1 = _s0;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
			{{
				_s3 = CHAOS_IL2CPP_STRING_ID("|");
			}}
			{
				auto chaos_value_raw = _s3;
				if (chaos_is_string_id(chaos_value_raw))
				{
					chaos_value_raw = chaos_string_materialize(chaos_value_raw);
				}
				auto chaos_value = chaos_value_raw;
				const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
				auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
				BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)]));
				chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
				GC_END_STUBBORN_CHANGE(chaos_array);
				chaos_gc_dirty_card(chaos_array);
			}
			_s1 = _s0;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
			_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[2]);
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				auto chaos_value_raw = _s3;
				if (chaos_is_string_id(chaos_value_raw))
				{
					chaos_value_raw = chaos_string_materialize(chaos_value_raw);
				}
				auto chaos_value = chaos_value_raw;
				const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
				auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
				BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)]));
				chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
				GC_END_STUBBORN_CHANGE(chaos_array);
				chaos_gc_dirty_card(chaos_array);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[13])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(124);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__Split_System_String___System_Char_System_StringSplitOptions_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2));
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = [&](){ auto* _c_arr = reinterpret_cast<chaos_managed_array*>(_s0); return _c_arr ? static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_c_arr->length)) : CHAOS_IL2CPP_INTPTR{}; }();
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			{
				if (_s0 == 0)
				{
					_s17 = chaos_locals[4];
					_s18 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
					{
						const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s18);
						auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s17);
						if (chaos_array == nullptr)
						{
							CHAOS_IL2CPP_FAIL();
						}
						if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
						{
							CHAOS_IL2CPP_FAIL();
						}
						const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
						_s17 = chaos_element;
					}
					_s18 = chaos_locals[0];
					{
						const auto chaos_arg_1 = _s18;
						const auto chaos_arg_0 = _s17;
						const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[15])(chaos_arg_0, chaos_arg_1);
						_s17 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
					}
					{
						if (_s17 == 0)
						{
							_s4 = chaos_locals[4];
							_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
							{
								const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s5);
								auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s4);
								if (chaos_array == nullptr)
								{
									CHAOS_IL2CPP_FAIL();
								}
								if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
								{
									CHAOS_IL2CPP_FAIL();
								}
								const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
								_s4 = chaos_element;
							}
							_s5 = chaos_locals[1];
							{
								const auto chaos_arg_1 = _s5;
								const auto chaos_arg_0 = _s4;
								const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[15])(chaos_arg_0, chaos_arg_1);
								_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
							}
							{
								if (_s4 == 0)
								{
									_s4 = chaos_locals[4];
									_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
									{
										const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s5);
										auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s4);
										const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
										_s4 = chaos_element;
									}
									{
										const auto chaos_arg_0 = _s4;
										const auto chaos_result = ChaosParseInt32(chaos_arg_0);
										_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
									}
									_s5 = chaos_locals[2];
									_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
									_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
									_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
									{
										if (_s4 == 0)
										{
											_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
											chaos_locals[6] = _s4;
										}
										else
										{
											_s11 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
											chaos_locals[6] = _s11;
										}
									}
								}
								else
								{
									_s8 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
									chaos_locals[6] = _s8;
								}
							}
						}
						else
						{
							_s21 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
							chaos_locals[6] = _s21;
						}
					}
				}
				else
				{
					_s17 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
					chaos_locals[6] = _s17;
				}
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			chaos_locals[6] = _s0;
	CHAOS_EH_END
	_s0 = chaos_locals[6];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: GenericSupplementSubjects/GenericSupplementNativeEntry::TestTypeLookupBySubjectId()
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupBySubjectId(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7) chaos_locals{};
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
	CHAOS_IL2CPP_INTPTR _s25{};
	CHAOS_IL2CPP_INTPTR _s26{};
	CHAOS_IL2CPP_INTPTR _s27{};
	CHAOS_IL2CPP_INTPTR _s28{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41972298u);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionGetTypeFullName(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = _s0;
			{
				if (_s1 == 0)
				{
					{
						auto* chaos_string = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_String);
						chaos_string->header.type_info = &chaos_mt_System_Private_CoreLib_System_String;
						chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(0);
						chaos_string->utf8_data = "";
						_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
					}
				}
				{{
					_s3 = CHAOS_IL2CPP_STRING_ID("List");
				}}
				{
					const auto chaos_arg_1 = _s3;
					const auto chaos_arg_0 = _s2;
					const auto chaos_result = ChaosStringContains(chaos_arg_0, chaos_arg_1);
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
				{
					if (_s2 == 0)
					{
						_s3 = chaos_locals[1];
						{
							const auto chaos_arg_0 = _s3;
							const auto chaos_result = ChaosReflectionGetTypeByName(chaos_arg_0);
							_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
						}
						_s4 = 0;
						{
							const auto chaos_arg_1 = _s4;
							const auto chaos_arg_0 = _s3;
							const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[21])(chaos_arg_0, chaos_arg_1);
							_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
						}
						{
							if (_s3 == 0)
							{
								_s3 = chaos_locals[2];
								_s4 = chaos_locals[0];
								{
									const auto chaos_arg_1 = _s4;
									const auto chaos_arg_0 = _s3;
									const auto chaos_result = ChaosTypeInequality(chaos_arg_0, chaos_arg_1);
									_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
								}
								{
									if (_s3 == 0)
									{
										_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
										chaos_locals[4] = _s3;
									}
									else
									{
										_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
										chaos_locals[4] = _s5;
									}
								}
							}
							else
							{
								_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
								chaos_locals[4] = _s6;
							}
						}
					}
					else
					{
						_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
						chaos_locals[4] = _s6;
					}
				}
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			chaos_locals[4] = _s0;
	CHAOS_EH_END
	_s0 = chaos_locals[4];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: GenericSupplementSubjects/GenericSupplementNativeEntry::TestTypeLookupByToken()
extern "C" CHAOS_IL2CPP_INT32 GenericSupplementSubjects_GenericSupplementNativeEntry_TestTypeLookupByToken(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 7) chaos_locals{};
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
	CHAOS_IL2CPP_INTPTR _s25{};
	CHAOS_IL2CPP_INTPTR _s26{};
	CHAOS_IL2CPP_INTPTR _s27{};
	CHAOS_IL2CPP_INTPTR _s28{};
	CHAOS_IL2CPP_INTPTR _s29{};
	CHAOS_IL2CPP_INTPTR _s30{};
	CHAOS_IL2CPP_INTPTR _s31{};
	CHAOS_IL2CPP_INTPTR _s32{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41972298u);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("Add");
			}}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(20);
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Type__GetMethod_System_Reflection_MethodInfo_System_String_System_Reflection_BindingFlags_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = 0;
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[23])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				if (_s0 == 0)
				{
					_s6 = chaos_locals[0];
					{
						const auto chaos_arg_0 = _s6;
						const auto chaos_result = ChaosReflectionGetMetadataToken(chaos_arg_0);
						_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
					}
					_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
					_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
					{
						if (_s6 == 0)
						{
							_s3 = chaos_locals[1];
							_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(24);
							_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_shift_right_int32(static_cast<CHAOS_IL2CPP_INT32>(_s3), static_cast<CHAOS_IL2CPP_INT32>(_s4)));
							_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(255);
							_s3 = chaos_store_uint64(chaos_load_uint64(_s3) & chaos_load_uint64(_s4));
							_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(6);
							_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s3) == static_cast<CHAOS_IL2CPP_INTPTR>(_s4) ? 1 : 0);
							_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
							_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s3) == static_cast<CHAOS_IL2CPP_INTPTR>(_s4) ? 1 : 0);
							{
								if (_s3 == 0)
								{
									_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
									chaos_locals[4] = _s3;
								}
								else
								{
									_s11 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
									chaos_locals[4] = _s11;
								}
							}
						}
						else
						{
							_s9 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
							chaos_locals[4] = _s9;
						}
					}
				}
				else
				{
					_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
					chaos_locals[4] = _s6;
				}
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			chaos_locals[4] = _s0;
	CHAOS_EH_END
	_s0 = chaos_locals[4];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}



}  // namespace chaos::il2cpp::codegen::GenericSupplementSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 9;

extern "C" void ChaosJitRegisterAll() {}