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

namespace chaos::il2cpp::codegen::PinvokeDllimportSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_PinvokeDllimportSubjects_PinvokeDllimportSubjects = {{nullptr, nullptr, 10274367341970332782ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_PinvokeDllimportSubjects_PinvokeDllimportSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(10274367341970332782ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
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

struct chaos_type_PinvokeDllimportSubjects_PinvokeDllimportSubjects
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

CHAOS_IL2CPP_INT32 chaos_static_PinvokeDllimportSubjects_PinvokeDllimportSubjects___exitCode = 0;

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_0(void);
extern "C" void PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_1(void);
extern "C" void PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_2(void);
extern "C" void PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_3(void);
extern "C" CHAOS_IL2CPP_INTPTR PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetCurrentProcess(void);
extern "C" CHAOS_IL2CPP_INTPTR PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetCurrentThread(void);
extern "C" CHAOS_IL2CPP_INTPTR PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetModuleHandle_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetProcessId_System_IntPtr(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentProcess(void);
extern "C" CHAOS_IL2CPP_INT32 PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentThread(void);
extern "C" CHAOS_IL2CPP_INT32 PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetModuleHandle(void);
extern "C" CHAOS_IL2CPP_INT32 PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetProcessId(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[13] = {
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	1u,
	2u,
	2u,
	2u,
	2u,
	2u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[12];
	CHAOS_IL2CPP_UINT8 params[2];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		12u,
		2u,
		424129345u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_0
		{ 0u, 0u },  // PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_1
		{ 0u, 0u },  // PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_2
		{ 0u, 0u },  // PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_3
		{ 2u, 0u },  // PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetCurrentProcess
		{ 2u, 0u },  // PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetCurrentThread
		{ 2u, 1u },  // PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetModuleHandle_System_String
		{ 2u, 1u },  // PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetProcessId_System_IntPtr
		{ 1u, 0u },  // PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentProcess
		{ 1u, 0u },  // PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentThread
		{ 1u, 0u },  // PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetModuleHandle
		{ 1u, 0u },  // PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetProcessId
	},
	{
		2u,
		2u,
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
		"PinvokeDllimportSubjects",
		"PinvokeDllimportNativeEntry",
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
		&chaos_mt_PinvokeDllimportSubjects_PinvokeDllimportSubjects.hot,
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
		/* .name_utf8         = */ "PinvokeDllimportSubjects",
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
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 — deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,

		/* .abi_manifest      = */ s_abi_manifest,
	};
	static const CHAOS_IL2CPP_UINT32 s_native_aot_module_id =
		::chaos::il2cpp::runtime_core::RegisterModule("PinvokeDllimportSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[12] = {
	{ "GetCurrentProcess", 0x00000008u, 0u },  // PinvokeDllimportNativeEntry
	{ "GetCurrentThread", 0x0000000Au, 0u },  // PinvokeDllimportNativeEntry
	{ "GetModuleHandle", 0x0000000Eu, 1u },  // PinvokeDllimportNativeEntry
	{ "GetProcessId", 0x0000000Cu, 1u },  // PinvokeDllimportNativeEntry
	{ "TestGetCurrentProcess", 0x00000009u, 0u },  // PinvokeDllimportNativeEntry
	{ "TestGetCurrentThread", 0x0000000Bu, 0u },  // PinvokeDllimportNativeEntry
	{ "TestGetModuleHandle", 0x0000000Fu, 0u },  // PinvokeDllimportNativeEntry
	{ "TestGetProcessId", 0x0000000Du, 0u },  // PinvokeDllimportNativeEntry
	{ "Subject_0", 0x00000004u, 0u },  // PinvokeDllimportSubjects
	{ "Subject_1", 0x00000005u, 0u },  // PinvokeDllimportSubjects
	{ "Subject_2", 0x00000006u, 0u },  // PinvokeDllimportSubjects
	{ "Subject_3", 0x00000007u, 0u },  // PinvokeDllimportSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[2] = {
	{ "PinvokeDllimportNativeEntry", "", 0u, 8u },
	{ "PinvokeDllimportSubjects", "", 8u, 4u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[12] = {
	{ 0x00000004u, 0u },
	{ 0x00000005u, 1u },
	{ 0x00000006u, 2u },
	{ 0x00000007u, 3u },
	{ 0x00000008u, 4u },
	{ 0x00000009u, 8u },
	{ 0x0000000Au, 5u },
	{ 0x0000000Bu, 9u },
	{ 0x0000000Cu, 7u },
	{ 0x0000000Du, 11u },
	{ 0x0000000Eu, 6u },
	{ 0x0000000Fu, 10u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[12] = {
	{ reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PinvokeDllimportSubjects::Subject_0
	{ reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PinvokeDllimportSubjects::Subject_1
	{ reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PinvokeDllimportSubjects::Subject_2
	{ reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PinvokeDllimportSubjects::Subject_3
	{ reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetCurrentProcess), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PinvokeDllimportNativeEntry::GetCurrentProcess
	{ reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetCurrentThread), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PinvokeDllimportNativeEntry::GetCurrentThread
	{ reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetModuleHandle_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PinvokeDllimportNativeEntry::GetModuleHandle
	{ reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetProcessId_System_IntPtr), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PinvokeDllimportNativeEntry::GetProcessId
	{ reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentProcess), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PinvokeDllimportNativeEntry::TestGetCurrentProcess
	{ reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentThread), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PinvokeDllimportNativeEntry::TestGetCurrentThread
	{ reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetModuleHandle), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PinvokeDllimportNativeEntry::TestGetModuleHandle
	{ reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetProcessId), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // PinvokeDllimportNativeEntry::TestGetProcessId
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"PinvokeDllimportSubjects",
	s_hotpatch_types,
	2u,
	s_hotpatch_methods,
	12u,
	s_hotpatch_slots,
	12u,
	s_hotpatch_entries,
	12u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[1] = {
	"PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode",
};

extern "C" void* kChaosExternalRuntimeFnTable[1] = {
	nullptr,
};

extern "C" int32_t kChaosExternalRuntimeCount = 1;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[12])() = {
	reinterpret_cast<void(*)()>(&PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetCurrentProcess),
	reinterpret_cast<void(*)()>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetCurrentThread),
	reinterpret_cast<void(*)()>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetModuleHandle_System_String),
	reinterpret_cast<void(*)()>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetProcessId_System_IntPtr),
	reinterpret_cast<void(*)()>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentProcess),
	reinterpret_cast<void(*)()>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentThread),
	reinterpret_cast<void(*)()>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetModuleHandle),
	reinterpret_cast<void(*)()>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetProcessId),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
static void (*kBenchmarkWrappers[12])() = {
	[]() {kAotMethods[0]();},
	[]() {kAotMethods[1]();},
	[]() {kAotMethods[2]();},
	[]() {kAotMethods[3]();},
	[]() {kAotMethods[4]();},
	[]() {kAotMethods[5]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[6])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[7])(0);},
	[]() {kAotMethods[8]();},
	[]() {kAotMethods[9]();},
	[]() {kAotMethods[10]();},
	[]() {kAotMethods[11]();},
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
static void* const kMethodPointers[12] = {
	reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_0),
	reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_1),
	reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_2),
	reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_3),
	reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetCurrentProcess),
	reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetCurrentThread),
	reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetModuleHandle_System_String),
	reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetProcessId_System_IntPtr),
	reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentProcess),
	reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentThread),
	reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetModuleHandle),
	reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetProcessId),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 12u,
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
	.image_name_utf8    = "PinvokeDllimportSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_PinvokeDllimportSubjects_PinvokeDllimportSubjects[1] = {
	{ 0u, "PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_PinvokeDllimportSubjects_PinvokeDllimportSubjects[4] = {
	{ 0u, "PinvokeDllimportSubjects/PinvokeDllimportSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "PinvokeDllimportSubjects/PinvokeDllimportSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "PinvokeDllimportSubjects/PinvokeDllimportSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "PinvokeDllimportSubjects/PinvokeDllimportSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_PinvokeDllimportSubjects_PinvokeDllimportNativeEntry[8] = {
	{ 0u, "PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetCurrentProcess:System.IntPtr()", "GetCurrentProcess", "System.Void", 0, nullptr, 0u },
	{ 0u, "PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetCurrentThread:System.IntPtr()", "GetCurrentThread", "System.Void", 0, nullptr, 0u },
	{ 0u, "PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetModuleHandle:System.IntPtr(System.String)", "GetModuleHandle", "System.Void", 0, nullptr, 0u },
	{ 0u, "PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetProcessId:System.UInt32(System.IntPtr)", "GetProcessId", "System.Void", 0, nullptr, 0u },
	{ 0u, "PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetCurrentProcess:System.Int32()", "TestGetCurrentProcess", "System.Void", 0, nullptr, 0u },
	{ 0u, "PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetCurrentThread:System.Int32()", "TestGetCurrentThread", "System.Void", 0, nullptr, 0u },
	{ 0u, "PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetModuleHandle:System.Int32()", "TestGetModuleHandle", "System.Void", 0, nullptr, 0u },
	{ 0u, "PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetProcessId:System.Int32()", "TestGetProcessId", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[2] = {
	{ 0u, "PinvokeDllimportSubjects/PinvokeDllimportSubjects", "PinvokeDllimportSubjects/PinvokeDllimportSubjects", "", "PinvokeDllimportSubjects", "PinvokeDllimportSubjects", nullptr, kReflFields_PinvokeDllimportSubjects_PinvokeDllimportSubjects, 1u, nullptr, 0u,
	kReflMethods_PinvokeDllimportSubjects_PinvokeDllimportSubjects, 4u },
	{ 0u, "PinvokeDllimportSubjects/PinvokeDllimportNativeEntry", "PinvokeDllimportSubjects/PinvokeDllimportNativeEntry", "", "PinvokeDllimportNativeEntry", "PinvokeDllimportNativeEntry", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_PinvokeDllimportSubjects_PinvokeDllimportNativeEntry, 8u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[2] = {
	&kReflTypes[0],
	&kReflTypes[1],
};

static const ReflectionQueryImageDescriptor kReflImage = { "PinvokeDllimportSubjects", kReflTypePtrs, 2u };

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
// Managed method: PinvokeDllimportSubjects/PinvokeDllimportSubjects::Subject_0()
extern "C" void PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_0(void)
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


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
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
				chaos_static_PinvokeDllimportSubjects_PinvokeDllimportSubjects___exitCode = chaos_value;
			}
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
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
				chaos_static_PinvokeDllimportSubjects_PinvokeDllimportSubjects___exitCode = chaos_value;
			}
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
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
				chaos_static_PinvokeDllimportSubjects_PinvokeDllimportSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: PinvokeDllimportSubjects/PinvokeDllimportSubjects::Subject_1()
extern "C" void PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_1(void)
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


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
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
				chaos_static_PinvokeDllimportSubjects_PinvokeDllimportSubjects___exitCode = chaos_value;
			}
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
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
				chaos_static_PinvokeDllimportSubjects_PinvokeDllimportSubjects___exitCode = chaos_value;
			}
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
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
				chaos_static_PinvokeDllimportSubjects_PinvokeDllimportSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: PinvokeDllimportSubjects/PinvokeDllimportSubjects::Subject_2()
extern "C" void PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_2(void)
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


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
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
				chaos_static_PinvokeDllimportSubjects_PinvokeDllimportSubjects___exitCode = chaos_value;
			}
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
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
				chaos_static_PinvokeDllimportSubjects_PinvokeDllimportSubjects___exitCode = chaos_value;
			}
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
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
				chaos_static_PinvokeDllimportSubjects_PinvokeDllimportSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: PinvokeDllimportSubjects/PinvokeDllimportSubjects::Subject_3()
