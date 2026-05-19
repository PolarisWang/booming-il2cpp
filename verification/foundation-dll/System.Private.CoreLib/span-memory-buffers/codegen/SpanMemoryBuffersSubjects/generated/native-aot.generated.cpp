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

namespace chaos::il2cpp::codegen::SpanMemoryBuffersSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects = {{nullptr, nullptr, 15749179977097503096ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(15749179977097503096ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Byte = {{nullptr, nullptr, 3217969471912086765ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Byte = static_cast<CHAOS_IL2CPP_INTPTR>(3217969471912086765ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Memory_System_Byte_ = {{nullptr, nullptr, 9124771167285068127ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Memory_System_Byte_ = static_cast<CHAOS_IL2CPP_INTPTR>(9124771167285068127ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_NotSupportedException = {{nullptr, nullptr, 9608462597425565551ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_NotSupportedException = static_cast<CHAOS_IL2CPP_INTPTR>(9608462597425565551ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_NullReferenceException = {{nullptr, nullptr, 5314911908653589872ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_NullReferenceException = static_cast<CHAOS_IL2CPP_INTPTR>(5314911908653589872ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_ReadOnlySpan_System_Byte_ = {{nullptr, nullptr, 1576841396539395100ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_ReadOnlySpan_System_Byte_ = static_cast<CHAOS_IL2CPP_INTPTR>(1576841396539395100ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {{nullptr, nullptr, 5474029880995115448ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly = static_cast<CHAOS_IL2CPP_INTPTR>(5474029880995115448ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName = {{nullptr, nullptr, 17082367815459723707ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = static_cast<CHAOS_IL2CPP_INTPTR>(17082367815459723707ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo = {{nullptr, nullptr, 4137207361503509124ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo = static_cast<CHAOS_IL2CPP_INTPTR>(4137207361503509124ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo = {{nullptr, nullptr, 17040031516751226236ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo = static_cast<CHAOS_IL2CPP_INTPTR>(17040031516751226236ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo = {{nullptr, nullptr, 10748947813473285525ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = static_cast<CHAOS_IL2CPP_INTPTR>(10748947813473285525ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Span_System_Byte_ = {{nullptr, nullptr, 16366464644310993382ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Span_System_Byte_ = static_cast<CHAOS_IL2CPP_INTPTR>(16366464644310993382ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_String = {{nullptr, nullptr, 1782325859292956794ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
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

struct chaos_type_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Byte
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Memory_System_Byte_
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_NotSupportedException
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR _message = 0;
	CHAOS_IL2CPP_INTPTR _innerException = 0;
	CHAOS_IL2CPP_INTPTR _stackTrace = 0;
	CHAOS_IL2CPP_INT32 _HResult = 0;
};

struct chaos_type_System_Private_CoreLib_System_NullReferenceException
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR _message = 0;
	CHAOS_IL2CPP_INTPTR _innerException = 0;
	CHAOS_IL2CPP_INTPTR _stackTrace = 0;
	CHAOS_IL2CPP_INT32 _HResult = 0;
};

struct chaos_type_System_Private_CoreLib_System_ReadOnlySpan_System_Byte_
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

struct chaos_type_System_Private_CoreLib_System_Span_System_Byte_
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_String
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR length = 0;
	const char* utf8_data = nullptr;
	CHAOS_IL2CPP_UINT64 string_id = 0u;  // stable StringId, 0 = not yet materialized
};

CHAOS_IL2CPP_INT32 chaos_static_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects___exitCode = 0;

bool chaos_try_get_static_field_data(
	CHAOS_IL2CPP_INTPTR chaos_field_handle,
	const CHAOS_IL2CPP_UINT8*& chaos_data,
	CHAOS_IL2CPP_SIZE& chaos_size)
{
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

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Memory_System_Runtime_InteropServices_MemoryMarshal__GetReference_System_Byte__System_Byte__System_ReadOnlySpan_System_Byte__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	(void)chaos_arg_0;
	static CHAOS_IL2CPP_UINT8 s_sentinel = 0;
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_sentinel);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Memory_System_Runtime_InteropServices_MemoryMarshal__GetReference_System_Byte__System_Byte__System_Span_System_Byte__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	(void)chaos_arg_0;
	static CHAOS_IL2CPP_UINT8 s_sentinel = 0;
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_sentinel);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Memory_System_Byte___ToArray_System_Byte____(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	(void)chaos_arg_0;
	static CHAOS_IL2CPP_UINT8 s_sentinel = 0;
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_sentinel);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Memory_System_Byte___get_Span_System_Span_System_Byte___(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	(void)chaos_arg_0;
	static CHAOS_IL2CPP_UINT8 s_sentinel = 0;
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_sentinel);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_ReadOnlySpan_System_Byte___ToArray_System_Byte____()
{
	static CHAOS_IL2CPP_UINT8 s_sentinel = 0;
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_sentinel);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Span_System_Byte___CopyTo_System_Void_System_Span_System_Byte__(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)
{
	(void)chaos_arg_0; (void)chaos_arg_1;
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Span_System_Byte___get_Empty_System_Span_System_Byte___()
{
	static CHAOS_IL2CPP_UINT8 s_sentinel = 0;
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_sentinel);
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_0(void);
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_1(void);
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_2(void);
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_3(void);
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_4(void);
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_5(void);
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_6(void);
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_7(void);
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_8(void);
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_9(void);
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_10(void);
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_11(void);
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_12(void);
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_13(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[15] = {
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
	::ChaosAbiMethodEntryV0 entries[14];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		14u,
		0u,
		4123785717u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_0
		{ 0u, 0u },  // SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_1
		{ 0u, 0u },  // SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_2
		{ 0u, 0u },  // SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_3
		{ 0u, 0u },  // SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_4
		{ 0u, 0u },  // SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_5
		{ 0u, 0u },  // SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_6
		{ 0u, 0u },  // SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_7
		{ 0u, 0u },  // SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_8
		{ 0u, 0u },  // SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_9
		{ 0u, 0u },  // SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_10
		{ 0u, 0u },  // SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_11
		{ 0u, 0u },  // SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_12
		{ 0u, 0u },  // SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_13
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
		"SpanMemoryBuffersSubjects",
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
		&chaos_mt_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects.hot,
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
		/* .name_utf8         = */ "SpanMemoryBuffersSubjects",
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
		::chaos::il2cpp::runtime_core::RegisterModule("SpanMemoryBuffersSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[14] = {
	{ "Subject_0", 0x00000003u, 0u },  // SpanMemoryBuffersSubjects
	{ "Subject_1", 0x00000004u, 0u },  // SpanMemoryBuffersSubjects
	{ "Subject_2", 0x00000005u, 0u },  // SpanMemoryBuffersSubjects
	{ "Subject_3", 0x00000006u, 0u },  // SpanMemoryBuffersSubjects
	{ "Subject_4", 0x00000007u, 0u },  // SpanMemoryBuffersSubjects
	{ "Subject_5", 0x00000008u, 0u },  // SpanMemoryBuffersSubjects
	{ "Subject_6", 0x00000009u, 0u },  // SpanMemoryBuffersSubjects
	{ "Subject_7", 0x0000000Au, 0u },  // SpanMemoryBuffersSubjects
	{ "Subject_8", 0x0000000Bu, 0u },  // SpanMemoryBuffersSubjects
	{ "Subject_9", 0x0000000Cu, 0u },  // SpanMemoryBuffersSubjects
	{ "Subject_10", 0x0000000Du, 0u },  // SpanMemoryBuffersSubjects
	{ "Subject_11", 0x0000000Eu, 0u },  // SpanMemoryBuffersSubjects
	{ "Subject_12", 0x0000000Fu, 0u },  // SpanMemoryBuffersSubjects
	{ "Subject_13", 0x00000010u, 0u },  // SpanMemoryBuffersSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "SpanMemoryBuffersSubjects", "", 0u, 14u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[14] = {
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
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[14] = {
	{ reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SpanMemoryBuffersSubjects::Subject_0
	{ reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SpanMemoryBuffersSubjects::Subject_1
	{ reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SpanMemoryBuffersSubjects::Subject_2
	{ reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SpanMemoryBuffersSubjects::Subject_3
	{ reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SpanMemoryBuffersSubjects::Subject_4
	{ reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SpanMemoryBuffersSubjects::Subject_5
	{ reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SpanMemoryBuffersSubjects::Subject_6
	{ reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SpanMemoryBuffersSubjects::Subject_7
	{ reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SpanMemoryBuffersSubjects::Subject_8
	{ reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SpanMemoryBuffersSubjects::Subject_9
	{ reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SpanMemoryBuffersSubjects::Subject_10
	{ reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SpanMemoryBuffersSubjects::Subject_11
	{ reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SpanMemoryBuffersSubjects::Subject_12
	{ reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // SpanMemoryBuffersSubjects::Subject_13
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"SpanMemoryBuffersSubjects",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	14u,
	s_hotpatch_slots,
	14u,
	s_hotpatch_entries,
	14u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[27] = {
	"System.Private.CoreLib/System.Byte",
	"System.Private.CoreLib/System.Span<System.Byte>::.ctor:System.Void(System.Byte[])",
	"System.Private.CoreLib/System.Span<System.Byte>::get_Item:System.Byte&(System.Int32)",
	"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode",
	"System.Private.CoreLib/System.Span<System.Byte>::Slice:System.Span<System.Byte>(System.Int32)",
	"System.Private.CoreLib/System.Span<System.Byte>::get_Length:System.Int32()",
	"System.Private.CoreLib/System.Int32::GetHashCode:System.Int32()",
	"System.Private.CoreLib/System.Span<System.Byte>::Slice:System.Span<System.Byte>(System.Int32,System.Int32)",
	"System.Memory/System.MemoryExtensions::AsSpan<System.Byte>:System.Span<System.Byte>(System.Byte[])",
	"System.Private.CoreLib/System.Span<System.Byte>::ToArray:System.Byte[]()",
	"System.Private.CoreLib/System.Span<System.Byte>",
	"System.Private.CoreLib/System.Span<System.Byte>::CopyTo:System.Void(System.Span<System.Byte>)",
	"System.Private.CoreLib/System.Span<System.Byte>::get_Empty:System.Span<System.Byte>()",
	"System.Private.CoreLib/System.ReadOnlySpan<System.Byte>::.ctor:System.Void(System.Byte[])",
	"System.Private.CoreLib/System.ReadOnlySpan<System.Byte>::get_Item:System.Byte&(System.Int32)",
	"System.Private.CoreLib/System.ReadOnlySpan<System.Byte>::Slice:System.ReadOnlySpan<System.Byte>(System.Int32)",
	"System.Private.CoreLib/System.ReadOnlySpan<System.Byte>::get_Length:System.Int32()",
	"System.Private.CoreLib/System.ReadOnlySpan<System.Byte>",
	"System.Private.CoreLib/System.ReadOnlySpan<System.Byte>::ToArray:System.Byte[]()",
	"System.Private.CoreLib/System.Memory<System.Byte>::.ctor:System.Void(System.Byte[])",
	"System.Private.CoreLib/System.Memory<System.Byte>::Slice:System.Memory<System.Byte>(System.Int32)",
	"System.Private.CoreLib/System.Memory<System.Byte>::get_Length:System.Int32()",
	"System.Private.CoreLib/System.Memory<System.Byte>",
	"System.Private.CoreLib/System.Memory<System.Byte>::get_Span:System.Span<System.Byte>()",
	"System.Private.CoreLib/System.Memory<System.Byte>::ToArray:System.Byte[]()",
	"System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference<System.Byte>:System.Byte&(System.ReadOnlySpan<System.Byte>)",
	"System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference<System.Byte>:System.Byte&(System.Span<System.Byte>)",
};

extern "C" void* kChaosExternalRuntimeFnTable[27] = {
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
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Span_System_Byte___CopyTo_System_Void_System_Span_System_Byte__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Span_System_Byte___get_Empty_System_Span_System_Byte___),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_ReadOnlySpan_System_Byte___ToArray_System_Byte____),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Memory_System_Byte___get_Span_System_Span_System_Byte___),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Memory_System_Byte___ToArray_System_Byte____),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Memory_System_Runtime_InteropServices_MemoryMarshal__GetReference_System_Byte__System_Byte__System_ReadOnlySpan_System_Byte__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Memory_System_Runtime_InteropServices_MemoryMarshal__GetReference_System_Byte__System_Byte__System_Span_System_Byte__),
};

extern "C" int32_t kChaosExternalRuntimeCount = 27;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[14])() = {
	reinterpret_cast<void(*)()>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_6),
	reinterpret_cast<void(*)()>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_7),
	reinterpret_cast<void(*)()>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_8),
	reinterpret_cast<void(*)()>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_9),
	reinterpret_cast<void(*)()>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_10),
	reinterpret_cast<void(*)()>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_11),
	reinterpret_cast<void(*)()>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_12),
	reinterpret_cast<void(*)()>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_13),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
static void (*kBenchmarkWrappers[14])() = {
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
static void* const kMethodPointers[14] = {
	reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_0),
	reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_1),
	reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_2),
	reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_3),
	reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_4),
	reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_5),
	reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_6),
	reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_7),
	reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_8),
	reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_9),
	reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_10),
	reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_11),
	reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_12),
	reinterpret_cast<void*>(&SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_13),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 14u,
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
	.image_name_utf8    = "SpanMemoryBuffersSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects[1] = {
	{ 0u, "SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects[14] = {
	{ 0u, "SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_6:System.Void()", "Subject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_7:System.Void()", "Subject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_8:System.Void()", "Subject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_9:System.Void()", "Subject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, "SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_10:System.Void()", "Subject_10", "System.Void", 0, nullptr, 0u },
	{ 0u, "SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_11:System.Void()", "Subject_11", "System.Void", 0, nullptr, 0u },
	{ 0u, "SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_12:System.Void()", "Subject_12", "System.Void", 0, nullptr, 0u },
	{ 0u, "SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_13:System.Void()", "Subject_13", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0u, "SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects", "SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects", "", "SpanMemoryBuffersSubjects", "SpanMemoryBuffersSubjects", nullptr, kReflFields_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects, 1u, nullptr, 0u,
	kReflMethods_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects, 14u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "SpanMemoryBuffersSubjects", kReflTypePtrs, 1u };

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
// Managed method: SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_0()
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_0(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
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
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Span_System_Byte_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Span_System_Byte_.hot;
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[1] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_0 = _s2;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[2])(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_address = _s2;
		const auto chaos_value = chaos_load_indirect<CHAOS_IL2CPP_UINT8>(chaos_address);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value);
	}
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s2) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s3) ? 1 : 0);
	{
		if (_s2 != 0)
		{
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s2;
				chaos_static_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_1()
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_1(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
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
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Span_System_Byte_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Span_System_Byte_.hot;
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[1] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_arg_0 = _s2;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[4])(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[2] = _s2;
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[2]);
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[5])();
		_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[3] = _s3;
	_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[3]);
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[6])();
		_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
	{
		if (_s4 != 0)
		{
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s4;
				chaos_static_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_2()
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_2(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
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
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Span_System_Byte_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Span_System_Byte_.hot;
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[1] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	{
		const auto chaos_arg_1 = _s3;
		const auto chaos_arg_0 = _s2;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[7])(chaos_arg_0, chaos_arg_1);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[2] = _s2;
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[2]);
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[5])();
		_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[3] = _s3;
	_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[3]);
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[6])();
		_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
	{
		if (_s4 != 0)
		{
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s4;
				chaos_static_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_3()
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_3(void)
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[8])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s0;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[9])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
	{
		if (_s1 != 0)
		{
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_4()
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	_s1 = _s0;
	{
		const auto chaos_address = _s1;
		if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
		{
			auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
			*chaos_slot = 0;
		}
		else
		{
			*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
		}
	}
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		const auto chaos_address = _s1;
		if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
		{
			auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
			*chaos_slot = 0;
		}
		else
		{
			*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
		}
	}
	_s1 = chaos_locals[1];
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Private_CoreLib_System_Span_System_Byte___CopyTo_System_Void_System_Span_System_Byte__(chaos_arg_0, chaos_arg_1);
	}
	return;
}

// Managed method: SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_5()
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
			{
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Span_System_Byte___get_Empty_System_Span_System_Byte___();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects___exitCode = chaos_value;
			}
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NotSupportedException.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
			{
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Span_System_Byte___get_Empty_System_Span_System_Byte___();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects___exitCode = chaos_value;
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NotSupportedException.hot))
			{
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
			{
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Span_System_Byte___get_Empty_System_Span_System_Byte___();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects___exitCode = chaos_value;
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NotSupportedException.hot))
			{
				chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_6()
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_6(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
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
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_ReadOnlySpan_System_Byte_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_ReadOnlySpan_System_Byte_.hot;
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[1] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_0 = _s2;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[14])(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_address = _s2;
		const auto chaos_value = chaos_load_indirect<CHAOS_IL2CPP_UINT8>(chaos_address);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value);
	}
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s2) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s3) ? 1 : 0);
	{
		if (_s2 != 0)
		{
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s2;
				chaos_static_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_7()
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_7(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
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
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_ReadOnlySpan_System_Byte_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_ReadOnlySpan_System_Byte_.hot;
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[1] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_arg_0 = _s2;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[15])(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[2] = _s2;
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[2]);
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[16])();
		_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[3] = _s3;
	_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[3]);
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[6])();
		_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
	{
		if (_s4 != 0)
		{
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s4;
				chaos_static_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_8()
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_8(void)
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


	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	_s1 = _s0;
	{
		const auto chaos_address = _s1;
		if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
		{
			auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
			*chaos_slot = 0;
		}
		else
		{
			*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
		}
	}
	{
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_ReadOnlySpan_System_Byte___ToArray_System_Byte____();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s2) ? 1 : 0);
	{
		if (_s1 != 0)
		{
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_9()
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_9(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
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
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Memory_System_Byte_, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Memory_System_Byte_.hot;
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[1] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_arg_0 = _s2;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[20])(chaos_arg_0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s2;
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[21])();
		_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s3) == static_cast<CHAOS_IL2CPP_INTPTR>(_s4) ? 1 : 0);
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s3) == static_cast<CHAOS_IL2CPP_INTPTR>(_s4) ? 1 : 0);
	{
		if (_s3 != 0)
		{
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_10()
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_10(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			_s1 = _s0;
			{
				const auto chaos_address = _s1;
				if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
				{
					auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
					*chaos_slot = 0;
				}
				else
				{
					*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
				}
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Memory_System_Byte___get_Span_System_Span_System_Byte___(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects___exitCode = chaos_value;
			}
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NotSupportedException.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			_s1 = _s0;
			{
				const auto chaos_address = _s1;
				if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
				{
					auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
					*chaos_slot = 0;
				}
				else
				{
					*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
				}
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Memory_System_Byte___get_Span_System_Span_System_Byte___(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects___exitCode = chaos_value;
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NotSupportedException.hot))
			{
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			_s1 = _s0;
			{
				const auto chaos_address = _s1;
				if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
				{
					auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
					*chaos_slot = 0;
				}
				else
				{
					*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
				}
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Memory_System_Byte___get_Span_System_Span_System_Byte___(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects___exitCode = chaos_value;
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NotSupportedException.hot))
			{
				chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_11()
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_11(void)
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


	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	_s1 = _s0;
	{
		const auto chaos_address = _s1;
		if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
		{
			auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
			*chaos_slot = 0;
		}
		else
		{
			*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
		}
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Memory_System_Byte___ToArray_System_Byte____(chaos_arg_0);
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
				chaos_static_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_12()
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_12(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s0;
				if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
				{
					auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
					*chaos_slot = 0;
				}
				else
				{
					*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
				}
			}
			_s0 = chaos_locals[0];
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Memory_System_Runtime_InteropServices_MemoryMarshal__GetReference_System_Byte__System_Byte__System_ReadOnlySpan_System_Byte__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects___exitCode = chaos_value;
			}
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s0;
				if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
				{
					auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
					*chaos_slot = 0;
				}
				else
				{
					*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
				}
			}
			_s0 = chaos_locals[0];
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Memory_System_Runtime_InteropServices_MemoryMarshal__GetReference_System_Byte__System_Byte__System_ReadOnlySpan_System_Byte__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects___exitCode = chaos_value;
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s0;
				if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
				{
					auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
					*chaos_slot = 0;
				}
				else
				{
					*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
				}
			}
			_s0 = chaos_locals[0];
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Memory_System_Runtime_InteropServices_MemoryMarshal__GetReference_System_Byte__System_Byte__System_ReadOnlySpan_System_Byte__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects___exitCode = chaos_value;
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}

// Managed method: SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_13()
extern "C" void SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_13(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s0;
				if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
				{
					auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
					*chaos_slot = 0;
				}
				else
				{
					*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
				}
			}
			_s0 = chaos_locals[0];
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Memory_System_Runtime_InteropServices_MemoryMarshal__GetReference_System_Byte__System_Byte__System_Span_System_Byte__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects___exitCode = chaos_value;
			}
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s0;
				if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
				{
					auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
					*chaos_slot = 0;
				}
				else
				{
					*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
				}
			}
			_s0 = chaos_locals[0];
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Memory_System_Runtime_InteropServices_MemoryMarshal__GetReference_System_Byte__System_Byte__System_Span_System_Byte__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects___exitCode = chaos_value;
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			{
				const auto chaos_address = _s0;
				if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
				{
					auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
					*chaos_slot = 0;
				}
				else
				{
					*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
				}
			}
			_s0 = chaos_locals[0];
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Memory_System_Runtime_InteropServices_MemoryMarshal__GetReference_System_Byte__System_Byte__System_Span_System_Byte__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects___exitCode = chaos_value;
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
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
	return;
}



}  // namespace chaos::il2cpp::codegen::SpanMemoryBuffersSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 14;

// ── JIT Method Entry Table ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kMethodJson_0[] = "{\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-0\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_0:System.Void()\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_0()\",\"identity\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"definitionSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_0:System.Void()\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_0:System.Void()\",\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-0\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_0()\"},\"nativeSymbol\":\"SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":4,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":2,\"resultType\":\"System.Byte[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":7,\"resultType\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\",\"callee\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::.ctor:System.Void(System.Byte[])\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::.ctor:System.Void(System.Byte[])\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":12,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":16,\"resultType\":\"System.Byte\\u0026\",\"callee\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::get_Item:System.Byte\\u0026(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::get_Item:System.Byte\\u0026(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::get_Item:System.Byte\\u0026(System.Int32)\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::get_Item:!0\\u0026(System.Int32)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Span\\u00601::get_Item:!0\\u0026(System.Int32);type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Span\\u00601::get_Item:!0\\u0026(System.Int32);type=[System.Byte];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::get_Item:!0\\u0026(System.Int32)\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Span\\u00601::get_Item:!0\\u0026(System.Int32);type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Span\\u00601::get_Item:!0\\u0026(System.Int32);type=[System.Byte];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::get_Item:System.Byte\\u0026(System.Int32)\",\"definitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::get_Item:!0\\u0026(System.Int32)\",\"displaySubjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::get_Item(System.Int32)\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::get_Item:!0\\u0026(System.Int32)\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldind.u1\",\"ilOffset\":21,\"resultType\":\"System.Byte\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":35,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\",\"ilOffset\":30,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":35,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SpanMemoryBuffersSubjects\"}";
static const char kMethodJson_1[] = "{\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-1\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_1:System.Void()\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_1()\",\"identity\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"definitionSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_1:System.Void()\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_1:System.Void()\",\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-1\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_1()\"},\"nativeSymbol\":\"SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":4,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":4,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":2,\"resultType\":\"System.Byte[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":7,\"resultType\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\",\"callee\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::.ctor:System.Void(System.Byte[])\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::.ctor:System.Void(System.Byte[])\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":12,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":16,\"resultType\":\"System.Span\\u003CSystem.Byte\\u003E\",\"callee\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::Slice:System.Span\\u003CSystem.Byte\\u003E(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::Slice:System.Span\\u003CSystem.Byte\\u003E(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::Slice:System.Span\\u003CSystem.Byte\\u003E(System.Int32)\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::Slice:System.Span\\u003C!0\\u003E(System.Int32)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Span\\u00601::Slice:System.Span\\u003C!0\\u003E(System.Int32);type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Span\\u00601::Slice:System.Span\\u003C!0\\u003E(System.Int32);type=[System.Byte];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::Slice:System.Span\\u003C!0\\u003E(System.Int32)\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Span\\u00601::Slice:System.Span\\u003C!0\\u003E(System.Int32);type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Span\\u00601::Slice:System.Span\\u003C!0\\u003E(System.Int32);type=[System.Byte];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::Slice:System.Span\\u003CSystem.Byte\\u003E(System.Int32)\",\"definitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::Slice:System.Span\\u003C!0\\u003E(System.Int32)\",\"displaySubjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::Slice(System.Int32)\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::Slice:System.Span\\u003C!0\\u003E(System.Int32)\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":2,\"ilOffset\":22,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::get_Length:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::get_Length:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::get_Length:System.Int32()\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::get_Length:System.Int32()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Span\\u00601::get_Length:System.Int32();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Span\\u00601::get_Length:System.Int32();type=[System.Byte];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::get_Length:System.Int32()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Span\\u00601::get_Length:System.Int32();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Span\\u00601::get_Length:System.Int32();type=[System.Byte];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::get_Length:System.Int32()\",\"definitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::get_Length:System.Int32()\",\"displaySubjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::get_Length()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::get_Length:System.Int32()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":3,\"ilOffset\":30,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Int32::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Int32::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":3,\"ilOffset\":37,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":38,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":40,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":41,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":43,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":44,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":53,\"ilOffset\":45,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":47,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\",\"ilOffset\":48,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":53,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SpanMemoryBuffersSubjects\"}";
static const char kMethodJson_2[] = "{\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-2\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_2:System.Void()\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_2()\",\"identity\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"definitionSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_2:System.Void()\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_2:System.Void()\",\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-2\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_2()\"},\"nativeSymbol\":\"SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_2\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":4,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":4,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":2,\"resultType\":\"System.Byte[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":7,\"resultType\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\",\"callee\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::.ctor:System.Void(System.Byte[])\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::.ctor:System.Void(System.Byte[])\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":12,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":2,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":17,\"resultType\":\"System.Span\\u003CSystem.Byte\\u003E\",\"callee\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::Slice:System.Span\\u003CSystem.Byte\\u003E(System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::Slice:System.Span\\u003CSystem.Byte\\u003E(System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::Slice:System.Span\\u003CSystem.Byte\\u003E(System.Int32,System.Int32)\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::Slice:System.Span\\u003C!0\\u003E(System.Int32,System.Int32)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Span\\u00601::Slice:System.Span\\u003C!0\\u003E(System.Int32,System.Int32);type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Span\\u00601::Slice:System.Span\\u003C!0\\u003E(System.Int32,System.Int32);type=[System.Byte];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::Slice:System.Span\\u003C!0\\u003E(System.Int32,System.Int32)\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Span\\u00601::Slice:System.Span\\u003C!0\\u003E(System.Int32,System.Int32);type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Span\\u00601::Slice:System.Span\\u003C!0\\u003E(System.Int32,System.Int32);type=[System.Byte];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::Slice:System.Span\\u003CSystem.Byte\\u003E(System.Int32,System.Int32)\",\"definitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::Slice:System.Span\\u003C!0\\u003E(System.Int32,System.Int32)\",\"displaySubjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::Slice(System.Int32,System.Int32)\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::Slice:System.Span\\u003C!0\\u003E(System.Int32,System.Int32)\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":22,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":2,\"ilOffset\":23,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::get_Length:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::get_Length:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::get_Length:System.Int32()\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::get_Length:System.Int32()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Span\\u00601::get_Length:System.Int32();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Span\\u00601::get_Length:System.Int32();type=[System.Byte];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::get_Length:System.Int32()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Span\\u00601::get_Length:System.Int32();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Span\\u00601::get_Length:System.Int32();type=[System.Byte];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::get_Length:System.Int32()\",\"definitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::get_Length:System.Int32()\",\"displaySubjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::get_Length()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::get_Length:System.Int32()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":30,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":3,\"ilOffset\":31,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Int32::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Int32::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":2,\"ilOffset\":38,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":39,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":41,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":42,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":44,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":45,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":54,\"ilOffset\":46,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":48,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\",\"ilOffset\":49,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":54,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SpanMemoryBuffersSubjects\"}";
static const char kMethodJson_3[] = "{\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-3\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_3:System.Void()\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_3()\",\"identity\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"definitionSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_3:System.Void()\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_3:System.Void()\",\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-3\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_3()\"},\"nativeSymbol\":\"SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_3\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":2,\"resultType\":\"System.Byte[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.Span\\u003CSystem.Byte\\u003E\",\"callee\":\"System.Memory/System.MemoryExtensions::AsSpan\\u003CSystem.Byte\\u003E:System.Span\\u003CSystem.Byte\\u003E(System.Byte[])\",\"reference\":{\"assemblyName\":\"System.Memory\",\"subjectKind\":\"method\",\"subjectId\":\"System.Memory/System.MemoryExtensions::AsSpan\\u003CSystem.Byte\\u003E:System.Span\\u003CSystem.Byte\\u003E(System.Byte[])\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Memory\",\"subjectId\":\"System.Memory/System.MemoryExtensions::AsSpan\\u003CSystem.Byte\\u003E:System.Span\\u003CSystem.Byte\\u003E(System.Byte[])\",\"openDefinitionSubjectId\":\"System.Memory/System.MemoryExtensions::AsSpan:System.Span\\u003C!!0\\u003E(!!0[])\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Memory/System.MemoryExtensions::AsSpan:System.Span\\u003C!!0\\u003E(!!0[]);type=[];method=[System.Byte]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Memory/System.MemoryExtensions::AsSpan:System.Span\\u003C!!0\\u003E(!!0[]);type=[];method=[System.Byte]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Memory/System.MemoryExtensions::AsSpan:System.Span\\u003C!!0\\u003E(!!0[])\",\"typeArguments\":[],\"methodArguments\":[\"System.Byte\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Memory/System.MemoryExtensions::AsSpan:System.Span\\u003C!!0\\u003E(!!0[]);type=[];method=[System.Byte]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Memory/System.MemoryExtensions::AsSpan:System.Span\\u003C!!0\\u003E(!!0[]);type=[];method=[System.Byte]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Memory/System.MemoryExtensions::AsSpan\\u003CSystem.Byte\\u003E:System.Span\\u003CSystem.Byte\\u003E(System.Byte[])\",\"definitionSubjectId\":\"System.Memory/System.MemoryExtensions::AsSpan:System.Span\\u003C!!0\\u003E(!!0[])\",\"displaySubjectId\":\"System.Memory/System.MemoryExtensions::AsSpan\\u003CSystem.Byte\\u003E(System.Byte[])\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Memory/System.MemoryExtensions::AsSpan:System.Span\\u003C!!0\\u003E(!!0[])\",\"typeArguments\":[],\"methodArguments\":[\"System.Byte\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":12,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":15,\"resultType\":\"System.Byte[]\",\"callee\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::ToArray:System.Byte[]()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::ToArray:System.Byte[]()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::ToArray:System.Byte[]()\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::ToArray:!0[]()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Span\\u00601::ToArray:!0[]();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Span\\u00601::ToArray:!0[]();type=[System.Byte];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::ToArray:!0[]()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Span\\u00601::ToArray:!0[]();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Span\\u00601::ToArray:!0[]();type=[System.Byte];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::ToArray:System.Byte[]()\",\"definitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::ToArray:!0[]()\",\"displaySubjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::ToArray()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::ToArray:!0[]()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldlen\",\"opCode\":46,\"ilOffset\":20,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":28,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":29,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":38,\"ilOffset\":30,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\",\"ilOffset\":33,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":38,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SpanMemoryBuffersSubjects\"}";
static const char kMethodJson_4[] = "{\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-4\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_4:System.Void()\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_4()\",\"identity\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"definitionSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_4:System.Void()\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_4:System.Void()\",\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-4\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_4()\"},\"nativeSymbol\":\"SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_4\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":3,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\",\"ilOffset\":4,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":10,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\",\"ilOffset\":12,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":19,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::CopyTo:System.Void(System.Span\\u003CSystem.Byte\\u003E)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::CopyTo:System.Void(System.Span\\u003CSystem.Byte\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::CopyTo:System.Void(System.Span\\u003CSystem.Byte\\u003E)\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::CopyTo:System.Void(System.Span\\u003C!0\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Span\\u00601::CopyTo:System.Void(System.Span\\u003C!0\\u003E);type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Span\\u00601::CopyTo:System.Void(System.Span\\u003C!0\\u003E);type=[System.Byte];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::CopyTo:System.Void(System.Span\\u003C!0\\u003E)\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Span\\u00601::CopyTo:System.Void(System.Span\\u003C!0\\u003E);type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Span\\u00601::CopyTo:System.Void(System.Span\\u003C!0\\u003E);type=[System.Byte];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::CopyTo:System.Void(System.Span\\u003CSystem.Byte\\u003E)\",\"definitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::CopyTo:System.Void(System.Span\\u003C!0\\u003E)\",\"displaySubjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::CopyTo(System.Span\\u003CSystem.Byte\\u003E)\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::CopyTo:System.Void(System.Span\\u003C!0\\u003E)\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":25,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SpanMemoryBuffersSubjects\"}";
static const char kMethodJson_5[] = "{\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-5\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_5:System.Void()\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_5()\",\"identity\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"definitionSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_5:System.Void()\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_5:System.Void()\",\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-5\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_5()\"},\"nativeSymbol\":\"SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_5\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":16,\"handlerOffset\":17,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NotSupportedException\"}],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Span\\u003CSystem.Byte\\u003E\",\"callee\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::get_Empty:System.Span\\u003CSystem.Byte\\u003E()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::get_Empty:System.Span\\u003CSystem.Byte\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::get_Empty:System.Span\\u003CSystem.Byte\\u003E()\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::get_Empty:System.Span\\u003C!0\\u003E()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Span\\u00601::get_Empty:System.Span\\u003C!0\\u003E();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Span\\u00601::get_Empty:System.Span\\u003C!0\\u003E();type=[System.Byte];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::get_Empty:System.Span\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Span\\u00601::get_Empty:System.Span\\u003C!0\\u003E();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Span\\u00601::get_Empty:System.Span\\u003C!0\\u003E();type=[System.Byte];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::get_Empty:System.Span\\u003CSystem.Byte\\u003E()\",\"definitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::get_Empty:System.Span\\u003C!0\\u003E()\",\"displaySubjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E::get_Empty()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Span\\u00601::get_Empty:System.Span\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\",\"ilOffset\":9,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":22,\"ilOffset\":15,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":17,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":22,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":22,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SpanMemoryBuffersSubjects\"}";
static const char kMethodJson_6[] = "{\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-6\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_6:System.Void()\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_6()\",\"identity\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"definitionSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_6:System.Void()\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_6:System.Void()\",\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-6\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_6()\"},\"nativeSymbol\":\"SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_6\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":4,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":2,\"resultType\":\"System.Byte[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":7,\"resultType\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E\",\"callee\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E::.ctor:System.Void(System.Byte[])\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E::.ctor:System.Void(System.Byte[])\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":12,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":16,\"resultType\":\"System.Byte\\u0026\",\"callee\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E::get_Item:System.Byte\\u0026(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E::get_Item:System.Byte\\u0026(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E::get_Item:System.Byte\\u0026(System.Int32)\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u00601::get_Item:!0\\u0026(System.Int32)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.ReadOnlySpan\\u00601::get_Item:!0\\u0026(System.Int32);type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.ReadOnlySpan\\u00601::get_Item:!0\\u0026(System.Int32);type=[System.Byte];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u00601::get_Item:!0\\u0026(System.Int32)\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.ReadOnlySpan\\u00601::get_Item:!0\\u0026(System.Int32);type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.ReadOnlySpan\\u00601::get_Item:!0\\u0026(System.Int32);type=[System.Byte];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E::get_Item:System.Byte\\u0026(System.Int32)\",\"definitionSubjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u00601::get_Item:!0\\u0026(System.Int32)\",\"displaySubjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E::get_Item(System.Int32)\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u00601::get_Item:!0\\u0026(System.Int32)\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldind.u1\",\"ilOffset\":21,\"resultType\":\"System.Byte\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":35,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\",\"ilOffset\":30,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":35,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SpanMemoryBuffersSubjects\"}";
static const char kMethodJson_7[] = "{\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-7\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_7:System.Void()\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_7()\",\"identity\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"definitionSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_7:System.Void()\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_7:System.Void()\",\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-7\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_7()\"},\"nativeSymbol\":\"SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_7\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":4,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":4,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":2,\"resultType\":\"System.Byte[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":7,\"resultType\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E\",\"callee\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E::.ctor:System.Void(System.Byte[])\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E::.ctor:System.Void(System.Byte[])\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":12,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":16,\"resultType\":\"System.ReadOnlySpan\\u003CSystem.Byte\\u003E\",\"callee\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E::Slice:System.ReadOnlySpan\\u003CSystem.Byte\\u003E(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E::Slice:System.ReadOnlySpan\\u003CSystem.Byte\\u003E(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E::Slice:System.ReadOnlySpan\\u003CSystem.Byte\\u003E(System.Int32)\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u00601::Slice:System.ReadOnlySpan\\u003C!0\\u003E(System.Int32)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.ReadOnlySpan\\u00601::Slice:System.ReadOnlySpan\\u003C!0\\u003E(System.Int32);type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.ReadOnlySpan\\u00601::Slice:System.ReadOnlySpan\\u003C!0\\u003E(System.Int32);type=[System.Byte];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u00601::Slice:System.ReadOnlySpan\\u003C!0\\u003E(System.Int32)\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.ReadOnlySpan\\u00601::Slice:System.ReadOnlySpan\\u003C!0\\u003E(System.Int32);type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.ReadOnlySpan\\u00601::Slice:System.ReadOnlySpan\\u003C!0\\u003E(System.Int32);type=[System.Byte];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E::Slice:System.ReadOnlySpan\\u003CSystem.Byte\\u003E(System.Int32)\",\"definitionSubjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u00601::Slice:System.ReadOnlySpan\\u003C!0\\u003E(System.Int32)\",\"displaySubjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E::Slice(System.Int32)\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u00601::Slice:System.ReadOnlySpan\\u003C!0\\u003E(System.Int32)\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":2,\"ilOffset\":22,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E::get_Length:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E::get_Length:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E::get_Length:System.Int32()\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u00601::get_Length:System.Int32()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.ReadOnlySpan\\u00601::get_Length:System.Int32();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.ReadOnlySpan\\u00601::get_Length:System.Int32();type=[System.Byte];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u00601::get_Length:System.Int32()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.ReadOnlySpan\\u00601::get_Length:System.Int32();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.ReadOnlySpan\\u00601::get_Length:System.Int32();type=[System.Byte];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E::get_Length:System.Int32()\",\"definitionSubjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u00601::get_Length:System.Int32()\",\"displaySubjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E::get_Length()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u00601::get_Length:System.Int32()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":3,\"ilOffset\":30,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Int32::GetHashCode:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Int32::GetHashCode:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32::GetHashCode:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":3,\"ilOffset\":37,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":38,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":40,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":41,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":43,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":44,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":53,\"ilOffset\":45,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":47,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\",\"ilOffset\":48,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":53,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SpanMemoryBuffersSubjects\"}";
static const char kMethodJson_8[] = "{\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-8\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_8:System.Void()\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_8()\",\"identity\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"definitionSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_8:System.Void()\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_8:System.Void()\",\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-8\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_8()\"},\"nativeSymbol\":\"SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_8\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":1,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":3,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E\",\"ilOffset\":4,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":10,\"resultType\":\"System.Byte[]\",\"callee\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E::ToArray:System.Byte[]()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E::ToArray:System.Byte[]()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E::ToArray:System.Byte[]()\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u00601::ToArray:!0[]()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.ReadOnlySpan\\u00601::ToArray:!0[]();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.ReadOnlySpan\\u00601::ToArray:!0[]();type=[System.Byte];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u00601::ToArray:!0[]()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.ReadOnlySpan\\u00601::ToArray:!0[]();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.ReadOnlySpan\\u00601::ToArray:!0[]();type=[System.Byte];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E::ToArray:System.Byte[]()\",\"definitionSubjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u00601::ToArray:!0[]()\",\"displaySubjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E::ToArray()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u00601::ToArray:!0[]()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldlen\",\"opCode\":46,\"ilOffset\":15,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":20,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":29,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\",\"ilOffset\":24,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":29,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SpanMemoryBuffersSubjects\"}";
static const char kMethodJson_9[] = "{\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-9\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_9:System.Void()\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_9()\",\"identity\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"definitionSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_9:System.Void()\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_9:System.Void()\",\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-9\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_9()\"},\"nativeSymbol\":\"SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_9\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":4,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":2,\"resultType\":\"System.Byte[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":7,\"resultType\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E\",\"callee\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E::.ctor:System.Void(System.Byte[])\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E::.ctor:System.Void(System.Byte[])\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":12,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":13,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":16,\"resultType\":\"System.Memory\\u003CSystem.Byte\\u003E\",\"callee\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E::Slice:System.Memory\\u003CSystem.Byte\\u003E(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E::Slice:System.Memory\\u003CSystem.Byte\\u003E(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E::Slice:System.Memory\\u003CSystem.Byte\\u003E(System.Int32)\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.Memory\\u00601::Slice:System.Memory\\u003C!0\\u003E(System.Int32)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Memory\\u00601::Slice:System.Memory\\u003C!0\\u003E(System.Int32);type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Memory\\u00601::Slice:System.Memory\\u003C!0\\u003E(System.Int32);type=[System.Byte];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Memory\\u00601::Slice:System.Memory\\u003C!0\\u003E(System.Int32)\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Memory\\u00601::Slice:System.Memory\\u003C!0\\u003E(System.Int32);type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Memory\\u00601::Slice:System.Memory\\u003C!0\\u003E(System.Int32);type=[System.Byte];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E::Slice:System.Memory\\u003CSystem.Byte\\u003E(System.Int32)\",\"definitionSubjectId\":\"System.Private.CoreLib/System.Memory\\u00601::Slice:System.Memory\\u003C!0\\u003E(System.Int32)\",\"displaySubjectId\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E::Slice(System.Int32)\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Memory\\u00601::Slice:System.Memory\\u003C!0\\u003E(System.Int32)\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":22,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E::get_Length:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E::get_Length:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E::get_Length:System.Int32()\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.Memory\\u00601::get_Length:System.Int32()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Memory\\u00601::get_Length:System.Int32();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Memory\\u00601::get_Length:System.Int32();type=[System.Byte];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Memory\\u00601::get_Length:System.Int32()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Memory\\u00601::get_Length:System.Int32();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Memory\\u00601::get_Length:System.Int32();type=[System.Byte];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E::get_Length:System.Int32()\",\"definitionSubjectId\":\"System.Private.CoreLib/System.Memory\\u00601::get_Length:System.Int32()\",\"displaySubjectId\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E::get_Length()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Memory\\u00601::get_Length:System.Int32()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":3,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":30,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":35,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":36,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":45,\"ilOffset\":37,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":39,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\",\"ilOffset\":40,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":45,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SpanMemoryBuffersSubjects\"}";
static const char kMethodJson_10[] = "{\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-10\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_10:System.Void()\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_10()\",\"identity\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"definitionSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_10:System.Void()\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_10:System.Void()\",\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-10\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_10()\"},\"nativeSymbol\":\"SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_10\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":25,\"handlerOffset\":26,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NotSupportedException\"}],\"instructions\":[{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":4,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E\",\"ilOffset\":5,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Span\\u003CSystem.Byte\\u003E\",\"callee\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E::get_Span:System.Span\\u003CSystem.Byte\\u003E()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E::get_Span:System.Span\\u003CSystem.Byte\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E::get_Span:System.Span\\u003CSystem.Byte\\u003E()\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.Memory\\u00601::get_Span:System.Span\\u003C!0\\u003E()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Memory\\u00601::get_Span:System.Span\\u003C!0\\u003E();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Memory\\u00601::get_Span:System.Span\\u003C!0\\u003E();type=[System.Byte];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Memory\\u00601::get_Span:System.Span\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Memory\\u00601::get_Span:System.Span\\u003C!0\\u003E();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Memory\\u00601::get_Span:System.Span\\u003C!0\\u003E();type=[System.Byte];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E::get_Span:System.Span\\u003CSystem.Byte\\u003E()\",\"definitionSubjectId\":\"System.Private.CoreLib/System.Memory\\u00601::get_Span:System.Span\\u003C!0\\u003E()\",\"displaySubjectId\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E::get_Span()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Memory\\u00601::get_Span:System.Span\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":16,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\",\"ilOffset\":18,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":31,\"ilOffset\":24,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":31,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":31,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SpanMemoryBuffersSubjects\"}";
static const char kMethodJson_11[] = "{\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-11\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_11:System.Void()\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_11()\",\"identity\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"definitionSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_11:System.Void()\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_11:System.Void()\",\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-11\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_11()\"},\"nativeSymbol\":\"SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_11\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":1,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":3,\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E\",\"ilOffset\":4,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":10,\"resultType\":\"System.Byte[]\",\"callee\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E::ToArray:System.Byte[]()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E::ToArray:System.Byte[]()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E::ToArray:System.Byte[]()\",\"openDefinitionSubjectId\":\"System.Private.CoreLib/System.Memory\\u00601::ToArray:!0[]()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Memory\\u00601::ToArray:!0[]();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Memory\\u00601::ToArray:!0[]();type=[System.Byte];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Memory\\u00601::ToArray:!0[]()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Private.CoreLib/System.Memory\\u00601::ToArray:!0[]();type=[System.Byte];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Private.CoreLib/System.Memory\\u00601::ToArray:!0[]();type=[System.Byte];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E::ToArray:System.Byte[]()\",\"definitionSubjectId\":\"System.Private.CoreLib/System.Memory\\u00601::ToArray:!0[]()\",\"displaySubjectId\":\"System.Private.CoreLib/System.Memory\\u003CSystem.Byte\\u003E::ToArray()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Private.CoreLib/System.Memory\\u00601::ToArray:!0[]()\",\"typeArguments\":[\"System.Byte\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldlen\",\"opCode\":46,\"ilOffset\":15,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":20,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":29,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\",\"ilOffset\":24,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":29,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SpanMemoryBuffersSubjects\"}";
static const char kMethodJson_12[] = "{\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-12\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_12:System.Void()\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_12()\",\"identity\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"definitionSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_12:System.Void()\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_12:System.Void()\",\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-12\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_12()\"},\"nativeSymbol\":\"SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_12\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":25,\"handlerOffset\":26,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E\",\"ilOffset\":4,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.ReadOnlySpan\\u003CSystem.Byte\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":10,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Byte\\u0026\",\"callee\":\"System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference\\u003CSystem.Byte\\u003E:System.Byte\\u0026(System.ReadOnlySpan\\u003CSystem.Byte\\u003E)\",\"reference\":{\"assemblyName\":\"System.Memory\",\"subjectKind\":\"method\",\"subjectId\":\"System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference\\u003CSystem.Byte\\u003E:System.Byte\\u0026(System.ReadOnlySpan\\u003CSystem.Byte\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Memory\",\"subjectId\":\"System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference\\u003CSystem.Byte\\u003E:System.Byte\\u0026(System.ReadOnlySpan\\u003CSystem.Byte\\u003E)\",\"openDefinitionSubjectId\":\"System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference:!!0\\u0026(System.ReadOnlySpan\\u003C!!0\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference:!!0\\u0026(System.ReadOnlySpan\\u003C!!0\\u003E);type=[];method=[System.Byte]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference:!!0\\u0026(System.ReadOnlySpan\\u003C!!0\\u003E);type=[];method=[System.Byte]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference:!!0\\u0026(System.ReadOnlySpan\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Byte\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference:!!0\\u0026(System.ReadOnlySpan\\u003C!!0\\u003E);type=[];method=[System.Byte]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference:!!0\\u0026(System.ReadOnlySpan\\u003C!!0\\u003E);type=[];method=[System.Byte]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference\\u003CSystem.Byte\\u003E:System.Byte\\u0026(System.ReadOnlySpan\\u003CSystem.Byte\\u003E)\",\"definitionSubjectId\":\"System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference:!!0\\u0026(System.ReadOnlySpan\\u003C!!0\\u003E)\",\"displaySubjectId\":\"System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference\\u003CSystem.Byte\\u003E(System.ReadOnlySpan\\u003CSystem.Byte\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference:!!0\\u0026(System.ReadOnlySpan\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Byte\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":16,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\",\"ilOffset\":18,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":31,\"ilOffset\":24,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":31,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":31,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SpanMemoryBuffersSubjects\"}";
static const char kMethodJson_13[] = "{\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-13\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_13:System.Void()\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_13()\",\"identity\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"definitionSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_13:System.Void()\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::Subject_13:System.Void()\",\"methodId\":\"span-memory-buffers-subjects.span-memory-buffers-subjects.subject-13\",\"signature\":\"System.Void SpanMemoryBuffersSubjects::Subject_13()\"},\"nativeSymbol\":\"SpanMemoryBuffersSubjects_SpanMemoryBuffersSubjects_Subject_13\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":25,\"handlerOffset\":26,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.NullReferenceException\"}],\"instructions\":[{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\",\"ilOffset\":4,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Span\\u003CSystem.Byte\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":10,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Byte\\u0026\",\"callee\":\"System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference\\u003CSystem.Byte\\u003E:System.Byte\\u0026(System.Span\\u003CSystem.Byte\\u003E)\",\"reference\":{\"assemblyName\":\"System.Memory\",\"subjectKind\":\"method\",\"subjectId\":\"System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference\\u003CSystem.Byte\\u003E:System.Byte\\u0026(System.Span\\u003CSystem.Byte\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Memory\",\"subjectId\":\"System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference\\u003CSystem.Byte\\u003E:System.Byte\\u0026(System.Span\\u003CSystem.Byte\\u003E)\",\"openDefinitionSubjectId\":\"System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference:!!0\\u0026(System.Span\\u003C!!0\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference:!!0\\u0026(System.Span\\u003C!!0\\u003E);type=[];method=[System.Byte]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference:!!0\\u0026(System.Span\\u003C!!0\\u003E);type=[];method=[System.Byte]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference:!!0\\u0026(System.Span\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Byte\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference:!!0\\u0026(System.Span\\u003C!!0\\u003E);type=[];method=[System.Byte]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference:!!0\\u0026(System.Span\\u003C!!0\\u003E);type=[];method=[System.Byte]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference\\u003CSystem.Byte\\u003E:System.Byte\\u0026(System.Span\\u003CSystem.Byte\\u003E)\",\"definitionSubjectId\":\"System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference:!!0\\u0026(System.Span\\u003C!!0\\u003E)\",\"displaySubjectId\":\"System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference\\u003CSystem.Byte\\u003E(System.Span\\u003CSystem.Byte\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Memory/System.Runtime.InteropServices.MemoryMarshal::GetReference:!!0\\u0026(System.Span\\u003C!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.Byte\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":16,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\",\"ilOffset\":18,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"SpanMemoryBuffersSubjects\",\"subjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"SpanMemoryBuffersSubjects/SpanMemoryBuffersSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":31,\"ilOffset\":24,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":31,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":31,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"SpanMemoryBuffersSubjects\"}";

extern "C" const JitMethodEntry kChaosJitMethodEntries[14] =
{
    { kMethodJson_0, 6649u, 0x00000003u, 0u },
    { kMethodJson_1, 10126u, 0x00000004u, 0u },
    { kMethodJson_2, 10398u, 0x00000005u, 0u },
    { kMethodJson_3, 8303u, 0x00000006u, 0u },
    { kMethodJson_4, 5327u, 0x00000007u, 0u },
    { kMethodJson_5, 5007u, 0x00000008u, 0u },
    { kMethodJson_6, 6785u, 0x00000009u, 0u },
    { kMethodJson_7, 10470u, 0x0000000Au, 0u },
    { kMethodJson_8, 5678u, 0x0000000Bu, 0u },
    { kMethodJson_9, 9393u, 0x0000000Cu, 0u },
    { kMethodJson_10, 5800u, 0x0000000Du, 0u },
    { kMethodJson_11, 5590u, 0x0000000Eu, 0u },
    { kMethodJson_12, 6361u, 0x0000000Fu, 0u },
    { kMethodJson_13, 6233u, 0x00000010u, 0u }
};

extern "C" const uint32_t kChaosJitMethodEntryCount = 14u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitMethods(kChaosJitMethodEntries, kChaosJitMethodEntryCount);
}