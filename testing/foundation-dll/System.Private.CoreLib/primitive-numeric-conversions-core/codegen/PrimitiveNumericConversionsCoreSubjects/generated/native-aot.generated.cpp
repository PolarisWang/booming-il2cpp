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

namespace chaos::il2cpp::codegen::PrimitiveNumericConversionsCoreSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects = {{nullptr, nullptr, 3329548187654970984ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(3329548187654970984ULL);
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

struct chaos_type_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects
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

CHAOS_IL2CPP_INT32 chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = 0;

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_0(void);
extern "C" void PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_1(void);
extern "C" void PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_2(void);
extern "C" void PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_3(void);
extern "C" void PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_4(void);
extern "C" void PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_5(void);
extern "C" void PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_6(void);
extern "C" void PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_7(void);
extern "C" void PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_8(void);
extern "C" void PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_9(void);
extern "C" CHAOS_IL2CPP_INT32 PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestByteToInt(void);
extern "C" CHAOS_IL2CPP_INT32 PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToFloat(void);
extern "C" CHAOS_IL2CPP_INT32 PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToInt(void);
extern "C" CHAOS_IL2CPP_INT32 PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestFloatToDouble(void);
extern "C" CHAOS_IL2CPP_INT32 PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToByte(void);
extern "C" CHAOS_IL2CPP_INT32 PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToDouble(void);
extern "C" CHAOS_IL2CPP_INT32 PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToLong(void);
extern "C" CHAOS_IL2CPP_INT32 PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToShort(void);
extern "C" CHAOS_IL2CPP_INT32 PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestLongToInt(void);
extern "C" CHAOS_IL2CPP_INT32 PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestUintToLong(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[21] = {
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
	::ChaosAbiMethodEntryV0 entries[20];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		20u,
		0u,
		2517616541u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_0
		{ 0u, 0u },  // PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_1
		{ 0u, 0u },  // PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_2
		{ 0u, 0u },  // PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_3
		{ 0u, 0u },  // PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_4
		{ 0u, 0u },  // PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_5
		{ 0u, 0u },  // PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_6
		{ 0u, 0u },  // PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_7
		{ 0u, 0u },  // PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_8
		{ 0u, 0u },  // PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_9
		{ 1u, 0u },  // PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestByteToInt
		{ 1u, 0u },  // PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToFloat
		{ 1u, 0u },  // PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToInt
		{ 1u, 0u },  // PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestFloatToDouble
		{ 1u, 0u },  // PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToByte
		{ 1u, 0u },  // PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToDouble
		{ 1u, 0u },  // PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToLong
		{ 1u, 0u },  // PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToShort
		{ 1u, 0u },  // PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestLongToInt
		{ 1u, 0u },  // PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestUintToLong
	},
	{
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
		"PrimitiveNumericConversionsCoreSubjects",
		"PrimitiveNumericConversionsCoreNativeEntry",
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
		&chaos_mt_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects.hot,
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
		/* .name_utf8         = */ "PrimitiveNumericConversionsCoreSubjects",
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
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 — deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,

		/* .abi_manifest      = */ s_abi_manifest,
	};
	static const CHAOS_IL2CPP_UINT32 s_native_aot_module_id =
		::chaos::il2cpp::runtime_core::RegisterModule("PrimitiveNumericConversionsCoreSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[20] = {
	{ "TestByteToInt", 0x0000000Eu, 0u },  // PrimitiveNumericConversionsCoreNativeEntry
	{ "TestDoubleToFloat", 0x00000013u, 0u },  // PrimitiveNumericConversionsCoreNativeEntry
	{ "TestDoubleToInt", 0x00000015u, 0u },  // PrimitiveNumericConversionsCoreNativeEntry
	{ "TestFloatToDouble", 0x00000012u, 0u },  // PrimitiveNumericConversionsCoreNativeEntry
	{ "TestIntToByte", 0x00000011u, 0u },  // PrimitiveNumericConversionsCoreNativeEntry
	{ "TestIntToDouble", 0x00000014u, 0u },  // PrimitiveNumericConversionsCoreNativeEntry
	{ "TestIntToLong", 0x0000000Fu, 0u },  // PrimitiveNumericConversionsCoreNativeEntry
	{ "TestIntToShort", 0x00000010u, 0u },  // PrimitiveNumericConversionsCoreNativeEntry
	{ "TestLongToInt", 0x00000017u, 0u },  // PrimitiveNumericConversionsCoreNativeEntry
	{ "TestUintToLong", 0x00000016u, 0u },  // PrimitiveNumericConversionsCoreNativeEntry
	{ "Subject_0", 0x00000004u, 0u },  // PrimitiveNumericConversionsCoreSubjects
	{ "Subject_1", 0x00000005u, 0u },  // PrimitiveNumericConversionsCoreSubjects
	{ "Subject_2", 0x00000006u, 0u },  // PrimitiveNumericConversionsCoreSubjects
	{ "Subject_3", 0x00000007u, 0u },  // PrimitiveNumericConversionsCoreSubjects
	{ "Subject_4", 0x00000008u, 0u },  // PrimitiveNumericConversionsCoreSubjects
	{ "Subject_5", 0x00000009u, 0u },  // PrimitiveNumericConversionsCoreSubjects
	{ "Subject_6", 0x0000000Au, 0u },  // PrimitiveNumericConversionsCoreSubjects
	{ "Subject_7", 0x0000000Bu, 0u },  // PrimitiveNumericConversionsCoreSubjects
	{ "Subject_8", 0x0000000Cu, 0u },  // PrimitiveNumericConversionsCoreSubjects
	{ "Subject_9", 0x0000000Du, 0u },  // PrimitiveNumericConversionsCoreSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[2] = {
	{ "PrimitiveNumericConversionsCoreNativeEntry", "", 0u, 10u },
	{ "PrimitiveNumericConversionsCoreSubjects", "", 10u, 10u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[20] = {
	{ 0x00000004u, 0u },
	{ 0x00000005u, 1u },
	{ 0x00000006u, 2u },
	{ 0x00000007u, 3u },
	{ 0x00000008u, 4u },
	{ 0x00000009u, 5u },
	{ 0x0000000Au, 6u },
	{ 0x0000000Bu, 7u },
	{ 0x0000000Cu, 8u },
	{ 0x0000000Du, 9u },
	{ 0x0000000Eu, 10u },
	{ 0x0000000Fu, 16u },
	{ 0x00000010u, 17u },
	{ 0x00000011u, 14u },
	{ 0x00000012u, 13u },
	{ 0x00000013u, 11u },
	{ 0x00000014u, 15u },
	{ 0x00000015u, 12u },
	{ 0x00000016u, 19u },
	{ 0x00000017u, 18u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[20] = {
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PrimitiveNumericConversionsCoreSubjects::Subject_0
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PrimitiveNumericConversionsCoreSubjects::Subject_1
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PrimitiveNumericConversionsCoreSubjects::Subject_2
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PrimitiveNumericConversionsCoreSubjects::Subject_3
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PrimitiveNumericConversionsCoreSubjects::Subject_4
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PrimitiveNumericConversionsCoreSubjects::Subject_5
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PrimitiveNumericConversionsCoreSubjects::Subject_6
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PrimitiveNumericConversionsCoreSubjects::Subject_7
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PrimitiveNumericConversionsCoreSubjects::Subject_8
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PrimitiveNumericConversionsCoreSubjects::Subject_9
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestByteToInt), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PrimitiveNumericConversionsCoreNativeEntry::TestByteToInt
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToFloat), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToFloat
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToInt), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToInt
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestFloatToDouble), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PrimitiveNumericConversionsCoreNativeEntry::TestFloatToDouble
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToByte), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PrimitiveNumericConversionsCoreNativeEntry::TestIntToByte
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToDouble), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PrimitiveNumericConversionsCoreNativeEntry::TestIntToDouble
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToLong), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PrimitiveNumericConversionsCoreNativeEntry::TestIntToLong
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToShort), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PrimitiveNumericConversionsCoreNativeEntry::TestIntToShort
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestLongToInt), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PrimitiveNumericConversionsCoreNativeEntry::TestLongToInt
	{ reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestUintToLong), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // PrimitiveNumericConversionsCoreNativeEntry::TestUintToLong
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"PrimitiveNumericConversionsCoreSubjects",
	s_hotpatch_types,
	2u,
	s_hotpatch_methods,
	20u,
	s_hotpatch_slots,
	20u,
	s_hotpatch_entries,
	20u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[1] = {
	"PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode",
};

