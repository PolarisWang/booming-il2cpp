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

namespace chaos::il2cpp::codegen::ReaderSubjects {

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

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_0(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_1(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_2(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_3(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_4(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_5(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_6(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_7(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_8(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_9(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_10(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_11(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_12(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_13(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_14(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_15(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_16(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_17(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_18(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_19(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_20(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_21(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_22(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_23(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_24(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_25(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_26(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_27(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_28(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_29(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_30(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_31(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_32(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_33(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_34(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_35(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_36(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_37(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_38(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_39(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_40(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_41(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_42(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_43(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_44(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_45(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_46(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_47(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_48(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_49(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_50(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_51(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_52(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_53(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_54(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_55(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_56(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_57(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_58(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_59(void);
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_60(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[62] = {
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
	::ChaosAbiMethodEntryV0 entries[61];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		61u,
		0u,
		1826613549u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_0
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_1
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_2
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_3
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_4
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_5
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_6
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_7
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_8
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_9
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_10
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_11
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_12
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_13
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_14
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_15
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_16
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_17
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_18
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_19
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_20
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_21
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_22
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_23
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_24
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_25
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_26
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_27
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_28
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_29
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_30
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_31
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_32
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_33
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_34
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_35
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_36
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_37
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_38
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_39
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_40
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_41
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_42
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_43
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_44
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_45
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_46
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_47
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_48
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_49
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_50
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_51
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_52
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_53
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_54
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_55
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_56
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_57
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_58
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_59
		{ 0u, 0u },  // ReaderSubjects_ReaderSubjects_CustomEntrySubject_60
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
		"ReaderSubjects",
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
		nullptr,
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
		/* .name_utf8         = */ "ReaderSubjects",
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
		::chaos::il2cpp::runtime_core::RegisterModule("ReaderSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[61] = {
	{ "CustomEntrySubject_0", 0x00000003u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_1", 0x00000004u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_2", 0x00000005u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_3", 0x00000006u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_4", 0x00000007u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_5", 0x00000008u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_6", 0x00000009u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_7", 0x0000000Au, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_8", 0x0000000Bu, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_9", 0x0000000Cu, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_10", 0x0000000Du, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_11", 0x0000000Eu, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_12", 0x0000000Fu, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_13", 0x00000010u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_14", 0x00000011u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_15", 0x00000012u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_16", 0x00000013u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_17", 0x00000014u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_18", 0x00000015u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_19", 0x00000016u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_20", 0x00000017u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_21", 0x00000018u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_22", 0x00000019u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_23", 0x0000001Au, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_24", 0x0000001Bu, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_25", 0x0000001Cu, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_26", 0x0000001Du, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_27", 0x0000001Eu, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_28", 0x0000001Fu, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_29", 0x00000020u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_30", 0x00000021u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_31", 0x00000022u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_32", 0x00000023u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_33", 0x00000024u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_34", 0x00000025u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_35", 0x00000026u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_36", 0x00000027u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_37", 0x00000028u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_38", 0x00000029u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_39", 0x0000002Au, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_40", 0x0000002Bu, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_41", 0x0000002Cu, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_42", 0x0000002Du, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_43", 0x0000002Eu, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_44", 0x0000002Fu, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_45", 0x00000030u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_46", 0x00000031u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_47", 0x00000032u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_48", 0x00000033u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_49", 0x00000034u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_50", 0x00000035u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_51", 0x00000036u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_52", 0x00000037u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_53", 0x00000038u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_54", 0x00000039u, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_55", 0x0000003Au, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_56", 0x0000003Bu, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_57", 0x0000003Cu, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_58", 0x0000003Du, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_59", 0x0000003Eu, 0u },  // ReaderSubjects
	{ "CustomEntrySubject_60", 0x0000003Fu, 0u },  // ReaderSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "ReaderSubjects", "", 0u, 61u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[61] = {
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
	{ 0x0000002Fu, 44u },
	{ 0x00000030u, 45u },
	{ 0x00000031u, 46u },
	{ 0x00000032u, 47u },
	{ 0x00000033u, 48u },
	{ 0x00000034u, 49u },
	{ 0x00000035u, 50u },
	{ 0x00000036u, 51u },
	{ 0x00000037u, 52u },
	{ 0x00000038u, 53u },
	{ 0x00000039u, 54u },
	{ 0x0000003Au, 55u },
	{ 0x0000003Bu, 56u },
	{ 0x0000003Cu, 57u },
	{ 0x0000003Du, 58u },
	{ 0x0000003Eu, 59u },
	{ 0x0000003Fu, 60u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[61] = {
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_0
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_1
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_2
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_3
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_4
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_5
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_6
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_7
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_8
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_9
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_10
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_11
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_12
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_13
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_14), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_14
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_15), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_15
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_16), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_16
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_17), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_17
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_18), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_18
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_19), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_19
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_20), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_20
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_21), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_21
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_22), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_22
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_23), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_23
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_24), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_24
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_25), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_25
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_26), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_26
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_27), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_27
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_28), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_28
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_29), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_29
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_30), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_30
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_31), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_31
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_32
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_33), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_33
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_34), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_34
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_35), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_35
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_36), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_36
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_37), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_37
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_38), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_38
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_39), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_39
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_40), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_40
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_41), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_41
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_42), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_42
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_43), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_43
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_44), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_44
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_45), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_45
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_46), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_46
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_47), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_47
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_48), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_48
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_49), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_49
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_50), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_50
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_51), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_51
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_52), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_52
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_53), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_53
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_54), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_54
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_55), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_55
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_56), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_56
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_57), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_57
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_58), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_58
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_59), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_59
	{ reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_60), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReaderSubjects::CustomEntrySubject_60
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"ReaderSubjects",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	61u,
	s_hotpatch_slots,
	61u,
	s_hotpatch_entries,
	61u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table (empty) ─────────────────
extern "C" const char* kChaosExternalRuntimeSubjects[1] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[1] = { nullptr };
extern "C" int32_t kChaosExternalRuntimeCount = 0;

// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[61])() = {
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_0),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_1),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_2),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_3),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_4),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_5),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_6),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_7),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_8),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_9),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_10),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_11),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_12),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_13),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_14),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_15),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_16),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_17),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_18),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_19),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_20),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_21),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_22),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_23),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_24),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_25),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_26),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_27),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_28),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_29),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_30),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_31),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_32),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_33),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_34),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_35),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_36),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_37),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_38),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_39),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_40),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_41),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_42),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_43),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_44),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_45),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_46),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_47),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_48),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_49),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_50),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_51),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_52),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_53),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_54),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_55),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_56),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_57),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_58),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_59),
	reinterpret_cast<void(*)()>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_60),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[61])() = {
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
	[]() {kAotMethods[43]();},
	[]() {kAotMethods[44]();},
	[]() {kAotMethods[45]();},
	[]() {kAotMethods[46]();},
	[]() {kAotMethods[47]();},
	[]() {kAotMethods[48]();},
	[]() {kAotMethods[49]();},
	[]() {kAotMethods[50]();},
	[]() {kAotMethods[51]();},
	[]() {kAotMethods[52]();},
	[]() {kAotMethods[53]();},
	[]() {kAotMethods[54]();},
	[]() {kAotMethods[55]();},
	[]() {kAotMethods[56]();},
	[]() {kAotMethods[57]();},
	[]() {kAotMethods[58]();},
	[]() {kAotMethods[59]();},
	[]() {kAotMethods[60]();},
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 61;
extern "C" const int kSubjectEntryIndices[61] = {
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
	18,
	19,
	20,
	21,
	22,
	23,
	24,
	25,
	26,
	27,
	28,
	29,
	30,
	31,
	32,
	33,
	34,
	35,
	36,
	37,
	38,
	39,
	40,
	41,
	42,
	43,
	44,
	45,
	46,
	47,
	48,
	49,
	50,
	51,
	52,
	53,
	54,
	55,
	56,
	57,
	58,
	59,
	60,
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
// ── GC Slot Map Section (empty) ───────────────────────────────
static const CHAOS_IL2CPP_UINT8 kChaosGcSlotMapsSection[1] = { 0 };
static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 0;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[61] = {
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_0),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_1),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_2),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_3),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_4),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_5),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_6),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_7),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_8),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_9),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_10),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_11),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_12),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_13),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_14),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_15),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_16),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_17),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_18),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_19),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_20),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_21),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_22),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_23),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_24),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_25),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_26),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_27),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_28),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_29),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_30),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_31),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_32),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_33),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_34),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_35),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_36),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_37),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_38),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_39),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_40),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_41),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_42),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_43),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_44),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_45),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_46),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_47),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_48),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_49),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_50),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_51),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_52),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_53),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_54),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_55),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_56),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_57),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_58),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_59),
	reinterpret_cast<void*>(&ReaderSubjects_ReaderSubjects_CustomEntrySubject_60),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 61u,
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
	.slot_map_section_begin = nullptr,
	.slot_map_section_end   = nullptr,
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
	.image_name_utf8    = "ReaderSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_ReaderSubjects_ReaderSubjects[1] = {
	{ 0u, "ReaderSubjects/ReaderSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_ReaderSubjects_ReaderSubjects[61] = {
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_0:System.Void()", "CustomEntrySubject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_1:System.Void()", "CustomEntrySubject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_2:System.Void()", "CustomEntrySubject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_3:System.Void()", "CustomEntrySubject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_4:System.Void()", "CustomEntrySubject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_5:System.Void()", "CustomEntrySubject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_6:System.Void()", "CustomEntrySubject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_7:System.Void()", "CustomEntrySubject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_8:System.Void()", "CustomEntrySubject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_9:System.Void()", "CustomEntrySubject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_10:System.Void()", "CustomEntrySubject_10", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_11:System.Void()", "CustomEntrySubject_11", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_12:System.Void()", "CustomEntrySubject_12", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_13:System.Void()", "CustomEntrySubject_13", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_14:System.Void()", "CustomEntrySubject_14", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_15:System.Void()", "CustomEntrySubject_15", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_16:System.Void()", "CustomEntrySubject_16", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_17:System.Void()", "CustomEntrySubject_17", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_18:System.Void()", "CustomEntrySubject_18", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_19:System.Void()", "CustomEntrySubject_19", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_20:System.Void()", "CustomEntrySubject_20", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_21:System.Void()", "CustomEntrySubject_21", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_22:System.Void()", "CustomEntrySubject_22", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_23:System.Void()", "CustomEntrySubject_23", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_24:System.Void()", "CustomEntrySubject_24", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_25:System.Void()", "CustomEntrySubject_25", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_26:System.Void()", "CustomEntrySubject_26", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_27:System.Void()", "CustomEntrySubject_27", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_28:System.Void()", "CustomEntrySubject_28", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_29:System.Void()", "CustomEntrySubject_29", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_30:System.Void()", "CustomEntrySubject_30", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_31:System.Void()", "CustomEntrySubject_31", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_32:System.Void()", "CustomEntrySubject_32", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_33:System.Void()", "CustomEntrySubject_33", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_34:System.Void()", "CustomEntrySubject_34", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_35:System.Void()", "CustomEntrySubject_35", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_36:System.Void()", "CustomEntrySubject_36", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_37:System.Void()", "CustomEntrySubject_37", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_38:System.Void()", "CustomEntrySubject_38", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_39:System.Void()", "CustomEntrySubject_39", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_40:System.Void()", "CustomEntrySubject_40", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_41:System.Void()", "CustomEntrySubject_41", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_42:System.Void()", "CustomEntrySubject_42", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_43:System.Void()", "CustomEntrySubject_43", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_44:System.Void()", "CustomEntrySubject_44", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_45:System.Void()", "CustomEntrySubject_45", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_46:System.Void()", "CustomEntrySubject_46", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_47:System.Void()", "CustomEntrySubject_47", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_48:System.Void()", "CustomEntrySubject_48", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_49:System.Void()", "CustomEntrySubject_49", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_50:System.Void()", "CustomEntrySubject_50", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_51:System.Void()", "CustomEntrySubject_51", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_52:System.Void()", "CustomEntrySubject_52", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_53:System.Void()", "CustomEntrySubject_53", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_54:System.Void()", "CustomEntrySubject_54", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_55:System.Void()", "CustomEntrySubject_55", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_56:System.Void()", "CustomEntrySubject_56", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_57:System.Void()", "CustomEntrySubject_57", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_58:System.Void()", "CustomEntrySubject_58", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_59:System.Void()", "CustomEntrySubject_59", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReaderSubjects/ReaderSubjects::CustomEntrySubject_60:System.Void()", "CustomEntrySubject_60", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0u, "ReaderSubjects/ReaderSubjects", "ReaderSubjects/ReaderSubjects", "", "ReaderSubjects", "ReaderSubjects", nullptr, kReflFields_ReaderSubjects_ReaderSubjects, 1u, nullptr, 0u,
	kReflMethods_ReaderSubjects_ReaderSubjects, 61u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "ReaderSubjects", kReflTypePtrs, 1u };

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
// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_0()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_1()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_2()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_3()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_4()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_5()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_6()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_6(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_7()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_7(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_8()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_9()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_9(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_10()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_10(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_11()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_11(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_12()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_12(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_13()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_13(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_14()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_14(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_15()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_15(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_16()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_16(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_17()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_17(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_18()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_18(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_19()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_19(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_20()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_20(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_21()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_21(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_22()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_22(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_23()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_23(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_24()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_24(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_25()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_25(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_26()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_26(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_27()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_27(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_28()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_28(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_29()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_29(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_30()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_30(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_31()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_31(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_32()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_32(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_33()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_33(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_34()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_34(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_35()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_35(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_36()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_36(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_37()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_37(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_38()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_38(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_39()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_39(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_40()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_40(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_41()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_41(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_42()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_42(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_43()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_43(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_44()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_44(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_45()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_45(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_46()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_46(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_47()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_47(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_48()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_48(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_49()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_49(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_50()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_50(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_51()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_51(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_52()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_52(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_53()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_53(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_54()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_54(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_55()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_55(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_56()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_56(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_57()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_57(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_58()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_58(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_59()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_59(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ReaderSubjects/ReaderSubjects::CustomEntrySubject_60()
extern "C" void ReaderSubjects_ReaderSubjects_CustomEntrySubject_60(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}



}  // namespace chaos::il2cpp::codegen::ReaderSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 61;

