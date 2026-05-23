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
#include "enum_stubs.h"

// Forward declaration for dispatch table entries (defined in runtime_stubs.cpp)
extern "C" void InterpreterEntryDirect(
    CHAOS_IL2CPP_UINTPTR method_key,
    void*     args_buf,
    void*     ret_buf) noexcept;

#pragma warning(push)
#pragma warning(disable: 4065 4244)

namespace chaos::il2cpp::codegen::Il2CppTranslationTest {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


const void* chaos_vtable_Il2CppTranslationTest_Point[];
inline TypeInfoV0 chaos_mt_Il2CppTranslationTest__PrivateImplementationDetails_ = {{nullptr, nullptr, 10542868236999590534ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_Il2CppTranslationTest__PrivateImplementationDetails_ = static_cast<CHAOS_IL2CPP_INTPTR>(10542868236999590534ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
inline TypeInfoV0 chaos_mt_Il2CppTranslationTest_Point = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_Il2CppTranslationTest_Point, 16410627618859170283ULL, 2u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_Il2CppTranslationTest_Point = static_cast<CHAOS_IL2CPP_INTPTR>(16410627618859170283ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Int32 = {{nullptr, nullptr, 11009693519287992193ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Int32 = static_cast<CHAOS_IL2CPP_INTPTR>(11009693519287992193ULL);
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

// ── Virtual method table arrays ──
extern "C" void Il2CppTranslationTest_Point__ctor_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" double Il2CppTranslationTest_Point_Distance(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_Il2CppTranslationTest_Point[] =
{
	reinterpret_cast<void*>(Il2CppTranslationTest_Point__ctor_System_Int32_System_Int32),
	reinterpret_cast<void*>(Il2CppTranslationTest_Point_Distance),
};
static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_Il2CppTranslationTest_Point[] =
{
	{ 0x00000010u, reinterpret_cast<void*>(&Il2CppTranslationTest_Point__ctor_System_Int32_System_Int32) },
	{ 0x00000011u, reinterpret_cast<void*>(&Il2CppTranslationTest_Point_Distance) },
};

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

struct chaos_type_Il2CppTranslationTest__PrivateImplementationDetails_
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR field_Il2CppTranslationTest__PrivateImplementationDetails___4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384 = 0;
};

struct chaos_type_System_Private_CoreLib_System_Object
{
	ThinLockableHeader header{};
};

struct chaos_type_Il2CppTranslationTest_Point : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR field_Il2CppTranslationTest_Point___x = 0;
	CHAOS_IL2CPP_INTPTR field_Il2CppTranslationTest_Point___y = 0;
};

struct chaos_type_System_Private_CoreLib_System_Int32
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


	// AOT-baked string table: sorted by StringId for binary search at runtime.
	constexpr chaos::il2cpp::string_table::StringEntry chaos_aot_string_entries[] = {
		{ 2134336015689706537U, "Hello, il2cpp!", 14u },
		{ 3520513421467838623U, "il2cpp", 6u },
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

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_Il2CppTranslationTest__PrivateImplementationDetails___4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384[] =
{
	0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x05, 0x00, 0x00, 0x00,
};

bool chaos_try_get_static_field_data(
	CHAOS_IL2CPP_INTPTR chaos_field_handle,
	const CHAOS_IL2CPP_UINT8*& chaos_data,
	CHAOS_IL2CPP_SIZE& chaos_size)
{
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(74427012u))
	{
		chaos_data = chaos_static_field_data_Il2CppTranslationTest__PrivateImplementationDetails___4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384;
		chaos_size = sizeof(chaos_static_field_data_Il2CppTranslationTest__PrivateImplementationDetails___4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384);
		return true;
	}
	chaos_data = nullptr;
	chaos_size = 0;
	return false;
}

void chaos_initialize_array_from_field_data_int32(CHAOS_IL2CPP_INTPTR chaos_array_value, CHAOS_IL2CPP_INTPTR chaos_field_handle)
{
	auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_array_value);
	if (chaos_array == nullptr || chaos_array->element_type_shape == 0)
	{
		CHAOS_IL2CPP_FAIL();
	}
	const CHAOS_IL2CPP_UINT8* chaos_bytes = nullptr;
	CHAOS_IL2CPP_SIZE chaos_size = 0;
	if (!chaos_try_get_static_field_data(chaos_field_handle, chaos_bytes, chaos_size))
	{
		CHAOS_IL2CPP_FAIL();
	}
	const auto copy_count = static_cast<CHAOS_IL2CPP_INT32>(
		chaos_size < static_cast<CHAOS_IL2CPP_SIZE>(chaos_array->length)
			? chaos_size
			: static_cast<CHAOS_IL2CPP_SIZE>(chaos_array->length));
	for (CHAOS_IL2CPP_INT32 chaos_index = 0; chaos_index < copy_count; chaos_index++)
	{
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] =
			static_cast<CHAOS_IL2CPP_INTPTR>(chaos_bytes[chaos_index]);
	}
}

CHAOS_IL2CPP_INTPTR chaos_create_field_data_span_int32(CHAOS_IL2CPP_INTPTR chaos_field_handle)
{
	const CHAOS_IL2CPP_UINT8* chaos_bytes = nullptr;
	CHAOS_IL2CPP_SIZE chaos_size = 0;
	if (!chaos_try_get_static_field_data(chaos_field_handle, chaos_bytes, chaos_size) ||
		(chaos_size % sizeof(CHAOS_IL2CPP_INT32)) != 0)
	{
		CHAOS_IL2CPP_FAIL();
	}
	return ChaosIl2cpp::Common::create_raw_span_int32(const_cast<CHAOS_IL2CPP_UINT8*>(chaos_bytes), static_cast<CHAOS_IL2CPP_INT32>(chaos_size / sizeof(CHAOS_IL2CPP_INT32)), true);
}

static CHAOS_IL2CPP_INT64 chaos_external_runtime_System_Private_CoreLib_System_Math__Sqrt_System_Double_System_Double_(CHAOS_IL2CPP_INT64 chaos_fn_arg_0)
{
	return ChaosMathSqrt(chaos_fn_arg_0);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	ChaosObjectCtor(chaos_fn_arg_0);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	chaos_initialize_array_from_field_data_int32(chaos_fn_arg_0, chaos_fn_arg_1);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_String__Contains_System_Boolean_System_String_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	return ChaosStringContains(chaos_fn_arg_0, chaos_fn_arg_1);
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

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };


// ── Bridge/import thunks ──
extern "C" void* kChaosExternalRuntimeFnTable[];

// Bridge/import thunk for: Il2CppTranslationTest/Point::_x
extern "C" void chaos_bridge_thunk_0(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[1])();
}

// Bridge/import thunk for: Il2CppTranslationTest/Point::_y
extern "C" void chaos_bridge_thunk_1(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[2])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Int32
extern "C" void chaos_bridge_thunk_2(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[6])();
}

// Bridge/import thunk for: Il2CppTranslationTest/<PrivateImplementationDetails>::4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384
extern "C" void chaos_bridge_thunk_3(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[7])();
}

