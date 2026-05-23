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
#include "enum_stubs.h"

// Forward declaration for dispatch table entries (defined in runtime_stubs.cpp)
extern "C" void InterpreterEntryDirect(
    CHAOS_IL2CPP_UINTPTR method_key,
    void*     args_buf,
    void*     ret_buf) noexcept;

#pragma warning(push)
#pragma warning(disable: 4065 4244)

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" CHAOS_IL2CPP_INTPTR PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetCurrentProcess(void);
extern "C" CHAOS_IL2CPP_INTPTR PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetCurrentThread(void);
extern "C" CHAOS_IL2CPP_INTPTR PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetModuleHandle_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetProcessId_System_IntPtr(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentProcess(void);
extern "C" CHAOS_IL2CPP_INT32 PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentThread(void);
extern "C" CHAOS_IL2CPP_INT32 PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetModuleHandle(void);
extern "C" CHAOS_IL2CPP_INT32 PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetProcessId(void);
extern "C" void PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_0(void);
extern "C" void PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_1(void);
extern "C" void PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_2(void);
extern "C" void PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_3(void);

namespace chaos::il2cpp::codegen::PinvokeDllimportSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_PinvokeDllimportSubjects_PinvokeDllimportSubjects = {{nullptr, nullptr, 10274367341970332782ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_PinvokeDllimportSubjects_PinvokeDllimportSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(10274367341970332782ULL);
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


// ── GcTypeLayout pointer offset arrays (computed via offsetof) ──
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly, runtime_assembly_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_name_value))};

extern "C" void ChaosRegisterGcLayouts() {
	auto& registry = chaos::il2cpp::runtime_core::GcLayoutRegistry::Instance();

	registry.Register(10274367341970332782ULL, sizeof(chaos_type_PinvokeDllimportSubjects_PinvokeDllimportSubjects), nullptr, 0);
	registry.Register(15228727185366376748ULL, sizeof(chaos_type_System_Private_CoreLib_System_Object), nullptr, 0);
	registry.Register(5474029880995115448ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly, 1);
	registry.Register(17082367815459723707ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, 1);
	registry.Register(4137207361503509124ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, 5);
	registry.Register(17040031516751226236ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, 2);
	registry.Register(10748947813473285525ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, 5);
	registry.Register(1782325859292956794ULL, sizeof(chaos_type_System_Private_CoreLib_System_String), nullptr, 0);

	// Register TypeInfoV0 address ranges for IsValidTypeInfoPointer.
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_PinvokeDllimportSubjects_PinvokeDllimportSubjects), reinterpret_cast<uintptr_t>(&chaos_mt_PinvokeDllimportSubjects_PinvokeDllimportSubjects) + sizeof(chaos_mt_PinvokeDllimportSubjects_PinvokeDllimportSubjects));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object) + sizeof(chaos_mt_System_Private_CoreLib_System_Object));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_Assembly));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String) + sizeof(chaos_mt_System_Private_CoreLib_System_String));
}

CHAOS_IL2CPP_INT32 chaos_static_PinvokeDllimportSubjects_PinvokeDllimportSubjects___exitCode = 0;

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };


// ── Bridge/import thunks ──
extern "C" void* kChaosExternalRuntimeFnTable[];

