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

namespace chaos::il2cpp::codegen::GenerationConcatSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_GenerationConcatSubjects__PrivateImplementationDetails_ = {{nullptr, nullptr, 830076558182334290ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_GenerationConcatSubjects__PrivateImplementationDetails_ = static_cast<CHAOS_IL2CPP_INTPTR>(830076558182334290ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Int32 = {{nullptr, nullptr, 11009693519287992193ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Int32 = static_cast<CHAOS_IL2CPP_INTPTR>(11009693519287992193ULL);
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

struct chaos_type_GenerationConcatSubjects__PrivateImplementationDetails_
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR field_GenerationConcatSubjects__PrivateImplementationDetails___4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D = 0;
	CHAOS_IL2CPP_INTPTR field_GenerationConcatSubjects__PrivateImplementationDetails___A10494D90314704E24CA5786F6376A6097558F10BB7880B539C8B80312DCA080 = 0;
};

struct chaos_type_System_Private_CoreLib_System_Int32
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

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_GenerationConcatSubjects__PrivateImplementationDetails___4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D[] =
{
	0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
};

const CHAOS_IL2CPP_UINT8 chaos_static_field_data_GenerationConcatSubjects__PrivateImplementationDetails___A10494D90314704E24CA5786F6376A6097558F10BB7880B539C8B80312DCA080[] =
{
	0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
};

bool chaos_try_get_static_field_data(
	CHAOS_IL2CPP_INTPTR chaos_field_handle,
	const CHAOS_IL2CPP_UINT8*& chaos_data,
	CHAOS_IL2CPP_SIZE& chaos_size)
{
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(76086948u))
	{
		chaos_data = chaos_static_field_data_GenerationConcatSubjects__PrivateImplementationDetails___4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D;
		chaos_size = sizeof(chaos_static_field_data_GenerationConcatSubjects__PrivateImplementationDetails___4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D);
		return true;
	}
	if (chaos_field_handle == static_cast<CHAOS_IL2CPP_INTPTR>(71182315u))
	{
		chaos_data = chaos_static_field_data_GenerationConcatSubjects__PrivateImplementationDetails___A10494D90314704E24CA5786F6376A6097558F10BB7880B539C8B80312DCA080;
		chaos_size = sizeof(chaos_static_field_data_GenerationConcatSubjects__PrivateImplementationDetails___A10494D90314704E24CA5786F6376A6097558F10BB7880B539C8B80312DCA080);
		return true;
	}
	chaos_data = nullptr;
	chaos_size = 0;
	return false;
}

void chaos_initialize_array_from_field_data_int32(CHAOS_IL2CPP_INTPTR chaos_array_value, CHAOS_IL2CPP_INTPTR chaos_field_handle)
{
	auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_array_value);
	if (chaos_array == nullptr || chaos_array->element_type_shape == 0)
	{
		CHAOS_IL2CPP_FAIL();
	}
	const CHAOS_IL2CPP_UINT8* chaos_bytes = nullptr;
	CHAOS_IL2CPP_SIZE chaos_size = 0;
	if (!chaos_try_get_static_field_data(chaos_field_handle, chaos_bytes, chaos_size))
	{
		CHAOS_IL2CPP_FAIL();
	}
	const auto copy_count = static_cast<CHAOS_IL2CPP_INT32>(
		chaos_size < static_cast<CHAOS_IL2CPP_SIZE>(chaos_array->length)
			? chaos_size
			: static_cast<CHAOS_IL2CPP_SIZE>(chaos_array->length));
	for (CHAOS_IL2CPP_INT32 chaos_index = 0; chaos_index < copy_count; chaos_index++)
	{
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] =
			static_cast<CHAOS_IL2CPP_INTPTR>(chaos_bytes[chaos_index]);
	}
}

CHAOS_IL2CPP_INTPTR chaos_create_field_data_span_int32(CHAOS_IL2CPP_INTPTR chaos_field_handle)
{
	const CHAOS_IL2CPP_UINT8* chaos_bytes = nullptr;
	CHAOS_IL2CPP_SIZE chaos_size = 0;
	if (!chaos_try_get_static_field_data(chaos_field_handle, chaos_bytes, chaos_size) ||
		(chaos_size % sizeof(CHAOS_IL2CPP_INT32)) != 0)
	{
		CHAOS_IL2CPP_FAIL();
	}
	return ChaosIl2cpp::Common::create_raw_span_int32(const_cast<CHAOS_IL2CPP_UINT8*>(chaos_bytes), static_cast<CHAOS_IL2CPP_INT32>(chaos_size / sizeof(CHAOS_IL2CPP_INT32)), true);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	chaos_initialize_array_from_field_data_int32(chaos_fn_arg_0, chaos_fn_arg_1);
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_0(void);
extern "C" void GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_1(void);
extern "C" void GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_2(void);
extern "C" void GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_3(void);
extern "C" void GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_4(void);
extern "C" void GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_5(void);
extern "C" void GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_6(void);
extern "C" void GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_7(void);
extern "C" void GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_8(void);
extern "C" void GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_9(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[11] = {
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
	::ChaosAbiMethodEntryV0 entries[10];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		10u,
		0u,
		3120489557u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_0
		{ 0u, 0u },  // GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_1
		{ 0u, 0u },  // GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_2
		{ 0u, 0u },  // GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_3
		{ 0u, 0u },  // GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_4
		{ 0u, 0u },  // GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_5
		{ 0u, 0u },  // GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_6
		{ 0u, 0u },  // GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_7
		{ 0u, 0u },  // GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_8
		{ 0u, 0u },  // GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_9
	},
	{
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[4] = {
		0u,
		1548u,
		8u,
		1545u,
	};

	static constexpr const char* s_type_names[4] = {
		"<Module>",
		"GenerationConcatSubjects",
		"<PrivateImplementationDetails>",
		"__StaticArrayInitTypeSize=12",
	};

	static constexpr const char* s_type_namespaces[4] = {
		"",
		"",
		"",
		"",
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_type_parent_tokens[4] = {
		0u,
		0u,
		0u,
		0u,
	};

	static const TypeInfoHot* const s_type_info_ptrs[4] = {
		nullptr,
		nullptr,
		&chaos_mt_GenerationConcatSubjects__PrivateImplementationDetails_.hot,
		nullptr,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_children[1] = {
		33554436u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[5] = {
		0u,
		0u,
		0u,
		1u,
		1u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset[5] = {
		0u,
		0u,
		0u,
		0u,
		0u,
	};



	static const ::chaos::il2cpp::runtime_core::ModuleDescriptor s_native_aot_module = {
		/* .name_utf8         = */ "GenerationConcatSubjects",
		/* .image             = */ &kReflImage,
		/* .type_flags        = */ s_type_flags,
		/* .type_names        = */ s_type_names,
		/* .type_namespaces   = */ s_type_namespaces,
		/* .type_parent_tokens= */ s_type_parent_tokens,
		/* .type_info_ptrs    = */ s_type_info_ptrs,
		/* .nested_type_children= */ s_nested_type_children,
		/* .nested_type_offset = */ s_nested_type_offset,
		/* .generic_param_constraint_data= */ nullptr,
		/* .generic_param_constraint_offset= */ s_generic_param_constraint_offset,
		/* .type_count        = */ 4u,
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 — deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,

		/* .abi_manifest      = */ s_abi_manifest,
	};
	static const CHAOS_IL2CPP_UINT32 s_native_aot_module_id =
		::chaos::il2cpp::runtime_core::RegisterModule("GenerationConcatSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[10] = {
	{ "CustomEntrySubject_0", 0x00000007u, 0u },  // GenerationConcatSubjects
	{ "CustomEntrySubject_1", 0x00000008u, 0u },  // GenerationConcatSubjects
	{ "CustomEntrySubject_2", 0x00000009u, 0u },  // GenerationConcatSubjects
	{ "CustomEntrySubject_3", 0x0000000Au, 0u },  // GenerationConcatSubjects
	{ "CustomEntrySubject_4", 0x0000000Bu, 0u },  // GenerationConcatSubjects
	{ "CustomEntrySubject_5", 0x0000000Cu, 0u },  // GenerationConcatSubjects
	{ "CustomEntrySubject_6", 0x0000000Du, 0u },  // GenerationConcatSubjects
	{ "CustomEntrySubject_7", 0x0000000Eu, 0u },  // GenerationConcatSubjects
	{ "CustomEntrySubject_8", 0x0000000Fu, 0u },  // GenerationConcatSubjects
	{ "CustomEntrySubject_9", 0x00000010u, 0u },  // GenerationConcatSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "GenerationConcatSubjects", "", 0u, 10u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[10] = {
	{ 0x00000007u, 0u },
	{ 0x00000008u, 1u },
	{ 0x00000009u, 2u },
	{ 0x0000000Au, 3u },
	{ 0x0000000Bu, 4u },
	{ 0x0000000Cu, 5u },
	{ 0x0000000Du, 6u },
	{ 0x0000000Eu, 7u },
	{ 0x0000000Fu, 8u },
	{ 0x00000010u, 9u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[10] = {
	{ reinterpret_cast<void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // GenerationConcatSubjects::CustomEntrySubject_0
	{ reinterpret_cast<void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // GenerationConcatSubjects::CustomEntrySubject_1
	{ reinterpret_cast<void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // GenerationConcatSubjects::CustomEntrySubject_2
	{ reinterpret_cast<void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // GenerationConcatSubjects::CustomEntrySubject_3
	{ reinterpret_cast<void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // GenerationConcatSubjects::CustomEntrySubject_4
	{ reinterpret_cast<void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // GenerationConcatSubjects::CustomEntrySubject_5
	{ reinterpret_cast<void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // GenerationConcatSubjects::CustomEntrySubject_6
	{ reinterpret_cast<void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // GenerationConcatSubjects::CustomEntrySubject_7
	{ reinterpret_cast<void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // GenerationConcatSubjects::CustomEntrySubject_8
	{ reinterpret_cast<void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // GenerationConcatSubjects::CustomEntrySubject_9
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"GenerationConcatSubjects",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	10u,
	s_hotpatch_slots,
	10u,
	s_hotpatch_entries,
	10u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[14] = {
	"System.Private.CoreLib/System.Int32",
	"GenerationConcatSubjects/<PrivateImplementationDetails>::4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D",
	"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)",
	"System.Linq/Enumerable::Append<System.Int32>:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)",
	"System.Private.CoreLib/System.Collections.Generic.IEnumerable<System.Int32>::GetEnumerator:System.Collections.Generic.IEnumerator<System.Int32>()",
	"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()",
	"System.Linq/Enumerable::Concat<System.Int32>:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)",
	"System.Linq/Enumerable::DefaultIfEmpty<System.Int32>:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)",
	"System.Linq/Enumerable::DefaultIfEmpty<System.Int32>:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)",
	"System.Linq/Enumerable::Empty<System.Int32>:System.Collections.Generic.IEnumerable<System.Int32>()",
	"System.Linq/Enumerable::Range:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32,System.Int32)",
	"GenerationConcatSubjects/<PrivateImplementationDetails>::A10494D90314704E24CA5786F6376A6097558F10BB7880B539C8B80312DCA080",
	"System.Linq/Enumerable::Prepend<System.Int32>:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)",
	"System.Linq/Enumerable::Repeat<System.Int32>:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32,System.Int32)",
};

extern "C" void* kChaosExternalRuntimeFnTable[14] = {
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers__InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle_),
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

extern "C" int32_t kChaosExternalRuntimeCount = 14;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[10])() = {
	reinterpret_cast<void(*)()>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_0),
	reinterpret_cast<void(*)()>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_1),
	reinterpret_cast<void(*)()>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_2),
	reinterpret_cast<void(*)()>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_3),
	reinterpret_cast<void(*)()>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_4),
	reinterpret_cast<void(*)()>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_5),
	reinterpret_cast<void(*)()>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_6),
	reinterpret_cast<void(*)()>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_7),
	reinterpret_cast<void(*)()>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_8),
	reinterpret_cast<void(*)()>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_9),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[10])() = {
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
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 10;
extern "C" const int kSubjectEntryIndices[10] = {
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

	/* ── Entry 0: GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry0;
	/* ── Entry 1: GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[10];
	} entry1;
	/* ── Entry 2: GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry2;
	/* ── Entry 3: GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry3;
	/* ── Entry 4: GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry4;
	/* ── Entry 5: GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry5;
	/* ── Entry 6: GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_6 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry6;
	/* ── Entry 7: GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_7 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry7;
	/* ── Entry 8: GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_8 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry8;
	/* ── Entry 9: GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_9 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry9;
} kChaosGcSlotMapsSection = {
	/* entry0 = GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_0 */
	.entry0 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_0),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry1 = GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_1 */
	.entry1 = {
		/* entry_total_size = 60 */ 60u,
		/* code_address */ reinterpret_cast<const void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_1),
		/* frame_size = 80 */ 80u,
		/* num_gc_slots = 10 */ 10u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u }
	},
	/* entry2 = GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_2 */
	.entry2 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_2),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry3 = GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_3 */
	.entry3 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_3),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry4 = GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_4 */
	.entry4 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_4),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry5 = GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_5 */
	.entry5 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_5),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry6 = GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_6 */
	.entry6 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_6),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry7 = GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_7 */
	.entry7 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_7),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry8 = GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_8 */
	.entry8 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_8),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry9 = GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_9 */
	.entry9 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_9),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 456u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[10] = {
	reinterpret_cast<void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_0),
	reinterpret_cast<void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_1),
	reinterpret_cast<void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_2),
	reinterpret_cast<void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_3),
	reinterpret_cast<void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_4),
	reinterpret_cast<void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_5),
	reinterpret_cast<void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_6),
	reinterpret_cast<void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_7),
	reinterpret_cast<void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_8),
	reinterpret_cast<void*>(&GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_9),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 10u,
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
	.image_name_utf8    = "GenerationConcatSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_GenerationConcatSubjects_GenerationConcatSubjects[1] = {
	{ 0u, "GenerationConcatSubjects/GenerationConcatSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_GenerationConcatSubjects_GenerationConcatSubjects[10] = {
	{ 0u, "GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_0:System.Void()", "CustomEntrySubject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_1:System.Void()", "CustomEntrySubject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_2:System.Void()", "CustomEntrySubject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_3:System.Void()", "CustomEntrySubject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_4:System.Void()", "CustomEntrySubject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_5:System.Void()", "CustomEntrySubject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_6:System.Void()", "CustomEntrySubject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_7:System.Void()", "CustomEntrySubject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_8:System.Void()", "CustomEntrySubject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_9:System.Void()", "CustomEntrySubject_9", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0u, "GenerationConcatSubjects/GenerationConcatSubjects", "GenerationConcatSubjects/GenerationConcatSubjects", "", "GenerationConcatSubjects", "GenerationConcatSubjects", nullptr, kReflFields_GenerationConcatSubjects_GenerationConcatSubjects, 1u, nullptr, 0u,
	kReflMethods_GenerationConcatSubjects_GenerationConcatSubjects, 10u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "GenerationConcatSubjects", kReflTypePtrs, 1u };

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
// Managed method: GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_0()
extern "C" void GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_0(void)
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
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(76086948u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[3])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[4])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[2] = _s1;
	_s1 = chaos_locals[2];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[5])();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_1()
