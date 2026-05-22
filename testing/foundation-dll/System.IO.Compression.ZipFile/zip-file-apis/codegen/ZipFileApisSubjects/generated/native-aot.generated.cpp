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

namespace chaos::il2cpp::codegen::ZipFileApisSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_IO_MemoryStream = {{nullptr, nullptr, 7740506271772842812ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_IO_MemoryStream = static_cast<CHAOS_IL2CPP_INTPTR>(7740506271772842812ULL);
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
inline TypeInfoV0 chaos_mt_ZipFileApisSubjects_ZipFileApisSubjects = {{nullptr, nullptr, 18185106730109511232ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ZipFileApisSubjects_ZipFileApisSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(18185106730109511232ULL);

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

struct chaos_type_System_Private_CoreLib_System_IO_MemoryStream
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

struct chaos_type_ZipFileApisSubjects_ZipFileApisSubjects
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

CHAOS_IL2CPP_INT32 chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = 0;


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

static void chaos_external_runtime_System_Private_CoreLib_System_IO_MemoryStream___ctor_System_Void__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	ChaosMemoryStreamCtor(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosObjectGetHashCode(chaos_fn_arg_0);
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_0(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_1(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_2(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_3(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_4(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_5(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_6(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_7(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_8(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_9(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_10(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_11(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_12(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_13(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_14(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_15(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_16(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_17(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_18(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_19(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_20(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_21(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_22(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_23(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_24(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_25(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_26(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_27(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_28(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_29(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_30(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_31(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_32(void);
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_33(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[35] = {
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
	::ChaosAbiMethodEntryV0 entries[34];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		34u,
		0u,
		2212991509u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_0
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_1
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_2
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_3
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_4
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_5
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_6
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_7
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_8
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_9
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_10
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_11
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_12
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_13
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_14
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_15
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_16
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_17
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_18
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_19
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_20
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_21
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_22
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_23
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_24
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_25
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_26
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_27
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_28
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_29
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_30
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_31
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_32
		{ 0u, 0u },  // ZipFileApisSubjects_ZipFileApisSubjects_Subject_33
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
		"ZipFileApisSubjects",
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
		&chaos_mt_ZipFileApisSubjects_ZipFileApisSubjects.hot,
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
		/* .name_utf8         = */ "ZipFileApisSubjects",
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
		::chaos::il2cpp::runtime_core::RegisterModule("ZipFileApisSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[34] = {
	{ "Subject_0", 0x00000003u, 0u },  // ZipFileApisSubjects
	{ "Subject_1", 0x00000004u, 0u },  // ZipFileApisSubjects
	{ "Subject_2", 0x00000005u, 0u },  // ZipFileApisSubjects
	{ "Subject_3", 0x00000006u, 0u },  // ZipFileApisSubjects
	{ "Subject_4", 0x00000007u, 0u },  // ZipFileApisSubjects
	{ "Subject_5", 0x00000008u, 0u },  // ZipFileApisSubjects
	{ "Subject_6", 0x00000009u, 0u },  // ZipFileApisSubjects
	{ "Subject_7", 0x0000000Au, 0u },  // ZipFileApisSubjects
	{ "Subject_8", 0x0000000Bu, 0u },  // ZipFileApisSubjects
	{ "Subject_9", 0x0000000Cu, 0u },  // ZipFileApisSubjects
	{ "Subject_10", 0x0000000Du, 0u },  // ZipFileApisSubjects
	{ "Subject_11", 0x0000000Eu, 0u },  // ZipFileApisSubjects
	{ "Subject_12", 0x0000000Fu, 0u },  // ZipFileApisSubjects
	{ "Subject_13", 0x00000010u, 0u },  // ZipFileApisSubjects
	{ "Subject_14", 0x00000011u, 0u },  // ZipFileApisSubjects
	{ "Subject_15", 0x00000012u, 0u },  // ZipFileApisSubjects
	{ "Subject_16", 0x00000013u, 0u },  // ZipFileApisSubjects
	{ "Subject_17", 0x00000014u, 0u },  // ZipFileApisSubjects
	{ "Subject_18", 0x00000015u, 0u },  // ZipFileApisSubjects
	{ "Subject_19", 0x00000016u, 0u },  // ZipFileApisSubjects
	{ "Subject_20", 0x00000017u, 0u },  // ZipFileApisSubjects
	{ "Subject_21", 0x00000018u, 0u },  // ZipFileApisSubjects
	{ "Subject_22", 0x00000019u, 0u },  // ZipFileApisSubjects
	{ "Subject_23", 0x0000001Au, 0u },  // ZipFileApisSubjects
	{ "Subject_24", 0x0000001Bu, 0u },  // ZipFileApisSubjects
	{ "Subject_25", 0x0000001Cu, 0u },  // ZipFileApisSubjects
	{ "Subject_26", 0x0000001Du, 0u },  // ZipFileApisSubjects
	{ "Subject_27", 0x0000001Eu, 0u },  // ZipFileApisSubjects
	{ "Subject_28", 0x0000001Fu, 0u },  // ZipFileApisSubjects
	{ "Subject_29", 0x00000020u, 0u },  // ZipFileApisSubjects
	{ "Subject_30", 0x00000021u, 0u },  // ZipFileApisSubjects
	{ "Subject_31", 0x00000022u, 0u },  // ZipFileApisSubjects
	{ "Subject_32", 0x00000023u, 0u },  // ZipFileApisSubjects
	{ "Subject_33", 0x00000024u, 0u },  // ZipFileApisSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "ZipFileApisSubjects", "", 0u, 34u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[34] = {
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
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[34] = {
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_0
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_1
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_2
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_3
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_4
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_5
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_6
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_7
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_8
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_9
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_10
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_11
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_12
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_13
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_14), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_14
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_15), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_15
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_16), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_16
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_17), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_17
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_18), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_18
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_19), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_19
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_20), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_20
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_21), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_21
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_22), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_22
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_23), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_23
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_24), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_24
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_25), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_25
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_26), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_26
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_27), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_27
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_28), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_28
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_29), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_29
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_30), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_30
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_31), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_31
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_32
	{ reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_33), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ZipFileApisSubjects::Subject_33
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"ZipFileApisSubjects",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	34u,
	s_hotpatch_slots,
	34u,
	s_hotpatch_entries,
	34u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[30] = {
	"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.String)",
	"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode",
	"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.String,CompressionLevel,System.Boolean)",
	"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.String,CompressionLevel,System.Boolean,System.Text.Encoding)",
	"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()",
	"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream)",
	"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream,CompressionLevel,System.Boolean)",
	"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream,CompressionLevel,System.Boolean,System.Text.Encoding)",
	"System.Private.CoreLib/System.Threading.CancellationToken",
	"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Threading.CancellationToken)",
	"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()",
	"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,CompressionLevel,System.Boolean,System.Threading.CancellationToken)",
	"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,CompressionLevel,System.Boolean,System.Text.Encoding,System.Threading.CancellationToken)",
	"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,System.Threading.CancellationToken)",
	"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,CompressionLevel,System.Boolean,System.Threading.CancellationToken)",
	"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,CompressionLevel,System.Boolean,System.Text.Encoding,System.Threading.CancellationToken)",
	"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String)",
	"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String,System.Text.Encoding,System.Boolean)",
	"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String)",
	"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String,System.Text.Encoding,System.Boolean)",
	"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Threading.CancellationToken)",
	"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Text.Encoding,System.Boolean,System.Threading.CancellationToken)",
	"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Threading.CancellationToken)",
	"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Text.Encoding,System.Boolean,System.Threading.CancellationToken)",
	"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Open:ZipArchive(System.String,ZipArchiveMode)",
	"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Open:ZipArchive(System.String,ZipArchiveMode,System.Text.Encoding)",
	"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenAsync:System.Threading.Tasks.Task<ZipArchive>(System.String,ZipArchiveMode,System.Threading.CancellationToken)",
	"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenAsync:System.Threading.Tasks.Task<ZipArchive>(System.String,ZipArchiveMode,System.Text.Encoding,System.Threading.CancellationToken)",
	"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenRead:ZipArchive(System.String)",
	"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenReadAsync:System.Threading.Tasks.Task<ZipArchive>(System.String,System.Threading.CancellationToken)",
};

extern "C" void* kChaosExternalRuntimeFnTable[30] = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_IO_MemoryStream___ctor_System_Void__),
	nullptr,
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

extern "C" int32_t kChaosExternalRuntimeCount = 30;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[34])() = {
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_6),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_7),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_8),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_9),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_10),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_11),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_12),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_13),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_14),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_15),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_16),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_17),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_18),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_19),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_20),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_21),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_22),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_23),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_24),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_25),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_26),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_27),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_28),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_29),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_30),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_31),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_32),
	reinterpret_cast<void(*)()>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_33),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[34])() = {
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
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 34;
extern "C" const int kSubjectEntryIndices[34] = {
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

	/* ── Entry 0: ZipFileApisSubjects_ZipFileApisSubjects_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry0;
	/* ── Entry 1: ZipFileApisSubjects_ZipFileApisSubjects_Subject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry1;
	/* ── Entry 2: ZipFileApisSubjects_ZipFileApisSubjects_Subject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry2;
	/* ── Entry 3: ZipFileApisSubjects_ZipFileApisSubjects_Subject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry3;
	/* ── Entry 4: ZipFileApisSubjects_ZipFileApisSubjects_Subject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry4;
	/* ── Entry 5: ZipFileApisSubjects_ZipFileApisSubjects_Subject_5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry5;
	/* ── Entry 6: ZipFileApisSubjects_ZipFileApisSubjects_Subject_6 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry6;
	/* ── Entry 7: ZipFileApisSubjects_ZipFileApisSubjects_Subject_7 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry7;
	/* ── Entry 8: ZipFileApisSubjects_ZipFileApisSubjects_Subject_8 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[9];
	} entry8;
	/* ── Entry 9: ZipFileApisSubjects_ZipFileApisSubjects_Subject_9 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry9;
	/* ── Entry 10: ZipFileApisSubjects_ZipFileApisSubjects_Subject_10 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry10;
	/* ── Entry 11: ZipFileApisSubjects_ZipFileApisSubjects_Subject_11 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[9];
	} entry11;
	/* ── Entry 12: ZipFileApisSubjects_ZipFileApisSubjects_Subject_12 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry12;
	/* ── Entry 13: ZipFileApisSubjects_ZipFileApisSubjects_Subject_15 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry13;
	/* ── Entry 14: ZipFileApisSubjects_ZipFileApisSubjects_Subject_16 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry14;
	/* ── Entry 15: ZipFileApisSubjects_ZipFileApisSubjects_Subject_19 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry15;
	/* ── Entry 16: ZipFileApisSubjects_ZipFileApisSubjects_Subject_20 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry16;
	/* ── Entry 17: ZipFileApisSubjects_ZipFileApisSubjects_Subject_23 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry17;
	/* ── Entry 18: ZipFileApisSubjects_ZipFileApisSubjects_Subject_24 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry18;
	/* ── Entry 19: ZipFileApisSubjects_ZipFileApisSubjects_Subject_27 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry19;
	/* ── Entry 20: ZipFileApisSubjects_ZipFileApisSubjects_Subject_28 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry20;
	/* ── Entry 21: ZipFileApisSubjects_ZipFileApisSubjects_Subject_29 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry21;
	/* ── Entry 22: ZipFileApisSubjects_ZipFileApisSubjects_Subject_30 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry22;
	/* ── Entry 23: ZipFileApisSubjects_ZipFileApisSubjects_Subject_31 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry23;
	/* ── Entry 24: ZipFileApisSubjects_ZipFileApisSubjects_Subject_32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry24;
	/* ── Entry 25: ZipFileApisSubjects_ZipFileApisSubjects_Subject_33 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry25;
} kChaosGcSlotMapsSection = {
	/* entry0 = ZipFileApisSubjects_ZipFileApisSubjects_Subject_0 */
	.entry0 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_0),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry1 = ZipFileApisSubjects_ZipFileApisSubjects_Subject_1 */
	.entry1 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_1),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry2 = ZipFileApisSubjects_ZipFileApisSubjects_Subject_2 */
	.entry2 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_2),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry3 = ZipFileApisSubjects_ZipFileApisSubjects_Subject_3 */
	.entry3 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_3),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry4 = ZipFileApisSubjects_ZipFileApisSubjects_Subject_4 */
	.entry4 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_4),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry5 = ZipFileApisSubjects_ZipFileApisSubjects_Subject_5 */
	.entry5 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_5),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry6 = ZipFileApisSubjects_ZipFileApisSubjects_Subject_6 */
	.entry6 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_6),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry7 = ZipFileApisSubjects_ZipFileApisSubjects_Subject_7 */
	.entry7 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_7),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry8 = ZipFileApisSubjects_ZipFileApisSubjects_Subject_8 */
	.entry8 = {
		/* entry_total_size = 56 */ 56u,
		/* code_address */ reinterpret_cast<const void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_8),
		/* frame_size = 72 */ 72u,
		/* num_gc_slots = 9 */ 9u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u }
	},
	/* entry9 = ZipFileApisSubjects_ZipFileApisSubjects_Subject_9 */
	.entry9 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_9),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry10 = ZipFileApisSubjects_ZipFileApisSubjects_Subject_10 */
	.entry10 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_10),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry11 = ZipFileApisSubjects_ZipFileApisSubjects_Subject_11 */
	.entry11 = {
		/* entry_total_size = 56 */ 56u,
		/* code_address */ reinterpret_cast<const void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_11),
		/* frame_size = 72 */ 72u,
		/* num_gc_slots = 9 */ 9u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u }
	},
	/* entry12 = ZipFileApisSubjects_ZipFileApisSubjects_Subject_12 */
	.entry12 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_12),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry13 = ZipFileApisSubjects_ZipFileApisSubjects_Subject_15 */
	.entry13 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_15),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry14 = ZipFileApisSubjects_ZipFileApisSubjects_Subject_16 */
	.entry14 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_16),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry15 = ZipFileApisSubjects_ZipFileApisSubjects_Subject_19 */
	.entry15 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_19),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry16 = ZipFileApisSubjects_ZipFileApisSubjects_Subject_20 */
	.entry16 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_20),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry17 = ZipFileApisSubjects_ZipFileApisSubjects_Subject_23 */
	.entry17 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_23),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry18 = ZipFileApisSubjects_ZipFileApisSubjects_Subject_24 */
	.entry18 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_24),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry19 = ZipFileApisSubjects_ZipFileApisSubjects_Subject_27 */
	.entry19 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_27),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry20 = ZipFileApisSubjects_ZipFileApisSubjects_Subject_28 */
	.entry20 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_28),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry21 = ZipFileApisSubjects_ZipFileApisSubjects_Subject_29 */
	.entry21 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_29),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry22 = ZipFileApisSubjects_ZipFileApisSubjects_Subject_30 */
	.entry22 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_30),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry23 = ZipFileApisSubjects_ZipFileApisSubjects_Subject_31 */
	.entry23 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_31),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry24 = ZipFileApisSubjects_ZipFileApisSubjects_Subject_32 */
	.entry24 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_32),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry25 = ZipFileApisSubjects_ZipFileApisSubjects_Subject_33 */
	.entry25 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_33),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 1072u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[34] = {
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_0),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_1),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_2),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_3),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_4),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_5),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_6),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_7),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_8),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_9),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_10),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_11),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_12),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_13),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_14),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_15),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_16),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_17),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_18),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_19),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_20),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_21),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_22),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_23),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_24),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_25),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_26),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_27),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_28),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_29),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_30),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_31),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_32),
	reinterpret_cast<void*>(&ZipFileApisSubjects_ZipFileApisSubjects_Subject_33),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 34u,
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
	.image_name_utf8    = "ZipFileApisSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_ZipFileApisSubjects_ZipFileApisSubjects[1] = {
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_ZipFileApisSubjects_ZipFileApisSubjects[34] = {
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_6:System.Void()", "Subject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_7:System.Void()", "Subject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_8:System.Void()", "Subject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_9:System.Void()", "Subject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_10:System.Void()", "Subject_10", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_11:System.Void()", "Subject_11", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_12:System.Void()", "Subject_12", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_13:System.Void()", "Subject_13", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_14:System.Void()", "Subject_14", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_15:System.Void()", "Subject_15", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_16:System.Void()", "Subject_16", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_17:System.Void()", "Subject_17", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_18:System.Void()", "Subject_18", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_19:System.Void()", "Subject_19", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_20:System.Void()", "Subject_20", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_21:System.Void()", "Subject_21", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_22:System.Void()", "Subject_22", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_23:System.Void()", "Subject_23", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_24:System.Void()", "Subject_24", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_25:System.Void()", "Subject_25", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_26:System.Void()", "Subject_26", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_27:System.Void()", "Subject_27", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_28:System.Void()", "Subject_28", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_29:System.Void()", "Subject_29", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_30:System.Void()", "Subject_30", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_31:System.Void()", "Subject_31", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_32:System.Void()", "Subject_32", "System.Void", 0, nullptr, 0u },
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects::Subject_33:System.Void()", "Subject_33", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0u, "ZipFileApisSubjects/ZipFileApisSubjects", "ZipFileApisSubjects/ZipFileApisSubjects", "", "ZipFileApisSubjects", "ZipFileApisSubjects", nullptr, kReflFields_ZipFileApisSubjects_ZipFileApisSubjects, 1u, nullptr, 0u,
	kReflMethods_ZipFileApisSubjects_ZipFileApisSubjects, 34u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "ZipFileApisSubjects", kReflTypePtrs, 1u };

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
// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_0()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[0])(chaos_arg_0, chaos_arg_1);
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_1()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				const auto chaos_arg_3 = _s3;
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[2])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_2()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_2(void)
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
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = 0;
			{
				const auto chaos_arg_4 = _s4;
				const auto chaos_arg_3 = _s3;
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[3])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_3()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_IO_MemoryStream, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_IO_MemoryStream.hot;
				chaos_external_runtime_System_Private_CoreLib_System_IO_MemoryStream___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[5])(chaos_arg_0, chaos_arg_1);
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_4()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_IO_MemoryStream, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_IO_MemoryStream.hot;
				chaos_external_runtime_System_Private_CoreLib_System_IO_MemoryStream___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				const auto chaos_arg_3 = _s3;
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[6])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_5()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_5(void)
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
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_IO_MemoryStream, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_IO_MemoryStream.hot;
				chaos_external_runtime_System_Private_CoreLib_System_IO_MemoryStream___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = 0;
			{
				const auto chaos_arg_4 = _s4;
				const auto chaos_arg_3 = _s3;
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[7])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_6()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_6(void)
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


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
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
			_s2 = chaos_locals[1];
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[9])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{{
				_s2 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_address = _s3;
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
			_s3 = chaos_locals[1];
			{
				const auto chaos_arg_2 = _s3;
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[9])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_7()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_7(void)
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
	CHAOS_IL2CPP_INTPTR _s21{};
	CHAOS_IL2CPP_INTPTR _s22{};


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_address = _s4;
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
			_s4 = chaos_locals[1];
			{
				const auto chaos_arg_4 = _s4;
				const auto chaos_arg_3 = _s3;
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[11])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{{
				_s2 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s5 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_address = _s5;
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
			_s5 = chaos_locals[1];
			{
				const auto chaos_arg_4 = _s5;
				const auto chaos_arg_3 = _s4;
				const auto chaos_arg_2 = _s3;
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[11])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_8()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_8(void)
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
	CHAOS_IL2CPP_INTPTR _s21{};
	CHAOS_IL2CPP_INTPTR _s22{};
	CHAOS_IL2CPP_INTPTR _s23{};
	CHAOS_IL2CPP_INTPTR _s24{};


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = 0;
			_s5 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_address = _s5;
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
			_s5 = chaos_locals[1];
			{
				const auto chaos_arg_5 = _s5;
				const auto chaos_arg_4 = _s4;
				const auto chaos_arg_3 = _s3;
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[12])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4, chaos_arg_5);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{{
				_s2 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s5 = 0;
			_s6 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_address = _s6;
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
			_s6 = chaos_locals[1];
			{
				const auto chaos_arg_5 = _s6;
				const auto chaos_arg_4 = _s5;
				const auto chaos_arg_3 = _s4;
				const auto chaos_arg_2 = _s3;
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[12])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4, chaos_arg_5);
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_9()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_9(void)
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


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_IO_MemoryStream, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_IO_MemoryStream.hot;
				chaos_external_runtime_System_Private_CoreLib_System_IO_MemoryStream___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
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
			_s2 = chaos_locals[1];
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[13])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_IO_MemoryStream, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_IO_MemoryStream.hot;
				chaos_external_runtime_System_Private_CoreLib_System_IO_MemoryStream___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
				_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_address = _s3;
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
			_s3 = chaos_locals[1];
			{
				const auto chaos_arg_2 = _s3;
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[13])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_10()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_10(void)
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
	CHAOS_IL2CPP_INTPTR _s21{};
	CHAOS_IL2CPP_INTPTR _s22{};


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_IO_MemoryStream, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_IO_MemoryStream.hot;
				chaos_external_runtime_System_Private_CoreLib_System_IO_MemoryStream___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_address = _s4;
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
			_s4 = chaos_locals[1];
			{
				const auto chaos_arg_4 = _s4;
				const auto chaos_arg_3 = _s3;
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[14])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_IO_MemoryStream, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_IO_MemoryStream.hot;
				chaos_external_runtime_System_Private_CoreLib_System_IO_MemoryStream___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
				_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s5 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_address = _s5;
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
			_s5 = chaos_locals[1];
			{
				const auto chaos_arg_4 = _s5;
				const auto chaos_arg_3 = _s4;
				const auto chaos_arg_2 = _s3;
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[14])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_11()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_11(void)
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
	CHAOS_IL2CPP_INTPTR _s21{};
	CHAOS_IL2CPP_INTPTR _s22{};
	CHAOS_IL2CPP_INTPTR _s23{};
	CHAOS_IL2CPP_INTPTR _s24{};


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_IO_MemoryStream, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_IO_MemoryStream.hot;
				chaos_external_runtime_System_Private_CoreLib_System_IO_MemoryStream___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = 0;
			_s5 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_address = _s5;
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
			_s5 = chaos_locals[1];
			{
				const auto chaos_arg_5 = _s5;
				const auto chaos_arg_4 = _s4;
				const auto chaos_arg_3 = _s3;
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[15])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4, chaos_arg_5);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_IO_MemoryStream, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_IO_MemoryStream.hot;
				chaos_external_runtime_System_Private_CoreLib_System_IO_MemoryStream___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
				_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s5 = 0;
			_s6 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_address = _s6;
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
			_s6 = chaos_locals[1];
			{
				const auto chaos_arg_5 = _s6;
				const auto chaos_arg_4 = _s5;
				const auto chaos_arg_3 = _s4;
				const auto chaos_arg_2 = _s3;
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[15])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4, chaos_arg_5);
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_12()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_12(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[16])(chaos_arg_0, chaos_arg_1);
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_13()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_13(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_14()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_14(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_15()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_15(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s2 = 0;
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				const auto chaos_arg_3 = _s3;
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[17])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_16()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_16(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_IO_MemoryStream, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_IO_MemoryStream.hot;
				chaos_external_runtime_System_Private_CoreLib_System_IO_MemoryStream___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[18])(chaos_arg_0, chaos_arg_1);
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_17()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_17(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_18()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_18(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_19()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_19(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	CHAOS_EH_TRY
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_IO_MemoryStream, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_IO_MemoryStream.hot;
				chaos_external_runtime_System_Private_CoreLib_System_IO_MemoryStream___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s2 = 0;
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				const auto chaos_arg_3 = _s3;
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[19])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_20()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_20(void)
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


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
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
			_s2 = chaos_locals[1];
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[20])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{{
				_s2 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_address = _s3;
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
			_s3 = chaos_locals[1];
			{
				const auto chaos_arg_2 = _s3;
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[20])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_21()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_21(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_22()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_22(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_23()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_23(void)
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
	CHAOS_IL2CPP_INTPTR _s21{};
	CHAOS_IL2CPP_INTPTR _s22{};


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s2 = 0;
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_address = _s4;
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
			_s4 = chaos_locals[1];
			{
				const auto chaos_arg_4 = _s4;
				const auto chaos_arg_3 = _s3;
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[21])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{{
				_s2 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s3 = 0;
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s5 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_address = _s5;
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
			_s5 = chaos_locals[1];
			{
				const auto chaos_arg_4 = _s5;
				const auto chaos_arg_3 = _s4;
				const auto chaos_arg_2 = _s3;
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[21])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_24()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_24(void)
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


	CHAOS_EH_TRY
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_IO_MemoryStream, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_IO_MemoryStream.hot;
				chaos_external_runtime_System_Private_CoreLib_System_IO_MemoryStream___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
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
			_s2 = chaos_locals[1];
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[22])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_IO_MemoryStream, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_IO_MemoryStream.hot;
				chaos_external_runtime_System_Private_CoreLib_System_IO_MemoryStream___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			{{
				_s2 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_address = _s3;
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
			_s3 = chaos_locals[1];
			{
				const auto chaos_arg_2 = _s3;
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[22])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_25()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_25(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_26()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_26(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_27()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_27(void)
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
	CHAOS_IL2CPP_INTPTR _s21{};
	CHAOS_IL2CPP_INTPTR _s22{};


	CHAOS_EH_TRY
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_IO_MemoryStream, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_IO_MemoryStream.hot;
				chaos_external_runtime_System_Private_CoreLib_System_IO_MemoryStream___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s2 = 0;
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_address = _s4;
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
			_s4 = chaos_locals[1];
			{
				const auto chaos_arg_4 = _s4;
				const auto chaos_arg_3 = _s3;
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[23])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_IO_MemoryStream, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_IO_MemoryStream.hot;
				chaos_external_runtime_System_Private_CoreLib_System_IO_MemoryStream___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			{{
				_s2 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s3 = 0;
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s5 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_address = _s5;
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
			_s5 = chaos_locals[1];
			{
				const auto chaos_arg_4 = _s5;
				const auto chaos_arg_3 = _s4;
				const auto chaos_arg_2 = _s3;
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[23])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_28()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_28(void)
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
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[24])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[24])(chaos_arg_0, chaos_arg_1);
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_29()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_29(void)
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
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = 0;
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[25])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s3 = 0;
			{
				const auto chaos_arg_2 = _s3;
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[25])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_30()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_30(void)
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


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
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
			_s2 = chaos_locals[1];
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[26])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_address = _s3;
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
			_s3 = chaos_locals[1];
			{
				const auto chaos_arg_2 = _s3;
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[26])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_31()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_31(void)
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
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = 0;
			_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_address = _s3;
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
			_s3 = chaos_locals[1];
			{
				const auto chaos_arg_3 = _s3;
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[27])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s3 = 0;
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_address = _s4;
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
			_s4 = chaos_locals[1];
			{
				const auto chaos_arg_3 = _s4;
				const auto chaos_arg_2 = _s3;
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[27])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_32()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_32(void)
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
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[28])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[28])(chaos_arg_0);
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ZipFileApisSubjects/ZipFileApisSubjects::Subject_33()
extern "C" void ZipFileApisSubjects_ZipFileApisSubjects_Subject_33(void)
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
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
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
			_s1 = chaos_locals[1];
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[29])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
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
			_s2 = chaos_locals[1];
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[29])(chaos_arg_0, chaos_arg_1);
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
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
				chaos_static_ZipFileApisSubjects_ZipFileApisSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}



}  // namespace chaos::il2cpp::codegen::ZipFileApisSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 34;

// ── JIT Method Entry Table ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kMethodJson_0[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-0\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_0:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_0()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_0:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_0:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-0\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_0()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":20,\"handlerOffset\":21,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":7,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":12,\"resultType\":\"System.Void\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":32,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":21,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":24,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":32,\"ilOffset\":30,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":32,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_1[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-1\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_1:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_1()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_1:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_1:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-1\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_1()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":22,\"handlerOffset\":23,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":7,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":14,\"resultType\":\"System.Void\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.String,CompressionLevel,System.Boolean)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.String,CompressionLevel,System.Boolean)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.String,CompressionLevel,System.Boolean)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":34,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":23,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":26,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":34,\"ilOffset\":32,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":34,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_2[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-2\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_2:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_2()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_2:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_2:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-2\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_2()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_2\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":23,\"handlerOffset\":24,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":7,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":14,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":15,\"resultType\":\"System.Void\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.String,CompressionLevel,System.Boolean,System.Text.Encoding)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.String,CompressionLevel,System.Boolean,System.Text.Encoding)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.String,CompressionLevel,System.Boolean,System.Text.Encoding)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":35,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":24,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":27,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":35,\"ilOffset\":33,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":35,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_3[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-3\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_3:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_3()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_3:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_3:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-3\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_3()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_3\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":20,\"handlerOffset\":21,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":7,\"resultType\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"callee\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":12,\"resultType\":\"System.Void\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":32,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":21,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":24,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":32,\"ilOffset\":30,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":32,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_4[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-4\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_4:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_4()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_4:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_4:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-4\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_4()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_4\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":22,\"handlerOffset\":23,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":7,\"resultType\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"callee\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":14,\"resultType\":\"System.Void\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream,CompressionLevel,System.Boolean)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream,CompressionLevel,System.Boolean)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream,CompressionLevel,System.Boolean)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":34,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":23,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":26,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":34,\"ilOffset\":32,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":34,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_5[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-5\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_5:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_5()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_5:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_5:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-5\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_5()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_5\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":23,\"handlerOffset\":24,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":7,\"resultType\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"callee\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":14,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":15,\"resultType\":\"System.Void\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream,CompressionLevel,System.Boolean,System.Text.Encoding)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream,CompressionLevel,System.Boolean,System.Text.Encoding)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectory:System.Void(System.String,System.IO.Stream,CompressionLevel,System.Boolean,System.Text.Encoding)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":35,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":24,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":27,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":35,\"ilOffset\":33,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":35,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_6[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-6\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_6:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_6()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_6:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_6:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-6\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_6()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_6\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":77,\"handlerOffset\":78,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":7,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":12,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":14,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":20,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":21,\"resultType\":\"System.Threading.Tasks.Task\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":31,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":36,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":41,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":43,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":49,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":50,\"resultType\":\"System.Threading.Tasks.Task\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":55,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":60,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":62,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":63,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":65,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":66,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":75,\"ilOffset\":67,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":69,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":70,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":89,\"ilOffset\":76,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":78,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":80,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":81,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":89,\"ilOffset\":87,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":89,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_7[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-7\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_7:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_7()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_7:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_7:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-7\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_7()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_7\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":81,\"handlerOffset\":82,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":7,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":14,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":22,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":23,\"resultType\":\"System.Threading.Tasks.Task\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,CompressionLevel,System.Boolean,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,CompressionLevel,System.Boolean,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,CompressionLevel,System.Boolean,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":33,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":38,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":43,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":45,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":47,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":53,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":54,\"resultType\":\"System.Threading.Tasks.Task\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,CompressionLevel,System.Boolean,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,CompressionLevel,System.Boolean,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,CompressionLevel,System.Boolean,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":59,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":64,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":66,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":67,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":69,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":70,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":79,\"ilOffset\":71,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":73,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":74,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":93,\"ilOffset\":80,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":82,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":84,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":85,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":93,\"ilOffset\":91,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":93,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_8[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-8\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_8:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_8()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_8:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_8:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-8\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_8()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_8\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":83,\"handlerOffset\":84,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":7,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":14,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":17,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":23,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":24,\"resultType\":\"System.Threading.Tasks.Task\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,CompressionLevel,System.Boolean,System.Text.Encoding,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,CompressionLevel,System.Boolean,System.Text.Encoding,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,CompressionLevel,System.Boolean,System.Text.Encoding,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":34,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":39,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":45,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":46,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":47,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":49,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":55,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":56,\"resultType\":\"System.Threading.Tasks.Task\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,CompressionLevel,System.Boolean,System.Text.Encoding,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,CompressionLevel,System.Boolean,System.Text.Encoding,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,CompressionLevel,System.Boolean,System.Text.Encoding,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":61,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":66,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":68,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":69,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":71,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":72,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":81,\"ilOffset\":73,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":75,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":76,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":95,\"ilOffset\":82,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":84,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":86,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":87,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":95,\"ilOffset\":93,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":95,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_9[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-9\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_9:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_9()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_9:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_9:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-9\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_9()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_9\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":77,\"handlerOffset\":78,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":7,\"resultType\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"callee\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":12,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":14,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":20,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":21,\"resultType\":\"System.Threading.Tasks.Task\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":31,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":36,\"resultType\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"callee\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":41,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":43,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":49,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":50,\"resultType\":\"System.Threading.Tasks.Task\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":55,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":60,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":62,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":63,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":65,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":66,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":75,\"ilOffset\":67,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":69,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":70,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":89,\"ilOffset\":76,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":78,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":80,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":81,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":89,\"ilOffset\":87,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":89,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_10[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-10\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_10:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_10()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_10:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_10:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-10\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_10()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_10\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":81,\"handlerOffset\":82,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":7,\"resultType\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"callee\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":14,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":22,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":23,\"resultType\":\"System.Threading.Tasks.Task\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,CompressionLevel,System.Boolean,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,CompressionLevel,System.Boolean,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,CompressionLevel,System.Boolean,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":33,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":38,\"resultType\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"callee\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":43,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":45,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":47,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":53,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":54,\"resultType\":\"System.Threading.Tasks.Task\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,CompressionLevel,System.Boolean,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,CompressionLevel,System.Boolean,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,CompressionLevel,System.Boolean,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":59,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":64,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":66,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":67,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":69,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":70,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":79,\"ilOffset\":71,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":73,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":74,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":93,\"ilOffset\":80,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":82,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":84,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":85,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":93,\"ilOffset\":91,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":93,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_11[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-11\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_11:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_11()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_11:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_11:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-11\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_11()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_11\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":83,\"handlerOffset\":84,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":7,\"resultType\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"callee\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":14,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":17,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":23,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":24,\"resultType\":\"System.Threading.Tasks.Task\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,CompressionLevel,System.Boolean,System.Text.Encoding,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,CompressionLevel,System.Boolean,System.Text.Encoding,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,CompressionLevel,System.Boolean,System.Text.Encoding,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":34,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":39,\"resultType\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"callee\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":45,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":46,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":47,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":49,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":55,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":56,\"resultType\":\"System.Threading.Tasks.Task\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,CompressionLevel,System.Boolean,System.Text.Encoding,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,CompressionLevel,System.Boolean,System.Text.Encoding,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::CreateFromDirectoryAsync:System.Threading.Tasks.Task(System.String,System.IO.Stream,CompressionLevel,System.Boolean,System.Text.Encoding,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":61,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":66,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":68,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":69,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":71,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":72,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":81,\"ilOffset\":73,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":75,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":76,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":95,\"ilOffset\":82,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":84,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":86,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":87,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":95,\"ilOffset\":93,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":95,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_12[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-12\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_12:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_12()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_12:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_12:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-12\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_12()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_12\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":20,\"handlerOffset\":21,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":7,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":12,\"resultType\":\"System.Void\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":32,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":21,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":24,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":32,\"ilOffset\":30,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":32,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_13[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-13\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_13:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_13()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_13:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_13:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-13\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_13()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_13\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_14[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-14\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_14:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_14()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_14:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_14:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-14\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_14()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_14\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_15[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-15\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_15:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_15()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_15:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_15:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-15\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_15()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_15\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":22,\"handlerOffset\":23,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":7,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":12,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":14,\"resultType\":\"System.Void\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String,System.Text.Encoding,System.Boolean)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String,System.Text.Encoding,System.Boolean)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.String,System.String,System.Text.Encoding,System.Boolean)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":34,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":23,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":26,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":34,\"ilOffset\":32,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":34,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_16[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-16\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_16:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_16()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_16:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_16:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-16\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_16()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_16\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":20,\"handlerOffset\":21,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":2,\"resultType\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"callee\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":7,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":12,\"resultType\":\"System.Void\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":32,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":21,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":24,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":32,\"ilOffset\":30,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":32,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_17[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-17\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_17:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_17()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_17:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_17:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-17\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_17()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_17\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_18[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-18\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_18:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_18()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_18:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_18:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-18\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_18()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_18\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_19[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-19\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_19:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_19()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_19:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_19:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-19\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_19()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_19\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":22,\"handlerOffset\":23,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":2,\"resultType\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"callee\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":7,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":12,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":14,\"resultType\":\"System.Void\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String,System.Text.Encoding,System.Boolean)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String,System.Text.Encoding,System.Boolean)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectory:System.Void(System.IO.Stream,System.String,System.Text.Encoding,System.Boolean)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":34,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":23,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":26,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":34,\"ilOffset\":32,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":34,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_20[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-20\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_20:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_20()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_20:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_20:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-20\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_20()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_20\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":77,\"handlerOffset\":78,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":7,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":12,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":14,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":20,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":21,\"resultType\":\"System.Threading.Tasks.Task\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":31,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":36,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":41,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":43,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":49,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":50,\"resultType\":\"System.Threading.Tasks.Task\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":55,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":60,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":62,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":63,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":65,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":66,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":75,\"ilOffset\":67,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":69,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":70,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":89,\"ilOffset\":76,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":78,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":80,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":81,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":89,\"ilOffset\":87,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":89,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_21[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-21\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_21:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_21()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_21:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_21:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-21\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_21()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_21\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_22[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-22\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_22:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_22()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_22:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_22:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-22\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_22()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_22\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_23[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-23\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_23:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_23()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_23:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_23:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-23\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_23()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_23\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":81,\"handlerOffset\":82,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":7,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":12,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":14,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":22,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":23,\"resultType\":\"System.Threading.Tasks.Task\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Text.Encoding,System.Boolean,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Text.Encoding,System.Boolean,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Text.Encoding,System.Boolean,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":33,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":38,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":43,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":45,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":47,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":53,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":54,\"resultType\":\"System.Threading.Tasks.Task\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Text.Encoding,System.Boolean,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Text.Encoding,System.Boolean,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.String,System.String,System.Text.Encoding,System.Boolean,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":59,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":64,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":66,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":67,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":69,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":70,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":79,\"ilOffset\":71,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":73,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":74,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":93,\"ilOffset\":80,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":82,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":84,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":85,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":93,\"ilOffset\":91,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":93,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_24[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-24\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_24:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_24()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_24:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_24:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-24\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_24()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_24\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":77,\"handlerOffset\":78,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":2,\"resultType\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"callee\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":7,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":12,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":14,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":20,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":21,\"resultType\":\"System.Threading.Tasks.Task\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":31,\"resultType\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"callee\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":36,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":41,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":43,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":49,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":50,\"resultType\":\"System.Threading.Tasks.Task\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":55,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":60,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":62,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":63,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":65,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":66,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":75,\"ilOffset\":67,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":69,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":70,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":89,\"ilOffset\":76,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":78,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":80,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":81,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":89,\"ilOffset\":87,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":89,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_25[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-25\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_25:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_25()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_25:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_25:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-25\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_25()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_25\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_26[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-26\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_26:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_26()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_26:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_26:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-26\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_26()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_26\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_27[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-27\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_27:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_27()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_27:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_27:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-27\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_27()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_27\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":81,\"handlerOffset\":82,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":2,\"resultType\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"callee\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":7,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":12,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":14,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":22,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":23,\"resultType\":\"System.Threading.Tasks.Task\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Text.Encoding,System.Boolean,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Text.Encoding,System.Boolean,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Text.Encoding,System.Boolean,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":33,\"resultType\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"callee\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":38,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":43,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":45,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":47,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":53,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":54,\"resultType\":\"System.Threading.Tasks.Task\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Text.Encoding,System.Boolean,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Text.Encoding,System.Boolean,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ExtractToDirectoryAsync:System.Threading.Tasks.Task(System.IO.Stream,System.String,System.Text.Encoding,System.Boolean,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":59,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":64,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":66,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":67,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":69,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":70,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":79,\"ilOffset\":71,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":73,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":74,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":93,\"ilOffset\":80,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":82,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":84,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":85,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":93,\"ilOffset\":91,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":93,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_28[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-28\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_28:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_28()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_28:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_28:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-28\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_28()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_28\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":51,\"handlerOffset\":52,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":8,\"resultType\":\"ZipArchive\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Open:ZipArchive(System.String,ZipArchiveMode)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Open:ZipArchive(System.String,ZipArchiveMode)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Open:ZipArchive(System.String,ZipArchiveMode)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":18,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":24,\"resultType\":\"ZipArchive\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Open:ZipArchive(System.String,ZipArchiveMode)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Open:ZipArchive(System.String,ZipArchiveMode)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Open:ZipArchive(System.String,ZipArchiveMode)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":34,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":36,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":37,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":40,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":49,\"ilOffset\":41,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":43,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":44,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":63,\"ilOffset\":50,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":52,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":54,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":55,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":63,\"ilOffset\":61,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":63,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_29[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-29\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_29:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_29()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_29:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_29:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-29\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_29()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_29\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":53,\"handlerOffset\":54,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":8,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":9,\"resultType\":\"ZipArchive\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Open:ZipArchive(System.String,ZipArchiveMode,System.Text.Encoding)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Open:ZipArchive(System.String,ZipArchiveMode,System.Text.Encoding)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Open:ZipArchive(System.String,ZipArchiveMode,System.Text.Encoding)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":19,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":25,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":26,\"resultType\":\"ZipArchive\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Open:ZipArchive(System.String,ZipArchiveMode,System.Text.Encoding)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Open:ZipArchive(System.String,ZipArchiveMode,System.Text.Encoding)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Open:ZipArchive(System.String,ZipArchiveMode,System.Text.Encoding)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":36,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":38,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":39,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":41,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":42,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":51,\"ilOffset\":43,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":45,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":46,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":65,\"ilOffset\":52,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":54,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":56,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":57,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":65,\"ilOffset\":63,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":65,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_30[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-30\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_30:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_30()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_30:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_30:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-30\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_30()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_30\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":69,\"handlerOffset\":70,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":8,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":10,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":16,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":17,\"resultType\":\"System.Threading.Tasks.Task\\u003CZipArchive\\u003E\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenAsync:System.Threading.Tasks.Task\\u003CZipArchive\\u003E(System.String,ZipArchiveMode,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenAsync:System.Threading.Tasks.Task\\u003CZipArchive\\u003E(System.String,ZipArchiveMode,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenAsync:System.Threading.Tasks.Task\\u003CZipArchive\\u003E(System.String,ZipArchiveMode,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":27,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":33,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":35,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":41,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":42,\"resultType\":\"System.Threading.Tasks.Task\\u003CZipArchive\\u003E\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenAsync:System.Threading.Tasks.Task\\u003CZipArchive\\u003E(System.String,ZipArchiveMode,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenAsync:System.Threading.Tasks.Task\\u003CZipArchive\\u003E(System.String,ZipArchiveMode,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenAsync:System.Threading.Tasks.Task\\u003CZipArchive\\u003E(System.String,ZipArchiveMode,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":47,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":52,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":54,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":55,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":57,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":58,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":67,\"ilOffset\":59,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":61,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":62,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":81,\"ilOffset\":68,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":70,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":72,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":73,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":81,\"ilOffset\":79,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":81,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_31[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-31\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_31:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_31()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_31:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_31:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-31\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_31()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_31\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":71,\"handlerOffset\":72,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":8,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":11,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":17,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":18,\"resultType\":\"System.Threading.Tasks.Task\\u003CZipArchive\\u003E\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenAsync:System.Threading.Tasks.Task\\u003CZipArchive\\u003E(System.String,ZipArchiveMode,System.Text.Encoding,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenAsync:System.Threading.Tasks.Task\\u003CZipArchive\\u003E(System.String,ZipArchiveMode,System.Text.Encoding,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenAsync:System.Threading.Tasks.Task\\u003CZipArchive\\u003E(System.String,ZipArchiveMode,System.Text.Encoding,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":28,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":34,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":35,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":37,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":43,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":44,\"resultType\":\"System.Threading.Tasks.Task\\u003CZipArchive\\u003E\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenAsync:System.Threading.Tasks.Task\\u003CZipArchive\\u003E(System.String,ZipArchiveMode,System.Text.Encoding,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenAsync:System.Threading.Tasks.Task\\u003CZipArchive\\u003E(System.String,ZipArchiveMode,System.Text.Encoding,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenAsync:System.Threading.Tasks.Task\\u003CZipArchive\\u003E(System.String,ZipArchiveMode,System.Text.Encoding,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":49,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":54,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":56,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":57,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":59,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":60,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":69,\"ilOffset\":61,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":63,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":64,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":83,\"ilOffset\":70,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":72,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":74,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":75,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":83,\"ilOffset\":81,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":83,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_32[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-32\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_32:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_32()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_32:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_32:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-32\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_32()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_32\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":49,\"handlerOffset\":50,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"ZipArchive\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenRead:ZipArchive(System.String)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenRead:ZipArchive(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenRead:ZipArchive(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":17,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":22,\"resultType\":\"ZipArchive\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenRead:ZipArchive(System.String)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenRead:ZipArchive(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenRead:ZipArchive(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":34,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":37,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":38,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":47,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":41,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":42,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":61,\"ilOffset\":48,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":50,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":52,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":53,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":61,\"ilOffset\":59,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":61,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";
static const char kMethodJson_33[] = "{\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-33\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_33:System.Void()\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_33()\",\"identity\":{\"assemblyName\":\"ZipFileApisSubjects\",\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"definitionSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_33:System.Void()\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::Subject_33:System.Void()\",\"methodId\":\"zip-file-apis-subjects.zip-file-apis-subjects.subject-33\",\"signature\":\"System.Void ZipFileApisSubjects::Subject_33()\"},\"nativeSymbol\":\"ZipFileApisSubjects_ZipFileApisSubjects_Subject_33\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":67,\"handlerOffset\":68,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":7,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":9,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":15,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":16,\"resultType\":\"System.Threading.Tasks.Task\\u003CZipArchive\\u003E\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenReadAsync:System.Threading.Tasks.Task\\u003CZipArchive\\u003E(System.String,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenReadAsync:System.Threading.Tasks.Task\\u003CZipArchive\\u003E(System.String,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenReadAsync:System.Threading.Tasks.Task\\u003CZipArchive\\u003E(System.String,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":26,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":31,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":39,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":40,\"resultType\":\"System.Threading.Tasks.Task\\u003CZipArchive\\u003E\",\"callee\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenReadAsync:System.Threading.Tasks.Task\\u003CZipArchive\\u003E(System.String,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectKind\":\"method\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenReadAsync:System.Threading.Tasks.Task\\u003CZipArchive\\u003E(System.String,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.IO.Compression.ZipFile\",\"subjectId\":\"System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::OpenReadAsync:System.Threading.Tasks.Task\\u003CZipArchive\\u003E(System.String,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":45,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":50,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":52,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":53,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":55,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":56,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":65,\"ilOffset\":57,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":59,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":60,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":79,\"ilOffset\":66,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":68,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":70,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"ilOffset\":71,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ZipFileApisSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ZipFileApisSubjects\",\"subjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ZipFileApisSubjects/ZipFileApisSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":79,\"ilOffset\":77,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":79,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ZipFileApisSubjects\"}";

extern "C" const JitMethodEntry kChaosJitMethodEntries[34] =
{
    { kMethodJson_0, 3421u, 0x00000003u, 0u },
    { kMethodJson_1, 3723u, 0x00000004u, 0u },
    { kMethodJson_2, 3878u, 0x00000005u, 0u },
    { kMethodJson_3, 3959u, 0x00000006u, 0u },
    { kMethodJson_4, 4261u, 0x00000007u, 0u },
    { kMethodJson_5, 4416u, 0x00000008u, 0u },
    { kMethodJson_6, 8941u, 0x00000009u, 0u },
    { kMethodJson_7, 9545u, 0x0000000Au, 0u },
    { kMethodJson_8, 9855u, 0x0000000Bu, 0u },
    { kMethodJson_9, 10017u, 0x0000000Cu, 0u },
    { kMethodJson_10, 10629u, 0x0000000Du, 0u },
    { kMethodJson_11, 10939u, 0x0000000Eu, 0u },
    { kMethodJson_12, 3426u, 0x0000000Fu, 0u },
    { kMethodJson_13, 1219u, 0x00000010u, 0u },
    { kMethodJson_14, 1219u, 0x00000011u, 0u },
    { kMethodJson_15, 3729u, 0x00000012u, 0u },
    { kMethodJson_16, 3964u, 0x00000013u, 0u },
    { kMethodJson_17, 1219u, 0x00000014u, 0u },
    { kMethodJson_18, 1219u, 0x00000015u, 0u },
    { kMethodJson_19, 4267u, 0x00000016u, 0u },
    { kMethodJson_20, 8943u, 0x00000017u, 0u },
    { kMethodJson_21, 1219u, 0x00000018u, 0u },
    { kMethodJson_22, 1219u, 0x00000019u, 0u },
    { kMethodJson_23, 9549u, 0x0000001Au, 0u },
    { kMethodJson_24, 10019u, 0x0000001Bu, 0u },
    { kMethodJson_25, 1219u, 0x0000001Cu, 0u },
    { kMethodJson_26, 1219u, 0x0000001Du, 0u },
    { kMethodJson_27, 10625u, 0x0000001Eu, 0u },
    { kMethodJson_28, 6920u, 0x0000001Fu, 0u },
    { kMethodJson_29, 7229u, 0x00000020u, 0u },
    { kMethodJson_30, 9028u, 0x00000021u, 0u },
    { kMethodJson_31, 9337u, 0x00000022u, 0u },
    { kMethodJson_32, 6649u, 0x00000023u, 0u },
    { kMethodJson_33, 8756u, 0x00000024u, 0u }
};

extern "C" const uint32_t kChaosJitMethodEntryCount = 34u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitMethods(kChaosJitMethodEntries, kChaosJitMethodEntryCount);
}