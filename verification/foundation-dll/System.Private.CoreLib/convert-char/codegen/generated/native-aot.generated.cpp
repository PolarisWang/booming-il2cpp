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

namespace chaos::il2cpp::codegen::ConvertCharPatchEntry {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_ConvertCharPatchEntry_ConvertCharPatchEntry = {{nullptr, nullptr, 11118279506912154456ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ConvertCharPatchEntry_ConvertCharPatchEntry = static_cast<CHAOS_IL2CPP_INTPTR>(11118279506912154456ULL);
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

struct chaos_type_ConvertCharPatchEntry_ConvertCharPatchEntry
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

	registry.Register(11118279506912154456ULL, sizeof(chaos_type_ConvertCharPatchEntry_ConvertCharPatchEntry), nullptr, 0);
	registry.Register(15228727185366376748ULL, sizeof(chaos_type_System_Private_CoreLib_System_Object), nullptr, 0);
	registry.Register(5474029880995115448ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly, 1);
	registry.Register(17082367815459723707ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, 1);
	registry.Register(4137207361503509124ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, 5);
	registry.Register(17040031516751226236ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, 2);
	registry.Register(10748947813473285525ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, 5);
	registry.Register(1782325859292956794ULL, sizeof(chaos_type_System_Private_CoreLib_System_String), nullptr, 0);

	// Register TypeInfoV0 address ranges for IsValidTypeInfoPointer.
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_ConvertCharPatchEntry_ConvertCharPatchEntry), reinterpret_cast<uintptr_t>(&chaos_mt_ConvertCharPatchEntry_ConvertCharPatchEntry) + sizeof(chaos_mt_ConvertCharPatchEntry_ConvertCharPatchEntry));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object) + sizeof(chaos_mt_System_Private_CoreLib_System_Object));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_Assembly));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String) + sizeof(chaos_mt_System_Private_CoreLib_System_String));
}

CHAOS_IL2CPP_INT32 chaos_static_ConvertCharPatchEntry_ConvertCharPatchEntry___exitCode = 0;

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };


// ── Bridge/import thunks ──
extern "C" void* kChaosExternalRuntimeFnTable[];

// Bridge/import thunk for: ConvertCharPatchEntry/ConvertCharPatchEntry::_exitCode
extern "C" void chaos_bridge_thunk_0(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[0])();
}

extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_0(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_1(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_2(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_3(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_4(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_5(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_6(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_7(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_8(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_9(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_10(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_11(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_12(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_13(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_14(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_15(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_16(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_17(void);
extern "C" void ConvertCharPatchEntry_ConvertCharPatchEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[20] = {
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
	1u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[19];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		19u,
		1u,
		4281279961u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 1u, 0u },  // ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_0
		{ 1u, 0u },  // ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_1
		{ 1u, 0u },  // ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_2
		{ 1u, 0u },  // ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_3
		{ 1u, 0u },  // ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_4
		{ 1u, 0u },  // ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_5
		{ 1u, 0u },  // ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_6
		{ 1u, 0u },  // ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_7
		{ 1u, 0u },  // ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_8
		{ 1u, 0u },  // ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_9
		{ 1u, 0u },  // ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_10
		{ 1u, 0u },  // ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_11
		{ 1u, 0u },  // ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_12
		{ 1u, 0u },  // ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_13
		{ 1u, 0u },  // ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_14
		{ 1u, 0u },  // ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_15
		{ 1u, 0u },  // ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_16
		{ 1u, 0u },  // ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_17
		{ 0u, 1u },  // ConvertCharPatchEntry_ConvertCharPatchEntry_Run_System_Int32
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
		"ConvertCharPatchEntry",
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
		&chaos_mt_ConvertCharPatchEntry_ConvertCharPatchEntry.hot,
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
		/* .name_utf8         = */ "ConvertCharPatchEntry",
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
		::chaos::il2cpp::runtime_core::RegisterModule("ConvertCharPatchEntry", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[19] = {
	{ "Subject_0", 0x00000003u, 0u },  // ConvertCharPatchEntry
	{ "Subject_1", 0x00000004u, 0u },  // ConvertCharPatchEntry
	{ "Subject_2", 0x00000005u, 0u },  // ConvertCharPatchEntry
	{ "Subject_3", 0x00000006u, 0u },  // ConvertCharPatchEntry
	{ "Subject_4", 0x00000007u, 0u },  // ConvertCharPatchEntry
	{ "Subject_5", 0x00000008u, 0u },  // ConvertCharPatchEntry
	{ "Subject_6", 0x00000009u, 0u },  // ConvertCharPatchEntry
	{ "Subject_7", 0x0000000Au, 0u },  // ConvertCharPatchEntry
	{ "Subject_8", 0x0000000Bu, 0u },  // ConvertCharPatchEntry
	{ "Subject_9", 0x0000000Cu, 0u },  // ConvertCharPatchEntry
	{ "Subject_10", 0x0000000Du, 0u },  // ConvertCharPatchEntry
	{ "Subject_11", 0x0000000Eu, 0u },  // ConvertCharPatchEntry
	{ "Subject_12", 0x0000000Fu, 0u },  // ConvertCharPatchEntry
	{ "Subject_13", 0x00000010u, 0u },  // ConvertCharPatchEntry
	{ "Subject_14", 0x00000011u, 0u },  // ConvertCharPatchEntry
	{ "Subject_15", 0x00000012u, 0u },  // ConvertCharPatchEntry
	{ "Subject_16", 0x00000013u, 0u },  // ConvertCharPatchEntry
	{ "Subject_17", 0x00000014u, 0u },  // ConvertCharPatchEntry
	{ "Run", 0x00000015u, 1u },  // ConvertCharPatchEntry
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "ConvertCharPatchEntry", "", 0u, 19u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[19] = {
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
	{ 0x00000012u, 15u },
	{ 0x00000013u, 16u },
	{ 0x00000014u, 17u },
	{ 0x00000015u, 18u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[19] = {
	{ reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ConvertCharPatchEntry::Subject_0
	{ reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ConvertCharPatchEntry::Subject_1
	{ reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ConvertCharPatchEntry::Subject_2
	{ reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ConvertCharPatchEntry::Subject_3
	{ reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ConvertCharPatchEntry::Subject_4
	{ reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ConvertCharPatchEntry::Subject_5
	{ reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ConvertCharPatchEntry::Subject_6
	{ reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ConvertCharPatchEntry::Subject_7
	{ reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ConvertCharPatchEntry::Subject_8
	{ reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ConvertCharPatchEntry::Subject_9
	{ reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ConvertCharPatchEntry::Subject_10
	{ reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ConvertCharPatchEntry::Subject_11
	{ reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ConvertCharPatchEntry::Subject_12
	{ reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ConvertCharPatchEntry::Subject_13
	{ reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_14), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ConvertCharPatchEntry::Subject_14
	{ reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_15), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ConvertCharPatchEntry::Subject_15
	{ reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_16), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ConvertCharPatchEntry::Subject_16
	{ reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_17), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ConvertCharPatchEntry::Subject_17
	{ reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Run_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ConvertCharPatchEntry::Run
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"ConvertCharPatchEntry",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	19u,
	s_hotpatch_slots,
	19u,
	s_hotpatch_entries,
	19u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[1] = {
	"ConvertCharPatchEntry/ConvertCharPatchEntry::_exitCode",
};

extern "C" void* kChaosExternalRuntimeFnTable[1] = {
	nullptr,
};

extern "C" int32_t kChaosExternalRuntimeCount = 1;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[19])() = {
	reinterpret_cast<void(*)()>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_0),
	reinterpret_cast<void(*)()>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_1),
	reinterpret_cast<void(*)()>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_2),
	reinterpret_cast<void(*)()>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_3),
	reinterpret_cast<void(*)()>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_4),
	reinterpret_cast<void(*)()>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_5),
	reinterpret_cast<void(*)()>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_6),
	reinterpret_cast<void(*)()>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_7),
	reinterpret_cast<void(*)()>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_8),
	reinterpret_cast<void(*)()>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_9),
	reinterpret_cast<void(*)()>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_10),
	reinterpret_cast<void(*)()>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_11),
	reinterpret_cast<void(*)()>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_12),
	reinterpret_cast<void(*)()>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_13),
	reinterpret_cast<void(*)()>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_14),
	reinterpret_cast<void(*)()>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_15),
	reinterpret_cast<void(*)()>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_16),
	reinterpret_cast<void(*)()>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_17),
	reinterpret_cast<void(*)()>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Run_System_Int32),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[19])() = {
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
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[18])(0);},
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 18;
extern "C" const int kSubjectEntryIndices[18] = {
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
	17,
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

	/* ── Entry 0: ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry0;
	/* ── Entry 1: ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry1;
	/* ── Entry 2: ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry2;
	/* ── Entry 3: ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry3;
	/* ── Entry 4: ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry4;
	/* ── Entry 5: ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry5;
	/* ── Entry 6: ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_6 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry6;
	/* ── Entry 7: ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_7 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry7;
	/* ── Entry 8: ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_8 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry8;
	/* ── Entry 9: ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_9 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry9;
	/* ── Entry 10: ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_10 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry10;
	/* ── Entry 11: ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_11 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry11;
	/* ── Entry 12: ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_12 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry12;
	/* ── Entry 13: ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_13 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry13;
	/* ── Entry 14: ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_14 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry14;
	/* ── Entry 15: ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_15 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry15;
	/* ── Entry 16: ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_16 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry16;
	/* ── Entry 17: ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_17 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry17;
	/* ── Entry 18: ConvertCharPatchEntry_ConvertCharPatchEntry_Run_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry18;
} kChaosGcSlotMapsSection = {
	/* entry0 = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_0 */
	.entry0 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_0),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry1 = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_1 */
	.entry1 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_1),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry2 = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_2 */
	.entry2 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_2),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry3 = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_3 */
	.entry3 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_3),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry4 = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_4 */
	.entry4 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_4),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry5 = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_5 */
	.entry5 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_5),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry6 = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_6 */
	.entry6 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_6),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry7 = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_7 */
	.entry7 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_7),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry8 = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_8 */
	.entry8 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_8),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry9 = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_9 */
	.entry9 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_9),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry10 = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_10 */
	.entry10 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_10),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry11 = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_11 */
	.entry11 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_11),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry12 = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_12 */
	.entry12 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_12),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry13 = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_13 */
	.entry13 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_13),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry14 = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_14 */
	.entry14 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_14),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry15 = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_15 */
	.entry15 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_15),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry16 = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_16 */
	.entry16 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_16),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry17 = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_17 */
	.entry17 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_17),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry18 = ConvertCharPatchEntry_ConvertCharPatchEntry_Run_System_Int32 */
	.entry18 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Run_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 8u, 16u, 24u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 536u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[19] = {
	reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_0),
	reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_1),
	reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_2),
	reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_3),
	reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_4),
	reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_5),
	reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_6),
	reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_7),
	reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_8),
	reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_9),
	reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_10),
	reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_11),
	reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_12),
	reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_13),
	reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_14),
	reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_15),
	reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_16),
	reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_17),
	reinterpret_cast<void*>(&ConvertCharPatchEntry_ConvertCharPatchEntry_Run_System_Int32),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 19u,
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
	.image_name_utf8    = "ConvertCharPatchEntry",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_ConvertCharPatchEntry_ConvertCharPatchEntry[1] = {
	{ 0x04000001u, "ConvertCharPatchEntry/ConvertCharPatchEntry::_exitCode", "_exitCode", "System.Int32", 0LL, 3u },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_ConvertCharPatchEntry_ConvertCharPatchEntry[19] = {
	{ 0x00000003u, "ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_0:System.Int32()", "Subject_0", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000004u, "ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_1:System.Int32()", "Subject_1", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000005u, "ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_2:System.Int32()", "Subject_2", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000006u, "ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_3:System.Int32()", "Subject_3", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000007u, "ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_4:System.Int32()", "Subject_4", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000008u, "ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_5:System.Int32()", "Subject_5", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000009u, "ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_6:System.Int32()", "Subject_6", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Au, "ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_7:System.Int32()", "Subject_7", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Bu, "ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_8:System.Int32()", "Subject_8", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Cu, "ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_9:System.Int32()", "Subject_9", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Du, "ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_10:System.Int32()", "Subject_10", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Eu, "ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_11:System.Int32()", "Subject_11", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Fu, "ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_12:System.Int32()", "Subject_12", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000010u, "ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_13:System.Int32()", "Subject_13", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000011u, "ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_14:System.Int32()", "Subject_14", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000012u, "ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_15:System.Int32()", "Subject_15", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000013u, "ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_16:System.Int32()", "Subject_16", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000014u, "ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_17:System.Int32()", "Subject_17", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000015u, "ConvertCharPatchEntry/ConvertCharPatchEntry::Run:System.Void(System.Int32)", "Run", "System.Void", 1, nullptr, 0u, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0x00000001u, "ConvertCharPatchEntry/ConvertCharPatchEntry", "ConvertCharPatchEntry/ConvertCharPatchEntry", "", "ConvertCharPatchEntry", "ConvertCharPatchEntry", nullptr, kReflFields_ConvertCharPatchEntry_ConvertCharPatchEntry, 1u, nullptr, 0u,
nullptr, 0u,    kReflMethods_ConvertCharPatchEntry_ConvertCharPatchEntry, 19u, nullptr, 0u, 0u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "ConvertCharPatchEntry", kReflTypePtrs, 1u, 1, 0, 0, 0 };

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
// Managed method: ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_0()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_0(void)
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

// Managed method: ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_1()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_1(void)
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

// Managed method: ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_2()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_2(void)
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

// Managed method: ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_3()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_3(void)
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

// Managed method: ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_4()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_4(void)
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

// Managed method: ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_5()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_5(void)
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

// Managed method: ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_6()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_6(void)
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

// Managed method: ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_7()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_7(void)
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

// Managed method: ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_8()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_8(void)
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

// Managed method: ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_9()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_9(void)
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

// Managed method: ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_10()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_10(void)
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

// Managed method: ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_11()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_11(void)
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

// Managed method: ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_12()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_12(void)
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

// Managed method: ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_13()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_13(void)
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

// Managed method: ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_14()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_14(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177266);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_15()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_15(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177265);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_16()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_16(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177264);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ConvertCharPatchEntry/ConvertCharPatchEntry::Subject_17()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_17(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177263);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ConvertCharPatchEntry/ConvertCharPatchEntry::Run(System.Int32)
extern "C" void ConvertCharPatchEntry_ConvertCharPatchEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
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
	CHAOS_IL2CPP_INTPTR _s19{};
	CHAOS_IL2CPP_INTPTR _s20{};
	CHAOS_IL2CPP_INTPTR _s21{};
	CHAOS_IL2CPP_INTPTR _s22{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	CHAOS_EH_TRY
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
								_d_hpresult = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_0();
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
								_d_hpresult = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_1();
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
								_d_hpresult = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_2();
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
								_d_hpresult = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_3();
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
								_d_hpresult = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_4();
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
								_d_hpresult = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_5();
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
								_d_hpresult = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_6();
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
								_d_hpresult = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_7();
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
								_d_hpresult = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_8();
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
								_d_hpresult = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_9();
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
								_d_hpresult = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_10();
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
								_d_hpresult = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_11();
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
								_d_hpresult = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_12();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 13:
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
								_d_hpresult = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_13();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 14:
						{
							auto& _d14 = s_hotpatch_entries[14];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d14)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d14))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d14.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_14();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 15:
						{
							auto& _d15 = s_hotpatch_entries[15];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d15)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d15))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d15.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_15();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 16:
						{
							auto& _d16 = s_hotpatch_entries[16];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d16)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d16))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d16.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_16();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 17:
					{
						{
							auto& _d17 = s_hotpatch_entries[17];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d17)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d17))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d17.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ConvertCharPatchEntry_ConvertCharPatchEntry_Subject_17();
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
				chaos_static_ConvertCharPatchEntry_ConvertCharPatchEntry___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}



}  // namespace chaos::il2cpp::codegen::ConvertCharPatchEntry
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 19;

extern "C" void ChaosJitRegisterAll() {}