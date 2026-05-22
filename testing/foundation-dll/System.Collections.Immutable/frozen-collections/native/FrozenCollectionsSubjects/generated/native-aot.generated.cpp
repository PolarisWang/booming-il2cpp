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

namespace chaos::il2cpp::codegen::FrozenCollectionsSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_FrozenCollectionsSubjects_FrozenCollectionsSubjects = {{nullptr, nullptr, 9155075195625744732ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_FrozenCollectionsSubjects_FrozenCollectionsSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(9155075195625744732ULL);
inline TypeInfoV0 chaos_mt_System_Collections_System_Collections_Generic_Dictionary_System_Byte_System_Byte_ = {{nullptr, nullptr, 5462541850016812929ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Collections_System_Collections_Generic_Dictionary_System_Byte_System_Byte_ = static_cast<CHAOS_IL2CPP_INTPTR>(5462541850016812929ULL);
inline TypeInfoV0 chaos_mt_System_Collections_System_Collections_Generic_Dictionary_System_String_System_Int32_ = {{nullptr, nullptr, 4038001413595486414ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Collections_System_Collections_Generic_Dictionary_System_String_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(4038001413595486414ULL);
inline TypeInfoV0 chaos_mt_System_Collections_System_Collections_Generic_Dictionary_System_String_System_String_ = {{nullptr, nullptr, 18165940823654175085ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Collections_System_Collections_Generic_Dictionary_System_String_System_String_ = static_cast<CHAOS_IL2CPP_INTPTR>(18165940823654175085ULL);
inline TypeInfoV0 chaos_mt_System_Collections_System_Collections_Generic_HashSet_System_Byte_ = {{nullptr, nullptr, 995041787150332022ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Collections_System_Collections_Generic_HashSet_System_Byte_ = static_cast<CHAOS_IL2CPP_INTPTR>(995041787150332022ULL);
inline TypeInfoV0 chaos_mt_System_Collections_System_Collections_Generic_HashSet_System_Int32_ = {{nullptr, nullptr, 11554060213542038524ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Collections_System_Collections_Generic_HashSet_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(11554060213542038524ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Byte = {{nullptr, nullptr, 3217969471912086765ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Byte = static_cast<CHAOS_IL2CPP_INTPTR>(3217969471912086765ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Int32 = {{nullptr, nullptr, 11009693519287992193ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Int32 = static_cast<CHAOS_IL2CPP_INTPTR>(11009693519287992193ULL);
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

struct chaos_type_FrozenCollectionsSubjects_FrozenCollectionsSubjects
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Collections_System_Collections_Generic_Dictionary_System_Byte_System_Byte_
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR chaos_native_storage = 0;  // native runtime storage ptr
};

struct chaos_type_System_Collections_System_Collections_Generic_Dictionary_System_String_System_Int32_
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR chaos_native_storage = 0;  // native runtime storage ptr
};

struct chaos_type_System_Collections_System_Collections_Generic_Dictionary_System_String_System_String_
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR chaos_native_storage = 0;  // native runtime storage ptr
};

struct chaos_type_System_Collections_System_Collections_Generic_HashSet_System_Byte_
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR chaos_native_storage = 0;  // native runtime storage ptr
};

struct chaos_type_System_Collections_System_Collections_Generic_HashSet_System_Int32_
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR chaos_native_storage = 0;  // native runtime storage ptr
};

struct chaos_type_System_Private_CoreLib_System_Byte
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Int32
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

CHAOS_IL2CPP_INT32 chaos_static_FrozenCollectionsSubjects_FrozenCollectionsSubjects___exitCode = 0;

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_0(void);
extern "C" void FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_1(void);
extern "C" void FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_2(void);
extern "C" void FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_3(void);
extern "C" void FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_4(void);
extern "C" void FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_5(void);
extern "C" void FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_6(void);
extern "C" void FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_7(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[9] = {
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
	::ChaosAbiMethodEntryV0 entries[8];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		8u,
		0u,
		1768495365u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_0
		{ 0u, 0u },  // FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_1
		{ 0u, 0u },  // FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_2
		{ 0u, 0u },  // FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_3
		{ 0u, 0u },  // FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_4
		{ 0u, 0u },  // FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_5
		{ 0u, 0u },  // FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_6
		{ 0u, 0u },  // FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_7
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
		"FrozenCollectionsSubjects",
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
		&chaos_mt_FrozenCollectionsSubjects_FrozenCollectionsSubjects.hot,
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
		/* .name_utf8         = */ "FrozenCollectionsSubjects",
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
		::chaos::il2cpp::runtime_core::RegisterModule("FrozenCollectionsSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[8] = {
	{ "CustomEntrySubject_0", 0x00000003u, 0u },  // FrozenCollectionsSubjects
	{ "CustomEntrySubject_1", 0x00000004u, 0u },  // FrozenCollectionsSubjects
	{ "CustomEntrySubject_2", 0x00000005u, 0u },  // FrozenCollectionsSubjects
	{ "CustomEntrySubject_3", 0x00000006u, 0u },  // FrozenCollectionsSubjects
	{ "CustomEntrySubject_4", 0x00000007u, 0u },  // FrozenCollectionsSubjects
	{ "CustomEntrySubject_5", 0x00000008u, 0u },  // FrozenCollectionsSubjects
	{ "CustomEntrySubject_6", 0x00000009u, 0u },  // FrozenCollectionsSubjects
	{ "CustomEntrySubject_7", 0x0000000Au, 0u },  // FrozenCollectionsSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "FrozenCollectionsSubjects", "", 0u, 8u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[8] = {
	{ 0x00000003u, 0u },
	{ 0x00000004u, 1u },
	{ 0x00000005u, 2u },
	{ 0x00000006u, 3u },
	{ 0x00000007u, 4u },
	{ 0x00000008u, 5u },
	{ 0x00000009u, 6u },
	{ 0x0000000Au, 7u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[8] = {
	{ reinterpret_cast<void*>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // FrozenCollectionsSubjects::CustomEntrySubject_0
	{ reinterpret_cast<void*>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // FrozenCollectionsSubjects::CustomEntrySubject_1
	{ reinterpret_cast<void*>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // FrozenCollectionsSubjects::CustomEntrySubject_2
	{ reinterpret_cast<void*>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // FrozenCollectionsSubjects::CustomEntrySubject_3
	{ reinterpret_cast<void*>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // FrozenCollectionsSubjects::CustomEntrySubject_4
	{ reinterpret_cast<void*>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // FrozenCollectionsSubjects::CustomEntrySubject_5
	{ reinterpret_cast<void*>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // FrozenCollectionsSubjects::CustomEntrySubject_6
	{ reinterpret_cast<void*>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // FrozenCollectionsSubjects::CustomEntrySubject_7
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"FrozenCollectionsSubjects",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	8u,
	s_hotpatch_slots,
	8u,
	s_hotpatch_entries,
	8u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[18] = {
	"System.Collections/System.Collections.Generic.Dictionary<System.String,System.Int32>::.ctor:System.Void()",
	"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary<System.String,System.Int32>:System.Collections.Frozen.FrozenDictionary<System.String,System.Int32>(System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.String,System.Int32>>,System.Collections.Generic.IEqualityComparer<System.String>)",
	"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary<System.String,System.Int32>::get_Count:System.Int32()",
	"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode",
	"System.Collections/System.Collections.Generic.Dictionary<System.Byte,System.Byte>::.ctor:System.Void()",
	"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary<System.Byte,System.Byte>:System.Collections.Frozen.FrozenDictionary<System.Byte,System.Byte>(System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.Byte,System.Byte>>,System.Collections.Generic.IEqualityComparer<System.Byte>)",
	"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary<System.Byte,System.Byte>::get_Count:System.Int32()",
	"System.Collections/System.Collections.Generic.Dictionary<System.String,System.String>::.ctor:System.Void()",
	"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary<System.String,System.String>:System.Collections.Frozen.FrozenDictionary<System.String,System.String>(System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.String,System.String>>,System.Collections.Generic.IEqualityComparer<System.String>)",
	"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary<System.String,System.String>::get_Count:System.Int32()",
	"System.Collections/System.Collections.Generic.HashSet<System.Byte>::.ctor:System.Void()",
	"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet<System.Byte>:System.Collections.Frozen.FrozenSet<System.Byte>(System.Collections.Generic.IEnumerable<System.Byte>,System.Collections.Generic.IEqualityComparer<System.Byte>)",
	"System.Collections.Immutable/System.Collections.Frozen.FrozenSet<System.Byte>::get_Count:System.Int32()",
	"System.Private.CoreLib/System.Byte",
	"System.Collections/System.Collections.Generic.HashSet<System.Int32>::.ctor:System.Void()",
	"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet<System.Int32>:System.Collections.Frozen.FrozenSet<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)",
	"System.Collections.Immutable/System.Collections.Frozen.FrozenSet<System.Int32>::get_Count:System.Int32()",
	"System.Private.CoreLib/System.Int32",
};

extern "C" void* kChaosExternalRuntimeFnTable[18] = {
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
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
};

extern "C" int32_t kChaosExternalRuntimeCount = 18;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[8])() = {
	reinterpret_cast<void(*)()>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_0),
	reinterpret_cast<void(*)()>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_1),
	reinterpret_cast<void(*)()>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_2),
	reinterpret_cast<void(*)()>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_3),
	reinterpret_cast<void(*)()>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_4),
	reinterpret_cast<void(*)()>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_5),
	reinterpret_cast<void(*)()>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_6),
	reinterpret_cast<void(*)()>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_7),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[8])() = {
	[]() {kAotMethods[0]();},
	[]() {kAotMethods[1]();},
	[]() {kAotMethods[2]();},
	[]() {kAotMethods[3]();},
	[]() {kAotMethods[4]();},
	[]() {kAotMethods[5]();},
	[]() {kAotMethods[6]();},
	[]() {kAotMethods[7]();},
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 8;
extern "C" const int kSubjectEntryIndices[8] = {
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
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

	/* ── Entry 0: FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry0;
	/* ── Entry 1: FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry1;
	/* ── Entry 2: FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry2;
	/* ── Entry 3: FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry3;
	/* ── Entry 4: FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry4;
	/* ── Entry 5: FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry5;
	/* ── Entry 6: FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_6 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry6;
	/* ── Entry 7: FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_7 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry7;
} kChaosGcSlotMapsSection = {
	/* entry0 = FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_0 */
	.entry0 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_0),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry1 = FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_1 */
	.entry1 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_1),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry2 = FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_2 */
	.entry2 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_2),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry3 = FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_3 */
	.entry3 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_3),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry4 = FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_4 */
	.entry4 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_4),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry5 = FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_5 */
	.entry5 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_5),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry6 = FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_6 */
	.entry6 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_6),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry7 = FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_7 */
	.entry7 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_7),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 344u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[8] = {
	reinterpret_cast<void*>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_0),
	reinterpret_cast<void*>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_1),
	reinterpret_cast<void*>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_2),
	reinterpret_cast<void*>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_3),
	reinterpret_cast<void*>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_4),
	reinterpret_cast<void*>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_5),
	reinterpret_cast<void*>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_6),
	reinterpret_cast<void*>(&FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_7),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 8u,
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
	.image_name_utf8    = "FrozenCollectionsSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_FrozenCollectionsSubjects_FrozenCollectionsSubjects[1] = {
	{ 0u, "FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_FrozenCollectionsSubjects_FrozenCollectionsSubjects[8] = {
	{ 0u, "FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_0:System.Void()", "CustomEntrySubject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_1:System.Void()", "CustomEntrySubject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_2:System.Void()", "CustomEntrySubject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_3:System.Void()", "CustomEntrySubject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_4:System.Void()", "CustomEntrySubject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_5:System.Void()", "CustomEntrySubject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_6:System.Void()", "CustomEntrySubject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_7:System.Void()", "CustomEntrySubject_7", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0u, "FrozenCollectionsSubjects/FrozenCollectionsSubjects", "FrozenCollectionsSubjects/FrozenCollectionsSubjects", "", "FrozenCollectionsSubjects", "FrozenCollectionsSubjects", nullptr, kReflFields_FrozenCollectionsSubjects_FrozenCollectionsSubjects, 1u, nullptr, 0u,
	kReflMethods_FrozenCollectionsSubjects_FrozenCollectionsSubjects, 8u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "FrozenCollectionsSubjects", kReflTypePtrs, 1u };

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
// Managed method: FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_0()
extern "C" void FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_0(void)
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


	CHAOS_EH_TRY
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Collections_System_Collections_Generic_Dictionary_System_String_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Collections_System_Collections_Generic_Dictionary_System_String_System_Int32_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s1 = 0;
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[1])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[2])();
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
				chaos_static_FrozenCollectionsSubjects_FrozenCollectionsSubjects___exitCode = chaos_value;
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
				chaos_static_FrozenCollectionsSubjects_FrozenCollectionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_1()
extern "C" void FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_1(void)
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


	CHAOS_EH_TRY
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Collections_System_Collections_Generic_Dictionary_System_Byte_System_Byte_, {});
				chaos_object->header.type_info = &chaos_mt_System_Collections_System_Collections_Generic_Dictionary_System_Byte_System_Byte_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s1 = 0;
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[5])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[6])();
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
				chaos_static_FrozenCollectionsSubjects_FrozenCollectionsSubjects___exitCode = chaos_value;
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
				chaos_static_FrozenCollectionsSubjects_FrozenCollectionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_2()
extern "C" void FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_2(void)
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


