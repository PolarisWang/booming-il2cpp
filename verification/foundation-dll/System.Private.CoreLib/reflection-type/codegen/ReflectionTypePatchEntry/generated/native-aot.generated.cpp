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

namespace chaos::il2cpp::codegen::ReflectionTypePatchEntry {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_ReflectionTypePatchEntry_ReflectionTypePatchEntry = {{nullptr, nullptr, 650025962648516220ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ReflectionTypePatchEntry_ReflectionTypePatchEntry = static_cast<CHAOS_IL2CPP_INTPTR>(650025962648516220ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Exception = {{nullptr, nullptr, 10972282733316558392ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Exception = static_cast<CHAOS_IL2CPP_INTPTR>(10972282733316558392ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {{nullptr, nullptr, 5474029880995115448ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
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

struct chaos_type_ReflectionTypePatchEntry_ReflectionTypePatchEntry
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Exception
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

CHAOS_IL2CPP_INT32 chaos_static_ReflectionTypePatchEntry_ReflectionTypePatchEntry___exitCode = 0;

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_0(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_1(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_2(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_3(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_4(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_5(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_6(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_7(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_8(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_9(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_10(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_11(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_12(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_13(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_14(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_15(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_16(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_17(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_18(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_19(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_20(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_21(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_22(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_23(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_24(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_25(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_26(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_27(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_28(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_29(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_30(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_31(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_32(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_33(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_34(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_35(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_36(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_37(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_38(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_39(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_40(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_41(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_42(void);
extern "C" void ReflectionTypePatchEntry_ReflectionTypePatchEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[45] = {
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
	1u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[44];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		44u,
		1u,
		524369274u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_0
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_1
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_2
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_3
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_4
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_5
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_6
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_7
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_8
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_9
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_10
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_11
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_12
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_13
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_14
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_15
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_16
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_17
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_18
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_19
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_20
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_21
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_22
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_23
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_24
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_25
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_26
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_27
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_28
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_29
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_30
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_31
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_32
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_33
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_34
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_35
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_36
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_37
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_38
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_39
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_40
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_41
		{ 1u, 0u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_42
		{ 0u, 1u },  // ReflectionTypePatchEntry_ReflectionTypePatchEntry_Run_System_Int32
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
		"ReflectionTypePatchEntry",
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
		&chaos_mt_ReflectionTypePatchEntry_ReflectionTypePatchEntry.hot,
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
		/* .name_utf8         = */ "ReflectionTypePatchEntry",
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
		::chaos::il2cpp::runtime_core::RegisterModule("ReflectionTypePatchEntry", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[44] = {
	{ "Subject_0", 0x00000003u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_1", 0x00000004u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_2", 0x00000005u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_3", 0x00000006u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_4", 0x00000007u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_5", 0x00000008u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_6", 0x00000009u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_7", 0x0000000Au, 0u },  // ReflectionTypePatchEntry
	{ "Subject_8", 0x0000000Bu, 0u },  // ReflectionTypePatchEntry
	{ "Subject_9", 0x0000000Cu, 0u },  // ReflectionTypePatchEntry
	{ "Subject_10", 0x0000000Du, 0u },  // ReflectionTypePatchEntry
	{ "Subject_11", 0x0000000Eu, 0u },  // ReflectionTypePatchEntry
	{ "Subject_12", 0x0000000Fu, 0u },  // ReflectionTypePatchEntry
	{ "Subject_13", 0x00000010u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_14", 0x00000011u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_15", 0x00000012u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_16", 0x00000013u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_17", 0x00000014u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_18", 0x00000015u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_19", 0x00000016u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_20", 0x00000017u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_21", 0x00000018u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_22", 0x00000019u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_23", 0x0000001Au, 0u },  // ReflectionTypePatchEntry
	{ "Subject_24", 0x0000001Bu, 0u },  // ReflectionTypePatchEntry
	{ "Subject_25", 0x0000001Cu, 0u },  // ReflectionTypePatchEntry
	{ "Subject_26", 0x0000001Du, 0u },  // ReflectionTypePatchEntry
	{ "Subject_27", 0x0000001Eu, 0u },  // ReflectionTypePatchEntry
	{ "Subject_28", 0x0000001Fu, 0u },  // ReflectionTypePatchEntry
	{ "Subject_29", 0x00000020u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_30", 0x00000021u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_31", 0x00000022u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_32", 0x00000023u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_33", 0x00000024u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_34", 0x00000025u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_35", 0x00000026u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_36", 0x00000027u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_37", 0x00000028u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_38", 0x00000029u, 0u },  // ReflectionTypePatchEntry
	{ "Subject_39", 0x0000002Au, 0u },  // ReflectionTypePatchEntry
	{ "Subject_40", 0x0000002Bu, 0u },  // ReflectionTypePatchEntry
	{ "Subject_41", 0x0000002Cu, 0u },  // ReflectionTypePatchEntry
	{ "Subject_42", 0x0000002Du, 0u },  // ReflectionTypePatchEntry
	{ "Run", 0x0000002Eu, 1u },  // ReflectionTypePatchEntry
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "ReflectionTypePatchEntry", "", 0u, 44u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[44] = {
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
	{ 0x00000016u, 19u },
	{ 0x00000017u, 20u },
	{ 0x00000018u, 21u },
	{ 0x00000019u, 22u },
	{ 0x0000001Au, 23u },
	{ 0x0000001Bu, 24u },
	{ 0x0000001Cu, 25u },
	{ 0x0000001Du, 26u },
	{ 0x0000001Eu, 27u },
	{ 0x0000001Fu, 28u },
	{ 0x00000020u, 29u },
	{ 0x00000021u, 30u },
	{ 0x00000022u, 31u },
	{ 0x00000023u, 32u },
	{ 0x00000024u, 33u },
	{ 0x00000025u, 34u },
	{ 0x00000026u, 35u },
	{ 0x00000027u, 36u },
	{ 0x00000028u, 37u },
	{ 0x00000029u, 38u },
	{ 0x0000002Au, 39u },
	{ 0x0000002Bu, 40u },
	{ 0x0000002Cu, 41u },
	{ 0x0000002Du, 42u },
	{ 0x0000002Eu, 43u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[44] = {
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_0
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_1
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_2
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_3
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_4
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_5
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_6
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_7
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_8
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_9
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_10
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_11
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_12
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_13
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_14), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_14
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_15), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_15
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_16), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_16
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_17), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_17
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_18), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_18
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_19), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_19
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_20), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_20
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_21), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_21
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_22), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_22
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_23), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_23
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_24), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_24
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_25), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_25
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_26), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_26
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_27), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_27
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_28), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_28
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_29), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_29
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_30), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_30
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_31), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_31
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_32
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_33), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_33
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_34), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_34
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_35), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_35
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_36), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_36
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_37), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_37
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_38), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_38
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_39), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_39
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_40), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_40
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_41), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_41
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_42), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Subject_42
	{ reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Run_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ReflectionTypePatchEntry::Run
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"ReflectionTypePatchEntry",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	44u,
	s_hotpatch_slots,
	44u,
	s_hotpatch_entries,
	44u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[1] = {
	"ReflectionTypePatchEntry/ReflectionTypePatchEntry::_exitCode",
};

extern "C" void* kChaosExternalRuntimeFnTable[1] = {
	nullptr,
};

extern "C" int32_t kChaosExternalRuntimeCount = 1;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[44])() = {
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_0),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_1),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_2),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_3),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_4),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_5),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_6),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_7),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_8),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_9),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_10),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_11),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_12),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_13),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_14),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_15),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_16),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_17),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_18),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_19),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_20),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_21),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_22),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_23),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_24),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_25),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_26),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_27),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_28),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_29),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_30),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_31),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_32),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_33),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_34),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_35),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_36),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_37),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_38),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_39),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_40),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_41),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_42),
	reinterpret_cast<void(*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Run_System_Int32),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
