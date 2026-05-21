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

namespace chaos::il2cpp::codegen::ComWrappersSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_ComWrappersSubjects_ComWrappersSubjects = {{nullptr, nullptr, 6835862197983294672ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ComWrappersSubjects_ComWrappersSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(6835862197983294672ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_ArgumentNullException = {{nullptr, nullptr, 12400915656331929738ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_ArgumentNullException = static_cast<CHAOS_IL2CPP_INTPTR>(12400915656331929738ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_IntPtr = {{nullptr, nullptr, 15622295425673451578ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_IntPtr = static_cast<CHAOS_IL2CPP_INTPTR>(15622295425673451578ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_NullReferenceException = {{nullptr, nullptr, 5314911908653589872ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_NullReferenceException = static_cast<CHAOS_IL2CPP_INTPTR>(5314911908653589872ULL);
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

struct chaos_type_ComWrappersSubjects_ComWrappersSubjects
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_ArgumentNullException
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR _message = 0;
	CHAOS_IL2CPP_INTPTR _innerException = 0;
	CHAOS_IL2CPP_INTPTR _stackTrace = 0;
	CHAOS_IL2CPP_INT32 _HResult = 0;
};

struct chaos_type_System_Private_CoreLib_System_IntPtr
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_NullReferenceException
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

struct chaos_boxed_type_System_Private_CoreLib_System_Int32
{
	PureTypeHeader header{};
	CHAOS_IL2CPP_INTPTR value = 0;
};

CHAOS_IL2CPP_INT32 chaos_static_ComWrappersSubjects_ComWrappersSubjects___exitCode = 0;
CHAOS_IL2CPP_INTPTR chaos_static_System_Private_CoreLib_System_IntPtr__Zero = 0;

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void ComWrappersSubjects_ComWrappersSubjects_Subject_0(void);
extern "C" void ComWrappersSubjects_ComWrappersSubjects_Subject_1(void);
extern "C" void ComWrappersSubjects_ComWrappersSubjects_Subject_2(void);
extern "C" void ComWrappersSubjects_ComWrappersSubjects_Subject_3(void);
extern "C" void ComWrappersSubjects_ComWrappersSubjects_Subject_4(void);
extern "C" void ComWrappersSubjects_ComWrappersSubjects_Subject_5(void);
extern "C" void ComWrappersSubjects_ComWrappersSubjects_Subject_6(void);
extern "C" void ComWrappersSubjects_ComWrappersSubjects_Subject_7(void);
extern "C" void ComWrappersSubjects_ComWrappersSubjects_Subject_8(void);
extern "C" void ComWrappersSubjects_ComWrappersSubjects_Subject_9(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[11] = {
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
	::ChaosAbiMethodEntryV0 entries[10];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		10u,
		0u,
		3120489557u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // ComWrappersSubjects_ComWrappersSubjects_Subject_0
		{ 0u, 0u },  // ComWrappersSubjects_ComWrappersSubjects_Subject_1
		{ 0u, 0u },  // ComWrappersSubjects_ComWrappersSubjects_Subject_2
		{ 0u, 0u },  // ComWrappersSubjects_ComWrappersSubjects_Subject_3
		{ 0u, 0u },  // ComWrappersSubjects_ComWrappersSubjects_Subject_4
		{ 0u, 0u },  // ComWrappersSubjects_ComWrappersSubjects_Subject_5
		{ 0u, 0u },  // ComWrappersSubjects_ComWrappersSubjects_Subject_6
		{ 0u, 0u },  // ComWrappersSubjects_ComWrappersSubjects_Subject_7
		{ 0u, 0u },  // ComWrappersSubjects_ComWrappersSubjects_Subject_8
		{ 0u, 0u },  // ComWrappersSubjects_ComWrappersSubjects_Subject_9
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
		"ComWrappersSubjects",
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
		&chaos_mt_ComWrappersSubjects_ComWrappersSubjects.hot,
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
		/* .name_utf8         = */ "ComWrappersSubjects",
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
		::chaos::il2cpp::runtime_core::RegisterModule("ComWrappersSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[10] = {
	{ "Subject_0", 0x00000003u, 0u },  // ComWrappersSubjects
	{ "Subject_1", 0x00000004u, 0u },  // ComWrappersSubjects
	{ "Subject_2", 0x00000005u, 0u },  // ComWrappersSubjects
	{ "Subject_3", 0x00000006u, 0u },  // ComWrappersSubjects
	{ "Subject_4", 0x00000007u, 0u },  // ComWrappersSubjects
	{ "Subject_5", 0x00000008u, 0u },  // ComWrappersSubjects
	{ "Subject_6", 0x00000009u, 0u },  // ComWrappersSubjects
	{ "Subject_7", 0x0000000Au, 0u },  // ComWrappersSubjects
	{ "Subject_8", 0x0000000Bu, 0u },  // ComWrappersSubjects
	{ "Subject_9", 0x0000000Cu, 0u },  // ComWrappersSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "ComWrappersSubjects", "", 0u, 10u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[10] = {
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
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[10] = {
	{ reinterpret_cast<void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ComWrappersSubjects::Subject_0
	{ reinterpret_cast<void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ComWrappersSubjects::Subject_1
	{ reinterpret_cast<void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ComWrappersSubjects::Subject_2
	{ reinterpret_cast<void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ComWrappersSubjects::Subject_3
	{ reinterpret_cast<void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ComWrappersSubjects::Subject_4
	{ reinterpret_cast<void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ComWrappersSubjects::Subject_5
	{ reinterpret_cast<void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ComWrappersSubjects::Subject_6
	{ reinterpret_cast<void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ComWrappersSubjects::Subject_7
	{ reinterpret_cast<void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ComWrappersSubjects::Subject_8
	{ reinterpret_cast<void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ComWrappersSubjects::Subject_9
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"ComWrappersSubjects",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	10u,
	s_hotpatch_slots,
	10u,
	s_hotpatch_entries,
	10u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[13] = {
	"System.Runtime.InteropServices/ComWrappers::GetIUnknownImpl:System.Void(System.IntPtr&,System.IntPtr&,System.IntPtr&)",
	"System.Private.CoreLib/System.Int32",
	"System.Runtime.InteropServices/ComWrappers::GetOrCreateComInterfaceForObject:System.IntPtr(System.Object,CreateComInterfaceFlags)",
	"ComWrappersSubjects/ComWrappersSubjects::_exitCode",
	"System.Private.CoreLib/System.IntPtr::Zero",
	"System.Runtime.InteropServices/ComWrappers::GetOrCreateObjectForComInstance:System.Object(System.IntPtr,CreateObjectFlags)",
	"System.Runtime.InteropServices/ComWrappers::GetOrCreateObjectForComInstance:System.Object(System.IntPtr,CreateObjectFlags,System.Object)",
	"System.Runtime.InteropServices/ComWrappers::GetOrRegisterObjectForComInstance:System.Object(System.IntPtr,CreateObjectFlags,System.Object)",
	"System.Runtime.InteropServices/ComWrappers::GetOrRegisterObjectForComInstance:System.Object(System.IntPtr,CreateObjectFlags,System.Object,System.IntPtr)",
	"System.Runtime.InteropServices/ComWrappers::RegisterForMarshalling:System.Void(ComWrappers)",
	"System.Runtime.InteropServices/ComWrappers::RegisterForTrackerSupport:System.Void(ComWrappers)",
	"System.Runtime.InteropServices/ComWrappers::TryGetComInstance:System.Boolean(System.Object,System.IntPtr&)",
	"System.Runtime.InteropServices/ComWrappers::TryGetObject:System.Boolean(System.IntPtr,System.Object&)",
};

extern "C" void* kChaosExternalRuntimeFnTable[13] = {
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

extern "C" int32_t kChaosExternalRuntimeCount = 13;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[10])() = {
	reinterpret_cast<void(*)()>(&ComWrappersSubjects_ComWrappersSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&ComWrappersSubjects_ComWrappersSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&ComWrappersSubjects_ComWrappersSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&ComWrappersSubjects_ComWrappersSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&ComWrappersSubjects_ComWrappersSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&ComWrappersSubjects_ComWrappersSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&ComWrappersSubjects_ComWrappersSubjects_Subject_6),
	reinterpret_cast<void(*)()>(&ComWrappersSubjects_ComWrappersSubjects_Subject_7),
	reinterpret_cast<void(*)()>(&ComWrappersSubjects_ComWrappersSubjects_Subject_8),
	reinterpret_cast<void(*)()>(&ComWrappersSubjects_ComWrappersSubjects_Subject_9),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[10])() = {
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
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 10;
extern "C" const int kSubjectEntryIndices[10] = {
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

	/* ── Entry 0: ComWrappersSubjects_ComWrappersSubjects_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry0;
	/* ── Entry 1: ComWrappersSubjects_ComWrappersSubjects_Subject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry1;
	/* ── Entry 2: ComWrappersSubjects_ComWrappersSubjects_Subject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry2;
	/* ── Entry 3: ComWrappersSubjects_ComWrappersSubjects_Subject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry3;
	/* ── Entry 4: ComWrappersSubjects_ComWrappersSubjects_Subject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry4;
	/* ── Entry 5: ComWrappersSubjects_ComWrappersSubjects_Subject_5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry5;
	/* ── Entry 6: ComWrappersSubjects_ComWrappersSubjects_Subject_6 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry6;
	/* ── Entry 7: ComWrappersSubjects_ComWrappersSubjects_Subject_7 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry7;
	/* ── Entry 8: ComWrappersSubjects_ComWrappersSubjects_Subject_8 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry8;
	/* ── Entry 9: ComWrappersSubjects_ComWrappersSubjects_Subject_9 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry9;
} kChaosGcSlotMapsSection = {
	/* entry0 = ComWrappersSubjects_ComWrappersSubjects_Subject_0 */
	.entry0 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_0),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry1 = ComWrappersSubjects_ComWrappersSubjects_Subject_1 */
	.entry1 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_1),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry2 = ComWrappersSubjects_ComWrappersSubjects_Subject_2 */
	.entry2 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_2),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry3 = ComWrappersSubjects_ComWrappersSubjects_Subject_3 */
	.entry3 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_3),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry4 = ComWrappersSubjects_ComWrappersSubjects_Subject_4 */
	.entry4 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_4),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry5 = ComWrappersSubjects_ComWrappersSubjects_Subject_5 */
	.entry5 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_5),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry6 = ComWrappersSubjects_ComWrappersSubjects_Subject_6 */
	.entry6 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_6),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry7 = ComWrappersSubjects_ComWrappersSubjects_Subject_7 */
	.entry7 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_7),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry8 = ComWrappersSubjects_ComWrappersSubjects_Subject_8 */
	.entry8 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_8),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry9 = ComWrappersSubjects_ComWrappersSubjects_Subject_9 */
	.entry9 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_9),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 360u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[10] = {
	reinterpret_cast<void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_0),
	reinterpret_cast<void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_1),
	reinterpret_cast<void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_2),
	reinterpret_cast<void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_3),
	reinterpret_cast<void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_4),
	reinterpret_cast<void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_5),
	reinterpret_cast<void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_6),
	reinterpret_cast<void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_7),
	reinterpret_cast<void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_8),
	reinterpret_cast<void*>(&ComWrappersSubjects_ComWrappersSubjects_Subject_9),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 10u,
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
	.image_name_utf8    = "ComWrappersSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_ComWrappersSubjects_ComWrappersSubjects[1] = {
	{ 0u, "ComWrappersSubjects/ComWrappersSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_ComWrappersSubjects_ComWrappersSubjects[10] = {
	{ 0u, "ComWrappersSubjects/ComWrappersSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ComWrappersSubjects/ComWrappersSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "ComWrappersSubjects/ComWrappersSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "ComWrappersSubjects/ComWrappersSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "ComWrappersSubjects/ComWrappersSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "ComWrappersSubjects/ComWrappersSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "ComWrappersSubjects/ComWrappersSubjects::Subject_6:System.Void()", "Subject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "ComWrappersSubjects/ComWrappersSubjects::Subject_7:System.Void()", "Subject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "ComWrappersSubjects/ComWrappersSubjects::Subject_8:System.Void()", "Subject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "ComWrappersSubjects/ComWrappersSubjects::Subject_9:System.Void()", "Subject_9", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0u, "ComWrappersSubjects/ComWrappersSubjects", "ComWrappersSubjects/ComWrappersSubjects", "", "ComWrappersSubjects", "ComWrappersSubjects", nullptr, kReflFields_ComWrappersSubjects_ComWrappersSubjects, 1u, nullptr, 0u,
	kReflMethods_ComWrappersSubjects_ComWrappersSubjects, 10u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "ComWrappersSubjects", kReflTypePtrs, 1u };

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
// Managed method: ComWrappersSubjects/ComWrappersSubjects::Subject_0()
extern "C" void ComWrappersSubjects_ComWrappersSubjects_Subject_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[2]);
	{
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[0])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	return;
}

// Managed method: ComWrappersSubjects/ComWrappersSubjects::Subject_1()
extern "C" void ComWrappersSubjects_ComWrappersSubjects_Subject_1(void)
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


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_value = _s1;
				auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_System_Private_CoreLib_System_Int32, {});
				chaos_boxed->header.type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
				chaos_boxed->value = chaos_value;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[2])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_ComWrappersSubjects_ComWrappersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
	CHAOS_EH_END
	return;
}

// Managed method: ComWrappersSubjects/ComWrappersSubjects::Subject_2()
extern "C" void ComWrappersSubjects_ComWrappersSubjects_Subject_2(void)
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
			_s0 = 0;
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[5])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_ComWrappersSubjects_ComWrappersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
	CHAOS_EH_END
	return;
}

// Managed method: ComWrappersSubjects/ComWrappersSubjects::Subject_3()
extern "C" void ComWrappersSubjects_ComWrappersSubjects_Subject_3(void)
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
			_s0 = 0;
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_value = _s3;
				auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_System_Private_CoreLib_System_Int32, {});
				chaos_boxed->header.type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
				chaos_boxed->value = chaos_value;
				_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
			}
			{
				const auto chaos_arg_2 = _s3;
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[6])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_ComWrappersSubjects_ComWrappersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
	CHAOS_EH_END
	return;
}

