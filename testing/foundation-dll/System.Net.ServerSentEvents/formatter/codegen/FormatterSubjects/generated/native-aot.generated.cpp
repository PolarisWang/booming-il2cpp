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

namespace chaos::il2cpp::codegen::FormatterSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_FormatterSubjects_FormatterSubjects = {{nullptr, nullptr, 6299927154239617552ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_FormatterSubjects_FormatterSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(6299927154239617552ULL);
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

struct chaos_type_FormatterSubjects_FormatterSubjects
{
	ThinLockableHeader header{};
};

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

CHAOS_IL2CPP_INT32 chaos_static_FormatterSubjects_FormatterSubjects___exitCode = 0;

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

extern "C" void FormatterSubjects_FormatterSubjects_Subject_0(void);
extern "C" void FormatterSubjects_FormatterSubjects_CustomEntrySubject_1(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[3] = {
	0u,
	0u,
	0u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[2];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		2u,
		0u,
		1268118805u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // FormatterSubjects_FormatterSubjects_Subject_0
		{ 0u, 0u },  // FormatterSubjects_FormatterSubjects_CustomEntrySubject_1
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
		"FormatterSubjects",
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
		&chaos_mt_FormatterSubjects_FormatterSubjects.hot,
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
		/* .name_utf8         = */ "FormatterSubjects",
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
		::chaos::il2cpp::runtime_core::RegisterModule("FormatterSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[2] = {
	{ "Subject_0", 0x00000003u, 0u },  // FormatterSubjects
	{ "CustomEntrySubject_1", 0x00000004u, 0u },  // FormatterSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "FormatterSubjects", "", 0u, 2u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[2] = {
	{ 0x00000003u, 0u },
	{ 0x00000004u, 1u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[2] = {
	{ reinterpret_cast<void*>(&FormatterSubjects_FormatterSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // FormatterSubjects::Subject_0
	{ reinterpret_cast<void*>(&FormatterSubjects_FormatterSubjects_CustomEntrySubject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // FormatterSubjects::CustomEntrySubject_1
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"FormatterSubjects",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	2u,
	s_hotpatch_slots,
	2u,
	s_hotpatch_entries,
	2u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[5] = {
	"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()",
	"System.Private.CoreLib/System.Threading.CancellationToken",
	"System.Net.ServerSentEvents/SseFormatter::WriteAsync:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable<SseItem<System.String>>,System.IO.Stream,System.Threading.CancellationToken)",
	"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()",
	"FormatterSubjects/FormatterSubjects::_exitCode",
};

extern "C" void* kChaosExternalRuntimeFnTable[5] = {
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_IO_MemoryStream___ctor_System_Void__),
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__),
	nullptr,
};

extern "C" int32_t kChaosExternalRuntimeCount = 5;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[2])() = {
	reinterpret_cast<void(*)()>(&FormatterSubjects_FormatterSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&FormatterSubjects_FormatterSubjects_CustomEntrySubject_1),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[2])() = {
	[]() {kAotMethods[0]();},
	[]() {kAotMethods[1]();},
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 2;
extern "C" const int kSubjectEntryIndices[2] = {
	0,
	1,
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

	/* ── Entry 0: FormatterSubjects_FormatterSubjects_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry0;
} kChaosGcSlotMapsSection = {
	/* entry0 = FormatterSubjects_FormatterSubjects_Subject_0 */
	.entry0 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&FormatterSubjects_FormatterSubjects_Subject_0),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 44u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[2] = {
	reinterpret_cast<void*>(&FormatterSubjects_FormatterSubjects_Subject_0),
	reinterpret_cast<void*>(&FormatterSubjects_FormatterSubjects_CustomEntrySubject_1),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 2u,
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
	.image_name_utf8    = "FormatterSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_FormatterSubjects_FormatterSubjects[1] = {
	{ 0u, "FormatterSubjects/FormatterSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_FormatterSubjects_FormatterSubjects[2] = {
	{ 0u, "FormatterSubjects/FormatterSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "FormatterSubjects/FormatterSubjects::CustomEntrySubject_1:System.Void()", "CustomEntrySubject_1", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0u, "FormatterSubjects/FormatterSubjects", "FormatterSubjects/FormatterSubjects", "", "FormatterSubjects", "FormatterSubjects", nullptr, kReflFields_FormatterSubjects_FormatterSubjects, 1u, nullptr, 0u,
	kReflMethods_FormatterSubjects_FormatterSubjects, 2u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "FormatterSubjects", kReflTypePtrs, 1u };

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
// Managed method: FormatterSubjects/FormatterSubjects::Subject_0()
extern "C" void FormatterSubjects_FormatterSubjects_Subject_0(void)
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
			_s0 = 0;
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[2])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = 0;
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
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[2])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
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
				chaos_static_FormatterSubjects_FormatterSubjects___exitCode = chaos_value;
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
				chaos_static_FormatterSubjects_FormatterSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: FormatterSubjects/FormatterSubjects::CustomEntrySubject_1()
extern "C" void FormatterSubjects_FormatterSubjects_CustomEntrySubject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}



}  // namespace chaos::il2cpp::codegen::FormatterSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 2;

// ── JIT Method Entry Table ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kMethodJson_0[] = "{\"methodId\":\"formatter-subjects.formatter-subjects.subject-0\",\"subjectId\":\"FormatterSubjects/FormatterSubjects::Subject_0:System.Void()\",\"signature\":\"System.Void FormatterSubjects::Subject_0()\",\"identity\":{\"assemblyName\":\"FormatterSubjects\",\"declaringTypeSubjectId\":\"FormatterSubjects/FormatterSubjects\",\"definitionSubjectId\":\"FormatterSubjects/FormatterSubjects::Subject_0:System.Void()\",\"subjectId\":\"FormatterSubjects/FormatterSubjects::Subject_0:System.Void()\",\"methodId\":\"formatter-subjects.formatter-subjects.subject-0\",\"signature\":\"System.Void FormatterSubjects::Subject_0()\"},\"nativeSymbol\":\"FormatterSubjects_FormatterSubjects_Subject_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":69,\"handlerOffset\":70,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":3,\"resultType\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"callee\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":8,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":10,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":16,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":17,\"resultType\":\"System.Threading.Tasks.Task\",\"callee\":\"System.Net.ServerSentEvents/SseFormatter::WriteAsync:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable\\u003CSseItem\\u003CSystem.String\\u003E\\u003E,System.IO.Stream,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.Net.ServerSentEvents\",\"subjectKind\":\"method\",\"subjectId\":\"System.Net.ServerSentEvents/SseFormatter::WriteAsync:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable\\u003CSseItem\\u003CSystem.String\\u003E\\u003E,System.IO.Stream,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Net.ServerSentEvents\",\"subjectId\":\"System.Net.ServerSentEvents/SseFormatter::WriteAsync:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable\\u003CSseItem\\u003CSystem.String\\u003E\\u003E,System.IO.Stream,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":27,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":28,\"resultType\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"callee\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IO.MemoryStream\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":33,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":35,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":41,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":42,\"resultType\":\"System.Threading.Tasks.Task\",\"callee\":\"System.Net.ServerSentEvents/SseFormatter::WriteAsync:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable\\u003CSseItem\\u003CSystem.String\\u003E\\u003E,System.IO.Stream,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.Net.ServerSentEvents\",\"subjectKind\":\"method\",\"subjectId\":\"System.Net.ServerSentEvents/SseFormatter::WriteAsync:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable\\u003CSseItem\\u003CSystem.String\\u003E\\u003E,System.IO.Stream,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Net.ServerSentEvents\",\"subjectId\":\"System.Net.ServerSentEvents/SseFormatter::WriteAsync:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable\\u003CSseItem\\u003CSystem.String\\u003E\\u003E,System.IO.Stream,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":47,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":52,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":54,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":55,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":57,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":58,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":67,\"ilOffset\":59,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":61,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"FormatterSubjects/FormatterSubjects::_exitCode\",\"ilOffset\":62,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"FormatterSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"FormatterSubjects/FormatterSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"FormatterSubjects\",\"subjectId\":\"FormatterSubjects/FormatterSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"FormatterSubjects/FormatterSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":81,\"ilOffset\":68,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":70,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":72,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"FormatterSubjects/FormatterSubjects::_exitCode\",\"ilOffset\":73,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"FormatterSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"FormatterSubjects/FormatterSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"FormatterSubjects\",\"subjectId\":\"FormatterSubjects/FormatterSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"FormatterSubjects/FormatterSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":81,\"ilOffset\":79,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":81,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"FormatterSubjects\"}";
static const char kMethodJson_1[] = "{\"methodId\":\"formatter-subjects.formatter-subjects.custom-entry-subject-1\",\"subjectId\":\"FormatterSubjects/FormatterSubjects::CustomEntrySubject_1:System.Void()\",\"signature\":\"System.Void FormatterSubjects::CustomEntrySubject_1()\",\"identity\":{\"assemblyName\":\"FormatterSubjects\",\"declaringTypeSubjectId\":\"FormatterSubjects/FormatterSubjects\",\"definitionSubjectId\":\"FormatterSubjects/FormatterSubjects::CustomEntrySubject_1:System.Void()\",\"subjectId\":\"FormatterSubjects/FormatterSubjects::CustomEntrySubject_1:System.Void()\",\"methodId\":\"formatter-subjects.formatter-subjects.custom-entry-subject-1\",\"signature\":\"System.Void FormatterSubjects::CustomEntrySubject_1()\"},\"nativeSymbol\":\"FormatterSubjects_FormatterSubjects_CustomEntrySubject_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"FormatterSubjects\"}";

extern "C" const JitMethodEntry kChaosJitMethodEntries[2] =
{
    { kMethodJson_0, 10136u, 0x00000003u, 0u },
    { kMethodJson_1, 1259u, 0x00000004u, 0u }
};

extern "C" const uint32_t kChaosJitMethodEntryCount = 2u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitMethods(kChaosJitMethodEntries, kChaosJitMethodEntryCount);
}