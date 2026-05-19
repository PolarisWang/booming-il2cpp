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

namespace chaos::il2cpp::codegen::ThreadingMonitorInterlockedSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {{nullptr, nullptr, 5474029880995115448ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly = static_cast<CHAOS_IL2CPP_INTPTR>(5474029880995115448ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName = {{nullptr, nullptr, 17082367815459723707ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = static_cast<CHAOS_IL2CPP_INTPTR>(17082367815459723707ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo = {{nullptr, nullptr, 4137207361503509124ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo = static_cast<CHAOS_IL2CPP_INTPTR>(4137207361503509124ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo = {{nullptr, nullptr, 17040031516751226236ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo = static_cast<CHAOS_IL2CPP_INTPTR>(17040031516751226236ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo = {{nullptr, nullptr, 10748947813473285525ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = static_cast<CHAOS_IL2CPP_INTPTR>(10748947813473285525ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_String = {{nullptr, nullptr, 1782325859292956794ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_INTPTR>(1782325859292956794ULL);inline TypeInfoV0 chaos_mt_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects = {{nullptr, nullptr, 4882207410565625576ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(4882207410565625576ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Int32 = {{nullptr, nullptr, 11009693519287992193ULL, 0u, 32, 2, 0}, {nullptr, nullptr, 0, 0, 0, 0}};
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

struct chaos_type_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects
{
	ThinLockableHeader header{};
};

struct chaos_boxed_type_System_Private_CoreLib_System_Int32
{
	PureTypeHeader header{};
	CHAOS_IL2CPP_INTPTR value = 0;
};

CHAOS_IL2CPP_INT32 chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = 0;

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_Interlocked__CompareExchange_System_Int32_System_Int32__System_Int32_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
{
	return ChaosInterlockedCompareExchangeInt32(chaos_fn_arg_0, chaos_fn_arg_1, chaos_fn_arg_2);
}

static void chaos_external_runtime_System_Private_CoreLib_Interlocked__MemoryBarrier_System_Void__(void)
{
	ChaosInterlockedMemoryBarrier();
}

static void chaos_external_runtime_System_Private_CoreLib_Monitor__Exit_System_Void_System_Object_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	chaos_monitor_exit(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_Monitor__TryEnter_System_Boolean_System_Object_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	return ChaosMonitorTryEnter(chaos_fn_arg_0, chaos_fn_arg_1);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	ChaosObjectCtor(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_Volatile__Read_System_Int32_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosVolatileRead(chaos_fn_arg_0);
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_0(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_1(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_2(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_3(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_4(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_5(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_6(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_7(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_8(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_9(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_10(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_11(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_12(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_13(void);


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
		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_0
		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_1
		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_2
		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_3
		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_4
		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_5
		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_6
		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_7
		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_8
		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_9
		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_10
		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_11
		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_12
		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_13
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
		"ThreadingMonitorInterlockedSubjects",
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
		&chaos_mt_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects.hot,
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
		/* .name_utf8         = */ "ThreadingMonitorInterlockedSubjects",
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
		::chaos::il2cpp::runtime_core::RegisterModule("ThreadingMonitorInterlockedSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[14] = {
	{ "CustomEntrySubject_0", 0x00000007u, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "CustomEntrySubject_1", 0x00000008u, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "Subject_2", 0x00000003u, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "Subject_3", 0x00000004u, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "CustomEntrySubject_4", 0x00000009u, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "CustomEntrySubject_5", 0x0000000Au, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "CustomEntrySubject_6", 0x0000000Bu, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "CustomEntrySubject_7", 0x0000000Cu, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "CustomEntrySubject_8", 0x0000000Du, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "CustomEntrySubject_9", 0x0000000Eu, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "CustomEntrySubject_10", 0x0000000Fu, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "CustomEntrySubject_11", 0x00000010u, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "Subject_12", 0x00000005u, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "Subject_13", 0x00000006u, 0u },  // ThreadingMonitorInterlockedSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "ThreadingMonitorInterlockedSubjects", "", 0u, 14u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[14] = {
	{ 0x00000003u, 2u },
	{ 0x00000004u, 3u },
	{ 0x00000005u, 12u },
	{ 0x00000006u, 13u },
	{ 0x00000007u, 0u },
	{ 0x00000008u, 1u },
	{ 0x00000009u, 4u },
	{ 0x0000000Au, 5u },
	{ 0x0000000Bu, 6u },
	{ 0x0000000Cu, 7u },
	{ 0x0000000Du, 8u },
	{ 0x0000000Eu, 9u },
	{ 0x0000000Fu, 10u },
	{ 0x00000010u, 11u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[14] = {
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_0
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_1
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingMonitorInterlockedSubjects::Subject_2
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingMonitorInterlockedSubjects::Subject_3
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_4
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_5
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_6
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_7
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_8
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_9
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_10
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_11
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingMonitorInterlockedSubjects::Subject_12
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ThreadingMonitorInterlockedSubjects::Subject_13
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"ThreadingMonitorInterlockedSubjects",
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

extern "C" const char* kChaosExternalRuntimeSubjects[17] = {
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
	"System.Threading/Monitor::Enter:System.Void(System.Object)",
	"System.Threading/Monitor::Exit:System.Void(System.Object)",
	"System.Private.CoreLib/System.Int32",
	"System.Threading/Monitor::TryEnter:System.Boolean(System.Object)",
	"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode",
	"System.Threading/Monitor::TryEnter:System.Boolean(System.Object,System.Int32)",
	"System.Threading/Monitor::Pulse:System.Void(System.Object)",
	"System.Threading/Monitor::PulseAll:System.Void(System.Object)",
	"System.Threading/Monitor::Wait:System.Boolean(System.Object,System.Int32)",
	"System.Threading/Interlocked::Increment:System.Int32(System.Int32&)",
	"System.Threading/Interlocked::Decrement:System.Int32(System.Int32&)",
	"System.Threading/Interlocked::Exchange:System.Int32(System.Int32&,System.Int32)",
	"System.Threading/Interlocked::CompareExchange:System.Int32(System.Int32&,System.Int32,System.Int32)",
	"System.Threading/Interlocked::Add:System.Int32(System.Int32&,System.Int32)",
	"System.Threading/Interlocked::MemoryBarrier:System.Void()",
	"System.Threading/Volatile::Read:System.Int32(System.Int32&)",
};

extern "C" void* kChaosExternalRuntimeFnTable[17] = {
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__),
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

extern "C" int32_t kChaosExternalRuntimeCount = 17;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[14])() = {
	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_0),
	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_1),
	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_4),
	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_5),
	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_6),
	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_7),
	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_8),
	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_9),
	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_10),
	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_11),
	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_12),
	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_13),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
static void (*kBenchmarkWrappers[14])() = {
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
static void* const kMethodPointers[14] = {
	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_0),
	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_1),
	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_2),
	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_3),
	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_4),
	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_5),
	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_6),
	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_7),
	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_8),
	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_9),
	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_10),
	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_11),
	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_12),
	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_13),
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
	.image_name_utf8    = "ThreadingMonitorInterlockedSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects[1] = {
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects[14] = {
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_0:System.Void()", "CustomEntrySubject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_1:System.Void()", "CustomEntrySubject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_4:System.Void()", "CustomEntrySubject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_5:System.Void()", "CustomEntrySubject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_6:System.Void()", "CustomEntrySubject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_7:System.Void()", "CustomEntrySubject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_8:System.Void()", "CustomEntrySubject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_9:System.Void()", "CustomEntrySubject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_10:System.Void()", "CustomEntrySubject_10", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_11:System.Void()", "CustomEntrySubject_11", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_12:System.Void()", "Subject_12", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_13:System.Void()", "Subject_13", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects", "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects", "", "ThreadingMonitorInterlockedSubjects", "ThreadingMonitorInterlockedSubjects", nullptr, kReflFields_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects, 1u, nullptr, 0u,
	kReflMethods_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects, 14u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "ThreadingMonitorInterlockedSubjects", kReflTypePtrs, 1u };

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
// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_0()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Object, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Object.hot;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s0;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[1])(chaos_arg_0);
	}
	_s0 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Private_CoreLib_Monitor__Exit_System_Void_System_Object_(chaos_arg_0);
	}
	return;
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_1()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Object, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Object.hot;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s0;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[1])(chaos_arg_0);
	}
	_s0 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Private_CoreLib_Monitor__Exit_System_Void_System_Object_(chaos_arg_0);
	}
	return;
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_2()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_2(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_boxed_type_System_Private_CoreLib_System_Int32 chaos_box_storage_0{};
	{
		const auto chaos_value = _s0;
		chaos_box_storage_0.header.type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
		chaos_box_storage_0.value = chaos_value;
	}
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_box_storage_0);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[4])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
		{
			if (_s0 != 0)
			{
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
				{
					auto chaos_value = _s0;
					chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
				}
			}
			return;
		}
	}
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_3()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_3(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_boxed_type_System_Private_CoreLib_System_Int32 chaos_box_storage_0{};
	{
		const auto chaos_value = _s0;
		chaos_box_storage_0.header.type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
		chaos_box_storage_0.value = chaos_value;
	}
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_box_storage_0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_Monitor__TryEnter_System_Boolean_System_Object_System_Int32_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
		{
			if (_s0 != 0)
			{
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
				{
					auto chaos_value = _s0;
					chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
				}
			}
			return;
		}
	}
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_4()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Object, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Object.hot;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s0;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[1])(chaos_arg_0);
	}
	_s0 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s0;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[7])(chaos_arg_0);
	}
	_s0 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Private_CoreLib_Monitor__Exit_System_Void_System_Object_(chaos_arg_0);
	}
	return;
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_5()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Object, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Object.hot;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s0;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[1])(chaos_arg_0);
	}
	_s0 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s0;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[8])(chaos_arg_0);
	}
	_s0 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Private_CoreLib_Monitor__Exit_System_Void_System_Object_(chaos_arg_0);
	}
	return;
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_6()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_6(void)
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


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Object, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Object.hot;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s0;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[1])(chaos_arg_0);
	}
	_s0 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s0;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[7])(chaos_arg_0);
	}
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[9])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Private_CoreLib_Monitor__Exit_System_Void_System_Object_(chaos_arg_0);
	}
	return;
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_7()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_7(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[0] = _s0;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[10])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_8()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_8(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	chaos_locals[0] = _s0;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[11])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_9()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_9(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_locals[0] = _s0;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(99);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[12])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
			}
		}
		_s0 = chaos_locals[0];
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(99);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
		{
			if (_s0 != 0)
			{
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
				{
					auto chaos_value = _s0;
					chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
				}
			}
			return;
		}
	}
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_10()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_10(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_locals[0] = _s0;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(99);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_Interlocked__CompareExchange_System_Int32_System_Int32__System_Int32_System_Int32_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
			}
		}
		_s0 = chaos_locals[0];
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(99);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
		{
			if (_s0 != 0)
			{
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
				{
					auto chaos_value = _s0;
					chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
				}
			}
			return;
		}
	}
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_11()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_11(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	chaos_locals[0] = _s0;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[14])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(15);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_12()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_12(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	{
		chaos_external_runtime_System_Private_CoreLib_Interlocked__MemoryBarrier_System_Void__();
	}
	return;
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_13()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_13(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_locals[0] = _s0;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_Volatile__Read_System_Int32_System_Int32__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}



}  // namespace chaos::il2cpp::codegen::ThreadingMonitorInterlockedSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 14;