extern "C" void* kChaosExternalRuntimeFnTable[1] = {
	nullptr,
};

extern "C" int32_t kChaosExternalRuntimeCount = 1;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[20])() = {
	reinterpret_cast<void(*)()>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_6),
	reinterpret_cast<void(*)()>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_7),
	reinterpret_cast<void(*)()>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_8),
	reinterpret_cast<void(*)()>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_9),
	reinterpret_cast<void(*)()>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestByteToInt),
	reinterpret_cast<void(*)()>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToFloat),
	reinterpret_cast<void(*)()>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToInt),
	reinterpret_cast<void(*)()>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestFloatToDouble),
	reinterpret_cast<void(*)()>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToByte),
	reinterpret_cast<void(*)()>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToDouble),
	reinterpret_cast<void(*)()>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToLong),
	reinterpret_cast<void(*)()>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToShort),
	reinterpret_cast<void(*)()>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestLongToInt),
	reinterpret_cast<void(*)()>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestUintToLong),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[20])() = {
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

	/* ── Entry 0: PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry0;
	/* ── Entry 1: PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry1;
	/* ── Entry 2: PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry2;
	/* ── Entry 3: PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry3;
	/* ── Entry 4: PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry4;
	/* ── Entry 5: PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry5;
	/* ── Entry 6: PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_6 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry6;
	/* ── Entry 7: PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_7 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry7;
	/* ── Entry 8: PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_8 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry8;
	/* ── Entry 9: PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_9 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry9;
	/* ── Entry 10: PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestByteToInt ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry10;
	/* ── Entry 11: PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToFloat ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[10];
	} entry11;
	/* ── Entry 12: PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToInt ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry12;
	/* ── Entry 13: PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestFloatToDouble ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry13;
	/* ── Entry 14: PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToByte ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry14;
	/* ── Entry 15: PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToDouble ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry15;
	/* ── Entry 16: PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToLong ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry16;
	/* ── Entry 17: PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToShort ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry17;
	/* ── Entry 18: PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestLongToInt ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry18;
	/* ── Entry 19: PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestUintToLong ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry19;
} kChaosGcSlotMapsSection = {
	/* entry0 = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_0 */
	.entry0 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry1 = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_1 */
	.entry1 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry2 = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_2 */
	.entry2 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_2),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry3 = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_3 */
	.entry3 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_3),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry4 = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_4 */
	.entry4 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_4),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry5 = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_5 */
	.entry5 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_5),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry6 = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_6 */
	.entry6 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_6),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry7 = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_7 */
	.entry7 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_7),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry8 = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_8 */
	.entry8 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_8),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry9 = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_9 */
	.entry9 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_9),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry10 = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestByteToInt */
	.entry10 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestByteToInt),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry11 = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToFloat */
	.entry11 = {
		/* entry_total_size = 60 */ 60u,
		/* code_address */ reinterpret_cast<const void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToFloat),
		/* frame_size = 80 */ 80u,
		/* num_gc_slots = 10 */ 10u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u }
	},
	/* entry12 = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToInt */
	.entry12 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToInt),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry13 = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestFloatToDouble */
	.entry13 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestFloatToDouble),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry14 = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToByte */
	.entry14 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToByte),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry15 = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToDouble */
	.entry15 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToDouble),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry16 = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToLong */
	.entry16 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToLong),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry17 = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToShort */
	.entry17 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToShort),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry18 = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestLongToInt */
	.entry18 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestLongToInt),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry19 = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestUintToLong */
	.entry19 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestUintToLong),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 796u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[20] = {
	reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_0),
	reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_1),
	reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_2),
	reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_3),
	reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_4),
	reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_5),
	reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_6),
	reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_7),
	reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_8),
	reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_9),
	reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestByteToInt),
	reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToFloat),
	reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToInt),
	reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestFloatToDouble),
	reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToByte),
	reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToDouble),
	reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToLong),
	reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToShort),
	reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestLongToInt),
	reinterpret_cast<void*>(&PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestUintToLong),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 20u,
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
	.image_name_utf8    = "PrimitiveNumericConversionsCoreSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects[1] = {
	{ 0u, "PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects[10] = {
	{ 0u, "PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_6:System.Void()", "Subject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_7:System.Void()", "Subject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_8:System.Void()", "Subject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_9:System.Void()", "Subject_9", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry[10] = {
	{ 0u, "PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestByteToInt:System.Int32()", "TestByteToInt", "System.Void", 0, nullptr, 0u },
	{ 0u, "PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToFloat:System.Int32()", "TestDoubleToFloat", "System.Void", 0, nullptr, 0u },
	{ 0u, "PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToInt:System.Int32()", "TestDoubleToInt", "System.Void", 0, nullptr, 0u },
	{ 0u, "PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestFloatToDouble:System.Int32()", "TestFloatToDouble", "System.Void", 0, nullptr, 0u },
	{ 0u, "PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToByte:System.Int32()", "TestIntToByte", "System.Void", 0, nullptr, 0u },
	{ 0u, "PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToDouble:System.Int32()", "TestIntToDouble", "System.Void", 0, nullptr, 0u },
	{ 0u, "PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToLong:System.Int32()", "TestIntToLong", "System.Void", 0, nullptr, 0u },
	{ 0u, "PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToShort:System.Int32()", "TestIntToShort", "System.Void", 0, nullptr, 0u },
	{ 0u, "PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestLongToInt:System.Int32()", "TestLongToInt", "System.Void", 0, nullptr, 0u },
	{ 0u, "PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestUintToLong:System.Int32()", "TestUintToLong", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[2] = {
	{ 0u, "PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects", "PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects", "", "PrimitiveNumericConversionsCoreSubjects", "PrimitiveNumericConversionsCoreSubjects", nullptr, kReflFields_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects, 1u, nullptr, 0u,
	kReflMethods_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects, 10u },
	{ 0u, "PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry", "PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry", "", "PrimitiveNumericConversionsCoreNativeEntry", "PrimitiveNumericConversionsCoreNativeEntry", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry, 10u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[2] = {
	&kReflTypes[0],
	&kReflTypes[1],
};

static const ReflectionQueryImageDescriptor kReflImage = { "PrimitiveNumericConversionsCoreSubjects", kReflTypePtrs, 2u };

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
// Managed method: PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_0()
extern "C" void PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_0(void)
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
					_d_hpresult = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestByteToInt();
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
					_d_hpresult = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestByteToInt();
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_1()
extern "C" void PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_1(void)
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
					_d_hpresult = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToLong();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
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
					_d_hpresult = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToLong();
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_2()
extern "C" void PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_2(void)
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
					_d_hpresult = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToShort();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
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
					_d_hpresult = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToShort();
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_3()
extern "C" void PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_3(void)
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
					_d_hpresult = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToByte();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
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
					_d_hpresult = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToByte();
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_4()
extern "C" void PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_4(void)
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
					_d_hpresult = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestFloatToDouble();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
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
					_d_hpresult = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestFloatToDouble();
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_5()
extern "C" void PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_5(void)
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
					_d_hpresult = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToFloat();
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
					_d_hpresult = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToFloat();
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_6()
extern "C" void PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_6(void)
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
					_d_hpresult = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToDouble();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
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
					_d_hpresult = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToDouble();
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_7()
extern "C" void PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_7(void)
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
					_d_hpresult = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToInt();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
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
					_d_hpresult = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToInt();
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_8()
extern "C" void PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_8(void)
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
					_d_hpresult = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestUintToLong();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
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
					_d_hpresult = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestUintToLong();
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreSubjects::Subject_9()
extern "C" void PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects_Subject_9(void)
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
					_d_hpresult = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestLongToInt();
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
			}
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
					_d_hpresult = PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestLongToInt();
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
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
				chaos_static_PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestByteToInt()
extern "C" CHAOS_IL2CPP_INT32 PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestByteToInt(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(200);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(200);
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
		chaos_locals[2] = _s0;
		_s0 = chaos_locals[2];
		return static_cast<CHAOS_IL2CPP_INT32>(_s0);
	}
}

// Managed method: PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToFloat()
extern "C" CHAOS_IL2CPP_INT32 PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToFloat(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_locals{};
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


	_s0 = ChaosStoreFloat64(2.71828);
	_s0 = ChaosStoreFloat32(static_cast<float>(_s0));
	_s0 = ChaosStoreFloat64(static_cast<double>(_s0));
	_s0 = ChaosStoreFloat32(static_cast<float>(_s0));
	_s1 = ChaosStoreFloat32(2.71828f);
	{
		const auto chaos_right = chaos_load_float32(_s1);
		const auto chaos_left = ChaosLoadFloat64(_s0);
		if (chaos_left != chaos_right)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		chaos_locals[4] = _s2;
		_s2 = chaos_locals[4];
		return static_cast<CHAOS_IL2CPP_INT32>(_s2);
	}
}

// Managed method: PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToInt()
extern "C" CHAOS_IL2CPP_INT32 PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToInt(void)
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


	_s0 = ChaosStoreFloat64(3.99);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s0)));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		if (chaos_left != chaos_right)
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

// Managed method: PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestFloatToDouble()
extern "C" CHAOS_IL2CPP_INT32 PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestFloatToDouble(void)
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


	_s0 = ChaosStoreFloat32(3.14f);
	_s0 = ChaosStoreFloat64(static_cast<double>(_s0));
	_s0 = ChaosStoreFloat32(static_cast<float>(_s0));
	_s1 = ChaosStoreFloat32(3.14f);
	{
		const auto chaos_right = chaos_load_float32(_s1);
		const auto chaos_left = chaos_load_float32(_s0);
		if (chaos_left != chaos_right)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		chaos_locals[3] = _s1;
		_s1 = chaos_locals[3];
		return static_cast<CHAOS_IL2CPP_INT32>(_s1);
	}
}

