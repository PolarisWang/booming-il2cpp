#include <chaos/common.h>
#include <chaos/type_info.h>
#include "runtime_core.h"
#include <chaos/eh.h>
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
#include "enum_stubs.h"
#include "enum_metadata.generated.h"

// Forward declaration for dispatch table entries (defined in runtime_stubs.cpp)
extern "C" void InterpreterEntryDirect(
    CHAOS_IL2CPP_UINTPTR method_key,
    void*     args_buf,
    void*     ret_buf) noexcept;

#pragma warning(push)
#pragma warning(disable: 4065 4244)

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" void PinvokeStressSubjects_PinvokeStressSubjects_Subject_0(void);
extern "C" void PinvokeStressSubjects_PinvokeStressSubjects_Subject_1(void);
extern "C" void PinvokeStressSubjects_PinvokeStressSubjects_Subject_2(void);
extern "C" void PinvokeStressSubjects_PinvokeStressSubjects_Subject_3(void);

namespace chaos::il2cpp::codegen::PinvokeStressSubjects {

// Bring runtime_core and jit declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;
using namespace chaos::il2cpp::jit;

// Runtime prelude dependencies included at translation unit level


inline MethodTable chaos_mt_PinvokeStressSubjects_PinvokeStressSubjects = {nullptr, nullptr, 15560340561090953884ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_PinvokeStressSubjects_PinvokeStressSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(15560340561090953884ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Object = {nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {nullptr, nullptr, 5474029880995115448ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly = static_cast<CHAOS_IL2CPP_INTPTR>(5474029880995115448ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName = {nullptr, nullptr, 17082367815459723707ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = static_cast<CHAOS_IL2CPP_INTPTR>(17082367815459723707ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo = {nullptr, nullptr, 4137207361503509124ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo = static_cast<CHAOS_IL2CPP_INTPTR>(4137207361503509124ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo = {nullptr, nullptr, 17040031516751226236ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo = static_cast<CHAOS_IL2CPP_INTPTR>(17040031516751226236ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo = {nullptr, nullptr, 10748947813473285525ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = static_cast<CHAOS_IL2CPP_INTPTR>(10748947813473285525ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_String = {nullptr, nullptr, 1782325859292956794ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
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
		return chaos_array->element_type_shape == chaos_type_shape_reference            && chaos_is_type_compatible(chaos_mt_System_Private_CoreLib_System_String.AsTypeInfoHot(), chaos_array->element_type_info);
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

struct chaos_type_PinvokeStressSubjects_PinvokeStressSubjects
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


// ── GcTypeLayout pointer offset arrays (computed via offsetof) ──
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly, runtime_assembly_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_name_value))};

extern "C" void ChaosRegisterGcLayouts() {
	auto& registry = chaos::il2cpp::runtime_core::GcLayoutRegistry::Instance();

	registry.Register(15560340561090953884ULL, sizeof(chaos_type_PinvokeStressSubjects_PinvokeStressSubjects), nullptr, 0);
	registry.Register(15228727185366376748ULL, sizeof(chaos_type_System_Private_CoreLib_System_Object), nullptr, 0);
	registry.Register(5474029880995115448ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly, 1);
	registry.Register(17082367815459723707ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, 1);
	registry.Register(4137207361503509124ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, 5);
	registry.Register(17040031516751226236ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, 2);
	registry.Register(10748947813473285525ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, 5);
	registry.Register(1782325859292956794ULL, sizeof(chaos_type_System_Private_CoreLib_System_String), nullptr, 0);

	// Register MethodTable address ranges for IsValidTypeInfoPointer.
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_PinvokeStressSubjects_PinvokeStressSubjects), reinterpret_cast<uintptr_t>(&chaos_mt_PinvokeStressSubjects_PinvokeStressSubjects) + sizeof(chaos_mt_PinvokeStressSubjects_PinvokeStressSubjects));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object) + sizeof(chaos_mt_System_Private_CoreLib_System_Object));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_Assembly));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String) + sizeof(chaos_mt_System_Private_CoreLib_System_String));
}

CHAOS_IL2CPP_INT32 chaos_static_PinvokeStressSubjects_PinvokeStressSubjects___exitCode = 0;

static CHAOS_IL2CPP_INT64 chaos_external_runtime_System_Private_CoreLib_System_GC__GetTotalMemory_System_Int64_System_Boolean_(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	return chaos_gc_get_total_memory(chaos_fn_arg_0);
}

static void chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Sleep_System_Void_System_Int32_(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	chaos_thread_sleep(chaos_fn_arg_0);
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };


// ── Bridge/import thunks ──
extern "C" void* kChaosExternalRuntimeFnTable[];

// Bridge/import thunk for: System.Private.CoreLib/System.Environment::get_TickCount:System.Int32()
extern "C" CHAOS_IL2CPP_INT32 chaos_bridge_thunk_0(void)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)()>(kChaosExternalRuntimeFnTable[0])();
	return result;
}

// Bridge/import thunk for: PinvokeStressSubjects/PinvokeStressSubjects::_exitCode
extern "C" void chaos_bridge_thunk_1(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[1])();
}

// Bridge/import thunk for: System.Runtime.InteropServices/Marshal::GetLastPInvokeError:System.Int32()
extern "C" CHAOS_IL2CPP_INT32 chaos_bridge_thunk_2(void)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)()>(kChaosExternalRuntimeFnTable[4])();
	return result;
}

