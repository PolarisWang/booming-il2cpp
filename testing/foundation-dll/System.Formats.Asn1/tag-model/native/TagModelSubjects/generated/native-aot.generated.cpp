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

namespace chaos::il2cpp::codegen::TagModelSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_System_Formats_Asn1_Asn1Tag = {{nullptr, nullptr, 1002182768936731605ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Formats_Asn1_Asn1Tag = static_cast<CHAOS_IL2CPP_INTPTR>(1002182768936731605ULL);
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
inline TypeInfoV0 chaos_mt_TagModelSubjects_TagModelSubjects = {{nullptr, nullptr, 10553850443217319370ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_TagModelSubjects_TagModelSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(10553850443217319370ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Int32 = {{nullptr, nullptr, 11009693519287992193ULL, 0u, 32, 2, 0}, {nullptr, nullptr, 0, 0, 0, 0}};
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

struct chaos_type_System_Formats_Asn1_Asn1Tag
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

struct chaos_type_TagModelSubjects_TagModelSubjects
{
	ThinLockableHeader header{};
};

struct chaos_boxed_type_System_Private_CoreLib_System_Int32
{
	PureTypeHeader header{};
	CHAOS_IL2CPP_INTPTR value = 0;
};

CHAOS_IL2CPP_INT32 chaos_static_TagModelSubjects_TagModelSubjects___exitCode = 0;

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

extern "C" void TagModelSubjects_TagModelSubjects_Subject_0(void);
extern "C" void TagModelSubjects_TagModelSubjects_Subject_1(void);
extern "C" void TagModelSubjects_TagModelSubjects_Subject_2(void);
extern "C" void TagModelSubjects_TagModelSubjects_Subject_3(void);
extern "C" void TagModelSubjects_TagModelSubjects_Subject_4(void);
extern "C" void TagModelSubjects_TagModelSubjects_Subject_5(void);
extern "C" void TagModelSubjects_TagModelSubjects_Subject_6(void);
extern "C" void TagModelSubjects_TagModelSubjects_Subject_7(void);
extern "C" void TagModelSubjects_TagModelSubjects_Subject_8(void);
extern "C" void TagModelSubjects_TagModelSubjects_Subject_9(void);
extern "C" void TagModelSubjects_TagModelSubjects_Subject_10(void);
extern "C" void TagModelSubjects_TagModelSubjects_Subject_11(void);
extern "C" void TagModelSubjects_TagModelSubjects_Subject_12(void);
extern "C" void TagModelSubjects_TagModelSubjects_Subject_13(void);
extern "C" void TagModelSubjects_TagModelSubjects_Subject_14(void);
extern "C" void TagModelSubjects_TagModelSubjects_Subject_15(void);
extern "C" void TagModelSubjects_TagModelSubjects_Subject_16(void);
extern "C" void TagModelSubjects_TagModelSubjects_Subject_17(void);
extern "C" void TagModelSubjects_TagModelSubjects_Subject_18(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[20] = {
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
	::ChaosAbiMethodEntryV0 entries[19];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		19u,
		0u,
		4125071901u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // TagModelSubjects_TagModelSubjects_Subject_0
		{ 0u, 0u },  // TagModelSubjects_TagModelSubjects_Subject_1
		{ 0u, 0u },  // TagModelSubjects_TagModelSubjects_Subject_2
		{ 0u, 0u },  // TagModelSubjects_TagModelSubjects_Subject_3
		{ 0u, 0u },  // TagModelSubjects_TagModelSubjects_Subject_4
		{ 0u, 0u },  // TagModelSubjects_TagModelSubjects_Subject_5
		{ 0u, 0u },  // TagModelSubjects_TagModelSubjects_Subject_6
		{ 0u, 0u },  // TagModelSubjects_TagModelSubjects_Subject_7
		{ 0u, 0u },  // TagModelSubjects_TagModelSubjects_Subject_8
		{ 0u, 0u },  // TagModelSubjects_TagModelSubjects_Subject_9
		{ 0u, 0u },  // TagModelSubjects_TagModelSubjects_Subject_10
		{ 0u, 0u },  // TagModelSubjects_TagModelSubjects_Subject_11
		{ 0u, 0u },  // TagModelSubjects_TagModelSubjects_Subject_12
		{ 0u, 0u },  // TagModelSubjects_TagModelSubjects_Subject_13
		{ 0u, 0u },  // TagModelSubjects_TagModelSubjects_Subject_14
		{ 0u, 0u },  // TagModelSubjects_TagModelSubjects_Subject_15
		{ 0u, 0u },  // TagModelSubjects_TagModelSubjects_Subject_16
		{ 0u, 0u },  // TagModelSubjects_TagModelSubjects_Subject_17
		{ 0u, 0u },  // TagModelSubjects_TagModelSubjects_Subject_18
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
		"TagModelSubjects",
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
		&chaos_mt_TagModelSubjects_TagModelSubjects.hot,
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
		/* .name_utf8         = */ "TagModelSubjects",
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
		::chaos::il2cpp::runtime_core::RegisterModule("TagModelSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[19] = {
	{ "Subject_0", 0x00000003u, 0u },  // TagModelSubjects
	{ "Subject_1", 0x00000004u, 0u },  // TagModelSubjects
	{ "Subject_2", 0x00000005u, 0u },  // TagModelSubjects
	{ "Subject_3", 0x00000006u, 0u },  // TagModelSubjects
	{ "Subject_4", 0x00000007u, 0u },  // TagModelSubjects
	{ "Subject_5", 0x00000008u, 0u },  // TagModelSubjects
	{ "Subject_6", 0x00000009u, 0u },  // TagModelSubjects
	{ "Subject_7", 0x0000000Au, 0u },  // TagModelSubjects
	{ "Subject_8", 0x0000000Bu, 0u },  // TagModelSubjects
	{ "Subject_9", 0x0000000Cu, 0u },  // TagModelSubjects
	{ "Subject_10", 0x0000000Du, 0u },  // TagModelSubjects
	{ "Subject_11", 0x0000000Eu, 0u },  // TagModelSubjects
	{ "Subject_12", 0x0000000Fu, 0u },  // TagModelSubjects
	{ "Subject_13", 0x00000010u, 0u },  // TagModelSubjects
	{ "Subject_14", 0x00000011u, 0u },  // TagModelSubjects
	{ "Subject_15", 0x00000012u, 0u },  // TagModelSubjects
	{ "Subject_16", 0x00000013u, 0u },  // TagModelSubjects
	{ "Subject_17", 0x00000014u, 0u },  // TagModelSubjects
	{ "Subject_18", 0x00000015u, 0u },  // TagModelSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "TagModelSubjects", "", 0u, 19u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[19] = {
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
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[19] = {
	{ reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // TagModelSubjects::Subject_0
	{ reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // TagModelSubjects::Subject_1
	{ reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // TagModelSubjects::Subject_2
	{ reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // TagModelSubjects::Subject_3
	{ reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // TagModelSubjects::Subject_4
	{ reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // TagModelSubjects::Subject_5
	{ reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // TagModelSubjects::Subject_6
	{ reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // TagModelSubjects::Subject_7
	{ reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // TagModelSubjects::Subject_8
	{ reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // TagModelSubjects::Subject_9
	{ reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // TagModelSubjects::Subject_10
	{ reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // TagModelSubjects::Subject_11
	{ reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // TagModelSubjects::Subject_12
	{ reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // TagModelSubjects::Subject_13
	{ reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_14), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // TagModelSubjects::Subject_14
	{ reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_15), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // TagModelSubjects::Subject_15
	{ reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_16), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // TagModelSubjects::Subject_16
	{ reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_17), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // TagModelSubjects::Subject_17
	{ reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_18), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // TagModelSubjects::Subject_18
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"TagModelSubjects",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	19u,
	s_hotpatch_slots,
	19u,
	s_hotpatch_entries,
	19u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[26] = {
	"System.Formats.Asn1/Asn1Tag",
	"System.Formats.Asn1/Asn1Tag::AsConstructed:Asn1Tag()",
	"System.Formats.Asn1/Asn1Tag::GetHashCode:System.Int32()",
	"TagModelSubjects/TagModelSubjects::_exitCode",
	"System.Formats.Asn1/Asn1Tag::AsPrimitive:Asn1Tag()",
	"System.Formats.Asn1/Asn1Tag::CalculateEncodedSize:System.Int32()",
	"System.Private.CoreLib/System.ReadOnlySpan<System.Byte>",
	"System.Formats.Asn1/Asn1Tag::Decode:Asn1Tag(System.ReadOnlySpan<System.Byte>,System.Int32&)",
	"System.Private.CoreLib/System.Span<System.Byte>",
	"System.Formats.Asn1/Asn1Tag::Encode:System.Int32(System.Span<System.Byte>)",
	"System.Formats.Asn1/Asn1Tag::Equals:System.Boolean(Asn1Tag)",
	"System.Private.CoreLib/System.Int32",
	"System.Formats.Asn1/Asn1Tag::Equals:System.Boolean(System.Object)",
	"System.Formats.Asn1/Asn1Tag::get_IsConstructed:System.Boolean()",
	"System.Formats.Asn1/Asn1Tag::get_TagClass:TagClass()",
	"System.Formats.Asn1/TagClass::GetHashCode:System.Int32()",
	"System.Formats.Asn1/Asn1Tag::get_TagValue:System.Int32()",
	"System.Formats.Asn1/Asn1Tag::HasSameClassAndValue:System.Boolean(Asn1Tag)",
	"System.Formats.Asn1/Asn1Tag::op_Equality:System.Boolean(Asn1Tag,Asn1Tag)",
	"System.Formats.Asn1/Asn1Tag::op_Inequality:System.Boolean(Asn1Tag,Asn1Tag)",
	"System.Formats.Asn1/Asn1Tag::ToString:System.String()",
	"System.Private.CoreLib/System.String::get_Length:System.Int32()",
	"System.Formats.Asn1/Asn1Tag::TryDecode:System.Boolean(System.ReadOnlySpan<System.Byte>,Asn1Tag&,System.Int32&)",
	"System.Formats.Asn1/Asn1Tag::TryEncode:System.Boolean(System.Span<System.Byte>,System.Int32&)",
	"System.Formats.Asn1/Asn1Tag::.ctor:System.Void(UniversalTagNumber,System.Boolean)",
	"System.Formats.Asn1/Asn1Tag::.ctor:System.Void(TagClass,System.Int32,System.Boolean)",
};

extern "C" void* kChaosExternalRuntimeFnTable[26] = {
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
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
};

extern "C" int32_t kChaosExternalRuntimeCount = 26;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[19])() = {
	reinterpret_cast<void(*)()>(&TagModelSubjects_TagModelSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&TagModelSubjects_TagModelSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&TagModelSubjects_TagModelSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&TagModelSubjects_TagModelSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&TagModelSubjects_TagModelSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&TagModelSubjects_TagModelSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&TagModelSubjects_TagModelSubjects_Subject_6),
	reinterpret_cast<void(*)()>(&TagModelSubjects_TagModelSubjects_Subject_7),
	reinterpret_cast<void(*)()>(&TagModelSubjects_TagModelSubjects_Subject_8),
	reinterpret_cast<void(*)()>(&TagModelSubjects_TagModelSubjects_Subject_9),
	reinterpret_cast<void(*)()>(&TagModelSubjects_TagModelSubjects_Subject_10),
	reinterpret_cast<void(*)()>(&TagModelSubjects_TagModelSubjects_Subject_11),
	reinterpret_cast<void(*)()>(&TagModelSubjects_TagModelSubjects_Subject_12),
	reinterpret_cast<void(*)()>(&TagModelSubjects_TagModelSubjects_Subject_13),
	reinterpret_cast<void(*)()>(&TagModelSubjects_TagModelSubjects_Subject_14),
	reinterpret_cast<void(*)()>(&TagModelSubjects_TagModelSubjects_Subject_15),
	reinterpret_cast<void(*)()>(&TagModelSubjects_TagModelSubjects_Subject_16),
	reinterpret_cast<void(*)()>(&TagModelSubjects_TagModelSubjects_Subject_17),
	reinterpret_cast<void(*)()>(&TagModelSubjects_TagModelSubjects_Subject_18),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[19])() = {
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
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 19;
extern "C" const int kSubjectEntryIndices[19] = {
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

	/* ── Entry 0: TagModelSubjects_TagModelSubjects_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry0;
	/* ── Entry 1: TagModelSubjects_TagModelSubjects_Subject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry1;
	/* ── Entry 2: TagModelSubjects_TagModelSubjects_Subject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry2;
	/* ── Entry 3: TagModelSubjects_TagModelSubjects_Subject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[9];
	} entry3;
	/* ── Entry 4: TagModelSubjects_TagModelSubjects_Subject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry4;
	/* ── Entry 5: TagModelSubjects_TagModelSubjects_Subject_5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry5;
	/* ── Entry 6: TagModelSubjects_TagModelSubjects_Subject_6 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry6;
	/* ── Entry 7: TagModelSubjects_TagModelSubjects_Subject_7 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry7;
	/* ── Entry 8: TagModelSubjects_TagModelSubjects_Subject_8 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[9];
	} entry8;
	/* ── Entry 9: TagModelSubjects_TagModelSubjects_Subject_9 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry9;
	/* ── Entry 10: TagModelSubjects_TagModelSubjects_Subject_10 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry10;
	/* ── Entry 11: TagModelSubjects_TagModelSubjects_Subject_11 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry11;
	/* ── Entry 12: TagModelSubjects_TagModelSubjects_Subject_12 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry12;
	/* ── Entry 13: TagModelSubjects_TagModelSubjects_Subject_13 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry13;
	/* ── Entry 14: TagModelSubjects_TagModelSubjects_Subject_14 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry14;
	/* ── Entry 15: TagModelSubjects_TagModelSubjects_Subject_15 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry15;
	/* ── Entry 16: TagModelSubjects_TagModelSubjects_Subject_16 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry16;
	/* ── Entry 17: TagModelSubjects_TagModelSubjects_Subject_17 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry17;
	/* ── Entry 18: TagModelSubjects_TagModelSubjects_Subject_18 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry18;
} kChaosGcSlotMapsSection = {
	/* entry0 = TagModelSubjects_TagModelSubjects_Subject_0 */
	.entry0 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&TagModelSubjects_TagModelSubjects_Subject_0),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry1 = TagModelSubjects_TagModelSubjects_Subject_1 */
	.entry1 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&TagModelSubjects_TagModelSubjects_Subject_1),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry2 = TagModelSubjects_TagModelSubjects_Subject_2 */
	.entry2 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&TagModelSubjects_TagModelSubjects_Subject_2),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry3 = TagModelSubjects_TagModelSubjects_Subject_3 */
	.entry3 = {
		/* entry_total_size = 56 */ 56u,
		/* code_address */ reinterpret_cast<const void*>(&TagModelSubjects_TagModelSubjects_Subject_3),
		/* frame_size = 72 */ 72u,
		/* num_gc_slots = 9 */ 9u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u }
	},
	/* entry4 = TagModelSubjects_TagModelSubjects_Subject_4 */
	.entry4 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&TagModelSubjects_TagModelSubjects_Subject_4),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry5 = TagModelSubjects_TagModelSubjects_Subject_5 */
	.entry5 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&TagModelSubjects_TagModelSubjects_Subject_5),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry6 = TagModelSubjects_TagModelSubjects_Subject_6 */
	.entry6 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&TagModelSubjects_TagModelSubjects_Subject_6),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry7 = TagModelSubjects_TagModelSubjects_Subject_7 */
	.entry7 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&TagModelSubjects_TagModelSubjects_Subject_7),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry8 = TagModelSubjects_TagModelSubjects_Subject_8 */
	.entry8 = {
		/* entry_total_size = 56 */ 56u,
		/* code_address */ reinterpret_cast<const void*>(&TagModelSubjects_TagModelSubjects_Subject_8),
		/* frame_size = 72 */ 72u,
		/* num_gc_slots = 9 */ 9u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u }
	},
	/* entry9 = TagModelSubjects_TagModelSubjects_Subject_9 */
	.entry9 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&TagModelSubjects_TagModelSubjects_Subject_9),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry10 = TagModelSubjects_TagModelSubjects_Subject_10 */
	.entry10 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&TagModelSubjects_TagModelSubjects_Subject_10),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry11 = TagModelSubjects_TagModelSubjects_Subject_11 */
	.entry11 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&TagModelSubjects_TagModelSubjects_Subject_11),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry12 = TagModelSubjects_TagModelSubjects_Subject_12 */
	.entry12 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&TagModelSubjects_TagModelSubjects_Subject_12),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry13 = TagModelSubjects_TagModelSubjects_Subject_13 */
	.entry13 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&TagModelSubjects_TagModelSubjects_Subject_13),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry14 = TagModelSubjects_TagModelSubjects_Subject_14 */
	.entry14 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&TagModelSubjects_TagModelSubjects_Subject_14),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry15 = TagModelSubjects_TagModelSubjects_Subject_15 */
	.entry15 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&TagModelSubjects_TagModelSubjects_Subject_15),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry16 = TagModelSubjects_TagModelSubjects_Subject_16 */
	.entry16 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&TagModelSubjects_TagModelSubjects_Subject_16),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry17 = TagModelSubjects_TagModelSubjects_Subject_17 */
	.entry17 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&TagModelSubjects_TagModelSubjects_Subject_17),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry18 = TagModelSubjects_TagModelSubjects_Subject_18 */
	.entry18 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&TagModelSubjects_TagModelSubjects_Subject_18),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 856u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[19] = {
	reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_0),
	reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_1),
	reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_2),
	reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_3),
	reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_4),
	reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_5),
	reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_6),
	reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_7),
	reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_8),
	reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_9),
	reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_10),
	reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_11),
	reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_12),
	reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_13),
	reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_14),
	reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_15),
	reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_16),
	reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_17),
	reinterpret_cast<void*>(&TagModelSubjects_TagModelSubjects_Subject_18),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 19u,
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
	.image_name_utf8    = "TagModelSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_TagModelSubjects_TagModelSubjects[1] = {
	{ 0u, "TagModelSubjects/TagModelSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_TagModelSubjects_TagModelSubjects[19] = {
	{ 0u, "TagModelSubjects/TagModelSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "TagModelSubjects/TagModelSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "TagModelSubjects/TagModelSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "TagModelSubjects/TagModelSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "TagModelSubjects/TagModelSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "TagModelSubjects/TagModelSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "TagModelSubjects/TagModelSubjects::Subject_6:System.Void()", "Subject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "TagModelSubjects/TagModelSubjects::Subject_7:System.Void()", "Subject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "TagModelSubjects/TagModelSubjects::Subject_8:System.Void()", "Subject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "TagModelSubjects/TagModelSubjects::Subject_9:System.Void()", "Subject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, "TagModelSubjects/TagModelSubjects::Subject_10:System.Void()", "Subject_10", "System.Void", 0, nullptr, 0u },
	{ 0u, "TagModelSubjects/TagModelSubjects::Subject_11:System.Void()", "Subject_11", "System.Void", 0, nullptr, 0u },
	{ 0u, "TagModelSubjects/TagModelSubjects::Subject_12:System.Void()", "Subject_12", "System.Void", 0, nullptr, 0u },
	{ 0u, "TagModelSubjects/TagModelSubjects::Subject_13:System.Void()", "Subject_13", "System.Void", 0, nullptr, 0u },
	{ 0u, "TagModelSubjects/TagModelSubjects::Subject_14:System.Void()", "Subject_14", "System.Void", 0, nullptr, 0u },
	{ 0u, "TagModelSubjects/TagModelSubjects::Subject_15:System.Void()", "Subject_15", "System.Void", 0, nullptr, 0u },
	{ 0u, "TagModelSubjects/TagModelSubjects::Subject_16:System.Void()", "Subject_16", "System.Void", 0, nullptr, 0u },
	{ 0u, "TagModelSubjects/TagModelSubjects::Subject_17:System.Void()", "Subject_17", "System.Void", 0, nullptr, 0u },
	{ 0u, "TagModelSubjects/TagModelSubjects::Subject_18:System.Void()", "Subject_18", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0u, "TagModelSubjects/TagModelSubjects", "TagModelSubjects/TagModelSubjects", "", "TagModelSubjects", "TagModelSubjects", nullptr, kReflFields_TagModelSubjects_TagModelSubjects, 1u, nullptr, 0u,
	kReflMethods_TagModelSubjects_TagModelSubjects, 19u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "TagModelSubjects", kReflTypePtrs, 1u };

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
// Managed method: TagModelSubjects/TagModelSubjects::Subject_0()
extern "C" void TagModelSubjects_TagModelSubjects_Subject_0(void)
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[1])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s1;
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[2])();
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			_s4 = _s3;
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
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[1])();
				_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s4;
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[2])();
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TagModelSubjects/TagModelSubjects::Subject_1()
extern "C" void TagModelSubjects_TagModelSubjects_Subject_1(void)
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[4])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s1;
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[2])();
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			_s4 = _s3;
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
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[4])();
				_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s4;
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[2])();
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TagModelSubjects/TagModelSubjects::Subject_2()
extern "C" void TagModelSubjects_TagModelSubjects_Subject_2(void)
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[5])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			_s3 = _s2;
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
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[5])();
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TagModelSubjects/TagModelSubjects::Subject_3()
extern "C" void TagModelSubjects_TagModelSubjects_Subject_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_locals{};
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
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[2]);
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
			_s0 = chaos_locals[2];
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[7])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[3] = _s0;
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[3]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[2])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[2]);
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
			_s2 = chaos_locals[2];
			_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[4]);
			{
				const auto chaos_arg_1 = _s3;
				const auto chaos_arg_0 = _s2;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[7])(chaos_arg_0, chaos_arg_1);
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[3] = _s2;
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[3]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[2])();
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TagModelSubjects/TagModelSubjects::Subject_4()
extern "C" void TagModelSubjects_TagModelSubjects_Subject_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
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
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[2]);
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
			_s1 = chaos_locals[2];
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[9])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			_s3 = _s2;
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
			_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[2]);
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
			_s3 = chaos_locals[2];
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[9])(chaos_arg_0);
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TagModelSubjects/TagModelSubjects::Subject_5()
extern "C" void TagModelSubjects_TagModelSubjects_Subject_5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
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
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[2]);
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
			_s1 = chaos_locals[2];
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[10])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			_s5 = _s4;
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
			_s5 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[2]);
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
			_s5 = chaos_locals[2];
			{
				const auto chaos_arg_0 = _s5;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[10])(chaos_arg_0);
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TagModelSubjects/TagModelSubjects::Subject_6()
extern "C" void TagModelSubjects_TagModelSubjects_Subject_6(void)
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
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_value = _s1;
				auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_System_Private_CoreLib_System_Int32, {});
				chaos_boxed->header.type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
				chaos_boxed->value = chaos_value;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[12])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			_s5 = _s4;
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
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_value = _s5;
				auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_System_Private_CoreLib_System_Int32, {});
				chaos_boxed->header.type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
				chaos_boxed->value = chaos_value;
				_s5 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
			}
			{
				const auto chaos_arg_0 = _s5;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[12])(chaos_arg_0);
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TagModelSubjects/TagModelSubjects::Subject_7()
extern "C" void TagModelSubjects_TagModelSubjects_Subject_7(void)
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[13])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			_s5 = _s4;
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
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[13])();
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TagModelSubjects/TagModelSubjects::Subject_8()
extern "C" void TagModelSubjects_TagModelSubjects_Subject_8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[14])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[2] = _s1;
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[2]);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[15])();
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			_s4 = _s3;
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
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[14])();
				_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[2] = _s4;
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[2]);
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TagModelSubjects/TagModelSubjects::Subject_9()
extern "C" void TagModelSubjects_TagModelSubjects_Subject_9(void)
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[16])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			_s3 = _s2;
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
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[16])();
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TagModelSubjects/TagModelSubjects::Subject_10()
extern "C" void TagModelSubjects_TagModelSubjects_Subject_10(void)
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[2])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			_s3 = _s2;
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
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[2])();
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TagModelSubjects/TagModelSubjects::Subject_11()
extern "C" void TagModelSubjects_TagModelSubjects_Subject_11(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
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
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[2]);
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
			_s1 = chaos_locals[2];
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[17])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			_s5 = _s4;
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
			_s5 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[2]);
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
			_s5 = chaos_locals[2];
			{
				const auto chaos_arg_0 = _s5;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[17])(chaos_arg_0);
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TagModelSubjects/TagModelSubjects::Subject_12()
extern "C" void TagModelSubjects_TagModelSubjects_Subject_12(void)
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
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
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
			_s0 = chaos_locals[1];
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[18])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
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
				const auto chaos_arg_1 = _s4;
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[18])(chaos_arg_0, chaos_arg_1);
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TagModelSubjects/TagModelSubjects::Subject_13()
extern "C" void TagModelSubjects_TagModelSubjects_Subject_13(void)
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
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
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
			_s0 = chaos_locals[1];
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[19])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
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
				const auto chaos_arg_1 = _s4;
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[19])(chaos_arg_0, chaos_arg_1);
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TagModelSubjects/TagModelSubjects::Subject_14()
extern "C" void TagModelSubjects_TagModelSubjects_Subject_14(void)
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[20])();
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
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			_s3 = _s2;
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
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[20])();
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TagModelSubjects/TagModelSubjects::Subject_15()
extern "C" void TagModelSubjects_TagModelSubjects_Subject_15(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 4) chaos_locals{};
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
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[3]);
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
			_s0 = chaos_locals[3];
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[2]);
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[22])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[3]);
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
			_s3 = chaos_locals[3];
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			_s5 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[2]);
			{
				const auto chaos_arg_2 = _s5;
				const auto chaos_arg_1 = _s4;
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[22])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TagModelSubjects/TagModelSubjects::Subject_16()
extern "C" void TagModelSubjects_TagModelSubjects_Subject_16(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 4) chaos_locals{};
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
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[2]);
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
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[3]);
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
			_s1 = chaos_locals[3];
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[23])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[2]);
			_s5 = _s4;
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
			_s5 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[3]);
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
			_s5 = chaos_locals[3];
			_s6 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_arg_1 = _s6;
				const auto chaos_arg_0 = _s5;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[23])(chaos_arg_0, chaos_arg_1);
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TagModelSubjects/TagModelSubjects::Subject_17()
extern "C" void TagModelSubjects_TagModelSubjects_Subject_17(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Formats_Asn1_Asn1Tag, {});
				chaos_object->header.type_info = &chaos_mt_System_Formats_Asn1_Asn1Tag.hot;
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: TagModelSubjects/TagModelSubjects::Subject_18()
extern "C" void TagModelSubjects_TagModelSubjects_Subject_18(void)
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
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Formats_Asn1_Asn1Tag, {});
				chaos_object->header.type_info = &chaos_mt_System_Formats_Asn1_Asn1Tag.hot;
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
				chaos_static_TagModelSubjects_TagModelSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}



}  // namespace chaos::il2cpp::codegen::TagModelSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 19;

