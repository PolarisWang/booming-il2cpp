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

namespace chaos::il2cpp::codegen::ImmutableHashSetSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_ImmutableHashSetSubjects_ImmutableHashSetSubjects = {{nullptr, nullptr, 17995600985302218760ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ImmutableHashSetSubjects_ImmutableHashSetSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(17995600985302218760ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Byte = {{nullptr, nullptr, 3217969471912086765ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Byte = static_cast<CHAOS_IL2CPP_INTPTR>(3217969471912086765ULL);
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

struct chaos_type_ImmutableHashSetSubjects_ImmutableHashSetSubjects
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Byte
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

CHAOS_IL2CPP_INT32 chaos_static_ImmutableHashSetSubjects_ImmutableHashSetSubjects___exitCode = 0;

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_0(void);
extern "C" void ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_1(void);
extern "C" void ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_2(void);
extern "C" void ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_3(void);
extern "C" void ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_4(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[6] = {
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[5];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		5u,
		0u,
		1078699117u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_0
		{ 0u, 0u },  // ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_1
		{ 0u, 0u },  // ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_2
		{ 0u, 0u },  // ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_3
		{ 0u, 0u },  // ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_4
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
		"ImmutableHashSetSubjects",
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
		&chaos_mt_ImmutableHashSetSubjects_ImmutableHashSetSubjects.hot,
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
		/* .name_utf8         = */ "ImmutableHashSetSubjects",
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
		::chaos::il2cpp::runtime_core::RegisterModule("ImmutableHashSetSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[5] = {
	{ "CustomEntrySubject_0", 0x00000003u, 0u },  // ImmutableHashSetSubjects
	{ "CustomEntrySubject_1", 0x00000004u, 0u },  // ImmutableHashSetSubjects
	{ "CustomEntrySubject_2", 0x00000005u, 0u },  // ImmutableHashSetSubjects
	{ "CustomEntrySubject_3", 0x00000006u, 0u },  // ImmutableHashSetSubjects
	{ "CustomEntrySubject_4", 0x00000007u, 0u },  // ImmutableHashSetSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "ImmutableHashSetSubjects", "", 0u, 5u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[5] = {
	{ 0x00000003u, 0u },
	{ 0x00000004u, 1u },
	{ 0x00000005u, 2u },
	{ 0x00000006u, 3u },
	{ 0x00000007u, 4u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[5] = {
	{ reinterpret_cast<void*>(&ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ImmutableHashSetSubjects::CustomEntrySubject_0
	{ reinterpret_cast<void*>(&ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ImmutableHashSetSubjects::CustomEntrySubject_1
	{ reinterpret_cast<void*>(&ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ImmutableHashSetSubjects::CustomEntrySubject_2
	{ reinterpret_cast<void*>(&ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ImmutableHashSetSubjects::CustomEntrySubject_3
	{ reinterpret_cast<void*>(&ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ImmutableHashSetSubjects::CustomEntrySubject_4
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"ImmutableHashSetSubjects",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	5u,
	s_hotpatch_slots,
	5u,
	s_hotpatch_entries,
	5u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[9] = {
	"System.Collections.Immutable/ImmutableHashSet::Create<System.Byte>:ImmutableHashSet<System.Byte>()",
	"System.Collections.Immutable/ImmutableHashSet<System.Byte>::get_Count:System.Int32()",
	"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode",
	"System.Collections.Immutable/ImmutableHashSet::Create<System.Byte>:ImmutableHashSet<System.Byte>(System.Byte)",
	"System.Private.CoreLib/System.Byte",
	"System.Collections.Immutable/ImmutableHashSet::Create<System.Byte>:ImmutableHashSet<System.Byte>(System.Byte[])",
	"System.Collections.Immutable/ImmutableHashSet::Create<System.Int32>:ImmutableHashSet<System.Int32>()",
	"System.Collections.Immutable/ImmutableHashSet<System.Int32>::get_Count:System.Int32()",
	"System.Collections.Immutable/ImmutableHashSet::Create<System.Int32>:ImmutableHashSet<System.Int32>(System.Int32)",
};

extern "C" void* kChaosExternalRuntimeFnTable[9] = {
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

extern "C" int32_t kChaosExternalRuntimeCount = 9;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[5])() = {
	reinterpret_cast<void(*)()>(&ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_0),
	reinterpret_cast<void(*)()>(&ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_1),
	reinterpret_cast<void(*)()>(&ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_2),
	reinterpret_cast<void(*)()>(&ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_3),
	reinterpret_cast<void(*)()>(&ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_4),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[5])() = {
	[]() {kAotMethods[0]();},
	[]() {kAotMethods[1]();},
	[]() {kAotMethods[2]();},
	[]() {kAotMethods[3]();},
	[]() {kAotMethods[4]();},
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 5;
extern "C" const int kSubjectEntryIndices[5] = {
	0,
	1,
	2,
	3,
	4,
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

	/* ── Entry 0: ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry0;
	/* ── Entry 1: ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry1;
	/* ── Entry 2: ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry2;
	/* ── Entry 3: ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry3;
	/* ── Entry 4: ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry4;
} kChaosGcSlotMapsSection = {
	/* entry0 = ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_0 */
	.entry0 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_0),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry1 = ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_1 */
	.entry1 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_1),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry2 = ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_2 */
	.entry2 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_2),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry3 = ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_3 */
	.entry3 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_3),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry4 = ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_4 */
	.entry4 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_4),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 208u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[5] = {
	reinterpret_cast<void*>(&ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_0),
	reinterpret_cast<void*>(&ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_1),
	reinterpret_cast<void*>(&ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_2),
	reinterpret_cast<void*>(&ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_3),
	reinterpret_cast<void*>(&ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_4),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 5u,
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
	.image_name_utf8    = "ImmutableHashSetSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_ImmutableHashSetSubjects_ImmutableHashSetSubjects[1] = {
	{ 0u, "ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_ImmutableHashSetSubjects_ImmutableHashSetSubjects[5] = {
	{ 0u, "ImmutableHashSetSubjects/ImmutableHashSetSubjects::CustomEntrySubject_0:System.Void()", "CustomEntrySubject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ImmutableHashSetSubjects/ImmutableHashSetSubjects::CustomEntrySubject_1:System.Void()", "CustomEntrySubject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "ImmutableHashSetSubjects/ImmutableHashSetSubjects::CustomEntrySubject_2:System.Void()", "CustomEntrySubject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "ImmutableHashSetSubjects/ImmutableHashSetSubjects::CustomEntrySubject_3:System.Void()", "CustomEntrySubject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "ImmutableHashSetSubjects/ImmutableHashSetSubjects::CustomEntrySubject_4:System.Void()", "CustomEntrySubject_4", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0u, "ImmutableHashSetSubjects/ImmutableHashSetSubjects", "ImmutableHashSetSubjects/ImmutableHashSetSubjects", "", "ImmutableHashSetSubjects", "ImmutableHashSetSubjects", nullptr, kReflFields_ImmutableHashSetSubjects_ImmutableHashSetSubjects, 1u, nullptr, 0u,
	kReflMethods_ImmutableHashSetSubjects_ImmutableHashSetSubjects, 5u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "ImmutableHashSetSubjects", kReflTypePtrs, 1u };

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
// Managed method: ImmutableHashSetSubjects/ImmutableHashSetSubjects::CustomEntrySubject_0()
extern "C" void ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_0(void)
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


	CHAOS_EH_TRY
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[0])();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[1])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s2) ? 1 : 0);
			chaos_locals[1] = _s1;
			_s1 = chaos_locals[1];
			// brfalse (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s2;
				chaos_static_ImmutableHashSetSubjects_ImmutableHashSetSubjects___exitCode = chaos_value;
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
				chaos_static_ImmutableHashSetSubjects_ImmutableHashSetSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ImmutableHashSetSubjects/ImmutableHashSetSubjects::CustomEntrySubject_1()
extern "C" void ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_1(void)
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


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[3])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[1])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
			chaos_locals[1] = _s1;
			_s1 = chaos_locals[1];
			// brfalse (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s2;
				chaos_static_ImmutableHashSetSubjects_ImmutableHashSetSubjects___exitCode = chaos_value;
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
				chaos_static_ImmutableHashSetSubjects_ImmutableHashSetSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ImmutableHashSetSubjects/ImmutableHashSetSubjects::CustomEntrySubject_2()
extern "C" void ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_2(void)
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
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
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
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				auto chaos_value_raw = _s3;
				const auto chaos_value = chaos_value_raw;
				const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
				auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
				if (chaos_array == nullptr)
				{
					CHAOS_IL2CPP_FAIL();
				}
				if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
				{
					CHAOS_IL2CPP_FAIL();
				}
				chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
			}
			_s1 = _s0;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(43);
			{
				auto chaos_value_raw = _s3;
				const auto chaos_value = chaos_value_raw;
				const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
				auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
				chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[5])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[1])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
			chaos_locals[1] = _s1;
			_s1 = chaos_locals[1];
			// brfalse (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s2;
				chaos_static_ImmutableHashSetSubjects_ImmutableHashSetSubjects___exitCode = chaos_value;
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
				chaos_static_ImmutableHashSetSubjects_ImmutableHashSetSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ImmutableHashSetSubjects/ImmutableHashSetSubjects::CustomEntrySubject_3()
extern "C" void ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_3(void)
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


	CHAOS_EH_TRY
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[6])();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[7])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s2) ? 1 : 0);
			chaos_locals[1] = _s1;
			_s1 = chaos_locals[1];
			// brfalse (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s2;
				chaos_static_ImmutableHashSetSubjects_ImmutableHashSetSubjects___exitCode = chaos_value;
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
				chaos_static_ImmutableHashSetSubjects_ImmutableHashSetSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ImmutableHashSetSubjects/ImmutableHashSetSubjects::CustomEntrySubject_4()
extern "C" void ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_4(void)
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


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[8])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[7])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
			chaos_locals[1] = _s1;
			_s1 = chaos_locals[1];
			// brfalse (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s2;
				chaos_static_ImmutableHashSetSubjects_ImmutableHashSetSubjects___exitCode = chaos_value;
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
				chaos_static_ImmutableHashSetSubjects_ImmutableHashSetSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}



}  // namespace chaos::il2cpp::codegen::ImmutableHashSetSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 5;

// ── JIT Method Entry Table ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kMethodJson_0[] = "{\"methodId\":\"immutable-hash-set-subjects.immutable-hash-set-subjects.custom-entry-subject-0\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::CustomEntrySubject_0:System.Void()\",\"signature\":\"System.Void ImmutableHashSetSubjects::CustomEntrySubject_0()\",\"identity\":{\"assemblyName\":\"ImmutableHashSetSubjects\",\"declaringTypeSubjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects\",\"definitionSubjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::CustomEntrySubject_0:System.Void()\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::CustomEntrySubject_0:System.Void()\",\"methodId\":\"immutable-hash-set-subjects.immutable-hash-set-subjects.custom-entry-subject-0\",\"signature\":\"System.Void ImmutableHashSetSubjects::CustomEntrySubject_0()\"},\"nativeSymbol\":\"ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"ImmutableHashSet\\u003CSystem.Byte\\u003E\",\"callee\":\"System.Collections.Immutable/ImmutableHashSet::Create\\u003CSystem.Byte\\u003E:ImmutableHashSet\\u003CSystem.Byte\\u003E()\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create\\u003CSystem.Byte\\u003E:ImmutableHashSet\\u003CSystem.Byte\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create\\u003CSystem.Byte\\u003E:ImmutableHashSet\\u003CSystem.Byte\\u003E()\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E();type=[];method=[System.Byte]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E();type=[];method=[System.Byte]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E()\",\"typeArguments\":[],\"methodArguments\":[\"System.Byte\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E();type=[];method=[System.Byte]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E();type=[];method=[System.Byte]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create\\u003CSystem.Byte\\u003E:ImmutableHashSet\\u003CSystem.Byte\\u003E()\",\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E()\",\"displaySubjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create\\u003CSystem.Byte\\u003E()\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E()\",\"typeArguments\":[],\"methodArguments\":[\"System.Byte\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":7,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"callee\":\"System.Collections.Immutable/ImmutableHashSet\\u003CSystem.Byte\\u003E::get_Count:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u003CSystem.Byte\\u003E::get_Count:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u003CSystem.Byte\\u003E::get_Count:System.Int32()\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32();type=[System.Byte];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32();type=[System.Byte];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u003CSystem.Byte\\u003E::get_Count:System.Int32()\",\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32()\",\"displaySubjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u003CSystem.Byte\\u003E::get_Count()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ImmutableHashSetSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ImmutableHashSetSubjects\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ImmutableHashSetSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ImmutableHashSetSubjects\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ImmutableHashSetSubjects\"}";
static const char kMethodJson_1[] = "{\"methodId\":\"immutable-hash-set-subjects.immutable-hash-set-subjects.custom-entry-subject-1\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::CustomEntrySubject_1:System.Void()\",\"signature\":\"System.Void ImmutableHashSetSubjects::CustomEntrySubject_1()\",\"identity\":{\"assemblyName\":\"ImmutableHashSetSubjects\",\"declaringTypeSubjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects\",\"definitionSubjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::CustomEntrySubject_1:System.Void()\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::CustomEntrySubject_1:System.Void()\",\"methodId\":\"immutable-hash-set-subjects.immutable-hash-set-subjects.custom-entry-subject-1\",\"signature\":\"System.Void ImmutableHashSetSubjects::CustomEntrySubject_1()\"},\"nativeSymbol\":\"ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":34,\"handlerOffset\":35,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":4,\"resultType\":\"ImmutableHashSet\\u003CSystem.Byte\\u003E\",\"callee\":\"System.Collections.Immutable/ImmutableHashSet::Create\\u003CSystem.Byte\\u003E:ImmutableHashSet\\u003CSystem.Byte\\u003E(System.Byte)\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create\\u003CSystem.Byte\\u003E:ImmutableHashSet\\u003CSystem.Byte\\u003E(System.Byte)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create\\u003CSystem.Byte\\u003E:ImmutableHashSet\\u003CSystem.Byte\\u003E(System.Byte)\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E(!!0)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E(!!0);type=[];method=[System.Byte]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E(!!0);type=[];method=[System.Byte]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E(!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Byte\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E(!!0);type=[];method=[System.Byte]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E(!!0);type=[];method=[System.Byte]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create\\u003CSystem.Byte\\u003E:ImmutableHashSet\\u003CSystem.Byte\\u003E(System.Byte)\",\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E(!!0)\",\"displaySubjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create\\u003CSystem.Byte\\u003E(System.Byte)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E(!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Byte\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":9,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":10,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"callee\":\"System.Collections.Immutable/ImmutableHashSet\\u003CSystem.Byte\\u003E::get_Count:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u003CSystem.Byte\\u003E::get_Count:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u003CSystem.Byte\\u003E::get_Count:System.Int32()\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32();type=[System.Byte];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32();type=[System.Byte];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u003CSystem.Byte\\u003E::get_Count:System.Int32()\",\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32()\",\"displaySubjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u003CSystem.Byte\\u003E::get_Count()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":23,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":32,\"ilOffset\":24,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\",\"ilOffset\":27,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ImmutableHashSetSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ImmutableHashSetSubjects\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":46,\"ilOffset\":33,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":35,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":37,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\",\"ilOffset\":38,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ImmutableHashSetSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ImmutableHashSetSubjects\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":46,\"ilOffset\":44,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":46,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ImmutableHashSetSubjects\"}";
static const char kMethodJson_2[] = "{\"methodId\":\"immutable-hash-set-subjects.immutable-hash-set-subjects.custom-entry-subject-2\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::CustomEntrySubject_2:System.Void()\",\"signature\":\"System.Void ImmutableHashSetSubjects::CustomEntrySubject_2()\",\"identity\":{\"assemblyName\":\"ImmutableHashSetSubjects\",\"declaringTypeSubjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects\",\"definitionSubjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::CustomEntrySubject_2:System.Void()\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::CustomEntrySubject_2:System.Void()\",\"methodId\":\"immutable-hash-set-subjects.immutable-hash-set-subjects.custom-entry-subject-2\",\"signature\":\"System.Void ImmutableHashSetSubjects::CustomEntrySubject_2()\"},\"nativeSymbol\":\"ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_2\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":48,\"handlerOffset\":49,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":2,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":3,\"resultType\":\"System.Byte[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stelem\",\"opCode\":45,\"operand\":\"System.SByte\",\"ilOffset\":12,\"resultType\":\"System.Void\",\"runtimeServiceKind\":13,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":43,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stelem\",\"opCode\":45,\"operand\":\"System.SByte\",\"ilOffset\":17,\"resultType\":\"System.Void\",\"runtimeServiceKind\":13,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":18,\"resultType\":\"ImmutableHashSet\\u003CSystem.Byte\\u003E\",\"callee\":\"System.Collections.Immutable/ImmutableHashSet::Create\\u003CSystem.Byte\\u003E:ImmutableHashSet\\u003CSystem.Byte\\u003E(System.Byte[])\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create\\u003CSystem.Byte\\u003E:ImmutableHashSet\\u003CSystem.Byte\\u003E(System.Byte[])\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create\\u003CSystem.Byte\\u003E:ImmutableHashSet\\u003CSystem.Byte\\u003E(System.Byte[])\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E(!!0[])\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E(!!0[]);type=[];method=[System.Byte]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E(!!0[]);type=[];method=[System.Byte]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E(!!0[])\",\"typeArguments\":[],\"methodArguments\":[\"System.Byte\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E(!!0[]);type=[];method=[System.Byte]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E(!!0[]);type=[];method=[System.Byte]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create\\u003CSystem.Byte\\u003E:ImmutableHashSet\\u003CSystem.Byte\\u003E(System.Byte[])\",\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E(!!0[])\",\"displaySubjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create\\u003CSystem.Byte\\u003E(System.Byte[])\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E(!!0[])\",\"typeArguments\":[],\"methodArguments\":[\"System.Byte\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":24,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"callee\":\"System.Collections.Immutable/ImmutableHashSet\\u003CSystem.Byte\\u003E::get_Count:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u003CSystem.Byte\\u003E::get_Count:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u003CSystem.Byte\\u003E::get_Count:System.Int32()\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32();type=[System.Byte];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32();type=[System.Byte];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u003CSystem.Byte\\u003E::get_Count:System.Int32()\",\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32()\",\"displaySubjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u003CSystem.Byte\\u003E::get_Count()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":2,\"ilOffset\":30,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":34,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":36,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":37,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":46,\"ilOffset\":38,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":40,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\",\"ilOffset\":41,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ImmutableHashSetSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ImmutableHashSetSubjects\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":60,\"ilOffset\":47,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":49,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":51,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\",\"ilOffset\":52,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ImmutableHashSetSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ImmutableHashSetSubjects\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":60,\"ilOffset\":58,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":60,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ImmutableHashSetSubjects\"}";
static const char kMethodJson_3[] = "{\"methodId\":\"immutable-hash-set-subjects.immutable-hash-set-subjects.custom-entry-subject-3\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::CustomEntrySubject_3:System.Void()\",\"signature\":\"System.Void ImmutableHashSetSubjects::CustomEntrySubject_3()\",\"identity\":{\"assemblyName\":\"ImmutableHashSetSubjects\",\"declaringTypeSubjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects\",\"definitionSubjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::CustomEntrySubject_3:System.Void()\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::CustomEntrySubject_3:System.Void()\",\"methodId\":\"immutable-hash-set-subjects.immutable-hash-set-subjects.custom-entry-subject-3\",\"signature\":\"System.Void ImmutableHashSetSubjects::CustomEntrySubject_3()\"},\"nativeSymbol\":\"ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_3\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"ImmutableHashSet\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Collections.Immutable/ImmutableHashSet::Create\\u003CSystem.Int32\\u003E:ImmutableHashSet\\u003CSystem.Int32\\u003E()\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create\\u003CSystem.Int32\\u003E:ImmutableHashSet\\u003CSystem.Int32\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create\\u003CSystem.Int32\\u003E:ImmutableHashSet\\u003CSystem.Int32\\u003E()\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E();type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E();type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E()\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E();type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E();type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create\\u003CSystem.Int32\\u003E:ImmutableHashSet\\u003CSystem.Int32\\u003E()\",\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E()\",\"displaySubjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create\\u003CSystem.Int32\\u003E()\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E()\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":7,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"callee\":\"System.Collections.Immutable/ImmutableHashSet\\u003CSystem.Int32\\u003E::get_Count:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u003CSystem.Int32\\u003E::get_Count:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u003CSystem.Int32\\u003E::get_Count:System.Int32()\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32();type=[System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32();type=[System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u003CSystem.Int32\\u003E::get_Count:System.Int32()\",\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32()\",\"displaySubjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u003CSystem.Int32\\u003E::get_Count()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ImmutableHashSetSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ImmutableHashSetSubjects\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ImmutableHashSetSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ImmutableHashSetSubjects\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ImmutableHashSetSubjects\"}";
static const char kMethodJson_4[] = "{\"methodId\":\"immutable-hash-set-subjects.immutable-hash-set-subjects.custom-entry-subject-4\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::CustomEntrySubject_4:System.Void()\",\"signature\":\"System.Void ImmutableHashSetSubjects::CustomEntrySubject_4()\",\"identity\":{\"assemblyName\":\"ImmutableHashSetSubjects\",\"declaringTypeSubjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects\",\"definitionSubjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::CustomEntrySubject_4:System.Void()\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::CustomEntrySubject_4:System.Void()\",\"methodId\":\"immutable-hash-set-subjects.immutable-hash-set-subjects.custom-entry-subject-4\",\"signature\":\"System.Void ImmutableHashSetSubjects::CustomEntrySubject_4()\"},\"nativeSymbol\":\"ImmutableHashSetSubjects_ImmutableHashSetSubjects_CustomEntrySubject_4\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":34,\"handlerOffset\":35,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":4,\"resultType\":\"ImmutableHashSet\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Collections.Immutable/ImmutableHashSet::Create\\u003CSystem.Int32\\u003E:ImmutableHashSet\\u003CSystem.Int32\\u003E(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create\\u003CSystem.Int32\\u003E:ImmutableHashSet\\u003CSystem.Int32\\u003E(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create\\u003CSystem.Int32\\u003E:ImmutableHashSet\\u003CSystem.Int32\\u003E(System.Int32)\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E(!!0)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E(!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E(!!0);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E(!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E(!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E(!!0);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create\\u003CSystem.Int32\\u003E:ImmutableHashSet\\u003CSystem.Int32\\u003E(System.Int32)\",\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E(!!0)\",\"displaySubjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create\\u003CSystem.Int32\\u003E(System.Int32)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet::Create:ImmutableHashSet\\u003C!!0\\u003E(!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":9,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":10,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"callee\":\"System.Collections.Immutable/ImmutableHashSet\\u003CSystem.Int32\\u003E::get_Count:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u003CSystem.Int32\\u003E::get_Count:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u003CSystem.Int32\\u003E::get_Count:System.Int32()\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32();type=[System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32();type=[System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u003CSystem.Int32\\u003E::get_Count:System.Int32()\",\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32()\",\"displaySubjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u003CSystem.Int32\\u003E::get_Count()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableHashSet\\u00601::get_Count:System.Int32()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":23,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":32,\"ilOffset\":24,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\",\"ilOffset\":27,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ImmutableHashSetSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ImmutableHashSetSubjects\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":46,\"ilOffset\":33,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":35,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":37,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\",\"ilOffset\":38,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ImmutableHashSetSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ImmutableHashSetSubjects\",\"subjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ImmutableHashSetSubjects/ImmutableHashSetSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":46,\"ilOffset\":44,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":46,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ImmutableHashSetSubjects\"}";

extern "C" const JitMethodEntry kChaosJitMethodEntries[5] =
{
    { kMethodJson_0, 8909u, 0x00000003u, 0u },
    { kMethodJson_1, 9282u, 0x00000004u, 0u },
    { kMethodJson_2, 10652u, 0x00000005u, 0u },
    { kMethodJson_3, 8936u, 0x00000006u, 0u },
    { kMethodJson_4, 9314u, 0x00000007u, 0u }
};

extern "C" const uint32_t kChaosJitMethodEntryCount = 5u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitMethods(kChaosJitMethodEntries, kChaosJitMethodEntryCount);
}