	CHAOS_EH_TRY
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Collections_System_Collections_Generic_Dictionary_System_String_System_String_, {});
				chaos_object->header.type_info = &chaos_mt_System_Collections_System_Collections_Generic_Dictionary_System_String_System_String_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s1 = 0;
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[8])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[9])();
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
				chaos_static_FrozenCollectionsSubjects_FrozenCollectionsSubjects___exitCode = chaos_value;
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
				chaos_static_FrozenCollectionsSubjects_FrozenCollectionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_3()
extern "C" void FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_3(void)
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


	CHAOS_EH_TRY
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Collections_System_Collections_Generic_HashSet_System_Byte_, {});
				chaos_object->header.type_info = &chaos_mt_System_Collections_System_Collections_Generic_HashSet_System_Byte_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s1 = 0;
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[11])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[12])();
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
				chaos_static_FrozenCollectionsSubjects_FrozenCollectionsSubjects___exitCode = chaos_value;
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
				chaos_static_FrozenCollectionsSubjects_FrozenCollectionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_4()
extern "C" void FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_4(void)
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


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
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
			_s1 = 0;
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[11])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[12])();
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
				chaos_static_FrozenCollectionsSubjects_FrozenCollectionsSubjects___exitCode = chaos_value;
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
				chaos_static_FrozenCollectionsSubjects_FrozenCollectionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_5()
