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

namespace chaos::il2cpp::codegen::ReflectionMemberCompleteSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects = {{nullptr, nullptr, 3755966391797372016ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(3755966391797372016ULL);
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

struct chaos_type_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects
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

CHAOS_IL2CPP_INT32 chaos_static_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects___exitCode = 0;

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Int32__GetHashCode_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosInt32GetHashCode(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT64 chaos_external_runtime_System_Private_CoreLib_System_Reflection_FieldInfo__get_FieldHandle_System_RuntimeFieldHandle__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionFieldGetFieldHandle(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Reflection_FieldInfo__get_IsInitOnly_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionFieldGetIsInitOnly(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Reflection_FieldInfo__get_IsLiteral_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionFieldGetIsLiteral(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Reflection_FieldInfo__get_IsStatic_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	(void)chaos_arg_0;
	return static_cast<CHAOS_IL2CPP_INT32>(0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_DeclaringType_System_Type__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetDeclaringType(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_MetadataToken_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetMetadataToken(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_Name_System_String__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetMemberName(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_ReflectedType_System_Type__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetReflectedType(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodBase__get_CallingConvention_System_Reflection_CallingConventions__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetCallingConvention(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodBase__get_IsPublic_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetIsPublic(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodBase__get_IsStatic_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	(void)chaos_arg_0;
	return static_cast<CHAOS_IL2CPP_INT32>(0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodBase__get_IsVirtual_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetIsVirtual(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodBase__get_MethodHandle_System_RuntimeMethodHandle__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetMethodHandle(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodInfo__GetBaseDefinition_System_Reflection_MethodInfo__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetBaseDefinition(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Reflection_PropertyInfo__GetIndexParameters_System_Reflection_ParameterInfo____(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionPropertyGetIndexParameters(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Reflection_PropertyInfo__get_CanRead_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionPropertyGetCanRead(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Reflection_PropertyInfo__get_CanWrite_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionPropertyGetCanWrite(chaos_fn_arg_0);
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_0(void);
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_1(void);
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_2(void);
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_3(void);
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_4(void);
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_5(void);
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_6(void);
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_7(void);
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_8(void);
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_9(void);
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_10(void);
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_11(void);
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_12(void);
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_13(void);
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_14(void);
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_15(void);
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_16(void);
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_17(void);
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_18(void);
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_19(void);
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_20(void);
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_21(void);


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
		{ 0u, 0u },  // ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_0
		{ 0u, 0u },  // ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_1
		{ 0u, 0u },  // ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_2
		{ 0u, 0u },  // ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_3
		{ 0u, 0u },  // ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_4
		{ 0u, 0u },  // ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_5
		{ 0u, 0u },  // ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_6
		{ 0u, 0u },  // ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_7
		{ 0u, 0u },  // ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_8
		{ 0u, 0u },  // ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_9
		{ 0u, 0u },  // ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_10
		{ 0u, 0u },  // ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_11
		{ 0u, 0u },  // ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_12
		{ 0u, 0u },  // ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_13
		{ 0u, 0u },  // ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_14
		{ 0u, 0u },  // ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_15
		{ 0u, 0u },  // ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_16
		{ 0u, 0u },  // ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_17
		{ 0u, 0u },  // ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_18
		{ 0u, 0u },  // ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_19
		{ 0u, 0u },  // ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_20
		{ 0u, 0u },  // ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_21
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
		"ReflectionMemberCompleteSubjects",
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
		&chaos_mt_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects.hot,
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
		/* .name_utf8         = */ "ReflectionMemberCompleteSubjects",
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
		::chaos::il2cpp::runtime_core::RegisterModule("ReflectionMemberCompleteSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[22] = {
	{ "Subject_0", 0x00000003u, 0u },  // ReflectionMemberCompleteSubjects
	{ "Subject_1", 0x00000004u, 0u },  // ReflectionMemberCompleteSubjects
	{ "Subject_2", 0x00000005u, 0u },  // ReflectionMemberCompleteSubjects
	{ "Subject_3", 0x00000006u, 0u },  // ReflectionMemberCompleteSubjects
	{ "Subject_4", 0x00000007u, 0u },  // ReflectionMemberCompleteSubjects
	{ "Subject_5", 0x00000008u, 0u },  // ReflectionMemberCompleteSubjects
	{ "Subject_6", 0x00000009u, 0u },  // ReflectionMemberCompleteSubjects
	{ "Subject_7", 0x0000000Au, 0u },  // ReflectionMemberCompleteSubjects
	{ "Subject_8", 0x0000000Bu, 0u },  // ReflectionMemberCompleteSubjects
	{ "Subject_9", 0x0000000Cu, 0u },  // ReflectionMemberCompleteSubjects
	{ "Subject_10", 0x0000000Du, 0u },  // ReflectionMemberCompleteSubjects
	{ "Subject_11", 0x0000000Eu, 0u },  // ReflectionMemberCompleteSubjects
	{ "Subject_12", 0x0000000Fu, 0u },  // ReflectionMemberCompleteSubjects
	{ "Subject_13", 0x00000010u, 0u },  // ReflectionMemberCompleteSubjects
	{ "Subject_14", 0x00000011u, 0u },  // ReflectionMemberCompleteSubjects
	{ "Subject_15", 0x00000012u, 0u },  // ReflectionMemberCompleteSubjects
	{ "Subject_16", 0x00000013u, 0u },  // ReflectionMemberCompleteSubjects
	{ "Subject_17", 0x00000014u, 0u },  // ReflectionMemberCompleteSubjects
	{ "Subject_18", 0x00000015u, 0u },  // ReflectionMemberCompleteSubjects
	{ "Subject_19", 0x00000016u, 0u },  // ReflectionMemberCompleteSubjects
	{ "Subject_20", 0x00000017u, 0u },  // ReflectionMemberCompleteSubjects
	{ "Subject_21", 0x00000018u, 0u },  // ReflectionMemberCompleteSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "ReflectionMemberCompleteSubjects", "", 0u, 22u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[22] = {
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
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[22] = {
	{ reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionMemberCompleteSubjects::Subject_0
	{ reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionMemberCompleteSubjects::Subject_1
	{ reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionMemberCompleteSubjects::Subject_2
	{ reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionMemberCompleteSubjects::Subject_3
	{ reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionMemberCompleteSubjects::Subject_4
	{ reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionMemberCompleteSubjects::Subject_5
	{ reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionMemberCompleteSubjects::Subject_6
	{ reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionMemberCompleteSubjects::Subject_7
	{ reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionMemberCompleteSubjects::Subject_8
	{ reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionMemberCompleteSubjects::Subject_9
	{ reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionMemberCompleteSubjects::Subject_10
	{ reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionMemberCompleteSubjects::Subject_11
	{ reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionMemberCompleteSubjects::Subject_12
	{ reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionMemberCompleteSubjects::Subject_13
	{ reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_14), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionMemberCompleteSubjects::Subject_14
	{ reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_15), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionMemberCompleteSubjects::Subject_15
	{ reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_16), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionMemberCompleteSubjects::Subject_16
	{ reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_17), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionMemberCompleteSubjects::Subject_17
	{ reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_18), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionMemberCompleteSubjects::Subject_18
	{ reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_19), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionMemberCompleteSubjects::Subject_19
	{ reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_20), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionMemberCompleteSubjects::Subject_20
	{ reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_21), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ReflectionMemberCompleteSubjects::Subject_21
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"ReflectionMemberCompleteSubjects",
	s_hotpatch_types,
	1u,
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

extern "C" const char* kChaosExternalRuntimeSubjects[21] = {
	"System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()",
	"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode",
	"System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes()",
	"System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()",
	"System.Private.CoreLib/System.Reflection.MemberInfo::get_ReflectedType:System.Type()",
	"System.Private.CoreLib/System.Reflection.MemberInfo::get_MetadataToken:System.Int32()",
	"System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()",
	"System.Private.CoreLib/System.Reflection.MethodInfo::GetBaseDefinition:System.Reflection.MethodInfo()",
	"System.Private.CoreLib/System.Reflection.MethodBase::get_MethodHandle:System.RuntimeMethodHandle()",
	"System.Private.CoreLib/System.Reflection.MethodBase::get_CallingConvention:System.Reflection.CallingConventions()",
	"System.Private.CoreLib/System.Reflection.MethodBase::get_IsPublic:System.Boolean()",
	"System.Private.CoreLib/System.Reflection.MethodBase::get_IsStatic:System.Boolean()",
	"System.Private.CoreLib/System.Reflection.MethodBase::get_IsVirtual:System.Boolean()",
	"System.Private.CoreLib/System.Int32::GetHashCode:System.Int32()",
	"System.Private.CoreLib/System.Reflection.FieldInfo::get_IsStatic:System.Boolean()",
	"System.Private.CoreLib/System.Reflection.FieldInfo::get_IsInitOnly:System.Boolean()",
	"System.Private.CoreLib/System.Reflection.FieldInfo::get_IsLiteral:System.Boolean()",
	"System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanRead:System.Boolean()",
	"System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanWrite:System.Boolean()",
	"System.Private.CoreLib/System.Reflection.PropertyInfo::GetIndexParameters:System.Reflection.ParameterInfo[]()",
	"System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldHandle:System.RuntimeFieldHandle()",
};

extern "C" void* kChaosExternalRuntimeFnTable[21] = {
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_Name_System_String__),
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_DeclaringType_System_Type__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_ReflectedType_System_Type__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Reflection_MemberInfo__get_MetadataToken_System_Int32__),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodInfo__GetBaseDefinition_System_Reflection_MethodInfo__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodBase__get_MethodHandle_System_RuntimeMethodHandle__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodBase__get_CallingConvention_System_Reflection_CallingConventions__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodBase__get_IsPublic_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodBase__get_IsStatic_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodBase__get_IsVirtual_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Int32__GetHashCode_System_Int32__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Reflection_FieldInfo__get_IsStatic_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Reflection_FieldInfo__get_IsInitOnly_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Reflection_FieldInfo__get_IsLiteral_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Reflection_PropertyInfo__get_CanRead_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Reflection_PropertyInfo__get_CanWrite_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Reflection_PropertyInfo__GetIndexParameters_System_Reflection_ParameterInfo____),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Reflection_FieldInfo__get_FieldHandle_System_RuntimeFieldHandle__),
};

extern "C" int32_t kChaosExternalRuntimeCount = 21;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[22])() = {
	reinterpret_cast<void(*)()>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_6),
	reinterpret_cast<void(*)()>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_7),
	reinterpret_cast<void(*)()>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_8),
	reinterpret_cast<void(*)()>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_9),
	reinterpret_cast<void(*)()>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_10),
	reinterpret_cast<void(*)()>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_11),
	reinterpret_cast<void(*)()>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_12),
	reinterpret_cast<void(*)()>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_13),
	reinterpret_cast<void(*)()>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_14),
	reinterpret_cast<void(*)()>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_15),
	reinterpret_cast<void(*)()>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_16),
	reinterpret_cast<void(*)()>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_17),
	reinterpret_cast<void(*)()>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_18),
	reinterpret_cast<void(*)()>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_19),
	reinterpret_cast<void(*)()>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_20),
	reinterpret_cast<void(*)()>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_21),
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
extern "C" const int kSubjectEntryCount = 22;
extern "C" const int kSubjectEntryIndices[22] = {
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

	/* ── Entry 0: ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry0;
	/* ── Entry 1: ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry1;
	/* ── Entry 2: ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry2;
	/* ── Entry 3: ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry3;
	/* ── Entry 4: ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry4;
	/* ── Entry 5: ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry5;
	/* ── Entry 6: ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_6 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry6;
	/* ── Entry 7: ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_7 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry7;
	/* ── Entry 8: ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_8 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry8;
	/* ── Entry 9: ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_9 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry9;
	/* ── Entry 10: ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_10 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry10;
	/* ── Entry 11: ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_11 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry11;
	/* ── Entry 12: ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_12 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry12;
	/* ── Entry 13: ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_13 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry13;
	/* ── Entry 14: ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_14 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry14;
	/* ── Entry 15: ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_15 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry15;
	/* ── Entry 16: ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_16 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry16;
	/* ── Entry 17: ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_17 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry17;
	/* ── Entry 18: ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_18 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry18;
	/* ── Entry 19: ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_19 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry19;
	/* ── Entry 20: ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_20 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry20;
	/* ── Entry 21: ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_21 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry21;
} kChaosGcSlotMapsSection = {
	/* entry0 = ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_0 */
	.entry0 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_0),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry1 = ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_1 */
	.entry1 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_1),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry2 = ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_2 */
	.entry2 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_2),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry3 = ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_3 */
	.entry3 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_3),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry4 = ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_4 */
	.entry4 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_4),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry5 = ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_5 */
	.entry5 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_5),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry6 = ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_6 */
	.entry6 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_6),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry7 = ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_7 */
	.entry7 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_7),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry8 = ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_8 */
	.entry8 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_8),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry9 = ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_9 */
	.entry9 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_9),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry10 = ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_10 */
	.entry10 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_10),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry11 = ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_11 */
	.entry11 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_11),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry12 = ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_12 */
	.entry12 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_12),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry13 = ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_13 */
	.entry13 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_13),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry14 = ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_14 */
	.entry14 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_14),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry15 = ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_15 */
	.entry15 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_15),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry16 = ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_16 */
	.entry16 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_16),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry17 = ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_17 */
	.entry17 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_17),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry18 = ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_18 */
	.entry18 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_18),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry19 = ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_19 */
	.entry19 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_19),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry20 = ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_20 */
	.entry20 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_20),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry21 = ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_21 */
	.entry21 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_21),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 560u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[22] = {
	reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_0),
	reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_1),
	reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_2),
	reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_3),
	reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_4),
	reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_5),
	reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_6),
	reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_7),
	reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_8),
	reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_9),
	reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_10),
	reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_11),
	reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_12),
	reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_13),
	reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_14),
	reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_15),
	reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_16),
	reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_17),
	reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_18),
	reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_19),
	reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_20),
	reinterpret_cast<void*>(&ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_21),
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
	.image_name_utf8    = "ReflectionMemberCompleteSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects[1] = {
	{ 0u, "ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects[22] = {
	{ 0u, "ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_6:System.Void()", "Subject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_7:System.Void()", "Subject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_8:System.Void()", "Subject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_9:System.Void()", "Subject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_10:System.Void()", "Subject_10", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_11:System.Void()", "Subject_11", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_12:System.Void()", "Subject_12", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_13:System.Void()", "Subject_13", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_14:System.Void()", "Subject_14", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_15:System.Void()", "Subject_15", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_16:System.Void()", "Subject_16", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_17:System.Void()", "Subject_17", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_18:System.Void()", "Subject_18", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_19:System.Void()", "Subject_19", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_20:System.Void()", "Subject_20", "System.Void", 0, nullptr, 0u },
	{ 0u, "ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_21:System.Void()", "Subject_21", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0u, "ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects", "ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects", "", "ReflectionMemberCompleteSubjects", "ReflectionMemberCompleteSubjects", nullptr, kReflFields_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects, 1u, nullptr, 0u,
	kReflMethods_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects, 22u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "ReflectionMemberCompleteSubjects", kReflTypePtrs, 1u };

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
// Managed method: ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_0()
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionGetMemberName(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects___exitCode = chaos_value;
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

// Managed method: ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_1()
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[2])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects___exitCode = chaos_value;
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

// Managed method: ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_2()
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionGetDeclaringType(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects___exitCode = chaos_value;
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

// Managed method: ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_3()
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionGetReflectedType(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects___exitCode = chaos_value;
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

// Managed method: ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_4()
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionGetMetadataToken(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects___exitCode = chaos_value;
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

// Managed method: ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_5()
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[6])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects___exitCode = chaos_value;
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

// Managed method: ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_6()
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_6(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionGetBaseDefinition(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects___exitCode = chaos_value;
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

// Managed method: ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_7()
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_7(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionGetMethodHandle(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects___exitCode = chaos_value;
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

// Managed method: ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_8()
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionGetCallingConvention(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects___exitCode = chaos_value;
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

// Managed method: ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_9()
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_9(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionGetIsPublic(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects___exitCode = chaos_value;
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

// Managed method: ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_10()
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_10(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_MethodBase__get_IsStatic_System_Boolean__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects___exitCode = chaos_value;
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

// Managed method: ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_11()
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_11(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionGetIsVirtual(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects___exitCode = chaos_value;
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

// Managed method: ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_12()
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_12(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[1] = _s0;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosInt32GetHashCode(chaos_arg_0);
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
				chaos_static_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_13()
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_13(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Reflection_FieldInfo__get_IsStatic_System_Boolean__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects___exitCode = chaos_value;
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

// Managed method: ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_14()
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_14(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionFieldGetIsInitOnly(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects___exitCode = chaos_value;
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

// Managed method: ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_15()
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_15(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionFieldGetIsLiteral(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects___exitCode = chaos_value;
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

// Managed method: ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_16()
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_16(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[1] = _s0;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosInt32GetHashCode(chaos_arg_0);
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
				chaos_static_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_17()
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_17(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionPropertyGetCanRead(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects___exitCode = chaos_value;
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

// Managed method: ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_18()
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_18(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionPropertyGetCanWrite(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects___exitCode = chaos_value;
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

// Managed method: ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_19()
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_19(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionPropertyGetIndexParameters(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects___exitCode = chaos_value;
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

// Managed method: ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_20()
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_20(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionFieldGetFieldHandle(chaos_arg_0);
				_s0 = ChaosStoreInt64(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects___exitCode = chaos_value;
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

// Managed method: ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_21()
extern "C" void ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_21(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionGetCallingConvention(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects___exitCode = chaos_value;
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



}  // namespace chaos::il2cpp::codegen::ReflectionMemberCompleteSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 22;

// ── JIT Method Entry Table ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kMethodJson_0[] = "{\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-0\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_0:System.Void()\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_0()\",\"identity\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"definitionSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_0:System.Void()\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_0:System.Void()\",\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-0\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_0()\"},\"nativeSymbol\":\"ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":17,\"handlerOffset\":18,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"ilOffset\":10,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionMemberCompleteSubjects\"}";
static const char kMethodJson_1[] = "{\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-1\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_1:System.Void()\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_1()\",\"identity\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"definitionSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_1:System.Void()\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_1:System.Void()\",\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-1\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_1()\"},\"nativeSymbol\":\"ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":17,\"handlerOffset\":18,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Reflection.MemberTypes\",\"callee\":\"System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"ilOffset\":10,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionMemberCompleteSubjects\"}";
static const char kMethodJson_2[] = "{\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-2\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_2:System.Void()\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_2()\",\"identity\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"definitionSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_2:System.Void()\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_2:System.Void()\",\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-2\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_2()\"},\"nativeSymbol\":\"ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_2\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":17,\"handlerOffset\":18,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"ilOffset\":10,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionMemberCompleteSubjects\"}";
static const char kMethodJson_3[] = "{\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-3\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_3:System.Void()\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_3()\",\"identity\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"definitionSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_3:System.Void()\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_3:System.Void()\",\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-3\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_3()\"},\"nativeSymbol\":\"ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_3\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":17,\"handlerOffset\":18,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Reflection.MemberInfo::get_ReflectedType:System.Type()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MemberInfo::get_ReflectedType:System.Type()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MemberInfo::get_ReflectedType:System.Type()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"ilOffset\":10,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionMemberCompleteSubjects\"}";
static const char kMethodJson_4[] = "{\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-4\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_4:System.Void()\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_4()\",\"identity\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"definitionSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_4:System.Void()\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_4:System.Void()\",\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-4\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_4()\"},\"nativeSymbol\":\"ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_4\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":17,\"handlerOffset\":18,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Reflection.MemberInfo::get_MetadataToken:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MemberInfo::get_MetadataToken:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MemberInfo::get_MetadataToken:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"ilOffset\":10,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionMemberCompleteSubjects\"}";
static const char kMethodJson_5[] = "{\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-5\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_5:System.Void()\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_5()\",\"identity\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"definitionSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_5:System.Void()\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_5:System.Void()\",\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-5\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_5()\"},\"nativeSymbol\":\"ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_5\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":17,\"handlerOffset\":18,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Type\",\"callee\":\"System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"ilOffset\":10,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionMemberCompleteSubjects\"}";
static const char kMethodJson_6[] = "{\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-6\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_6:System.Void()\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_6()\",\"identity\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"definitionSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_6:System.Void()\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_6:System.Void()\",\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-6\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_6()\"},\"nativeSymbol\":\"ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_6\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":17,\"handlerOffset\":18,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Reflection.MethodInfo\",\"callee\":\"System.Private.CoreLib/System.Reflection.MethodInfo::GetBaseDefinition:System.Reflection.MethodInfo()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MethodInfo::GetBaseDefinition:System.Reflection.MethodInfo()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MethodInfo::GetBaseDefinition:System.Reflection.MethodInfo()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"ilOffset\":10,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionMemberCompleteSubjects\"}";
static const char kMethodJson_7[] = "{\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-7\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_7:System.Void()\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_7()\",\"identity\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"definitionSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_7:System.Void()\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_7:System.Void()\",\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-7\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_7()\"},\"nativeSymbol\":\"ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_7\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":17,\"handlerOffset\":18,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.RuntimeMethodHandle\",\"callee\":\"System.Private.CoreLib/System.Reflection.MethodBase::get_MethodHandle:System.RuntimeMethodHandle()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MethodBase::get_MethodHandle:System.RuntimeMethodHandle()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MethodBase::get_MethodHandle:System.RuntimeMethodHandle()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"ilOffset\":10,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionMemberCompleteSubjects\"}";
static const char kMethodJson_8[] = "{\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-8\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_8:System.Void()\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_8()\",\"identity\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"definitionSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_8:System.Void()\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_8:System.Void()\",\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-8\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_8()\"},\"nativeSymbol\":\"ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_8\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":17,\"handlerOffset\":18,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Reflection.CallingConventions\",\"callee\":\"System.Private.CoreLib/System.Reflection.MethodBase::get_CallingConvention:System.Reflection.CallingConventions()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MethodBase::get_CallingConvention:System.Reflection.CallingConventions()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MethodBase::get_CallingConvention:System.Reflection.CallingConventions()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"ilOffset\":10,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionMemberCompleteSubjects\"}";
static const char kMethodJson_9[] = "{\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-9\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_9:System.Void()\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_9()\",\"identity\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"definitionSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_9:System.Void()\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_9:System.Void()\",\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-9\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_9()\"},\"nativeSymbol\":\"ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_9\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":17,\"handlerOffset\":18,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Reflection.MethodBase::get_IsPublic:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MethodBase::get_IsPublic:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MethodBase::get_IsPublic:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"ilOffset\":10,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionMemberCompleteSubjects\"}";
static const char kMethodJson_10[] = "{\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-10\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_10:System.Void()\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_10()\",\"identity\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"definitionSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_10:System.Void()\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_10:System.Void()\",\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-10\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_10()\"},\"nativeSymbol\":\"ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_10\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":17,\"handlerOffset\":18,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Reflection.MethodBase::get_IsStatic:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MethodBase::get_IsStatic:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MethodBase::get_IsStatic:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"ilOffset\":10,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionMemberCompleteSubjects\"}";
static const char kMethodJson_11[] = "{\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-11\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_11:System.Void()\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_11()\",\"identity\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"definitionSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_11:System.Void()\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_11:System.Void()\",\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-11\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_11()\"},\"nativeSymbol\":\"ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_11\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":17,\"handlerOffset\":18,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Reflection.MethodBase::get_IsVirtual:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MethodBase::get_IsVirtual:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MethodBase::get_IsVirtual:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"ilOffset\":10,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionMemberCompleteSubjects\"}";
static const char kMethodJson_12[] = "{\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-12\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_12:System.Void()\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_12()\",\"identity\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"definitionSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_12:System.Void()\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_12:System.Void()\",\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-12\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_12()\"},\"nativeSymbol\":\"ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_12\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":3,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":5,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Int32::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Int32::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":23,\"ilOffset\":15,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"ilOffset\":18,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionMemberCompleteSubjects\"}";
static const char kMethodJson_13[] = "{\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-13\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_13:System.Void()\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_13()\",\"identity\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"definitionSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_13:System.Void()\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_13:System.Void()\",\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-13\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_13()\"},\"nativeSymbol\":\"ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_13\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":17,\"handlerOffset\":18,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Reflection.FieldInfo::get_IsStatic:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.FieldInfo::get_IsStatic:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.FieldInfo::get_IsStatic:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"ilOffset\":10,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionMemberCompleteSubjects\"}";
static const char kMethodJson_14[] = "{\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-14\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_14:System.Void()\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_14()\",\"identity\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"definitionSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_14:System.Void()\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_14:System.Void()\",\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-14\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_14()\"},\"nativeSymbol\":\"ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_14\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":17,\"handlerOffset\":18,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Reflection.FieldInfo::get_IsInitOnly:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.FieldInfo::get_IsInitOnly:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.FieldInfo::get_IsInitOnly:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"ilOffset\":10,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionMemberCompleteSubjects\"}";
static const char kMethodJson_15[] = "{\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-15\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_15:System.Void()\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_15()\",\"identity\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"definitionSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_15:System.Void()\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_15:System.Void()\",\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-15\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_15()\"},\"nativeSymbol\":\"ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_15\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":17,\"handlerOffset\":18,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Reflection.FieldInfo::get_IsLiteral:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.FieldInfo::get_IsLiteral:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.FieldInfo::get_IsLiteral:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"ilOffset\":10,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionMemberCompleteSubjects\"}";
static const char kMethodJson_16[] = "{\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-16\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_16:System.Void()\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_16()\",\"identity\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"definitionSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_16:System.Void()\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_16:System.Void()\",\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-16\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_16()\"},\"nativeSymbol\":\"ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_16\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":3,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":5,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Int32::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Int32::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":23,\"ilOffset\":15,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"ilOffset\":18,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionMemberCompleteSubjects\"}";
static const char kMethodJson_17[] = "{\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-17\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_17:System.Void()\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_17()\",\"identity\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"definitionSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_17:System.Void()\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_17:System.Void()\",\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-17\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_17()\"},\"nativeSymbol\":\"ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_17\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":17,\"handlerOffset\":18,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanRead:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanRead:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanRead:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"ilOffset\":10,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionMemberCompleteSubjects\"}";
static const char kMethodJson_18[] = "{\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-18\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_18:System.Void()\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_18()\",\"identity\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"definitionSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_18:System.Void()\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_18:System.Void()\",\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-18\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_18()\"},\"nativeSymbol\":\"ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_18\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":17,\"handlerOffset\":18,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanWrite:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanWrite:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanWrite:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"ilOffset\":10,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionMemberCompleteSubjects\"}";
static const char kMethodJson_19[] = "{\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-19\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_19:System.Void()\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_19()\",\"identity\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"definitionSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_19:System.Void()\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_19:System.Void()\",\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-19\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_19()\"},\"nativeSymbol\":\"ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_19\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":17,\"handlerOffset\":18,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Reflection.ParameterInfo[]\",\"callee\":\"System.Private.CoreLib/System.Reflection.PropertyInfo::GetIndexParameters:System.Reflection.ParameterInfo[]()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.PropertyInfo::GetIndexParameters:System.Reflection.ParameterInfo[]()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.PropertyInfo::GetIndexParameters:System.Reflection.ParameterInfo[]()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"ilOffset\":10,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionMemberCompleteSubjects\"}";
static const char kMethodJson_20[] = "{\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-20\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_20:System.Void()\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_20()\",\"identity\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"definitionSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_20:System.Void()\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_20:System.Void()\",\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-20\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_20()\"},\"nativeSymbol\":\"ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_20\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":17,\"handlerOffset\":18,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.RuntimeFieldHandle\",\"callee\":\"System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldHandle:System.RuntimeFieldHandle()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldHandle:System.RuntimeFieldHandle()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldHandle:System.RuntimeFieldHandle()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"ilOffset\":10,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionMemberCompleteSubjects\"}";
static const char kMethodJson_21[] = "{\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-21\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_21:System.Void()\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_21()\",\"identity\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"definitionSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_21:System.Void()\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::Subject_21:System.Void()\",\"methodId\":\"reflection-member-complete-subjects.reflection-member-complete-subjects.subject-21\",\"signature\":\"System.Void ReflectionMemberCompleteSubjects::Subject_21()\"},\"nativeSymbol\":\"ReflectionMemberCompleteSubjects_ReflectionMemberCompleteSubjects_Subject_21\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":17,\"handlerOffset\":18,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Reflection.CallingConventions\",\"callee\":\"System.Private.CoreLib/System.Reflection.MethodBase::get_CallingConvention:System.Reflection.CallingConventions()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MethodBase::get_CallingConvention:System.Reflection.CallingConventions()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Reflection.MethodBase::get_CallingConvention:System.Reflection.CallingConventions()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"ilOffset\":10,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ReflectionMemberCompleteSubjects\",\"subjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ReflectionMemberCompleteSubjects/ReflectionMemberCompleteSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":23,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ReflectionMemberCompleteSubjects\"}";

extern "C" const JitMethodEntry kChaosJitMethodEntries[22] =
{
    { kMethodJson_0, 3593u, 0x00000003u, 0u },
    { kMethodJson_1, 3675u, 0x00000004u, 0u },
    { kMethodJson_2, 3612u, 0x00000005u, 0u },
    { kMethodJson_3, 3612u, 0x00000006u, 0u },
    { kMethodJson_4, 3616u, 0x00000007u, 0u },
    { kMethodJson_5, 3603u, 0x00000008u, 0u },
    { kMethodJson_6, 3680u, 0x00000009u, 0u },
    { kMethodJson_7, 3669u, 0x0000000Au, 0u },
    { kMethodJson_8, 3724u, 0x0000000Bu, 0u },
    { kMethodJson_9, 3609u, 0x0000000Cu, 0u },
    { kMethodJson_10, 3617u, 0x0000000Du, 0u },
    { kMethodJson_11, 3620u, 0x0000000Eu, 0u },
    { kMethodJson_12, 3754u, 0x0000000Fu, 0u },
    { kMethodJson_13, 3614u, 0x00000010u, 0u },
    { kMethodJson_14, 3620u, 0x00000011u, 0u },
    { kMethodJson_15, 3617u, 0x00000012u, 0u },
    { kMethodJson_16, 3754u, 0x00000013u, 0u },
    { kMethodJson_17, 3620u, 0x00000014u, 0u },
    { kMethodJson_18, 3623u, 0x00000015u, 0u },
    { kMethodJson_19, 3717u, 0x00000016u, 0u },
    { kMethodJson_20, 3667u, 0x00000017u, 0u },
    { kMethodJson_21, 3732u, 0x00000018u, 0u }
};

extern "C" const uint32_t kChaosJitMethodEntryCount = 22u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitMethods(kChaosJitMethodEntries, kChaosJitMethodEntryCount);
}