// Bridge/import thunk for: PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode
extern "C" void chaos_bridge_thunk_0(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[0])();
}

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
	{ reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PinvokeDllimportSubjects::Subject_0
	{ reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PinvokeDllimportSubjects::Subject_1
	{ reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PinvokeDllimportSubjects::Subject_2
	{ reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PinvokeDllimportSubjects::Subject_3
	{ reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetCurrentProcess), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PinvokeDllimportNativeEntry::GetCurrentProcess
	{ reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetCurrentThread), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PinvokeDllimportNativeEntry::GetCurrentThread
	{ reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetModuleHandle_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PinvokeDllimportNativeEntry::GetModuleHandle
	{ reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetProcessId_System_IntPtr), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PinvokeDllimportNativeEntry::GetProcessId
	{ reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentProcess), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PinvokeDllimportNativeEntry::TestGetCurrentProcess
	{ reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentThread), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PinvokeDllimportNativeEntry::TestGetCurrentThread
	{ reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetModuleHandle), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PinvokeDllimportNativeEntry::TestGetModuleHandle
	{ reinterpret_cast<void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetProcessId), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PinvokeDllimportNativeEntry::TestGetProcessId
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
extern "C" void (*kBenchmarkWrappers[12])() = {
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

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 4;
extern "C" const int kSubjectEntryIndices[4] = {
	0,
	1,
	2,
	3,
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

	/* ── Entry 0: PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry0;
	/* ── Entry 1: PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry1;
	/* ── Entry 2: PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry2;
	/* ── Entry 3: PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry3;
	/* ── Entry 4: PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentProcess ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry4;
	/* ── Entry 5: PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentThread ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry5;
	/* ── Entry 6: PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetModuleHandle ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry6;
	/* ── Entry 7: PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetProcessId ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry7;
} kChaosGcSlotMapsSection = {
	/* entry0 = PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_0 */
	.entry0 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry1 = PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_1 */
	.entry1 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry2 = PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_2 */
	.entry2 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_2),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry3 = PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_3 */
	.entry3 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&PinvokeDllimportSubjects_PinvokeDllimportSubjects_Subject_3),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry4 = PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentProcess */
	.entry4 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentProcess),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry5 = PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentThread */
	.entry5 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentThread),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry6 = PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetModuleHandle */
	.entry6 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetModuleHandle),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry7 = PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetProcessId */
	.entry7 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetProcessId),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 276u;

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
	.image_name_utf8    = "PinvokeDllimportSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_PinvokeDllimportSubjects_PinvokeDllimportSubjects[1] = {
	{ 0x04000001u, "PinvokeDllimportSubjects/PinvokeDllimportSubjects::_exitCode", "_exitCode", "System.Int32", 0LL, 3u },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_PinvokeDllimportSubjects_PinvokeDllimportSubjects[4] = {
	{ 0x00000004u, "PinvokeDllimportSubjects/PinvokeDllimportSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000005u, "PinvokeDllimportSubjects/PinvokeDllimportSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000006u, "PinvokeDllimportSubjects/PinvokeDllimportSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000007u, "PinvokeDllimportSubjects/PinvokeDllimportSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_PinvokeDllimportSubjects_PinvokeDllimportNativeEntry[8] = {
	{ 0x00000008u, "PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetCurrentProcess:System.IntPtr()", "GetCurrentProcess", "System.IntPtr", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Au, "PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetCurrentThread:System.IntPtr()", "GetCurrentThread", "System.IntPtr", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Eu, "PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetModuleHandle:System.IntPtr(System.String)", "GetModuleHandle", "System.IntPtr", 1, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Cu, "PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::GetProcessId:System.UInt32(System.IntPtr)", "GetProcessId", "System.UInt32", 1, nullptr, 0u, nullptr, 0u },
	{ 0x00000009u, "PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetCurrentProcess:System.Int32()", "TestGetCurrentProcess", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Bu, "PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetCurrentThread:System.Int32()", "TestGetCurrentThread", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Fu, "PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetModuleHandle:System.Int32()", "TestGetModuleHandle", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Du, "PinvokeDllimportSubjects/PinvokeDllimportNativeEntry::TestGetProcessId:System.Int32()", "TestGetProcessId", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[2] = {
	{ 0x00000001u, "PinvokeDllimportSubjects/PinvokeDllimportSubjects", "PinvokeDllimportSubjects/PinvokeDllimportSubjects", "", "PinvokeDllimportSubjects", "PinvokeDllimportSubjects", nullptr, kReflFields_PinvokeDllimportSubjects_PinvokeDllimportSubjects, 1u, nullptr, 0u,
nullptr, 0u,    kReflMethods_PinvokeDllimportSubjects_PinvokeDllimportSubjects, 4u, nullptr, 0u, 0u },
	{ 0x00000002u, "PinvokeDllimportSubjects/PinvokeDllimportNativeEntry", "PinvokeDllimportSubjects/PinvokeDllimportNativeEntry", "", "PinvokeDllimportNativeEntry", "PinvokeDllimportNativeEntry", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_PinvokeDllimportSubjects_PinvokeDllimportNativeEntry, 8u, nullptr, 0u, 0u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[2] = {
	&kReflTypes[0],
	&kReflTypes[1],
};

static const ReflectionQueryImageDescriptor kReflImage = { "PinvokeDllimportSubjects", kReflTypePtrs, 2u, 1, 0, 0, 0 };

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
	CHAOS_IL2CPP_INTPTR _s8{};


	CHAOS_EH_TRY
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
					_d_hpresult = PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentProcess();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
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
					_d_hpresult = PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentProcess();
				}
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
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
				chaos_static_PinvokeDllimportSubjects_PinvokeDllimportSubjects___exitCode = chaos_value;
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
				chaos_static_PinvokeDllimportSubjects_PinvokeDllimportSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
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
	CHAOS_IL2CPP_INTPTR _s8{};


	CHAOS_EH_TRY
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
					_d_hpresult = PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentThread();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
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
					_d_hpresult = PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetCurrentThread();
				}
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
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
				chaos_static_PinvokeDllimportSubjects_PinvokeDllimportSubjects___exitCode = chaos_value;
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
				chaos_static_PinvokeDllimportSubjects_PinvokeDllimportSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
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
	CHAOS_IL2CPP_INTPTR _s8{};


	CHAOS_EH_TRY
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
					_d_hpresult = PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetProcessId();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
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
					_d_hpresult = PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetProcessId();
				}
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
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
				chaos_static_PinvokeDllimportSubjects_PinvokeDllimportSubjects___exitCode = chaos_value;
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
				chaos_static_PinvokeDllimportSubjects_PinvokeDllimportSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
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
	CHAOS_IL2CPP_INTPTR _s8{};


	CHAOS_EH_TRY
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
					_d_hpresult = PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetModuleHandle();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
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
					_d_hpresult = PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_TestGetModuleHandle();
				}
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
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
				chaos_static_PinvokeDllimportSubjects_PinvokeDllimportSubjects___exitCode = chaos_value;
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
				chaos_static_PinvokeDllimportSubjects_PinvokeDllimportSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
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
		auto& _d4 = s_hotpatch_entries[4];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d4)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d4))
		{
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d4.method_key, nullptr, &_d_hpresult);
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
		auto& _d5 = s_hotpatch_entries[5];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d5)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d5))
		{
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d5.method_key, nullptr, &_d_hpresult);
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
		auto& _d6 = s_hotpatch_entries[6];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d6)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d6))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d6.method_key, _d_ab, &_d_hpresult);
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
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
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
		auto& _d4 = s_hotpatch_entries[4];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d4)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d4))
		{
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d4.method_key, nullptr, &_d_hpresult);
		}
		else
		{
			_d_hpresult = PinvokeDllimportSubjects_PinvokeDllimportNativeEntry_GetCurrentProcess();
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	{
		const auto chaos_arg_0 = _s0;
		auto& _d7 = s_hotpatch_entries[7];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d7)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d7))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d7.method_key, _d_ab, &_d_hpresult);
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
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
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

extern "C" void ChaosJitRegisterAll() {}