extern "C" void FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_5(void)
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
			_s1 = 0;
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[11])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[12])();
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
				chaos_static_FrozenCollectionsSubjects_FrozenCollectionsSubjects___exitCode = chaos_value;
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
				chaos_static_FrozenCollectionsSubjects_FrozenCollectionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_6()
extern "C" void FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_6(void)
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


	CHAOS_EH_TRY
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Collections_System_Collections_Generic_HashSet_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Collections_System_Collections_Generic_HashSet_System_Int32_.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s1 = 0;
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[15])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[16])();
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
				chaos_static_FrozenCollectionsSubjects_FrozenCollectionsSubjects___exitCode = chaos_value;
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
				chaos_static_FrozenCollectionsSubjects_FrozenCollectionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_7()
extern "C" void FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_7(void)
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


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				const auto chaos_length = static_cast<CHAOS_IL2CPP_INT32>(_s0);
				if (chaos_length < 0)
				{
					CHAOS_IL2CPP_FAIL();
				}
				auto* chaos_array = CHAOS_IL2CPP_NEW_GC(chaos_managed_array, {});
				chaos_array->header.type_info = &chaos_type_info_managed_array.hot;
				chaos_array->element_type_shape = 1;
				chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
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
			_s1 = 0;
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[15])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[16])();
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
				chaos_static_FrozenCollectionsSubjects_FrozenCollectionsSubjects___exitCode = chaos_value;
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
				chaos_static_FrozenCollectionsSubjects_FrozenCollectionsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}



}  // namespace chaos::il2cpp::codegen::FrozenCollectionsSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 8;