// ── JIT Method Entry Table ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kMethodJson_0[] = "{\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-0\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_0:System.Void()\",\"signature\":\"System.Void TagModelSubjects::Subject_0()\",\"identity\":{\"assemblyName\":\"TagModelSubjects\",\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"definitionSubjectId\":\"TagModelSubjects/TagModelSubjects::Subject_0:System.Void()\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_0:System.Void()\",\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-0\",\"signature\":\"System.Void TagModelSubjects::Subject_0()\"},\"nativeSymbol\":\"TagModelSubjects_TagModelSubjects_Subject_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":75,\"handlerOffset\":76,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":4,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":5,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"Asn1Tag\",\"callee\":\"System.Formats.Asn1/Asn1Tag::AsConstructed:Asn1Tag()\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::AsConstructed:Asn1Tag()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::AsConstructed:Asn1Tag()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":17,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"callee\":\"System.Formats.Asn1/Asn1Tag::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Formats.Asn1/Asn1Tag\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":30,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":39,\"resultType\":\"Asn1Tag\",\"callee\":\"System.Formats.Asn1/Asn1Tag::AsConstructed:Asn1Tag()\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::AsConstructed:Asn1Tag()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::AsConstructed:Asn1Tag()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":45,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":53,\"resultType\":\"System.Int32\",\"callee\":\"System.Formats.Asn1/Asn1Tag::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Formats.Asn1/Asn1Tag\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":58,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":60,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":61,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":63,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":64,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":73,\"ilOffset\":65,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":67,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":68,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":87,\"ilOffset\":74,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":76,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":78,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":79,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":87,\"ilOffset\":85,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":87,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"TagModelSubjects\"}";
static const char kMethodJson_1[] = "{\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-1\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_1:System.Void()\",\"signature\":\"System.Void TagModelSubjects::Subject_1()\",\"identity\":{\"assemblyName\":\"TagModelSubjects\",\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"definitionSubjectId\":\"TagModelSubjects/TagModelSubjects::Subject_1:System.Void()\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_1:System.Void()\",\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-1\",\"signature\":\"System.Void TagModelSubjects::Subject_1()\"},\"nativeSymbol\":\"TagModelSubjects_TagModelSubjects_Subject_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":75,\"handlerOffset\":76,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":4,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":5,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"Asn1Tag\",\"callee\":\"System.Formats.Asn1/Asn1Tag::AsPrimitive:Asn1Tag()\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::AsPrimitive:Asn1Tag()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::AsPrimitive:Asn1Tag()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":17,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"callee\":\"System.Formats.Asn1/Asn1Tag::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Formats.Asn1/Asn1Tag\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":30,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":39,\"resultType\":\"Asn1Tag\",\"callee\":\"System.Formats.Asn1/Asn1Tag::AsPrimitive:Asn1Tag()\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::AsPrimitive:Asn1Tag()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::AsPrimitive:Asn1Tag()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":45,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":53,\"resultType\":\"System.Int32\",\"callee\":\"System.Formats.Asn1/Asn1Tag::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Formats.Asn1/Asn1Tag\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":58,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":60,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":61,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":63,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":64,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":73,\"ilOffset\":65,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":67,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":68,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":87,\"ilOffset\":74,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":76,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":78,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":79,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":87,\"ilOffset\":85,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":87,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"TagModelSubjects\"}";
static const char kMethodJson_2[] = "{\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-2\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_2:System.Void()\",\"signature\":\"System.Void TagModelSubjects::Subject_2()\",\"identity\":{\"assemblyName\":\"TagModelSubjects\",\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"definitionSubjectId\":\"TagModelSubjects/TagModelSubjects::Subject_2:System.Void()\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_2:System.Void()\",\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-2\",\"signature\":\"System.Void TagModelSubjects::Subject_2()\"},\"nativeSymbol\":\"TagModelSubjects_TagModelSubjects_Subject_2\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":47,\"handlerOffset\":48,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":4,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":5,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"callee\":\"System.Formats.Asn1/Asn1Tag::CalculateEncodedSize:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::CalculateEncodedSize:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::CalculateEncodedSize:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":16,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":19,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"callee\":\"System.Formats.Asn1/Asn1Tag::CalculateEncodedSize:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::CalculateEncodedSize:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::CalculateEncodedSize:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":30,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":35,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":36,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":45,\"ilOffset\":37,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":39,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":40,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":59,\"ilOffset\":46,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":48,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":50,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":51,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":59,\"ilOffset\":57,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":59,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"TagModelSubjects\"}";
static const char kMethodJson_3[] = "{\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-3\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_3:System.Void()\",\"signature\":\"System.Void TagModelSubjects::Subject_3()\",\"identity\":{\"assemblyName\":\"TagModelSubjects\",\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"definitionSubjectId\":\"TagModelSubjects/TagModelSubjects::Subject_3:System.Void()\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_3:System.Void()\",\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-3\",\"signature\":\"System.Void TagModelSubjects::Subject_3()\"},\"nativeSymbol\":\"TagModelSubjects_TagModelSubjects_Subject_3\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":5,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":79,\"handlerOffset\":80,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldloca\",\"opCode\":76,\"operand\":2,\"ilOffset\":2,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E\",\"ilOffset\":4,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":10,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":11,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"Asn1Tag\",\"callee\":\"System.Formats.Asn1/Asn1Tag::Decode:Asn1Tag(System.ReadOnlySpan\\u003CSystem.Byte\\u003E,System.Int32\\u0026)\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::Decode:Asn1Tag(System.ReadOnlySpan\\u003CSystem.Byte\\u003E,System.Int32\\u0026)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::Decode:Asn1Tag(System.ReadOnlySpan\\u003CSystem.Byte\\u003E,System.Int32\\u0026)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":3,\"ilOffset\":19,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"callee\":\"System.Formats.Asn1/Asn1Tag::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Formats.Asn1/Asn1Tag\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":2,\"ilOffset\":32,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E\",\"ilOffset\":34,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":40,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":4,\"ilOffset\":41,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":43,\"resultType\":\"Asn1Tag\",\"callee\":\"System.Formats.Asn1/Asn1Tag::Decode:Asn1Tag(System.ReadOnlySpan\\u003CSystem.Byte\\u003E,System.Int32\\u0026)\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::Decode:Asn1Tag(System.ReadOnlySpan\\u003CSystem.Byte\\u003E,System.Int32\\u0026)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::Decode:Asn1Tag(System.ReadOnlySpan\\u003CSystem.Byte\\u003E,System.Int32\\u0026)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":48,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":3,\"ilOffset\":49,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":57,\"resultType\":\"System.Int32\",\"callee\":\"System.Formats.Asn1/Asn1Tag::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Formats.Asn1/Asn1Tag\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":62,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":64,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":65,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":67,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":68,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":77,\"ilOffset\":69,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":71,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":72,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":91,\"ilOffset\":78,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":80,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":82,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":83,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":91,\"ilOffset\":89,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":91,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"TagModelSubjects\"}";
static const char kMethodJson_4[] = "{\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-4\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_4:System.Void()\",\"signature\":\"System.Void TagModelSubjects::Subject_4()\",\"identity\":{\"assemblyName\":\"TagModelSubjects\",\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"definitionSubjectId\":\"TagModelSubjects/TagModelSubjects::Subject_4:System.Void()\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_4:System.Void()\",\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-4\",\"signature\":\"System.Void TagModelSubjects::Subject_4()\"},\"nativeSymbol\":\"TagModelSubjects_TagModelSubjects_Subject_4\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":3,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":65,\"handlerOffset\":66,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":4,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":5,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":2,\"ilOffset\":11,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\",\"ilOffset\":13,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"callee\":\"System.Formats.Asn1/Asn1Tag::Encode:System.Int32(System.Span\\u003CSystem.Byte\\u003E)\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::Encode:System.Int32(System.Span\\u003CSystem.Byte\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::Encode:System.Int32(System.Span\\u003CSystem.Byte\\u003E)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":27,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":28,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":2,\"ilOffset\":34,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\",\"ilOffset\":36,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":42,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":43,\"resultType\":\"System.Int32\",\"callee\":\"System.Formats.Asn1/Asn1Tag::Encode:System.Int32(System.Span\\u003CSystem.Byte\\u003E)\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::Encode:System.Int32(System.Span\\u003CSystem.Byte\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::Encode:System.Int32(System.Span\\u003CSystem.Byte\\u003E)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":48,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":50,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":51,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":53,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":54,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":63,\"ilOffset\":55,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":57,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":58,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":77,\"ilOffset\":64,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":66,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":68,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":69,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":77,\"ilOffset\":75,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":77,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"TagModelSubjects\"}";
static const char kMethodJson_5[] = "{\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-5\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_5:System.Void()\",\"signature\":\"System.Void TagModelSubjects::Subject_5()\",\"identity\":{\"assemblyName\":\"TagModelSubjects\",\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"definitionSubjectId\":\"TagModelSubjects/TagModelSubjects::Subject_5:System.Void()\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_5:System.Void()\",\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-5\",\"signature\":\"System.Void TagModelSubjects::Subject_5()\"},\"nativeSymbol\":\"TagModelSubjects_TagModelSubjects_Subject_5\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":3,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":77,\"handlerOffset\":78,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":4,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":5,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":2,\"ilOffset\":11,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":13,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":20,\"resultType\":\"System.Boolean\",\"callee\":\"System.Formats.Asn1/Asn1Tag::Equals:System.Boolean(Asn1Tag)\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::Equals:System.Boolean(Asn1Tag)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::Equals:System.Boolean(Asn1Tag)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":30,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":31,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":30,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":31,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":33,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":34,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":2,\"ilOffset\":40,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":42,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":48,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":49,\"resultType\":\"System.Boolean\",\"callee\":\"System.Formats.Asn1/Asn1Tag::Equals:System.Boolean(Asn1Tag)\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::Equals:System.Boolean(Asn1Tag)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::Equals:System.Boolean(Asn1Tag)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":59,\"ilOffset\":54,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":56,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":60,\"ilOffset\":57,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":59,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":60,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":62,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":63,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":65,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":66,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":75,\"ilOffset\":67,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":69,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":70,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":89,\"ilOffset\":76,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":78,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":80,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":81,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":89,\"ilOffset\":87,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":89,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"TagModelSubjects\"}";
static const char kMethodJson_6[] = "{\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-6\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_6:System.Void()\",\"signature\":\"System.Void TagModelSubjects::Subject_6()\",\"identity\":{\"assemblyName\":\"TagModelSubjects\",\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"definitionSubjectId\":\"TagModelSubjects/TagModelSubjects::Subject_6:System.Void()\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_6:System.Void()\",\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-6\",\"signature\":\"System.Void TagModelSubjects::Subject_6()\"},\"nativeSymbol\":\"TagModelSubjects_TagModelSubjects_Subject_6\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":85,\"handlerOffset\":86,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":4,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":5,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"box\",\"opCode\":35,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":13,\"resultType\":\"System.Object\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":9,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":24,\"resultType\":\"System.Boolean\",\"callee\":\"System.Formats.Asn1/Asn1Tag::Equals:System.Boolean(System.Object)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Formats.Asn1/Asn1Tag\",\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":34,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":35,\"ilOffset\":32,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":34,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":35,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":37,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":38,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"box\",\"opCode\":35,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":46,\"resultType\":\"System.Object\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":9,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":57,\"resultType\":\"System.Boolean\",\"callee\":\"System.Formats.Asn1/Asn1Tag::Equals:System.Boolean(System.Object)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Formats.Asn1/Asn1Tag\",\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":67,\"ilOffset\":62,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":64,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":68,\"ilOffset\":65,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":67,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":68,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":70,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":71,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":73,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":74,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":83,\"ilOffset\":75,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":77,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":78,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":97,\"ilOffset\":84,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":86,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":88,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":89,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":97,\"ilOffset\":95,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":97,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"TagModelSubjects\"}";
static const char kMethodJson_7[] = "{\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-7\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_7:System.Void()\",\"signature\":\"System.Void TagModelSubjects::Subject_7()\",\"identity\":{\"assemblyName\":\"TagModelSubjects\",\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"definitionSubjectId\":\"TagModelSubjects/TagModelSubjects::Subject_7:System.Void()\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_7:System.Void()\",\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-7\",\"signature\":\"System.Void TagModelSubjects::Subject_7()\"},\"nativeSymbol\":\"TagModelSubjects_TagModelSubjects_Subject_7\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":59,\"handlerOffset\":60,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":4,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":5,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Boolean\",\"callee\":\"System.Formats.Asn1/Asn1Tag::get_IsConstructed:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::get_IsConstructed:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::get_IsConstructed:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":21,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":22,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":22,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":24,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":25,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":31,\"resultType\":\"System.Boolean\",\"callee\":\"System.Formats.Asn1/Asn1Tag::get_IsConstructed:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::get_IsConstructed:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::get_IsConstructed:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":41,\"ilOffset\":36,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":38,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":42,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":41,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":42,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":45,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":47,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":48,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":57,\"ilOffset\":49,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":51,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":52,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":71,\"ilOffset\":58,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":60,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":62,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":63,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":71,\"ilOffset\":69,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":71,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"TagModelSubjects\"}";
static const char kMethodJson_8[] = "{\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-8\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_8:System.Void()\",\"signature\":\"System.Void TagModelSubjects::Subject_8()\",\"identity\":{\"assemblyName\":\"TagModelSubjects\",\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"definitionSubjectId\":\"TagModelSubjects/TagModelSubjects::Subject_8:System.Void()\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_8:System.Void()\",\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-8\",\"signature\":\"System.Void TagModelSubjects::Subject_8()\"},\"nativeSymbol\":\"TagModelSubjects_TagModelSubjects_Subject_8\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":3,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":75,\"handlerOffset\":76,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":4,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":5,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"TagClass\",\"callee\":\"System.Formats.Asn1/Asn1Tag::get_TagClass:TagClass()\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::get_TagClass:TagClass()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::get_TagClass:TagClass()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":2,\"ilOffset\":17,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"callee\":\"System.Formats.Asn1/TagClass::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Formats.Asn1/TagClass\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":30,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":39,\"resultType\":\"TagClass\",\"callee\":\"System.Formats.Asn1/Asn1Tag::get_TagClass:TagClass()\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::get_TagClass:TagClass()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::get_TagClass:TagClass()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":44,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":2,\"ilOffset\":45,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":53,\"resultType\":\"System.Int32\",\"callee\":\"System.Formats.Asn1/TagClass::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Formats.Asn1/TagClass\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":58,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":60,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":61,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":63,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":64,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":73,\"ilOffset\":65,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":67,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":68,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":87,\"ilOffset\":74,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":76,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":78,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":79,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":87,\"ilOffset\":85,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":87,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"TagModelSubjects\"}";
static const char kMethodJson_9[] = "{\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-9\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_9:System.Void()\",\"signature\":\"System.Void TagModelSubjects::Subject_9()\",\"identity\":{\"assemblyName\":\"TagModelSubjects\",\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"definitionSubjectId\":\"TagModelSubjects/TagModelSubjects::Subject_9:System.Void()\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_9:System.Void()\",\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-9\",\"signature\":\"System.Void TagModelSubjects::Subject_9()\"},\"nativeSymbol\":\"TagModelSubjects_TagModelSubjects_Subject_9\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":47,\"handlerOffset\":48,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":4,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":5,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"callee\":\"System.Formats.Asn1/Asn1Tag::get_TagValue:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::get_TagValue:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::get_TagValue:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":16,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":19,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"callee\":\"System.Formats.Asn1/Asn1Tag::get_TagValue:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::get_TagValue:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::get_TagValue:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":30,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":35,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":36,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":45,\"ilOffset\":37,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":39,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":40,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":59,\"ilOffset\":46,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":48,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":50,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":51,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":59,\"ilOffset\":57,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":59,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"TagModelSubjects\"}";
static const char kMethodJson_10[] = "{\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-10\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_10:System.Void()\",\"signature\":\"System.Void TagModelSubjects::Subject_10()\",\"identity\":{\"assemblyName\":\"TagModelSubjects\",\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"definitionSubjectId\":\"TagModelSubjects/TagModelSubjects::Subject_10:System.Void()\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_10:System.Void()\",\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-10\",\"signature\":\"System.Void TagModelSubjects::Subject_10()\"},\"nativeSymbol\":\"TagModelSubjects_TagModelSubjects_Subject_10\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":59,\"handlerOffset\":60,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":4,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":5,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"callee\":\"System.Formats.Asn1/Asn1Tag::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Formats.Asn1/Asn1Tag\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":22,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":24,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":25,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":37,\"resultType\":\"System.Int32\",\"callee\":\"System.Formats.Asn1/Asn1Tag::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Formats.Asn1/Asn1Tag\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":42,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":45,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":47,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":48,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":57,\"ilOffset\":49,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":51,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":52,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":71,\"ilOffset\":58,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":60,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":62,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":63,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":71,\"ilOffset\":69,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":71,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"TagModelSubjects\"}";
static const char kMethodJson_11[] = "{\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-11\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_11:System.Void()\",\"signature\":\"System.Void TagModelSubjects::Subject_11()\",\"identity\":{\"assemblyName\":\"TagModelSubjects\",\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"definitionSubjectId\":\"TagModelSubjects/TagModelSubjects::Subject_11:System.Void()\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_11:System.Void()\",\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-11\",\"signature\":\"System.Void TagModelSubjects::Subject_11()\"},\"nativeSymbol\":\"TagModelSubjects_TagModelSubjects_Subject_11\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":3,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":77,\"handlerOffset\":78,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":4,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":5,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":2,\"ilOffset\":11,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":13,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":20,\"resultType\":\"System.Boolean\",\"callee\":\"System.Formats.Asn1/Asn1Tag::HasSameClassAndValue:System.Boolean(Asn1Tag)\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::HasSameClassAndValue:System.Boolean(Asn1Tag)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::HasSameClassAndValue:System.Boolean(Asn1Tag)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":30,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":31,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":30,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":31,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":33,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":34,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":2,\"ilOffset\":40,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":42,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":48,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":49,\"resultType\":\"System.Boolean\",\"callee\":\"System.Formats.Asn1/Asn1Tag::HasSameClassAndValue:System.Boolean(Asn1Tag)\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::HasSameClassAndValue:System.Boolean(Asn1Tag)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::HasSameClassAndValue:System.Boolean(Asn1Tag)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":59,\"ilOffset\":54,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":56,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":60,\"ilOffset\":57,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":59,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":60,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":62,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":63,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":65,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":66,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":75,\"ilOffset\":67,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":69,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":70,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":89,\"ilOffset\":76,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":78,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":80,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":81,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":89,\"ilOffset\":87,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":89,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"TagModelSubjects\"}";
static const char kMethodJson_12[] = "{\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-12\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_12:System.Void()\",\"signature\":\"System.Void TagModelSubjects::Subject_12()\",\"identity\":{\"assemblyName\":\"TagModelSubjects\",\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"definitionSubjectId\":\"TagModelSubjects/TagModelSubjects::Subject_12:System.Void()\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_12:System.Void()\",\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-12\",\"signature\":\"System.Void TagModelSubjects::Subject_12()\"},\"nativeSymbol\":\"TagModelSubjects_TagModelSubjects_Subject_12\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":77,\"handlerOffset\":78,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":4,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":10,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":11,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":13,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":20,\"resultType\":\"System.Boolean\",\"callee\":\"System.Formats.Asn1/Asn1Tag::op_Equality:System.Boolean(Asn1Tag,Asn1Tag)\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::op_Equality:System.Boolean(Asn1Tag,Asn1Tag)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::op_Equality:System.Boolean(Asn1Tag,Asn1Tag)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":30,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":31,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":30,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":31,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":39,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":40,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":42,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":48,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":49,\"resultType\":\"System.Boolean\",\"callee\":\"System.Formats.Asn1/Asn1Tag::op_Equality:System.Boolean(Asn1Tag,Asn1Tag)\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::op_Equality:System.Boolean(Asn1Tag,Asn1Tag)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::op_Equality:System.Boolean(Asn1Tag,Asn1Tag)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":59,\"ilOffset\":54,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":56,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":60,\"ilOffset\":57,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":59,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":60,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":62,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":63,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":65,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":66,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":75,\"ilOffset\":67,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":69,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":70,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":89,\"ilOffset\":76,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":78,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":80,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":81,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":89,\"ilOffset\":87,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":89,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"TagModelSubjects\"}";
static const char kMethodJson_13[] = "{\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-13\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_13:System.Void()\",\"signature\":\"System.Void TagModelSubjects::Subject_13()\",\"identity\":{\"assemblyName\":\"TagModelSubjects\",\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"definitionSubjectId\":\"TagModelSubjects/TagModelSubjects::Subject_13:System.Void()\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_13:System.Void()\",\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-13\",\"signature\":\"System.Void TagModelSubjects::Subject_13()\"},\"nativeSymbol\":\"TagModelSubjects_TagModelSubjects_Subject_13\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":77,\"handlerOffset\":78,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":4,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":10,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":11,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":13,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":20,\"resultType\":\"System.Boolean\",\"callee\":\"System.Formats.Asn1/Asn1Tag::op_Inequality:System.Boolean(Asn1Tag,Asn1Tag)\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::op_Inequality:System.Boolean(Asn1Tag,Asn1Tag)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::op_Inequality:System.Boolean(Asn1Tag,Asn1Tag)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":30,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":31,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":30,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":31,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":39,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":40,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":42,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":48,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":49,\"resultType\":\"System.Boolean\",\"callee\":\"System.Formats.Asn1/Asn1Tag::op_Inequality:System.Boolean(Asn1Tag,Asn1Tag)\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::op_Inequality:System.Boolean(Asn1Tag,Asn1Tag)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::op_Inequality:System.Boolean(Asn1Tag,Asn1Tag)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":59,\"ilOffset\":54,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":56,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":60,\"ilOffset\":57,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":59,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":60,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":62,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":63,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":65,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":66,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":75,\"ilOffset\":67,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":69,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":70,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":89,\"ilOffset\":76,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":78,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":80,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":81,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":89,\"ilOffset\":87,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":89,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"TagModelSubjects\"}";
static const char kMethodJson_14[] = "{\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-14\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_14:System.Void()\",\"signature\":\"System.Void TagModelSubjects::Subject_14()\",\"identity\":{\"assemblyName\":\"TagModelSubjects\",\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"definitionSubjectId\":\"TagModelSubjects/TagModelSubjects::Subject_14:System.Void()\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_14:System.Void()\",\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-14\",\"signature\":\"System.Void TagModelSubjects::Subject_14()\"},\"nativeSymbol\":\"TagModelSubjects_TagModelSubjects_Subject_14\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":69,\"handlerOffset\":70,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":4,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":5,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":17,\"resultType\":\"System.String\",\"callee\":\"System.Formats.Asn1/Asn1Tag::ToString:System.String()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::ToString:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::ToString:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Formats.Asn1/Asn1Tag\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":27,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":29,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":30,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":42,\"resultType\":\"System.String\",\"callee\":\"System.Formats.Asn1/Asn1Tag::ToString:System.String()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::ToString:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::ToString:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":1,\"constrainedTypeSubjectId\":\"System.Formats.Asn1/Asn1Tag\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":47,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::get_Length:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":52,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":54,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":55,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":57,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":58,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":67,\"ilOffset\":59,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":61,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":62,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":81,\"ilOffset\":68,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":70,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":72,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":73,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":81,\"ilOffset\":79,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":81,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"TagModelSubjects\"}";
static const char kMethodJson_15[] = "{\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-15\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_15:System.Void()\",\"signature\":\"System.Void TagModelSubjects::Subject_15()\",\"identity\":{\"assemblyName\":\"TagModelSubjects\",\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"definitionSubjectId\":\"TagModelSubjects/TagModelSubjects::Subject_15:System.Void()\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_15:System.Void()\",\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-15\",\"signature\":\"System.Void TagModelSubjects::Subject_15()\"},\"nativeSymbol\":\"TagModelSubjects_TagModelSubjects_Subject_15\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":4,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":67,\"handlerOffset\":68,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldloca\",\"opCode\":76,\"operand\":3,\"ilOffset\":2,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E\",\"ilOffset\":4,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":3,\"ilOffset\":10,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":11,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":2,\"ilOffset\":13,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":15,\"resultType\":\"System.Boolean\",\"callee\":\"System.Formats.Asn1/Asn1Tag::TryDecode:System.Boolean(System.ReadOnlySpan\\u003CSystem.Byte\\u003E,Asn1Tag\\u0026,System.Int32\\u0026)\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::TryDecode:System.Boolean(System.ReadOnlySpan\\u003CSystem.Byte\\u003E,Asn1Tag\\u0026,System.Int32\\u0026)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::TryDecode:System.Boolean(System.ReadOnlySpan\\u003CSystem.Byte\\u003E,Asn1Tag\\u0026,System.Int32\\u0026)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":25,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":26,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":3,\"ilOffset\":26,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E\",\"ilOffset\":28,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":3,\"ilOffset\":34,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":35,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":2,\"ilOffset\":37,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":39,\"resultType\":\"System.Boolean\",\"callee\":\"System.Formats.Asn1/Asn1Tag::TryDecode:System.Boolean(System.ReadOnlySpan\\u003CSystem.Byte\\u003E,Asn1Tag\\u0026,System.Int32\\u0026)\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::TryDecode:System.Boolean(System.ReadOnlySpan\\u003CSystem.Byte\\u003E,Asn1Tag\\u0026,System.Int32\\u0026)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::TryDecode:System.Boolean(System.ReadOnlySpan\\u003CSystem.Byte\\u003E,Asn1Tag\\u0026,System.Int32\\u0026)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":49,\"ilOffset\":44,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":46,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":50,\"ilOffset\":47,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":49,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":50,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":52,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":53,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":55,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":56,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":65,\"ilOffset\":57,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":59,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":60,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":79,\"ilOffset\":66,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":68,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":70,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":71,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":79,\"ilOffset\":77,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":79,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"TagModelSubjects\"}";
static const char kMethodJson_16[] = "{\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-16\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_16:System.Void()\",\"signature\":\"System.Void TagModelSubjects::Subject_16()\",\"identity\":{\"assemblyName\":\"TagModelSubjects\",\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"definitionSubjectId\":\"TagModelSubjects/TagModelSubjects::Subject_16:System.Void()\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_16:System.Void()\",\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-16\",\"signature\":\"System.Void TagModelSubjects::Subject_16()\"},\"nativeSymbol\":\"TagModelSubjects_TagModelSubjects_Subject_16\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":4,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":81,\"handlerOffset\":82,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldloca\",\"opCode\":76,\"operand\":2,\"ilOffset\":2,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":4,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":5,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":3,\"ilOffset\":11,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\",\"ilOffset\":13,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":3,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":20,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":22,\"resultType\":\"System.Boolean\",\"callee\":\"System.Formats.Asn1/Asn1Tag::TryEncode:System.Boolean(System.Span\\u003CSystem.Byte\\u003E,System.Int32\\u0026)\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::TryEncode:System.Boolean(System.Span\\u003CSystem.Byte\\u003E,System.Int32\\u0026)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::TryEncode:System.Boolean(System.Span\\u003CSystem.Byte\\u003E,System.Int32\\u0026)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":32,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":33,\"ilOffset\":30,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":2,\"ilOffset\":33,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":35,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Formats.Asn1/Asn1Tag\",\"ilOffset\":36,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"type\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":3,\"ilOffset\":42,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\",\"ilOffset\":44,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":3,\"ilOffset\":50,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":51,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":53,\"resultType\":\"System.Boolean\",\"callee\":\"System.Formats.Asn1/Asn1Tag::TryEncode:System.Boolean(System.Span\\u003CSystem.Byte\\u003E,System.Int32\\u0026)\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::TryEncode:System.Boolean(System.Span\\u003CSystem.Byte\\u003E,System.Int32\\u0026)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::TryEncode:System.Boolean(System.Span\\u003CSystem.Byte\\u003E,System.Int32\\u0026)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":63,\"ilOffset\":58,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":60,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":64,\"ilOffset\":61,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":63,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":64,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":66,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":67,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":69,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":70,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":79,\"ilOffset\":71,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":73,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":74,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":93,\"ilOffset\":80,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":82,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":84,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":85,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":93,\"ilOffset\":91,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":93,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"TagModelSubjects\"}";
static const char kMethodJson_17[] = "{\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-17\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_17:System.Void()\",\"signature\":\"System.Void TagModelSubjects::Subject_17()\",\"identity\":{\"assemblyName\":\"TagModelSubjects\",\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"definitionSubjectId\":\"TagModelSubjects/TagModelSubjects::Subject_17:System.Void()\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_17:System.Void()\",\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-17\",\"signature\":\"System.Void TagModelSubjects::Subject_17()\"},\"nativeSymbol\":\"TagModelSubjects_TagModelSubjects_Subject_17\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":4,\"resultType\":\"System.Formats.Asn1/Asn1Tag\",\"callee\":\"System.Formats.Asn1/Asn1Tag::.ctor:System.Void(UniversalTagNumber,System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::.ctor:System.Void(UniversalTagNumber,System.Boolean)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"TagModelSubjects\"}";
static const char kMethodJson_18[] = "{\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-18\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_18:System.Void()\",\"signature\":\"System.Void TagModelSubjects::Subject_18()\",\"identity\":{\"assemblyName\":\"TagModelSubjects\",\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"definitionSubjectId\":\"TagModelSubjects/TagModelSubjects::Subject_18:System.Void()\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::Subject_18:System.Void()\",\"methodId\":\"tag-model-subjects.tag-model-subjects.subject-18\",\"signature\":\"System.Void TagModelSubjects::Subject_18()\"},\"nativeSymbol\":\"TagModelSubjects_TagModelSubjects_Subject_18\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":14,\"handlerOffset\":15,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":5,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":6,\"resultType\":\"System.Formats.Asn1/Asn1Tag\",\"callee\":\"System.Formats.Asn1/Asn1Tag::.ctor:System.Void(TagClass,System.Int32,System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Formats.Asn1\",\"subjectKind\":\"method\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag::.ctor:System.Void(TagClass,System.Int32,System.Boolean)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Formats.Asn1\",\"subjectId\":\"System.Formats.Asn1/Asn1Tag\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":11,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":26,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":15,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"ilOffset\":18,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"TagModelSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"TagModelSubjects\",\"subjectId\":\"TagModelSubjects/TagModelSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"TagModelSubjects/TagModelSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":26,\"ilOffset\":24,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":26,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"TagModelSubjects\"}";

