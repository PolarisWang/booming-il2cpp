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

namespace chaos::il2cpp::codegen::ThreadingSyncSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


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
inline TypeInfoV0 chaos_mt_System_Threading_AutoResetEvent = {{nullptr, nullptr, 9111865296079522101ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Threading_AutoResetEvent = static_cast<CHAOS_IL2CPP_INTPTR>(9111865296079522101ULL);
inline TypeInfoV0 chaos_mt_System_Threading_ManualResetEvent = {{nullptr, nullptr, 8137036073600629176ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Threading_ManualResetEvent = static_cast<CHAOS_IL2CPP_INTPTR>(8137036073600629176ULL);
inline TypeInfoV0 chaos_mt_System_Threading_SemaphoreSlim = {{nullptr, nullptr, 10823106288603913950ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Threading_SemaphoreSlim = static_cast<CHAOS_IL2CPP_INTPTR>(10823106288603913950ULL);
inline TypeInfoV0 chaos_mt_System_Threading_SpinLock = {{nullptr, nullptr, 8630705894240840840ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Threading_SpinLock = static_cast<CHAOS_IL2CPP_INTPTR>(8630705894240840840ULL);
inline TypeInfoV0 chaos_mt_ThreadingSyncSubjects_ThreadingSyncSubjects = {{nullptr, nullptr, 4025799244050259528ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ThreadingSyncSubjects_ThreadingSyncSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(4025799244050259528ULL);

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

struct chaos_type_System_Threading_AutoResetEvent
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Threading_ManualResetEvent
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Threading_SemaphoreSlim
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Threading_SpinLock
{
	ThinLockableHeader header{};
};

struct chaos_type_ThreadingSyncSubjects_ThreadingSyncSubjects
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR field_ThreadingSyncSubjects_ThreadingSyncSubjects__s_slock = 0;
};

CHAOS_IL2CPP_INT32 chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects___exitCode = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects__s_are = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects__s_mre = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects__s_sem = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects__s_slock = 0;

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_0(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_1(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_2(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_3(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_4(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_5(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_6(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_7(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_8(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_9(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_10(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_11(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_12(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_13(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_14(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_15(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_16(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_cctor(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[19] = {
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
	::ChaosAbiMethodEntryV0 entries[18];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		18u,
		0u,
		400696213u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_0
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_1
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_2
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_3
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_4
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_5
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_6
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_7
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_8
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_9
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_10
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_11
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_12
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_13
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_14
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_15
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_16
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_cctor
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
		"ThreadingSyncSubjects",
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
		&chaos_mt_ThreadingSyncSubjects_ThreadingSyncSubjects.hot,
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
		/* .name_utf8         = */ "ThreadingSyncSubjects",
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
		::chaos::il2cpp::runtime_core::RegisterModule("ThreadingSyncSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[18] = {
	{ "Subject_0", 0x00000007u, 0u },  // ThreadingSyncSubjects
	{ "Subject_1", 0x00000008u, 0u },  // ThreadingSyncSubjects
	{ "CustomEntrySubject_2", 0x0000000Cu, 0u },  // ThreadingSyncSubjects
	{ "CustomEntrySubject_3", 0x0000000Du, 0u },  // ThreadingSyncSubjects
	{ "CustomEntrySubject_4", 0x0000000Eu, 0u },  // ThreadingSyncSubjects
	{ "Subject_5", 0x00000009u, 0u },  // ThreadingSyncSubjects
	{ "CustomEntrySubject_6", 0x0000000Fu, 0u },  // ThreadingSyncSubjects
	{ "CustomEntrySubject_7", 0x00000010u, 0u },  // ThreadingSyncSubjects
	{ "CustomEntrySubject_8", 0x00000011u, 0u },  // ThreadingSyncSubjects
	{ "Subject_9", 0x0000000Au, 0u },  // ThreadingSyncSubjects
	{ "CustomEntrySubject_10", 0x00000012u, 0u },  // ThreadingSyncSubjects
	{ "CustomEntrySubject_11", 0x00000013u, 0u },  // ThreadingSyncSubjects
	{ "CustomEntrySubject_12", 0x00000014u, 0u },  // ThreadingSyncSubjects
	{ "Subject_13", 0x0000000Bu, 0u },  // ThreadingSyncSubjects
	{ "CustomEntrySubject_14", 0x00000015u, 0u },  // ThreadingSyncSubjects
	{ "CustomEntrySubject_15", 0x00000016u, 0u },  // ThreadingSyncSubjects
	{ "CustomEntrySubject_16", 0x00000017u, 0u },  // ThreadingSyncSubjects
	{ ".cctor", 0x00000018u, 0u },  // ThreadingSyncSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "ThreadingSyncSubjects", "", 0u, 18u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[18] = {
	{ 0x00000007u, 0u },
	{ 0x00000008u, 1u },
	{ 0x00000009u, 5u },
	{ 0x0000000Au, 9u },
	{ 0x0000000Bu, 13u },
	{ 0x0000000Cu, 2u },
	{ 0x0000000Du, 3u },
	{ 0x0000000Eu, 4u },
	{ 0x0000000Fu, 6u },
	{ 0x00000010u, 7u },
	{ 0x00000011u, 8u },
	{ 0x00000012u, 10u },
	{ 0x00000013u, 11u },
	{ 0x00000014u, 12u },
	{ 0x00000015u, 14u },
	{ 0x00000016u, 15u },
	{ 0x00000017u, 16u },
	{ 0x00000018u, 17u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[18] = {
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingSyncSubjects::Subject_0
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingSyncSubjects::Subject_1
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingSyncSubjects::CustomEntrySubject_2
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingSyncSubjects::CustomEntrySubject_3
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingSyncSubjects::CustomEntrySubject_4
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingSyncSubjects::Subject_5
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingSyncSubjects::CustomEntrySubject_6
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingSyncSubjects::CustomEntrySubject_7
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingSyncSubjects::CustomEntrySubject_8
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingSyncSubjects::Subject_9
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingSyncSubjects::CustomEntrySubject_10
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingSyncSubjects::CustomEntrySubject_11
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingSyncSubjects::CustomEntrySubject_12
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingSyncSubjects::Subject_13
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_14), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingSyncSubjects::CustomEntrySubject_14
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_15), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingSyncSubjects::CustomEntrySubject_15
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_16), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingSyncSubjects::CustomEntrySubject_16
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_cctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingSyncSubjects::.cctor
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"ThreadingSyncSubjects",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	18u,
	s_hotpatch_slots,
	18u,
	s_hotpatch_entries,
	18u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[20] = {
	"System.Threading/SemaphoreSlim::.ctor:System.Void(System.Int32)",
	"System.Threading/SemaphoreSlim::.ctor:System.Void(System.Int32,System.Int32)",
	"ThreadingSyncSubjects/ThreadingSyncSubjects::s_sem",
	"System.Threading/SemaphoreSlim::Wait:System.Void()",
	"System.Threading/SemaphoreSlim::Release:System.Int32()",
	"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode",
	"System.Threading/SemaphoreSlim::get_CurrentCount:System.Int32()",
	"System.Threading/ManualResetEvent::.ctor:System.Void(System.Boolean)",
	"ThreadingSyncSubjects/ThreadingSyncSubjects::s_mre",
	"System.Threading/EventWaitHandle::Reset:System.Boolean()",
	"System.Threading/EventWaitHandle::Set:System.Boolean()",
	"System.Private.CoreLib/System.Threading.WaitHandle::WaitOne:System.Boolean(System.Int32)",
	"System.Threading/AutoResetEvent::.ctor:System.Void(System.Boolean)",
	"ThreadingSyncSubjects/ThreadingSyncSubjects::s_are",
	"System.Threading/SpinLock::.ctor:System.Void(System.Boolean)",
	"ThreadingSyncSubjects/ThreadingSyncSubjects::s_slock",
	"System.Threading/SpinLock::Enter:System.Void(System.Boolean&)",
	"System.Threading/SpinLock::Exit:System.Void()",
	"System.Threading/SpinWait",
	"System.Threading/SpinWait::SpinOnce:System.Void()",
};

extern "C" void* kChaosExternalRuntimeFnTable[20] = {
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

extern "C" int32_t kChaosExternalRuntimeCount = 20;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[18])() = {
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_2),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_3),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_4),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_6),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_7),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_8),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_9),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_10),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_11),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_12),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_13),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_14),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_15),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_16),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_cctor),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[18])() = {
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
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 17;
extern "C" const int kSubjectEntryIndices[17] = {
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

	/* ── Entry 0: ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry0;
	/* ── Entry 1: ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry1;
	/* ── Entry 2: ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry2;
	/* ── Entry 3: ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry3;
	/* ── Entry 4: ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry4;
	/* ── Entry 5: ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry5;
	/* ── Entry 6: ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_6 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry6;
	/* ── Entry 7: ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_7 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry7;
	/* ── Entry 8: ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_8 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry8;
	/* ── Entry 9: ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_9 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry9;
	/* ── Entry 10: ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_10 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry10;
	/* ── Entry 11: ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_11 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry11;
	/* ── Entry 12: ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_12 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry12;
	/* ── Entry 13: ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_13 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry13;
	/* ── Entry 14: ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_14 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry14;
	/* ── Entry 15: ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_15 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry15;
	/* ── Entry 16: ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_16 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry16;
	/* ── Entry 17: ThreadingSyncSubjects_ThreadingSyncSubjects_cctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry17;
} kChaosGcSlotMapsSection = {
	/* entry0 = ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_0 */
	.entry0 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_0),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry1 = ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_1 */
	.entry1 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_1),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry2 = ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_2 */
	.entry2 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_2),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry3 = ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_3 */
	.entry3 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_3),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry4 = ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_4 */
	.entry4 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_4),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry5 = ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_5 */
	.entry5 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_5),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry6 = ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_6 */
	.entry6 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_6),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry7 = ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_7 */
	.entry7 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_7),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry8 = ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_8 */
	.entry8 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_8),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry9 = ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_9 */
	.entry9 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_9),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry10 = ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_10 */
	.entry10 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_10),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry11 = ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_11 */
	.entry11 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_11),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry12 = ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_12 */
	.entry12 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_12),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry13 = ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_13 */
	.entry13 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_13),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry14 = ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_14 */
	.entry14 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_14),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry15 = ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_15 */
	.entry15 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_15),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry16 = ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_16 */
	.entry16 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_16),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry17 = ThreadingSyncSubjects_ThreadingSyncSubjects_cctor */
	.entry17 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_cctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 612u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[18] = {
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_0),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_1),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_2),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_3),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_4),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_5),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_6),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_7),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_8),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_9),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_10),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_11),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_12),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_13),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_14),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_15),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_16),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_cctor),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 18u,
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
	.image_name_utf8    = "ThreadingSyncSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_ThreadingSyncSubjects_ThreadingSyncSubjects[5] = {
	{ 0u, "ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
	{ 0u, "ThreadingSyncSubjects/ThreadingSyncSubjects::s_sem", "s_sem", "System.Int32", 0LL },
	{ 0u, "ThreadingSyncSubjects/ThreadingSyncSubjects::s_mre", "s_mre", "System.Int32", 0LL },
	{ 0u, "ThreadingSyncSubjects/ThreadingSyncSubjects::s_are", "s_are", "System.Int32", 0LL },
	{ 0u, "ThreadingSyncSubjects/ThreadingSyncSubjects::s_slock", "s_slock", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_ThreadingSyncSubjects_ThreadingSyncSubjects[18] = {
	{ 0u, "ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_2:System.Void()", "CustomEntrySubject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_3:System.Void()", "CustomEntrySubject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_4:System.Void()", "CustomEntrySubject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_6:System.Void()", "CustomEntrySubject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_7:System.Void()", "CustomEntrySubject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_8:System.Void()", "CustomEntrySubject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_9:System.Void()", "Subject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_10:System.Void()", "CustomEntrySubject_10", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_11:System.Void()", "CustomEntrySubject_11", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_12:System.Void()", "CustomEntrySubject_12", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_13:System.Void()", "Subject_13", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_14:System.Void()", "CustomEntrySubject_14", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_15:System.Void()", "CustomEntrySubject_15", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_16:System.Void()", "CustomEntrySubject_16", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingSyncSubjects/ThreadingSyncSubjects::.cctor:System.Void()", ".cctor", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0u, "ThreadingSyncSubjects/ThreadingSyncSubjects", "ThreadingSyncSubjects/ThreadingSyncSubjects", "", "ThreadingSyncSubjects", "ThreadingSyncSubjects", nullptr, kReflFields_ThreadingSyncSubjects_ThreadingSyncSubjects, 5u, nullptr, 0u,
	kReflMethods_ThreadingSyncSubjects_ThreadingSyncSubjects, 18u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "ThreadingSyncSubjects", kReflTypePtrs, 1u };

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
// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_0()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Threading_SemaphoreSlim, {});
		chaos_object->header.type_info = &chaos_mt_System_Threading_SemaphoreSlim.hot;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	return;
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_1()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Threading_SemaphoreSlim, {});
		chaos_object->header.type_info = &chaos_mt_System_Threading_SemaphoreSlim.hot;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	return;
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_2()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects__s_sem;
	{
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[3])();
	}
	_s1 = chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects__s_sem;
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[4])();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_3()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_3(void)
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


	_s0 = chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects__s_sem;
	{
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[3])();
	}
	_s1 = chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects__s_sem;
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[4])();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s2) < static_cast<CHAOS_IL2CPP_INT32>(_s3) ? 1 : 0);
	{
		if (_s2 != 0)
		{
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s2;
				chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_4()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_4(void)
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


	_s0 = chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects__s_sem;
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[6])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1) < static_cast<CHAOS_IL2CPP_INT32>(_s2) ? 1 : 0);
	{
		if (_s1 != 0)
		{
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_5()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Threading_ManualResetEvent, {});
		chaos_object->header.type_info = &chaos_mt_System_Threading_ManualResetEvent.hot;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	return;
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_6()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_6(void)
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


	_s0 = chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects__s_mre;
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[9])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects__s_mre;
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[10])();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
	{
		if (_s2 != 0)
		{
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s2;
				chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_7()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_7(void)
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


	_s0 = chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects__s_mre;
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[10])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects__s_mre;
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[9])();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
	{
		if (_s2 != 0)
		{
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s2;
				chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_8()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_8(void)
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


	_s0 = chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects__s_mre;
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[10])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects__s_mre;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_arg_0 = _s2;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[11])(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
	{
		if (_s2 != 0)
		{
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s2;
				chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_9()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_9(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Threading_AutoResetEvent, {});
		chaos_object->header.type_info = &chaos_mt_System_Threading_AutoResetEvent.hot;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	return;
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_10()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_10(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	_s0 = chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects__s_are;
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[10])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
	{
		if (_s1 != 0)
		{
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_11()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_11(void)
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


	_s0 = chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects__s_are;
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[10])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects__s_are;
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[9])();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
	{
		if (_s2 != 0)
		{
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s2;
				chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_12()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_12(void)
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


	_s0 = chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects__s_are;
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[10])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects__s_are;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_arg_0 = _s2;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[11])(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
	{
		if (_s2 != 0)
		{
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s2;
				chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_13()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_13(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Threading_SpinLock, {});
		chaos_object->header.type_info = &chaos_mt_System_Threading_SpinLock.hot;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	return;
}

// AOT-unreachable stub: ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_14:System.Void()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_14(void)
{
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_15()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_15(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[0] = _s0;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[14])(chaos_arg_0);
	}
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_arg_0 = _s2;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[16])(chaos_arg_0);
	}
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[17])();
	}
	return;
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_16()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_16(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
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
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[19])();
	}
	return;
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::.cctor()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_cctor(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Threading_SemaphoreSlim, {});
		chaos_object->header.type_info = &chaos_mt_System_Threading_SemaphoreSlim.hot;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto chaos_value = _s0;
		chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects__s_sem = chaos_value;
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Threading_ManualResetEvent, {});
		chaos_object->header.type_info = &chaos_mt_System_Threading_ManualResetEvent.hot;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto chaos_value = _s0;
		chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects__s_mre = chaos_value;
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Threading_AutoResetEvent, {});
		chaos_object->header.type_info = &chaos_mt_System_Threading_AutoResetEvent.hot;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto chaos_value = _s0;
		chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects__s_are = chaos_value;
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Threading_SpinLock, {});
		chaos_object->header.type_info = &chaos_mt_System_Threading_SpinLock.hot;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto chaos_value = _s0;
		chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects__s_slock = chaos_value;
	}
	return;
}



}  // namespace chaos::il2cpp::codegen::ThreadingSyncSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 18;