// ── JIT Method Entry Table ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kMethodJson_0[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-0\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_0:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_0()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_0:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_0:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-0\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_0()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_1[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-1\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_1:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_1()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_1:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_1:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-1\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_1()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_2[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-2\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_2:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_2()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_2:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_2:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-2\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_2()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_2\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_3[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-3\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_3:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_3()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_3:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_3:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-3\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_3()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_3\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_4[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-4\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_4:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_4()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_4:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_4:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-4\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_4()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_4\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_5[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-5\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_5:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_5()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_5:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_5:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-5\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_5()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_5\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_6[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-6\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_6:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_6()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_6:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_6:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-6\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_6()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_6\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_7[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-7\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_7:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_7()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_7:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_7:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-7\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_7()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_7\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_8[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-8\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_8:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_8()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_8:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_8:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-8\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_8()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_8\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_9[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-9\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_9:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_9()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_9:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_9:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-9\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_9()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_9\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_10[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-10\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_10:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_10()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_10:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_10:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-10\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_10()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_10\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_11[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-11\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_11:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_11()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_11:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_11:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-11\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_11()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_11\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_12[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-12\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_12:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_12()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_12:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_12:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-12\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_12()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_12\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_13[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-13\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_13:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_13()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_13:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_13:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-13\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_13()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_13\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_14[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-14\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_14:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_14()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_14:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_14:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-14\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_14()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_14\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_15[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-15\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_15:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_15()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_15:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_15:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-15\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_15()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_15\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_16[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-16\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_16:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_16()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_16:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_16:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-16\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_16()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_16\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_17[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-17\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_17:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_17()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_17:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_17:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-17\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_17()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_17\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_18[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-18\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_18:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_18()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_18:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_18:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-18\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_18()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_18\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_19[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-19\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_19:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_19()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_19:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_19:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-19\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_19()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_19\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_20[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-20\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_20:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_20()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_20:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_20:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-20\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_20()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_20\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_21[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-21\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_21:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_21()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_21:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_21:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-21\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_21()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_21\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_22[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-22\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_22:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_22()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_22:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_22:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-22\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_22()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_22\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_23[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-23\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_23:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_23()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_23:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_23:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-23\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_23()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_23\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_24[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-24\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_24:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_24()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_24:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_24:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-24\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_24()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_24\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_25[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-25\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_25:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_25()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_25:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_25:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-25\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_25()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_25\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_26[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-26\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_26:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_26()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_26:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_26:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-26\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_26()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_26\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_27[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-27\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_27:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_27()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_27:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_27:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-27\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_27()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_27\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_28[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-28\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_28:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_28()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_28:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_28:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-28\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_28()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_28\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_29[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-29\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_29:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_29()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_29:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_29:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-29\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_29()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_29\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_30[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-30\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_30:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_30()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_30:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_30:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-30\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_30()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_30\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_31[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-31\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_31:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_31()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_31:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_31:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-31\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_31()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_31\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_32[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-32\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_32:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_32()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_32:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_32:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-32\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_32()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_32\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_33[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-33\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_33:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_33()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_33:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_33:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-33\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_33()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_33\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_34[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-34\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_34:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_34()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_34:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_34:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-34\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_34()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_34\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_35[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-35\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_35:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_35()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_35:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_35:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-35\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_35()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_35\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_36[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-36\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_36:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_36()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_36:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_36:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-36\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_36()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_36\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_37[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-37\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_37:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_37()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_37:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_37:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-37\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_37()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_37\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_38[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-38\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_38:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_38()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_38:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_38:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-38\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_38()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_38\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_39[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-39\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_39:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_39()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_39:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_39:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-39\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_39()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_39\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_40[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-40\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_40:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_40()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_40:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_40:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-40\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_40()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_40\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_41[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-41\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_41:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_41()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_41:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_41:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-41\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_41()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_41\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_42[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-42\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_42:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_42()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_42:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_42:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-42\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_42()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_42\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_43[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-43\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_43:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_43()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_43:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_43:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-43\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_43()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_43\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_44[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-44\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_44:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_44()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_44:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_44:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-44\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_44()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_44\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_45[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-45\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_45:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_45()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_45:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_45:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-45\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_45()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_45\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_46[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-46\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_46:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_46()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_46:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_46:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-46\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_46()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_46\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_47[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-47\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_47:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_47()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_47:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_47:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-47\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_47()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_47\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_48[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-48\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_48:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_48()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_48:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_48:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-48\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_48()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_48\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_49[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-49\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_49:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_49()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_49:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_49:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-49\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_49()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_49\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_50[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-50\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_50:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_50()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_50:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_50:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-50\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_50()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_50\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_51[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-51\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_51:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_51()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_51:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_51:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-51\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_51()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_51\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_52[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-52\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_52:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_52()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_52:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_52:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-52\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_52()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_52\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_53[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-53\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_53:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_53()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_53:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_53:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-53\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_53()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_53\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_54[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-54\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_54:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_54()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_54:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_54:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-54\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_54()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_54\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_55[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-55\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_55:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_55()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_55:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_55:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-55\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_55()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_55\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_56[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-56\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_56:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_56()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_56:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_56:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-56\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_56()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_56\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_57[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-57\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_57:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_57()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_57:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_57:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-57\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_57()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_57\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_58[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-58\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_58:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_58()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_58:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_58:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-58\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_58()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_58\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_59[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-59\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_59:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_59()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_59:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_59:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-59\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_59()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_59\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";
static const char kMethodJson_60[] = "{\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-60\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_60:System.Void()\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_60()\",\"identity\":{\"assemblyName\":\"ReaderSubjects\",\"declaringTypeSubjectId\":\"ReaderSubjects/ReaderSubjects\",\"definitionSubjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_60:System.Void()\",\"subjectId\":\"ReaderSubjects/ReaderSubjects::CustomEntrySubject_60:System.Void()\",\"methodId\":\"reader-subjects.reader-subjects.custom-entry-subject-60\",\"signature\":\"System.Void ReaderSubjects::CustomEntrySubject_60()\"},\"nativeSymbol\":\"ReaderSubjects_ReaderSubjects_CustomEntrySubject_60\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReaderSubjects\"}";