extern "C" void Il2CppTranslationTest_AutoGeneratedProgram__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void Il2CppTranslationTest_AutoGeneratedProgram_Main_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void Il2CppTranslationTest_Point__ctor_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" double Il2CppTranslationTest_Point_Distance(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void Il2CppTranslationTest_Program__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 Il2CppTranslationTest_Program_Main___g__Add_0_0_System_Int32_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 Il2CppTranslationTest_Program_Main___g__Multiply_0_1_System_Int32_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 Il2CppTranslationTest_Program_Main___g__SumArray_0_2_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 Il2CppTranslationTest_Program_Main_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);


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
	1u,
	3u,
	3u,
	3u,
	5u,
	7u,
	8u,
	9u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[9];
	CHAOS_IL2CPP_UINT8 params[9];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		9u,
		9u,
		3310402915u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // Il2CppTranslationTest_AutoGeneratedProgram__ctor
		{ 0u, 1u },  // Il2CppTranslationTest_AutoGeneratedProgram_Main_System_String
		{ 0u, 2u },  // Il2CppTranslationTest_Point__ctor_System_Int32_System_Int32
		{ 9u, 0u },  // Il2CppTranslationTest_Point_Distance
		{ 0u, 0u },  // Il2CppTranslationTest_Program__ctor
		{ 1u, 2u },  // Il2CppTranslationTest_Program_Main___g__Add_0_0_System_Int32_System_Int32
		{ 1u, 2u },  // Il2CppTranslationTest_Program_Main___g__Multiply_0_1_System_Int32_System_Int32
		{ 1u, 1u },  // Il2CppTranslationTest_Program_Main___g__SumArray_0_2_System_Int32
		{ 1u, 1u },  // Il2CppTranslationTest_Program_Main_System_String
	},
	{
		2u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		2u,
		2u,
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[6] = {
		0u,
		0u,
		0u,
		1536u,
		8u,
		1545u,
	};

	static constexpr const char* s_type_names[6] = {
		"<Module>",
		"Program",
		"AutoGeneratedProgram",
		"Point",
		"<PrivateImplementationDetails>",
		"__StaticArrayInitTypeSize=20",
	};

	static constexpr const char* s_type_namespaces[6] = {
		"",
		"",
		"",
		"Il2CppTranslationTest",
		"",
		"",
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_type_parent_tokens[6] = {
		0u,
		0u,
		0u,
		0u,
		0u,
		0u,
	};

	static const TypeInfoHot* const s_type_info_ptrs[6] = {
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		&chaos_mt_Il2CppTranslationTest__PrivateImplementationDetails_.hot,
		nullptr,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_children[1] = {
		33554438u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[7] = {
		0u,
		0u,
		0u,
		0u,
		0u,
		1u,
		1u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset[7] = {
		0u,
		0u,
		0u,
		0u,
		0u,
		0u,
		0u,
	};



	static const ::chaos::il2cpp::runtime_core::ModuleDescriptor s_native_aot_module = {
		/* .name_utf8         = */ "Il2CppTranslationTest",
		/* .image             = */ &kReflImage,
		/* .type_flags        = */ s_type_flags,
		/* .type_names        = */ s_type_names,
		/* .type_namespaces   = */ s_type_namespaces,
		/* .type_parent_tokens= */ s_type_parent_tokens,
		/* .type_info_ptrs    = */ s_type_info_ptrs,
		/* .nested_type_children= */ s_nested_type_children,
		/* .nested_type_offset = */ s_nested_type_offset,
		/* .generic_param_constraint_data= */ nullptr,
		/* .generic_param_constraint_offset= */ s_generic_param_constraint_offset,
		/* .type_count        = */ 6u,
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
		::chaos::il2cpp::runtime_core::RegisterModule("Il2CppTranslationTest", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[9] = {
	{ ".ctor", 0x0000000Fu, 0u },  // AutoGeneratedProgram
	{ "Main", 0x0000000Eu, 1u },  // AutoGeneratedProgram
	{ ".ctor", 0x00000010u, 2u },  // Point
	{ "Distance", 0x00000011u, 0u },  // Point
	{ ".ctor", 0x0000000Au, 0u },  // Program
	{ "<<Main>$>g__Add|0_0", 0x0000000Bu, 2u },  // Program
	{ "<<Main>$>g__Multiply|0_1", 0x0000000Cu, 2u },  // Program
	{ "<<Main>$>g__SumArray|0_2", 0x0000000Du, 1u },  // Program
	{ "<Main>$", 0x00000009u, 1u },  // Program
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[3] = {
	{ "AutoGeneratedProgram", "", 0u, 2u },
	{ "Point", "", 2u, 2u },
	{ "Program", "", 4u, 5u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[9] = {
	{ 0x00000009u, 8u },
	{ 0x0000000Au, 4u },
	{ 0x0000000Bu, 5u },
	{ 0x0000000Cu, 6u },
	{ 0x0000000Du, 7u },
	{ 0x0000000Eu, 1u },
	{ 0x0000000Fu, 0u },
	{ 0x00000010u, 2u },
	{ 0x00000011u, 3u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[9] = {
	{ reinterpret_cast<void*>(&Il2CppTranslationTest_AutoGeneratedProgram__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGeneratedProgram::.ctor
	{ reinterpret_cast<void*>(&Il2CppTranslationTest_AutoGeneratedProgram_Main_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGeneratedProgram::Main
	{ reinterpret_cast<void*>(&Il2CppTranslationTest_Point__ctor_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Point::.ctor
	{ reinterpret_cast<void*>(&Il2CppTranslationTest_Point_Distance), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Point::Distance
	{ reinterpret_cast<void*>(&Il2CppTranslationTest_Program__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Program::.ctor
	{ reinterpret_cast<void*>(&Il2CppTranslationTest_Program_Main___g__Add_0_0_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Program::<<Main>$>g__Add|0_0
	{ reinterpret_cast<void*>(&Il2CppTranslationTest_Program_Main___g__Multiply_0_1_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Program::<<Main>$>g__Multiply|0_1
	{ reinterpret_cast<void*>(&Il2CppTranslationTest_Program_Main___g__SumArray_0_2_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Program::<<Main>$>g__SumArray|0_2
	{ reinterpret_cast<void*>(&Il2CppTranslationTest_Program_Main_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Program::<Main>$
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"Il2CppTranslationTest",
	s_hotpatch_types,
	3u,
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

extern "C" const char* kChaosExternalRuntimeSubjects[9] = {
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
	"Il2CppTranslationTest/Point::_x",
	"Il2CppTranslationTest/Point::_y",
	"System.Private.CoreLib/System.Math::Sqrt:System.Double(System.Double)",
	"System.Private.CoreLib/System.String::get_Length:System.Int32()",
	"System.Private.CoreLib/System.String::Contains:System.Boolean(System.String)",
	"System.Private.CoreLib/System.Int32",
	"Il2CppTranslationTest/<PrivateImplementationDetails>::4F6ADDC9659D6FB90FE94B6688A79F2A1FA8D36EC43F8F3E1D9B6528C448A384",
	"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)",
};

extern "C" void* kChaosExternalRuntimeFnTable[9] = {
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__),
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Math__Sqrt_System_Double_System_Double_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_String__Contains_System_Boolean_System_String_),
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_),
};

extern "C" int32_t kChaosExternalRuntimeCount = 9;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[9])() = {
	reinterpret_cast<void(*)()>(&Il2CppTranslationTest_AutoGeneratedProgram__ctor),
	reinterpret_cast<void(*)()>(&Il2CppTranslationTest_AutoGeneratedProgram_Main_System_String),
	reinterpret_cast<void(*)()>(&Il2CppTranslationTest_Point__ctor_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&Il2CppTranslationTest_Point_Distance),
	reinterpret_cast<void(*)()>(&Il2CppTranslationTest_Program__ctor),
	reinterpret_cast<void(*)()>(&Il2CppTranslationTest_Program_Main___g__Add_0_0_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&Il2CppTranslationTest_Program_Main___g__Multiply_0_1_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&Il2CppTranslationTest_Program_Main___g__SumArray_0_2_System_Int32),
	reinterpret_cast<void(*)()>(&Il2CppTranslationTest_Program_Main_System_String),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[9])() = {
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[0])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[1])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[2])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[3])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[4])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[5])(0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[6])(0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[7])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[8])(0);},
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

	/* ── Entry 0: Il2CppTranslationTest_AutoGeneratedProgram__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry0;
	/* ── Entry 1: Il2CppTranslationTest_AutoGeneratedProgram_Main_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry1;
	/* ── Entry 2: Il2CppTranslationTest_Point__ctor_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry2;
	/* ── Entry 3: Il2CppTranslationTest_Point_Distance ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry3;
	/* ── Entry 4: Il2CppTranslationTest_Program__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry4;
	/* ── Entry 5: Il2CppTranslationTest_Program_Main___g__Add_0_0_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry5;
	/* ── Entry 6: Il2CppTranslationTest_Program_Main___g__Multiply_0_1_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry6;
	/* ── Entry 7: Il2CppTranslationTest_Program_Main___g__SumArray_0_2_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[9];
	} entry7;
	/* ── Entry 8: Il2CppTranslationTest_Program_Main_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[19];
	} entry8;
} kChaosGcSlotMapsSection = {
	/* entry0 = Il2CppTranslationTest_AutoGeneratedProgram__ctor */
	.entry0 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&Il2CppTranslationTest_AutoGeneratedProgram__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry1 = Il2CppTranslationTest_AutoGeneratedProgram_Main_System_String */
	.entry1 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&Il2CppTranslationTest_AutoGeneratedProgram_Main_System_String),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry2 = Il2CppTranslationTest_Point__ctor_System_Int32_System_Int32 */
	.entry2 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&Il2CppTranslationTest_Point__ctor_System_Int32_System_Int32),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 24u, 32u }
	},
	/* entry3 = Il2CppTranslationTest_Point_Distance */
	.entry3 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&Il2CppTranslationTest_Point_Distance),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry4 = Il2CppTranslationTest_Program__ctor */
	.entry4 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&Il2CppTranslationTest_Program__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry5 = Il2CppTranslationTest_Program_Main___g__Add_0_0_System_Int32_System_Int32 */
	.entry5 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&Il2CppTranslationTest_Program_Main___g__Add_0_0_System_Int32_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 16u, 24u }
	},
	/* entry6 = Il2CppTranslationTest_Program_Main___g__Multiply_0_1_System_Int32_System_Int32 */
	.entry6 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&Il2CppTranslationTest_Program_Main___g__Multiply_0_1_System_Int32_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 16u, 24u }
	},
	/* entry7 = Il2CppTranslationTest_Program_Main___g__SumArray_0_2_System_Int32 */
	.entry7 = {
		/* entry_total_size = 56 */ 56u,
		/* code_address */ reinterpret_cast<const void*>(&Il2CppTranslationTest_Program_Main___g__SumArray_0_2_System_Int32),
		/* frame_size = 72 */ 72u,
		/* num_gc_slots = 9 */ 9u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u }
	},
	/* entry8 = Il2CppTranslationTest_Program_Main_System_String */
	.entry8 = {
		/* entry_total_size = 96 */ 96u,
		/* code_address */ reinterpret_cast<const void*>(&Il2CppTranslationTest_Program_Main_System_String),
		/* frame_size = 152 */ 152u,
		/* num_gc_slots = 19 */ 19u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u, 80u, 88u, 96u, 104u, 112u, 120u, 128u, 136u, 144u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 372u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[9] = {
	reinterpret_cast<void*>(&Il2CppTranslationTest_AutoGeneratedProgram__ctor),
	reinterpret_cast<void*>(&Il2CppTranslationTest_AutoGeneratedProgram_Main_System_String),
	reinterpret_cast<void*>(&Il2CppTranslationTest_Point__ctor_System_Int32_System_Int32),
	reinterpret_cast<void*>(&Il2CppTranslationTest_Point_Distance),
	reinterpret_cast<void*>(&Il2CppTranslationTest_Program__ctor),
	reinterpret_cast<void*>(&Il2CppTranslationTest_Program_Main___g__Add_0_0_System_Int32_System_Int32),
	reinterpret_cast<void*>(&Il2CppTranslationTest_Program_Main___g__Multiply_0_1_System_Int32_System_Int32),
	reinterpret_cast<void*>(&Il2CppTranslationTest_Program_Main___g__SumArray_0_2_System_Int32),
	reinterpret_cast<void*>(&Il2CppTranslationTest_Program_Main_System_String),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0xE3BE42DF4350B9EB),
		0x00000003u,
		0u,
		2u,
		reinterpret_cast<const void*>(kSlots_Il2CppTranslationTest_Point),
		reinterpret_cast<const void**>(chaos_vtable_Il2CppTranslationTest_Point),
		2u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
};
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
	.vtable_descriptors = kChaosVTableDescriptors,
	.vtable_descriptor_count = 1u,
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
	.image_name_utf8    = "Il2CppTranslationTest",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryMethodDescriptor kReflMethods_Il2CppTranslationTest_AutoGeneratedProgram[2] = {
	{ 0x0000000Fu, "Il2CppTranslationTest/AutoGeneratedProgram::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Eu, "Il2CppTranslationTest/AutoGeneratedProgram::Main:System.Void(System.String[])", "Main", "System.Void", 1, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_Il2CppTranslationTest_Point[2] = {
	{ 0x00000010u, "Il2CppTranslationTest/Point::.ctor:System.Void(System.Int32,System.Int32)", ".ctor", "System.Void", 2, nullptr, 0u, nullptr, 0u },
	{ 0x00000011u, "Il2CppTranslationTest/Point::Distance:System.Double()", "Distance", "System.Double", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_Il2CppTranslationTest_Program[5] = {
	{ 0x0000000Au, "Il2CppTranslationTest/Program::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Bu, "Il2CppTranslationTest/Program::<<Main>$>g__Add|0_0:System.Int32(System.Int32,System.Int32)", "<<Main>$>g__Add|0_0", "System.Int32", 2, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Cu, "Il2CppTranslationTest/Program::<<Main>$>g__Multiply|0_1:System.Int32(System.Int32,System.Int32)", "<<Main>$>g__Multiply|0_1", "System.Int32", 2, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Du, "Il2CppTranslationTest/Program::<<Main>$>g__SumArray|0_2:System.Int32(System.Int32[])", "<<Main>$>g__SumArray|0_2", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
	{ 0x00000009u, "Il2CppTranslationTest/Program::<Main>$:System.Int32(System.String[])", "<Main>$", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[3] = {
	{ 0x00000002u, "Il2CppTranslationTest/AutoGeneratedProgram", "Il2CppTranslationTest/AutoGeneratedProgram", "", "AutoGeneratedProgram", "AutoGeneratedProgram", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_Il2CppTranslationTest_AutoGeneratedProgram, 2u, nullptr, 0u, 0u },
	{ 0x00000003u, "Il2CppTranslationTest/Point", "Il2CppTranslationTest/Point", "", "Point", "Point", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_Il2CppTranslationTest_Point, 2u, nullptr, 0u, 0u },
	{ 0x00000001u, "Il2CppTranslationTest/Program", "Il2CppTranslationTest/Program", "", "Program", "Program", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_Il2CppTranslationTest_Program, 5u, nullptr, 0u, 0u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[3] = {
	&kReflTypes[0],
	&kReflTypes[1],
	&kReflTypes[2],
};

static const ReflectionQueryImageDescriptor kReflImage = { "Il2CppTranslationTest", kReflTypePtrs, 3u, 1, 0, 0, 0 };

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
// Managed method: Il2CppTranslationTest/AutoGeneratedProgram::.ctor()
extern "C" void Il2CppTranslationTest_AutoGeneratedProgram__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: Il2CppTranslationTest/AutoGeneratedProgram::Main(System.String[])
extern "C" void Il2CppTranslationTest_AutoGeneratedProgram_Main_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	return;
}

// Managed method: Il2CppTranslationTest/Point::.ctor(System.Int32,System.Int32)
extern "C" void Il2CppTranslationTest_Point__ctor_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_Il2CppTranslationTest_Point*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_Il2CppTranslationTest_Point___x));
		chaos_object->field_Il2CppTranslationTest_Point___x = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	_s0 = chaos_args[0];
	_s1 = chaos_args[2];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_Il2CppTranslationTest_Point*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_Il2CppTranslationTest_Point___y));
		chaos_object->field_Il2CppTranslationTest_Point___y = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	return;
}

// Managed method: Il2CppTranslationTest/Point::Distance()
extern "C" double Il2CppTranslationTest_Point_Distance(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_Il2CppTranslationTest_Point*>(_s0);
		_s0 = chaos_object->field_Il2CppTranslationTest_Point___x;
	}
	_s1 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_Il2CppTranslationTest_Point*>(_s1);
		_s1 = chaos_object->field_Il2CppTranslationTest_Point___x;
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	_s1 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_Il2CppTranslationTest_Point*>(_s1);
		_s1 = chaos_object->field_Il2CppTranslationTest_Point___y;
	}
	_s2 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_Il2CppTranslationTest_Point*>(_s2);
		_s2 = chaos_object->field_Il2CppTranslationTest_Point___y;
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s1), static_cast<CHAOS_IL2CPP_INT32>(_s2)));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	_s0 = ChaosStoreFloat64(static_cast<double>(_s0));
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosMathSqrt(ChaosLoadInt64(chaos_arg_0));
		_s0 = ChaosStoreInt64(chaos_result);
	}
	return ChaosLoadFloat64(_s0);
}

// Managed method: Il2CppTranslationTest/Program::.ctor()
extern "C" void Il2CppTranslationTest_Program__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: Il2CppTranslationTest/Program::<<Main>$>g__Add|0_0(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 Il2CppTranslationTest_Program_Main___g__Add_0_0_System_Int32_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: Il2CppTranslationTest/Program::<<Main>$>g__Multiply|0_1(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 Il2CppTranslationTest_Program_Main___g__Multiply_0_1_System_Int32_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: Il2CppTranslationTest/Program::<<Main>$>g__SumArray|0_2(System.Int32[])
extern "C" CHAOS_IL2CPP_INT32 Il2CppTranslationTest_Program_Main___g__SumArray_0_2_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 4) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	// pc-dispatch state machine for irreducible CFG
	int32_t chaos_pc = 0;
	while (chaos_pc >= 0)
	{
		switch (chaos_pc)
		{
		case 0:
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			chaos_locals[0] = _s0;
			_s0 = chaos_args[0];
			chaos_locals[1] = _s0;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			chaos_locals[2] = _s0;
			chaos_pc = 2;
			break;
		}
		case 1:
		{
			_s0 = chaos_locals[1];
			_s1 = chaos_locals[2];
			{
				const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
				auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
				if (chaos_array == nullptr)
				{
					CHAOS_IL2CPP_FAIL();
				}
				if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
				{
					CHAOS_IL2CPP_FAIL();
				}
				const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
				_s0 = chaos_element;
			}
			chaos_locals[3] = _s0;
			_s0 = chaos_locals[0];
			_s1 = chaos_locals[3];
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[2];
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
			chaos_locals[2] = _s0;
			chaos_pc = -1;
			break;
		}
		case 2:
		{
			_s0 = chaos_locals[2];
			_s1 = chaos_locals[1];
			_s1 = [&](){ auto* _c_arr = reinterpret_cast<chaos_managed_array*>(_s1); return _c_arr ? static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_c_arr->length)) : CHAOS_IL2CPP_INTPTR{}; }();
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			chaos_pc = 1;
			break;
		}
		case 3:
		{
			_s2 = chaos_locals[0];
			return static_cast<CHAOS_IL2CPP_INT32>(_s2);
			chaos_pc = -1;
			break;
		}
		default:
			CHAOS_IL2CPP_FAIL("invalid pc-dispatch value");
			chaos_pc = -1;
			break;
		}
	}
}

// Managed method: Il2CppTranslationTest/Program::<Main>$(System.String[])
extern "C" CHAOS_IL2CPP_INT32 Il2CppTranslationTest_Program_Main_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 12) chaos_locals{};
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("Hello, il2cpp!");
	}}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[0];
	{{
		_s1 = CHAOS_IL2CPP_STRING_ID("il2cpp");
	}}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosStringContains(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[2] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(20);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		auto& _d5 = s_hotpatch_entries[5];
		CHAOS_IL2CPP_INT32 _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d5)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d5))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
			_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d5.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = Il2CppTranslationTest_Program_Main___g__Add_0_0_System_Int32_System_Int32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	chaos_locals[3] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(7);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		auto& _d6 = s_hotpatch_entries[6];
		CHAOS_IL2CPP_INT32 _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d6)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d6))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
			_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d6.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = Il2CppTranslationTest_Program_Main___g__Multiply_0_1_System_Int32_System_Int32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	chaos_locals[4] = _s0;
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(74427012u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_locals[5] = _s0;
	_s0 = chaos_locals[5];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
		if (chaos_array == nullptr)
		{
			CHAOS_IL2CPP_FAIL();
		}
		if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s0 = chaos_element;
	}
	chaos_locals[6] = _s0;
	_s0 = chaos_locals[5];
	_s1 = chaos_locals[5];
	_s1 = [&](){ auto* _c_arr = reinterpret_cast<chaos_managed_array*>(_s1); return _c_arr ? static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_c_arr->length)) : CHAOS_IL2CPP_INTPTR{}; }();
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapSub(static_cast<CHAOS_IL2CPP_INT32>(_s1), static_cast<CHAOS_IL2CPP_INT32>(_s2)));
	{
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);
		if (chaos_array == nullptr)
		{
			CHAOS_IL2CPP_FAIL();
		}
		if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
		{
			CHAOS_IL2CPP_FAIL();
		}
		const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
		_s0 = chaos_element;
	}
	chaos_locals[7] = _s0;
	_s0 = chaos_locals[5];
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s0);
		auto& _d7 = s_hotpatch_entries[7];
		CHAOS_IL2CPP_INT32 _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d7)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d7))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d7.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = Il2CppTranslationTest_Program_Main___g__SumArray_0_2_System_Int32(chaos_arg_0);
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	chaos_locals[8] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
	{
		const auto chaos_arg_2 = _s1;
		const auto chaos_arg_1 = _s0;
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_Il2CppTranslationTest_Point, {});
		chaos_object->header.type_info = &chaos_mt_Il2CppTranslationTest_Point.hot;
		Il2CppTranslationTest_Point__ctor_System_Int32_System_Int32(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s0);
		auto& _d3 = s_hotpatch_entries[3];
		double _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d3)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d3))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d3.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = Il2CppTranslationTest_Point_Distance(chaos_arg_0);
		}
		_s0 = ChaosStoreFloat64(_d_hpresult);
	}
	chaos_locals[10] = _s0;
	_s0 = chaos_locals[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0) > ChaosLoadFloat64(_s1) ? 1 : 0);
	_s1 = chaos_locals[2];
	_s0 = chaos_store_uint64(chaos_load_uint64(_s0) & chaos_load_uint64(_s1));
	{
		if (_s0 != 0)
		{
			_s0 = chaos_locals[3];
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(30);
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				if (chaos_left == chaos_right)
				{
					_s0 = chaos_locals[4];
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(21);
					{
						const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
						const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
						if (chaos_left == chaos_right)
						{
							_s0 = chaos_locals[6];
							_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
							{
								const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
								const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
								if (chaos_left == chaos_right)
								{
									_s0 = chaos_locals[7];
									_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
									{
										const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
										const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
										if (chaos_left == chaos_right)
										{
											_s0 = chaos_locals[8];
											_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(15);
											{
												const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
												const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
												if (chaos_left == chaos_right)
												{
													_s0 = chaos_locals[10];
													_s1 = ChaosStoreFloat64(0);
													{
														const auto chaos_right = ChaosLoadFloat64(_s1);
														const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(_s0);
														if (chaos_left > chaos_right)
														{
															_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
														}
														else
														{
															_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		_s27 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		_s28 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		chaos_locals[11] = _s28;
		_s28 = chaos_locals[11];
		return static_cast<CHAOS_IL2CPP_INT32>(_s28);
	}
}



}  // namespace chaos::il2cpp::codegen::Il2CppTranslationTest
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 9;

extern "C" void ChaosJitRegisterAll() {}