extern "C" void PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_3(void)
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


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
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
				chaos_static_PinvokeDllimportSubjects_PinvokeDllimportSubjects___exitCode = chaos_value;
			}
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
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
				chaos_static_PinvokeDllimportSubjects_PinvokeDllimportSubjects___exitCode = chaos_value;
			}
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
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
				chaos_static_PinvokeDllimportSubjects_PinvokeDllimportSubjects___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// P/Invoke: kernel32!GetCurrentProcess (blittable)
extern "C" CHAOS_IL2CPP_INTPTR PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetCurrentProcess(void)
{
	using FnPtr = CHAOS_IL2CPP_INTPTR(*)();
	static void* s_pinvoke_lib_ = nullptr;
	static FnPtr s_pinvoke_fn_ = nullptr;
	if (s_pinvoke_fn_ == nullptr)
	{
		s_pinvoke_lib_ = ::chaos::il2cpp::runtime_core::TryResolveDllImport("PinvokeDllimportSubjects", "kernel32");
		if (s_pinvoke_lib_ == nullptr)
		{
		s_pinvoke_lib_ = ::chaos::il2cpp::runtime_core::NativeLibraryLoad("kernel32");
		}
		if (s_pinvoke_lib_ == nullptr) CHAOS_IL2CPP_FAIL();
		s_pinvoke_fn_ = reinterpret_cast<FnPtr>(
			::chaos::il2cpp::runtime_core::NativeLibraryGetProcAddress(s_pinvoke_lib_, "GetCurrentProcess"));
		if (s_pinvoke_fn_ == nullptr) CHAOS_IL2CPP_FAIL();
	}

	GC_TRANSITION_TO_PREEMPTIVE();
	return s_pinvoke_fn_();
	GC_TRANSITION_TO_COOPERATIVE();
}