static void (*kBenchmarkWrappers[44])() = {
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
	[]() {kAotMethods[22]();},
	[]() {kAotMethods[23]();},
	[]() {kAotMethods[24]();},
	[]() {kAotMethods[25]();},
	[]() {kAotMethods[26]();},
	[]() {kAotMethods[27]();},
	[]() {kAotMethods[28]();},
	[]() {kAotMethods[29]();},
	[]() {kAotMethods[30]();},
	[]() {kAotMethods[31]();},
	[]() {kAotMethods[32]();},
	[]() {kAotMethods[33]();},
	[]() {kAotMethods[34]();},
	[]() {kAotMethods[35]();},
	[]() {kAotMethods[36]();},
	[]() {kAotMethods[37]();},
	[]() {kAotMethods[38]();},
	[]() {kAotMethods[39]();},
	[]() {kAotMethods[40]();},
	[]() {kAotMethods[41]();},
	[]() {kAotMethods[42]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[43])(0);},
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
static void* const kMethodPointers[44] = {
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_0),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_1),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_2),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_3),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_4),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_5),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_6),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_7),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_8),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_9),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_10),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_11),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_12),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_13),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_14),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_15),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_16),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_17),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_18),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_19),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_20),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_21),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_22),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_23),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_24),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_25),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_26),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_27),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_28),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_29),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_30),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_31),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_32),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_33),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_34),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_35),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_36),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_37),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_38),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_39),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_40),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_41),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_42),
	reinterpret_cast<void*>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Run_System_Int32),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 44u,
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
	.image_name_utf8    = "ReflectionTypePatchEntry",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_ReflectionTypePatchEntry_ReflectionTypePatchEntry[1] = {
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_ReflectionTypePatchEntry_ReflectionTypePatchEntry[44] = {
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_0:System.Int32()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_1:System.Int32()", "Subject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_2:System.Int32()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_3:System.Int32()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_4:System.Int32()", "Subject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_5:System.Int32()", "Subject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_6:System.Int32()", "Subject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_7:System.Int32()", "Subject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_8:System.Int32()", "Subject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_9:System.Int32()", "Subject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_10:System.Int32()", "Subject_10", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_11:System.Int32()", "Subject_11", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_12:System.Int32()", "Subject_12", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_13:System.Int32()", "Subject_13", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_14:System.Int32()", "Subject_14", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_15:System.Int32()", "Subject_15", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_16:System.Int32()", "Subject_16", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_17:System.Int32()", "Subject_17", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_18:System.Int32()", "Subject_18", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_19:System.Int32()", "Subject_19", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_20:System.Int32()", "Subject_20", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_21:System.Int32()", "Subject_21", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_22:System.Int32()", "Subject_22", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_23:System.Int32()", "Subject_23", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_24:System.Int32()", "Subject_24", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_25:System.Int32()", "Subject_25", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_26:System.Int32()", "Subject_26", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_27:System.Int32()", "Subject_27", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_28:System.Int32()", "Subject_28", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_29:System.Int32()", "Subject_29", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_30:System.Int32()", "Subject_30", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_31:System.Int32()", "Subject_31", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_32:System.Int32()", "Subject_32", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_33:System.Int32()", "Subject_33", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_34:System.Int32()", "Subject_34", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_35:System.Int32()", "Subject_35", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_36:System.Int32()", "Subject_36", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_37:System.Int32()", "Subject_37", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_38:System.Int32()", "Subject_38", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_39:System.Int32()", "Subject_39", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_40:System.Int32()", "Subject_40", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_41:System.Int32()", "Subject_41", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_42:System.Int32()", "Subject_42", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry::Run:System.Void(System.Int32)", "Run", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0u, "ReflectionTypePatchEntry/ReflectionTypePatchEntry", "ReflectionTypePatchEntry/ReflectionTypePatchEntry", "", "ReflectionTypePatchEntry", "ReflectionTypePatchEntry", nullptr, kReflFields_ReflectionTypePatchEntry_ReflectionTypePatchEntry, 1u, nullptr, 0u,
	kReflMethods_ReflectionTypePatchEntry_ReflectionTypePatchEntry, 44u },
};