extern "C" const JitMethodEntry kChaosJitMethodEntries[19] =
{
    { kMethodJson_0, 7846u, 0x00000003u, 0u },
    { kMethodJson_1, 7834u, 0x00000004u, 0u },
    { kMethodJson_2, 6180u, 0x00000005u, 0u },
    { kMethodJson_3, 8647u, 0x00000006u, 0u },
    { kMethodJson_4, 7856u, 0x00000007u, 0u },
    { kMethodJson_5, 8338u, 0x00000008u, 0u },
    { kMethodJson_6, 8448u, 0x00000009u, 0u },
    { kMethodJson_7, 6998u, 0x0000000Au, 0u },
    { kMethodJson_8, 7852u, 0x0000000Bu, 0u },
    { kMethodJson_9, 6132u, 0x0000000Cu, 0u },
    { kMethodJson_10, 6304u, 0x0000000Du, 0u },
    { kMethodJson_11, 8430u, 0x0000000Eu, 0u },
    { kMethodJson_12, 8451u, 0x0000000Fu, 0u },
    { kMethodJson_13, 8463u, 0x00000010u, 0u },
    { kMethodJson_14, 7528u, 0x00000011u, 0u },
    { kMethodJson_15, 8095u, 0x00000012u, 0u },
    { kMethodJson_16, 9042u, 0x00000013u, 0u },
    { kMethodJson_17, 3214u, 0x00000014u, 0u },
    { kMethodJson_18, 3324u, 0x00000015u, 0u }
};

extern "C" const uint32_t kChaosJitMethodEntryCount = 19u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitMethods(kChaosJitMethodEntries, kChaosJitMethodEntryCount);
}