// ── JIT Method Entry Table ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kMethodJson_0[] = "{\"methodId\":\"threading-sync-subjects.threading-sync-subjects.subject-0\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_0:System.Void()\",\"signature\":\"System.Void ThreadingSyncSubjects::Subject_0()\",\"identity\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"definitionSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_0:System.Void()\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_0:System.Void()\",\"methodId\":\"threading-sync-subjects.threading-sync-subjects.subject-0\",\"signature\":\"System.Void ThreadingSyncSubjects::Subject_0()\"},\"nativeSymbol\":\"ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":3,\"resultType\":\"System.Threading/SemaphoreSlim\",\"callee\":\"System.Threading/SemaphoreSlim::.ctor:System.Void(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/SemaphoreSlim::.ctor:System.Void(System.Int32)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/SemaphoreSlim\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingSyncSubjects\"}";
static const char kMethodJson_1[] = "{\"methodId\":\"threading-sync-subjects.threading-sync-subjects.subject-1\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_1:System.Void()\",\"signature\":\"System.Void ThreadingSyncSubjects::Subject_1()\",\"identity\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"definitionSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_1:System.Void()\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_1:System.Void()\",\"methodId\":\"threading-sync-subjects.threading-sync-subjects.subject-1\",\"signature\":\"System.Void ThreadingSyncSubjects::Subject_1()\"},\"nativeSymbol\":\"ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":5,\"resultType\":\"System.Threading/SemaphoreSlim\",\"callee\":\"System.Threading/SemaphoreSlim::.ctor:System.Void(System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/SemaphoreSlim::.ctor:System.Void(System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/SemaphoreSlim\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":10,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":11,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingSyncSubjects\"}";
static const char kMethodJson_2[] = "{\"methodId\":\"threading-sync-subjects.threading-sync-subjects.custom-entry-subject-2\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_2:System.Void()\",\"signature\":\"System.Void ThreadingSyncSubjects::CustomEntrySubject_2()\",\"identity\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"definitionSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_2:System.Void()\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_2:System.Void()\",\"methodId\":\"threading-sync-subjects.threading-sync-subjects.custom-entry-subject-2\",\"signature\":\"System.Void ThreadingSyncSubjects::CustomEntrySubject_2()\"},\"nativeSymbol\":\"ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_2\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_sem\",\"ilOffset\":1,\"resultType\":\"SemaphoreSlim\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_sem\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_sem\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"SemaphoreSlim\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":6,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/SemaphoreSlim::Wait:System.Void()\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/SemaphoreSlim::Wait:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/SemaphoreSlim::Wait:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_sem\",\"ilOffset\":12,\"resultType\":\"SemaphoreSlim\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_sem\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_sem\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"SemaphoreSlim\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"callee\":\"System.Threading/SemaphoreSlim::Release:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/SemaphoreSlim::Release:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/SemaphoreSlim::Release:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":22,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingSyncSubjects\"}";
static const char kMethodJson_3[] = "{\"methodId\":\"threading-sync-subjects.threading-sync-subjects.custom-entry-subject-3\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_3:System.Void()\",\"signature\":\"System.Void ThreadingSyncSubjects::CustomEntrySubject_3()\",\"identity\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"definitionSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_3:System.Void()\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_3:System.Void()\",\"methodId\":\"threading-sync-subjects.threading-sync-subjects.custom-entry-subject-3\",\"signature\":\"System.Void ThreadingSyncSubjects::CustomEntrySubject_3()\"},\"nativeSymbol\":\"ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_3\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_sem\",\"ilOffset\":1,\"resultType\":\"SemaphoreSlim\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_sem\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_sem\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"SemaphoreSlim\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":6,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/SemaphoreSlim::Wait:System.Void()\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/SemaphoreSlim::Wait:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/SemaphoreSlim::Wait:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_sem\",\"ilOffset\":12,\"resultType\":\"SemaphoreSlim\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_sem\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_sem\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"SemaphoreSlim\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"callee\":\"System.Threading/SemaphoreSlim::Release:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/SemaphoreSlim::Release:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/SemaphoreSlim::Release:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":23,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"clt\",\"opCode\":32,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":37,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\",\"ilOffset\":32,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":37,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingSyncSubjects\"}";
static const char kMethodJson_4[] = "{\"methodId\":\"threading-sync-subjects.threading-sync-subjects.custom-entry-subject-4\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_4:System.Void()\",\"signature\":\"System.Void ThreadingSyncSubjects::CustomEntrySubject_4()\",\"identity\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"definitionSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_4:System.Void()\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_4:System.Void()\",\"methodId\":\"threading-sync-subjects.threading-sync-subjects.custom-entry-subject-4\",\"signature\":\"System.Void ThreadingSyncSubjects::CustomEntrySubject_4()\"},\"nativeSymbol\":\"ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_4\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_sem\",\"ilOffset\":1,\"resultType\":\"SemaphoreSlim\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_sem\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_sem\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"SemaphoreSlim\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":6,\"resultType\":\"System.Int32\",\"callee\":\"System.Threading/SemaphoreSlim::get_CurrentCount:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/SemaphoreSlim::get_CurrentCount:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/SemaphoreSlim::get_CurrentCount:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":12,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"clt\",\"opCode\":32,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":17,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":26,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\",\"ilOffset\":21,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":26,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingSyncSubjects\"}";
static const char kMethodJson_5[] = "{\"methodId\":\"threading-sync-subjects.threading-sync-subjects.subject-5\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_5:System.Void()\",\"signature\":\"System.Void ThreadingSyncSubjects::Subject_5()\",\"identity\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"definitionSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_5:System.Void()\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_5:System.Void()\",\"methodId\":\"threading-sync-subjects.threading-sync-subjects.subject-5\",\"signature\":\"System.Void ThreadingSyncSubjects::Subject_5()\"},\"nativeSymbol\":\"ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_5\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":2,\"resultType\":\"System.Threading/ManualResetEvent\",\"callee\":\"System.Threading/ManualResetEvent::.ctor:System.Void(System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/ManualResetEvent::.ctor:System.Void(System.Boolean)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/ManualResetEvent\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingSyncSubjects\"}";
static const char kMethodJson_6[] = "{\"methodId\":\"threading-sync-subjects.threading-sync-subjects.custom-entry-subject-6\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_6:System.Void()\",\"signature\":\"System.Void ThreadingSyncSubjects::CustomEntrySubject_6()\",\"identity\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"definitionSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_6:System.Void()\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_6:System.Void()\",\"methodId\":\"threading-sync-subjects.threading-sync-subjects.custom-entry-subject-6\",\"signature\":\"System.Void ThreadingSyncSubjects::CustomEntrySubject_6()\"},\"nativeSymbol\":\"ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_6\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_mre\",\"ilOffset\":1,\"resultType\":\"ManualResetEvent\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_mre\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_mre\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"ManualResetEvent\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":6,\"resultType\":\"System.Boolean\",\"callee\":\"System.Threading/EventWaitHandle::Reset:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/EventWaitHandle::Reset:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/EventWaitHandle::Reset:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":11,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_mre\",\"ilOffset\":12,\"resultType\":\"ManualResetEvent\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_mre\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_mre\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"ManualResetEvent\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":17,\"resultType\":\"System.Boolean\",\"callee\":\"System.Threading/EventWaitHandle::Set:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/EventWaitHandle::Set:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/EventWaitHandle::Set:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":35,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\",\"ilOffset\":30,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":35,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingSyncSubjects\"}";
static const char kMethodJson_7[] = "{\"methodId\":\"threading-sync-subjects.threading-sync-subjects.custom-entry-subject-7\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_7:System.Void()\",\"signature\":\"System.Void ThreadingSyncSubjects::CustomEntrySubject_7()\",\"identity\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"definitionSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_7:System.Void()\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_7:System.Void()\",\"methodId\":\"threading-sync-subjects.threading-sync-subjects.custom-entry-subject-7\",\"signature\":\"System.Void ThreadingSyncSubjects::CustomEntrySubject_7()\"},\"nativeSymbol\":\"ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_7\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_mre\",\"ilOffset\":1,\"resultType\":\"ManualResetEvent\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_mre\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_mre\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"ManualResetEvent\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":6,\"resultType\":\"System.Boolean\",\"callee\":\"System.Threading/EventWaitHandle::Set:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/EventWaitHandle::Set:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/EventWaitHandle::Set:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":11,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_mre\",\"ilOffset\":12,\"resultType\":\"ManualResetEvent\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_mre\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_mre\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"ManualResetEvent\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":17,\"resultType\":\"System.Boolean\",\"callee\":\"System.Threading/EventWaitHandle::Reset:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/EventWaitHandle::Reset:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/EventWaitHandle::Reset:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":35,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\",\"ilOffset\":30,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":35,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingSyncSubjects\"}";
static const char kMethodJson_8[] = "{\"methodId\":\"threading-sync-subjects.threading-sync-subjects.custom-entry-subject-8\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_8:System.Void()\",\"signature\":\"System.Void ThreadingSyncSubjects::CustomEntrySubject_8()\",\"identity\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"definitionSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_8:System.Void()\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_8:System.Void()\",\"methodId\":\"threading-sync-subjects.threading-sync-subjects.custom-entry-subject-8\",\"signature\":\"System.Void ThreadingSyncSubjects::CustomEntrySubject_8()\"},\"nativeSymbol\":\"ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_8\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_mre\",\"ilOffset\":1,\"resultType\":\"ManualResetEvent\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_mre\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_mre\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"ManualResetEvent\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":6,\"resultType\":\"System.Boolean\",\"callee\":\"System.Threading/EventWaitHandle::Set:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/EventWaitHandle::Set:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/EventWaitHandle::Set:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":11,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_mre\",\"ilOffset\":12,\"resultType\":\"ManualResetEvent\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_mre\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_mre\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"ManualResetEvent\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":18,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Threading.WaitHandle::WaitOne:System.Boolean(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Threading.WaitHandle::WaitOne:System.Boolean(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.WaitHandle::WaitOne:System.Boolean(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":26,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":27,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":36,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":30,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\",\"ilOffset\":31,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":36,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingSyncSubjects\"}";
static const char kMethodJson_9[] = "{\"methodId\":\"threading-sync-subjects.threading-sync-subjects.subject-9\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_9:System.Void()\",\"signature\":\"System.Void ThreadingSyncSubjects::Subject_9()\",\"identity\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"definitionSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_9:System.Void()\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_9:System.Void()\",\"methodId\":\"threading-sync-subjects.threading-sync-subjects.subject-9\",\"signature\":\"System.Void ThreadingSyncSubjects::Subject_9()\"},\"nativeSymbol\":\"ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_9\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":2,\"resultType\":\"System.Threading/AutoResetEvent\",\"callee\":\"System.Threading/AutoResetEvent::.ctor:System.Void(System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/AutoResetEvent::.ctor:System.Void(System.Boolean)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/AutoResetEvent\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingSyncSubjects\"}";
static const char kMethodJson_10[] = "{\"methodId\":\"threading-sync-subjects.threading-sync-subjects.custom-entry-subject-10\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_10:System.Void()\",\"signature\":\"System.Void ThreadingSyncSubjects::CustomEntrySubject_10()\",\"identity\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"definitionSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_10:System.Void()\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_10:System.Void()\",\"methodId\":\"threading-sync-subjects.threading-sync-subjects.custom-entry-subject-10\",\"signature\":\"System.Void ThreadingSyncSubjects::CustomEntrySubject_10()\"},\"nativeSymbol\":\"ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_10\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_are\",\"ilOffset\":1,\"resultType\":\"AutoResetEvent\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_are\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_are\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"AutoResetEvent\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":6,\"resultType\":\"System.Boolean\",\"callee\":\"System.Threading/EventWaitHandle::Set:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/EventWaitHandle::Set:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/EventWaitHandle::Set:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":15,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":24,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\",\"ilOffset\":19,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingSyncSubjects\"}";
static const char kMethodJson_11[] = "{\"methodId\":\"threading-sync-subjects.threading-sync-subjects.custom-entry-subject-11\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_11:System.Void()\",\"signature\":\"System.Void ThreadingSyncSubjects::CustomEntrySubject_11()\",\"identity\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"definitionSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_11:System.Void()\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_11:System.Void()\",\"methodId\":\"threading-sync-subjects.threading-sync-subjects.custom-entry-subject-11\",\"signature\":\"System.Void ThreadingSyncSubjects::CustomEntrySubject_11()\"},\"nativeSymbol\":\"ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_11\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_are\",\"ilOffset\":1,\"resultType\":\"AutoResetEvent\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_are\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_are\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"AutoResetEvent\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":6,\"resultType\":\"System.Boolean\",\"callee\":\"System.Threading/EventWaitHandle::Set:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/EventWaitHandle::Set:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/EventWaitHandle::Set:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":11,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_are\",\"ilOffset\":12,\"resultType\":\"AutoResetEvent\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_are\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_are\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"AutoResetEvent\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":17,\"resultType\":\"System.Boolean\",\"callee\":\"System.Threading/EventWaitHandle::Reset:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/EventWaitHandle::Reset:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/EventWaitHandle::Reset:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":35,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\",\"ilOffset\":30,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":35,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingSyncSubjects\"}";
static const char kMethodJson_12[] = "{\"methodId\":\"threading-sync-subjects.threading-sync-subjects.custom-entry-subject-12\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_12:System.Void()\",\"signature\":\"System.Void ThreadingSyncSubjects::CustomEntrySubject_12()\",\"identity\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"definitionSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_12:System.Void()\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_12:System.Void()\",\"methodId\":\"threading-sync-subjects.threading-sync-subjects.custom-entry-subject-12\",\"signature\":\"System.Void ThreadingSyncSubjects::CustomEntrySubject_12()\"},\"nativeSymbol\":\"ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_12\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_are\",\"ilOffset\":1,\"resultType\":\"AutoResetEvent\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_are\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_are\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"AutoResetEvent\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":6,\"resultType\":\"System.Boolean\",\"callee\":\"System.Threading/EventWaitHandle::Set:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/EventWaitHandle::Set:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/EventWaitHandle::Set:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":11,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_are\",\"ilOffset\":12,\"resultType\":\"AutoResetEvent\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_are\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_are\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"AutoResetEvent\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":18,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Threading.WaitHandle::WaitOne:System.Boolean(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Threading.WaitHandle::WaitOne:System.Boolean(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.WaitHandle::WaitOne:System.Boolean(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":26,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":27,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":36,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":30,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\",\"ilOffset\":31,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":36,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingSyncSubjects\"}";
static const char kMethodJson_13[] = "{\"methodId\":\"threading-sync-subjects.threading-sync-subjects.subject-13\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_13:System.Void()\",\"signature\":\"System.Void ThreadingSyncSubjects::Subject_13()\",\"identity\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"definitionSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_13:System.Void()\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_13:System.Void()\",\"methodId\":\"threading-sync-subjects.threading-sync-subjects.subject-13\",\"signature\":\"System.Void ThreadingSyncSubjects::Subject_13()\"},\"nativeSymbol\":\"ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_13\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":2,\"resultType\":\"System.Threading/SpinLock\",\"callee\":\"System.Threading/SpinLock::.ctor:System.Void(System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/SpinLock::.ctor:System.Void(System.Boolean)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/SpinLock\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingSyncSubjects\"}";
static const char kMethodJson_14[] = "{\"methodId\":\"threading-sync-subjects.threading-sync-subjects.custom-entry-subject-14\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_14:System.Void()\",\"signature\":\"System.Void ThreadingSyncSubjects::CustomEntrySubject_14()\",\"identity\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"definitionSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_14:System.Void()\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_14:System.Void()\",\"methodId\":\"threading-sync-subjects.threading-sync-subjects.custom-entry-subject-14\",\"signature\":\"System.Void ThreadingSyncSubjects::CustomEntrySubject_14()\"},\"nativeSymbol\":\"ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_14\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldsflda\",\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_slock\",\"ilOffset\":3,\"resultType\":\"System.IntPtr\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_slock\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_slock\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"SpinLock\"},\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":10,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/SpinLock::Enter:System.Void(System.Boolean\\u0026)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/SpinLock::Enter:System.Void(System.Boolean\\u0026)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/SpinLock::Enter:System.Void(System.Boolean\\u0026)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":16,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":21,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":30,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\",\"ilOffset\":25,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ldsflda\",\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_slock\",\"ilOffset\":30,\"resultType\":\"System.IntPtr\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_slock\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_slock\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"SpinLock\"},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":35,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/SpinLock::Exit:System.Void()\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/SpinLock::Exit:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/SpinLock::Exit:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingSyncSubjects\"}";
static const char kMethodJson_15[] = "{\"methodId\":\"threading-sync-subjects.threading-sync-subjects.custom-entry-subject-15\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_15:System.Void()\",\"signature\":\"System.Void ThreadingSyncSubjects::CustomEntrySubject_15()\",\"identity\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"definitionSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_15:System.Void()\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_15:System.Void()\",\"methodId\":\"threading-sync-subjects.threading-sync-subjects.custom-entry-subject-15\",\"signature\":\"System.Void ThreadingSyncSubjects::CustomEntrySubject_15()\"},\"nativeSymbol\":\"ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_15\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":3,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":5,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/SpinLock::.ctor:System.Void(System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/SpinLock::.ctor:System.Void(System.Boolean)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/SpinLock::.ctor:System.Void(System.Boolean)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":11,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":13,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":15,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/SpinLock::Enter:System.Void(System.Boolean\\u0026)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/SpinLock::Enter:System.Void(System.Boolean\\u0026)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/SpinLock::Enter:System.Void(System.Boolean\\u0026)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":23,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/SpinLock::Exit:System.Void()\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/SpinLock::Exit:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/SpinLock::Exit:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":29,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingSyncSubjects\"}";
static const char kMethodJson_16[] = "{\"methodId\":\"threading-sync-subjects.threading-sync-subjects.custom-entry-subject-16\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_16:System.Void()\",\"signature\":\"System.Void ThreadingSyncSubjects::CustomEntrySubject_16()\",\"identity\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"definitionSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_16:System.Void()\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_16:System.Void()\",\"methodId\":\"threading-sync-subjects.threading-sync-subjects.custom-entry-subject-16\",\"signature\":\"System.Void ThreadingSyncSubjects::CustomEntrySubject_16()\"},\"nativeSymbol\":\"ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_16\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Threading/SpinWait\",\"ilOffset\":3,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"type\",\"subjectId\":\"System.Threading/SpinWait\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/SpinWait\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":9,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/SpinWait::SpinOnce:System.Void()\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/SpinWait::SpinOnce:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/SpinWait::SpinOnce:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":17,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingSyncSubjects\"}";
static const char kMethodJson_17[] = "{\"methodId\":\"threading-sync-subjects.threading-sync-subjects.cctor\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::.cctor:System.Void()\",\"signature\":\"System.Void ThreadingSyncSubjects::.cctor()\",\"identity\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"definitionSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::.cctor:System.Void()\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::.cctor:System.Void()\",\"methodId\":\"threading-sync-subjects.threading-sync-subjects.cctor\",\"signature\":\"System.Void ThreadingSyncSubjects::.cctor()\"},\"nativeSymbol\":\"ThreadingSyncSubjects_ThreadingSyncSubjects_cctor\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":2,\"resultType\":\"System.Threading/SemaphoreSlim\",\"callee\":\"System.Threading/SemaphoreSlim::.ctor:System.Void(System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/SemaphoreSlim::.ctor:System.Void(System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/SemaphoreSlim\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_sem\",\"ilOffset\":7,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_sem\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_sem\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"SemaphoreSlim\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":13,\"resultType\":\"System.Threading/ManualResetEvent\",\"callee\":\"System.Threading/ManualResetEvent::.ctor:System.Void(System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/ManualResetEvent::.ctor:System.Void(System.Boolean)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/ManualResetEvent\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_mre\",\"ilOffset\":18,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_mre\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_mre\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"ManualResetEvent\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":24,\"resultType\":\"System.Threading/AutoResetEvent\",\"callee\":\"System.Threading/AutoResetEvent::.ctor:System.Void(System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/AutoResetEvent::.ctor:System.Void(System.Boolean)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/AutoResetEvent\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_are\",\"ilOffset\":29,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_are\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_are\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"AutoResetEvent\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":34,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":35,\"resultType\":\"System.Threading/SpinLock\",\"callee\":\"System.Threading/SpinLock::.ctor:System.Void(System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/SpinLock::.ctor:System.Void(System.Boolean)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/SpinLock\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_slock\",\"ilOffset\":40,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_slock\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingSyncSubjects\",\"subjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects::s_slock\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingSyncSubjects/ThreadingSyncSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"SpinLock\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":45,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingSyncSubjects\"}";