// ── JIT Method Entry Table ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kMethodJson_0[] = "{\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.custom-entry-subject-0\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_0:System.Void()\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::CustomEntrySubject_0()\",\"identity\":{\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"declaringTypeSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects\",\"definitionSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_0:System.Void()\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_0:System.Void()\",\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.custom-entry-subject-0\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::CustomEntrySubject_0()\"},\"nativeSymbol\":\"ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":1,\"resultType\":\"System.Private.CoreLib/System.Object\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":8,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/Monitor::Enter:System.Void(System.Object)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Monitor::Enter:System.Void(System.Object)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Monitor::Enter:System.Void(System.Object)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":15,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/Monitor::Exit:System.Void(System.Object)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Monitor::Exit:System.Void(System.Object)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Monitor::Exit:System.Void(System.Object)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":21,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingMonitorInterlockedSubjects\"}";
static const char kMethodJson_1[] = "{\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.custom-entry-subject-1\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_1:System.Void()\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::CustomEntrySubject_1()\",\"identity\":{\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"declaringTypeSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects\",\"definitionSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_1:System.Void()\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_1:System.Void()\",\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.custom-entry-subject-1\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::CustomEntrySubject_1()\"},\"nativeSymbol\":\"ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":1,\"resultType\":\"System.Private.CoreLib/System.Object\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":8,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/Monitor::Enter:System.Void(System.Object)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Monitor::Enter:System.Void(System.Object)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Monitor::Enter:System.Void(System.Object)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":15,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/Monitor::Exit:System.Void(System.Object)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Monitor::Exit:System.Void(System.Object)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Monitor::Exit:System.Void(System.Object)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":21,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingMonitorInterlockedSubjects\"}";
static const char kMethodJson_2[] = "{\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.subject-2\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_2:System.Void()\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::Subject_2()\",\"identity\":{\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"declaringTypeSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects\",\"definitionSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_2:System.Void()\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_2:System.Void()\",\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.subject-2\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::Subject_2()\"},\"nativeSymbol\":\"ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_2\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"box\",\"opCode\":35,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":3,\"resultType\":\"System.Object\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":9,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":8,\"resultType\":\"System.Boolean\",\"callee\":\"System.Threading/Monitor::TryEnter:System.Boolean(System.Object)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Monitor::TryEnter:System.Boolean(System.Object)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Monitor::TryEnter:System.Boolean(System.Object)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":18,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":19,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":35,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\",\"ilOffset\":30,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":35,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingMonitorInterlockedSubjects\"}";
static const char kMethodJson_3[] = "{\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.subject-3\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_3:System.Void()\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::Subject_3()\",\"identity\":{\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"declaringTypeSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects\",\"definitionSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_3:System.Void()\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_3:System.Void()\",\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.subject-3\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::Subject_3()\"},\"nativeSymbol\":\"ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_3\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"box\",\"opCode\":35,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":3,\"resultType\":\"System.Object\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":9,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":10,\"resultType\":\"System.Boolean\",\"callee\":\"System.Threading/Monitor::TryEnter:System.Boolean(System.Object,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Monitor::TryEnter:System.Boolean(System.Object,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Monitor::TryEnter:System.Boolean(System.Object,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":20,\"ilOffset\":15,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":21,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":37,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\",\"ilOffset\":32,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":37,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingMonitorInterlockedSubjects\"}";
static const char kMethodJson_4[] = "{\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.custom-entry-subject-4\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_4:System.Void()\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::CustomEntrySubject_4()\",\"identity\":{\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"declaringTypeSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects\",\"definitionSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_4:System.Void()\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_4:System.Void()\",\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.custom-entry-subject-4\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::CustomEntrySubject_4()\"},\"nativeSymbol\":\"ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_4\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":1,\"resultType\":\"System.Private.CoreLib/System.Object\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":8,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/Monitor::Enter:System.Void(System.Object)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Monitor::Enter:System.Void(System.Object)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Monitor::Enter:System.Void(System.Object)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":15,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/Monitor::Pulse:System.Void(System.Object)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Monitor::Pulse:System.Void(System.Object)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Monitor::Pulse:System.Void(System.Object)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":21,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":22,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/Monitor::Exit:System.Void(System.Object)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Monitor::Exit:System.Void(System.Object)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Monitor::Exit:System.Void(System.Object)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":28,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingMonitorInterlockedSubjects\"}";
static const char kMethodJson_5[] = "{\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.custom-entry-subject-5\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_5:System.Void()\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::CustomEntrySubject_5()\",\"identity\":{\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"declaringTypeSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects\",\"definitionSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_5:System.Void()\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_5:System.Void()\",\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.custom-entry-subject-5\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::CustomEntrySubject_5()\"},\"nativeSymbol\":\"ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_5\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":1,\"resultType\":\"System.Private.CoreLib/System.Object\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":8,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/Monitor::Enter:System.Void(System.Object)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Monitor::Enter:System.Void(System.Object)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Monitor::Enter:System.Void(System.Object)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":15,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/Monitor::PulseAll:System.Void(System.Object)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Monitor::PulseAll:System.Void(System.Object)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Monitor::PulseAll:System.Void(System.Object)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":21,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":22,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/Monitor::Exit:System.Void(System.Object)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Monitor::Exit:System.Void(System.Object)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Monitor::Exit:System.Void(System.Object)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":28,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingMonitorInterlockedSubjects\"}";
static const char kMethodJson_6[] = "{\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.custom-entry-subject-6\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_6:System.Void()\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::CustomEntrySubject_6()\",\"identity\":{\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"declaringTypeSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects\",\"definitionSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_6:System.Void()\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_6:System.Void()\",\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.custom-entry-subject-6\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::CustomEntrySubject_6()\"},\"nativeSymbol\":\"ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_6\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":1,\"resultType\":\"System.Private.CoreLib/System.Object\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":8,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/Monitor::Enter:System.Void(System.Object)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Monitor::Enter:System.Void(System.Object)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Monitor::Enter:System.Void(System.Object)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":15,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/Monitor::Pulse:System.Void(System.Object)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Monitor::Pulse:System.Void(System.Object)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Monitor::Pulse:System.Void(System.Object)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":21,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":23,\"resultType\":\"System.Boolean\",\"callee\":\"System.Threading/Monitor::Wait:System.Boolean(System.Object,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Monitor::Wait:System.Boolean(System.Object,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Monitor::Wait:System.Boolean(System.Object,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":29,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":30,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/Monitor::Exit:System.Void(System.Object)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Monitor::Exit:System.Void(System.Object)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Monitor::Exit:System.Void(System.Object)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":36,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingMonitorInterlockedSubjects\"}";
static const char kMethodJson_7[] = "{\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.custom-entry-subject-7\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_7:System.Void()\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::CustomEntrySubject_7()\",\"identity\":{\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"declaringTypeSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects\",\"definitionSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_7:System.Void()\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_7:System.Void()\",\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.custom-entry-subject-7\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::CustomEntrySubject_7()\"},\"nativeSymbol\":\"ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_7\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":3,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":5,\"resultType\":\"System.Int32\",\"callee\":\"System.Threading/Interlocked::Increment:System.Int32(System.Int32\\u0026)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Interlocked::Increment:System.Int32(System.Int32\\u0026)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Interlocked::Increment:System.Int32(System.Int32\\u0026)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":17,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":26,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\",\"ilOffset\":21,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":26,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingMonitorInterlockedSubjects\"}";
static const char kMethodJson_8[] = "{\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.custom-entry-subject-8\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_8:System.Void()\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::CustomEntrySubject_8()\",\"identity\":{\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"declaringTypeSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects\",\"definitionSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_8:System.Void()\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_8:System.Void()\",\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.custom-entry-subject-8\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::CustomEntrySubject_8()\"},\"nativeSymbol\":\"ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_8\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":3,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":5,\"resultType\":\"System.Int32\",\"callee\":\"System.Threading/Interlocked::Decrement:System.Int32(System.Int32\\u0026)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Interlocked::Decrement:System.Int32(System.Int32\\u0026)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Interlocked::Decrement:System.Int32(System.Int32\\u0026)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":23,\"ilOffset\":15,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\",\"ilOffset\":18,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingMonitorInterlockedSubjects\"}";
static const char kMethodJson_9[] = "{\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.custom-entry-subject-9\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_9:System.Void()\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::CustomEntrySubject_9()\",\"identity\":{\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"declaringTypeSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects\",\"definitionSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_9:System.Void()\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_9:System.Void()\",\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.custom-entry-subject-9\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::CustomEntrySubject_9()\"},\"nativeSymbol\":\"ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_9\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":3,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":3,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":99,\"ilOffset\":6,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"callee\":\"System.Threading/Interlocked::Exchange:System.Int32(System.Int32\\u0026,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Interlocked::Exchange:System.Int32(System.Int32\\u0026,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Interlocked::Exchange:System.Int32(System.Int32\\u0026,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":21,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":30,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\",\"ilOffset\":25,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":99,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":36,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":38,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":39,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":48,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":42,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\",\"ilOffset\":43,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":48,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingMonitorInterlockedSubjects\"}";
static const char kMethodJson_10[] = "{\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.custom-entry-subject-10\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_10:System.Void()\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::CustomEntrySubject_10()\",\"identity\":{\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"declaringTypeSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects\",\"definitionSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_10:System.Void()\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_10:System.Void()\",\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.custom-entry-subject-10\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::CustomEntrySubject_10()\"},\"nativeSymbol\":\"ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_10\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":3,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":3,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":99,\"ilOffset\":6,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"callee\":\"System.Threading/Interlocked::CompareExchange:System.Int32(System.Int32\\u0026,System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Interlocked::CompareExchange:System.Int32(System.Int32\\u0026,System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Interlocked::CompareExchange:System.Int32(System.Int32\\u0026,System.Int32,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":23,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":32,\"ilOffset\":24,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\",\"ilOffset\":27,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":99,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":37,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":38,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":40,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":41,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":50,\"ilOffset\":42,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\",\"ilOffset\":45,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":50,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingMonitorInterlockedSubjects\"}";
static const char kMethodJson_11[] = "{\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.custom-entry-subject-11\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_11:System.Void()\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::CustomEntrySubject_11()\",\"identity\":{\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"declaringTypeSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects\",\"definitionSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_11:System.Void()\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_11:System.Void()\",\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.custom-entry-subject-11\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::CustomEntrySubject_11()\"},\"nativeSymbol\":\"ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_11\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":10,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":3,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":5,\"ilOffset\":6,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"System.Threading/Interlocked::Add:System.Int32(System.Int32\\u0026,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Interlocked::Add:System.Int32(System.Int32\\u0026,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Interlocked::Add:System.Int32(System.Int32\\u0026,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":15,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":20,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":29,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\",\"ilOffset\":24,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":29,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingMonitorInterlockedSubjects\"}";
static const char kMethodJson_12[] = "{\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.subject-12\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_12:System.Void()\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::Subject_12()\",\"identity\":{\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"declaringTypeSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects\",\"definitionSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_12:System.Void()\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_12:System.Void()\",\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.subject-12\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::Subject_12()\"},\"nativeSymbol\":\"ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_12\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Threading/Interlocked::MemoryBarrier:System.Void()\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Interlocked::MemoryBarrier:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Interlocked::MemoryBarrier:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingMonitorInterlockedSubjects\"}";
static const char kMethodJson_13[] = "{\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.subject-13\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_13:System.Void()\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::Subject_13()\",\"identity\":{\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"declaringTypeSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects\",\"definitionSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_13:System.Void()\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_13:System.Void()\",\"methodId\":\"threading-monitor-interlocked-subjects.threading-monitor-interlocked-subjects.subject-13\",\"signature\":\"System.Void ThreadingMonitorInterlockedSubjects::Subject_13()\"},\"nativeSymbol\":\"ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_13\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":3,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Int32\",\"callee\":\"System.Threading/Volatile::Read:System.Int32(System.Int32\\u0026)\",\"reference\":{\"assemblyName\":\"System.Threading\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading/Volatile::Read:System.Int32(System.Int32\\u0026)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading\",\"subjectId\":\"System.Threading/Volatile::Read:System.Int32(System.Int32\\u0026)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":28,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\",\"ilOffset\":23,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ThreadingMonitorInterlockedSubjects\",\"subjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":28,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ThreadingMonitorInterlockedSubjects\"}";

extern "C" const JitMethodEntry kChaosJitMethodEntries[14] =
{
    { kMethodJson_0, 3606u, 0x00000007u, 0u },
    { kMethodJson_1, 3606u, 0x00000008u, 0u },
    { kMethodJson_2, 4748u, 0x00000003u, 0u },
    { kMethodJson_3, 4891u, 0x00000004u, 0u },
    { kMethodJson_4, 4265u, 0x00000009u, 0u },
    { kMethodJson_5, 4274u, 0x0000000Au, 0u },
    { kMethodJson_6, 5136u, 0x0000000Bu, 0u },
    { kMethodJson_7, 4126u, 0x0000000Cu, 0u },
    { kMethodJson_8, 3937u, 0x0000000Du, 0u },
    { kMethodJson_9, 5943u, 0x0000000Eu, 0u },
    { kMethodJson_10, 6115u, 0x0000000Fu, 0u },
    { kMethodJson_11, 4259u, 0x00000010u, 0u },
    { kMethodJson_12, 2088u, 0x00000005u, 0u },
    { kMethodJson_13, 4020u, 0x00000006u, 0u }
};

extern "C" const uint32_t kChaosJitMethodEntryCount = 14u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitMethods(kChaosJitMethodEntries, kChaosJitMethodEntryCount);
}