// Managed method: ComWrappersSubjects/ComWrappersSubjects::Subject_4()
extern "C" void ComWrappersSubjects_ComWrappersSubjects_Subject_4(void)
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
			_s0 = 0;
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_value = _s3;
				auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_System_Private_CoreLib_System_Int32, {});
				chaos_boxed->header.type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
				chaos_boxed->value = chaos_value;
				_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
			}
			{
				const auto chaos_arg_2 = _s3;
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[7])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_ComWrappersSubjects_ComWrappersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
	CHAOS_EH_END
	return;
}

// Managed method: ComWrappersSubjects/ComWrappersSubjects::Subject_5()
extern "C" void ComWrappersSubjects_ComWrappersSubjects_Subject_5(void)
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


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_value = _s3;
				auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_System_Private_CoreLib_System_Int32, {});
				chaos_boxed->header.type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
				chaos_boxed->value = chaos_value;
				_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
			}
			_s4 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_3 = _s4;
				const auto chaos_arg_2 = _s3;
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[8])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_ComWrappersSubjects_ComWrappersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
	CHAOS_EH_END
	return;
}

// Managed method: ComWrappersSubjects/ComWrappersSubjects::Subject_6()
extern "C" void ComWrappersSubjects_ComWrappersSubjects_Subject_6(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[9])(chaos_arg_0);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ComWrappersSubjects_ComWrappersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_ArgumentNullException.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
	CHAOS_EH_END
	return;
}