// Managed method: PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToByte()
extern "C" CHAOS_IL2CPP_INT32 PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToByte(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(298);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT8>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		if (chaos_left != chaos_right)
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

// Managed method: PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToDouble()
extern "C" CHAOS_IL2CPP_INT32 PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToDouble(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_s0 = ChaosStoreFloat64(static_cast<double>(_s0));
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		if (chaos_left != chaos_right)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		chaos_locals[3] = _s1;
		_s1 = chaos_locals[3];
		return static_cast<CHAOS_IL2CPP_INT32>(_s1);
	}
}

// Managed method: PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToLong()
extern "C" CHAOS_IL2CPP_INT32 PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToLong(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
	_s0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
	_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		if (chaos_left != chaos_right)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		chaos_locals[2] = _s1;
		_s1 = chaos_locals[2];
		return static_cast<CHAOS_IL2CPP_INT32>(_s1);
	}
}

// Managed method: PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestIntToShort()
extern "C" CHAOS_IL2CPP_INT32 PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestIntToShort(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(65535);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT16>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		if (chaos_left != chaos_right)
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

// Managed method: PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestLongToInt()
extern "C" CHAOS_IL2CPP_INT32 PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestLongToInt(void)
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


	_s0 = ChaosStoreInt64(4294967297LL);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		if (chaos_left != chaos_right)
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

// Managed method: PrimitiveNumericConversionsCoreSubjects/PrimitiveNumericConversionsCoreNativeEntry::TestUintToLong()
extern "C" CHAOS_IL2CPP_INT32 PrimitiveNumericConversionsCoreSubjects_PrimitiveNumericConversionsCoreNativeEntry_TestUintToLong(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(CHAOS_IL2CPP_INT32));
	_s0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(CHAOS_IL2CPP_INT32));
	_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		if (chaos_left != chaos_right)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		chaos_locals[2] = _s1;
		_s1 = chaos_locals[2];
		return static_cast<CHAOS_IL2CPP_INT32>(_s1);
	}
}



}  // namespace chaos::il2cpp::codegen::PrimitiveNumericConversionsCoreSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 20;

extern "C" void ChaosJitRegisterAll() {}