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

namespace chaos::il2cpp::codegen::CollectionsGenericCoreSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects = {{nullptr, nullptr, 1254734368184984154ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(1254734368184984154ULL);inline TypeInfoV0 chaos_mt_System_Collections_System_Collections_Generic_Dictionary_System_String_System_Int32_ = {{nullptr, nullptr, 4038001413595486414ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Collections_System_Collections_Generic_Dictionary_System_String_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(4038001413595486414ULL);inline TypeInfoV0 chaos_mt_System_Collections_System_Collections_Generic_List_System_Int32_ = {{nullptr, nullptr, 8124737553590710378ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Collections_System_Collections_Generic_List_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(8124737553590710378ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {{nullptr, nullptr, 5474029880995115448ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
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

struct chaos_type_CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Collections_System_Collections_Generic_Dictionary_System_String_System_Int32_
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Collections_System_Collections_Generic_List_System_Int32_
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

CHAOS_IL2CPP_INT32 chaos_static_CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects___exitCode = 0;

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_Dictionary_System_String_System_Int32___get_Count_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	return CollectionDictionaryGetCount(chaos_arg_0);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_List_System_Int32___Add_System_Void_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
	CollectionListAdd(chaos_arg_0, chaos_arg_1);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_List_System_Int32___Clear_System_Void__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	CollectionListClear(chaos_arg_0);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_List_System_Int32___RemoveAt_System_Void_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1)
{
	CollectionListRemoveAt(chaos_arg_0, chaos_arg_1);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_List_System_Int32___Sort_System_Void__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	CollectionListSort(chaos_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_List_System_Int32___ToArray_System_Int32____(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	return CollectionListToArray(chaos_arg_0);
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_0(void);
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_1(void);
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_2(void);
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_3(void);
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_4(void);
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_5(void);
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_6(void);
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_7(void);
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_8(void);
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_9(void);
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_10(void);
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_11(void);
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_12(void);
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_13(void);
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_14(void);
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_15(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[17] = {
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
	::ChaosAbiMethodEntryV0 entries[16];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		16u,
		0u,
		187360325u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_0
		{ 0u, 0u },  // CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_1
		{ 0u, 0u },  // CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_2
		{ 0u, 0u },  // CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_3
		{ 0u, 0u },  // CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_4
		{ 0u, 0u },  // CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_5
		{ 0u, 0u },  // CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_6
		{ 0u, 0u },  // CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_7
		{ 0u, 0u },  // CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_8
		{ 0u, 0u },  // CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_9
		{ 0u, 0u },  // CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_10
		{ 0u, 0u },  // CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_11
		{ 0u, 0u },  // CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_12
		{ 0u, 0u },  // CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_13
		{ 0u, 0u },  // CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_14
		{ 0u, 0u },  // CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_15
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
		"CollectionsGenericCoreSubjects",
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
		&chaos_mt_CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects.hot,
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
		/* .name_utf8         = */ "CollectionsGenericCoreSubjects",
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
		::chaos::il2cpp::runtime_core::RegisterModule("CollectionsGenericCoreSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[16] = {
	{ "Subject_0", 0x00000003u, 0u },  // CollectionsGenericCoreSubjects
	{ "Subject_1", 0x00000004u, 0u },  // CollectionsGenericCoreSubjects
	{ "Subject_2", 0x00000005u, 0u },  // CollectionsGenericCoreSubjects
	{ "Subject_3", 0x00000006u, 0u },  // CollectionsGenericCoreSubjects
	{ "Subject_4", 0x00000007u, 0u },  // CollectionsGenericCoreSubjects
	{ "Subject_5", 0x00000008u, 0u },  // CollectionsGenericCoreSubjects
	{ "Subject_6", 0x00000009u, 0u },  // CollectionsGenericCoreSubjects
	{ "Subject_7", 0x0000000Au, 0u },  // CollectionsGenericCoreSubjects
	{ "Subject_8", 0x0000000Bu, 0u },  // CollectionsGenericCoreSubjects
	{ "Subject_9", 0x0000000Cu, 0u },  // CollectionsGenericCoreSubjects
	{ "Subject_10", 0x0000000Du, 0u },  // CollectionsGenericCoreSubjects
	{ "Subject_11", 0x0000000Eu, 0u },  // CollectionsGenericCoreSubjects
	{ "Subject_12", 0x0000000Fu, 0u },  // CollectionsGenericCoreSubjects
	{ "Subject_13", 0x00000010u, 0u },  // CollectionsGenericCoreSubjects
	{ "Subject_14", 0x00000011u, 0u },  // CollectionsGenericCoreSubjects
	{ "Subject_15", 0x00000012u, 0u },  // CollectionsGenericCoreSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "CollectionsGenericCoreSubjects", "", 0u, 16u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[16] = {
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
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[16] = {
	{ reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CollectionsGenericCoreSubjects::Subject_0
	{ reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CollectionsGenericCoreSubjects::Subject_1
	{ reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CollectionsGenericCoreSubjects::Subject_2
	{ reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CollectionsGenericCoreSubjects::Subject_3
	{ reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CollectionsGenericCoreSubjects::Subject_4
	{ reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CollectionsGenericCoreSubjects::Subject_5
	{ reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CollectionsGenericCoreSubjects::Subject_6
	{ reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CollectionsGenericCoreSubjects::Subject_7
	{ reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CollectionsGenericCoreSubjects::Subject_8
	{ reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CollectionsGenericCoreSubjects::Subject_9
	{ reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CollectionsGenericCoreSubjects::Subject_10
	{ reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CollectionsGenericCoreSubjects::Subject_11
	{ reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CollectionsGenericCoreSubjects::Subject_12
	{ reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CollectionsGenericCoreSubjects::Subject_13
	{ reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_14), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CollectionsGenericCoreSubjects::Subject_14
	{ reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_15), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // CollectionsGenericCoreSubjects::Subject_15
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"CollectionsGenericCoreSubjects",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	16u,
	s_hotpatch_slots,
	16u,
	s_hotpatch_entries,
	16u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[9] = {
	"System.Collections/System.Collections.Generic.List<System.Int32>::.ctor:System.Void()",
	"System.Collections/System.Collections.Generic.List<System.Int32>::Clear:System.Void()",
	"System.Collections/System.Collections.Generic.List<System.Int32>::Add:System.Void(System.Int32)",
	"System.Collections/System.Collections.Generic.List<System.Int32>::RemoveAt:System.Void(System.Int32)",
	"System.Collections/System.Collections.Generic.List<System.Int32>::Sort:System.Void()",
	"System.Collections/System.Collections.Generic.List<System.Int32>::ToArray:System.Int32[]()",
	"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::_exitCode",
	"System.Collections/System.Collections.Generic.Dictionary<System.String,System.Int32>::.ctor:System.Void()",
	"System.Collections/System.Collections.Generic.Dictionary<System.String,System.Int32>::get_Count:System.Int32()",
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
static void (*kAotMethods[16])() = {
	reinterpret_cast<void(*)()>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_6),
	reinterpret_cast<void(*)()>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_7),
	reinterpret_cast<void(*)()>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_8),
	reinterpret_cast<void(*)()>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_9),
	reinterpret_cast<void(*)()>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_10),
	reinterpret_cast<void(*)()>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_11),
	reinterpret_cast<void(*)()>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_12),
	reinterpret_cast<void(*)()>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_13),
	reinterpret_cast<void(*)()>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_14),
	reinterpret_cast<void(*)()>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_15),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
static void (*kBenchmarkWrappers[16])() = {
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
static void* const kMethodPointers[16] = {
	reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_0),
	reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_1),
	reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_2),
	reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_3),
	reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_4),
	reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_5),
	reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_6),
	reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_7),
	reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_8),
	reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_9),
	reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_10),
	reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_11),
	reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_12),
	reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_13),
	reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_14),
	reinterpret_cast<void*>(&CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_15),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 16u,
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
	.image_name_utf8    = "CollectionsGenericCoreSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects[1] = {
	{ 0u, "CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects[16] = {
	{ 0u, "CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_6:System.Void()", "Subject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_7:System.Void()", "Subject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_8:System.Void()", "Subject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_9:System.Void()", "Subject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, "CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_10:System.Void()", "Subject_10", "System.Void", 0, nullptr, 0u },
	{ 0u, "CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_11:System.Void()", "Subject_11", "System.Void", 0, nullptr, 0u },
	{ 0u, "CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_12:System.Void()", "Subject_12", "System.Void", 0, nullptr, 0u },
	{ 0u, "CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_13:System.Void()", "Subject_13", "System.Void", 0, nullptr, 0u },
	{ 0u, "CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_14:System.Void()", "Subject_14", "System.Void", 0, nullptr, 0u },
	{ 0u, "CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_15:System.Void()", "Subject_15", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0u, "CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects", "CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects", "", "CollectionsGenericCoreSubjects", "CollectionsGenericCoreSubjects", nullptr, kReflFields_CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects, 1u, nullptr, 0u,
	kReflMethods_CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects, 16u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "CollectionsGenericCoreSubjects", kReflTypePtrs, 1u };

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
// Managed method: CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_0()
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_1()
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Collections_System_Collections_Generic_List_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_System_Collections_System_Collections_Generic_List_System_Int32_.hot;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_List_System_Int32___Clear_System_Void__(chaos_arg_0);
	}
	return;
}

// Managed method: CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_2()
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_3()
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_4()
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_5()
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_5(void)
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


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Collections_System_Collections_Generic_List_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_System_Collections_System_Collections_Generic_List_System_Int32_.hot;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_List_System_Int32___Add_System_Void_System_Int32_(chaos_arg_0, chaos_arg_1);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_List_System_Int32___Add_System_Void_System_Int32_(chaos_arg_0, chaos_arg_1);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_List_System_Int32___Add_System_Void_System_Int32_(chaos_arg_0, chaos_arg_1);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_List_System_Int32___RemoveAt_System_Void_System_Int32_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
	}
	return;
}

// Managed method: CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_6()
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_6(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Collections_System_Collections_Generic_List_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_System_Collections_System_Collections_Generic_List_System_Int32_.hot;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_List_System_Int32___Sort_System_Void__(chaos_arg_0);
	}
	return;
}

// Managed method: CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_7()
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_7(void)
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


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Collections_System_Collections_Generic_List_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_System_Collections_System_Collections_Generic_List_System_Int32_.hot;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_List_System_Int32___ToArray_System_Int32____(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_8()
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_9()
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_9(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Collections_System_Collections_Generic_Dictionary_System_String_System_Int32_, {});
		chaos_object->header.type_info = &chaos_mt_System_Collections_System_Collections_Generic_Dictionary_System_String_System_Int32_.hot;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Collections_Generic_Dictionary_System_String_System_Int32___get_Count_System_Int32__(chaos_arg_0);
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
				chaos_static_CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_10()
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_10(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_11()
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_11(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_12()
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_12(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_13()
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_13(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_14()
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_14(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_15()
extern "C" void CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_15(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}



}  // namespace chaos::il2cpp::codegen::CollectionsGenericCoreSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 16;

// ── JIT Method Entry Table ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kMethodJson_0[] = "{\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-0\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_0:System.Void()\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_0()\",\"identity\":{\"assemblyName\":\"CollectionsGenericCoreSubjects\",\"declaringTypeSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects\",\"definitionSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_0:System.Void()\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_0:System.Void()\",\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-0\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_0()\"},\"nativeSymbol\":\"CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CollectionsGenericCoreSubjects\"}";
static const char kMethodJson_1[] = "{\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-1\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_1:System.Void()\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_1()\",\"identity\":{\"assemblyName\":\"CollectionsGenericCoreSubjects\",\"declaringTypeSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects\",\"definitionSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_1:System.Void()\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_1:System.Void()\",\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-1\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_1()\"},\"nativeSymbol\":\"CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":1,\"resultType\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Collections\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Collections\",\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Void\",\"callee\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::Clear:System.Void()\",\"reference\":{\"assemblyName\":\"System.Collections\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::Clear:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections\",\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::Clear:System.Void()\",\"openDefinitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::Clear:System.Void()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.List\\u00601::Clear:System.Void();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.List\\u00601::Clear:System.Void();type=[System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::Clear:System.Void()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.List\\u00601::Clear:System.Void();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.List\\u00601::Clear:System.Void();type=[System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::Clear:System.Void()\",\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::Clear:System.Void()\",\"displaySubjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::Clear()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::Clear:System.Void()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":12,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CollectionsGenericCoreSubjects\"}";
static const char kMethodJson_2[] = "{\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-2\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_2:System.Void()\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_2()\",\"identity\":{\"assemblyName\":\"CollectionsGenericCoreSubjects\",\"declaringTypeSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects\",\"definitionSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_2:System.Void()\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_2:System.Void()\",\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-2\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_2()\"},\"nativeSymbol\":\"CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_2\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CollectionsGenericCoreSubjects\"}";
static const char kMethodJson_3[] = "{\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-3\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_3:System.Void()\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_3()\",\"identity\":{\"assemblyName\":\"CollectionsGenericCoreSubjects\",\"declaringTypeSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects\",\"definitionSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_3:System.Void()\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_3:System.Void()\",\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-3\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_3()\"},\"nativeSymbol\":\"CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_3\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CollectionsGenericCoreSubjects\"}";
static const char kMethodJson_4[] = "{\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-4\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_4:System.Void()\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_4()\",\"identity\":{\"assemblyName\":\"CollectionsGenericCoreSubjects\",\"declaringTypeSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects\",\"definitionSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_4:System.Void()\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_4:System.Void()\",\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-4\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_4()\"},\"nativeSymbol\":\"CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_4\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CollectionsGenericCoreSubjects\"}";
static const char kMethodJson_5[] = "{\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-5\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_5:System.Void()\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_5()\",\"identity\":{\"assemblyName\":\"CollectionsGenericCoreSubjects\",\"declaringTypeSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects\",\"definitionSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_5:System.Void()\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_5:System.Void()\",\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-5\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_5()\"},\"nativeSymbol\":\"CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_5\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":1,\"resultType\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Collections\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Collections\",\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":6,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Void\",\"callee\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::Add:System.Void(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Collections\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::Add:System.Void(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections\",\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::Add:System.Void(System.Int32)\",\"openDefinitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::Add:System.Void(!0)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.List\\u00601::Add:System.Void(!0);type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.List\\u00601::Add:System.Void(!0);type=[System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::Add:System.Void(!0)\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.List\\u00601::Add:System.Void(!0);type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.List\\u00601::Add:System.Void(!0);type=[System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::Add:System.Void(System.Int32)\",\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::Add:System.Void(!0)\",\"displaySubjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::Add(System.Int32)\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::Add:System.Void(!0)\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":2,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":16,\"resultType\":\"System.Void\",\"callee\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::Add:System.Void(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Collections\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::Add:System.Void(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections\",\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::Add:System.Void(System.Int32)\",\"openDefinitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::Add:System.Void(!0)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.List\\u00601::Add:System.Void(!0);type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.List\\u00601::Add:System.Void(!0);type=[System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::Add:System.Void(!0)\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.List\\u00601::Add:System.Void(!0);type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.List\\u00601::Add:System.Void(!0);type=[System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::Add:System.Void(System.Int32)\",\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::Add:System.Void(!0)\",\"displaySubjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::Add(System.Int32)\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::Add:System.Void(!0)\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":22,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":3,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":24,\"resultType\":\"System.Void\",\"callee\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::Add:System.Void(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Collections\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::Add:System.Void(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections\",\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::Add:System.Void(System.Int32)\",\"openDefinitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::Add:System.Void(!0)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.List\\u00601::Add:System.Void(!0);type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.List\\u00601::Add:System.Void(!0);type=[System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::Add:System.Void(!0)\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.List\\u00601::Add:System.Void(!0);type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.List\\u00601::Add:System.Void(!0);type=[System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::Add:System.Void(System.Int32)\",\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::Add:System.Void(!0)\",\"displaySubjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::Add(System.Int32)\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::Add:System.Void(!0)\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":30,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":31,\"resultType\":\"System.Void\",\"callee\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::RemoveAt:System.Void(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Collections\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::RemoveAt:System.Void(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections\",\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::RemoveAt:System.Void(System.Int32)\",\"openDefinitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::RemoveAt:System.Void(System.Int32)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.List\\u00601::RemoveAt:System.Void(System.Int32);type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.List\\u00601::RemoveAt:System.Void(System.Int32);type=[System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::RemoveAt:System.Void(System.Int32)\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.List\\u00601::RemoveAt:System.Void(System.Int32);type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.List\\u00601::RemoveAt:System.Void(System.Int32);type=[System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::RemoveAt:System.Void(System.Int32)\",\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::RemoveAt:System.Void(System.Int32)\",\"displaySubjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::RemoveAt(System.Int32)\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::RemoveAt:System.Void(System.Int32)\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":37,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CollectionsGenericCoreSubjects\"}";
static const char kMethodJson_6[] = "{\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-6\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_6:System.Void()\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_6()\",\"identity\":{\"assemblyName\":\"CollectionsGenericCoreSubjects\",\"declaringTypeSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects\",\"definitionSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_6:System.Void()\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_6:System.Void()\",\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-6\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_6()\"},\"nativeSymbol\":\"CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_6\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":1,\"resultType\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Collections\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Collections\",\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Void\",\"callee\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::Sort:System.Void()\",\"reference\":{\"assemblyName\":\"System.Collections\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::Sort:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections\",\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::Sort:System.Void()\",\"openDefinitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::Sort:System.Void()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.List\\u00601::Sort:System.Void();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.List\\u00601::Sort:System.Void();type=[System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::Sort:System.Void()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.List\\u00601::Sort:System.Void();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.List\\u00601::Sort:System.Void();type=[System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::Sort:System.Void()\",\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::Sort:System.Void()\",\"displaySubjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::Sort()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::Sort:System.Void()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":12,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CollectionsGenericCoreSubjects\"}";
static const char kMethodJson_7[] = "{\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-7\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_7:System.Void()\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_7()\",\"identity\":{\"assemblyName\":\"CollectionsGenericCoreSubjects\",\"declaringTypeSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects\",\"definitionSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_7:System.Void()\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_7:System.Void()\",\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-7\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_7()\"},\"nativeSymbol\":\"CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_7\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":1,\"resultType\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Collections\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Collections\",\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Int32[]\",\"callee\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::ToArray:System.Int32[]()\",\"reference\":{\"assemblyName\":\"System.Collections\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::ToArray:System.Int32[]()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections\",\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::ToArray:System.Int32[]()\",\"openDefinitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::ToArray:!0[]()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.List\\u00601::ToArray:!0[]();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.List\\u00601::ToArray:!0[]();type=[System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::ToArray:!0[]()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.List\\u00601::ToArray:!0[]();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.List\\u00601::ToArray:!0[]();type=[System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::ToArray:System.Int32[]()\",\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::ToArray:!0[]()\",\"displaySubjectId\":\"System.Collections/System.Collections.Generic.List\\u003CSystem.Int32\\u003E::ToArray()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.List\\u00601::ToArray:!0[]()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldlen\",\"opCode\":46,\"ilOffset\":11,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":15,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":16,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":25,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::_exitCode\",\"ilOffset\":20,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"CollectionsGenericCoreSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"CollectionsGenericCoreSubjects\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":25,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CollectionsGenericCoreSubjects\"}";
static const char kMethodJson_8[] = "{\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-8\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_8:System.Void()\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_8()\",\"identity\":{\"assemblyName\":\"CollectionsGenericCoreSubjects\",\"declaringTypeSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects\",\"definitionSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_8:System.Void()\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_8:System.Void()\",\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-8\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_8()\"},\"nativeSymbol\":\"CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_8\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CollectionsGenericCoreSubjects\"}";
static const char kMethodJson_9[] = "{\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-9\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_9:System.Void()\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_9()\",\"identity\":{\"assemblyName\":\"CollectionsGenericCoreSubjects\",\"declaringTypeSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects\",\"definitionSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_9:System.Void()\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_9:System.Void()\",\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-9\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_9()\"},\"nativeSymbol\":\"CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_9\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":1,\"resultType\":\"System.Collections/System.Collections.Generic.Dictionary\\u003CSystem.String,System.Int32\\u003E\",\"callee\":\"System.Collections/System.Collections.Generic.Dictionary\\u003CSystem.String,System.Int32\\u003E::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Collections\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections/System.Collections.Generic.Dictionary\\u003CSystem.String,System.Int32\\u003E::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Collections\",\"subjectId\":\"System.Collections/System.Collections.Generic.Dictionary\\u003CSystem.String,System.Int32\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Int32\",\"callee\":\"System.Collections/System.Collections.Generic.Dictionary\\u003CSystem.String,System.Int32\\u003E::get_Count:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Collections\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections/System.Collections.Generic.Dictionary\\u003CSystem.String,System.Int32\\u003E::get_Count:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections\",\"subjectId\":\"System.Collections/System.Collections.Generic.Dictionary\\u003CSystem.String,System.Int32\\u003E::get_Count:System.Int32()\",\"openDefinitionSubjectId\":\"System.Collections/System.Collections.Generic.Dictionary\\u00602::get_Count:System.Int32()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.Dictionary\\u00602::get_Count:System.Int32();type=[System.String,System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.Dictionary\\u00602::get_Count:System.Int32();type=[System.String,System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.Dictionary\\u00602::get_Count:System.Int32()\",\"typeArguments\":[\"System.String\",\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.Dictionary\\u00602::get_Count:System.Int32();type=[System.String,System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.Dictionary\\u00602::get_Count:System.Int32();type=[System.String,System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections/System.Collections.Generic.Dictionary\\u003CSystem.String,System.Int32\\u003E::get_Count:System.Int32()\",\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.Dictionary\\u00602::get_Count:System.Int32()\",\"displaySubjectId\":\"System.Collections/System.Collections.Generic.Dictionary\\u003CSystem.String,System.Int32\\u003E::get_Count()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.Dictionary\\u00602::get_Count:System.Int32()\",\"typeArguments\":[\"System.String\",\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":15,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":24,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::_exitCode\",\"ilOffset\":19,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"CollectionsGenericCoreSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"CollectionsGenericCoreSubjects\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CollectionsGenericCoreSubjects\"}";
static const char kMethodJson_10[] = "{\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-10\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_10:System.Void()\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_10()\",\"identity\":{\"assemblyName\":\"CollectionsGenericCoreSubjects\",\"declaringTypeSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects\",\"definitionSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_10:System.Void()\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_10:System.Void()\",\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-10\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_10()\"},\"nativeSymbol\":\"CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_10\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CollectionsGenericCoreSubjects\"}";
static const char kMethodJson_11[] = "{\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-11\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_11:System.Void()\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_11()\",\"identity\":{\"assemblyName\":\"CollectionsGenericCoreSubjects\",\"declaringTypeSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects\",\"definitionSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_11:System.Void()\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_11:System.Void()\",\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-11\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_11()\"},\"nativeSymbol\":\"CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_11\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CollectionsGenericCoreSubjects\"}";
static const char kMethodJson_12[] = "{\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-12\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_12:System.Void()\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_12()\",\"identity\":{\"assemblyName\":\"CollectionsGenericCoreSubjects\",\"declaringTypeSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects\",\"definitionSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_12:System.Void()\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_12:System.Void()\",\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-12\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_12()\"},\"nativeSymbol\":\"CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_12\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CollectionsGenericCoreSubjects\"}";
static const char kMethodJson_13[] = "{\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-13\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_13:System.Void()\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_13()\",\"identity\":{\"assemblyName\":\"CollectionsGenericCoreSubjects\",\"declaringTypeSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects\",\"definitionSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_13:System.Void()\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_13:System.Void()\",\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-13\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_13()\"},\"nativeSymbol\":\"CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_13\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CollectionsGenericCoreSubjects\"}";
static const char kMethodJson_14[] = "{\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-14\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_14:System.Void()\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_14()\",\"identity\":{\"assemblyName\":\"CollectionsGenericCoreSubjects\",\"declaringTypeSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects\",\"definitionSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_14:System.Void()\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_14:System.Void()\",\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-14\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_14()\"},\"nativeSymbol\":\"CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_14\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CollectionsGenericCoreSubjects\"}";
static const char kMethodJson_15[] = "{\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-15\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_15:System.Void()\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_15()\",\"identity\":{\"assemblyName\":\"CollectionsGenericCoreSubjects\",\"declaringTypeSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects\",\"definitionSubjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_15:System.Void()\",\"subjectId\":\"CollectionsGenericCoreSubjects/CollectionsGenericCoreSubjects::Subject_15:System.Void()\",\"methodId\":\"collections-generic-core-subjects.collections-generic-core-subjects.subject-15\",\"signature\":\"System.Void CollectionsGenericCoreSubjects::Subject_15()\"},\"nativeSymbol\":\"CollectionsGenericCoreSubjects_CollectionsGenericCoreSubjects_Subject_15\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CollectionsGenericCoreSubjects\"}";

extern "C" const JitMethodEntry kChaosJitMethodEntries[16] =
{
    { kMethodJson_0, 1409u, 0x00000003u, 0u },
    { kMethodJson_1, 4449u, 0x00000004u, 0u },
    { kMethodJson_2, 1409u, 0x00000005u, 0u },
    { kMethodJson_3, 1409u, 0x00000006u, 0u },
    { kMethodJson_4, 1409u, 0x00000007u, 0u },
    { kMethodJson_5, 12288u, 0x00000008u, 0u },
    { kMethodJson_6, 4436u, 0x00000009u, 0u },
    { kMethodJson_7, 5888u, 0x0000000Au, 0u },
    { kMethodJson_8, 1409u, 0x0000000Bu, 0u },
    { kMethodJson_9, 6192u, 0x0000000Cu, 0u },
    { kMethodJson_10, 1417u, 0x0000000Du, 0u },
    { kMethodJson_11, 1417u, 0x0000000Eu, 0u },
    { kMethodJson_12, 1417u, 0x0000000Fu, 0u },
    { kMethodJson_13, 1417u, 0x00000010u, 0u },
    { kMethodJson_14, 1417u, 0x00000011u, 0u },
    { kMethodJson_15, 1417u, 0x00000012u, 0u }
};

extern "C" const uint32_t kChaosJitMethodEntryCount = 16u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitMethods(kChaosJitMethodEntries, kChaosJitMethodEntryCount);
}