// AOT-unreachable stub: PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetCurrentThread:System.IntPtr()
extern "C" CHAOS_IL2CPP_INTPTR PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetCurrentThread(void)
{
	return {};
}

// AOT-unreachable stub: PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetModuleHandle:System.IntPtr(System.String)
extern "C" CHAOS_IL2CPP_INTPTR PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetModuleHandle_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return {};
}

// AOT-unreachable stub: PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetProcessId:System.UInt32(System.IntPtr)
extern "C" CHAOS_IL2CPP_INTPTR PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetProcessId_System_IntPtr(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return {};
}

// Managed method: PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetCurrentProcess()
extern "C" CHAOS_IL2CPP_INT32 PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentProcess(void)
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


	{
		auto& _d0 = s_hotpatch_entries[0];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d0)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d0))
		{
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d0.method_key, nullptr, &_d_hpresult);
		}
		else
		{
			_d_hpresult = PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetCurrentProcess();
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		if (chaos_left != chaos_right)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		chaos_locals[1] = _s0;
		_s0 = chaos_locals[1];
		return static_cast<CHAOS_IL2CPP_INT32>(_s0);
	}
}

// Managed method: PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetCurrentThread()
extern "C" CHAOS_IL2CPP_INT32 PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentThread(void)
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


	{
		auto& _d1 = s_hotpatch_entries[1];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d1)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d1))
		{
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d1.method_key, nullptr, &_d_hpresult);
		}
		else
		{
			_d_hpresult = PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetCurrentThread();
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(-2);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		if (chaos_left != chaos_right)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		chaos_locals[1] = _s0;
		_s0 = chaos_locals[1];
		return static_cast<CHAOS_IL2CPP_INT32>(_s0);
	}
}