extern "C" const JitMethodEntry kChaosJitMethodEntries[61] =
{
    { kMethodJson_0, 1205u, 0x00000003u, 0u },
    { kMethodJson_1, 1205u, 0x00000004u, 0u },
    { kMethodJson_2, 1205u, 0x00000005u, 0u },
    { kMethodJson_3, 1205u, 0x00000006u, 0u },
    { kMethodJson_4, 1205u, 0x00000007u, 0u },
    { kMethodJson_5, 1205u, 0x00000008u, 0u },
    { kMethodJson_6, 1205u, 0x00000009u, 0u },
    { kMethodJson_7, 1205u, 0x0000000Au, 0u },
    { kMethodJson_8, 1205u, 0x0000000Bu, 0u },
    { kMethodJson_9, 1205u, 0x0000000Cu, 0u },
    { kMethodJson_10, 1213u, 0x0000000Du, 0u },
    { kMethodJson_11, 1213u, 0x0000000Eu, 0u },
    { kMethodJson_12, 1213u, 0x0000000Fu, 0u },
    { kMethodJson_13, 1213u, 0x00000010u, 0u },
    { kMethodJson_14, 1213u, 0x00000011u, 0u },
    { kMethodJson_15, 1213u, 0x00000012u, 0u },
    { kMethodJson_16, 1213u, 0x00000013u, 0u },
    { kMethodJson_17, 1213u, 0x00000014u, 0u },
    { kMethodJson_18, 1213u, 0x00000015u, 0u },
    { kMethodJson_19, 1213u, 0x00000016u, 0u },
    { kMethodJson_20, 1213u, 0x00000017u, 0u },
    { kMethodJson_21, 1213u, 0x00000018u, 0u },
    { kMethodJson_22, 1213u, 0x00000019u, 0u },
    { kMethodJson_23, 1213u, 0x0000001Au, 0u },
    { kMethodJson_24, 1213u, 0x0000001Bu, 0u },
    { kMethodJson_25, 1213u, 0x0000001Cu, 0u },
    { kMethodJson_26, 1213u, 0x0000001Du, 0u },
    { kMethodJson_27, 1213u, 0x0000001Eu, 0u },
    { kMethodJson_28, 1213u, 0x0000001Fu, 0u },
    { kMethodJson_29, 1213u, 0x00000020u, 0u },
    { kMethodJson_30, 1213u, 0x00000021u, 0u },
    { kMethodJson_31, 1213u, 0x00000022u, 0u },
    { kMethodJson_32, 1213u, 0x00000023u, 0u },
    { kMethodJson_33, 1213u, 0x00000024u, 0u },
    { kMethodJson_34, 1213u, 0x00000025u, 0u },
    { kMethodJson_35, 1213u, 0x00000026u, 0u },
    { kMethodJson_36, 1213u, 0x00000027u, 0u },
    { kMethodJson_37, 1213u, 0x00000028u, 0u },
    { kMethodJson_38, 1213u, 0x00000029u, 0u },
    { kMethodJson_39, 1213u, 0x0000002Au, 0u },
    { kMethodJson_40, 1213u, 0x0000002Bu, 0u },
    { kMethodJson_41, 1213u, 0x0000002Cu, 0u },
    { kMethodJson_42, 1213u, 0x0000002Du, 0u },
    { kMethodJson_43, 1213u, 0x0000002Eu, 0u },
    { kMethodJson_44, 1213u, 0x0000002Fu, 0u },
    { kMethodJson_45, 1213u, 0x00000030u, 0u },
    { kMethodJson_46, 1213u, 0x00000031u, 0u },
    { kMethodJson_47, 1213u, 0x00000032u, 0u },
    { kMethodJson_48, 1213u, 0x00000033u, 0u },
    { kMethodJson_49, 1213u, 0x00000034u, 0u },
    { kMethodJson_50, 1213u, 0x00000035u, 0u },
    { kMethodJson_51, 1213u, 0x00000036u, 0u },
    { kMethodJson_52, 1213u, 0x00000037u, 0u },
    { kMethodJson_53, 1213u, 0x00000038u, 0u },
    { kMethodJson_54, 1213u, 0x00000039u, 0u },
    { kMethodJson_55, 1213u, 0x0000003Au, 0u },
    { kMethodJson_56, 1213u, 0x0000003Bu, 0u },
    { kMethodJson_57, 1213u, 0x0000003Cu, 0u },
    { kMethodJson_58, 1213u, 0x0000003Du, 0u },
    { kMethodJson_59, 1213u, 0x0000003Eu, 0u },
    { kMethodJson_60, 1213u, 0x0000003Fu, 0u }
};

extern "C" const uint32_t kChaosJitMethodEntryCount = 61u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitMethods(kChaosJitMethodEntries, kChaosJitMethodEntryCount);
}