extern "C" void PinvokeStressSubjects_PinvokeStressSubjects_Subject_0(void);
extern "C" void PinvokeStressSubjects_PinvokeStressSubjects_Subject_1(void);
extern "C" void PinvokeStressSubjects_PinvokeStressSubjects_Subject_2(void);
extern "C" void PinvokeStressSubjects_PinvokeStressSubjects_Subject_3(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[5] = {
	0u,
	0u,
	0u,
	0u,
	0u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[4];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		4u,
		0u,
		2615243109u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // PinvokeStressSubjects_PinvokeStressSubjects_Subject_0
		{ 0u, 0u },  // PinvokeStressSubjects_PinvokeStressSubjects_Subject_1
		{ 0u, 0u },  // PinvokeStressSubjects_PinvokeStressSubjects_Subject_2
		{ 0u, 0u },  // PinvokeStressSubjects_PinvokeStressSubjects_Subject_3
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
		"PinvokeStressSubjects",
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
		chaos_mt_PinvokeStressSubjects_PinvokeStressSubjects.AsTypeInfoHot(),
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
		/* .name_utf8         = */ "PinvokeStressSubjects",
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
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 -- deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,
	/* .custom_attribute_method_offset   = */ nullptr,
	/* .custom_attribute_field_offset    = */ nullptr,
	/* .custom_attribute_property_offset = */ nullptr,
	/* .custom_attribute_param_offset    = */ nullptr,
	/* .custom_attribute_method_count    = */ 0u,
	/* .custom_attribute_field_count     = */ 0u,
	/* .custom_attribute_property_count  = */ 0u,
	/* .custom_attribute_param_count   = */ 0u,
		/* .abi_manifest      = */ s_abi_manifest,
	};
	static const CHAOS_IL2CPP_UINT32 s_native_aot_module_id =
		::chaos::il2cpp::runtime_core::RegisterModule("PinvokeStressSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[4] = {
	{ "Subject_0", 0x00000003u, 0u },  // PinvokeStressSubjects
	{ "Subject_1", 0x00000004u, 0u },  // PinvokeStressSubjects
	{ "Subject_2", 0x00000005u, 0u },  // PinvokeStressSubjects
	{ "Subject_3", 0x00000006u, 0u },  // PinvokeStressSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "PinvokeStressSubjects", "", 0u, 4u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[4] = {
	{ 0x00000003u, 0u },
	{ 0x00000004u, 1u },
	{ 0x00000005u, 2u },
	{ 0x00000006u, 3u },
};

// Dispatch table (function pointers)
// s_hotpatch_entries — external linkage for verification_dispatch
// NOTE: `static` is deliberately omitted so that the verification dispatch
// TU (verification_dispatch.generated.cpp) can access this array via
// GetHotpatchEntries(). Without external linkage, the linker cannot resolve
// the symbol across translation units.
extern "C" HotpatchEntryV0 s_hotpatch_entries[4] = {
	{ reinterpret_cast<void*>(&PinvokeStressSubjects_PinvokeStressSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PinvokeStressSubjects::Subject_0
	{ reinterpret_cast<void*>(&PinvokeStressSubjects_PinvokeStressSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PinvokeStressSubjects::Subject_1
	{ reinterpret_cast<void*>(&PinvokeStressSubjects_PinvokeStressSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PinvokeStressSubjects::Subject_2
	{ reinterpret_cast<void*>(&PinvokeStressSubjects_PinvokeStressSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PinvokeStressSubjects::Subject_3
};

// Accessor functions for verification_dispatch.generated.cpp
extern "C" const HotpatchEntryV0* GetHotpatchEntries() noexcept { return s_hotpatch_entries; }
extern "C" int32_t GetHotpatchEntryCount() noexcept { return 4; }

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"PinvokeStressSubjects",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	4u,
	s_hotpatch_slots,
	4u,
	s_hotpatch_entries,
	4u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[5] = {
	"System.Private.CoreLib/System.Environment::get_TickCount:System.Int32()",
	"PinvokeStressSubjects/PinvokeStressSubjects::_exitCode",
	"System.Threading.Thread/System.Threading.Thread::Sleep:System.Void(System.Int32)",
	"System.Private.CoreLib/System.GC::GetTotalMemory:System.Int64(System.Boolean)",
	"System.Runtime.InteropServices/Marshal::GetLastPInvokeError:System.Int32()",
};

extern "C" void* kChaosExternalRuntimeFnTable[5] = {
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Sleep_System_Void_System_Int32_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_GC__GetTotalMemory_System_Int64_System_Boolean_),
	nullptr,
};

extern "C" int32_t kChaosExternalRuntimeCount = 5;
// (no method AOT entries for this module)
// ── Method table (kMethodTable[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
// NOTE: Pure data only — dispatch routing is in <chaos/hotpatch_dispatch.h>.
// JIT mode uses direct_ptr (precode trampoline → JIT compilation).

// ── Subject slot map (kSubjectSlotMap[]) ────────────────────────────
// Maps subject index (0-based sequential) to kMethodTable index.
// External consumers use this to route --benchmark N to the correct
// AOT method slot, since kMethodTable[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 4;
extern "C" const int kSubjectSlotMap[4] = {
	0,
	1,
	2,
	3,
};
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

	/* ── Entry 0: PinvokeStressSubjects_PinvokeStressSubjects_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry0;
	/* ── Entry 1: PinvokeStressSubjects_PinvokeStressSubjects_Subject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry1;
	/* ── Entry 2: PinvokeStressSubjects_PinvokeStressSubjects_Subject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry2;
	/* ── Entry 3: PinvokeStressSubjects_PinvokeStressSubjects_Subject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry3;
} kChaosGcSlotMapsSection = {
	/* entry0 = PinvokeStressSubjects_PinvokeStressSubjects_Subject_0 */
	.entry0 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&PinvokeStressSubjects_PinvokeStressSubjects_Subject_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry1 = PinvokeStressSubjects_PinvokeStressSubjects_Subject_1 */
	.entry1 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&PinvokeStressSubjects_PinvokeStressSubjects_Subject_1),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry2 = PinvokeStressSubjects_PinvokeStressSubjects_Subject_2 */
	.entry2 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&PinvokeStressSubjects_PinvokeStressSubjects_Subject_2),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry3 = PinvokeStressSubjects_PinvokeStressSubjects_Subject_3 */
	.entry3 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&PinvokeStressSubjects_PinvokeStressSubjects_Subject_3),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 120u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[4] = {
	reinterpret_cast<void*>(&PinvokeStressSubjects_PinvokeStressSubjects_Subject_0),
	reinterpret_cast<void*>(&PinvokeStressSubjects_PinvokeStressSubjects_Subject_1),
	reinterpret_cast<void*>(&PinvokeStressSubjects_PinvokeStressSubjects_Subject_2),
	reinterpret_cast<void*>(&PinvokeStressSubjects_PinvokeStressSubjects_Subject_3),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 4u,
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
	.image_name_utf8    = "PinvokeStressSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_PinvokeStressSubjects_PinvokeStressSubjects[1] = {
	{ 0x04000001u, "PinvokeStressSubjects/PinvokeStressSubjects::_exitCode", "_exitCode", "System.Int32", 0LL, 3u },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_PinvokeStressSubjects_PinvokeStressSubjects[4] = {
	{ 0x00000003u, "PinvokeStressSubjects/PinvokeStressSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000004u, "PinvokeStressSubjects/PinvokeStressSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000005u, "PinvokeStressSubjects/PinvokeStressSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000006u, "PinvokeStressSubjects/PinvokeStressSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0x00000001u, "PinvokeStressSubjects/PinvokeStressSubjects", "PinvokeStressSubjects/PinvokeStressSubjects", "", "PinvokeStressSubjects", "PinvokeStressSubjects", nullptr, kReflFields_PinvokeStressSubjects_PinvokeStressSubjects, 1u, nullptr, 0u,
nullptr, 0u,    kReflMethods_PinvokeStressSubjects_PinvokeStressSubjects, 4u, nullptr, 0u, 0u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "PinvokeStressSubjects", kReflTypePtrs, 1u, 1, 0, 0, 0 };

// Fake ImageHandle that ResolveSubjectId will decode back to kReflImage.
// BootstrapRuntime's aot_image_handle fallback discovers this via
// LookupModule(mid)->image at lines 311-321 of bootstrap.cpp.
	// Register generated enum metadata (tables + dispatch + type descriptors).
	// ChaosRegisterEnumGeneratedMetadata is defined in enum_metadata.generated.h.
	static const CHAOS_IL2CPP_UINT32 s_enum_registered = []() noexcept {
		ChaosRegisterEnumGeneratedMetadata();
		return 1u;
	}();

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
// Managed method: PinvokeStressSubjects/PinvokeStressSubjects::Subject_0()
extern "C" void PinvokeStressSubjects_PinvokeStressSubjects_Subject_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	CHAOS_EH_TRY
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[0])();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[0])();
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
				chaos_static_PinvokeStressSubjects_PinvokeStressSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_PinvokeStressSubjects_PinvokeStressSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: PinvokeStressSubjects/PinvokeStressSubjects::Subject_1()
extern "C" void PinvokeStressSubjects_PinvokeStressSubjects_Subject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_raw_arg_0 = _s0;
				const auto chaos_arg_0 = chaos_raw_arg_0;
				chaos_thread_sleep(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_PinvokeStressSubjects_PinvokeStressSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: PinvokeStressSubjects/PinvokeStressSubjects::Subject_2()
extern "C" void PinvokeStressSubjects_PinvokeStressSubjects_Subject_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_gc_get_total_memory(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
				_s0 = ChaosStoreInt64(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = chaos_gc_get_total_memory(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
				_s1 = ChaosStoreInt64(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_PinvokeStressSubjects_PinvokeStressSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_PinvokeStressSubjects_PinvokeStressSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: PinvokeStressSubjects/PinvokeStressSubjects::Subject_3()
extern "C" void PinvokeStressSubjects_PinvokeStressSubjects_Subject_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	CHAOS_EH_TRY
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[4])();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[4])();
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
				chaos_static_PinvokeStressSubjects_PinvokeStressSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_PinvokeStressSubjects_PinvokeStressSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}



}  // namespace chaos::il2cpp::codegen::PinvokeStressSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 4;

// --- JIT Method Entry Table ---
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kJitJson_0[] = "{\"methodId\":\"pinvoke-stress-subjects.pinvoke-stress-subjects.subject-0\",\"subjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects::Subject_0:System.Void()\",\"signature\":\"System.Void PinvokeStressSubjects::Subject_0()\",\"identity\":{\"assemblyName\":\"PinvokeStressSubjects\",\"declaringTypeSubjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects\",\"definitionSubjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects::Subject_0:System.Void()\",\"subjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects::Subject_0:System.Void()\",\"methodId\":\"pinvoke-stress-subjects.pinvoke-stress-subjects.subject-0\",\"signature\":\"System.Void PinvokeStressSubjects::Subject_0()\"},\"nativeSymbol\":\"PinvokeStressSubjects_PinvokeStressSubjects_Subject_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Environment::get_TickCount:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Environment::get_TickCount:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Environment::get_TickCount:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Environment::get_TickCount:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Environment::get_TickCount:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Environment::get_TickCount:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PinvokeStressSubjects/PinvokeStressSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PinvokeStressSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PinvokeStressSubjects\",\"subjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PinvokeStressSubjects/PinvokeStressSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PinvokeStressSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PinvokeStressSubjects\",\"subjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PinvokeStressSubjects\"}";
static const char kJitJson_1[] = "{\"methodId\":\"pinvoke-stress-subjects.pinvoke-stress-subjects.subject-1\",\"subjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects::Subject_1:System.Void()\",\"signature\":\"System.Void PinvokeStressSubjects::Subject_1()\",\"identity\":{\"assemblyName\":\"PinvokeStressSubjects\",\"declaringTypeSubjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects\",\"definitionSubjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects::Subject_1:System.Void()\",\"subjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects::Subject_1:System.Void()\",\"methodId\":\"pinvoke-stress-subjects.pinvoke-stress-subjects.subject-1\",\"signature\":\"System.Void PinvokeStressSubjects::Subject_1()\"},\"nativeSymbol\":\"PinvokeStressSubjects_PinvokeStressSubjects_Subject_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":12,\"handlerOffset\":13,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Threading.Thread/System.Threading.Thread::Sleep:System.Void(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Threading.Thread\",\"subjectKind\":\"method\",\"subjectId\":\"System.Threading.Thread/System.Threading.Thread::Sleep:System.Void(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Threading.Thread\",\"subjectId\":\"System.Threading.Thread/System.Threading.Thread::Sleep:System.Void(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PinvokeStressSubjects/PinvokeStressSubjects::_exitCode\",\"ilOffset\":16,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PinvokeStressSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PinvokeStressSubjects\",\"subjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":24,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PinvokeStressSubjects\"}";
static const char kJitJson_2[] = "{\"methodId\":\"pinvoke-stress-subjects.pinvoke-stress-subjects.subject-2\",\"subjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects::Subject_2:System.Void()\",\"signature\":\"System.Void PinvokeStressSubjects::Subject_2()\",\"identity\":{\"assemblyName\":\"PinvokeStressSubjects\",\"declaringTypeSubjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects\",\"definitionSubjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects::Subject_2:System.Void()\",\"subjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects::Subject_2:System.Void()\",\"methodId\":\"pinvoke-stress-subjects.pinvoke-stress-subjects.subject-2\",\"signature\":\"System.Void PinvokeStressSubjects::Subject_2()\"},\"nativeSymbol\":\"PinvokeStressSubjects_PinvokeStressSubjects_Subject_2\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":33,\"handlerOffset\":34,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"callee\":\"System.Private.CoreLib/System.GC::GetTotalMemory:System.Int64(System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.GC::GetTotalMemory:System.Int64(System.Boolean)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.GC::GetTotalMemory:System.Int64(System.Boolean)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":10,\"resultType\":\"System.Int64\",\"callee\":\"System.Private.CoreLib/System.GC::GetTotalMemory:System.Int64(System.Boolean)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.GC::GetTotalMemory:System.Int64(System.Boolean)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.GC::GetTotalMemory:System.Int64(System.Boolean)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":22,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":31,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PinvokeStressSubjects/PinvokeStressSubjects::_exitCode\",\"ilOffset\":26,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PinvokeStressSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PinvokeStressSubjects\",\"subjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":45,\"ilOffset\":32,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":34,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":36,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PinvokeStressSubjects/PinvokeStressSubjects::_exitCode\",\"ilOffset\":37,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PinvokeStressSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PinvokeStressSubjects\",\"subjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":45,\"ilOffset\":43,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":45,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PinvokeStressSubjects\"}";
static const char kJitJson_3[] = "{\"methodId\":\"pinvoke-stress-subjects.pinvoke-stress-subjects.subject-3\",\"subjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects::Subject_3:System.Void()\",\"signature\":\"System.Void PinvokeStressSubjects::Subject_3()\",\"identity\":{\"assemblyName\":\"PinvokeStressSubjects\",\"declaringTypeSubjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects\",\"definitionSubjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects::Subject_3:System.Void()\",\"subjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects::Subject_3:System.Void()\",\"methodId\":\"pinvoke-stress-subjects.pinvoke-stress-subjects.subject-3\",\"signature\":\"System.Void PinvokeStressSubjects::Subject_3()\"},\"nativeSymbol\":\"PinvokeStressSubjects_PinvokeStressSubjects_Subject_3\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":29,\"handlerOffset\":30,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"System.Runtime.InteropServices/Marshal::GetLastPInvokeError:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Runtime.InteropServices\",\"subjectKind\":\"method\",\"subjectId\":\"System.Runtime.InteropServices/Marshal::GetLastPInvokeError:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Runtime.InteropServices\",\"subjectId\":\"System.Runtime.InteropServices/Marshal::GetLastPInvokeError:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"System.Runtime.InteropServices/Marshal::GetLastPInvokeError:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Runtime.InteropServices\",\"subjectKind\":\"method\",\"subjectId\":\"System.Runtime.InteropServices/Marshal::GetLastPInvokeError:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Runtime.InteropServices\",\"subjectId\":\"System.Runtime.InteropServices/Marshal::GetLastPInvokeError:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":27,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PinvokeStressSubjects/PinvokeStressSubjects::_exitCode\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PinvokeStressSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PinvokeStressSubjects\",\"subjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":30,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"PinvokeStressSubjects/PinvokeStressSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"PinvokeStressSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"PinvokeStressSubjects\",\"subjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"PinvokeStressSubjects/PinvokeStressSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":41,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"PinvokeStressSubjects\"}";

extern "C" const JitEntry kChaosJitEntries[4] =
{
    { kJitJson_0, 5087u, 0x00000003u, 0u },
    { kJitJson_1, 3224u, 0x00000004u, 0u },
    { kJitJson_2, 5513u, 0x00000005u, 0u },
    { kJitJson_3, 5137u, 0x00000006u, 0u }
};

extern "C" const uint32_t kChaosJitEntryCount = 4u;

extern "C" void ChaosJitRegisterAll() {
    // Register hotpatch module so GetDispatchEntry can resolve tokens → slots
    chaos::il2cpp::runtime_core::RegisterHotpatchModule(&s_hotpatch_module);
    RegisterJitEntryMethods(kChaosJitEntries, kChaosJitEntryCount);
}