extern "C" void GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 4) chaos_locals{};
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	{
		auto chaos_value_raw = _s3;
		const auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
	}
	chaos_locals[0] = _s0;
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
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
	{
		auto chaos_value_raw = _s3;
		const auto chaos_value = chaos_value_raw;
		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s2);
		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s1);
		chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[1];
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[6])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[2];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[4])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[3] = _s1;
	_s1 = chaos_locals[3];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[5])();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_2()
extern "C" void GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_2(void)
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
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(76086948u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[7])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[4])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[2] = _s1;
	_s1 = chaos_locals[2];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[5])();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_3()
extern "C" void GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_3(void)
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
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(76086948u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[8])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[4])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[2] = _s1;
	_s1 = chaos_locals[2];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[5])();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_4()
extern "C" void GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[9])();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[4])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s1;
	_s1 = chaos_locals[1];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[5])();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_5()
extern "C" void GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_5(void)
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
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(100);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[10])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[4])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s1;
	_s1 = chaos_locals[1];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[5])();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_6()
extern "C" void GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_6(void)
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
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(71182315u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[12])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[4])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[2] = _s1;
	_s1 = chaos_locals[2];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[5])();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_7()
extern "C" void GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_7(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[10])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[4])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s1;
	_s1 = chaos_locals[1];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[5])();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_8()