extern "C" const JitMethodEntry kChaosJitMethodEntries[18] =
{
    { kMethodJson_0, 1995u, 0x00000007u, 0u },
    { kMethodJson_1, 2126u, 0x00000008u, 0u },
    { kMethodJson_2, 3908u, 0x0000000Cu, 0u },
    { kMethodJson_3, 5291u, 0x0000000Du, 0u },
    { kMethodJson_4, 4068u, 0x0000000Eu, 0u },
    { kMethodJson_5, 2010u, 0x00000009u, 0u },
    { kMethodJson_6, 5212u, 0x0000000Fu, 0u },
    { kMethodJson_7, 5212u, 0x00000010u, 0u },
    { kMethodJson_8, 5423u, 0x00000011u, 0u },
    { kMethodJson_9, 2002u, 0x0000000Au, 0u },
    { kMethodJson_10, 3878u, 0x00000012u, 0u },
    { kMethodJson_11, 5212u, 0x00000013u, 0u },
    { kMethodJson_12, 5423u, 0x00000014u, 0u },
    { kMethodJson_13, 1986u, 0x0000000Bu, 0u },
    { kMethodJson_14, 5451u, 0x00000015u, 0u },
    { kMethodJson_15, 3812u, 0x00000016u, 0u },
    { kMethodJson_16, 2600u, 0x00000017u, 0u },
    { kMethodJson_17, 6859u, 0x00000018u, 0u }
};

extern "C" const uint32_t kChaosJitMethodEntryCount = 18u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitMethods(kChaosJitMethodEntries, kChaosJitMethodEntryCount);
}