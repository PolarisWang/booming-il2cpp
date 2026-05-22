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

namespace chaos::il2cpp::codegen::GuidRandomHashcodeSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_GuidRandomHashcodeSubjects__PrivateImplementationDetails_ = {{nullptr, nullptr, 4123915259771640705ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_GuidRandomHashcodeSubjects__PrivateImplementationDetails_ = static_cast<CHAOS_IL2CPP_INTPTR>(4123915259771640705ULL);
inline TypeInfoV0 chaos_mt_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects = {{nullptr, nullptr, 5733992221058677214ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(5733992221058677214ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Byte = {{nullptr, nullptr, 3217969471912086765ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Byte = static_cast<CHAOS_IL2CPP_INTPTR>(3217969471912086765ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Guid = {{nullptr, nullptr, 17983019571512764410ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Guid = static_cast<CHAOS_IL2CPP_INTPTR>(17983019571512764410ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Random = {{nullptr, nullptr, 5623716467429953792ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Random = static_cast<CHAOS_IL2CPP_INTPTR>(5623716467429953792ULL);
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

struct chaos_type_GuidRandomHashcodeSubjects__PrivateImplementationDetails_
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR field_GuidRandomHashcodeSubjects__PrivateImplementationDetails___039058C6F2C0CB492C533B0A4D14EF77CC0F78ABCCCED5287D84A1A2011CFB81 = 0;
};

struct chaos_type_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Byte
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Guid
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Object
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Random
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

CHAOS_IL2CPP_INT32 chaos_static_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects___exitCode = 0;


	// AOT-baked string table: sorted by StringId for binary search at runtime.
	constexpr chaos::il2cpp::string_table::StringEntry chaos_aot_string_entries[] = {
		{ 8950988243607919089U, "00000000-0000-0000-0000-000000000000", 36u },
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

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_GuidRandomHashcodeSubjects__PrivateImplementationDetails___039058C6F2C0CB492C533B0A4D14EF77CC0F78ABCCCED5287D84A1A2011CFB81[] =
{
	0x01, 0x02, 0x03,
};

bool chaos_try_get_static_field_data(
	CHAOS_IL2CPP_INTPTR chaos_field_handle,
	const CHAOS_IL2CPP_UINT8*& chaos_data,
	CHAOS_IL2CPP_SIZE& chaos_size)
{
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(73644948u))
	{
		chaos_data = chaos_static_field_data_GuidRandomHashcodeSubjects__PrivateImplementationDetails___039058C6F2C0CB492C533B0A4D14EF77CC0F78ABCCCED5287D84A1A2011CFB81;
		chaos_size = sizeof(chaos_static_field_data_GuidRandomHashcodeSubjects__PrivateImplementationDetails___039058C6F2C0CB492C533B0A4D14EF77CC0F78ABCCCED5287D84A1A2011CFB81);
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

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Guid__GetHashCode_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	return ChaosGuidGetHashCode(chaos_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Guid__NewGuid_System_Guid__()
{
	return ChaosGuidNewGuid();
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Guid__Parse_System_Guid_System_String_(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	return ChaosGuidParse(chaos_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Guid__ToString_System_String__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	return ChaosGuidToString(chaos_arg_0);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Guid___ctor_System_Void_System_Byte___(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
	ChaosGuidCtor(chaos_arg_0, chaos_arg_1);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Guid___ctor_System_Void_System_String_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
	ChaosGuidCtor(chaos_arg_0, chaos_arg_1);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_HashCode__Combine_System_Int32_System_Int32__System_Int32_System_Int32_System_Int32_(CHAOS_IL2CPP_INT32 chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1)
{
	return ChaosHashCodeCombine2(chaos_arg_0, chaos_arg_1);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_HashCode__ToHashCode_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	(void)chaos_arg_0;
	return ChaosHashCodeToHashCode(chaos_arg_0);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Random__NextBytes_System_Void_System_Byte___(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	ChaosRandomNextBytes(chaos_fn_arg_0, chaos_fn_arg_1);
}

static CHAOS_IL2CPP_INT64 chaos_external_runtime_System_Private_CoreLib_System_Random__NextDouble_System_Double__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosRandomNextDouble(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Random__Next_System_Int32_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	return ChaosRandomNextMax(0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Random__Next_System_Int32__()
{
	return ChaosRandomNext(0);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Random___ctor_System_Void__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	ChaosRandomCtor(chaos_fn_arg_0);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	chaos_initialize_array_from_field_data_int32(chaos_fn_arg_0, chaos_fn_arg_1);
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

extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_0(void);
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_1(void);
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_2(void);
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_3(void);
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_4(void);
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_5(void);
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_6(void);
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_7(void);
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_8(void);
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_9(void);
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_10(void);
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_11(void);
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_12(void);
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_13(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[15] = {
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
	::ChaosAbiMethodEntryV0 entries[14];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		14u,
		0u,
		4123785717u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_0
		{ 0u, 0u },  // GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_1
		{ 0u, 0u },  // GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_2
		{ 0u, 0u },  // GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_3
		{ 0u, 0u },  // GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_4
		{ 0u, 0u },  // GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_5
		{ 0u, 0u },  // GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_6
		{ 0u, 0u },  // GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_7
		{ 0u, 0u },  // GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_8
		{ 0u, 0u },  // GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_9
		{ 0u, 0u },  // GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_10
		{ 0u, 0u },  // GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_11
		{ 0u, 0u },  // GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_12
		{ 0u, 0u },  // GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_13
	},
	{
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[4] = {
		0u,
		1548u,
		8u,
		1545u,
	};

	static constexpr const char* s_type_names[4] = {
		"<Module>",
		"GuidRandomHashcodeSubjects",
		"<PrivateImplementationDetails>",
		"__StaticArrayInitTypeSize=3",
	};

	static constexpr const char* s_type_namespaces[4] = {
		"",
		"",
		"",
		"",
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_type_parent_tokens[4] = {
		0u,
		0u,
		0u,
		0u,
	};

	static const TypeInfoHot* const s_type_info_ptrs[4] = {
		nullptr,
		&chaos_mt_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects.hot,
		&chaos_mt_GuidRandomHashcodeSubjects__PrivateImplementationDetails_.hot,
		nullptr,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_children[1] = {
		33554436u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[5] = {
		0u,
		0u,
		0u,
		1u,
		1u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset[5] = {
		0u,
		0u,
		0u,
		0u,
		0u,
	};



	static const ::chaos::il2cpp::runtime_core::ModuleDescriptor s_native_aot_module = {
		/* .name_utf8         = */ "GuidRandomHashcodeSubjects",
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
		/* .type_count        = */ 4u,
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 — deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,

		/* .abi_manifest      = */ s_abi_manifest,
	};
	static const CHAOS_IL2CPP_UINT32 s_native_aot_module_id =
		::chaos::il2cpp::runtime_core::RegisterModule("GuidRandomHashcodeSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[14] = {
	{ "Subject_0", 0x00000006u, 0u },  // GuidRandomHashcodeSubjects
	{ "Subject_1", 0x00000007u, 0u },  // GuidRandomHashcodeSubjects
	{ "Subject_2", 0x00000008u, 0u },  // GuidRandomHashcodeSubjects
	{ "Subject_3", 0x00000009u, 0u },  // GuidRandomHashcodeSubjects
	{ "Subject_4", 0x0000000Au, 0u },  // GuidRandomHashcodeSubjects
	{ "Subject_5", 0x0000000Bu, 0u },  // GuidRandomHashcodeSubjects
	{ "Subject_6", 0x0000000Cu, 0u },  // GuidRandomHashcodeSubjects
	{ "Subject_7", 0x0000000Du, 0u },  // GuidRandomHashcodeSubjects
	{ "Subject_8", 0x0000000Eu, 0u },  // GuidRandomHashcodeSubjects
	{ "Subject_9", 0x0000000Fu, 0u },  // GuidRandomHashcodeSubjects
	{ "Subject_10", 0x00000010u, 0u },  // GuidRandomHashcodeSubjects
	{ "Subject_11", 0x00000011u, 0u },  // GuidRandomHashcodeSubjects
	{ "Subject_12", 0x00000012u, 0u },  // GuidRandomHashcodeSubjects
	{ "Subject_13", 0x00000013u, 0u },  // GuidRandomHashcodeSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "GuidRandomHashcodeSubjects", "", 0u, 14u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[14] = {
	{ 0x00000006u, 0u },
	{ 0x00000007u, 1u },
	{ 0x00000008u, 2u },
	{ 0x00000009u, 3u },
	{ 0x0000000Au, 4u },
	{ 0x0000000Bu, 5u },
	{ 0x0000000Cu, 6u },
	{ 0x0000000Du, 7u },
	{ 0x0000000Eu, 8u },
	{ 0x0000000Fu, 9u },
	{ 0x00000010u, 10u },
	{ 0x00000011u, 11u },
	{ 0x00000012u, 12u },
	{ 0x00000013u, 13u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[14] = {
	{ reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GuidRandomHashcodeSubjects::Subject_0
	{ reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GuidRandomHashcodeSubjects::Subject_1
	{ reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GuidRandomHashcodeSubjects::Subject_2
	{ reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GuidRandomHashcodeSubjects::Subject_3
	{ reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GuidRandomHashcodeSubjects::Subject_4
	{ reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GuidRandomHashcodeSubjects::Subject_5
	{ reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GuidRandomHashcodeSubjects::Subject_6
	{ reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GuidRandomHashcodeSubjects::Subject_7
	{ reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GuidRandomHashcodeSubjects::Subject_8
	{ reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GuidRandomHashcodeSubjects::Subject_9
	{ reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GuidRandomHashcodeSubjects::Subject_10
	{ reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GuidRandomHashcodeSubjects::Subject_11
	{ reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GuidRandomHashcodeSubjects::Subject_12
	{ reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // GuidRandomHashcodeSubjects::Subject_13
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"GuidRandomHashcodeSubjects",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	14u,
	s_hotpatch_slots,
	14u,
	s_hotpatch_entries,
	14u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[21] = {
	"System.Private.CoreLib/System.Guid::.ctor:System.Void(System.String)",
	"GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::_exitCode",
	"System.Private.CoreLib/System.Byte",
	"GuidRandomHashcodeSubjects/<PrivateImplementationDetails>::039058C6F2C0CB492C533B0A4D14EF77CC0F78ABCCCED5287D84A1A2011CFB81",
	"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)",
	"System.Private.CoreLib/System.Guid::.ctor:System.Void(System.Byte[])",
	"System.Private.CoreLib/System.Guid::NewGuid:System.Guid()",
	"System.Private.CoreLib/System.Guid::GetHashCode:System.Int32()",
	"System.Private.CoreLib/System.Guid::Parse:System.Guid(System.String)",
	"System.Private.CoreLib/System.Guid::TryParse:System.Boolean(System.String,System.Guid&)",
	"System.Private.CoreLib/System.Guid::ToString:System.String()",
	"System.Private.CoreLib/System.String::get_Length:System.Int32()",
	"System.Private.CoreLib/System.Random::.ctor:System.Void()",
	"System.Private.CoreLib/System.Random::Next:System.Int32()",
	"System.Private.CoreLib/System.Random::Next:System.Int32(System.Int32)",
	"System.Private.CoreLib/System.Random::NextDouble:System.Double()",
	"System.Private.CoreLib/System.Random::NextBytes:System.Void(System.Byte[])",
	"System.Private.CoreLib/System.HashCode",
	"System.Private.CoreLib/System.HashCode::Add<System.Int32>:System.Void(System.Int32)",
	"System.Private.CoreLib/System.HashCode::ToHashCode:System.Int32()",
	"System.Private.CoreLib/System.HashCode::Combine<System.Int32,System.Int32>:System.Int32(System.Int32,System.Int32)",
};

extern "C" void* kChaosExternalRuntimeFnTable[21] = {
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Guid___ctor_System_Void_System_String_),
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Guid___ctor_System_Void_System_Byte___),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Guid__NewGuid_System_Guid__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Guid__GetHashCode_System_Int32__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Guid__Parse_System_Guid_System_String_),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Guid__ToString_System_String__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Random___ctor_System_Void__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Random__Next_System_Int32__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Random__Next_System_Int32_System_Int32_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Random__NextDouble_System_Double__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Random__NextBytes_System_Void_System_Byte___),
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_HashCode__ToHashCode_System_Int32__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_HashCode__Combine_System_Int32_System_Int32__System_Int32_System_Int32_System_Int32_),
};

extern "C" int32_t kChaosExternalRuntimeCount = 21;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[14])() = {
	reinterpret_cast<void(*)()>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_6),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_7),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_8),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_9),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_10),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_11),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_12),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_13),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[14])() = {
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
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 14;
extern "C" const int kSubjectEntryIndices[14] = {
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

	/* ── Entry 0: GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry0;
	/* ── Entry 1: GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry1;
	/* ── Entry 2: GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry2;
	/* ── Entry 3: GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry3;
	/* ── Entry 4: GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry4;
	/* ── Entry 5: GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry5;
	/* ── Entry 6: GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_6 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry6;
	/* ── Entry 7: GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_7 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry7;
	/* ── Entry 8: GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_8 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry8;
	/* ── Entry 9: GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_9 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry9;
	/* ── Entry 10: GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_10 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry10;
	/* ── Entry 11: GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_11 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry11;
	/* ── Entry 12: GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_12 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry12;
	/* ── Entry 13: GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_13 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry13;
} kChaosGcSlotMapsSection = {
	/* entry0 = GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_0 */
	.entry0 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_0),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry1 = GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_1 */
	.entry1 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_1),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry2 = GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_2 */
	.entry2 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_2),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry3 = GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_3 */
	.entry3 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_3),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry4 = GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_4 */
	.entry4 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_4),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry5 = GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_5 */
	.entry5 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_5),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry6 = GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_6 */
	.entry6 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_6),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry7 = GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_7 */
	.entry7 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_7),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry8 = GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_8 */
	.entry8 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_8),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry9 = GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_9 */
	.entry9 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_9),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry10 = GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_10 */
	.entry10 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_10),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry11 = GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_11 */
	.entry11 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_11),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry12 = GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_12 */
	.entry12 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_12),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry13 = GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_13 */
	.entry13 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_13),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 500u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[14] = {
	reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_0),
	reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_1),
	reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_2),
	reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_3),
	reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_4),
	reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_5),
	reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_6),
	reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_7),
	reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_8),
	reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_9),
	reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_10),
	reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_11),
	reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_12),
	reinterpret_cast<void*>(&GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_13),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 14u,
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
	.image_name_utf8    = "GuidRandomHashcodeSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects[1] = {
	{ 0u, "GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects[14] = {
	{ 0u, "GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_6:System.Void()", "Subject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_7:System.Void()", "Subject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_8:System.Void()", "Subject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_9:System.Void()", "Subject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_10:System.Void()", "Subject_10", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_11:System.Void()", "Subject_11", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_12:System.Void()", "Subject_12", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_13:System.Void()", "Subject_13", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0u, "GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects", "GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects", "", "GuidRandomHashcodeSubjects", "GuidRandomHashcodeSubjects", nullptr, kReflFields_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects, 1u, nullptr, 0u,
	kReflMethods_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects, 14u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "GuidRandomHashcodeSubjects", kReflTypePtrs, 1u };

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
// Managed method: GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_0()
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("00000000-0000-0000-0000-000000000000");
			}}
			{
				const auto chaos_arg_1 = _s0;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Guid, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Guid.hot;
				chaos_external_runtime_System_Private_CoreLib_System_Guid___ctor_System_Void_System_String_(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), chaos_arg_1);
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
				chaos_static_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_1()
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_1(void)
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


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
			{
				const auto chaos_length = static_cast<CHAOS_IL2CPP_INT32>(_s0);
				if (chaos_length < 0)
				{
					CHAOS_IL2CPP_FAIL();
				}
				auto* chaos_array = CHAOS_IL2CPP_NEW_GC(chaos_managed_array, {});
				chaos_array->header.type_info = &chaos_type_info_managed_array.hot;
				chaos_array->element_type_shape = 1;
				chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Byte.hot;
				chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
				chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
			}
			_s1 = _s0;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(73644948u);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_raw_arg_0 = _s1;
				const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
				chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
			}
			{
				const auto chaos_arg_1 = _s0;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Guid, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Guid.hot;
				chaos_external_runtime_System_Private_CoreLib_System_Guid___ctor_System_Void_System_Byte___(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), chaos_arg_1);
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
				chaos_static_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_2()
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_2(void)
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


	CHAOS_EH_TRY
			{
				const auto chaos_result = ChaosGuidNewGuid();
				_s0 = ChaosStoreInt64(chaos_result);
			}
			chaos_locals[1] = _s0;
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosGuidGetHashCode(ChaosLoadInt64(chaos_arg_0));
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_result = ChaosGuidNewGuid();
				_s1 = ChaosStoreInt64(chaos_result);
			}
			chaos_locals[1] = _s1;
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosGuidGetHashCode(ChaosLoadInt64(chaos_arg_0));
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
				chaos_static_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects___exitCode = chaos_value;
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
				chaos_static_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_3()
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_3(void)
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
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("00000000-0000-0000-0000-000000000000");
			}}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosGuidParse(chaos_arg_0);
				_s0 = ChaosStoreInt64(chaos_result);
			}
			chaos_locals[1] = _s0;
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosGuidGetHashCode(ChaosLoadInt64(chaos_arg_0));
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("00000000-0000-0000-0000-000000000000");
			}}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosGuidParse(chaos_arg_0);
				_s1 = ChaosStoreInt64(chaos_result);
			}
			chaos_locals[1] = _s1;
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosGuidGetHashCode(ChaosLoadInt64(chaos_arg_0));
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
				chaos_static_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects___exitCode = chaos_value;
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
				chaos_static_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_4()
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_4(void)
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
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("00000000-0000-0000-0000-000000000000");
			}}
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[9])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{{
				_s3 = CHAOS_IL2CPP_STRING_ID("00000000-0000-0000-0000-000000000000");
			}}
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_arg_1 = _s4;
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[9])(chaos_arg_0, chaos_arg_1);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			chaos_locals[0] = _s4;
			_s4 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s5;
				chaos_static_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects___exitCode = chaos_value;
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
				chaos_static_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_5()
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_5(void)
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
			{
				const auto chaos_result = ChaosGuidNewGuid();
				_s0 = ChaosStoreInt64(chaos_result);
			}
			chaos_locals[1] = _s0;
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosGuidToString(ChaosLoadInt64(chaos_arg_0));
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
			{
				const auto chaos_result = ChaosGuidNewGuid();
				_s1 = ChaosStoreInt64(chaos_result);
			}
			chaos_locals[1] = _s1;
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosGuidToString(ChaosLoadInt64(chaos_arg_0));
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
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects___exitCode = chaos_value;
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
				chaos_static_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_6()
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_6(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Random, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Random.hot;
				chaos_external_runtime_System_Private_CoreLib_System_Random___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
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
				chaos_static_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_7()
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_7(void)
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Random, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Random.hot;
				chaos_external_runtime_System_Private_CoreLib_System_Random___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			{
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Random__Next_System_Int32__();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Random, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Random.hot;
				chaos_external_runtime_System_Private_CoreLib_System_Random___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
				_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			{
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Random__Next_System_Int32__();
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
				chaos_static_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects___exitCode = chaos_value;
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
				chaos_static_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_8()
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_8(void)
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
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Random, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Random.hot;
				chaos_external_runtime_System_Private_CoreLib_System_Random___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s1;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Random__Next_System_Int32_System_Int32_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Random, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Random.hot;
				chaos_external_runtime_System_Private_CoreLib_System_Random___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
				_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s3;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Random__Next_System_Int32_System_Int32_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
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
				chaos_static_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects___exitCode = chaos_value;
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
				chaos_static_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_9()
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_9(void)
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
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Random, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Random.hot;
				chaos_external_runtime_System_Private_CoreLib_System_Random___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosRandomNextDouble(chaos_arg_0);
				_s0 = ChaosStoreInt64(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Random, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Random.hot;
				chaos_external_runtime_System_Private_CoreLib_System_Random___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosRandomNextDouble(chaos_arg_0);
				_s1 = ChaosStoreInt64(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects___exitCode = chaos_value;
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
				chaos_static_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_10()
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_10(void)
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


	CHAOS_EH_TRY
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Random, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Random.hot;
				chaos_external_runtime_System_Private_CoreLib_System_Random___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
			{
				const auto chaos_length = static_cast<CHAOS_IL2CPP_INT32>(_s1);
				if (chaos_length < 0)
				{
					CHAOS_IL2CPP_FAIL();
				}
				auto* chaos_array = CHAOS_IL2CPP_NEW_GC(chaos_managed_array, {});
				chaos_array->header.type_info = &chaos_type_info_managed_array.hot;
				chaos_array->element_type_shape = 1;
				chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Byte.hot;
				chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
				chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
			}
			_s2 = _s1;
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(73644948u);
			{
				const auto chaos_arg_1 = _s3;
				const auto chaos_raw_arg_0 = _s2;
				const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
				chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
			}
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				ChaosRandomNextBytes(chaos_arg_0, chaos_arg_1);
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
				chaos_static_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_11()
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_11(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	CHAOS_EH_TRY
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			_s1 = _s0;
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
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[18])(chaos_arg_0);
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
				chaos_static_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_12()
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_12(void)
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


	CHAOS_EH_TRY
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			_s1 = _s0;
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
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_HashCode__ToHashCode_System_Int32__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			_s2 = _s1;
			{
				const auto chaos_address = _s2;
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
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_HashCode__ToHashCode_System_Int32__(chaos_arg_0);
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
				chaos_static_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects___exitCode = chaos_value;
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
				chaos_static_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: GuidRandomHashcodeSubjects/GuidRandomHashcodeSubjects::Subject_13()
extern "C" void GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects_Subject_13(void)
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
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_HashCode__Combine_System_Int32_System_Int32__System_Int32_System_Int32_System_Int32_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_HashCode__Combine_System_Int32_System_Int32__System_Int32_System_Int32_System_Int32_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
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
				chaos_static_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects___exitCode = chaos_value;
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
				chaos_static_GuidRandomHashcodeSubjects_GuidRandomHashcodeSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}



}  // namespace chaos::il2cpp::codegen::GuidRandomHashcodeSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 14;

extern "C" void ChaosJitRegisterAll() {}