// Managed method: PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetModuleHandle()
extern "C" CHAOS_IL2CPP_INT32 PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetModuleHandle(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	_s0 = 0;
	{
		const auto chaos_arg_0 = _s0;
		auto& _d2 = s_hotpatch_entries[2];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d2)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d2))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d2.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetModuleHandle_System_String(chaos_arg_0);
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	{
		if (_s0 == 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		chaos_locals[1] = _s0;
		_s0 = chaos_locals[1];
		return static_cast<CHAOS_IL2CPP_INT32>(_s0);
	}
}

// Managed method: PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetProcessId()
extern "C" CHAOS_IL2CPP_INT32 PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetProcessId(void)
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


	{
		auto& _d0 = s_hotpatch_entries[0];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d0)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d0))
		{
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d0.method_key, nullptr, &_d_hpresult);
		}
		else
		{
			_d_hpresult = PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetCurrentProcess();
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	{
		const auto chaos_arg_0 = _s0;
		auto& _d3 = s_hotpatch_entries[3];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
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
			_d_hpresult = PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetProcessId_System_IntPtr(chaos_arg_0);
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	{
		if (_s0 == 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		chaos_locals[2] = _s0;
		_s0 = chaos_locals[2];
		return static_cast<CHAOS_IL2CPP_INT32>(_s0);
	}
}



}  // namespace chaos::il2cpp::codegen::PinvokeDllimportSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 12;

// ── JIT Method Entry Table ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kMethodJson_0[] = "{\"methodId\":\"pinvoke-dllimport-subjects.pinvoke-dllimport-subjects.subject-0\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::Subject_0:System.Void()\",\"signature\":\"System.Void PinvokeDllimportSubjects::Subject_0()\",\"identity\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"declaringTypeSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects\",\"definitionSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::Subject_0:System.Void()\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::Subject_0:System.Void()\",\"methodId\":\"pinvoke-dllimport-subjects.pinvoke-dllimport-subjects.subject-0\",\"signature\":\"System.Void PinvokeDllimportSubjects::Subject_0()\"},\"nativeSymbol\":\"PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetCurrentProcess:System.Int32()\",\"reference\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetCurrentProcess:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetCurrentProcess:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentProcess\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetCurrentProcess:System.Int32()\",\"reference\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetCurrentProcess:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetCurrentProcess:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentProcess\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PinvokeDllimportSubjects\"}";
static const char kMethodJson_1[] = "{\"methodId\":\"pinvoke-dllimport-subjects.pinvoke-dllimport-subjects.subject-1\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::Subject_1:System.Void()\",\"signature\":\"System.Void PinvokeDllimportSubjects::Subject_1()\",\"identity\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"declaringTypeSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects\",\"definitionSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::Subject_1:System.Void()\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::Subject_1:System.Void()\",\"methodId\":\"pinvoke-dllimport-subjects.pinvoke-dllimport-subjects.subject-1\",\"signature\":\"System.Void PinvokeDllimportSubjects::Subject_1()\"},\"nativeSymbol\":\"PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetCurrentThread:System.Int32()\",\"reference\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetCurrentThread:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetCurrentThread:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentThread\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetCurrentThread:System.Int32()\",\"reference\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetCurrentThread:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetCurrentThread:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentThread\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PinvokeDllimportSubjects\"}";
static const char kMethodJson_2[] = "{\"methodId\":\"pinvoke-dllimport-subjects.pinvoke-dllimport-subjects.subject-2\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::Subject_2:System.Void()\",\"signature\":\"System.Void PinvokeDllimportSubjects::Subject_2()\",\"identity\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"declaringTypeSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects\",\"definitionSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::Subject_2:System.Void()\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::Subject_2:System.Void()\",\"methodId\":\"pinvoke-dllimport-subjects.pinvoke-dllimport-subjects.subject-2\",\"signature\":\"System.Void PinvokeDllimportSubjects::Subject_2()\"},\"nativeSymbol\":\"PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_2\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetProcessId:System.Int32()\",\"reference\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetProcessId:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetProcessId:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetProcessId\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetProcessId:System.Int32()\",\"reference\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetProcessId:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetProcessId:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetProcessId\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PinvokeDllimportSubjects\"}";
static const char kMethodJson_3[] = "{\"methodId\":\"pinvoke-dllimport-subjects.pinvoke-dllimport-subjects.subject-3\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::Subject_3:System.Void()\",\"signature\":\"System.Void PinvokeDllimportSubjects::Subject_3()\",\"identity\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"declaringTypeSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects\",\"definitionSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::Subject_3:System.Void()\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::Subject_3:System.Void()\",\"methodId\":\"pinvoke-dllimport-subjects.pinvoke-dllimport-subjects.subject-3\",\"signature\":\"System.Void PinvokeDllimportSubjects::Subject_3()\"},\"nativeSymbol\":\"PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_3\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetModuleHandle:System.Int32()\",\"reference\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetModuleHandle:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetModuleHandle:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetModuleHandle\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetModuleHandle:System.Int32()\",\"reference\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetModuleHandle:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetModuleHandle:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetModuleHandle\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PinvokeDllimportSubjects\"}";
static const char kMethodJson_4[] = "{\"methodId\":\"pinvoke-dllimport-subjects.pinvoke-dllimport-native-entry.get-current-process\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetCurrentProcess:System.IntPtr()\",\"signature\":\"System.IntPtr PinvokeDllimportNativeEntry::GetCurrentProcess()\",\"identity\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"declaringTypeSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry\",\"definitionSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetCurrentProcess:System.IntPtr()\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetCurrentProcess:System.IntPtr()\",\"methodId\":\"pinvoke-dllimport-subjects.pinvoke-dllimport-native-entry.get-current-process\",\"signature\":\"System.IntPtr PinvokeDllimportNativeEntry::GetCurrentProcess()\"},\"nativeSymbol\":\"PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetCurrentProcess\",\"isStatic\":true,\"returnType\":\"System.IntPtr\",\"returnAbi\":{\"carrierKindCode\":2,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[],\"isPInvoke\":true,\"isUnmanagedCallersOnly\":false,\"importModuleName\":\"kernel32\",\"importEntryPointName\":\"GetCurrentProcess\",\"importCallingConvention\":256,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"stringParameterIndices\":[],\"safeHandleParameterIndices\":[],\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PinvokeDllimportSubjects\"}";
static const char kMethodJson_5[] = "{\"methodId\":\"pinvoke-dllimport-subjects.pinvoke-dllimport-native-entry.get-current-thread\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetCurrentThread:System.IntPtr()\",\"signature\":\"System.IntPtr PinvokeDllimportNativeEntry::GetCurrentThread()\",\"identity\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"declaringTypeSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry\",\"definitionSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetCurrentThread:System.IntPtr()\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetCurrentThread:System.IntPtr()\",\"methodId\":\"pinvoke-dllimport-subjects.pinvoke-dllimport-native-entry.get-current-thread\",\"signature\":\"System.IntPtr PinvokeDllimportNativeEntry::GetCurrentThread()\"},\"nativeSymbol\":\"PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetCurrentThread\",\"isStatic\":true,\"returnType\":\"System.IntPtr\",\"returnAbi\":{\"carrierKindCode\":2,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[],\"isPInvoke\":true,\"isUnmanagedCallersOnly\":false,\"importModuleName\":\"kernel32\",\"importEntryPointName\":\"GetCurrentThread\",\"importCallingConvention\":256,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"stringParameterIndices\":[],\"safeHandleParameterIndices\":[],\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PinvokeDllimportSubjects\"}";
static const char kMethodJson_6[] = "{\"methodId\":\"pinvoke-dllimport-subjects.pinvoke-dllimport-native-entry.get-module-handle\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetModuleHandle:System.IntPtr(System.String)\",\"signature\":\"System.IntPtr PinvokeDllimportNativeEntry::GetModuleHandle(System.String)\",\"identity\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"declaringTypeSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry\",\"definitionSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetModuleHandle:System.IntPtr(System.String)\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetModuleHandle:System.IntPtr(System.String)\",\"methodId\":\"pinvoke-dllimport-subjects.pinvoke-dllimport-native-entry.get-module-handle\",\"signature\":\"System.IntPtr PinvokeDllimportNativeEntry::GetModuleHandle(System.String)\"},\"nativeSymbol\":\"PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetModuleHandle_System_String\",\"isStatic\":true,\"returnType\":\"System.IntPtr\",\"returnAbi\":{\"carrierKindCode\":2,\"typeShape\":2},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[],\"isPInvoke\":true,\"isUnmanagedCallersOnly\":false,\"importModuleName\":\"kernel32\",\"importEntryPointName\":\"GetModuleHandleW\",\"importCallingConvention\":256,\"importCharSet\":4,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"stringParameterIndices\":[0],\"safeHandleParameterIndices\":[],\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PinvokeDllimportSubjects\"}";
static const char kMethodJson_7[] = "{\"methodId\":\"pinvoke-dllimport-subjects.pinvoke-dllimport-native-entry.get-process-id\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetProcessId:System.UInt32(System.IntPtr)\",\"signature\":\"System.UInt32 PinvokeDllimportNativeEntry::GetProcessId(System.IntPtr)\",\"identity\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"declaringTypeSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry\",\"definitionSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetProcessId:System.UInt32(System.IntPtr)\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetProcessId:System.UInt32(System.IntPtr)\",\"methodId\":\"pinvoke-dllimport-subjects.pinvoke-dllimport-native-entry.get-process-id\",\"signature\":\"System.UInt32 PinvokeDllimportNativeEntry::GetProcessId(System.IntPtr)\"},\"nativeSymbol\":\"PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetProcessId_System_IntPtr\",\"isStatic\":true,\"returnType\":\"System.UInt32\",\"returnAbi\":{\"carrierKindCode\":2,\"typeShape\":2},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[],\"isPInvoke\":true,\"isUnmanagedCallersOnly\":false,\"importModuleName\":\"kernel32\",\"importEntryPointName\":\"GetProcessId\",\"importCallingConvention\":256,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"stringParameterIndices\":[],\"safeHandleParameterIndices\":[],\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PinvokeDllimportSubjects\"}";
static const char kMethodJson_8[] = "{\"methodId\":\"pinvoke-dllimport-subjects.pinvoke-dllimport-native-entry.test-get-current-process\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetCurrentProcess:System.Int32()\",\"signature\":\"System.Int32 PinvokeDllimportNativeEntry::TestGetCurrentProcess()\",\"identity\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"declaringTypeSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry\",\"definitionSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetCurrentProcess:System.Int32()\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetCurrentProcess:System.Int32()\",\"methodId\":\"pinvoke-dllimport-subjects.pinvoke-dllimport-native-entry.test-get-current-process\",\"signature\":\"System.Int32 PinvokeDllimportNativeEntry::TestGetCurrentProcess()\"},\"nativeSymbol\":\"PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentProcess\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.IntPtr\",\"callee\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetCurrentProcess:System.IntPtr()\",\"reference\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetCurrentProcess:System.IntPtr()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetCurrentProcess:System.IntPtr()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetCurrentProcess\",\"targetParameterCount\":0,\"targetReturnType\":\"System.IntPtr\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":-1,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i\",\"opCode\":65,\"ilOffset\":9,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"bne.un\",\"opCode\":79,\"operand\":15,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":16,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":19,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":20,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PinvokeDllimportSubjects\"}";
static const char kMethodJson_9[] = "{\"methodId\":\"pinvoke-dllimport-subjects.pinvoke-dllimport-native-entry.test-get-current-thread\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetCurrentThread:System.Int32()\",\"signature\":\"System.Int32 PinvokeDllimportNativeEntry::TestGetCurrentThread()\",\"identity\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"declaringTypeSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry\",\"definitionSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetCurrentThread:System.Int32()\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetCurrentThread:System.Int32()\",\"methodId\":\"pinvoke-dllimport-subjects.pinvoke-dllimport-native-entry.test-get-current-thread\",\"signature\":\"System.Int32 PinvokeDllimportNativeEntry::TestGetCurrentThread()\"},\"nativeSymbol\":\"PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentThread\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.IntPtr\",\"callee\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetCurrentThread:System.IntPtr()\",\"reference\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetCurrentThread:System.IntPtr()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetCurrentThread:System.IntPtr()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetCurrentThread\",\"targetParameterCount\":0,\"targetReturnType\":\"System.IntPtr\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":-2,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i\",\"opCode\":65,\"ilOffset\":10,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"bne.un\",\"opCode\":79,\"operand\":16,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":17,\"ilOffset\":14,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":20,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":20,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":21,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PinvokeDllimportSubjects\"}";
static const char kMethodJson_10[] = "{\"methodId\":\"pinvoke-dllimport-subjects.pinvoke-dllimport-native-entry.test-get-module-handle\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetModuleHandle:System.Int32()\",\"signature\":\"System.Int32 PinvokeDllimportNativeEntry::TestGetModuleHandle()\",\"identity\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"declaringTypeSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry\",\"definitionSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetModuleHandle:System.Int32()\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetModuleHandle:System.Int32()\",\"methodId\":\"pinvoke-dllimport-subjects.pinvoke-dllimport-native-entry.test-get-module-handle\",\"signature\":\"System.Int32 PinvokeDllimportNativeEntry::TestGetModuleHandle()\"},\"nativeSymbol\":\"PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetModuleHandle\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.IntPtr\",\"callee\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetModuleHandle:System.IntPtr(System.String)\",\"reference\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetModuleHandle:System.IntPtr(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetModuleHandle:System.IntPtr(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetModuleHandle_System_String\",\"targetParameterCount\":1,\"targetReturnType\":\"System.IntPtr\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":7,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":14,\"ilOffset\":9,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":15,\"ilOffset\":12,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":18,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":19,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PinvokeDllimportSubjects\"}";
static const char kMethodJson_11[] = "{\"methodId\":\"pinvoke-dllimport-subjects.pinvoke-dllimport-native-entry.test-get-process-id\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetProcessId:System.Int32()\",\"signature\":\"System.Int32 PinvokeDllimportNativeEntry::TestGetProcessId()\",\"identity\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"declaringTypeSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry\",\"definitionSubjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetProcessId:System.Int32()\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetProcessId:System.Int32()\",\"methodId\":\"pinvoke-dllimport-subjects.pinvoke-dllimport-native-entry.test-get-process-id\",\"signature\":\"System.Int32 PinvokeDllimportNativeEntry::TestGetProcessId()\"},\"nativeSymbol\":\"PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetProcessId\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":3,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.IntPtr\",\"callee\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetCurrentProcess:System.IntPtr()\",\"reference\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetCurrentProcess:System.IntPtr()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetCurrentProcess:System.IntPtr()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetCurrentProcess\",\"targetParameterCount\":0,\"targetReturnType\":\"System.IntPtr\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":8,\"resultType\":\"System.UInt32\",\"callee\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetProcessId:System.UInt32(System.IntPtr)\",\"reference\":{\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetProcessId:System.UInt32(System.IntPtr)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"PinvokeDllimportSubjects\",\"subjectId\":\"PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetProcessId:System.UInt32(System.IntPtr)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetProcessId_System_IntPtr\",\"targetParameterCount\":1,\"targetReturnType\":\"System.UInt32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":20,\"ilOffset\":15,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":21,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":24,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":25,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PinvokeDllimportSubjects\"}";

extern "C" const JitMethodEntry kChaosJitMethodEntries[12] =
{
    { kMethodJson_0, 5625u, 0x00000004u, 0u },
    { kMethodJson_1, 5617u, 0x00000005u, 0u },
    { kMethodJson_2, 5585u, 0x00000006u, 0u },
    { kMethodJson_3, 5609u, 0x00000007u, 0u },
    { kMethodJson_4, 1481u, 0x00000008u, 0u },
    { kMethodJson_5, 1472u, 0x0000000Au, 0u },
    { kMethodJson_6, 1634u, 0x0000000Eu, 0u },
    { kMethodJson_7, 1550u, 0x0000000Cu, 0u },
    { kMethodJson_8, 3325u, 0x00000009u, 0u },
    { kMethodJson_9, 3314u, 0x0000000Bu, 0u },
    { kMethodJson_10, 3250u, 0x0000000Fu, 0u },
    { kMethodJson_11, 4136u, 0x0000000Du, 0u }
};

extern "C" const uint32_t kChaosJitMethodEntryCount = 12u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitMethods(kChaosJitMethodEntries, kChaosJitMethodEntryCount);
}