static constexpr const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static constexpr ReflectionQueryImageDescriptor kReflImage = { "ReflectionTypePatchEntry", kReflTypePtrs, 1u };

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
// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_0()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_0(void)
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

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_1()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_1(void)
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

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_2()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_2(void)
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

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_3()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_3(void)
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

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_4()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_4(void)
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

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_5()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_5(void)
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

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_6()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_6(void)
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

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_7()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_7(void)
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

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_8()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_8(void)
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

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_9()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_9(void)
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

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_10()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_10(void)
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

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_11()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_11(void)
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

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_12()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_12(void)
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

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_13()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_13(void)
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

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_14()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_14(void)
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

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_15()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_15(void)
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

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_16()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_16(void)
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

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_17()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_17(void)
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

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_18()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_18(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177262);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_19()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_19(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177261);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_20()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_20(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177260);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_21()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_21(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177259);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_22()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_22(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177258);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_23()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_23(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177257);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_24()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_24(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177256);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_25()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_25(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177255);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_26()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_26(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177254);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_27()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_27(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177253);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_28()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_28(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177252);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_29()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_29(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177251);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_30()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_30(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177250);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_31()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_31(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177249);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_32()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_32(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177248);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_33()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_33(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177247);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_34()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_34(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177246);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_35()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_35(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177245);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_36()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_36(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177244);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_37()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_37(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177243);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_38()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_38(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177242);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_39()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_39(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177241);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_40()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_40(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177240);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_41()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_41(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177239);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Subject_42()
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_42(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1342177238);
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ReflectionTypePatchEntry/ReflectionTypePatchEntry::Run(System.Int32)
extern "C" void ReflectionTypePatchEntry_ReflectionTypePatchEntry_Run_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
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
	CHAOS_IL2CPP_INTPTR _s23{};
	CHAOS_IL2CPP_INTPTR _s24{};
	CHAOS_IL2CPP_INTPTR _s25{};
	CHAOS_IL2CPP_INTPTR _s26{};
	CHAOS_IL2CPP_INTPTR _s27{};
	CHAOS_IL2CPP_INTPTR _s28{};
	CHAOS_IL2CPP_INTPTR _s29{};
	CHAOS_IL2CPP_INTPTR _s30{};
	CHAOS_IL2CPP_INTPTR _s31{};
	CHAOS_IL2CPP_INTPTR _s32{};
	CHAOS_IL2CPP_INTPTR _s33{};
	CHAOS_IL2CPP_INTPTR _s34{};
	CHAOS_IL2CPP_INTPTR _s35{};
	CHAOS_IL2CPP_INTPTR _s36{};
	CHAOS_IL2CPP_INTPTR _s37{};
	CHAOS_IL2CPP_INTPTR _s38{};
	CHAOS_IL2CPP_INTPTR _s39{};
	CHAOS_IL2CPP_INTPTR _s40{};
	CHAOS_IL2CPP_INTPTR _s41{};
	CHAOS_IL2CPP_INTPTR _s42{};
	CHAOS_IL2CPP_INTPTR _s43{};
	CHAOS_IL2CPP_INTPTR _s44{};
	CHAOS_IL2CPP_INTPTR _s45{};
	CHAOS_IL2CPP_INTPTR _s46{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_0();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_1();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_2();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_3();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_4();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_5();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_6();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_7();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_8();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_9();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_10();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_11();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_12();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_13();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_14();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_15();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_16();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 17:
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_17();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 18:
						{
							auto& _d18 = s_hotpatch_entries[18];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d18)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d18))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d18.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_18();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 19:
						{
							auto& _d19 = s_hotpatch_entries[19];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d19)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d19))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d19.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_19();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 20:
						{
							auto& _d20 = s_hotpatch_entries[20];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d20)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d20))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d20.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_20();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 21:
						{
							auto& _d21 = s_hotpatch_entries[21];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d21)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d21))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d21.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_21();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 22:
						{
							auto& _d22 = s_hotpatch_entries[22];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d22)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d22))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d22.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_22();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 23:
						{
							auto& _d23 = s_hotpatch_entries[23];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d23)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d23))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d23.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_23();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 24:
						{
							auto& _d24 = s_hotpatch_entries[24];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d24)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d24))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d24.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_24();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 25:
						{
							auto& _d25 = s_hotpatch_entries[25];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d25)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d25))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d25.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_25();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 26:
						{
							auto& _d26 = s_hotpatch_entries[26];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d26)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d26))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d26.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_26();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 27:
						{
							auto& _d27 = s_hotpatch_entries[27];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d27)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d27))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d27.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_27();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 28:
						{
							auto& _d28 = s_hotpatch_entries[28];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d28)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d28))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d28.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_28();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 29:
						{
							auto& _d29 = s_hotpatch_entries[29];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d29)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d29))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d29.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_29();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 30:
						{
							auto& _d30 = s_hotpatch_entries[30];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d30)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d30))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d30.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_30();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 31:
						{
							auto& _d31 = s_hotpatch_entries[31];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d31)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d31))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d31.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_31();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 32:
						{
							auto& _d32 = s_hotpatch_entries[32];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d32)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d32))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d32.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_32();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 33:
						{
							auto& _d33 = s_hotpatch_entries[33];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d33)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d33))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d33.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_33();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 34:
						{
							auto& _d34 = s_hotpatch_entries[34];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d34)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d34))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d34.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_34();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 35:
						{
							auto& _d35 = s_hotpatch_entries[35];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d35)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d35))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d35.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_35();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 36:
						{
							auto& _d36 = s_hotpatch_entries[36];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d36)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d36))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d36.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_36();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 37:
						{
							auto& _d37 = s_hotpatch_entries[37];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d37)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d37))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d37.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_37();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 38:
						{
							auto& _d38 = s_hotpatch_entries[38];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d38)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d38))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d38.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_38();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 39:
						{
							auto& _d39 = s_hotpatch_entries[39];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d39)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d39))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d39.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_39();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 40:
						{
							auto& _d40 = s_hotpatch_entries[40];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d40)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d40))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d40.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_40();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 41:
						{
							auto& _d41 = s_hotpatch_entries[41];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d41)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d41))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d41.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_41();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 42:
					{
						{
							auto& _d42 = s_hotpatch_entries[42];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d42)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d42))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d42.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_42();
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
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Exception.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionTypePatchEntry_ReflectionTypePatchEntry___exitCode = chaos_value;
			}
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_0();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_1();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_2();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_3();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_4();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_5();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_6();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_7();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_8();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_9();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_10();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_11();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_12();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_13();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_14();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_15();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_16();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 17:
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_17();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 18:
						{
							auto& _d18 = s_hotpatch_entries[18];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d18)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d18))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d18.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_18();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 19:
						{
							auto& _d19 = s_hotpatch_entries[19];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d19)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d19))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d19.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_19();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 20:
						{
							auto& _d20 = s_hotpatch_entries[20];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d20)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d20))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d20.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_20();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 21:
						{
							auto& _d21 = s_hotpatch_entries[21];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d21)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d21))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d21.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_21();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 22:
						{
							auto& _d22 = s_hotpatch_entries[22];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d22)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d22))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d22.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_22();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 23:
						{
							auto& _d23 = s_hotpatch_entries[23];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d23)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d23))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d23.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_23();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 24:
						{
							auto& _d24 = s_hotpatch_entries[24];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d24)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d24))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d24.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_24();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 25:
						{
							auto& _d25 = s_hotpatch_entries[25];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d25)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d25))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d25.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_25();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 26:
						{
							auto& _d26 = s_hotpatch_entries[26];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d26)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d26))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d26.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_26();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 27:
						{
							auto& _d27 = s_hotpatch_entries[27];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d27)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d27))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d27.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_27();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 28:
						{
							auto& _d28 = s_hotpatch_entries[28];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d28)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d28))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d28.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_28();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 29:
						{
							auto& _d29 = s_hotpatch_entries[29];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d29)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d29))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d29.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_29();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 30:
						{
							auto& _d30 = s_hotpatch_entries[30];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d30)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d30))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d30.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_30();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 31:
						{
							auto& _d31 = s_hotpatch_entries[31];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d31)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d31))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d31.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_31();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 32:
						{
							auto& _d32 = s_hotpatch_entries[32];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d32)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d32))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d32.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_32();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 33:
						{
							auto& _d33 = s_hotpatch_entries[33];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d33)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d33))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d33.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_33();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 34:
						{
							auto& _d34 = s_hotpatch_entries[34];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d34)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d34))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d34.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_34();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 35:
						{
							auto& _d35 = s_hotpatch_entries[35];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d35)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d35))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d35.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_35();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 36:
						{
							auto& _d36 = s_hotpatch_entries[36];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d36)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d36))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d36.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_36();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 37:
						{
							auto& _d37 = s_hotpatch_entries[37];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d37)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d37))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d37.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_37();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 38:
						{
							auto& _d38 = s_hotpatch_entries[38];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d38)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d38))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d38.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_38();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 39:
						{
							auto& _d39 = s_hotpatch_entries[39];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d39)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d39))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d39.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_39();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 40:
						{
							auto& _d40 = s_hotpatch_entries[40];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d40)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d40))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d40.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_40();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 41:
						{
							auto& _d41 = s_hotpatch_entries[41];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d41)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d41))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d41.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_41();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 42:
					{
						{
							auto& _d42 = s_hotpatch_entries[42];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d42)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d42))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d42.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_42();
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Exception.hot))
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
				chaos_static_ReflectionTypePatchEntry_ReflectionTypePatchEntry___exitCode = chaos_value;
			}
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_0();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_1();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_2();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_3();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_4();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_5();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_6();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_7();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_8();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_9();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_10();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_11();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_12();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_13();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_14();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_15();
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_16();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 17:
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
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_17();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 18:
						{
							auto& _d18 = s_hotpatch_entries[18];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d18)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d18))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d18.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_18();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 19:
						{
							auto& _d19 = s_hotpatch_entries[19];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d19)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d19))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d19.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_19();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 20:
						{
							auto& _d20 = s_hotpatch_entries[20];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d20)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d20))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d20.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_20();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 21:
						{
							auto& _d21 = s_hotpatch_entries[21];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d21)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d21))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d21.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_21();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 22:
						{
							auto& _d22 = s_hotpatch_entries[22];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d22)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d22))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d22.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_22();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 23:
						{
							auto& _d23 = s_hotpatch_entries[23];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d23)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d23))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d23.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_23();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 24:
						{
							auto& _d24 = s_hotpatch_entries[24];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d24)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d24))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d24.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_24();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 25:
						{
							auto& _d25 = s_hotpatch_entries[25];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d25)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d25))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d25.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_25();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 26:
						{
							auto& _d26 = s_hotpatch_entries[26];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d26)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d26))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d26.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_26();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 27:
						{
							auto& _d27 = s_hotpatch_entries[27];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d27)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d27))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d27.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_27();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 28:
						{
							auto& _d28 = s_hotpatch_entries[28];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d28)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d28))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d28.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_28();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 29:
						{
							auto& _d29 = s_hotpatch_entries[29];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d29)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d29))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d29.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_29();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 30:
						{
							auto& _d30 = s_hotpatch_entries[30];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d30)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d30))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d30.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_30();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 31:
						{
							auto& _d31 = s_hotpatch_entries[31];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d31)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d31))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d31.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_31();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 32:
						{
							auto& _d32 = s_hotpatch_entries[32];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d32)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d32))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d32.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_32();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 33:
						{
							auto& _d33 = s_hotpatch_entries[33];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d33)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d33))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d33.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_33();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 34:
						{
							auto& _d34 = s_hotpatch_entries[34];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d34)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d34))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d34.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_34();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 35:
						{
							auto& _d35 = s_hotpatch_entries[35];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d35)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d35))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d35.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_35();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 36:
						{
							auto& _d36 = s_hotpatch_entries[36];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d36)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d36))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d36.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_36();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 37:
						{
							auto& _d37 = s_hotpatch_entries[37];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d37)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d37))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d37.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_37();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 38:
						{
							auto& _d38 = s_hotpatch_entries[38];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d38)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d38))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d38.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_38();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 39:
						{
							auto& _d39 = s_hotpatch_entries[39];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d39)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d39))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d39.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_39();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 40:
						{
							auto& _d40 = s_hotpatch_entries[40];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d40)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d40))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d40.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_40();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 41:
						{
							auto& _d41 = s_hotpatch_entries[41];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d41)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d41))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d41.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_41();
							}
							_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
						}
					case 42:
					{
						{
							auto& _d42 = s_hotpatch_entries[42];
							CHAOS_IL2CPP_INT32 _d_hpresult{};
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d42)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d42))
							{
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d42.method_key, nullptr, &_d_hpresult);
							}
							else
							{
								_d_hpresult = ReflectionTypePatchEntry_ReflectionTypePatchEntry_Subject_42();
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Exception.hot))
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
				chaos_static_ReflectionTypePatchEntry_ReflectionTypePatchEntry___exitCode = chaos_value;
			}
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}



}  // namespace chaos::il2cpp::codegen::ReflectionTypePatchEntry
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 44;