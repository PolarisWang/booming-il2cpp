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

namespace chaos::il2cpp::codegen::GuidRandomHashcodePatchEntry {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry = {{nullptr, nullptr, 5713635720441985600ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry = static_cast<CHAOS_IL2CPP_INTPTR>(5713635720441985600ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {{nullptr, nullptr, 5474029880995115448ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly = static_cast<CHAOS_IL2CPP_INTPTR>(5474029880995115448ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName = {{nullptr, nullptr, 17082367815459723707ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = static_cast<CHAOS_IL2CPP_INTPTR>(17082367815459723707ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo = {{nullptr, nullptr, 4137207361503509124ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo = static_cast<CHAOS_IL2CPP_INTPTR>(4137207361503509124ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo = {{nullptr, nullptr, 17040031516751226236ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo = static_cast<CHAOS_IL2CPP_INTPTR>(17040031516751226236ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo = {{nullptr, nullptr, 10748947813473285525ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = static_cast<CHAOS_IL2CPP_INTPTR>(10748947813473285525ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_String = {{nullptr, nullptr, 1782325859292956794ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
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

struct chaos_type_GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry
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

CHAOS_IL2CPP_INT32 chaos_static_GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry___exitCode = 0;

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_0(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_1(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_2(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_3(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_4(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_5(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_6(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_7(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_8(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_9(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_10(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_11(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_12(void);
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_13(void);
extern "C" void GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[16] = {
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
	1u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[15];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		15u,
		1u,
		261690089u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 1u, 0u },  // GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_0
		{ 1u, 0u },  // GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_1
		{ 1u, 0u },  // GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_2
		{ 1u, 0u },  // GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_3
		{ 1u, 0u },  // GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_4
		{ 1u, 0u },  // GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_5
		{ 1u, 0u },  // GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_6
		{ 1u, 0u },  // GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_7
		{ 1u, 0u },  // GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_8
		{ 1u, 0u },  // GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_9
		{ 1u, 0u },  // GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_10
		{ 1u, 0u },  // GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_11
		{ 1u, 0u },  // GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_12
		{ 1u, 0u },  // GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_13
		{ 0u, 1u },  // GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Run_System_Int32
	},
	{
		1u,
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
		"GuidRandomHashcodePatchEntry",
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
		&chaos_mt_GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry.hot,
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
		/* .name_utf8         = */ "GuidRandomHashcodePatchEntry",
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
		::chaos::il2cpp::runtime_core::RegisterModule("GuidRandomHashcodePatchEntry", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[15] = {
	{ "Subject_0", 0x00000003u, 0u },  // GuidRandomHashcodePatchEntry
	{ "Subject_1", 0x00000004u, 0u },  // GuidRandomHashcodePatchEntry
	{ "Subject_2", 0x00000005u, 0u },  // GuidRandomHashcodePatchEntry
	{ "Subject_3", 0x00000006u, 0u },  // GuidRandomHashcodePatchEntry
	{ "Subject_4", 0x00000007u, 0u },  // GuidRandomHashcodePatchEntry
	{ "Subject_5", 0x00000008u, 0u },  // GuidRandomHashcodePatchEntry
	{ "Subject_6", 0x00000009u, 0u },  // GuidRandomHashcodePatchEntry
	{ "Subject_7", 0x0000000Au, 0u },  // GuidRandomHashcodePatchEntry
	{ "Subject_8", 0x0000000Bu, 0u },  // GuidRandomHashcodePatchEntry
	{ "Subject_9", 0x0000000Cu, 0u },  // GuidRandomHashcodePatchEntry
	{ "Subject_10", 0x0000000Du, 0u },  // GuidRandomHashcodePatchEntry
	{ "Subject_11", 0x0000000Eu, 0u },  // GuidRandomHashcodePatchEntry
	{ "Subject_12", 0x0000000Fu, 0u },  // GuidRandomHashcodePatchEntry
	{ "Subject_13", 0x00000010u, 0u },  // GuidRandomHashcodePatchEntry
	{ "Run", 0x00000011u, 1u },  // GuidRandomHashcodePatchEntry
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "GuidRandomHashcodePatchEntry", "", 0u, 15u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[15] = {
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
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[15] = {
	{ reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GuidRandomHashcodePatchEntry::Subject_0
	{ reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GuidRandomHashcodePatchEntry::Subject_1
	{ reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GuidRandomHashcodePatchEntry::Subject_2
	{ reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GuidRandomHashcodePatchEntry::Subject_3
	{ reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GuidRandomHashcodePatchEntry::Subject_4
	{ reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GuidRandomHashcodePatchEntry::Subject_5
	{ reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GuidRandomHashcodePatchEntry::Subject_6
	{ reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GuidRandomHashcodePatchEntry::Subject_7
	{ reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GuidRandomHashcodePatchEntry::Subject_8
	{ reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GuidRandomHashcodePatchEntry::Subject_9
	{ reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GuidRandomHashcodePatchEntry::Subject_10
	{ reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GuidRandomHashcodePatchEntry::Subject_11
	{ reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GuidRandomHashcodePatchEntry::Subject_12
	{ reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GuidRandomHashcodePatchEntry::Subject_13
	{ reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Run_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // GuidRandomHashcodePatchEntry::Run
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"GuidRandomHashcodePatchEntry",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	15u,
	s_hotpatch_slots,
	15u,
	s_hotpatch_entries,
	15u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[1] = {
	"GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::_exitCode",
};

extern "C" void* kChaosExternalRuntimeFnTable[1] = {
	nullptr,
};

extern "C" int32_t kChaosExternalRuntimeCount = 1;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[15])() = {
	reinterpret_cast<void(*)()>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_0),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_1),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_2),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_3),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_4),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_5),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_6),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_7),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_8),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_9),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_10),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_11),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_12),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_13),
	reinterpret_cast<void(*)()>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Run_System_Int32),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
static void (*kBenchmarkWrappers[15])() = {
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
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[14])(0);},
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
static void* const kMethodPointers[15] = {
	reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_0),
	reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_1),
	reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_2),
	reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_3),
	reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_4),
	reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_5),
	reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_6),
	reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_7),
	reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_8),
	reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_9),
	reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_10),
	reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_11),
	reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_12),
	reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_13),
	reinterpret_cast<void*>(&GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Run_System_Int32),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 15u,
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
	.image_name_utf8    = "GuidRandomHashcodePatchEntry",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry[1] = {
	{ 0u, "GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry[15] = {
	{ 0u, "GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_0:System.Int32()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_1:System.Int32()", "Subject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_2:System.Int32()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_3:System.Int32()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_4:System.Int32()", "Subject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_5:System.Int32()", "Subject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_6:System.Int32()", "Subject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_7:System.Int32()", "Subject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_8:System.Int32()", "Subject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_9:System.Int32()", "Subject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_10:System.Int32()", "Subject_10", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_11:System.Int32()", "Subject_11", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_12:System.Int32()", "Subject_12", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_13:System.Int32()", "Subject_13", "System.Void", 0, nullptr, 0u },
	{ 0u, "GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Run:System.Void(System.Int32)", "Run", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0u, "GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry", "GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry", "", "GuidRandomHashcodePatchEntry", "GuidRandomHashcodePatchEntry", nullptr, kReflFields_GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry, 1u, nullptr, 0u,
	kReflMethods_GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry, 15u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "GuidRandomHashcodePatchEntry", kReflTypePtrs, 1u };

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
// Managed method: GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_0()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177280);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_1()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177279);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_2()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177278);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_3()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177277);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_4()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177276);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_5()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177275);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_6()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_6(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177274);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_7()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_7(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177273);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_8()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177272);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_9()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_9(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177271);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_10()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_10(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177270);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_11()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_11(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177269);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_12()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_12(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177268);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Subject_13()
extern "C" CHAOS_IL2CPP_INT32 GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_13(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177267);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: GuidRandomHashcodePatchEntry/GuidRandomHashcodePatchEntry::Run(System.Int32)
extern "C" void GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
			_s0 = chaos_args[0];
			{
				const auto chaos_switch_value = static_cast<CHAOS_IL2CPP_INT32>(_s0);
				switch (chaos_switch_value)
				{
					case 0:
						{
							auto& _d0 = s_hotpatch_entries[0];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d0)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d0))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d0.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_0();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 1:
						{
							auto& _d1 = s_hotpatch_entries[1];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d1)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d1))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d1.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_1();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 2:
						{
							auto& _d2 = s_hotpatch_entries[2];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d2)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d2))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d2.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_2();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 3:
						{
							auto& _d3 = s_hotpatch_entries[3];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d3)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d3))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d3.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_3();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 4:
						{
							auto& _d4 = s_hotpatch_entries[4];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d4)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d4))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d4.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_4();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 5:
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
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_5();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 6:
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
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_6();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 7:
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
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_7();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 8:
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
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_8();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 9:
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
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_9();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 10:
						{
							auto& _d10 = s_hotpatch_entries[10];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d10)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d10))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d10.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_10();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 11:
						{
							auto& _d11 = s_hotpatch_entries[11];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d11)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d11))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d11.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_11();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 12:
						{
							auto& _d12 = s_hotpatch_entries[12];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d12)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d12))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d12.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_12();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 13:
					{
						{
							auto& _d13 = s_hotpatch_entries[13];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d13)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d13))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d13.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_13();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
						break;
					}
					default:
					{
						break;
					}
				}
			}
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry___exitCode = chaos_value;
			}
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
			_s0 = chaos_args[0];
			{
				const auto chaos_switch_value = static_cast<CHAOS_IL2CPP_INT32>(_s0);
				switch (chaos_switch_value)
				{
					case 0:
						{
							auto& _d0 = s_hotpatch_entries[0];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d0)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d0))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d0.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_0();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 1:
						{
							auto& _d1 = s_hotpatch_entries[1];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d1)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d1))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d1.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_1();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 2:
						{
							auto& _d2 = s_hotpatch_entries[2];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d2)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d2))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d2.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_2();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 3:
						{
							auto& _d3 = s_hotpatch_entries[3];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d3)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d3))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d3.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_3();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 4:
						{
							auto& _d4 = s_hotpatch_entries[4];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d4)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d4))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d4.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_4();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 5:
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
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_5();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 6:
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
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_6();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 7:
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
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_7();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 8:
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
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_8();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 9:
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
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_9();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 10:
						{
							auto& _d10 = s_hotpatch_entries[10];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d10)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d10))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d10.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_10();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 11:
						{
							auto& _d11 = s_hotpatch_entries[11];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d11)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d11))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d11.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_11();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 12:
						{
							auto& _d12 = s_hotpatch_entries[12];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d12)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d12))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d12.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_12();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 13:
					{
						{
							auto& _d13 = s_hotpatch_entries[13];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d13)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d13))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d13.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_13();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
						break;
					}
					default:
					{
						break;
					}
				}
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry___exitCode = chaos_value;
			}
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
			_s0 = chaos_args[0];
			{
				const auto chaos_switch_value = static_cast<CHAOS_IL2CPP_INT32>(_s0);
				switch (chaos_switch_value)
				{
					case 0:
						{
							auto& _d0 = s_hotpatch_entries[0];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d0)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d0))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d0.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_0();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 1:
						{
							auto& _d1 = s_hotpatch_entries[1];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d1)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d1))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d1.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_1();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 2:
						{
							auto& _d2 = s_hotpatch_entries[2];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d2)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d2))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d2.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_2();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 3:
						{
							auto& _d3 = s_hotpatch_entries[3];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d3)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d3))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d3.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_3();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 4:
						{
							auto& _d4 = s_hotpatch_entries[4];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d4)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d4))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d4.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_4();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 5:
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
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_5();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 6:
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
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_6();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 7:
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
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_7();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 8:
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
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_8();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 9:
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
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_9();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 10:
						{
							auto& _d10 = s_hotpatch_entries[10];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d10)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d10))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d10.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_10();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 11:
						{
							auto& _d11 = s_hotpatch_entries[11];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d11)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d11))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d11.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_11();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 12:
						{
							auto& _d12 = s_hotpatch_entries[12];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d12)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d12))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d12.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_12();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 13:
					{
						{
							auto& _d13 = s_hotpatch_entries[13];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d13)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d13))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d13.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry_Subject_13();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
						break;
					}
					default:
					{
						break;
					}
				}
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{
				chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_GuidRandomHashcodePatchEntry_GuidRandomHashcodePatchEntry___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}



}  // namespace chaos::il2cpp::codegen::GuidRandomHashcodePatchEntry
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 15;

extern "C" void ChaosJitRegisterAll() {}