// Managed method: ComWrappersSubjects/ComWrappersSubjects::Subject_7()
extern "C" void ComWrappersSubjects_ComWrappersSubjects_Subject_7(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[10])(chaos_arg_0);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ComWrappersSubjects_ComWrappersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_ArgumentNullException.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
	CHAOS_EH_END
	return;
}

// Managed method: ComWrappersSubjects/ComWrappersSubjects::Subject_8()
extern "C" void ComWrappersSubjects_ComWrappersSubjects_Subject_8(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_value = _s0;
		auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_System_Private_CoreLib_System_Int32, {});
		chaos_boxed->header.type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
		chaos_boxed->value = chaos_value;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
	}
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[11])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
		{
			if (_s0 != 0)
			{
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
				{
					auto chaos_value = _s0;
					chaos_static_ComWrappersSubjects_ComWrappersSubjects___exitCode = chaos_value;
				}
			}
			return;
		}
	}
}

// Managed method: ComWrappersSubjects/ComWrappersSubjects::Subject_9()
extern "C" void ComWrappersSubjects_ComWrappersSubjects_Subject_9(void)
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


	_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[12])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
		{
			if (_s0 != 0)
			{
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
				{
					auto chaos_value = _s0;
					chaos_static_ComWrappersSubjects_ComWrappersSubjects___exitCode = chaos_value;
				}
			}
			return;
		}
	}
}



}  // namespace chaos::il2cpp::codegen::ComWrappersSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 10;

