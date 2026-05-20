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
#include "enum_metadata.generated.h"

// Forward declaration for dispatch table entries (defined in runtime_stubs.cpp)
extern "C" void InterpreterEntryDirect(
    CHAOS_IL2CPP_UINTPTR method_key,
    void*     args_buf,
    void*     ret_buf) noexcept;

#pragma warning(push)
#pragma warning(disable: 4065 4244)

namespace chaos::il2cpp::codegen::ThreadingThreadBasicsSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_PlatformNotSupportedException = {{nullptr, nullptr, 3627294964229478006ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_PlatformNotSupportedException = static_cast<CHAOS_IL2CPP_INTPTR>(3627294964229478006ULL);
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
inline TypeInfoV0 chaos_mt_System_Threading_Thread_System_Threading_ThreadInterruptedException = {{nullptr, nullptr, 1689251155357643907ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Threading_Thread_System_Threading_ThreadInterruptedException = static_cast<CHAOS_IL2CPP_INTPTR>(1689251155357643907ULL);

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

struct chaos_type_System_Private_CoreLib_System_PlatformNotSupportedException
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR _message = 0;
	CHAOS_IL2CPP_INTPTR _innerException = 0;
	CHAOS_IL2CPP_INTPTR _stackTrace = 0;
	CHAOS_IL2CPP_INT32 _HResult = 0;
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

struct chaos_type_System_Threading_Thread_System_Threading_ThreadInterruptedException
{
	ThinLockableHeader header{};
};

static void chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Interrupt_System_Void__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	chaos_thread_interrupt(chaos_fn_arg_0);
}

static void chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Sleep_System_Void_System_Int32_(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	chaos_thread_sleep(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Yield_System_Boolean__(void)
{
	return chaos_thread_yield();
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__(void)
{
	return chaos_thread_get_current();
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_IsBackground_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return chaos_thread_is_background(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_IsThreadPoolThread_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return chaos_thread_is_threadpool(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_Priority_System_Threading_ThreadPriority__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return chaos_thread_get_priority(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_ThreadState_System_Threading_ThreadState__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return chaos_thread_get_state(chaos_fn_arg_0);
}

static void chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__set_IsBackground_System_Void_System_Boolean_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	chaos_thread_set_background(chaos_fn_arg_0, chaos_fn_arg_1);
}

static void chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__set_Priority_System_Void_System_Threading_ThreadPriority_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	chaos_thread_set_priority(chaos_fn_arg_0, chaos_fn_arg_1);
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_0(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_1(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_2(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_3(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_4(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_5(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_6(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_7(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_8(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_9(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_10(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod0(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod1(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod10(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod2(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod3(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod4(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod5(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod6(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod7(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod8(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod9(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[23] = {
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
	::ChaosAbiMethodEntryV0 entries[22];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		22u,
		0u,
		2614092085u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_0
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_1
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_2
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_3
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_4
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_5
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_6
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_7
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_8
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_9
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_10
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod0
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod1
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod10
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod2
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod3
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod4
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod5
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod6
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod7
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod8
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod9
	},
	{
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
		"ThreadingThreadBasicsSubjects",
		"ThreadingThreadBasicsNativeEntry",
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
		nullptr,
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
		/* .name_utf8         = */ "ThreadingThreadBasicsSubjects",
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
		::chaos::il2cpp::runtime_core::RegisterModule("ThreadingThreadBasicsSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[22] = {
	{ "CustomEntryMethod0", 0x0000000Fu, 0u },  // ThreadingThreadBasicsNativeEntry
	{ "CustomEntryMethod1", 0x00000010u, 0u },  // ThreadingThreadBasicsNativeEntry
	{ "CustomEntryMethod10", 0x00000019u, 0u },  // ThreadingThreadBasicsNativeEntry
	{ "CustomEntryMethod2", 0x00000011u, 0u },  // ThreadingThreadBasicsNativeEntry
	{ "CustomEntryMethod3", 0x00000012u, 0u },  // ThreadingThreadBasicsNativeEntry
	{ "CustomEntryMethod4", 0x00000013u, 0u },  // ThreadingThreadBasicsNativeEntry
	{ "CustomEntryMethod5", 0x00000014u, 0u },  // ThreadingThreadBasicsNativeEntry
	{ "CustomEntryMethod6", 0x00000015u, 0u },  // ThreadingThreadBasicsNativeEntry
	{ "CustomEntryMethod7", 0x00000016u, 0u },  // ThreadingThreadBasicsNativeEntry
	{ "CustomEntryMethod8", 0x00000017u, 0u },  // ThreadingThreadBasicsNativeEntry
	{ "CustomEntryMethod9", 0x00000018u, 0u },  // ThreadingThreadBasicsNativeEntry
	{ "CustomEntrySubject_0", 0x00000004u, 0u },  // ThreadingThreadBasicsSubjects
	{ "CustomEntrySubject_1", 0x00000005u, 0u },  // ThreadingThreadBasicsSubjects
	{ "CustomEntrySubject_2", 0x00000006u, 0u },  // ThreadingThreadBasicsSubjects
	{ "CustomEntrySubject_3", 0x00000007u, 0u },  // ThreadingThreadBasicsSubjects
	{ "CustomEntrySubject_4", 0x00000008u, 0u },  // ThreadingThreadBasicsSubjects
	{ "CustomEntrySubject_5", 0x00000009u, 0u },  // ThreadingThreadBasicsSubjects
	{ "CustomEntrySubject_6", 0x0000000Au, 0u },  // ThreadingThreadBasicsSubjects
	{ "CustomEntrySubject_7", 0x0000000Bu, 0u },  // ThreadingThreadBasicsSubjects
	{ "CustomEntrySubject_8", 0x0000000Cu, 0u },  // ThreadingThreadBasicsSubjects
	{ "CustomEntrySubject_9", 0x0000000Du, 0u },  // ThreadingThreadBasicsSubjects
	{ "CustomEntrySubject_10", 0x0000000Eu, 0u },  // ThreadingThreadBasicsSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[2] = {
	{ "ThreadingThreadBasicsNativeEntry", "", 0u, 11u },
	{ "ThreadingThreadBasicsSubjects", "", 11u, 11u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[22] = {
	{ 0x00000004u, 0u },
	{ 0x00000005u, 1u },
	{ 0x00000006u, 2u },
	{ 0x00000007u, 3u },
	{ 0x00000008u, 4u },
	{ 0x00000009u, 5u },
	{ 0x0000000Au, 6u },
	{ 0x0000000Bu, 7u },
	{ 0x0000000Cu, 8u },
	{ 0x0000000Du, 9u },
	{ 0x0000000Eu, 10u },
	{ 0x0000000Fu, 11u },
	{ 0x00000010u, 12u },
	{ 0x00000011u, 14u },
	{ 0x00000012u, 15u },
	{ 0x00000013u, 16u },
	{ 0x00000014u, 17u },
	{ 0x00000015u, 18u },
	{ 0x00000016u, 19u },
	{ 0x00000017u, 20u },
	{ 0x00000018u, 21u },
	{ 0x00000019u, 13u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[22] = {
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsSubjects::CustomEntrySubject_0
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsSubjects::CustomEntrySubject_1
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsSubjects::CustomEntrySubject_2
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsSubjects::CustomEntrySubject_3
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsSubjects::CustomEntrySubject_4
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsSubjects::CustomEntrySubject_5
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsSubjects::CustomEntrySubject_6
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsSubjects::CustomEntrySubject_7
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsSubjects::CustomEntrySubject_8
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsSubjects::CustomEntrySubject_9
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsSubjects::CustomEntrySubject_10
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsNativeEntry::CustomEntryMethod0
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsNativeEntry::CustomEntryMethod1
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsNativeEntry::CustomEntryMethod10
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsNativeEntry::CustomEntryMethod2
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsNativeEntry::CustomEntryMethod3
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsNativeEntry::CustomEntryMethod4
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsNativeEntry::CustomEntryMethod5
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsNativeEntry::CustomEntryMethod6
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsNativeEntry::CustomEntryMethod7
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsNativeEntry::CustomEntryMethod8
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsNativeEntry::CustomEntryMethod9
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"ThreadingThreadBasicsSubjects",
	s_hotpatch_types,
	2u,
	s_hotpatch_methods,
	22u,
	s_hotpatch_slots,
	22u,
	s_hotpatch_entries,
	22u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[12] = {
	"System.Threading.Thread/System.Threading.Thread::get_CurrentThread:System.Threading.Thread()",
	"System.Threading.Thread/System.Threading.Thread::get_ThreadState:System.Threading.ThreadState()",
	"System.Threading.Thread/System.Threading.Thread::get_Priority:System.Threading.ThreadPriority()",
	"System.Threading.Thread/System.Threading.Thread::set_Priority:System.Void(System.Threading.ThreadPriority)",
	"System.Threading.Thread/System.Threading.Thread::get_IsBackground:System.Boolean()",
	"System.Threading.Thread/System.Threading.Thread::set_IsBackground:System.Void(System.Boolean)",
	"System.Threading.Thread/System.Threading.Thread::get_IsThreadPoolThread:System.Boolean()",
	"System.Threading.Thread/System.Threading.Thread::Abort:System.Void()",
	"System.Threading.Thread/System.Threading.Thread::Interrupt:System.Void()",
	"System.Threading.Thread/System.Threading.Thread::Sleep:System.Void(System.Int32)",
	"System.Threading.Thread/System.Threading.Thread::ResetAbort:System.Void()",
	"System.Threading.Thread/System.Threading.Thread::Yield:System.Boolean()",
};

extern "C" void* kChaosExternalRuntimeFnTable[12] = {
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_ThreadState_System_Threading_ThreadState__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_Priority_System_Threading_ThreadPriority__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__set_Priority_System_Void_System_Threading_ThreadPriority_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_IsBackground_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__set_IsBackground_System_Void_System_Boolean_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_IsThreadPoolThread_System_Boolean__),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Interrupt_System_Void__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Sleep_System_Void_System_Int32_),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Yield_System_Boolean__),
};

extern "C" int32_t kChaosExternalRuntimeCount = 12;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[22])() = {
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_0),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_1),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_2),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_3),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_4),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_5),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_6),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_7),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_8),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_9),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_10),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod0),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod1),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod10),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod2),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod3),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod4),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod5),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod6),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod7),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod8),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod9),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[22])() = {
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
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 11;
extern "C" const int kSubjectEntryIndices[11] = {
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
	10
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

// Pure AOT benchmark: switch-based direct dispatch per method.
// Each case is a compile-time constant, enabling MSVC to devirtualize and inline
// the method body into the timing loop — eliminating function pointer indirection.
extern "C" double BenchmarkMethod(
	int chaos_entry_index, int iterations) {
	if (chaos_entry_index < 0 || chaos_entry_index >= kAotMethodCount)
		return -1.0;
	switch (chaos_entry_index) {
	case 0: {
		auto start = std::chrono::steady_clock::now();
		for (int i = 0; i < iterations; i++) {
			ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_0();
		}
		auto end = std::chrono::steady_clock::now();
		return std::chrono::duration<double, std::milli>(
			end - start).count();
	}
	case 1: {
		auto start = std::chrono::steady_clock::now();
		for (int i = 0; i < iterations; i++) {
			ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_1();
		}
		auto end = std::chrono::steady_clock::now();
		return std::chrono::duration<double, std::milli>(
			end - start).count();
	}
	case 2: {
		auto start = std::chrono::steady_clock::now();
		for (int i = 0; i < iterations; i++) {
			ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_2();
		}
		auto end = std::chrono::steady_clock::now();
		return std::chrono::duration<double, std::milli>(
			end - start).count();
	}
	case 3: {
		auto start = std::chrono::steady_clock::now();
		for (int i = 0; i < iterations; i++) {
			ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_3();
		}
		auto end = std::chrono::steady_clock::now();
		return std::chrono::duration<double, std::milli>(
			end - start).count();
	}
	case 4: {
		auto start = std::chrono::steady_clock::now();
		for (int i = 0; i < iterations; i++) {
			ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_4();
		}
		auto end = std::chrono::steady_clock::now();
		return std::chrono::duration<double, std::milli>(
			end - start).count();
	}
	case 5: {
		auto start = std::chrono::steady_clock::now();
		for (int i = 0; i < iterations; i++) {
			ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_5();
		}
		auto end = std::chrono::steady_clock::now();
		return std::chrono::duration<double, std::milli>(
			end - start).count();
	}
	case 6: {
		auto start = std::chrono::steady_clock::now();
		for (int i = 0; i < iterations; i++) {
			ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_6();
		}
		auto end = std::chrono::steady_clock::now();
		return std::chrono::duration<double, std::milli>(
			end - start).count();
	}
	case 7: {
		auto start = std::chrono::steady_clock::now();
		for (int i = 0; i < iterations; i++) {
			ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_7();
		}
		auto end = std::chrono::steady_clock::now();
		return std::chrono::duration<double, std::milli>(
			end - start).count();
	}
	case 8: {
		auto start = std::chrono::steady_clock::now();
		for (int i = 0; i < iterations; i++) {
			ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_8();
		}
		auto end = std::chrono::steady_clock::now();
		return std::chrono::duration<double, std::milli>(
			end - start).count();
	}
	case 9: {
		auto start = std::chrono::steady_clock::now();
		for (int i = 0; i < iterations; i++) {
			ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_9();
		}
		auto end = std::chrono::steady_clock::now();
		return std::chrono::duration<double, std::milli>(
			end - start).count();
	}
	case 10: {
		auto start = std::chrono::steady_clock::now();
		for (int i = 0; i < iterations; i++) {
			ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_10();
		}
		auto end = std::chrono::steady_clock::now();
		return std::chrono::duration<double, std::milli>(
			end - start).count();
	}
	case 11: {
		auto start = std::chrono::steady_clock::now();
		for (int i = 0; i < iterations; i++) {
			ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod0();
		}
		auto end = std::chrono::steady_clock::now();
		return std::chrono::duration<double, std::milli>(
			end - start).count();
	}
	case 12: {
		auto start = std::chrono::steady_clock::now();
		for (int i = 0; i < iterations; i++) {
			ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod1();
		}
		auto end = std::chrono::steady_clock::now();
		return std::chrono::duration<double, std::milli>(
			end - start).count();
	}
	case 13: {
		auto start = std::chrono::steady_clock::now();
		for (int i = 0; i < iterations; i++) {
			ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod10();
		}
		auto end = std::chrono::steady_clock::now();
		return std::chrono::duration<double, std::milli>(
			end - start).count();
	}
	case 14: {
		auto start = std::chrono::steady_clock::now();
		for (int i = 0; i < iterations; i++) {
			ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod2();
		}
		auto end = std::chrono::steady_clock::now();
		return std::chrono::duration<double, std::milli>(
			end - start).count();
	}
	case 15: {
		auto start = std::chrono::steady_clock::now();
		for (int i = 0; i < iterations; i++) {
			ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod3();
		}
		auto end = std::chrono::steady_clock::now();
		return std::chrono::duration<double, std::milli>(
			end - start).count();
	}
	case 16: {
		auto start = std::chrono::steady_clock::now();
		for (int i = 0; i < iterations; i++) {
			ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod4();
		}
		auto end = std::chrono::steady_clock::now();
		return std::chrono::duration<double, std::milli>(
			end - start).count();
	}
	case 17: {
		auto start = std::chrono::steady_clock::now();
		for (int i = 0; i < iterations; i++) {
			ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod5();
		}
		auto end = std::chrono::steady_clock::now();
		return std::chrono::duration<double, std::milli>(
			end - start).count();
	}
	case 18: {
		auto start = std::chrono::steady_clock::now();
		for (int i = 0; i < iterations; i++) {
			ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod6();
		}
		auto end = std::chrono::steady_clock::now();
		return std::chrono::duration<double, std::milli>(
			end - start).count();
	}
	case 19: {
		auto start = std::chrono::steady_clock::now();
		for (int i = 0; i < iterations; i++) {
			ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod7();
		}
		auto end = std::chrono::steady_clock::now();
		return std::chrono::duration<double, std::milli>(
			end - start).count();
	}
	case 20: {
		auto start = std::chrono::steady_clock::now();
		for (int i = 0; i < iterations; i++) {
			ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod8();
		}
		auto end = std::chrono::steady_clock::now();
		return std::chrono::duration<double, std::milli>(
			end - start).count();
	}
	case 21: {
		auto start = std::chrono::steady_clock::now();
		for (int i = 0; i < iterations; i++) {
			ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod9();
		}
		auto end = std::chrono::steady_clock::now();
		return std::chrono::duration<double, std::milli>(
			end - start).count();
	}
	default:
		return -1.0;
	}
}
// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[22] = {
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_0),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_1),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_2),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_3),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_4),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_5),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_6),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_7),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_8),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_9),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_10),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod0),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod1),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod10),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod2),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod3),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod4),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod5),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod6),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod7),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod8),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod9),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 22u,
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
	.image_name_utf8    = "ThreadingThreadBasicsSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects[1] = {
	{ 0u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects[11] = {
	{ 0u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_0:System.Void()", "CustomEntrySubject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_1:System.Void()", "CustomEntrySubject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_2:System.Void()", "CustomEntrySubject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_3:System.Void()", "CustomEntrySubject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_4:System.Void()", "CustomEntrySubject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_5:System.Void()", "CustomEntrySubject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_6:System.Void()", "CustomEntrySubject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_7:System.Void()", "CustomEntrySubject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_8:System.Void()", "CustomEntrySubject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_9:System.Void()", "CustomEntrySubject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_10:System.Void()", "CustomEntrySubject_10", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry[11] = {
	{ 0u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod0:System.Void()", "CustomEntryMethod0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod1:System.Void()", "CustomEntryMethod1", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod10:System.Void()", "CustomEntryMethod10", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod2:System.Void()", "CustomEntryMethod2", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod3:System.Void()", "CustomEntryMethod3", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod4:System.Void()", "CustomEntryMethod4", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod5:System.Void()", "CustomEntryMethod5", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod6:System.Void()", "CustomEntryMethod6", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod7:System.Void()", "CustomEntryMethod7", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod8:System.Void()", "CustomEntryMethod8", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod9:System.Void()", "CustomEntryMethod9", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[2] = {
	{ 0u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects", "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects", "", "ThreadingThreadBasicsSubjects", "ThreadingThreadBasicsSubjects", nullptr, kReflFields_ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects, 1u, nullptr, 0u,
	kReflMethods_ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects, 11u },
	{ 0u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry", "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry", "", "ThreadingThreadBasicsNativeEntry", "ThreadingThreadBasicsNativeEntry", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry, 11u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[2] = {
	&kReflTypes[0],
	&kReflTypes[1],
};

static const ReflectionQueryImageDescriptor kReflImage = { "ThreadingThreadBasicsSubjects", kReflTypePtrs, 2u };

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
// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_0()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	{
		const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_ThreadState_System_Threading_ThreadState__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_1()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	{
		const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_Priority_System_Threading_ThreadPriority__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_2()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	{
		const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__set_Priority_System_Void_System_Threading_ThreadPriority_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
	}
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_3()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	{
		const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_IsBackground_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_4()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	{
		const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__set_IsBackground_System_Void_System_Boolean_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
	}
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_5()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	{
		const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_IsThreadPoolThread_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_6()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_6(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
			{
				const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[7])();
			}
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_PlatformNotSupportedException.hot))
			{
				throw;
			}
		}
		_s1 = chaos_exception.object_value;
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
			{
				const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[7])();
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_PlatformNotSupportedException.hot))
			{
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
			{
				const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[7])();
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_PlatformNotSupportedException.hot))
			{
				chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_7()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_7(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	{
		const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Interrupt_System_Void__(chaos_arg_0);
	}
#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_0 = _s0;
				chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Sleep_System_Void_System_Int32_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
			}
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Threading_Thread_System_Threading_ThreadInterruptedException.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_0 = _s0;
				chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Sleep_System_Void_System_Int32_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Threading_Thread_System_Threading_ThreadInterruptedException.hot))
			{
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_0 = _s0;
				chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Sleep_System_Void_System_Int32_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Threading_Thread_System_Threading_ThreadInterruptedException.hot))
			{
				chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_8()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
			{
				reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[10])();
			}
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_PlatformNotSupportedException.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
			{
				reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[10])();
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_PlatformNotSupportedException.hot))
			{
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
			{
				reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[10])();
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_PlatformNotSupportedException.hot))
			{
				chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_9()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_9(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	{
		const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Yield_System_Boolean__();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_10()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_10(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Sleep_System_Void_System_Int32_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
	}
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod0()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	{
		const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_ThreadState_System_Threading_ThreadState__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod1()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	{
		const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_Priority_System_Threading_ThreadPriority__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod10()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod10(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Sleep_System_Void_System_Int32_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
	}
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod2()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	{
		const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__set_Priority_System_Void_System_Threading_ThreadPriority_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
	}
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod3()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	{
		const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_IsBackground_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod4()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	{
		const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__set_IsBackground_System_Void_System_Boolean_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
	}
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod5()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	{
		const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_IsThreadPoolThread_System_Boolean__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod6()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod6(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
			{
				const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[7])();
			}
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_PlatformNotSupportedException.hot))
			{
				throw;
			}
		}
		_s1 = chaos_exception.object_value;
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
			{
				const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[7])();
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_PlatformNotSupportedException.hot))
			{
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
			{
				const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[7])();
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_PlatformNotSupportedException.hot))
			{
				chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod7()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod7(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	{
		const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Interrupt_System_Void__(chaos_arg_0);
	}
#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_0 = _s0;
				chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Sleep_System_Void_System_Int32_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
			}
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Threading_Thread_System_Threading_ThreadInterruptedException.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_0 = _s0;
				chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Sleep_System_Void_System_Int32_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Threading_Thread_System_Threading_ThreadInterruptedException.hot))
			{
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_0 = _s0;
				chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Sleep_System_Void_System_Int32_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Threading_Thread_System_Threading_ThreadInterruptedException.hot))
			{
				chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod8()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
			{
				reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[10])();
			}
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_PlatformNotSupportedException.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
			{
				reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[10])();
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_PlatformNotSupportedException.hot))
			{
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
			{
				reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[10])();
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_PlatformNotSupportedException.hot))
			{
				chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod9()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod9(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	{
		const auto chaos_result = chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Yield_System_Boolean__();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}



}  // namespace chaos::il2cpp::codegen::ThreadingThreadBasicsSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 22;

extern "C" void ChaosJitRegisterAll() {}