// ── JIT Method Entry Table ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kMethodJson_0[] = "{\"methodId\":\"frozen-collections-subjects.frozen-collections-subjects.custom-entry-subject-0\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_0:System.Void()\",\"signature\":\"System.Void FrozenCollectionsSubjects::CustomEntrySubject_0()\",\"identity\":{\"assemblyName\":\"FrozenCollectionsSubjects\",\"declaringTypeSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects\",\"definitionSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_0:System.Void()\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_0:System.Void()\",\"methodId\":\"frozen-collections-subjects.frozen-collections-subjects.custom-entry-subject-0\",\"signature\":\"System.Void FrozenCollectionsSubjects::CustomEntrySubject_0()\"},\"nativeSymbol\":\"FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":35,\"handlerOffset\":36,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":2,\"resultType\":\"System.Collections/System.Collections.Generic.Dictionary\\u003CSystem.String,System.Int32\\u003E\",\"callee\":\"System.Collections/System.Collections.Generic.Dictionary\\u003CSystem.String,System.Int32\\u003E::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Collections\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections/System.Collections.Generic.Dictionary\\u003CSystem.String,System.Int32\\u003E::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Collections\",\"subjectId\":\"System.Collections/System.Collections.Generic.Dictionary\\u003CSystem.String,System.Int32\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":7,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":8,\"resultType\":\"System.Collections.Frozen.FrozenDictionary\\u003CSystem.String,System.Int32\\u003E\",\"callee\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary\\u003CSystem.String,System.Int32\\u003E:System.Collections.Frozen.FrozenDictionary\\u003CSystem.String,System.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003CSystem.String,System.Int32\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.String\\u003E)\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary\\u003CSystem.String,System.Int32\\u003E:System.Collections.Frozen.FrozenDictionary\\u003CSystem.String,System.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003CSystem.String,System.Int32\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.String\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary\\u003CSystem.String,System.Int32\\u003E:System.Collections.Frozen.FrozenDictionary\\u003CSystem.String,System.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003CSystem.String,System.Int32\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.String\\u003E)\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary:System.Collections.Frozen.FrozenDictionary\\u003C!!0,!!1\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003C!!0,!!1\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary:System.Collections.Frozen.FrozenDictionary\\u003C!!0,!!1\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003C!!0,!!1\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.String,System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary:System.Collections.Frozen.FrozenDictionary\\u003C!!0,!!1\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003C!!0,!!1\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.String,System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary:System.Collections.Frozen.FrozenDictionary\\u003C!!0,!!1\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003C!!0,!!1\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.String\",\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary:System.Collections.Frozen.FrozenDictionary\\u003C!!0,!!1\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003C!!0,!!1\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.String,System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary:System.Collections.Frozen.FrozenDictionary\\u003C!!0,!!1\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003C!!0,!!1\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.String,System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary\\u003CSystem.String,System.Int32\\u003E:System.Collections.Frozen.FrozenDictionary\\u003CSystem.String,System.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003CSystem.String,System.Int32\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.String\\u003E)\",\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary:System.Collections.Frozen.FrozenDictionary\\u003C!!0,!!1\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003C!!0,!!1\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"displaySubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary\\u003CSystem.String,System.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003CSystem.String,System.Int32\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.String\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary:System.Collections.Frozen.FrozenDictionary\\u003C!!0,!!1\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003C!!0,!!1\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.String\",\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"callee\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u003CSystem.String,System.Int32\\u003E::get_Count:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u003CSystem.String,System.Int32\\u003E::get_Count:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u003CSystem.String,System.Int32\\u003E::get_Count:System.Int32()\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u00602::get_Count:System.Int32()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u00602::get_Count:System.Int32();type=[System.String,System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u00602::get_Count:System.Int32();type=[System.String,System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u00602::get_Count:System.Int32()\",\"typeArguments\":[\"System.String\",\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u00602::get_Count:System.Int32();type=[System.String,System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u00602::get_Count:System.Int32();type=[System.String,System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u003CSystem.String,System.Int32\\u003E::get_Count:System.Int32()\",\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u00602::get_Count:System.Int32()\",\"displaySubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u003CSystem.String,System.Int32\\u003E::get_Count()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u00602::get_Count:System.Int32()\",\"typeArguments\":[\"System.String\",\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":24,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":33,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"ilOffset\":28,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":47,\"ilOffset\":34,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":36,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":38,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"ilOffset\":39,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":47,\"ilOffset\":45,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":47,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"FrozenCollectionsSubjects\"}";
static const char kMethodJson_1[] = "{\"methodId\":\"frozen-collections-subjects.frozen-collections-subjects.custom-entry-subject-1\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_1:System.Void()\",\"signature\":\"System.Void FrozenCollectionsSubjects::CustomEntrySubject_1()\",\"identity\":{\"assemblyName\":\"FrozenCollectionsSubjects\",\"declaringTypeSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects\",\"definitionSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_1:System.Void()\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_1:System.Void()\",\"methodId\":\"frozen-collections-subjects.frozen-collections-subjects.custom-entry-subject-1\",\"signature\":\"System.Void FrozenCollectionsSubjects::CustomEntrySubject_1()\"},\"nativeSymbol\":\"FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":35,\"handlerOffset\":36,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":2,\"resultType\":\"System.Collections/System.Collections.Generic.Dictionary\\u003CSystem.Byte,System.Byte\\u003E\",\"callee\":\"System.Collections/System.Collections.Generic.Dictionary\\u003CSystem.Byte,System.Byte\\u003E::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Collections\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections/System.Collections.Generic.Dictionary\\u003CSystem.Byte,System.Byte\\u003E::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Collections\",\"subjectId\":\"System.Collections/System.Collections.Generic.Dictionary\\u003CSystem.Byte,System.Byte\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":7,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":8,\"resultType\":\"System.Collections.Frozen.FrozenDictionary\\u003CSystem.Byte,System.Byte\\u003E\",\"callee\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary\\u003CSystem.Byte,System.Byte\\u003E:System.Collections.Frozen.FrozenDictionary\\u003CSystem.Byte,System.Byte\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003CSystem.Byte,System.Byte\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Byte\\u003E)\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary\\u003CSystem.Byte,System.Byte\\u003E:System.Collections.Frozen.FrozenDictionary\\u003CSystem.Byte,System.Byte\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003CSystem.Byte,System.Byte\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Byte\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary\\u003CSystem.Byte,System.Byte\\u003E:System.Collections.Frozen.FrozenDictionary\\u003CSystem.Byte,System.Byte\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003CSystem.Byte,System.Byte\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Byte\\u003E)\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary:System.Collections.Frozen.FrozenDictionary\\u003C!!0,!!1\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003C!!0,!!1\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary:System.Collections.Frozen.FrozenDictionary\\u003C!!0,!!1\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003C!!0,!!1\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.Byte,System.Byte]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary:System.Collections.Frozen.FrozenDictionary\\u003C!!0,!!1\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003C!!0,!!1\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.Byte,System.Byte]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary:System.Collections.Frozen.FrozenDictionary\\u003C!!0,!!1\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003C!!0,!!1\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Byte\",\"System.Byte\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary:System.Collections.Frozen.FrozenDictionary\\u003C!!0,!!1\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003C!!0,!!1\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.Byte,System.Byte]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary:System.Collections.Frozen.FrozenDictionary\\u003C!!0,!!1\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003C!!0,!!1\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.Byte,System.Byte]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary\\u003CSystem.Byte,System.Byte\\u003E:System.Collections.Frozen.FrozenDictionary\\u003CSystem.Byte,System.Byte\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003CSystem.Byte,System.Byte\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Byte\\u003E)\",\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary:System.Collections.Frozen.FrozenDictionary\\u003C!!0,!!1\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003C!!0,!!1\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"displaySubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary\\u003CSystem.Byte,System.Byte\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003CSystem.Byte,System.Byte\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Byte\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary:System.Collections.Frozen.FrozenDictionary\\u003C!!0,!!1\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003C!!0,!!1\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Byte\",\"System.Byte\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"callee\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u003CSystem.Byte,System.Byte\\u003E::get_Count:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u003CSystem.Byte,System.Byte\\u003E::get_Count:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u003CSystem.Byte,System.Byte\\u003E::get_Count:System.Int32()\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u00602::get_Count:System.Int32()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u00602::get_Count:System.Int32();type=[System.Byte,System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u00602::get_Count:System.Int32();type=[System.Byte,System.Byte];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u00602::get_Count:System.Int32()\",\"typeArguments\":[\"System.Byte\",\"System.Byte\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u00602::get_Count:System.Int32();type=[System.Byte,System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u00602::get_Count:System.Int32();type=[System.Byte,System.Byte];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u003CSystem.Byte,System.Byte\\u003E::get_Count:System.Int32()\",\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u00602::get_Count:System.Int32()\",\"displaySubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u003CSystem.Byte,System.Byte\\u003E::get_Count()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u00602::get_Count:System.Int32()\",\"typeArguments\":[\"System.Byte\",\"System.Byte\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":24,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":33,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"ilOffset\":28,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":47,\"ilOffset\":34,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":36,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":38,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"ilOffset\":39,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":47,\"ilOffset\":45,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":47,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"FrozenCollectionsSubjects\"}";
static const char kMethodJson_2[] = "{\"methodId\":\"frozen-collections-subjects.frozen-collections-subjects.custom-entry-subject-2\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_2:System.Void()\",\"signature\":\"System.Void FrozenCollectionsSubjects::CustomEntrySubject_2()\",\"identity\":{\"assemblyName\":\"FrozenCollectionsSubjects\",\"declaringTypeSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects\",\"definitionSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_2:System.Void()\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_2:System.Void()\",\"methodId\":\"frozen-collections-subjects.frozen-collections-subjects.custom-entry-subject-2\",\"signature\":\"System.Void FrozenCollectionsSubjects::CustomEntrySubject_2()\"},\"nativeSymbol\":\"FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_2\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":35,\"handlerOffset\":36,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":2,\"resultType\":\"System.Collections/System.Collections.Generic.Dictionary\\u003CSystem.String,System.String\\u003E\",\"callee\":\"System.Collections/System.Collections.Generic.Dictionary\\u003CSystem.String,System.String\\u003E::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Collections\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections/System.Collections.Generic.Dictionary\\u003CSystem.String,System.String\\u003E::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Collections\",\"subjectId\":\"System.Collections/System.Collections.Generic.Dictionary\\u003CSystem.String,System.String\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":7,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":8,\"resultType\":\"System.Collections.Frozen.FrozenDictionary\\u003CSystem.String,System.String\\u003E\",\"callee\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary\\u003CSystem.String,System.String\\u003E:System.Collections.Frozen.FrozenDictionary\\u003CSystem.String,System.String\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003CSystem.String,System.String\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.String\\u003E)\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary\\u003CSystem.String,System.String\\u003E:System.Collections.Frozen.FrozenDictionary\\u003CSystem.String,System.String\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003CSystem.String,System.String\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.String\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary\\u003CSystem.String,System.String\\u003E:System.Collections.Frozen.FrozenDictionary\\u003CSystem.String,System.String\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003CSystem.String,System.String\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.String\\u003E)\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary:System.Collections.Frozen.FrozenDictionary\\u003C!!0,!!1\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003C!!0,!!1\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary:System.Collections.Frozen.FrozenDictionary\\u003C!!0,!!1\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003C!!0,!!1\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.String,System.String]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary:System.Collections.Frozen.FrozenDictionary\\u003C!!0,!!1\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003C!!0,!!1\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.String,System.String]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary:System.Collections.Frozen.FrozenDictionary\\u003C!!0,!!1\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003C!!0,!!1\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.String\",\"System.String\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary:System.Collections.Frozen.FrozenDictionary\\u003C!!0,!!1\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003C!!0,!!1\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.String,System.String]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary:System.Collections.Frozen.FrozenDictionary\\u003C!!0,!!1\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003C!!0,!!1\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.String,System.String]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary\\u003CSystem.String,System.String\\u003E:System.Collections.Frozen.FrozenDictionary\\u003CSystem.String,System.String\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003CSystem.String,System.String\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.String\\u003E)\",\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary:System.Collections.Frozen.FrozenDictionary\\u003C!!0,!!1\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003C!!0,!!1\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"displaySubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary\\u003CSystem.String,System.String\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003CSystem.String,System.String\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.String\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::ToFrozenDictionary:System.Collections.Frozen.FrozenDictionary\\u003C!!0,!!1\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Collections.Generic.KeyValuePair\\u003C!!0,!!1\\u003E\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.String\",\"System.String\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"callee\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u003CSystem.String,System.String\\u003E::get_Count:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u003CSystem.String,System.String\\u003E::get_Count:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u003CSystem.String,System.String\\u003E::get_Count:System.Int32()\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u00602::get_Count:System.Int32()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u00602::get_Count:System.Int32();type=[System.String,System.String];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u00602::get_Count:System.Int32();type=[System.String,System.String];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u00602::get_Count:System.Int32()\",\"typeArguments\":[\"System.String\",\"System.String\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u00602::get_Count:System.Int32();type=[System.String,System.String];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u00602::get_Count:System.Int32();type=[System.String,System.String];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u003CSystem.String,System.String\\u003E::get_Count:System.Int32()\",\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u00602::get_Count:System.Int32()\",\"displaySubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u003CSystem.String,System.String\\u003E::get_Count()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary\\u00602::get_Count:System.Int32()\",\"typeArguments\":[\"System.String\",\"System.String\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":24,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":33,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"ilOffset\":28,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":47,\"ilOffset\":34,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":36,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":38,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"ilOffset\":39,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":47,\"ilOffset\":45,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":47,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"FrozenCollectionsSubjects\"}";
static const char kMethodJson_3[] = "{\"methodId\":\"frozen-collections-subjects.frozen-collections-subjects.custom-entry-subject-3\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_3:System.Void()\",\"signature\":\"System.Void FrozenCollectionsSubjects::CustomEntrySubject_3()\",\"identity\":{\"assemblyName\":\"FrozenCollectionsSubjects\",\"declaringTypeSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects\",\"definitionSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_3:System.Void()\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_3:System.Void()\",\"methodId\":\"frozen-collections-subjects.frozen-collections-subjects.custom-entry-subject-3\",\"signature\":\"System.Void FrozenCollectionsSubjects::CustomEntrySubject_3()\"},\"nativeSymbol\":\"FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_3\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":35,\"handlerOffset\":36,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":2,\"resultType\":\"System.Collections/System.Collections.Generic.HashSet\\u003CSystem.Byte\\u003E\",\"callee\":\"System.Collections/System.Collections.Generic.HashSet\\u003CSystem.Byte\\u003E::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Collections\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections/System.Collections.Generic.HashSet\\u003CSystem.Byte\\u003E::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Collections\",\"subjectId\":\"System.Collections/System.Collections.Generic.HashSet\\u003CSystem.Byte\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":7,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":8,\"resultType\":\"System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E\",\"callee\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet\\u003CSystem.Byte\\u003E:System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Byte\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Byte\\u003E)\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet\\u003CSystem.Byte\\u003E:System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Byte\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Byte\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet\\u003CSystem.Byte\\u003E:System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Byte\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Byte\\u003E)\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.Byte]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.Byte]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Byte\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.Byte]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.Byte]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet\\u003CSystem.Byte\\u003E:System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Byte\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Byte\\u003E)\",\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"displaySubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet\\u003CSystem.Byte\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Byte\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Byte\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Byte\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"callee\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E::get_Count:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E::get_Count:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E::get_Count:System.Int32()\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32();type=[System.Byte];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32();type=[System.Byte];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E::get_Count:System.Int32()\",\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32()\",\"displaySubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E::get_Count()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":24,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":33,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"ilOffset\":28,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":47,\"ilOffset\":34,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":36,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":38,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"ilOffset\":39,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":47,\"ilOffset\":45,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":47,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"FrozenCollectionsSubjects\"}";
static const char kMethodJson_4[] = "{\"methodId\":\"frozen-collections-subjects.frozen-collections-subjects.custom-entry-subject-4\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_4:System.Void()\",\"signature\":\"System.Void FrozenCollectionsSubjects::CustomEntrySubject_4()\",\"identity\":{\"assemblyName\":\"FrozenCollectionsSubjects\",\"declaringTypeSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects\",\"definitionSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_4:System.Void()\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_4:System.Void()\",\"methodId\":\"frozen-collections-subjects.frozen-collections-subjects.custom-entry-subject-4\",\"signature\":\"System.Void FrozenCollectionsSubjects::CustomEntrySubject_4()\"},\"nativeSymbol\":\"FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_4\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":44,\"handlerOffset\":45,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":3,\"resultType\":\"System.Byte[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stelem\",\"opCode\":45,\"operand\":\"System.SByte\",\"ilOffset\":12,\"resultType\":\"System.Void\",\"runtimeServiceKind\":13,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":13,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":14,\"resultType\":\"System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E\",\"callee\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet\\u003CSystem.Byte\\u003E:System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Byte\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Byte\\u003E)\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet\\u003CSystem.Byte\\u003E:System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Byte\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Byte\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet\\u003CSystem.Byte\\u003E:System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Byte\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Byte\\u003E)\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.Byte]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.Byte]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Byte\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.Byte]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.Byte]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet\\u003CSystem.Byte\\u003E:System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Byte\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Byte\\u003E)\",\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"displaySubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet\\u003CSystem.Byte\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Byte\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Byte\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Byte\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":20,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"callee\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E::get_Count:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E::get_Count:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E::get_Count:System.Int32()\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32();type=[System.Byte];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32();type=[System.Byte];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E::get_Count:System.Int32()\",\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32()\",\"displaySubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E::get_Count()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":30,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":33,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":42,\"ilOffset\":34,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":36,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"ilOffset\":37,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":56,\"ilOffset\":43,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":45,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":47,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"ilOffset\":48,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":56,\"ilOffset\":54,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":56,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"FrozenCollectionsSubjects\"}";
static const char kMethodJson_5[] = "{\"methodId\":\"frozen-collections-subjects.frozen-collections-subjects.custom-entry-subject-5\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_5:System.Void()\",\"signature\":\"System.Void FrozenCollectionsSubjects::CustomEntrySubject_5()\",\"identity\":{\"assemblyName\":\"FrozenCollectionsSubjects\",\"declaringTypeSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects\",\"definitionSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_5:System.Void()\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_5:System.Void()\",\"methodId\":\"frozen-collections-subjects.frozen-collections-subjects.custom-entry-subject-5\",\"signature\":\"System.Void FrozenCollectionsSubjects::CustomEntrySubject_5()\"},\"nativeSymbol\":\"FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_5\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":49,\"handlerOffset\":50,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":2,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":3,\"resultType\":\"System.Byte[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stelem\",\"opCode\":45,\"operand\":\"System.SByte\",\"ilOffset\":12,\"resultType\":\"System.Void\",\"runtimeServiceKind\":13,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":43,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stelem\",\"opCode\":45,\"operand\":\"System.SByte\",\"ilOffset\":17,\"resultType\":\"System.Void\",\"runtimeServiceKind\":13,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":18,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":19,\"resultType\":\"System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E\",\"callee\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet\\u003CSystem.Byte\\u003E:System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Byte\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Byte\\u003E)\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet\\u003CSystem.Byte\\u003E:System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Byte\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Byte\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet\\u003CSystem.Byte\\u003E:System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Byte\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Byte\\u003E)\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.Byte]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.Byte]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Byte\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.Byte]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.Byte]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet\\u003CSystem.Byte\\u003E:System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Byte\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Byte\\u003E)\",\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"displaySubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet\\u003CSystem.Byte\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Byte\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Byte\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Byte\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":24,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":25,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"callee\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E::get_Count:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E::get_Count:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E::get_Count:System.Int32()\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32();type=[System.Byte];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32();type=[System.Byte];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E::get_Count:System.Int32()\",\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32()\",\"displaySubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u003CSystem.Byte\\u003E::get_Count()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":2,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":34,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":37,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":38,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":47,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":41,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"ilOffset\":42,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":61,\"ilOffset\":48,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":50,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":52,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"ilOffset\":53,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":61,\"ilOffset\":59,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":61,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"FrozenCollectionsSubjects\"}";
static const char kMethodJson_6[] = "{\"methodId\":\"frozen-collections-subjects.frozen-collections-subjects.custom-entry-subject-6\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_6:System.Void()\",\"signature\":\"System.Void FrozenCollectionsSubjects::CustomEntrySubject_6()\",\"identity\":{\"assemblyName\":\"FrozenCollectionsSubjects\",\"declaringTypeSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects\",\"definitionSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_6:System.Void()\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_6:System.Void()\",\"methodId\":\"frozen-collections-subjects.frozen-collections-subjects.custom-entry-subject-6\",\"signature\":\"System.Void FrozenCollectionsSubjects::CustomEntrySubject_6()\"},\"nativeSymbol\":\"FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_6\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":35,\"handlerOffset\":36,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":2,\"resultType\":\"System.Collections/System.Collections.Generic.HashSet\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Collections/System.Collections.Generic.HashSet\\u003CSystem.Int32\\u003E::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Collections\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections/System.Collections.Generic.HashSet\\u003CSystem.Int32\\u003E::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Collections\",\"subjectId\":\"System.Collections/System.Collections.Generic.HashSet\\u003CSystem.Int32\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":7,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":8,\"resultType\":\"System.Collections.Frozen.FrozenSet\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet\\u003CSystem.Int32\\u003E:System.Collections.Frozen.FrozenSet\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Int32\\u003E)\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet\\u003CSystem.Int32\\u003E:System.Collections.Frozen.FrozenSet\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Int32\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet\\u003CSystem.Int32\\u003E:System.Collections.Frozen.FrozenSet\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Int32\\u003E)\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet\\u003CSystem.Int32\\u003E:System.Collections.Frozen.FrozenSet\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Int32\\u003E)\",\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"displaySubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Int32\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"callee\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u003CSystem.Int32\\u003E::get_Count:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u003CSystem.Int32\\u003E::get_Count:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u003CSystem.Int32\\u003E::get_Count:System.Int32()\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32();type=[System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32();type=[System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u003CSystem.Int32\\u003E::get_Count:System.Int32()\",\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32()\",\"displaySubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u003CSystem.Int32\\u003E::get_Count()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":24,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":33,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"ilOffset\":28,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":47,\"ilOffset\":34,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":36,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":38,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"ilOffset\":39,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":47,\"ilOffset\":45,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":47,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"FrozenCollectionsSubjects\"}";
static const char kMethodJson_7[] = "{\"methodId\":\"frozen-collections-subjects.frozen-collections-subjects.custom-entry-subject-7\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_7:System.Void()\",\"signature\":\"System.Void FrozenCollectionsSubjects::CustomEntrySubject_7()\",\"identity\":{\"assemblyName\":\"FrozenCollectionsSubjects\",\"declaringTypeSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects\",\"definitionSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_7:System.Void()\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::CustomEntrySubject_7:System.Void()\",\"methodId\":\"frozen-collections-subjects.frozen-collections-subjects.custom-entry-subject-7\",\"signature\":\"System.Void FrozenCollectionsSubjects::CustomEntrySubject_7()\"},\"nativeSymbol\":\"FrozenCollectionsSubjects_FrozenCollectionsSubjects_CustomEntrySubject_7\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":44,\"handlerOffset\":45,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":3,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stelem\",\"opCode\":45,\"operand\":\"System.Int32\",\"ilOffset\":12,\"resultType\":\"System.Void\",\"runtimeServiceKind\":13,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":13,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":14,\"resultType\":\"System.Collections.Frozen.FrozenSet\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet\\u003CSystem.Int32\\u003E:System.Collections.Frozen.FrozenSet\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Int32\\u003E)\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet\\u003CSystem.Int32\\u003E:System.Collections.Frozen.FrozenSet\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Int32\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet\\u003CSystem.Int32\\u003E:System.Collections.Frozen.FrozenSet\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Int32\\u003E)\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet\\u003CSystem.Int32\\u003E:System.Collections.Frozen.FrozenSet\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Int32\\u003E)\",\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"displaySubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Collections.Generic.IEqualityComparer\\u003CSystem.Int32\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet::ToFrozenSet:System.Collections.Frozen.FrozenSet\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEqualityComparer\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":20,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"callee\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u003CSystem.Int32\\u003E::get_Count:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u003CSystem.Int32\\u003E::get_Count:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u003CSystem.Int32\\u003E::get_Count:System.Int32()\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32();type=[System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32();type=[System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u003CSystem.Int32\\u003E::get_Count:System.Int32()\",\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32()\",\"displaySubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u003CSystem.Int32\\u003E::get_Count()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections.Immutable/System.Collections.Frozen.FrozenSet\\u00601::get_Count:System.Int32()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":30,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":33,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":42,\"ilOffset\":34,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":36,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"ilOffset\":37,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":56,\"ilOffset\":43,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":45,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":47,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"ilOffset\":48,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"FrozenCollectionsSubjects\",\"subjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"FrozenCollectionsSubjects/FrozenCollectionsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":56,\"ilOffset\":54,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":56,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"FrozenCollectionsSubjects\"}";

extern "C" const JitMethodEntry kChaosJitMethodEntries[8] =
{
    { kMethodJson_0, 13806u, 0x00000003u, 0u },
    { kMethodJson_1, 13688u, 0x00000004u, 0u },
    { kMethodJson_2, 13842u, 0x00000005u, 0u },
    { kMethodJson_3, 12145u, 0x00000006u, 0u },
    { kMethodJson_4, 12620u, 0x00000007u, 0u },
    { kMethodJson_5, 13028u, 0x00000008u, 0u },
    { kMethodJson_6, 12186u, 0x00000009u, 0u },
    { kMethodJson_7, 12661u, 0x0000000Au, 0u }
};

extern "C" const uint32_t kChaosJitMethodEntryCount = 8u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitMethods(kChaosJitMethodEntries, kChaosJitMethodEntryCount);
}