// ── JIT Method Entry Table ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kMethodJson_0[] = "{\"methodId\":\"com-wrappers-subjects.com-wrappers-subjects.subject-0\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_0:System.Void()\",\"signature\":\"System.Void ComWrappersSubjects::Subject_0()\",\"identity\":{\"assemblyName\":\"ComWrappersSubjects\",\"declaringTypeSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects\",\"definitionSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_0:System.Void()\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_0:System.Void()\",\"methodId\":\"com-wrappers-subjects.com-wrappers-subjects.subject-0\",\"signature\":\"System.Void ComWrappersSubjects::Subject_0()\"},\"nativeSymbol\":\"ComWrappersSubjects_ComWrappersSubjects_Subject_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":3,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":3,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":2,\"ilOffset\":5,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Void\",\"callee\":\"System.Runtime.InteropServices/ComWrappers::GetIUnknownImpl:System.Void(System.IntPtr\\u0026,System.IntPtr\\u0026,System.IntPtr\\u0026)\",\"reference\":{\"assemblyName\":\"System.Runtime.InteropServices\",\"subjectKind\":\"method\",\"subjectId\":\"System.Runtime.InteropServices/ComWrappers::GetIUnknownImpl:System.Void(System.IntPtr\\u0026,System.IntPtr\\u0026,System.IntPtr\\u0026)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Runtime.InteropServices\",\"subjectId\":\"System.Runtime.InteropServices/ComWrappers::GetIUnknownImpl:System.Void(System.IntPtr\\u0026,System.IntPtr\\u0026,System.IntPtr\\u0026)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":13,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ComWrappersSubjects\"}";
static const char kMethodJson_1[] = "{\"methodId\":\"com-wrappers-subjects.com-wrappers-subjects.subject-1\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_1:System.Void()\",\"signature\":\"System.Void ComWrappersSubjects::Subject_1()\",\"identity\":{\"assemblyName\":\"ComWrappersSubjects\",\"declaringTypeSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects\",\"definitionSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_1:System.Void()\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_1:System.Void()\",\"methodId\":\"com-wrappers-subjects.com-wrappers-subjects.subject-1\",\"signature\":\"System.Void ComWrappersSubjects::Subject_1()\"},\"nativeSymbol\":\"ComWrappersSubjects_ComWrappersSubjects_Subject_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":25,\"handlerOffset\":26,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"box\",\"opCode\":35,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":5,\"resultType\":\"System.Object\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":9,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":11,\"resultType\":\"System.IntPtr\",\"callee\":\"System.Runtime.InteropServices/ComWrappers::GetOrCreateComInterfaceForObject:System.IntPtr(System.Object,CreateComInterfaceFlags)\",\"reference\":{\"assemblyName\":\"System.Runtime.InteropServices\",\"subjectKind\":\"method\",\"subjectId\":\"System.Runtime.InteropServices/ComWrappers::GetOrCreateComInterfaceForObject:System.IntPtr(System.Object,CreateComInterfaceFlags)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Runtime.InteropServices\",\"subjectId\":\"System.Runtime.InteropServices/ComWrappers::GetOrCreateComInterfaceForObject:System.IntPtr(System.Object,CreateComInterfaceFlags)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":16,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\",\"ilOffset\":18,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ComWrappersSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ComWrappersSubjects\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":31,\"ilOffset\":24,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":31,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":31,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ComWrappersSubjects\"}";
static const char kMethodJson_2[] = "{\"methodId\":\"com-wrappers-subjects.com-wrappers-subjects.subject-2\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_2:System.Void()\",\"signature\":\"System.Void ComWrappersSubjects::Subject_2()\",\"identity\":{\"assemblyName\":\"ComWrappersSubjects\",\"declaringTypeSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects\",\"definitionSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_2:System.Void()\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_2:System.Void()\",\"methodId\":\"com-wrappers-subjects.com-wrappers-subjects.subject-2\",\"signature\":\"System.Void ComWrappersSubjects::Subject_2()\"},\"nativeSymbol\":\"ComWrappersSubjects_ComWrappersSubjects_Subject_2\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":23,\"handlerOffset\":24,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"System.Private.CoreLib/System.IntPtr::Zero\",\"ilOffset\":3,\"resultType\":\"System.IntPtr\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"field\",\"subjectId\":\"System.Private.CoreLib/System.IntPtr::Zero\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IntPtr::Zero\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"System.Private.CoreLib/System.IntPtr\",\"declaringTypeShape\":1},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":9,\"resultType\":\"System.Object\",\"callee\":\"System.Runtime.InteropServices/ComWrappers::GetOrCreateObjectForComInstance:System.Object(System.IntPtr,CreateObjectFlags)\",\"reference\":{\"assemblyName\":\"System.Runtime.InteropServices\",\"subjectKind\":\"method\",\"subjectId\":\"System.Runtime.InteropServices/ComWrappers::GetOrCreateObjectForComInstance:System.Object(System.IntPtr,CreateObjectFlags)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Runtime.InteropServices\",\"subjectId\":\"System.Runtime.InteropServices/ComWrappers::GetOrCreateObjectForComInstance:System.Object(System.IntPtr,CreateObjectFlags)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ComWrappersSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ComWrappersSubjects\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":29,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":24,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":29,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":29,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ComWrappersSubjects\"}";
static const char kMethodJson_3[] = "{\"methodId\":\"com-wrappers-subjects.com-wrappers-subjects.subject-3\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_3:System.Void()\",\"signature\":\"System.Void ComWrappersSubjects::Subject_3()\",\"identity\":{\"assemblyName\":\"ComWrappersSubjects\",\"declaringTypeSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects\",\"definitionSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_3:System.Void()\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_3:System.Void()\",\"methodId\":\"com-wrappers-subjects.com-wrappers-subjects.subject-3\",\"signature\":\"System.Void ComWrappersSubjects::Subject_3()\"},\"nativeSymbol\":\"ComWrappersSubjects_ComWrappersSubjects_Subject_3\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":30,\"handlerOffset\":31,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"System.Private.CoreLib/System.IntPtr::Zero\",\"ilOffset\":3,\"resultType\":\"System.IntPtr\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"field\",\"subjectId\":\"System.Private.CoreLib/System.IntPtr::Zero\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IntPtr::Zero\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"System.Private.CoreLib/System.IntPtr\",\"declaringTypeShape\":1},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"box\",\"opCode\":35,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":11,\"resultType\":\"System.Object\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":9,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":16,\"resultType\":\"System.Object\",\"callee\":\"System.Runtime.InteropServices/ComWrappers::GetOrCreateObjectForComInstance:System.Object(System.IntPtr,CreateObjectFlags,System.Object)\",\"reference\":{\"assemblyName\":\"System.Runtime.InteropServices\",\"subjectKind\":\"method\",\"subjectId\":\"System.Runtime.InteropServices/ComWrappers::GetOrCreateObjectForComInstance:System.Object(System.IntPtr,CreateObjectFlags,System.Object)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Runtime.InteropServices\",\"subjectId\":\"System.Runtime.InteropServices/ComWrappers::GetOrCreateObjectForComInstance:System.Object(System.IntPtr,CreateObjectFlags,System.Object)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":21,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\",\"ilOffset\":23,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ComWrappersSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ComWrappersSubjects\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":36,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":31,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":36,\"ilOffset\":34,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":36,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ComWrappersSubjects\"}";
static const char kMethodJson_4[] = "{\"methodId\":\"com-wrappers-subjects.com-wrappers-subjects.subject-4\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_4:System.Void()\",\"signature\":\"System.Void ComWrappersSubjects::Subject_4()\",\"identity\":{\"assemblyName\":\"ComWrappersSubjects\",\"declaringTypeSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects\",\"definitionSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_4:System.Void()\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_4:System.Void()\",\"methodId\":\"com-wrappers-subjects.com-wrappers-subjects.subject-4\",\"signature\":\"System.Void ComWrappersSubjects::Subject_4()\"},\"nativeSymbol\":\"ComWrappersSubjects_ComWrappersSubjects_Subject_4\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":30,\"handlerOffset\":31,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"System.Private.CoreLib/System.IntPtr::Zero\",\"ilOffset\":3,\"resultType\":\"System.IntPtr\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"field\",\"subjectId\":\"System.Private.CoreLib/System.IntPtr::Zero\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IntPtr::Zero\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"System.Private.CoreLib/System.IntPtr\",\"declaringTypeShape\":1},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"box\",\"opCode\":35,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":11,\"resultType\":\"System.Object\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":9,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":16,\"resultType\":\"System.Object\",\"callee\":\"System.Runtime.InteropServices/ComWrappers::GetOrRegisterObjectForComInstance:System.Object(System.IntPtr,CreateObjectFlags,System.Object)\",\"reference\":{\"assemblyName\":\"System.Runtime.InteropServices\",\"subjectKind\":\"method\",\"subjectId\":\"System.Runtime.InteropServices/ComWrappers::GetOrRegisterObjectForComInstance:System.Object(System.IntPtr,CreateObjectFlags,System.Object)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Runtime.InteropServices\",\"subjectId\":\"System.Runtime.InteropServices/ComWrappers::GetOrRegisterObjectForComInstance:System.Object(System.IntPtr,CreateObjectFlags,System.Object)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":21,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\",\"ilOffset\":23,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ComWrappersSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ComWrappersSubjects\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":36,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":31,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":36,\"ilOffset\":34,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":36,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ComWrappersSubjects\"}";
static const char kMethodJson_5[] = "{\"methodId\":\"com-wrappers-subjects.com-wrappers-subjects.subject-5\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_5:System.Void()\",\"signature\":\"System.Void ComWrappersSubjects::Subject_5()\",\"identity\":{\"assemblyName\":\"ComWrappersSubjects\",\"declaringTypeSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects\",\"definitionSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_5:System.Void()\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_5:System.Void()\",\"methodId\":\"com-wrappers-subjects.com-wrappers-subjects.subject-5\",\"signature\":\"System.Void ComWrappersSubjects::Subject_5()\"},\"nativeSymbol\":\"ComWrappersSubjects_ComWrappersSubjects_Subject_5\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":35,\"handlerOffset\":36,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"System.Private.CoreLib/System.IntPtr::Zero\",\"ilOffset\":3,\"resultType\":\"System.IntPtr\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"field\",\"subjectId\":\"System.Private.CoreLib/System.IntPtr::Zero\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IntPtr::Zero\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"System.Private.CoreLib/System.IntPtr\",\"declaringTypeShape\":1},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"box\",\"opCode\":35,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":11,\"resultType\":\"System.Object\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":9,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"System.Private.CoreLib/System.IntPtr::Zero\",\"ilOffset\":16,\"resultType\":\"System.IntPtr\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"field\",\"subjectId\":\"System.Private.CoreLib/System.IntPtr::Zero\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IntPtr::Zero\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"System.Private.CoreLib/System.IntPtr\",\"declaringTypeShape\":1},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":21,\"resultType\":\"System.Object\",\"callee\":\"System.Runtime.InteropServices/ComWrappers::GetOrRegisterObjectForComInstance:System.Object(System.IntPtr,CreateObjectFlags,System.Object,System.IntPtr)\",\"reference\":{\"assemblyName\":\"System.Runtime.InteropServices\",\"subjectKind\":\"method\",\"subjectId\":\"System.Runtime.InteropServices/ComWrappers::GetOrRegisterObjectForComInstance:System.Object(System.IntPtr,CreateObjectFlags,System.Object,System.IntPtr)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Runtime.InteropServices\",\"subjectId\":\"System.Runtime.InteropServices/ComWrappers::GetOrRegisterObjectForComInstance:System.Object(System.IntPtr,CreateObjectFlags,System.Object,System.IntPtr)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\",\"ilOffset\":28,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ComWrappersSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ComWrappersSubjects\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":34,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":36,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ComWrappersSubjects\"}";
static const char kMethodJson_6[] = "{\"methodId\":\"com-wrappers-subjects.com-wrappers-subjects.subject-6\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_6:System.Void()\",\"signature\":\"System.Void ComWrappersSubjects::Subject_6()\",\"identity\":{\"assemblyName\":\"ComWrappersSubjects\",\"declaringTypeSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects\",\"definitionSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_6:System.Void()\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_6:System.Void()\",\"methodId\":\"com-wrappers-subjects.com-wrappers-subjects.subject-6\",\"signature\":\"System.Void ComWrappersSubjects::Subject_6()\"},\"nativeSymbol\":\"ComWrappersSubjects_ComWrappersSubjects_Subject_6\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":17,\"handlerOffset\":18,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.ArgumentNullException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":3,\"resultType\":\"System.Void\",\"callee\":\"System.Runtime.InteropServices/ComWrappers::RegisterForMarshalling:System.Void(ComWrappers)\",\"reference\":{\"assemblyName\":\"System.Runtime.InteropServices\",\"subjectKind\":\"method\",\"subjectId\":\"System.Runtime.InteropServices/ComWrappers::RegisterForMarshalling:System.Void(ComWrappers)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Runtime.InteropServices\",\"subjectId\":\"System.Runtime.InteropServices/ComWrappers::RegisterForMarshalling:System.Void(ComWrappers)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\",\"ilOffset\":10,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ComWrappersSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ComWrappersSubjects\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ComWrappersSubjects\"}";
static const char kMethodJson_7[] = "{\"methodId\":\"com-wrappers-subjects.com-wrappers-subjects.subject-7\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_7:System.Void()\",\"signature\":\"System.Void ComWrappersSubjects::Subject_7()\",\"identity\":{\"assemblyName\":\"ComWrappersSubjects\",\"declaringTypeSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects\",\"definitionSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_7:System.Void()\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_7:System.Void()\",\"methodId\":\"com-wrappers-subjects.com-wrappers-subjects.subject-7\",\"signature\":\"System.Void ComWrappersSubjects::Subject_7()\"},\"nativeSymbol\":\"ComWrappersSubjects_ComWrappersSubjects_Subject_7\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":17,\"handlerOffset\":18,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.ArgumentNullException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":3,\"resultType\":\"System.Void\",\"callee\":\"System.Runtime.InteropServices/ComWrappers::RegisterForTrackerSupport:System.Void(ComWrappers)\",\"reference\":{\"assemblyName\":\"System.Runtime.InteropServices\",\"subjectKind\":\"method\",\"subjectId\":\"System.Runtime.InteropServices/ComWrappers::RegisterForTrackerSupport:System.Void(ComWrappers)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Runtime.InteropServices\",\"subjectId\":\"System.Runtime.InteropServices/ComWrappers::RegisterForTrackerSupport:System.Void(ComWrappers)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\",\"ilOffset\":10,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ComWrappersSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ComWrappersSubjects\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ComWrappersSubjects\"}";
static const char kMethodJson_8[] = "{\"methodId\":\"com-wrappers-subjects.com-wrappers-subjects.subject-8\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_8:System.Void()\",\"signature\":\"System.Void ComWrappersSubjects::Subject_8()\",\"identity\":{\"assemblyName\":\"ComWrappersSubjects\",\"declaringTypeSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects\",\"definitionSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_8:System.Void()\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_8:System.Void()\",\"methodId\":\"com-wrappers-subjects.com-wrappers-subjects.subject-8\",\"signature\":\"System.Void ComWrappersSubjects::Subject_8()\"},\"nativeSymbol\":\"ComWrappersSubjects_ComWrappersSubjects_Subject_8\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"box\",\"opCode\":35,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":3,\"resultType\":\"System.Object\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":9,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":8,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":10,\"resultType\":\"System.Boolean\",\"callee\":\"System.Runtime.InteropServices/ComWrappers::TryGetComInstance:System.Boolean(System.Object,System.IntPtr\\u0026)\",\"reference\":{\"assemblyName\":\"System.Runtime.InteropServices\",\"subjectKind\":\"method\",\"subjectId\":\"System.Runtime.InteropServices/ComWrappers::TryGetComInstance:System.Boolean(System.Object,System.IntPtr\\u0026)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Runtime.InteropServices\",\"subjectId\":\"System.Runtime.InteropServices/ComWrappers::TryGetComInstance:System.Boolean(System.Object,System.IntPtr\\u0026)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":20,\"ilOffset\":15,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":21,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":24,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":25,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":34,\"ilOffset\":26,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\",\"ilOffset\":29,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ComWrappersSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ComWrappersSubjects\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":34,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ComWrappersSubjects\"}";
static const char kMethodJson_9[] = "{\"methodId\":\"com-wrappers-subjects.com-wrappers-subjects.subject-9\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_9:System.Void()\",\"signature\":\"System.Void ComWrappersSubjects::Subject_9()\",\"identity\":{\"assemblyName\":\"ComWrappersSubjects\",\"declaringTypeSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects\",\"definitionSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_9:System.Void()\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::Subject_9:System.Void()\",\"methodId\":\"com-wrappers-subjects.com-wrappers-subjects.subject-9\",\"signature\":\"System.Void ComWrappersSubjects::Subject_9()\"},\"nativeSymbol\":\"ComWrappersSubjects_ComWrappersSubjects_Subject_9\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"System.Private.CoreLib/System.IntPtr::Zero\",\"ilOffset\":1,\"resultType\":\"System.IntPtr\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"field\",\"subjectId\":\"System.Private.CoreLib/System.IntPtr::Zero\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IntPtr::Zero\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"System.Private.CoreLib/System.IntPtr\",\"declaringTypeShape\":1},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":6,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":8,\"resultType\":\"System.Boolean\",\"callee\":\"System.Runtime.InteropServices/ComWrappers::TryGetObject:System.Boolean(System.IntPtr,System.Object\\u0026)\",\"reference\":{\"assemblyName\":\"System.Runtime.InteropServices\",\"subjectKind\":\"method\",\"subjectId\":\"System.Runtime.InteropServices/ComWrappers::TryGetObject:System.Boolean(System.IntPtr,System.Object\\u0026)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Runtime.InteropServices\",\"subjectId\":\"System.Runtime.InteropServices/ComWrappers::TryGetObject:System.Boolean(System.IntPtr,System.Object\\u0026)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":18,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":19,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":23,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":32,\"ilOffset\":24,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\",\"ilOffset\":27,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ComWrappersSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ComWrappersSubjects\",\"subjectId\":\"ComWrappersSubjects/ComWrappersSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ComWrappersSubjects/ComWrappersSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":32,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ComWrappersSubjects\"}";

extern "C" const JitMethodEntry kChaosJitMethodEntries[10] =
{
    { kMethodJson_0, 2354u, 0x00000003u, 0u },
    { kMethodJson_1, 4135u, 0x00000004u, 0u },
    { kMethodJson_2, 4098u, 0x00000005u, 0u },
    { kMethodJson_3, 4774u, 0x00000006u, 0u },
    { kMethodJson_4, 4780u, 0x00000007u, 0u },
    { kMethodJson_5, 5441u, 0x00000008u, 0u },
    { kMethodJson_6, 3216u, 0x00000009u, 0u },
    { kMethodJson_7, 3225u, 0x0000000Au, 0u },
    { kMethodJson_8, 4381u, 0x0000000Bu, 0u },
    { kMethodJson_9, 4351u, 0x0000000Cu, 0u }
};

extern "C" const uint32_t kChaosJitMethodEntryCount = 10u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitMethods(kChaosJitMethodEntries, kChaosJitMethodEntryCount);
}