extern "C" void GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_8(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[13])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[4])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s1;
	_s1 = chaos_locals[1];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[5])();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_9()
extern "C" void GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_9(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
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
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(76086948u);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[4])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s1;
	_s1 = chaos_locals[1];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[5])();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}



}  // namespace chaos::il2cpp::codegen::GenerationConcatSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 10;

// ── JIT Method Entry Table ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kMethodJson_0[] = "{\"methodId\":\"generation-concat-subjects.generation-concat-subjects.custom-entry-subject-0\",\"subjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_0:System.Void()\",\"signature\":\"System.Void GenerationConcatSubjects::CustomEntrySubject_0()\",\"identity\":{\"assemblyName\":\"GenerationConcatSubjects\",\"declaringTypeSubjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects\",\"definitionSubjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_0:System.Void()\",\"subjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_0:System.Void()\",\"methodId\":\"generation-concat-subjects.generation-concat-subjects.custom-entry-subject-0\",\"signature\":\"System.Void GenerationConcatSubjects::CustomEntrySubject_0()\"},\"nativeSymbol\":\"GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":3,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":3,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"GenerationConcatSubjects/\\u003CPrivateImplementationDetails\\u003E::4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D\",\"ilOffset\":8,\"resultType\":\"System.RuntimeFieldHandle\",\"reference\":{\"assemblyName\":\"GenerationConcatSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"GenerationConcatSubjects/\\u003CPrivateImplementationDetails\\u003E::4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"GenerationConcatSubjects\",\"subjectId\":\"GenerationConcatSubjects/\\u003CPrivateImplementationDetails\\u003E::4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"GenerationConcatSubjects/\\u003CPrivateImplementationDetails\\u003E\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"\\u003CPrivateImplementationDetails\\u003E\\u002B__StaticArrayInitTypeSize=12\"},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":4,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":21,\"resultType\":\"System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Linq/Enumerable::Append\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::Append\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::Append\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::Append:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::Append:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::Append:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::Append:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::Append:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::Append:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::Append\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"definitionSubjectId\":\"System.Linq/Enumerable::Append:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0)\",\"displaySubjectId\":\"System.Linq/Enumerable::Append\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::Append:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":26,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":27,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":28,\"resultType\":\"System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"displaySubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":33,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":34,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":35,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":40,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"GenerationConcatSubjects\"}";
static const char kMethodJson_1[] = "{\"methodId\":\"generation-concat-subjects.generation-concat-subjects.custom-entry-subject-1\",\"subjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_1:System.Void()\",\"signature\":\"System.Void GenerationConcatSubjects::CustomEntrySubject_1()\",\"identity\":{\"assemblyName\":\"GenerationConcatSubjects\",\"declaringTypeSubjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects\",\"definitionSubjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_1:System.Void()\",\"subjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_1:System.Void()\",\"methodId\":\"generation-concat-subjects.generation-concat-subjects.custom-entry-subject-1\",\"signature\":\"System.Void GenerationConcatSubjects::CustomEntrySubject_1()\"},\"nativeSymbol\":\"GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":4,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":2,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stelem\",\"opCode\":45,\"operand\":\"System.Int32\",\"ilOffset\":10,\"resultType\":\"System.Void\",\"runtimeServiceKind\":13,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":11,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":2,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stelem\",\"opCode\":45,\"operand\":\"System.Int32\",\"ilOffset\":14,\"resultType\":\"System.Void\",\"runtimeServiceKind\":13,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":15,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":2,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":17,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":22,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":3,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stelem\",\"opCode\":45,\"operand\":\"System.Int32\",\"ilOffset\":25,\"resultType\":\"System.Void\",\"runtimeServiceKind\":13,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":4,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stelem\",\"opCode\":45,\"operand\":\"System.Int32\",\"ilOffset\":29,\"resultType\":\"System.Void\",\"runtimeServiceKind\":13,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":30,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":31,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":33,\"resultType\":\"System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Linq/Enumerable::Concat\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::Concat\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::Concat\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::Concat:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::Concat:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::Concat:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::Concat:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::Concat:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::Concat:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::Concat\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"definitionSubjectId\":\"System.Linq/Enumerable::Concat:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"displaySubjectId\":\"System.Linq/Enumerable::Concat\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::Concat:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":38,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":39,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":40,\"resultType\":\"System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"displaySubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":45,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":3,\"ilOffset\":46,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":47,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":52,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":53,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"GenerationConcatSubjects\"}";
static const char kMethodJson_2[] = "{\"methodId\":\"generation-concat-subjects.generation-concat-subjects.custom-entry-subject-2\",\"subjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_2:System.Void()\",\"signature\":\"System.Void GenerationConcatSubjects::CustomEntrySubject_2()\",\"identity\":{\"assemblyName\":\"GenerationConcatSubjects\",\"declaringTypeSubjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects\",\"definitionSubjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_2:System.Void()\",\"subjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_2:System.Void()\",\"methodId\":\"generation-concat-subjects.generation-concat-subjects.custom-entry-subject-2\",\"signature\":\"System.Void GenerationConcatSubjects::CustomEntrySubject_2()\"},\"nativeSymbol\":\"GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_2\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":3,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":3,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"GenerationConcatSubjects/\\u003CPrivateImplementationDetails\\u003E::4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D\",\"ilOffset\":8,\"resultType\":\"System.RuntimeFieldHandle\",\"reference\":{\"assemblyName\":\"GenerationConcatSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"GenerationConcatSubjects/\\u003CPrivateImplementationDetails\\u003E::4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"GenerationConcatSubjects\",\"subjectId\":\"GenerationConcatSubjects/\\u003CPrivateImplementationDetails\\u003E::4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"GenerationConcatSubjects/\\u003CPrivateImplementationDetails\\u003E\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"\\u003CPrivateImplementationDetails\\u003E\\u002B__StaticArrayInitTypeSize=12\"},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":20,\"resultType\":\"System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Linq/Enumerable::DefaultIfEmpty\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::DefaultIfEmpty\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::DefaultIfEmpty\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::DefaultIfEmpty:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::DefaultIfEmpty:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::DefaultIfEmpty:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::DefaultIfEmpty:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::DefaultIfEmpty:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::DefaultIfEmpty:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::DefaultIfEmpty\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"definitionSubjectId\":\"System.Linq/Enumerable::DefaultIfEmpty:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"displaySubjectId\":\"System.Linq/Enumerable::DefaultIfEmpty\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::DefaultIfEmpty:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":27,\"resultType\":\"System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"displaySubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":32,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":33,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":34,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":39,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":40,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"GenerationConcatSubjects\"}";
static const char kMethodJson_3[] = "{\"methodId\":\"generation-concat-subjects.generation-concat-subjects.custom-entry-subject-3\",\"subjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_3:System.Void()\",\"signature\":\"System.Void GenerationConcatSubjects::CustomEntrySubject_3()\",\"identity\":{\"assemblyName\":\"GenerationConcatSubjects\",\"declaringTypeSubjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects\",\"definitionSubjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_3:System.Void()\",\"subjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_3:System.Void()\",\"methodId\":\"generation-concat-subjects.generation-concat-subjects.custom-entry-subject-3\",\"signature\":\"System.Void GenerationConcatSubjects::CustomEntrySubject_3()\"},\"nativeSymbol\":\"GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_3\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":3,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":3,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"GenerationConcatSubjects/\\u003CPrivateImplementationDetails\\u003E::4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D\",\"ilOffset\":8,\"resultType\":\"System.RuntimeFieldHandle\",\"reference\":{\"assemblyName\":\"GenerationConcatSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"GenerationConcatSubjects/\\u003CPrivateImplementationDetails\\u003E::4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"GenerationConcatSubjects\",\"subjectId\":\"GenerationConcatSubjects/\\u003CPrivateImplementationDetails\\u003E::4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"GenerationConcatSubjects/\\u003CPrivateImplementationDetails\\u003E\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"\\u003CPrivateImplementationDetails\\u003E\\u002B__StaticArrayInitTypeSize=12\"},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":22,\"resultType\":\"System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Linq/Enumerable::DefaultIfEmpty\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::DefaultIfEmpty\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::DefaultIfEmpty\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::DefaultIfEmpty:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::DefaultIfEmpty:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::DefaultIfEmpty:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::DefaultIfEmpty:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::DefaultIfEmpty:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::DefaultIfEmpty:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::DefaultIfEmpty\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"definitionSubjectId\":\"System.Linq/Enumerable::DefaultIfEmpty:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0)\",\"displaySubjectId\":\"System.Linq/Enumerable::DefaultIfEmpty\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::DefaultIfEmpty:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":29,\"resultType\":\"System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"displaySubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":34,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":35,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":36,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":41,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":42,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"GenerationConcatSubjects\"}";
static const char kMethodJson_4[] = "{\"methodId\":\"generation-concat-subjects.generation-concat-subjects.custom-entry-subject-4\",\"subjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_4:System.Void()\",\"signature\":\"System.Void GenerationConcatSubjects::CustomEntrySubject_4()\",\"identity\":{\"assemblyName\":\"GenerationConcatSubjects\",\"declaringTypeSubjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects\",\"definitionSubjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_4:System.Void()\",\"subjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_4:System.Void()\",\"methodId\":\"generation-concat-subjects.generation-concat-subjects.custom-entry-subject-4\",\"signature\":\"System.Void GenerationConcatSubjects::CustomEntrySubject_4()\"},\"nativeSymbol\":\"GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_4\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Linq/Enumerable::Empty\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E()\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::Empty\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::Empty\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E()\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::Empty:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::Empty:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E();type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::Empty:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E();type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::Empty:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E()\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::Empty:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E();type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::Empty:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E();type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::Empty\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E()\",\"definitionSubjectId\":\"System.Linq/Enumerable::Empty:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E()\",\"displaySubjectId\":\"System.Linq/Enumerable::Empty\\u003CSystem.Int32\\u003E()\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::Empty:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E()\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"displaySubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":15,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":20,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":21,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"GenerationConcatSubjects\"}";
static const char kMethodJson_5[] = "{\"methodId\":\"generation-concat-subjects.generation-concat-subjects.custom-entry-subject-5\",\"subjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_5:System.Void()\",\"signature\":\"System.Void GenerationConcatSubjects::CustomEntrySubject_5()\",\"identity\":{\"assemblyName\":\"GenerationConcatSubjects\",\"declaringTypeSubjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects\",\"definitionSubjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_5:System.Void()\",\"subjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_5:System.Void()\",\"methodId\":\"generation-concat-subjects.generation-concat-subjects.custom-entry-subject-5\",\"signature\":\"System.Void GenerationConcatSubjects::CustomEntrySubject_5()\"},\"nativeSymbol\":\"GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_5\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":100,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":4,\"resultType\":\"System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Linq/Enumerable::Range:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::Range:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::Range:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Int32,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":9,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":10,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":11,\"resultType\":\"System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"displaySubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":17,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":18,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":23,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"GenerationConcatSubjects\"}";
static const char kMethodJson_6[] = "{\"methodId\":\"generation-concat-subjects.generation-concat-subjects.custom-entry-subject-6\",\"subjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_6:System.Void()\",\"signature\":\"System.Void GenerationConcatSubjects::CustomEntrySubject_6()\",\"identity\":{\"assemblyName\":\"GenerationConcatSubjects\",\"declaringTypeSubjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects\",\"definitionSubjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_6:System.Void()\",\"subjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_6:System.Void()\",\"methodId\":\"generation-concat-subjects.generation-concat-subjects.custom-entry-subject-6\",\"signature\":\"System.Void GenerationConcatSubjects::CustomEntrySubject_6()\"},\"nativeSymbol\":\"GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_6\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":3,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":3,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"GenerationConcatSubjects/\\u003CPrivateImplementationDetails\\u003E::A10494D90314704E24CA5786F6376A6097558F10BB7880B539C8B80312DCA080\",\"ilOffset\":8,\"resultType\":\"System.RuntimeFieldHandle\",\"reference\":{\"assemblyName\":\"GenerationConcatSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"GenerationConcatSubjects/\\u003CPrivateImplementationDetails\\u003E::A10494D90314704E24CA5786F6376A6097558F10BB7880B539C8B80312DCA080\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"GenerationConcatSubjects\",\"subjectId\":\"GenerationConcatSubjects/\\u003CPrivateImplementationDetails\\u003E::A10494D90314704E24CA5786F6376A6097558F10BB7880B539C8B80312DCA080\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"GenerationConcatSubjects/\\u003CPrivateImplementationDetails\\u003E\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"\\u003CPrivateImplementationDetails\\u003E\\u002B__StaticArrayInitTypeSize=12\"},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":21,\"resultType\":\"System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Linq/Enumerable::Prepend\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::Prepend\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::Prepend\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::Prepend:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::Prepend:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::Prepend:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::Prepend:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::Prepend:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::Prepend:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::Prepend\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"definitionSubjectId\":\"System.Linq/Enumerable::Prepend:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0)\",\"displaySubjectId\":\"System.Linq/Enumerable::Prepend\\u003CSystem.Int32\\u003E(System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E,System.Int32)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::Prepend:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(System.Collections.Generic.IEnumerable\\u003C!!0\\u003E,!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":26,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":27,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":28,\"resultType\":\"System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"displaySubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":33,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":34,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":35,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":40,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":41,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"GenerationConcatSubjects\"}";
static const char kMethodJson_7[] = "{\"methodId\":\"generation-concat-subjects.generation-concat-subjects.custom-entry-subject-7\",\"subjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_7:System.Void()\",\"signature\":\"System.Void GenerationConcatSubjects::CustomEntrySubject_7()\",\"identity\":{\"assemblyName\":\"GenerationConcatSubjects\",\"declaringTypeSubjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects\",\"definitionSubjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_7:System.Void()\",\"subjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_7:System.Void()\",\"methodId\":\"generation-concat-subjects.generation-concat-subjects.custom-entry-subject-7\",\"signature\":\"System.Void GenerationConcatSubjects::CustomEntrySubject_7()\"},\"nativeSymbol\":\"GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_7\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":5,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":3,\"resultType\":\"System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Linq/Enumerable::Range:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::Range:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::Range:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Int32,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":10,\"resultType\":\"System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"displaySubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":16,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":17,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":22,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"GenerationConcatSubjects\"}";
static const char kMethodJson_8[] = "{\"methodId\":\"generation-concat-subjects.generation-concat-subjects.custom-entry-subject-8\",\"subjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_8:System.Void()\",\"signature\":\"System.Void GenerationConcatSubjects::CustomEntrySubject_8()\",\"identity\":{\"assemblyName\":\"GenerationConcatSubjects\",\"declaringTypeSubjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects\",\"definitionSubjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_8:System.Void()\",\"subjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_8:System.Void()\",\"methodId\":\"generation-concat-subjects.generation-concat-subjects.custom-entry-subject-8\",\"signature\":\"System.Void GenerationConcatSubjects::CustomEntrySubject_8()\"},\"nativeSymbol\":\"GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_8\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":3,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":4,\"resultType\":\"System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Linq/Enumerable::Repeat\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Linq\",\"subjectKind\":\"method\",\"subjectId\":\"System.Linq/Enumerable::Repeat\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Linq\",\"subjectId\":\"System.Linq/Enumerable::Repeat\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Int32,System.Int32)\",\"openDefinitionSubjectId\":\"System.Linq/Enumerable::Repeat:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(!!0,System.Int32)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::Repeat:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(!!0,System.Int32);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::Repeat:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(!!0,System.Int32);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::Repeat:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(!!0,System.Int32)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Linq/Enumerable::Repeat:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(!!0,System.Int32);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Linq/Enumerable::Repeat:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(!!0,System.Int32);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Linq/Enumerable::Repeat\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E(System.Int32,System.Int32)\",\"definitionSubjectId\":\"System.Linq/Enumerable::Repeat:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(!!0,System.Int32)\",\"displaySubjectId\":\"System.Linq/Enumerable::Repeat\\u003CSystem.Int32\\u003E(System.Int32,System.Int32)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Linq/Enumerable::Repeat:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E(!!0,System.Int32)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":9,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":10,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":11,\"resultType\":\"System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"displaySubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":17,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":18,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":23,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":24,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"GenerationConcatSubjects\"}";
static const char kMethodJson_9[] = "{\"methodId\":\"generation-concat-subjects.generation-concat-subjects.custom-entry-subject-9\",\"subjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_9:System.Void()\",\"signature\":\"System.Void GenerationConcatSubjects::CustomEntrySubject_9()\",\"identity\":{\"assemblyName\":\"GenerationConcatSubjects\",\"declaringTypeSubjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects\",\"definitionSubjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_9:System.Void()\",\"subjectId\":\"GenerationConcatSubjects/GenerationConcatSubjects::CustomEntrySubject_9:System.Void()\",\"methodId\":\"generation-concat-subjects.generation-concat-subjects.custom-entry-subject-9\",\"signature\":\"System.Void GenerationConcatSubjects::CustomEntrySubject_9()\"},\"nativeSymbol\":\"GenerationConcatSubjects_GenerationConcatSubjects_CustomEntrySubject_9\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":3,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Int32\",\"ilOffset\":2,\"resultType\":\"System.Int32[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Int32\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldtoken\",\"opCode\":70,\"operand\":\"GenerationConcatSubjects/\\u003CPrivateImplementationDetails\\u003E::4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D\",\"ilOffset\":8,\"resultType\":\"System.RuntimeFieldHandle\",\"reference\":{\"assemblyName\":\"GenerationConcatSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"GenerationConcatSubjects/\\u003CPrivateImplementationDetails\\u003E::4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"GenerationConcatSubjects\",\"subjectId\":\"GenerationConcatSubjects/\\u003CPrivateImplementationDetails\\u003E::4636993D3E1DA4E9D6B8F87B79E8F7C6D018580D52661950EABC3845C5897A4D\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"GenerationConcatSubjects/\\u003CPrivateImplementationDetails\\u003E\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"\\u003CPrivateImplementationDetails\\u003E\\u002B__StaticArrayInitTypeSize=12\"},\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":20,\"resultType\":\"System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E();type=[System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Int32\\u003E()\",\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"displaySubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E::GetEnumerator()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Collections.Generic.IEnumerable\\u00601::GetEnumerator:System.Collections.Generic.IEnumerator\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":27,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":33,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"GenerationConcatSubjects\"}";

extern "C" const JitMethodEntry kChaosJitMethodEntries[10] =
{
    { kMethodJson_0, 11715u, 0x00000007u, 0u },
    { kMethodJson_1, 12796u, 0x00000008u, 0u },
    { kMethodJson_2, 11619u, 0x00000009u, 0u },
    { kMethodJson_3, 11820u, 0x0000000Au, 0u },
    { kMethodJson_4, 8051u, 0x0000000Bu, 0u },
    { kMethodJson_5, 6621u, 0x0000000Cu, 0u },
    { kMethodJson_6, 11728u, 0x0000000Du, 0u },
    { kMethodJson_7, 6618u, 0x0000000Eu, 0u },
    { kMethodJson_8, 8524u, 0x0000000Fu, 0u },
    { kMethodJson_9, 8146u, 0x00000010u, 0u }
};

extern "C" const uint32_t kChaosJitMethodEntryCount = 10u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitMethods(kChaosJitMethodEntries, kChaosJitMethodEntryCount);
}